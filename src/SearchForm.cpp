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
#include "WordValidator.h"
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

    resultLabel = new QLabel (this, "resultLabel");
    Q_CHECK_PTR (resultLabel);
    specVlay->addWidget (resultLabel);
    updateResultLabel (0);

    resultList = new QListBox (this, "resultList");
    Q_CHECK_PTR (resultList);
    specVlay->addWidget (resultList);
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
    resultList->insertStringList (engine->search (specForm->getSearchSpec()));
    resultList->sort();
    updateResultLabel (resultList->count());
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
// updateResultLabel
//
//! Display the number of words currently in the search results.
//! @param num the number of words
//---------------------------------------------------------------------------
void
SearchForm::updateResultLabel (int num)
{
    QString text = "Search Results : " + QString::number (num) + " word";
    if (num != 1) text += "s";
    resultLabel->setText (text);
}
