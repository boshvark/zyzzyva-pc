//---------------------------------------------------------------------------
// SearchForm.cpp
//
// A form for searching for words, patterns, anagrams, etc.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
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
    validator->setOptions (WordValidator::AllowQuestionMarks);
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

    if (patternButton->isChecked())
        resultList->insertStringList (engine->matchPattern (word));
    else if (anagramButton->isChecked())
        resultList->insertStringList (engine->matchAnagram (word));
    else if (subanagramButton->isChecked())
        resultList->insertStringList (engine->matchSubanagram (word));

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
