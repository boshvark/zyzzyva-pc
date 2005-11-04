//---------------------------------------------------------------------------
// WordTableView.cpp
//
// A model for representing word lists.
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

#include "WordTableView.h"
#include "DefinitionDialog.h"
#include "MainWindow.h"
#include "QuizSpec.h"
#include "SearchSpec.h"
#include "WordEngine.h"
#include "WordTableModel.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
#include "Auxil.h"
#include <QAction>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QSignalMapper>
#include <QTextStream>

using namespace std;

//---------------------------------------------------------------------------
//  WordTableView
//
//! Constructor.
//
//! @param parent the parent object
//---------------------------------------------------------------------------
WordTableView::WordTableView (WordEngine* e, QWidget* parent)
    : QTableView (parent), wordEngine (e)
{
    setSelectionBehavior (QAbstractItemView::SelectRows);
    setSelectionMode (QAbstractItemView::SingleSelection);

    // FIXME: Once Trolltech fixes the assert in QHeaderView, continue with
    // statements like these
    //if (!MainSettings::getWordListShowHooks()) {
    //    setColumnHidden (WordTableModel::FRONT_HOOK_COLUMN, true);
    //    setColumnHidden (WordTableModel::BACK_HOOK_COLUMN, true);
    //}
}

//---------------------------------------------------------------------------
//  resizeAllColumnsToContents
//
//! Resize all columns to fit the model contents.
//---------------------------------------------------------------------------
void
WordTableView::resizeAllColumnsToContents()
{
    for (int i = 0; i < model()->columnCount(); ++i)
        resizeColumnToContents (i);
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
    index = index.sibling (index.row(), WordTableModel::WORD_COLUMN);
    QString word = model()->data (index, Qt::UserRole).toString();
    DefinitionDialog* dialog = new DefinitionDialog (wordEngine, word, this,
                                                     Qt::WDestructiveClose);
    Q_CHECK_PTR (dialog);
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
WordTableView::viewVariation (int variation)
{
    QModelIndex index = currentIndex();
    index = index.sibling (index.row(), WordTableModel::WORD_COLUMN);
    QString word = model()->data (index, Qt::UserRole).toString();
    WordVariationType type = static_cast<WordVariationType>(variation);
    WordVariationDialog* dialog = new WordVariationDialog (wordEngine, word,
        type, this, Qt::WDestructiveClose);
    Q_CHECK_PTR (dialog);
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
        QMessageBox::warning (this, "Cannot Save Word List",
                              "Cannot save word list:\nNo words to save.");
        return;
    }

    QString filename = QFileDialog::getSaveFileName (this, "Save Word List",
        Auxil::getWordsDir() + "/saved", "Text Files (*.txt)");

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
WordTableView::createQuizRequested()
{
    int numWords = model()->rowCount();
    if (numWords == 0) {
        QMessageBox::warning (this, "Error Creating Quiz",
                              "Cannot create quiz:\nNo words in the list.");
    }

    QuizSpec quizSpec;
    SearchSpec searchSpec;
    SearchCondition searchCondition;
    QString searchString;
    QModelIndex index = model()->index (0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < numWords; ) {
        if (!searchString.isEmpty())
            searchString += " ";
        searchString += model()->data (index, Qt::UserRole).toString();
        index = index.sibling (++i, WordTableModel::WORD_COLUMN);
    }

    searchCondition.type = SearchCondition::InWordList;
    searchCondition.stringValue = searchString;
    searchSpec.conditions.push_back (searchCondition);
    quizSpec.setSearchSpec (searchSpec);
    MainWindow::getInstance()->newQuizFormInteractive (quizSpec);
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
WordTableView::exportFile (const QString& filename, QString* err) const
{
    if (model()->rowCount() == 0) {
        if (err)
            *err = "No words to save";
        return false;
    }

    // FIXME Qt4: QIODevice::Translate does not exist!
    //if (!file.open (QIODevice::WriteOnly | QIODevice::Translate)) {
    QFile file (filename);
    if (!file.open (QIODevice::WriteOnly)) {
        if (err)
            *err = file.errorString();
        return false;
    }

    QTextStream stream (&file);
    QModelIndex index = model()->index (0, WordTableModel::WORD_COLUMN);
    for (int i = 0; i < model()->rowCount(); ) {
        QString word = model()->data (index, Qt::UserRole).toString();
        stream << word;
        endl (stream);
        index = index.sibling (++i, WordTableModel::WORD_COLUMN);
    }

    return true;
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
WordTableView::contextMenuEvent (QContextMenuEvent* e)
{
    int row = rowAt (e->y());
    int column = columnAt (e->x());
    bool wordOptions = ((row >= 0) && (column >= 0));

    QMenu* popupMenu = new QMenu;
    Q_CHECK_PTR (popupMenu);

    if (wordOptions) {
        QMenu* viewMenu = new QMenu ("View");
        Q_CHECK_PTR (viewMenu);
        popupMenu->addMenu (viewMenu);

        QSignalMapper* viewMapper = new QSignalMapper (viewMenu);
        Q_CHECK_PTR (viewMapper);

        // View Definition
        QAction* definitionAction = new QAction ("Definition", viewMenu);
        Q_CHECK_PTR (definitionAction);
        connect (definitionAction, SIGNAL (triggered()),
                 SLOT (viewDefinition()));
        viewMenu->addAction (definitionAction);

        // View Anagrams
        QAction* anagramsAction = new QAction ("Anagrams", viewMenu);
        Q_CHECK_PTR (anagramsAction);
        connect (anagramsAction, SIGNAL (triggered()),
                 viewMapper, SLOT (map()));
        viewMapper->setMapping (anagramsAction, VariationAnagrams);
        viewMenu->addAction (anagramsAction);

        // View Subanagrams
        QAction* subanagramsAction = new QAction ("Subanagrams", viewMenu);
        Q_CHECK_PTR (subanagramsAction);
        connect (subanagramsAction, SIGNAL (triggered()),
                 viewMapper, SLOT (map()));
        viewMapper->setMapping (subanagramsAction, VariationSubanagrams);
        viewMenu->addAction (subanagramsAction);

        // View Hooks
        QAction* hooksAction = new QAction ("Hooks", viewMenu);
        Q_CHECK_PTR (hooksAction);
        connect (hooksAction, SIGNAL (triggered()), viewMapper, SLOT (map()));
        viewMapper->setMapping (hooksAction, VariationHooks);
        viewMenu->addAction (hooksAction);

        // View Extensions
        QAction* extensionsAction = new QAction ("Extensions", viewMenu);
        Q_CHECK_PTR (extensionsAction);
        connect (extensionsAction, SIGNAL (triggered()),
                 viewMapper, SLOT (map()));
        viewMapper->setMapping (extensionsAction, VariationExtensions);
        viewMenu->addAction (extensionsAction);

        // View Anagram Hooks
        QAction* anagramHooksAction = new QAction ("Anagram Hooks", viewMenu);
        Q_CHECK_PTR (anagramHooksAction);
        connect (anagramHooksAction, SIGNAL (triggered()),
                 viewMapper, SLOT (map()));
        viewMapper->setMapping (anagramHooksAction, VariationAnagramHooks);
        viewMenu->addAction (anagramHooksAction);

        // View Blank Anagrams
        QAction* blankAnagramsAction = new QAction ("Blank Anagrams",
                                                    viewMenu);
        Q_CHECK_PTR (blankAnagramsAction);
        connect (blankAnagramsAction, SIGNAL (triggered()),
                 viewMapper, SLOT (map()));
        viewMapper->setMapping (blankAnagramsAction, VariationBlankAnagrams);
        viewMenu->addAction (blankAnagramsAction);

        // View Blank Matches
        QAction* blankMatchesAction = new QAction ("Blank Matches", viewMenu);
        Q_CHECK_PTR (blankMatchesAction);
        connect (blankMatchesAction, SIGNAL (triggered()),
                 viewMapper, SLOT (map()));
        viewMapper->setMapping (blankMatchesAction, VariationBlankMatches);
        viewMenu->addAction (blankMatchesAction);

        // View Transpositions
        QAction* transpositionsAction = new QAction ("Transpositions",
                                                     viewMenu);
        Q_CHECK_PTR (transpositionsAction);
        connect (transpositionsAction, SIGNAL (triggered()),
                 viewMapper, SLOT (map()));
        viewMapper->setMapping (transpositionsAction,
                                VariationTranspositions);
        viewMenu->addAction (transpositionsAction);

        // Connect View signal mappings to viewVariation
        connect (viewMapper, SIGNAL (mapped (int)),
                 SLOT (viewVariation (int)));
    }

    QAction* exportAction = new QAction ("Save list...", popupMenu);
    Q_CHECK_PTR (exportAction);
    connect (exportAction, SIGNAL (triggered()), SLOT (exportRequested()));
    popupMenu->addAction (exportAction);

    QAction* createQuizAction = new QAction ("Quiz from list...", popupMenu);
    Q_CHECK_PTR (createQuizAction);
    connect (createQuizAction, SIGNAL (triggered()),
             SLOT (createQuizRequested()));
    popupMenu->addAction (createQuizAction);

    popupMenu->exec (QCursor::pos());
    delete popupMenu;
}
