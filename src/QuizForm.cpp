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
#include <qmessagebox.h>

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
    questionLabel = new QLabel (quizGbox, "label");
    Q_CHECK_PTR (questionLabel);
    quizBoxHlay->addWidget (questionLabel);
    quizBoxHlay->addStretch (1);

    responseList = new QListBox (this, "responseList");
    Q_CHECK_PTR (responseList);
    mainVlay->addWidget (responseList);

    QHBoxLayout* statsHlay = new QHBoxLayout (SPACING, "statsHlay");
    Q_CHECK_PTR (statsHlay);
    mainVlay->addLayout (statsHlay);

    recallLabel = new QLabel (this, "recallLabel");
    Q_CHECK_PTR (recallLabel);
    statsHlay->addWidget (recallLabel);

    precisionLabel = new QLabel (this, "precisionLabel");
    Q_CHECK_PTR (precisionLabel);
    statsHlay->addWidget (precisionLabel);

    responseStatusLabel = new QLabel (this, "responseStatusLabel");
    Q_CHECK_PTR (responseStatusLabel);
    mainVlay->addWidget (responseStatusLabel);

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

    QPushButton* newQuizButton = new QPushButton ("New &Quiz", this,
                                                  "newQuizButton");
    Q_CHECK_PTR (newQuizButton);
    newQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (newQuizButton, SIGNAL (clicked()), SLOT (newQuizClicked()));
    buttonHlay->addWidget (newQuizButton);

    nextQuestionButton = new QPushButton ("&Next", this,
                                          "nextQuestionButton");
    Q_CHECK_PTR (nextQuestionButton);
    nextQuestionButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (nextQuestionButton, SIGNAL (clicked()),
             SLOT (nextQuestionClicked()));
    buttonHlay->addWidget (nextQuestionButton);

    QPushButton* checkResponseButton = new QPushButton ("&Check", this,
                                                        "checkResponseButton");
    Q_CHECK_PTR (checkResponseButton);
    checkResponseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (checkResponseButton, SIGNAL (clicked()),
             SLOT (checkResponseClicked()));
    buttonHlay->addWidget (checkResponseButton);
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
    QString statusStr = "";

    if (status == QuizEngine::Correct) {
        responseList->insertItem (response);
        statusStr = "<font color=\"blue\">Correct</font>";
    }
    else if (status == QuizEngine::Incorrect)
        statusStr = "<font color=\"red\">Incorrect</font>";
    else if (status == QuizEngine::Duplicate)
        statusStr = "<font color=\"purple\">Duplicate</font>";
    inputLine->clear();

    // Update the stats if the stats are already shown
    if (!recallLabel->text().isEmpty())
        updateStats();

    // Update the response status label
    responseStatusLabel->setText(response + " : " + statusStr);
}

//---------------------------------------------------------------------------
// newQuizClicked
//
//! Called when the New Quiz button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::newQuizClicked()
{
    //int code = newQuizDialog->exec();
    //if (code != QDialog::Accepted)
    //    return;

    engine->newQuiz ("???", QuizEngine::Pattern, false);
    updateForm (false);
}

//---------------------------------------------------------------------------
// nextQuestionClicked
//
//! Called when the New Question button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::nextQuestionClicked()
{
    if (!engine->nextQuestion())
        QMessageBox::warning (this, "Error getting next question",
                              "Error getting next question.");

    updateForm (false);
}

//---------------------------------------------------------------------------
// checkResponseClicked
//
//! Called when the Check Responses button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::checkResponseClicked()
{
    updateStats();
}

//---------------------------------------------------------------------------
// updateStats
//
//! Update the form, including setting the question label, clearing the
//! response list, and enabling and disabling form elements as
//! appropriate.
//---------------------------------------------------------------------------
void
QuizForm::updateForm (bool showStats)
{
    if (showStats)
        updateStats();
    else
        clearStats();
    questionLabel->setText (engine->getQuestion());
    responseList->clear();
    responseStatusLabel->setText ("");
    nextQuestionButton->setEnabled ((engine->numQuestions() > 0) &&
                                    !engine->onLastQuestion());
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
// clearStats
//
//! Clear the recall and precision statistics.
//---------------------------------------------------------------------------
void
QuizForm::clearStats()
{
    recallLabel->setText ("");
    precisionLabel->setText ("");
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
                           + " / " + QString::number (total)
                           + " (" + QString::number(((correct * 100.0)/ total),
                                                    'f', 1)
                           + "%)");
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
                             + " / " + QString::number (total)
                             + " ("
                             + QString::number(((correct * 100.0)/ total),
                                                 'f', 1)
                             + "%)");
}
