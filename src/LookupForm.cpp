//---------------------------------------------------------------------------
// LookupForm.cpp
//
// A form for looking up words.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "LookupForm.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include <qlabel.h>
#include <qlayout.h>

//---------------------------------------------------------------------------
// LookupForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
LookupForm::LookupForm (WordEngine* e, QWidget* parent, const char* name,
                        WFlags f)
    : QFrame (parent, name, f), engine (e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, 0, "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lookupHlay = new QHBoxLayout (0, "lookupHlay");
    Q_CHECK_PTR (lookupHlay);
    mainVlay->addLayout (lookupHlay);

    QLabel* label = new QLabel ("Look Up : ", this, "label");
    Q_CHECK_PTR (label);
    lookupHlay->addWidget (label);

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    wordLine->setValidator (new WordValidator (wordLine));
    connect (wordLine, SIGNAL (returnPressed()), SLOT (lookupWord()));
    lookupHlay->addWidget (wordLine);

    QHBoxLayout* resultHlay = new QHBoxLayout (0, "resultHlay");
    mainVlay->addLayout (resultHlay);
    resultHlay->addStretch (1);
    resultLabel = new QLabel (this, "resultLabel");
    resultHlay->addWidget (resultLabel);
    resultHlay->addStretch (1);
}

//---------------------------------------------------------------------------
// lookupWord
//
//! Look up and display the acceptability of the word currently in the word
//! edit area.
//---------------------------------------------------------------------------
void
LookupForm::lookupWord()
{
    QString word = wordLine->text();
    if (word.isEmpty()) return;
    resultLabel->setText (word + " : " +
                          (engine->isAcceptable (word) ?
                           QString ("Acceptable") :
                           QString ("Unacceptable")));
}
