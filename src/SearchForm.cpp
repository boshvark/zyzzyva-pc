//---------------------------------------------------------------------------
// SearchForm.cpp
//
// A form for searching for words, patterns, anagrams, etc.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#include "SearchForm.h"
#include "DefinitionDialog.h"
#include "SearchSpecForm.h"
#include "WordEngine.h"
#include "WordPopupMenu.h"
#include "WordValidator.h"
#include "ZListView.h"
#include "ZListViewItem.h"
#include "Defs.h"
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qlayout.h>

using namespace Defs;

//---------------------------------------------------------------------------
// SearchForm
//
//! Constructor.
//
//! @e the word engine
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchForm::SearchForm (WordEngine* e, QWidget* parent, const char* name,
                        WFlags f)
    : QFrame (parent, name, f), engine (e)
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this, MARGIN, SPACING,
                                             "mainHlay");
    Q_CHECK_PTR (mainHlay);

    QVBoxLayout* specVlay = new QVBoxLayout (SPACING, "specVlay");
    Q_CHECK_PTR (specVlay);
    mainHlay->addLayout (specVlay);

    specForm = new SearchSpecForm (this, "specForm");
    Q_CHECK_PTR (specForm);
    connect (specForm, SIGNAL (returnPressed()), SLOT (search()));
    specVlay->addWidget (specForm);

    lowerCaseCbox = new QCheckBox ("Use lower-case letters for wildcard "
                                   "matches", this, "lowerCaseCbox");
    Q_CHECK_PTR (lowerCaseCbox);
    specVlay->addWidget (lowerCaseCbox);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    specVlay->addLayout (buttonHlay);

    resetButton = new QPushButton ("&Reset", this, "resetButton");
    Q_CHECK_PTR (resetButton);
    resetButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (resetButton, SIGNAL (clicked()), SLOT (resetSpec()));
    buttonHlay->addWidget (resetButton);

    searchButton = new QPushButton ("&Search", this, "searchButton");
    Q_CHECK_PTR (searchButton);
    searchButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (searchButton, SIGNAL (clicked()), SLOT (search()));
    buttonHlay->addWidget (searchButton);

    resultList = new ZListView (this, "resultList");
    Q_CHECK_PTR (resultList);
    resultList->setResizeMode (QListView::LastColumn);
    resultList->addColumn ("Search Results");
    resultList->setShowSortIndicator (true);
    connect (resultList, SIGNAL (contextMenuRequested (QListViewItem*, const
                                                       QPoint&, int)),
             SLOT (menuRequested (QListViewItem*, const QPoint&, int)));
    specVlay->addWidget (resultList);
    updateResultTotal (0);
}

//---------------------------------------------------------------------------
// search
//
//! Search for the word or pattern in the edit area, and display the results
//! in the list box.
//---------------------------------------------------------------------------
void
SearchForm::search()
{
    QString pattern = specForm->getPattern();
    if (pattern.isEmpty()) return;

    QApplication::setOverrideCursor (Qt::waitCursor);
    resultList->clear();
    QStringList wordList = engine->search (specForm->getSearchSpec(),
                                           !lowerCaseCbox->isChecked());
    int numWords = 0;
    QStringList::iterator it;
    for (it = wordList.begin(); it != wordList.end(); ++it, ++numWords)
        new ZListViewItem (resultList, *it);

    resultList->sort();
    updateResultTotal (numWords);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
// search
//
//! Search for the word or pattern in the edit area, and display the results
//! in the list box.
//---------------------------------------------------------------------------
void
SearchForm::resetSpec()
{
    specForm->reset();
}

//---------------------------------------------------------------------------
// updateResultTotal
//
//! Display the number of words currently in the search results.
//! @param num the number of words
//---------------------------------------------------------------------------
void
SearchForm::updateResultTotal (int num)
{
    QString text = "Search Results : " + QString::number (num) + " word";
    if (num != 1) text += "s";
    resultList->setColumnText (0, QIconSet(), text);
}

//---------------------------------------------------------------------------
// menuRequested
//
//! Called when a right-click menu is requested.
//! @param item the selected listview item
//! @param point the point at which the menu was requested
//---------------------------------------------------------------------------
void
SearchForm::menuRequested (QListViewItem* item, const QPoint& point, int)
{
    if (!item)
        return;

    WordPopupMenu* menu = new WordPopupMenu (this, "menu");
    int choice = menu->exec(point);
    delete menu;

    if (choice == WordPopupMenu::ShowDefinition) {
        DefinitionDialog* dialog = new DefinitionDialog (engine, item->text
                                                         (0), this, "dialog",
                                                         true);
        dialog->exec();
        delete dialog;
    }
}
