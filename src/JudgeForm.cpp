//---------------------------------------------------------------------------
// JudgeForm.cpp
//
// A form for looking up words.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "JudgeForm.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "Defs.h"
#include <qlayout.h>

using namespace Defs;

//---------------------------------------------------------------------------
// JudgeForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
JudgeForm::JudgeForm (WordEngine* e, QWidget* parent, const char* name,
                        WFlags f)
    : QFrame (parent, name, f), engine (e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lookupHlay = new QHBoxLayout (SPACING, "lookupHlay");
    Q_CHECK_PTR (lookupHlay);
    mainVlay->addLayout (lookupHlay);

    QLabel* label = new QLabel ("Word : ", this, "label");
    Q_CHECK_PTR (label);
    lookupHlay->addWidget (label);

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    wordLine->setValidator (new WordValidator (wordLine));
    connect (wordLine, SIGNAL (returnPressed()), SLOT (lookupWord()));
    lookupHlay->addWidget (wordLine);

    QHBoxLayout* resultHlay = new QHBoxLayout (SPACING, "resultHlay");
    mainVlay->addLayout (resultHlay);
    resultLabel = new QLabel (this, "resultLabel");
    resultHlay->addWidget (resultLabel, 0, Qt::AlignHCenter);
}

//---------------------------------------------------------------------------
// lookupWord
//
//! Look up and display the acceptability of the word currently in the word
//! edit area.
//---------------------------------------------------------------------------
void
JudgeForm::lookupWord()
{
    QString word = wordLine->text();
    if (word.isEmpty()) return;
    QString statusStr = engine->isAcceptable (word) ?
                        QString ("<font color=\"blue\">Acceptable</font>") :
                        QString ("<font color=\"red\">Unacceptable</font>");
    resultLabel->setText (word + " : " + statusStr);
}
