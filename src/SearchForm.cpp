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
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

//---------------------------------------------------------------------------
// SearchForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchForm::SearchForm (WordEngine* e, QWidget* parent, const char* name,
                        WFlags f)
    : QFrame (parent, name, f), engine (e)
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this, 0, 0, "mainHlay");
    Q_CHECK_PTR (mainHlay);

    QVBoxLayout* optionVlay = new QVBoxLayout (0, "optionVlay");
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
    subanagramButton = new QRadioButton ("Build", optionGroup,
                                         "subanagramButton");
    Q_CHECK_PTR (subanagramButton);

    optionVlay->addStretch (1);

    QVBoxLayout* searchVlay = new QVBoxLayout (0, "searchVlay");
    Q_CHECK_PTR (searchVlay);
    mainHlay->addLayout (searchVlay);

    QHBoxLayout* lineHlay = new QHBoxLayout (0, "lineHlay");
    Q_CHECK_PTR (lineHlay);
    searchVlay->addLayout (lineHlay);

    QLabel* label = new QLabel ("Word : ", this, "label");
    Q_CHECK_PTR (label);
    lineHlay->addWidget (label);

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    WordValidator* validator = new WordValidator (wordLine);
    Q_CHECK_PTR (validator);
    validator->setOptions (WordValidator::AllowQuestionMarks);
    wordLine->setValidator (validator);
    connect (wordLine, SIGNAL (returnPressed()), SLOT (search()));
    lineHlay->addWidget (wordLine);

    QGridLayout* resultGlay = new QGridLayout (1, 2);
    Q_CHECK_PTR (resultGlay);
    searchVlay->addLayout (resultGlay);

    QLabel* resultLabel = new QLabel ("Results : ", this, "resultLabel");
    Q_CHECK_PTR (resultLabel);
    resultGlay->addWidget (resultLabel, 0, 0, Qt::AlignTop);

    resultList = new QListBox (this, "resultList");
    Q_CHECK_PTR (resultList);
    resultGlay->addWidget (resultList, 0, 1);
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
    resultList->clear();

    QString word = wordLine->text();
    if (word.isEmpty()) return;

    resultList->insertStringList (engine->matchPattern (word));
    resultList->sort();
}
