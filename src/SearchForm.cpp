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

    QVBoxLayout* optionVlay = new QVBoxLayout (SPACING, "optionVlay");
    Q_CHECK_PTR (optionVlay);
    mainHlay->addLayout (optionVlay);

    QButtonGroup* optionGroup = new QButtonGroup (3, QButtonGroup::Vertical,
                                                  "Search Type", this);
    Q_CHECK_PTR (optionGroup);
    optionGroup->setExclusive (true);
    optionVlay->addWidget (optionGroup);

    patternButton = new QRadioButton ("Pattern", optionGroup,
                                      "patternButton");
    Q_CHECK_PTR (patternButton);
    patternButton->setChecked (true);
    anagramButton = new QRadioButton ("Anagram", optionGroup,
                                      "anagramButton");
    Q_CHECK_PTR (anagramButton);
    subanagramButton = new QRadioButton ("Subanagram", optionGroup,
                                         "subanagramButton");
    Q_CHECK_PTR (subanagramButton);

    optionVlay->addStretch (1);

    QVBoxLayout* searchVlay = new QVBoxLayout (SPACING, "searchVlay");
    Q_CHECK_PTR (searchVlay);
    mainHlay->addLayout (searchVlay);

    QLabel* label = new QLabel ("Input", this, "label");
    Q_CHECK_PTR (label);
    searchVlay->addWidget (label);

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    WordValidator* validator = new WordValidator (wordLine);
    Q_CHECK_PTR (validator);
    validator->setOptions (WordValidator::AllowQuestionMarks |
                           WordValidator::AllowAsterisks);
    wordLine->setValidator (validator);
    connect (wordLine, SIGNAL (returnPressed()), SLOT (search()));
    searchVlay->addWidget (wordLine);

    resultLabel = new QLabel (this, "resultLabel");
    Q_CHECK_PTR (resultLabel);
    searchVlay->addWidget (resultLabel);
    updateResultLabel (0);

    resultList = new QListBox (this, "resultList");
    Q_CHECK_PTR (resultList);
    searchVlay->addWidget (resultList);
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
    QString word = wordLine->text();
    if (word.isEmpty()) return;

    QApplication::setOverrideCursor (Qt::waitCursor);
    resultList->clear();

    SearchSpec spec;
    spec.pattern = word;
    if (patternButton->isChecked())
        spec.type = Pattern;
    else if (anagramButton->isChecked())
        spec.type = Anagram;
    else if (subanagramButton->isChecked())
        spec.type = Subanagram;

    resultList->insertStringList (engine->search (spec));

    resultList->sort();
    updateResultLabel (resultList->count());
    QApplication::restoreOverrideCursor();
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
