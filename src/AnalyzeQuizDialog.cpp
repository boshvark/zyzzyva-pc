//---------------------------------------------------------------------------
// AnalyzeQuizDialog.cpp
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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
#include "ZPushButton.h"
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
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    questionLabel = new QLabel;
    Q_CHECK_PTR (questionLabel);
    questionLabel->setAlignment (Qt::AlignHCenter);
    mainVlay->addWidget (questionLabel);

    QHBoxLayout* recallHlay = new QHBoxLayout;
    Q_CHECK_PTR (recallHlay);
    recallHlay->setSpacing (SPACING);
    mainVlay->addLayout (recallHlay);

    missedLabel = new QLabel;
    Q_CHECK_PTR (missedLabel);
    recallHlay->addWidget (missedLabel);

    recallHlay->addStretch (1);

    recallLabel = new QLabel;
    Q_CHECK_PTR (recallLabel);
    recallHlay->addWidget (recallLabel);

    missedView = new WordTableView (wordEngine);
    Q_CHECK_PTR (missedView);
    missedView->verticalHeader()->hide();
    mainVlay->addWidget (missedView);

    missedModel = new WordTableModel (wordEngine, this);
    Q_CHECK_PTR (missedModel);
    connect (missedModel, SIGNAL (wordsChanged()),
             missedView, SLOT (resizeAllColumnsToContents()));
    missedView->setModel (missedModel);

    QHBoxLayout* precisionHlay = new QHBoxLayout;
    Q_CHECK_PTR (precisionHlay);
    precisionHlay->setSpacing (SPACING);
    mainVlay->addLayout (precisionHlay);

    incorrectLabel = new QLabel;
    Q_CHECK_PTR (incorrectLabel);
    precisionHlay->addWidget (incorrectLabel);

    precisionHlay->addStretch (1);

    precisionLabel = new QLabel;
    Q_CHECK_PTR (precisionLabel);
    precisionHlay->addWidget (precisionLabel);

    incorrectView = new WordTableView (wordEngine);
    Q_CHECK_PTR (incorrectView);
    incorrectView->verticalHeader()->hide();
    mainVlay->addWidget (incorrectView);

    incorrectModel = new WordTableModel (wordEngine, this);
    Q_CHECK_PTR (incorrectModel);
    connect (incorrectModel, SIGNAL (wordsChanged()),
             incorrectView, SLOT (resizeAllColumnsToContents()));
    incorrectView->setModel (incorrectModel);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    closeButton = new ZPushButton ("&Close", this);
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setAutoDefault (true);
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    resize (700, 500);
    setWindowTitle (DIALOG_CAPTION);
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

    QStringList missedList;
    for (it = missed.begin(); it != missed.end(); ++it)
        missedList.append (it.key());
    addMissed (missedList, false);

    QMap<QString, int> incorrect = progress.getIncorrect();
    QStringList incorrectList;
    for (it = incorrect.begin(); it != incorrect.end(); ++it)
        incorrectList.append (it.key());
    addIncorrect (incorrectList, false);

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
    missedModel->addWord (WordTableModel::WordItem
                          (word, WordTableModel::WordMissed), false);
    if (update)
        updateStats();
}

//---------------------------------------------------------------------------
//  addMissed
//
//! Add a list of words to the Missed list.
//
//! @param words the missed words
//! @param update true if visible stats should be updated
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::addMissed (const QStringList& words, bool update)
{
    QList<WordTableModel::WordItem> wordItems;
    QString word;
    foreach (word, words) {
        wordItems.append (WordTableModel::WordItem
                          (word, WordTableModel::WordMissed));
    }
    missedModel->addWords (wordItems);
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
    incorrectModel->addWord (WordTableModel::WordItem
                             (word, WordTableModel::WordIncorrect), false);
    if (update)
        updateStats();
}

//---------------------------------------------------------------------------
//  addIncorrect
//
//! Add a list of words to the Incorrect list.
//
//! @param words the incorrect words
//! @param update true if visible stats should be updated
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::addIncorrect (const QStringList& words, bool update)
{
    QList<WordTableModel::WordItem> wordItems;
    QString word;
    foreach (word, words) {
        wordItems.append (WordTableModel::WordItem
                          (word, WordTableModel::WordIncorrect));
    }
    incorrectModel->addWords (wordItems);
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
