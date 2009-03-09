//---------------------------------------------------------------------------
// WordTableView.cpp
//
// A class derived from QTableView, used to display word lists.
//
// Copyright 2005, 2006, 2007, 2008, 2009 Michael W Thelen <mthelen@gmail.com>.
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

#include "WordTableView.h"
#include "CardboxAddDialog.h"
#include "CardboxRemoveDialog.h"
#include "DefinitionDialog.h"
#include "MainSettings.h"
#include "MainWindow.h"
#include "QuizDatabase.h"
#include "QuizSpec.h"
#include "SearchSpec.h"
#include "WordEngine.h"
#include "WordListSaveDialog.h"
#include "WordTableModel.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
#include "Auxil.h"
#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QSignalMapper>
#include <QTextStream>
#include <QToolTip>

using namespace std;

const int WordTableModel::ITEM_XPADDING = 5;
const int WordTableModel::ITEM_YPADDING = 0;

//---------------------------------------------------------------------------
//  WordTableView
//
//! Constructor.
//
//! @param parent the parent object
//---------------------------------------------------------------------------
WordTableView::WordTableView(WordEngine* e, QWidget* parent)
    : QTreeView(parent), wordEngine(e)
{
    setFocusPolicy(Qt::NoFocus);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(false);

    // FIXME: Once Trolltech fixes the assert in QHeaderView, continue with
    // statements like these
    //if (!MainSettings::getWordListShowHooks()) {
    //    setColumnHidden(WordTableModel::FRONT_HOOK_COLUMN, true);
    //    setColumnHidden(WordTableModel::BACK_HOOK_COLUMN, true);
    //}
}

//---------------------------------------------------------------------------
//  resizeItemsToContents
//
//! Resize all columns to fit the model contents.
//---------------------------------------------------------------------------
void
WordTableView::resizeItemsToContents()
{
//    for (int i = 0; i < model()->rowCount(); ++i)
//        resizeRowToContents(i);
    for (int i = 0; i < model()->columnCount(); ++i)
        resizeColumnToContents(i);
}

//---------------------------------------------------------------------------
//  viewDefinition
//
//! Display the definition of the currently selected word.
//---------------------------------------------------------------------------
void
WordTableView::viewDefinition()
{
    QModelIndex index = currentIndex();
    index = index.sibling(index.row(), WordTableModel::WORD_COLUMN);
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString word = wordModel->data(index, Qt::EditRole).toString();
    QString lexicon = wordModel->getLexicon();
    DefinitionDialog* dialog = new DefinitionDialog(wordEngine, lexicon, word,
                                                    this);
    Q_CHECK_PTR(dialog);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

//---------------------------------------------------------------------------
//  viewVariation
//
//! Display a variation of the currently selected word.
//
//! @param variation the variation to display
//---------------------------------------------------------------------------
void
WordTableView::viewVariation(int variation)
{
    QModelIndex index = currentIndex();
    index = index.sibling(index.row(), WordTableModel::WORD_COLUMN);
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString word = wordModel->data(index, Qt::EditRole).toString();
    QString lexicon = wordModel->getLexicon();
    WordVariationType type = static_cast<WordVariationType>(variation);
    WordVariationDialog* dialog = new WordVariationDialog(wordEngine, lexicon,
                                                          word, type, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    Q_CHECK_PTR(dialog);
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
WordTableView::exportRequested()
{
    if (model()->rowCount() == 0) {
        QString caption = "Cannot Save Word List";
        QString message = "Cannot save word list:\nNo words to save.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    WordListSaveDialog* dialog = new WordListSaveDialog(this);
    Q_CHECK_PTR(dialog);
    int code = dialog->exec();
    if (code != QDialog::Accepted) {
        delete dialog;
        return;
    }

    QList<WordAttribute> attributes = dialog->getSelectedAttributes();
    WordListFormat format = dialog->getWordListFormat();
    delete dialog;

    if (attributes.empty())
        return;

    QString filename = QFileDialog::getSaveFileName(this, "Save Word List",
        Auxil::getUserWordsDir() + "/saved", "Text Files (*.txt)", 0,
        QFileDialog::DontConfirmOverwrite);

    if (filename.isEmpty())
        return;

    if (!filename.endsWith(".txt", Qt::CaseInsensitive))
        filename += ".txt";

    bool append = false;
    if (QFile::exists(filename)) {
        QFileInfo fileInfo (filename);
        QString caption = "File Exists";
        QString message = "An item named \"" + fileInfo.fileName() +
            "\" already exists in this location.  Do you want to replace "
            "it with the one you are saving, or append words to the "
            "existing file?";
        message = Auxil::dialogWordWrap(message);

        QMessageBox messageBox(QMessageBox::Warning, caption, message,
                               QMessageBox::NoButton, this);
        //QPushButton* replaceButton =
            messageBox.addButton("Replace", QMessageBox::AcceptRole);
        QPushButton* appendButton =
            messageBox.addButton("Append", QMessageBox::AcceptRole);
        QPushButton* cancelButton =
            messageBox.addButton("Cancel", QMessageBox::RejectRole);
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setDefaultButton(cancelButton);

        int code = messageBox.exec();
        if (code != QMessageBox::Accepted)
            return;

        if (messageBox.clickedButton() == appendButton)
            append = true;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString error;
    bool ok = exportFile(filename, format, attributes, &error, append);
    QApplication::restoreOverrideCursor();
    if (!ok) {
        QString caption = "Error Saving Word List";
        QString message = "Cannot save word list:\n" + error + ".";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
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
WordTableView::createQuizRequested()
{
    int numWords = model()->rowCount();
    if (numWords == 0) {
        QString caption = "Error Creating Quiz";
        QString message = "Cannot create quiz:\nNo words in the list.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    QuizSpec quizSpec;
    SearchSpec searchSpec;
    SearchCondition searchCondition;
    QString searchString;
    QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < numWords; ) {
        if (!searchString.isEmpty())
            searchString += " ";
        searchString += model()->data(index, Qt::EditRole).toString();
        index = index.sibling(++i, WordTableModel::WORD_COLUMN);
    }

    searchCondition.type = SearchCondition::InWordList;
    searchCondition.stringValue = searchString;
    searchSpec.conditions.push_back(searchCondition);
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString lexicon = wordModel->getLexicon();
    if (lexicon.isEmpty())
        lexicon = MainSettings::getDefaultLexicon();
    quizSpec.setLexicon(lexicon);
    quizSpec.setSearchSpec(searchSpec);
    MainWindow::getInstance()->newQuizFormInteractive(quizSpec);
}

//---------------------------------------------------------------------------
//  addToCardboxRequested
//
//! Called when the user indicates that the word list should be added to the
//! cardbox system.
//---------------------------------------------------------------------------
void
WordTableView::addToCardboxRequested()
{
    int numWords = model()->rowCount();
    if (numWords == 0) {
        QString caption = "Error Adding Words to Cardbox";
        QString message = "Cannot add words to cardbox:\n"
            "No words in the list.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    CardboxAddDialog* dialog = new CardboxAddDialog(this);
    Q_CHECK_PTR(dialog);

    QStringList words;
    QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < numWords; ) {
        words.append(model()->data(index, Qt::EditRole).toString());
        index = index.sibling(++i, WordTableModel::WORD_COLUMN);
    }

    dialog->setWords(words);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        WordTableModel* wordModel = static_cast<WordTableModel*>(model());
        QString lexicon = wordModel->getLexicon();
        QStringList words = dialog->getWords();
        QString quizType = dialog->getQuizType();
        bool estimateCardbox = dialog->getEstimateCardbox();
        int cardbox = dialog->getSpecifyCardbox();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool ok = addToCardbox(words, lexicon, quizType, estimateCardbox,
                               cardbox);
        QApplication::restoreOverrideCursor();

        if (!ok) {
            QString caption = "Error Adding Words to Cardbox";
            QString message = "An error occurred while adding words to the "
                "cardbox system.";
            message = Auxil::dialogWordWrap(message);
            QMessageBox::warning(this, caption, message);
        }
    }

    delete dialog;
}

//---------------------------------------------------------------------------
//  removeFromCardboxRequested
//
//! Called when the user indicates that the word list should be added to the
//! cardbox system.
//---------------------------------------------------------------------------
void
WordTableView::removeFromCardboxRequested()
{
    int numWords = model()->rowCount();
    if (numWords == 0) {
        QString caption = "Error Removing Words from Cardbox";
        QString message = "Cannot remove words from cardbox:\n"
            "No words in the list.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        return;
    }

    CardboxRemoveDialog* dialog = new CardboxRemoveDialog(this);
    Q_CHECK_PTR(dialog);

    QStringList words;
    QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < numWords; ) {
        words.append(model()->data(index, Qt::EditRole).toString());
        index = index.sibling(++i, WordTableModel::WORD_COLUMN);
    }

    dialog->setWords(words);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        WordTableModel* wordModel = static_cast<WordTableModel*>(model());
        QString lexicon = wordModel->getLexicon();
        QStringList words = dialog->getWords();
        QString quizType = dialog->getQuizType();

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        bool ok = removeFromCardbox(words, lexicon, quizType);
        QApplication::restoreOverrideCursor();

        if (!ok) {
            QString caption = "Error Removing Words from Cardbox";
            QString message = "An error occurred while removing words "
                "from the cardbox system.";
            message = Auxil::dialogWordWrap(message);
            QMessageBox::warning(this, caption, message);
        }
    }

    delete dialog;
}

//---------------------------------------------------------------------------
//  exportFile
//
//! Export the words in the list to a file, one word per line.
//
//! @param filename the name of the file
//! @param format the save file format
//! @param attributes a list of attributes to export
//! @param err return error string on failure
//
//! @return true if successful or if the user cancels, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::exportFile(const QString& filename, WordListFormat format,
                          const QList<WordAttribute>& attributes,
                          QString* err, bool append) const
{
    if (model()->rowCount() == 0) {
        if (err)
            *err = "No words to save";
        return false;
    }

    QFile file (filename);
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Text;
    if (append)
        mode |= QIODevice::Append;

    if (!file.open(mode)) {
        if (err)
            *err = file.errorString();
        return false;
    }

    QTextStream stream (&file);

    if (format == WordListOnePerLine) {
        QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
        for (int i = 0; i < model()->rowCount(); ++i) {
            index = index.sibling(i, WordTableModel::WORD_COLUMN);
            QStringList strings = getExportStrings(index, attributes);
            stream << strings.join("\t");
            endl(stream);
        }
    }

    else if (format == WordListAnagramQuestionAnswer) {

        // Build map of alphagrams to indexes
        QMap<QString, QList<int> > alphaIndexes;
        QModelIndex index = model()->index(0, WordTableModel::WORD_COLUMN);
        for (int i = 0; i < model()->rowCount(); ++i) {
            index = index.sibling(i, WordTableModel::WORD_COLUMN);
            QString word = model()->data(index, Qt::EditRole).toString();
            QString alphagram = Auxil::getAlphagram(word);
            alphaIndexes[alphagram].append(i);
        }

        QMapIterator<QString, QList<int> > it (alphaIndexes);
        while (it.hasNext()) {
            it.next();
            stream << "Q: " << it.key();
            endl(stream);

            QListIterator<int> jt (it.value());
            while (jt.hasNext()) {
                int row = jt.next();
                index = index.sibling(row, WordTableModel::WORD_COLUMN);
                QStringList strings = getExportStrings(index, attributes);
                stream << "A: " << strings.join(" ");
                endl(stream);
            }
            endl(stream);
        }
    }

    return true;
}

//---------------------------------------------------------------------------
//  getExportStrings
//
//! Get a list of strings to be exported corresponding to a model index and a
//! list of word attributes.
//
//! @param index the index to use
//! @param attributes the list of word attributes to export
//
//! @return an ordered list of strings to be exported
//---------------------------------------------------------------------------
QStringList
WordTableView::getExportStrings(QModelIndex& index, const
                                QList<WordAttribute>& attributes) const
{
    QListIterator<WordAttribute> attrIt (attributes);
    QStringList strings;

    bool exportInnerHooks = attributes.contains(WordAttrInnerHooks);
    bool exportSymbols = attributes.contains(WordAttrLexiconSymbols);

    while (attrIt.hasNext()) {
        WordAttribute attribute = attrIt.next();
        int column = -1;
        switch (attribute) {
            case WordAttrWord:
            column = WordTableModel::WORD_COLUMN;
            break;

            case WordAttrDefinition:
            column = WordTableModel::DEFINITION_COLUMN;
            break;

            case WordAttrFrontHooks:
            column = WordTableModel::FRONT_HOOK_COLUMN;
            break;

            case WordAttrBackHooks:
            column = WordTableModel::BACK_HOOK_COLUMN;
            break;

            case WordAttrProbabilityOrder:
            column = WordTableModel::PROBABILITY_ORDER_COLUMN;
            break;

            default: break;
        }

        if (column < 0)
            continue;

        index = index.sibling(index.row(), column); QString str;

        // XXX: inner hook and symbols should be stored under separate roles
        // in the WordTableModel, so they can be easily queried

        // special processing for inner hooks and symbols
        if ((column == WordTableModel::WORD_COLUMN) &&
            (exportInnerHooks || exportSymbols))
        {
            str = model()->data(index, Qt::DisplayRole).toString().toUpper();
            if (!exportSymbols) {
                QString ihChar = (MainSettings::getWordListShowHookParents() ?
                                  QString(".") : QString());
                QRegExp regex(QString("([^\\W_\\d]+%1).*").arg(ihChar));
                str.replace(regex, "\\1");
            }
            else if (!exportInnerHooks &&
                     MainSettings::getWordListShowHookParents())
            {
                str.replace(QRegExp(".([^\\W_\\d]+)."), "\\1");
            }
        }
        else if (!exportSymbols &&
                 ((column == WordTableModel::FRONT_HOOK_COLUMN) ||
                  (column == WordTableModel::BACK_HOOK_COLUMN)))
        {
            str = model()->data(index, Qt::DisplayRole).toString();
            str.replace(QRegExp("[\\W_\\d]+"), QString());
        }
        else {
            str = model()->data(index, Qt::EditRole).toString();
        }
        strings.append(str);
    }

    return strings;
}

//---------------------------------------------------------------------------
//  addToCardbox
//
//! Add a list of words to the cardbox system.
//
//! @param words the words to add
//! @param lexicon the lexicon to use
//! @param quizType the quiz type to use
//! @param estimateCardbox whether to estimate cardboxes based on past
//! performance
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::addToCardbox(const QStringList& words, const QString& lexicon,
    const QString& quizType, bool estimateCardbox, int cardbox) const
{
    QuizSpec::QuizType type = Auxil::stringToQuizType(quizType);
    if (type == QuizSpec::UnknownQuizType)
        return false;

    QStringList questions;

    switch (type) {
        case QuizSpec::QuizAnagrams:
        case QuizSpec::QuizAnagramsWithHooks:
        case QuizSpec::QuizSubanagrams:
        case QuizSpec::QuizAnagramJumble:
        case QuizSpec::QuizSubanagramJumble: {
            QSet<QString> alphagramSet;
            QStringListIterator it (words);
            while (it.hasNext()) {
                alphagramSet.insert(Auxil::getAlphagram(it.next()));
            }
            questions = QStringList::fromSet(alphagramSet);
        }
        break;

        default:
        questions = words;
    }

    QuizDatabase db (lexicon, quizType);
    if (!db.isValid())
        return false;

    db.addToCardbox(questions, estimateCardbox, cardbox);
    return true;
}

//---------------------------------------------------------------------------
//  removeFromCardbox
//
//! Remove a list of words from the cardbox system.
//
//! @param words the words to remove
//! @param lexicon the lexicon to use
//! @param quizType the quiz type to use
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::removeFromCardbox(const QStringList& words, const QString&
                                 lexicon, const QString& quizType) const
{
    QuizSpec::QuizType type = Auxil::stringToQuizType(quizType);
    if (type == QuizSpec::UnknownQuizType)
        return false;

    QStringList questions;

    switch (type) {
        case QuizSpec::QuizAnagrams:
        case QuizSpec::QuizAnagramsWithHooks:
        case QuizSpec::QuizSubanagrams:
        case QuizSpec::QuizAnagramJumble:
        case QuizSpec::QuizSubanagramJumble: {
            QSet<QString> alphagramSet;
            QStringListIterator it (words);
            while (it.hasNext()) {
                alphagramSet.insert(Auxil::getAlphagram(it.next()));
            }
            questions = QStringList::fromSet(alphagramSet);
        }
        break;

        default:
        questions = words;
    }

    QuizDatabase db (lexicon, quizType);
    if (!db.isValid())
        return false;

    db.removeFromCardbox(questions);
    return true;
}

//---------------------------------------------------------------------------
//  hookToolTipText
//
//! This event handler, for event e, can be reimplemented in a subclass to
//! receive widget context menu events.
//
//! @param word the word whose hooks to display
//! @param hooks the hooks to display
//! @param front true if the hooks are fronthooks, false if back hooks
//
//! @return the tool tip text
//---------------------------------------------------------------------------
QString
WordTableView::hookToolTipText(const QString& word, const QString& hooks,
                               bool front) const
{
    WordTableModel* wordModel = static_cast<WordTableModel*>(model());
    QString lexicon = wordModel->getLexicon();

    QString text;
    int offset = 0;
    QRegExp regex("([^\\W_\\d])([\\W_\\d]*)?");
    while ((offset = regex.indexIn(hooks, offset)) >= 0) {
        QString hook = regex.cap(1);
        QString symbols = regex.cap(2);
        QString hookWord = (front ? (hook.toUpper() + word)
                                  : (word + hook.toUpper()));

        if (!text.isEmpty())
            text += "\n\n";

        text += hookWord + symbols + " : " +
            wordEngine->getDefinition(lexicon, hookWord);

        offset += regex.cap(0).length();
    }

    return text;
}

//---------------------------------------------------------------------------
//  contextMenuEvent
//
//! This event handler, for event e, can be reimplemented in a subclass to
//! receive widget context menu events.
//
//! @param e the context menu event
//
//! @return true if successful or if the user cancels, false otherwise
//---------------------------------------------------------------------------
void
WordTableView::contextMenuEvent(QContextMenuEvent* e)
{
    QModelIndex index = indexAt(e->pos());
    bool wordOptions = (index.isValid() &&
                        (index.row() >= 0) && (index.column() >= 0));

    QMenu* popupMenu = new QMenu;
    Q_CHECK_PTR(popupMenu);

    if (wordOptions) {
        QMenu* wordMenu = new QMenu("Word");
        Q_CHECK_PTR(wordMenu);
        popupMenu->addMenu(wordMenu);

        QSignalMapper* wordMapper = new QSignalMapper(wordMenu);
        Q_CHECK_PTR(wordMapper);

        // Word Definition
        QAction* definitionAction = new QAction("Definition", wordMenu);
        Q_CHECK_PTR(definitionAction);
        connect(definitionAction, SIGNAL(triggered()), SLOT(viewDefinition()));
        wordMenu->addAction(definitionAction);

        // Word Anagrams
        QAction* anagramsAction = new QAction("Anagrams", wordMenu);
        Q_CHECK_PTR(anagramsAction);
        connect(anagramsAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
        wordMapper->setMapping(anagramsAction, VariationAnagrams);
        wordMenu->addAction(anagramsAction);

        // Word Subanagrams
        QAction* subanagramsAction = new QAction("Subanagrams", wordMenu);
        Q_CHECK_PTR(subanagramsAction);
        connect(subanagramsAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(subanagramsAction, VariationSubanagrams);
        wordMenu->addAction(subanagramsAction);

        // Word Hooks
        QAction* hooksAction = new QAction("Hooks", wordMenu);
        Q_CHECK_PTR(hooksAction);
        connect(hooksAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
        wordMapper->setMapping(hooksAction, VariationHooks);
        wordMenu->addAction(hooksAction);

        // Word Extensions
        QAction* extensionsAction = new QAction("Extensions", wordMenu);
        Q_CHECK_PTR(extensionsAction);
        connect(extensionsAction, SIGNAL(triggered()), wordMapper, SLOT(map()));
        wordMapper->setMapping(extensionsAction, VariationExtensions);
        wordMenu->addAction(extensionsAction);

        // Word Anagram Hooks
        QAction* anagramHooksAction = new QAction("Anagram Hooks", wordMenu);
        Q_CHECK_PTR(anagramHooksAction);
        connect(anagramHooksAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(anagramHooksAction, VariationAnagramHooks);
        wordMenu->addAction(anagramHooksAction);

        // Word Blank Anagrams
        QAction* blankAnagramsAction = new QAction("Blank Anagrams", wordMenu);
        Q_CHECK_PTR(blankAnagramsAction);
        connect(blankAnagramsAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(blankAnagramsAction, VariationBlankAnagrams);
        wordMenu->addAction(blankAnagramsAction);

        // Word Blank Matches
        QAction* blankMatchesAction = new QAction("Blank Matches", wordMenu);
        Q_CHECK_PTR(blankMatchesAction);
        connect(blankMatchesAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(blankMatchesAction, VariationBlankMatches);
        wordMenu->addAction(blankMatchesAction);

        // Word Transpositions
        QAction* transpositionsAction = new QAction("Transpositions", wordMenu);
        Q_CHECK_PTR(transpositionsAction);
        connect(transpositionsAction, SIGNAL(triggered()),
                wordMapper, SLOT(map()));
        wordMapper->setMapping(transpositionsAction, VariationTranspositions);
        wordMenu->addAction(transpositionsAction);

        // Connect Word signal mappings to viewVariation
        connect(wordMapper, SIGNAL(mapped(int)), SLOT(viewVariation(int)));
    }

    QAction* exportAction = new QAction("Save list...", popupMenu);
    Q_CHECK_PTR(exportAction);
    connect(exportAction, SIGNAL(triggered()), SLOT(exportRequested()));
    popupMenu->addAction(exportAction);

    QAction* createQuizAction = new QAction("Quiz from list...", popupMenu);
    Q_CHECK_PTR(createQuizAction);
    connect(createQuizAction, SIGNAL(triggered()), SLOT(createQuizRequested()));
    popupMenu->addAction(createQuizAction);

    QAction* addToCardboxAction =
        new QAction("Add list to Cardbox...", popupMenu);
    Q_CHECK_PTR(addToCardboxAction);
    connect(addToCardboxAction, SIGNAL(triggered()),
             SLOT(addToCardboxRequested()));
    popupMenu->addAction(addToCardboxAction);

    QAction* removeFromCardboxAction =
        new QAction("Remove list from Cardbox...", popupMenu);
    Q_CHECK_PTR(removeFromCardboxAction);
    connect(removeFromCardboxAction, SIGNAL(triggered()),
            SLOT(removeFromCardboxRequested()));
    popupMenu->addAction(removeFromCardboxAction);

    popupMenu->exec(QCursor::pos());
    delete popupMenu;
}

//---------------------------------------------------------------------------
//  viewportEvent
//
//! This event handler, for event e, can be reimplemented in a subclass to
//! receive viewport events.
//
//! @param e the viewport event
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableView::viewportEvent(QEvent* e)
{
    switch (e->type()) {
        case QEvent::ToolTip: {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
            QModelIndex index = indexAt(helpEvent->pos());
            if (!index.isValid())
                break;

            QString toolTipText;
            switch (index.column()) {
                case WordTableModel::DEFINITION_COLUMN:
                toolTipText = index.model()->data(index,
                                                  Qt::DisplayRole).toString();
                break;

                case WordTableModel::FRONT_HOOK_COLUMN:
                case WordTableModel::BACK_HOOK_COLUMN: {
                    QString hooks =
                        index.model()->data(index, Qt::EditRole).toString();
                    QString word = index.model()->data(
                        index.sibling(index.row(), WordTableModel::WORD_COLUMN),
                        Qt::EditRole).toString();
                    bool front = (index.column() ==
                                  WordTableModel::FRONT_HOOK_COLUMN);

                    toolTipText = hookToolTipText(word, hooks, front);
                }
                break;

                default:
                break;
            }

            if (!toolTipText.isEmpty())
                QToolTip::showText(helpEvent->globalPos(), toolTipText);
        }
        return true;

        default:
        break;
    }

    return QTreeView::viewportEvent(e);
}

//---------------------------------------------------------------------------
//  sizeHintForColumn
//
//! Return the width size hint for a column.  Directly call the equivalent
//! function from QAbstractItemView instead of allowing the QTableView version
//! to take precedence, because we want the content of all rows to be
//! considered, not just the visible ones.
//
//! @param column the column index
//! @return the size hint for the column
//---------------------------------------------------------------------------
int
WordTableView::sizeHintForColumn(int column) const
{
    return QAbstractItemView::sizeHintForColumn(column) +
        (2 * WordTableModel::ITEM_XPADDING);
}

//---------------------------------------------------------------------------
//  sizeHintForRow
//
//! Return the height size hint for a row.  Directly call the equivalent
//! function from QAbstractItemView instead of allowing the QTableView version
//! to take precedence, because we want the content of all columns to be
//! considered, not just the visible ones.
//
//! @param column the column index
//! @return the size hint for the column
//---------------------------------------------------------------------------
int
WordTableView::sizeHintForRow(int row) const
{
    return QAbstractItemView::sizeHintForRow(row) +
        (2 * WordTableModel::ITEM_YPADDING);
}
