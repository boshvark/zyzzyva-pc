//---------------------------------------------------------------------------
// QuizForm.cpp
//
// A form for quizzing the user.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "QuizForm.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "Defs.h"
#include <qlayout.h>
#include <qgroupbox.h>

using namespace Defs;

//---------------------------------------------------------------------------
// QuizForm
//
//! Constructor.
//
//! @param e the word engine
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
QuizForm::QuizForm (WordEngine* e, QWidget* parent, const char* name,
                    WFlags f)
    : QFrame (parent, name, f), engine (e)
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this, MARGIN, SPACING,
                                             "mainHlay");
    Q_CHECK_PTR (mainHlay);

    QVBoxLayout* mainVlay = new QVBoxLayout (SPACING, "mainVlay");
    Q_CHECK_PTR (mainVlay);
    mainHlay->addLayout (mainVlay);

    QGroupBox* quizGbox = new QGroupBox (this, "quizGbox");
    mainVlay->addWidget (quizGbox);

    QHBoxLayout* quizBoxHlay = new QHBoxLayout (quizGbox, MARGIN, SPACING,
                                                "quizBoxHlay");
    quizBoxHlay->addStretch (1);
    quizLabel = new QLabel ("Hello, world!", quizGbox, "label");
    quizBoxHlay->addWidget (quizLabel);
    quizBoxHlay->addStretch (1);

    mainVlay->addStretch (1);
}
