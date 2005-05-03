//---------------------------------------------------------------------------
// NewQuizDialog.cpp
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

#include "NewQuizDialog.h"
#include "SearchSpec.h"
#include "SearchSpecForm.h"
#include "Defs.h"

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>

const QString DIALOG_CAPTION = "New Quiz";
const QString TIMER_PER_QUESTION = "per question";
const QString TIMER_PER_RESPONSE = "per response";

using namespace Defs;

//---------------------------------------------------------------------------
//  NewQuizDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
NewQuizDialog::NewQuizDialog (QWidget* parent, const char* name,
                              bool modal, WFlags f)
    : QDialog (parent, name, modal, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    specForm = new SearchSpecForm (this, "specForm");
    Q_CHECK_PTR (specForm);
    connect (specForm, SIGNAL (patternChanged (const QString&)),
             SLOT (inputChanged (const QString&)));
    mainVlay->addWidget (specForm);

    alphagramCbox = new QCheckBox ("Use individual &alphagrams as questions",
                                   this, "alphagramCbox");
    Q_CHECK_PTR (alphagramCbox);
    connect (alphagramCbox, SIGNAL (toggled (bool)),
             SLOT (alphagramsToggled (bool)));
    mainVlay->addWidget (alphagramCbox);

    randomCbox = new QCheckBox ("&Randomize order", this, "randomCbox");
    Q_CHECK_PTR (randomCbox);
    randomCbox->setEnabled (false);
    mainVlay->addWidget (randomCbox);

    QHBoxLayout* timerHlay = new QHBoxLayout (SPACING, "timerHlay");
    Q_CHECK_PTR (timerHlay);
    mainVlay->addLayout (timerHlay);

    timerCbox = new QCheckBox ("&Timer:", this, "timerCbox");
    Q_CHECK_PTR (timerCbox);
    connect (timerCbox, SIGNAL (toggled (bool)), SLOT (timerToggled (bool)));
    timerHlay->addWidget (timerCbox);

    timerWidget = new QWidget (this, "timerWidget");
    Q_CHECK_PTR (timerWidget);
    timerWidget->setEnabled (false);
    timerHlay->addWidget (timerWidget);

    QHBoxLayout* timerWidgetHlay = new QHBoxLayout (timerWidget, 0, SPACING,
                                                    "timerWidgetHlay");
    Q_CHECK_PTR (timerWidgetHlay);

    timerSbox = new QSpinBox (1, 9999, 1, timerWidget, "timerSbox");
    Q_CHECK_PTR (timerSbox);
    timerSbox->setValue (10);
    timerWidgetHlay->addWidget (timerSbox);

    QLabel* timerLabel = new QLabel ("seconds", timerWidget, "timerLabel");
    Q_CHECK_PTR (timerLabel);
    timerWidgetHlay->addWidget (timerLabel);

    timerCombo = new QComboBox (timerWidget, "timerCombo");
    timerCombo->insertItem (TIMER_PER_QUESTION);
    timerCombo->insertItem (TIMER_PER_RESPONSE);
    timerCombo->setCurrentText (TIMER_PER_RESPONSE);
    timerWidgetHlay->addWidget (timerCombo);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    okButton = new QPushButton ("OK", this, "okButton");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    okButton->setEnabled (false);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel", this,
                                                 "cancelButton");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    resize (minimumSizeHint());
    setCaption (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  getSearchSpec
//
//! Get a SearchSpec corresponding to the state of the SearchSpecForm.
//
//! @return the SearchSpec
//---------------------------------------------------------------------------
SearchSpec
NewQuizDialog::getSearchSpec() const
{
    return specForm->getSearchSpec();
}

//---------------------------------------------------------------------------
//  getQuizAlphagrams
//
//! Get whether the quiz should use alphagrams of the individual words as
//! quiz questions.
//
//! @return true if alphagrams should be used, false otherwise
//---------------------------------------------------------------------------
bool
NewQuizDialog::getQuizAlphagrams() const
{
    return alphagramCbox->isChecked();
}

//---------------------------------------------------------------------------
//  getQuizRandomOrder
//
//! Get whether the quiz should present questions in random order.
//
//! @return true if random order, false otherwise
//---------------------------------------------------------------------------
bool
NewQuizDialog::getQuizRandomOrder() const
{
    return randomCbox->isChecked();
}

//---------------------------------------------------------------------------
//  getTimerEnabled
//
//! Get whether the timer should be enabled.
//
//! @return true if enabled, false otherwise
//---------------------------------------------------------------------------
bool
NewQuizDialog::getTimerEnabled() const
{
    return timerCbox->isChecked();
}

//---------------------------------------------------------------------------
//  getTimerDuration
//
//! Get the timer duration selected, if the timer is enabled.
//
//! @return the duration if the timer is enabled, -1 otherwise
//---------------------------------------------------------------------------
int
NewQuizDialog::getTimerDuration() const
{
    return timerCbox->isChecked() ? timerSbox->value() : -1;
}

//---------------------------------------------------------------------------
//  getTimerType
//
//! Get the timer type, if the timer is enabled.
//
//! @return the timer type if the timer is enabled, NoTimer otherwise
//---------------------------------------------------------------------------
QuizTimerType
NewQuizDialog::getTimerType() const
{
    if (!timerCbox->isChecked())
        return NoTimer;
    QString type = timerCombo->currentText();
    if (type == TIMER_PER_QUESTION)
        return PerQuestion;
    if (type == TIMER_PER_RESPONSE)
        return PerResponse;
    return NoTimer;
}

//---------------------------------------------------------------------------
//  alphagramsToggled
//
//! Called when the Alphagrams checkbox is toggled.  Disable the Random
//! checkbox unless the Alphagrams checkbox is checked.
//
//! @param on whether the checkbox is checked
//---------------------------------------------------------------------------
void
NewQuizDialog::alphagramsToggled (bool on)
{
    randomCbox->setEnabled (on);
    if (!on)
        randomCbox->setChecked (false);
}

//---------------------------------------------------------------------------
//  inputChanged
//
//! Called when the text in the input line changes.
//
//! @param text the new text
//---------------------------------------------------------------------------
void
NewQuizDialog::inputChanged (const QString& text)
{
    okButton->setEnabled (!text.isEmpty());
}

//---------------------------------------------------------------------------
//  timerToggled
//
//! Called when the Timer checkbox is toggled.  Disable the timer
//! configuration unless the Alphagrams checkbox is checked.
//
//! @param on whether the checkbox is checked
//---------------------------------------------------------------------------
void
NewQuizDialog::timerToggled (bool on)
{
    timerWidget->setEnabled (on);
}
