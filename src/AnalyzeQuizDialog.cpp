//---------------------------------------------------------------------------
// AnalyzeQuizDialog.cpp
//
// A dialog for prompting the user for a quiz.
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

#include "AnalyzeQuizDialog.h"
#include "Defs.h"
#include "QuizEngine.h"
#include "QuizSpec.h"
#include "WordEngine.h"
#include "WordTableModel.h"
#include "WordTableView.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Analyze Quiz";
const QString MISSED_LABEL_PREFIX = "Missed : ";
const QString INCORRECT_LABEL_PREFIX = "Incorrect : ";

using namespace Defs;

//---------------------------------------------------------------------------
//  AnalyzeQuizDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
AnalyzeQuizDialog::AnalyzeQuizDialog (QuizEngine* qe, WordEngine* we, QWidget*
                                      parent, Qt::WFlags f)
    : QDialog (parent, f), quizEngine (qe), wordEngine (we)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING);
    Q_CHECK_PTR (mainVlay);

    questionLabel = new QLabel;
    Q_CHECK_PTR (questionLabel);
    mainVlay->addWidget (questionLabel);

    QHBoxLayout* mainHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (mainHlay);
    mainVlay->addLayout (mainHlay);

    QVBoxLayout* missedVlay = new QVBoxLayout (SPACING);
    Q_CHECK_PTR (missedVlay);
    mainHlay->addLayout (missedVlay, 1);

    recallLabel = new QLabel;
    Q_CHECK_PTR (recallLabel);
    missedVlay->addWidget (recallLabel);

    missedLabel = new QLabel;
    Q_CHECK_PTR (missedLabel);
    missedVlay->addWidget (missedLabel);

    missedView = new WordTableView (wordEngine);
    Q_CHECK_PTR (missedView);
    missedView->verticalHeader()->hide();
    missedVlay->addWidget (missedView);

    missedModel = new WordTableModel (wordEngine, this);
    Q_CHECK_PTR (missedModel);
    connect (missedModel, SIGNAL (wordsChanged()),
             missedView, SLOT (resizeItemsToContents()));
    missedView->setModel (missedModel);

    QVBoxLayout* incorrectVlay = new QVBoxLayout (SPACING);
    Q_CHECK_PTR (incorrectVlay);
    mainHlay->addLayout (incorrectVlay, 1);

    precisionLabel = new QLabel;
    Q_CHECK_PTR (precisionLabel);
    incorrectVlay->addWidget (precisionLabel);

    incorrectLabel = new QLabel;
    Q_CHECK_PTR (incorrectLabel);
    incorrectVlay ->addWidget (incorrectLabel);

    incorrectView = new WordTableView (wordEngine);
    Q_CHECK_PTR (incorrectView);
    incorrectView->verticalHeader()->hide();
    incorrectVlay->addWidget (incorrectView);

    incorrectModel = new WordTableModel (wordEngine, this);
    Q_CHECK_PTR (incorrectModel);
    connect (incorrectModel, SIGNAL (wordsChanged()),
             incorrectView, SLOT (resizeItemsToContents()));
    incorrectView->setModel (incorrectModel);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    closeButton = new QPushButton ("&Close", this);
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setAutoDefault (true);
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    resize (minimumSizeHint().width() * 4, 500);
    setCaption (DIALOG_CAPTION);
    updateStats();

    closeButton->setFocus();
}

//---------------------------------------------------------------------------
//  newQuiz
//
//! Reset the dialog with a new quiz.
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::newQuiz (const QuizSpec& spec)
{
    questionLabel->setText (spec.asString());
    clearMissed();
    clearIncorrect();

    // Restore incorrect and missed words from quiz progress
    QuizProgress progress = spec.getProgress();
    QMap<QString, int> missed = progress.getMissed();
    QMap<QString, int>::iterator it;
    for (it = missed.begin(); it != missed.end(); ++it)
        addMissed (it.key(), false);

    QMap<QString, int> incorrect = progress.getIncorrect();
    for (it = incorrect.begin(); it != incorrect.end(); ++it)
        addIncorrect (it.key(), false);

    updateStats();
}

//---------------------------------------------------------------------------
//  updateStats
//
//! Update the recall and precision statistics.
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::updateStats()
{
    int correct = quizEngine->getQuizCorrect();
    setRecall (correct, quizEngine->getQuizTotal());
    setPrecision (correct, correct + quizEngine->getQuizIncorrect());
    int missed = missedModel->rowCount();
    QString missedText = MISSED_LABEL_PREFIX + QString::number (missed) +
        " word";
    if (missed != 1)
        missedText += "s";
    missedLabel->setText (missedText);
    int incorrect = incorrectModel->rowCount();
    QString incorrectText = INCORRECT_LABEL_PREFIX + QString::number
        (incorrect) + " word";
    if (incorrect != 1)
        incorrectText += "s";
    incorrectLabel->setText (incorrectText);
}

//---------------------------------------------------------------------------
//  addMissed
//
//! Add a word to the Missed list.
//
//! @param word the missed word
//! @param update true if visible stats should be updated
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::addMissed (const QString& word, bool update)
{
    missedModel->addWord (word, WordTableModel::WordMissed);
    if (update)
        updateStats();
}

//---------------------------------------------------------------------------
//  addIncorrect
//
//! Add a word to the Incorrect list.
//
//! @param word the incorrect word
//! @param update true if visible stats should be updated
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::addIncorrect (const QString& word, bool update)
{
    incorrectModel->addWord (word, WordTableModel::WordIncorrect);
    if (update)
        updateStats();
}

//---------------------------------------------------------------------------
//  clearMissed
//
//! Clear the list of missed words.
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::clearMissed()
{
    missedModel->clear();
}

//---------------------------------------------------------------------------
//  clearIncorrect
//
//! Clear the list of incorrect words.
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::clearIncorrect()
{
    incorrectModel->clear();
}

//---------------------------------------------------------------------------
//  setRecall
//
//! Set the recall numbers (correct user responses divided by total correct
//! responses).
//
//! @param correct the number of correct responses
//! @param total the total number of correct answers
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::setRecall (int correct, int total)
{
    recallLabel->setText ("Recall: " + percentString (correct, total));
}

//---------------------------------------------------------------------------
//  setPrecision
//
//! Set the precision numbers (correct user responses divided by total user
//! responses).
//
//! @param correct the number of correct responses
//! @param total the total number of user responses
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::setPrecision (int correct, int total)
{
    precisionLabel->setText ("Precision: " + percentString (correct, total));
}

//---------------------------------------------------------------------------
//  percentString
//
//! Create a string to represent a percentage to be displayed.
//
//! @param numerator the numerator
//! @param denominator the denominator
//---------------------------------------------------------------------------
QString
AnalyzeQuizDialog::percentString (int numerator, int denominator) const
{
    double pct = denominator ? (numerator * 100.0) / denominator : 0;
    return QString::number (numerator) + " / " + QString::number (denominator)
        + " (" + QString::number (pct, 'f', 1) + "%)";
}
