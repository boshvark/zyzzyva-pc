//---------------------------------------------------------------------------
// CardboxRescheduleDialog.cpp
//
// A dialog for rescheduling words within the cardbox system.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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

#include "CardboxRescheduleDialog.h"
#include "CardboxRescheduleDaysSpinBox.h"
#include "LexiconSelectWidget.h"
#include "SearchSpecForm.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

const QString DIALOG_CAPTION = "Reschedule Words in Cardbox";

using namespace Defs;

//---------------------------------------------------------------------------
//  CardboxRescheduleDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CardboxRescheduleDialog::CardboxRescheduleDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QHBoxLayout* lexiconHlay = new QHBoxLayout;
    lexiconHlay->setMargin(0);
    mainVlay->addLayout(lexiconHlay);

    lexiconHlay->addStretch(1);

    QGridLayout* lexiconGlay = new QGridLayout;
    lexiconGlay->setMargin(0);
    lexiconGlay->setSpacing(SPACING);
    lexiconHlay->addLayout(lexiconGlay);

    QLabel* lexiconLabel = new QLabel;
    lexiconLabel->setText("Lexicon:");
    lexiconGlay->addWidget(lexiconLabel, 0, 0);

    lexiconWidget = new LexiconSelectWidget;
    lexiconWidget->setLabelVisible(false);
    lexiconGlay->addWidget(lexiconWidget, 0, 1);

    QLabel* quizTypeLabel = new QLabel;
    quizTypeLabel->setText("Quiz Type:");
    lexiconGlay->addWidget(quizTypeLabel, 1, 0);

    quizTypeCombo = new QComboBox;
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizAnagrams));
    quizTypeCombo->addItem(
        Auxil::quizTypeToString(QuizSpec::QuizAnagramsWithHooks));
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizHooks));
    lexiconGlay->addWidget(quizTypeCombo, 1, 1);

    lexiconHlay->addStretch(1);

    QGroupBox* methodGroup = new QGroupBox;
    methodGroup->setTitle("Reschedule method");
    mainVlay->addWidget(methodGroup);

    QVBoxLayout* methodVlay = new QVBoxLayout(methodGroup);

    QButtonGroup* methodButtons = new QButtonGroup(this);

    QHBoxLayout* daysHlay = new QHBoxLayout;
    methodVlay->addLayout(daysHlay);

    shiftDaysButton = new QRadioButton;
    shiftDaysButton->setText("Shift words by this many days:");
    connect(shiftDaysButton, SIGNAL(toggled(bool)),
            SLOT(shiftDaysButtonToggled(bool)));
    methodButtons->addButton(shiftDaysButton);
    daysHlay->addWidget(shiftDaysButton);

    daysSbox = new CardboxRescheduleDaysSpinBox;
    connect(daysSbox, SIGNAL(valueChanged(int)),
        SLOT(shiftDaysValueChanged(int)));
    daysHlay->addWidget(daysSbox);

    QHBoxLayout* backlogHlay = new QHBoxLayout;
    methodVlay->addLayout(backlogHlay);

    shiftQuestionsButton = new QRadioButton;
    shiftQuestionsButton->setText("Shift words so this many are ready now:");
    connect(shiftQuestionsButton, SIGNAL(toggled(bool)),
            SLOT(shiftQuestionsButtonToggled(bool)));
    methodButtons->addButton(shiftQuestionsButton);
    backlogHlay->addWidget(shiftQuestionsButton);

    backlogSbox = new QSpinBox;
    backlogSbox->setMinimum(1);
    backlogSbox->setMaximum(999999);
    backlogSbox->setEnabled(false);
    backlogHlay->addWidget(backlogSbox);

    rescheduleQuestionsButton = new QRadioButton;
    rescheduleQuestionsButton->setText("Reschedule words according to "
                                       "their cardbox");
    methodButtons->addButton(rescheduleQuestionsButton);
    methodVlay->addWidget(rescheduleQuestionsButton);

    QGroupBox* selectGroup = new QGroupBox;
    selectGroup->setTitle("Words to reschedule");
    mainVlay->addWidget(selectGroup);

    QVBoxLayout* selectVlay = new QVBoxLayout(selectGroup);

    QButtonGroup* selectButtons = new QButtonGroup(this);

    selectAllButton = new QRadioButton;
    selectAllButton->setText("Reschedule all words");
    selectButtons->addButton(selectAllButton);
    selectVlay->addWidget(selectAllButton);

    selectSearchButton = new QRadioButton;
    connect(selectSearchButton, SIGNAL(toggled(bool)),
            SLOT(useSearchButtonToggled(bool)));
    selectSearchButton->setText("Reschedule only words matching search "
                                "specification");
    selectButtons->addButton(selectSearchButton);
    selectVlay->addWidget(selectSearchButton);

    searchSpecGbox = new QGroupBox("Search Specification");
    selectVlay->addWidget(searchSpecGbox);

    QHBoxLayout* specHlay = new QHBoxLayout(searchSpecGbox);

    searchSpecForm = new SearchSpecForm;
    specHlay->addWidget(searchSpecForm);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    okButton = new ZPushButton("&OK");
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    buttonHlay->addStretch(1);

    setWindowTitle(DIALOG_CAPTION);
    shiftDaysButton->setChecked(true);
    selectAllButton->setChecked(true);
    searchSpecGbox->setEnabled(false);
}

//---------------------------------------------------------------------------
//  getLexicon
//
//! Return the lexicon chosen by the user.
//
//! @return the lexicon
//---------------------------------------------------------------------------
QString
CardboxRescheduleDialog::getLexicon() const
{
    return lexiconWidget->getCurrentLexicon();
}

//---------------------------------------------------------------------------
//  getQuizType
//
//! Return the quiz type chosen by the user.
//
//! @return the quiz type
//---------------------------------------------------------------------------
QString
CardboxRescheduleDialog::getQuizType() const
{
    return quizTypeCombo->currentText();
}

//---------------------------------------------------------------------------
//  getRescheduleType
//
//! Determine whether questions are to be shifted by a number of days.
//
//! @return true if questions should be shifted by days, false otherwise
//---------------------------------------------------------------------------
CardboxRescheduleType
CardboxRescheduleDialog::getRescheduleType() const
{
    if (shiftDaysButton->isChecked())
        return CardboxRescheduleShiftDays;
    else if (shiftQuestionsButton->isChecked())
        return CardboxRescheduleShiftBacklog;
    else if (rescheduleQuestionsButton->isChecked())
        return CardboxRescheduleByCardbox;
    else
        return CardboxRescheduleUnknown;
}

//---------------------------------------------------------------------------
//  getBacklogSize
//
//! Determine the desired backlog size.
//
//! @return the backlog size
//---------------------------------------------------------------------------
int
CardboxRescheduleDialog::getBacklogSize() const
{
    return backlogSbox->value();
}

//---------------------------------------------------------------------------
//  getNumDays
//
//! Determine the number of days to shift.
//
//! @return the number of days
//---------------------------------------------------------------------------
int
CardboxRescheduleDialog::getNumDays() const
{
    return daysSbox->value();
}

//---------------------------------------------------------------------------
//  getRescheduleAll
//
//! Determine whether all questions are to be rescheduled.
//
//! @return true if all questions should be rescheduled, false otherwise
//---------------------------------------------------------------------------
bool
CardboxRescheduleDialog::getRescheduleAll() const
{
    return selectAllButton->isChecked();
}

//---------------------------------------------------------------------------
//  getSearchSpec
//
//! Determine whether all questions are to be rescheduled
//
//! @return true if all questions should be rescheduled
//---------------------------------------------------------------------------
SearchSpec
CardboxRescheduleDialog::getSearchSpec() const
{
    return searchSpecForm->getSearchSpec();
}

//---------------------------------------------------------------------------
//  shiftDaysButtonToggled
//
//! Called when the state of the Shift By Days button is toggled. Enable or
//! disable the days spin box.
//
//! @param checked true if the button is checked
//---------------------------------------------------------------------------
void
CardboxRescheduleDialog::shiftDaysButtonToggled(bool checked)
{
    daysSbox->setEnabled(checked);
    okButton->setEnabled(!checked || (daysSbox->value() != 0));
}

//---------------------------------------------------------------------------
//  shiftDaysValueChanged
//
//! Called when the value of the Shift By Days spin box is changed.
//
//! @param value the new value
//---------------------------------------------------------------------------
void
CardboxRescheduleDialog::shiftDaysValueChanged(int value)
{
    if (shiftDaysButton->isChecked())
        okButton->setEnabled(value != 0);
}

//---------------------------------------------------------------------------
//  shiftQuestionsButtonToggled
//
//! Called when the state of the Shift Backlog button is toggled.  Enable or
//! disable the backlog size spin box.
//
//! @param checked true if the button is checked
//---------------------------------------------------------------------------
void
CardboxRescheduleDialog::shiftQuestionsButtonToggled(bool checked)
{
    backlogSbox->setEnabled(checked);
}

//---------------------------------------------------------------------------
//  useSearchButtonToggled
//
//! Called when the state of the Use Search button is toggled.  Enable or
//! disable the search specification form.
//
//! @param checked true if the button is checked
//---------------------------------------------------------------------------
void
CardboxRescheduleDialog::useSearchButtonToggled(bool checked)
{
    searchSpecGbox->setEnabled(checked);
}
