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
#include "QuizEngine.h"
#include "WordValidator.h"
#include "Defs.h"
#include <qlayout.h>
#include <qgroupbox.h>
#include <qpushbutton.h>

using namespace Defs;

//---------------------------------------------------------------------------
// QuizForm
//
//! Constructor.
//
//! @param e the quiz engine
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
QuizForm::QuizForm (QuizEngine* e, QWidget* parent, const char* name,
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
    Q_CHECK_PTR (quizGbox);
    mainVlay->addWidget (quizGbox);

    QHBoxLayout* quizBoxHlay = new QHBoxLayout (quizGbox, MARGIN, SPACING,
                                                "quizBoxHlay");
    Q_CHECK_PTR (quizBoxHlay);
    quizBoxHlay->addStretch (1);
    quizLabel = new QLabel ("OPST", quizGbox, "label");
    Q_CHECK_PTR (quizLabel);
    quizBoxHlay->addWidget (quizLabel);
    quizBoxHlay->addStretch (1);

    responseList = new QListBox (this, "responseList");
    Q_CHECK_PTR (responseList);
    mainVlay->addWidget (responseList);

    recallLabel = new QLabel (this, "recallLabel");
    Q_CHECK_PTR (recallLabel);
    mainVlay->addWidget (recallLabel);

    precisionLabel = new QLabel (this, "precisionLabel");
    Q_CHECK_PTR (precisionLabel);
    mainVlay->addWidget (precisionLabel);

    inputLine = new QLineEdit (this, "inputLine");
    Q_CHECK_PTR (inputLine);
    WordValidator* validator = new WordValidator (inputLine);
    Q_CHECK_PTR (validator);
    inputLine->setValidator (validator);
    connect (inputLine, SIGNAL (returnPressed()), SLOT (responseEntered()));
    mainVlay->addWidget (inputLine);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    QPushButton* newQuizButton = new QPushButton ("&New Quiz", this,
                                                  "newQuizButton");
    Q_CHECK_PTR (newQuizButton);
    newQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (newQuizButton, SIGNAL (clicked()), SLOT (newQuizClicked()));
    buttonHlay->addWidget (newQuizButton);

    updateStats();
}

//---------------------------------------------------------------------------
// responseEntered
//
//! Called when a response is entered into the input line.
//---------------------------------------------------------------------------
void
QuizForm::responseEntered()
{
    QString response = inputLine->text();
    if (response.isEmpty())
        return;

    QuizEngine::ResponseStatus status = engine->respond (response);

    if (status == QuizEngine::Correct) {
        responseList->insertItem (response);
        inputLine->clear();
    }

    updateStats();
}

//---------------------------------------------------------------------------
// newQuizClicked
//
//! Called when the New Quiz button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::newQuizClicked()
{
    engine->newQuiz ("OPST");
    updateStats();
    responseList->clear();
}

//---------------------------------------------------------------------------
// updateStats
//
//! Update the recall and precision statistics.
//---------------------------------------------------------------------------
void
QuizForm::updateStats()
{
    int correct = engine->correct();
    setRecall (correct, engine->total());
    setPrecision (correct, correct + engine->incorrect());
}

//---------------------------------------------------------------------------
// setRecall
//
//! Set the recall numbers (correct user responses divided by total correct
//! responses).
//
//! @param correct the number of correct responses
//! @param total the total number of correct answers
//---------------------------------------------------------------------------
void
QuizForm::setRecall (int correct, int total)
{
    recallLabel->setText ("Recall: " + QString::number (correct)
                           + " / " + QString::number (total));
}

//---------------------------------------------------------------------------
// setPrecision
//
//! Set the precision numbers (correct user responses divided by total user
//! responses).
//
//! @param correct the number of correct responses
//! @param total the total number of user responses
//---------------------------------------------------------------------------
void
QuizForm::setPrecision (int correct, int total)
{
    precisionLabel->setText ("Precision: " + QString::number (correct)
                             + " / " + QString::number (total));
}
