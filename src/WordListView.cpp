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
#include "MainWindow.h"
#include "QuizSpec.h"
#include "WordEngine.h"
#include "WordPopupMenu.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
#include "Auxil.h"
#include <qfile.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <set>

using namespace std;

//---------------------------------------------------------------------------
//  WordListView
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
WordListView::WordListView (WordEngine* e, QWidget* parent, const char* name, WFlags
                      f)
    : QListView (parent, name, f), wordEngine (e)
{
    // Not all list views are going to be search results of course - this is a
    // convenient place holder to get a reasonable initial size
    addColumn ("Word");
    addColumn ("Front Hooks");
    addColumn ("Back Hooks");
    addColumn ("Definition");

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
    WordListViewItem* item = static_cast<WordListViewItem*> (firstChild());
    for (; item; item = static_cast<WordListViewItem*> (item->nextSibling()))
        item->setFont (font);
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
    SearchSpec spec;
    SearchCondition condition;

    condition.type = SearchCondition::PatternMatch;
    condition.stringValue = "?" + word;
    spec.conditions << condition;

    // Put first letter of each word in a set, for alphabetical order
    QStringList frontWords = wordEngine->search (spec, true);
    set<QChar> frontLetters;
    QStringList::iterator it;
    for (it = frontWords.begin(); it != frontWords.end(); ++it)
        frontLetters.insert ((*it).at (0).lower());

    QString front;
    set<QChar>::iterator sit;
    for (sit = frontLetters.begin(); sit != frontLetters.end(); ++sit)
        front += *sit;
    if (front.isEmpty())
        front = "-";

    // Put last letter of each word in a set, for alphabetical order
    spec.conditions.clear();
    condition.type = SearchCondition::PatternMatch;
    condition.stringValue = word + "?";
    spec.conditions << condition;

    // Put first letter of each word in a set, for alphabetical order
    QStringList backWords = wordEngine->search (spec, true);
    set<QChar> backLetters;
    for (it = backWords.begin(); it != backWords.end(); ++it)
        backLetters.insert ((*it).at ((*it).length() - 1).lower());

    QString back;
    for (sit = backLetters.begin(); sit != backLetters.end(); ++sit)
        back += *sit;
    if (back.isEmpty())
        back = "-";

    QString definition = wordEngine->getDefinition (word);

    // XXX: Populate all visible columns
    return new WordListViewItem (this, word, front, back, definition);
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
    displayDefinition (item->text (0).upper());
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
        word = item->text (0).upper();

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
        searchString += item->text (0);
    }
    searchCondition.type = SearchCondition::InWordList;
    searchCondition.stringValue = searchString;
    searchSpec.conditions.push_back (searchCondition);
    quizSpec.searchSpec = searchSpec;
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
//  setTitle
//
//! Set the displayed title of the list view.
//
//! @param title the title to display
//---------------------------------------------------------------------------
void
WordListView::setTitle (const QString& title)
{
    setColumnText (0, QIconSet(), title);
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

    if (!file.open (IO_WriteOnly)) {
        if (err)
            *err = file.errorString();
        return false;
    }

    QTextStream stream (&file);
    for (QListViewItem* item = firstChild(); item; item = item->nextSibling())
    {
        stream << item->text (0);
        endl (stream);
    }

    return true;
}
