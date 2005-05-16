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
#include "WordEngine.h"
#include "WordListView.h"
#include "WordListViewItem.h"
#include <qlayout.h>

const QString DIALOG_CAPTION = "Analyze Quiz";

using namespace Defs;

//---------------------------------------------------------------------------
//  AnalyzeQuizDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
AnalyzeQuizDialog::AnalyzeQuizDialog (QuizEngine* qe, WordEngine* we, QWidget*
                                      parent, const char* name, bool modal,
                                      WFlags f)
    : QDialog (parent, name, modal, f), quizEngine (qe), wordEngine (we)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    questionLabel = new QLabel (this, "questionLabel");
    Q_CHECK_PTR (questionLabel);
    mainVlay->addWidget (questionLabel);

    QHBoxLayout* statsHlay = new QHBoxLayout (SPACING, "statsHlay");
    Q_CHECK_PTR (statsHlay);
    mainVlay->addLayout (statsHlay);

    recallLabel = new QLabel (this, "recallLabel");
    Q_CHECK_PTR (recallLabel);
    statsHlay->addWidget (recallLabel);

    precisionLabel = new QLabel (this, "precisionLabel");
    Q_CHECK_PTR (precisionLabel);
    statsHlay->addWidget (precisionLabel);

    QHBoxLayout* mainHlay = new QHBoxLayout (SPACING, "mainHlay");
    Q_CHECK_PTR (mainHlay);
    mainVlay->addLayout (mainHlay);

    QVBoxLayout* missedVlay = new QVBoxLayout (SPACING, "missedVlay");
    Q_CHECK_PTR (missedVlay);
    mainHlay->addLayout (missedVlay);

    missedList = new WordListView (wordEngine, this, "missedList");
    Q_CHECK_PTR (missedList);
    missedList->setResizeMode (QListView::LastColumn);
    missedList->setTitle ("Missed");
    missedVlay->addWidget (missedList);

    QVBoxLayout* incorrectVlay = new QVBoxLayout (SPACING, "incorrectVlay");
    Q_CHECK_PTR (incorrectVlay);
    mainHlay->addLayout (incorrectVlay);

    incorrectList = new WordListView (wordEngine, this, "incorrectList");
    Q_CHECK_PTR (incorrectList);
    incorrectList->setResizeMode (QListView::LastColumn);
    incorrectList->setTitle ("Incorrect");
    incorrectVlay->addWidget (incorrectList);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    closeButton = new QPushButton ("&Close", this, "closeButton");
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setAutoDefault (false);
    closeButton->setFocus();
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    resize (minimumSizeHint().width(), 500);
    setCaption (DIALOG_CAPTION);
    updateStats();
}

//---------------------------------------------------------------------------
//  newQuiz
//
//! Reset the dialog with a new quiz.
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::newQuiz (const SearchSpec& spec)
{
    questionLabel->setText (spec.asString());
    clearMissed();
    clearIncorrect();
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
}

//---------------------------------------------------------------------------
//  addMissed
//
//! Add a word to the Missed list.
//
//! @param word the missed word
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::addMissed (const QString& word)
{
    WordListViewItem* item = new WordListViewItem (missedList, word);
    item->setTextColor (VALID_MISSED_WORD_COLOR);
}

//---------------------------------------------------------------------------
//  addIncorrect
//
//! Add a word to the Incorrect list.
//
//! @param word the incorrect word
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::addIncorrect (const QString& word)
{
    WordListViewItem* item = new WordListViewItem (incorrectList, word);
    item->setTextColor (INVALID_WORD_COLOR);
}

//---------------------------------------------------------------------------
//  clearMissed
//
//! Clear the list of missed words.
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::clearMissed()
{
    missedList->clear();
}

//---------------------------------------------------------------------------
//  clearIncorrect
//
//! Clear the list of incorrect words.
//---------------------------------------------------------------------------
void
AnalyzeQuizDialog::clearIncorrect()
{
    incorrectList->clear();
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
