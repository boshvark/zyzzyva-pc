//---------------------------------------------------------------------------
// CardboxRescheduleDialog.cpp
//
// A dialog for rescheduling words within the cardbox system.
//
// Copyright 2006, 2007 Michael W Thelen <mthelen@gmail.com>.
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
#include "SearchSpecForm.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

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
    Q_CHECK_PTR(mainVlay);

    QHBoxLayout* quizTypeHlay = new QHBoxLayout;
    Q_CHECK_PTR(quizTypeHlay);
    mainVlay->addLayout(quizTypeHlay);

    QLabel* quizTypeLabel = new QLabel;
    Q_CHECK_PTR(quizTypeLabel);
    quizTypeLabel->setText("Reschedule words for quiz type:");
    quizTypeHlay->addWidget(quizTypeLabel);

    quizTypeCombo = new QComboBox;
    Q_CHECK_PTR(quizTypeCombo);
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizAnagrams));
    quizTypeCombo->addItem(
        Auxil::quizTypeToString(QuizSpec::QuizAnagramsWithHooks));
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizHooks));
    quizTypeHlay->addWidget(quizTypeCombo);

    QButtonGroup* methodGroup = new QButtonGroup(this);
    Q_CHECK_PTR(methodGroup);

    QHBoxLayout* backlogHlay = new QHBoxLayout;
    Q_CHECK_PTR(backlogHlay);
    mainVlay->addLayout(backlogHlay);

    shiftQuestionsButton = new QRadioButton;
    Q_CHECK_PTR(shiftQuestionsButton);
    shiftQuestionsButton->setText("Shift words to resize backlog: ");
    connect(shiftQuestionsButton, SIGNAL(toggled(bool)),
            SLOT(shiftQuestionsButtonToggled(bool)));
    methodGroup->addButton(shiftQuestionsButton);
    backlogHlay->addWidget(shiftQuestionsButton);

    backlogSbox = new QSpinBox;
    Q_CHECK_PTR(backlogSbox);
    backlogSbox->setMinimum(1);
    backlogSbox->setMaximum(999999);
    backlogHlay->addWidget(backlogSbox);

    rescheduleQuestionsButton = new QRadioButton;
    Q_CHECK_PTR(rescheduleQuestionsButton);
    rescheduleQuestionsButton->setText("Reschedule words according to cardbox");
    methodGroup->addButton(rescheduleQuestionsButton);
    mainVlay->addWidget(rescheduleQuestionsButton);

    QFrame* hline = new QFrame;
    Q_CHECK_PTR(hline);
    hline->setFrameShape(QFrame::HLine);
    mainVlay->addWidget(hline);

    QButtonGroup* selectGroup = new QButtonGroup(this);
    Q_CHECK_PTR(selectGroup);

    selectAllButton = new QRadioButton;
    Q_CHECK_PTR(selectAllButton);
    selectAllButton->setText("Reschedule all words");
    selectGroup->addButton(selectAllButton);
    mainVlay->addWidget(selectAllButton);

    selectSearchButton = new QRadioButton;
    Q_CHECK_PTR(selectSearchButton);
    connect(selectSearchButton, SIGNAL(toggled(bool)),
            SLOT(useSearchButtonToggled(bool)));
    selectSearchButton->setText("Reschedule words matching search "
                                "specification");
    selectGroup->addButton(selectSearchButton);
    mainVlay->addWidget(selectSearchButton);

    searchSpecGbox = new QGroupBox("Search Specification");
    Q_CHECK_PTR(searchSpecGbox);
    mainVlay->addWidget(searchSpecGbox);

    QHBoxLayout* specHlay = new QHBoxLayout(searchSpecGbox);
    Q_CHECK_PTR(specHlay);

    searchSpecForm = new SearchSpecForm;
    Q_CHECK_PTR(searchSpecForm);
    specHlay->addWidget(searchSpecForm);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    Q_CHECK_PTR(buttonHlay);
    mainVlay->addLayout(buttonHlay);

    ZPushButton* okButton = new ZPushButton("&OK");
    Q_CHECK_PTR(okButton);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    Q_CHECK_PTR(cancelButton);
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    setWindowTitle(DIALOG_CAPTION);
    shiftQuestionsButton->setChecked(true);
    selectAllButton->setChecked(true);
    searchSpecGbox->setEnabled(false);
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
//  getShiftQuestions
//
//! Determine whether questions are to be shifted, as opposed to completely
//! rescheduled.
//
//! @return true if questions should be shifted, false otherwise
//---------------------------------------------------------------------------
bool
CardboxRescheduleDialog::getShiftQuestions() const
{
    return shiftQuestionsButton->isChecked();
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
