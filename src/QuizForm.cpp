//---------------------------------------------------------------------------
// QuizForm.cpp
//
// A form for quizzing the user.
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

#include "QuizForm.h"
#include "AnalyzeQuizDialog.h"
#include "DefinitionDialog.h"
#include "NewQuizDialog.h"
#include "QuizEngine.h"
#include "WordEngine.h"
#include "WordPopupMenu.h"
#include "WordValidator.h"
#include "ZListView.h"
#include "ZListViewItem.h"
#include "Defs.h"
#include <qlayout.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qmessagebox.h>

using namespace Defs;

//---------------------------------------------------------------------------
// QuizForm
//
//! Constructor.
//
//! @param qe the quiz engine
//! @param we the word engine
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
QuizForm::QuizForm (QuizEngine* qe, WordEngine* we, QWidget* parent, const
                    char* name, WFlags f)
    : QFrame (parent, name, f), quizEngine (qe), wordEngine (we),
    newQuizDialog (new NewQuizDialog (this, "newQuizDialog", true)),
    analyzeDialog (new AnalyzeQuizDialog (qe, this, "analyzeDialog", false))
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this, MARGIN, SPACING,
                                             "mainHlay");
    Q_CHECK_PTR (mainHlay);

    QVBoxLayout* mainVlay = new QVBoxLayout (SPACING, "mainVlay");
    Q_CHECK_PTR (mainVlay);
    mainHlay->addLayout (mainVlay);

    questionNumLabel = new QLabel (this, "questionNumLabel");
    Q_CHECK_PTR (questionNumLabel);
    mainVlay->addWidget (questionNumLabel);

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

    responseList = new ZListView (this, "responseList");
    Q_CHECK_PTR (responseList);
    responseList->setResizeMode (QListView::LastColumn);
    responseList->addColumn ("Responses");
    responseList->header()->hide();
    connect (responseList, SIGNAL (contextMenuRequested (QListViewItem*, const
                                                         QPoint&, int)),
             SLOT (menuRequested (QListViewItem*, const QPoint&, int)));
    connect (responseList, SIGNAL (returnPressed (QListViewItem*)),
             SLOT (returnPressed (QListViewItem*)));
    mainVlay->addWidget (responseList);

    // Question status
    QHBoxLayout* statusHlay = new QHBoxLayout (SPACING, "questionStatusHlay");
    Q_CHECK_PTR (statusHlay);
    mainVlay->addLayout (statusHlay);

    responseStatusLabel = new QLabel (this, "responseStatusLabel");
    Q_CHECK_PTR (responseStatusLabel);
    statusHlay->addWidget (responseStatusLabel);

    questionStatusLabel = new QLabel (this, "questionStatusLabel");
    Q_CHECK_PTR (questionStatusLabel);
    statusHlay->addWidget (questionStatusLabel);

    // Input line
    inputLine = new QLineEdit (this, "inputLine");
    Q_CHECK_PTR (inputLine);
    WordValidator* validator = new WordValidator (inputLine);
    Q_CHECK_PTR (validator);
    inputLine->setValidator (validator);
    connect (inputLine, SIGNAL (returnPressed()), SLOT (responseEntered()));
    mainVlay->addWidget (inputLine);

    // Buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    QPushButton* newQuizButton = new QPushButton ("New &Quiz...", this,
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
    nextQuestionButton->setEnabled (false);
    buttonHlay->addWidget (nextQuestionButton);

    checkResponseButton = new QPushButton ("&Check", this,
                                           "checkResponseButton");
    Q_CHECK_PTR (checkResponseButton);
    checkResponseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (checkResponseButton, SIGNAL (clicked()),
             SLOT (checkResponseClicked()));
    checkResponseButton->setEnabled (false);
    buttonHlay->addWidget (checkResponseButton);

    analyzeButton = new QPushButton ("&Analyze...", this, "analyzeButton");
    Q_CHECK_PTR (analyzeButton);
    analyzeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (analyzeButton, SIGNAL (clicked()), SLOT (analyzeClicked()));
    buttonHlay->addWidget (analyzeButton);
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
    if (response.isEmpty() && checkResponseButton->isEnabled()) {
        checkResponseClicked();
        return;
    }

    QuizEngine::ResponseStatus status = quizEngine->respond (response);
    QString statusStr = "";

    if (status == QuizEngine::Correct) {
        ZListViewItem* item = new ZListViewItem (responseList, response);
        responseList->setSelected (item, true);
        responseList->ensureItemVisible (item);
        item->setTextColor (Qt::blue);
        statusStr = "<font color=\"blue\">Correct</font>";
        analyzeDialog->updateStats();
    }
    else if (status == QuizEngine::Incorrect) {
        statusStr = "<font color=\"red\">Incorrect</font>";
        analyzeDialog->addIncorrect (response);
        analyzeDialog->updateStats();
    }
    else if (status == QuizEngine::Duplicate) {
        statusStr = "<font color=\"purple\">Duplicate</font>";
    }
    inputLine->clear();

    // Update the stats if the stats are already shown
    if (!questionStatusLabel->text().isEmpty())
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
    int code = newQuizDialog->exec();
    if (code != QDialog::Accepted)
        return;

    SearchSpec spec = newQuizDialog->getSearchSpec();
    bool alphagrams = newQuizDialog->getQuizAlphagrams();
    bool random = newQuizDialog->getQuizRandomOrder();
    quizEngine->newQuiz (spec, alphagrams, random);
    updateForm (false);
    analyzeDialog->newQuiz (spec);
}

//---------------------------------------------------------------------------
// nextQuestionClicked
//
//! Called when the New Question button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::nextQuestionClicked()
{
    if (!quizEngine->nextQuestion())
        QMessageBox::warning (this, "Error getting next question",
                              "Error getting next question.");

    updateForm (false);
    analyzeDialog->updateStats();
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
    inputLine->setEnabled (false);
    checkResponseButton->setEnabled (false);
    analyzeDialog->updateStats();

    QStringList unanswered = quizEngine->getMissed();
    QStringList::iterator it;
    for (it = unanswered.begin(); it != unanswered.end(); ++it) {
        ZListViewItem* item = new ZListViewItem (responseList, *it);
        item->setTextColor (Qt::magenta);
        analyzeDialog->addMissed (*it);
    }

    if (nextQuestionButton->isEnabled())
        nextQuestionButton->setFocus();
    else
        analyzeButton->setFocus();
}

//---------------------------------------------------------------------------
// analyzeClicked
//
//! Called when the Analyze button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::analyzeClicked()
{
    analyzeDialog->show();
}

//---------------------------------------------------------------------------
// returnPressed
//
//! Called when return is pressed on an item in the response list.  Displays
//! the selected word's definition.
//
//! @param item the selected listview item
//---------------------------------------------------------------------------
void
QuizForm::returnPressed (QListViewItem* item)
{
    if (!item)
        return;
    displayDefinition (item->text (0));
}

//---------------------------------------------------------------------------
// menuRequested
//
//! Called when a right-click menu is requested.
//! @param item the selected listview item
//! @param point the point at which the menu was requested
//---------------------------------------------------------------------------
void
QuizForm::menuRequested (QListViewItem* item, const QPoint& point, int)
{
    if (!item)
        return;

    WordPopupMenu* menu = new WordPopupMenu (this, "menu");
    int choice = menu->exec (point);
    delete menu;

    if (choice == WordPopupMenu::ShowDefinition)
        displayDefinition (item->text (0));
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
    setQuestionNum (quizEngine->getQuestionIndex() + 1,
                    quizEngine->numQuestions());
    questionLabel->setText (quizEngine->getQuestion());
    responseList->clear();
    responseStatusLabel->setText ("");
    inputLine->setEnabled (true);
    nextQuestionButton->setEnabled ((quizEngine->numQuestions() > 0) &&
                                    !quizEngine->onLastQuestion());
    checkResponseButton->setEnabled (true);

    inputLine->setFocus();
}

//---------------------------------------------------------------------------
// updateStats
//
//! Update the recall and precision statistics.
//---------------------------------------------------------------------------
void
QuizForm::updateStats()
{
    setQuestionStatus (quizEngine->getQuestionCorrect(),
                       quizEngine->getQuestionTotal());
}

//---------------------------------------------------------------------------
// clearStats
//
//! Clear the recall and precision statistics.
//---------------------------------------------------------------------------
void
QuizForm::clearStats()
{
    questionStatusLabel->setText ("");
}

//---------------------------------------------------------------------------
// clearQuestionNum
//
//! Clear the question number label.
//---------------------------------------------------------------------------
void
QuizForm::clearQuestionNum()
{
    questionNumLabel->setText ("");
}

//---------------------------------------------------------------------------
// setQuestionNum
//
//! Set the current question number and the total number of questions.
//
//! @param num the current question number
//! @param total the total number of questions
//---------------------------------------------------------------------------
void
QuizForm::setQuestionNum (int num, int total)
{
    questionNumLabel->setText ("Question " + QString::number (num)
                               + " of " + QString::number (total));
}

//---------------------------------------------------------------------------
// setQuestionStatus
//
//! Set the status of the question after the user clicks the Check button.
//
//! @param correct the number of correct responses
//! @param total the total number of correct answers
//---------------------------------------------------------------------------
void
QuizForm::setQuestionStatus (int correct, int total)
{
    questionStatusLabel->setText ("Correct: " + QString::number (correct)
                          + " of " + QString::number (total));
}

//---------------------------------------------------------------------------
// displayDefinition
//
//! Displays the definition of a word.
//
//! @param word the word whose definition to display
//---------------------------------------------------------------------------
void
QuizForm::displayDefinition (const QString& word)
{
    DefinitionDialog* dialog = new DefinitionDialog (wordEngine, word, this,
                                                     "dialog", true);
    dialog->exec();
    delete dialog;
}
