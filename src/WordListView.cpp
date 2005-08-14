//---------------------------------------------------------------------------
// WordListView.cpp
//
// A customized list view.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "WordListView.h"
#include "WordListViewItem.h"
#include "DefinitionDialog.h"
#include "MainSettings.h"
#include "MainWindow.h"
#include "QuizSpec.h"
#include "WordEngine.h"
#include "WordPopupMenu.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
#include "Auxil.h"
#include <qfile.h>
#include <qfiledialog.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qmessagebox.h>
#include <set>

using namespace std;

const QChar WordListView::PARENT_HOOK_CHAR = '-';
const QString FRONT_HOOK_HEADER = "Hooks";
const QString WORD_HEADER = "Word";
const QString BACK_HOOK_HEADER = "Hooks";
const QString DEFINITION_HEADER = "Definition";
const int ITEM_MARGIN = 5;
const int DEFAULT_COLUMN_WIDTH = 100;
const int NUM_COLUMNS = 4;

//---------------------------------------------------------------------------
//  WordListView
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
WordListView::WordListView (WordEngine* e, QWidget* parent, const char* name,
                            WFlags f)
    : QListView (parent, name, f), wordEngine (e)
{
    addColumn (FRONT_HOOK_HEADER);
    hidden[FRONT_HOOK_COLUMN] = false;
    setColumnWidthMode (FRONT_HOOK_COLUMN, QListView::Manual);
    setColumnAlignment (FRONT_HOOK_COLUMN, Qt::AlignRight);

    addColumn (WORD_HEADER);
    hidden[WORD_COLUMN] = false;
    setColumnWidthMode (WORD_COLUMN, QListView::Manual);

    addColumn (BACK_HOOK_HEADER);
    hidden[BACK_HOOK_COLUMN] = false;
    setColumnWidthMode (BACK_HOOK_COLUMN, QListView::Manual);

    addColumn (DEFINITION_HEADER);
    hidden[DEFINITION_COLUMN] = false;
    setColumnWidthMode (DEFINITION_COLUMN, QListView::Manual);

    setAllColumnsShowFocus (true);
    setItemMargin (ITEM_MARGIN);
    setSorting (WORD_COLUMN);

    setResizeMode (QListView::NoColumn);
    showHooks (MainSettings::getWordListShowHooks());
    showDefinitions (MainSettings::getWordListShowDefinitions());
    resetColumnWidths();

    // Only connect certain slots if word engine is non-null
    if (wordEngine) {
        // XXX: This signal should be connected regardless... only certain
        // popup menu items should be available in the absence of a word
        // engine.
        connect (this, SIGNAL (contextMenuRequested (QListViewItem*, const
                                                     QPoint&, int)),
                 SLOT (doPopupMenu (QListViewItem*, const QPoint&, int)));

        connect (this, SIGNAL (returnPressed (QListViewItem*)),
                 SLOT (doReturnPressed (QListViewItem*)));
    }
}

//---------------------------------------------------------------------------
//  ~WordListView
//
//! Destructor.
//---------------------------------------------------------------------------
WordListView::~WordListView()
{
}

//---------------------------------------------------------------------------
//  setFont
//
//! Set the font of all listview items.
//---------------------------------------------------------------------------
void
WordListView::setFont (const QFont& font)
{
    QListView::setFont (font);
    for (QListViewItem* item = firstChild(); item; item = item->nextSibling())
        static_cast<WordListViewItem*>(item)->setFont (font);
}

//---------------------------------------------------------------------------
//  addWord
//
//! Add a word and populate all available columns.
//
//! @param word the word to add to the list
//! @return the new word list view item
//---------------------------------------------------------------------------
WordListViewItem*
WordListView::addWord (const QString& word)
{
    QString front, back, definition;
    QString wordCopy = word;
    QString wordUpper = word.upper();

    if (!hidden[FRONT_HOOK_COLUMN]) {
        front = getFrontHookLetters (wordUpper);
        resizeColumnText (FRONT_HOOK_COLUMN, front);
    }

    if (!hidden[BACK_HOOK_COLUMN]) {
        back = getBackHookLetters (wordUpper);
        resizeColumnText (BACK_HOOK_COLUMN, back);
    }

    if (MainSettings::getWordListShowHookParents()) {
        wordCopy =
            (isFrontHook (wordUpper) ? PARENT_HOOK_CHAR : QChar (' ')) +
            word +
            (isBackHook (wordUpper) ? PARENT_HOOK_CHAR : QChar (' '));
    }
    resizeColumnText (WORD_COLUMN, wordCopy);

    if (!hidden[DEFINITION_COLUMN]) {
        definition = wordEngine->getDefinition (wordUpper);
        resizeColumnText (DEFINITION_COLUMN, definition);
    }

    return new WordListViewItem (this, front, wordCopy, back, definition);
}

//---------------------------------------------------------------------------
//  resetColumnWidths
//
//! Reset the column widths.
//
//! @param item the selected listview item
//---------------------------------------------------------------------------
void
WordListView::resetColumnWidths()
{
    QFont font;
    QString fontStr = MainSettings::getMainFont();
    bool fontOk = font.fromString (fontStr);
    QFontMetrics fontMetrics (font);

    for (int i = 0; i < NUM_COLUMNS; ++i) {
        QString header;
        switch (i) {
            case FRONT_HOOK_COLUMN: header = FRONT_HOOK_HEADER; break;
            case WORD_COLUMN: header = WORD_HEADER; break;
            case BACK_HOOK_COLUMN: header = BACK_HOOK_HEADER; break;
            case DEFINITION_COLUMN: header = DEFINITION_HEADER; break;
            default: break;
        }

        widths[i] = (fontOk ?  fontMetrics.width (header) + (2 * ITEM_MARGIN)
                     : DEFAULT_COLUMN_WIDTH);
    }
    resizeColumns();
}

//---------------------------------------------------------------------------
//  resizeColumns
//
//! Adjust the column widths so they are at least as wide as their respective
//! minimum widths.
//---------------------------------------------------------------------------
void
WordListView::resizeColumns()
{
    for (int i = 0; i < NUM_COLUMNS; ++i) {
        setColumnWidth (i, hidden[i] ? 0 : widths[i]);
    }
}

//---------------------------------------------------------------------------
//  resizeColumnText
//
//! Resize a column if necessary to be able to display some text.
//
//! @param column the column
//! @param text the text that needs to be displayed
//---------------------------------------------------------------------------
void
WordListView::resizeColumnText (int column, const QString& text)
{
    int width = getTextColumnWidth (text);
    if (width > widths[column]) {
        widths[column] = width;
        setColumnWidth (column, width);
    }
}

//---------------------------------------------------------------------------
//  getTextColumnWidth
//
//! Determine the width of some text as it would be displayed in a word list
//! column.  Include the margins.
//
//! @param text the text whose width to determine
//! @return the minimum width of a column that could display the text
//---------------------------------------------------------------------------
int
WordListView::getTextColumnWidth (const QString& text) const
{
    QFont font;
    QString fontStr = MainSettings::getWordListFont();
    bool fontOk = font.fromString (fontStr);
    QFontMetrics fontMetrics (font);
    int width = 2 * ITEM_MARGIN;
    if (fontOk)
        width += fontMetrics.width (text);
    return width;
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the word list.  Also reset the column widths.
//---------------------------------------------------------------------------
void
WordListView::clear()
{
    QListView::clear();
    showHooks (MainSettings::getWordListShowHooks());
    showDefinitions (MainSettings::getWordListShowDefinitions());
    resetColumnWidths();
}

//---------------------------------------------------------------------------
//  doReturnPressed
//
//! Called when return is pressed on an item in the response list.  Displays
//! the selected word's definition.
//
//! @param item the selected listview item
//---------------------------------------------------------------------------
void
WordListView::doReturnPressed (QListViewItem* item)
{
    if (!item)
        return;
    displayDefinition
        (static_cast<WordListViewItem*>(item)->getWord().upper());
}

//---------------------------------------------------------------------------
//  doPopupMenu
//
//! Called when a right-click menu is requested.  Creates a popup menu and
//! allows the user to choose an action for the selected item.
//! @param item the selected listview item, or NULL if no listview item is
//!        selected
//! @param point the point at which the menu was requested
//---------------------------------------------------------------------------
void
WordListView::doPopupMenu (QListViewItem* item, const QPoint& point, int)
{
    WordPopupMenu* menu = new WordPopupMenu (item ? true : false, this,
                                             "menu");
    int choice = menu->exec (point);
    delete menu;

    QString word;
    if (item)
        word = static_cast<WordListViewItem*>(item)->getWord().upper();

    WordVariationType variation = VariationNone;

    switch (choice) {
        case WordPopupMenu::ViewDefinition:
        displayDefinition (word);
        return;

        case WordPopupMenu::ViewAnagrams:
        variation = VariationAnagrams;
        break;

        case WordPopupMenu::ViewSubanagrams:
        variation = VariationSubanagrams;
        break;

        case WordPopupMenu::ViewHooks:
        variation = VariationHooks;
        break;

        case WordPopupMenu::ViewExtensions:
        variation = VariationExtensions;
        break;

        case WordPopupMenu::ViewAnagramHooks:
        variation = VariationAnagramHooks;
        break;

        case WordPopupMenu::ViewBlankAnagrams:
        variation = VariationBlankAnagrams;
        break;

        case WordPopupMenu::ViewBlankMatches:
        variation = VariationBlankMatches;
        break;

        case WordPopupMenu::ViewTranspositions:
        variation = VariationTranspositions;
        break;

        case WordPopupMenu::ExportList:
        exportRequested();
        return;

        case WordPopupMenu::CreateQuiz:
        createQuizRequested();
        return;

        default: break;
    }

    if (variation == VariationNone)
        return;

    WordVariationDialog* dialog = new WordVariationDialog (
        wordEngine, word, variation, this, "dialog", false,
        Qt::WDestructiveClose);
    dialog->show();
}

//---------------------------------------------------------------------------
//  exportRequested
//
//! Called when the user indicates that the word list should be exported.
//! Display a dialog to let the user choose where to export the list, then
//! actually do the export.
//---------------------------------------------------------------------------
void
WordListView::exportRequested()
{
    if (childCount() == 0) {
        QMessageBox::warning (this, "Cannot Save Word List",
                              "Cannot save word list:\nNo words to save.");
        return;
    }

    QString filename = QFileDialog::getSaveFileName
        (Auxil::getWordsDir() + "/saved", "Text Files (*.txt)", this,
         "exportDialog", "Save Word List");

    if (filename.isEmpty())
        return;

    if (!filename.endsWith (".txt", false))
        filename += ".txt";

    QString error;
    if (!exportFile (filename, &error)) {
        QMessageBox::warning (this, "Error Saving Word List",
                              "Cannot save word list:\n" + error + ".");
    }
}

//---------------------------------------------------------------------------
//  createQuizRequested
//
//! Called when the user indicates that the word list should be used to create
//! a quiz.  Display a quiz dialog with a single In Word List condition
//! already filled in with the contents of the list.
//---------------------------------------------------------------------------
void
WordListView::createQuizRequested()
{
    QuizSpec quizSpec;
    SearchSpec searchSpec;
    SearchCondition searchCondition;
    QString searchString;
    QListViewItem* item;
    for (item = firstChild(); item; item = item->nextSibling()) {
        if (!searchString.isEmpty())
            searchString += " ";
        searchString += static_cast<WordListViewItem*>(item)->getWord();
    }
    searchCondition.type = SearchCondition::InWordList;
    searchCondition.stringValue = searchString;
    searchSpec.conditions.push_back (searchCondition);
    quizSpec.setSearchSpec (searchSpec);
    MainWindow::getInstance()->newQuizFormInteractive (quizSpec);
}

//---------------------------------------------------------------------------
//  displayDefinition
//
//! Displays the definition of a word.
//
//! @param word the word whose definition to display
//---------------------------------------------------------------------------
void
WordListView::displayDefinition (const QString& word)
{
    if (!wordEngine)
        return;

    DefinitionDialog* dialog = new DefinitionDialog (wordEngine, word, this,
                                                     "dialog", false,
                                                     Qt::WDestructiveClose);
    dialog->show();
}

//---------------------------------------------------------------------------
//  showHooks
//
//! Change whether the list displays word hooks.
//
//! @param show true if hooks are to be displayed, false otherwise
//---------------------------------------------------------------------------
void
WordListView::showHooks (bool show)
{
    hidden[FRONT_HOOK_COLUMN] = hidden[BACK_HOOK_COLUMN] = !show;
    resizeColumns();
}

//---------------------------------------------------------------------------
//  showDefinitions
//
//! Change whether the list displays word definitions.
//
//! @param show true if definitions are to be displayed, false otherwise
//---------------------------------------------------------------------------
void
WordListView::showDefinitions (bool show)
{
    hidden[DEFINITION_COLUMN] = !show;
    resizeColumns();
}

//---------------------------------------------------------------------------
//  contextMenuEvent
//
//! Reimplementation of QWidget::contextMenuEvent.  Called when a context menu
//! is requested of this list view.
//
//! @param e the context menu event
//---------------------------------------------------------------------------
void
WordListView::contextMenuEvent (QContextMenuEvent* e)
{
    doPopupMenu (0, e->globalPos(), -1);
}

//---------------------------------------------------------------------------
//  exportFile
//
//! Export the words in the list to a file, one word per line.
//
//! @param filename the name of the file
//
//! @return true if successful or if the user cancels, false otherwise
//---------------------------------------------------------------------------
bool
WordListView::exportFile (const QString& filename, QString* err) const
{
    if (childCount() == 0) {
        if (err)
            *err = "No words to save";
        return false;
    }

    QFile file (filename);
    if (file.exists()) {
        int code = QMessageBox::warning (0, "Overwrite Existing File?",
                                         "The file already exists.  "
                                         "Overwrite it?", QMessageBox::Yes,
                                         QMessageBox::No);
        if (code != QMessageBox::Yes)
            return true;
    }

    if (!file.open (IO_WriteOnly | IO_Translate)) {
        if (err)
            *err = file.errorString();
        return false;
    }

    QTextStream stream (&file);
    for (QListViewItem* item = firstChild(); item; item = item->nextSibling())
    {
        stream << static_cast<WordListViewItem*>(item)->getWord();
        endl (stream);
    }

    return true;
}

//---------------------------------------------------------------------------
//  isFrontHook
//
//! Determine whether a word is a front hook.  A word is a front hook if its
//! first letter can be removed to form a valid word.
//
//! @param word the word, assumed to be upper case
//! @return true if the word is a front hook, false otherwise
//---------------------------------------------------------------------------
bool
WordListView::isFrontHook (const QString& word) const
{
    return wordEngine->isAcceptable (word.right (word.length() - 1));
}

//---------------------------------------------------------------------------
//  isBackHook
//
//! Determine whether a word is a back hook.  A word is a back hook if its
//! last letter can be removed to form a valid word.
//
//! @param word the word, assumed to be upper case
//! @return true if the word is a back hook, false otherwise
//---------------------------------------------------------------------------
bool
WordListView::isBackHook (const QString& word) const
{
    return wordEngine->isAcceptable (word.left (word.length() - 1));
}

//---------------------------------------------------------------------------
//  getFrontHookLetters
//
//! Get a string of letters that can be added to the front of a word to make
//! other valid words.
//
//! @param word the word, assumed to be upper case
//! @return a string containing lower case letters representing front hooks
//---------------------------------------------------------------------------
QString
WordListView::getFrontHookLetters (const QString& word) const
{
    SearchSpec spec;
    SearchCondition condition;
    condition.type = SearchCondition::PatternMatch;
    condition.stringValue = "?" + word;
    spec.conditions << condition;

    // Put first letter of each word in a set, for alphabetical order
    QStringList words = wordEngine->search (spec, true);
    set<QChar> letters;
    QStringList::iterator it;
    for (it = words.begin(); it != words.end(); ++it)
        letters.insert ((*it).at (0).lower());

    QString ret;
    set<QChar>::iterator sit;
    for (sit = letters.begin(); sit != letters.end(); ++sit)
        ret += *sit;
    return ret;
}

//---------------------------------------------------------------------------
//  getBackHookLetters
//
//! Get a string of letters that can be added to the back of a word to make
//! other valid words.
//
//! @param word the word, assumed to be upper case
//! @return a string containing lower case letters representing back hooks
//---------------------------------------------------------------------------
QString
WordListView::getBackHookLetters (const QString& word) const
{
    SearchSpec spec;
    SearchCondition condition;
    condition.type = SearchCondition::PatternMatch;
    condition.stringValue = word + "?";
    spec.conditions << condition;

    // Put first letter of each word in a set, for alphabetical order
    QStringList words = wordEngine->search (spec, true);
    set<QChar> letters;
    QStringList::iterator it;
    for (it = words.begin(); it != words.end(); ++it)
        letters.insert ((*it).at ((*it).length() - 1).lower());

    QString ret;
    set<QChar>::iterator sit;
    for (sit = letters.begin(); sit != letters.end(); ++sit)
        ret += *sit;
    return ret;
}
