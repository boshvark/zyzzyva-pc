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
#include "SearchSpecForm.h"
#include "WordEngine.h"
#include "WordListView.h"
#include "WordListViewItem.h"
#include "Defs.h"
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qsplitter.h>

using namespace Defs;

//---------------------------------------------------------------------------
//  SearchForm
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
    : ActionForm (SearchFormType, parent, name, f), engine (e)
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
    connect (specForm, SIGNAL (contentsChanged()), SLOT (specChanged()));
    specVlay->addWidget (specForm);

    //lowerCaseCbox = new QCheckBox ("Use lower-case letters for wildcard "
    //                               "matches", this, "lowerCaseCbox");
    //Q_CHECK_PTR (lowerCaseCbox);
    //specVlay->addWidget (lowerCaseCbox);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    specVlay->addLayout (buttonHlay);

    searchButton = new QPushButton ("&Search", this, "searchButton");
    Q_CHECK_PTR (searchButton);
    searchButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (searchButton, SIGNAL (clicked()), SLOT (search()));
    buttonHlay->addWidget (searchButton);

    resultList = new WordListView (engine, this, "resultList");
    Q_CHECK_PTR (resultList);
    resultList->setResizeMode (QListView::LastColumn);
    //resultList->setTitle ("Search Results");
    resultList->setShowSortIndicator (true);
    specVlay->addWidget (resultList, 1);

    updateResultTotal (0);
    specChanged();
}

//---------------------------------------------------------------------------
//  search
//
//! Search for the word or pattern in the edit area, and display the results
//! in the list box.
//---------------------------------------------------------------------------
void
SearchForm::search()
{
    SearchSpec spec = specForm->getSearchSpec();
    if (spec.conditions.empty())
        return;

    QApplication::setOverrideCursor (Qt::waitCursor);
    resultList->clear();
    QStringList wordList = engine->search (specForm->getSearchSpec(), true);
                                           //!lowerCaseCbox->isChecked());
    int numWords = 0;
    QStringList::iterator it;
    for (it = wordList.begin(); it != wordList.end(); ++it, ++numWords)
        resultList->addWord (*it);

    resultList->sort();
    updateResultTotal (numWords);
    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  specChanged
//
//! Called when the contents of the search spec form change.  Enable or
//! disable the Search button appropriately.
//---------------------------------------------------------------------------
void
SearchForm::specChanged()
{
    searchButton->setEnabled (specForm->isValid());
}

//---------------------------------------------------------------------------
//  updateResultTotal
//
//! Display the number of words currently in the search results.
//! @param num the number of words
//---------------------------------------------------------------------------
void
SearchForm::updateResultTotal (int num)
{
    QString text = "Search Results : " + QString::number (num) + " word";
    if (num != 1) text += "s";

    // FIXME: Don't set listview title.  Instead update a label on the form.
    //resultList->setTitle (text);
}
