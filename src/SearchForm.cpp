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
    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, 0, "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lineHlay = new QHBoxLayout (0, "lineHlay");
    Q_CHECK_PTR (lineHlay);
    mainVlay->addLayout (lineHlay);

    QLabel* label = new QLabel ("Word : ", this, "label");
    Q_CHECK_PTR (label);
    lineHlay->addWidget (label);

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    WordValidator* validator = new WordValidator (wordLine);
    validator->setOptions (WordValidator::AllowQuestionMarks);
    wordLine->setValidator (validator);
    connect (wordLine, SIGNAL (returnPressed()), SLOT (search()));
    lineHlay->addWidget (wordLine);

    QGridLayout* resultGlay = new QGridLayout (1, 2);
    mainVlay->addLayout (resultGlay);

    QLabel* resultLabel = new QLabel ("Results : ", this, "resultLabel");
    resultGlay->addWidget (resultLabel, 0, 0, Qt::AlignTop);

    resultList = new QListBox (this, "resultList");
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

    if (engine->isAcceptable (word))
        resultList->insertItem (word);
}
