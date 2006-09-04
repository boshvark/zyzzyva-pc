//---------------------------------------------------------------------------
// CardboxAddDialog.cpp
//
// A dialog for adding words to the cardbox system.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include "CardboxAddDialog.h"
#include "Defs.h"
#include "ZPushButton.h"
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Add Words to Cardbox";

using namespace Defs;

//---------------------------------------------------------------------------
//  CardboxAddDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CardboxAddDialog::CardboxAddDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* quizTypeHlay = new QHBoxLayout;
    Q_CHECK_PTR (quizTypeHlay);
    mainVlay->addLayout (quizTypeHlay);

    QLabel* quizTypeLabel = new QLabel (this);
    Q_CHECK_PTR (quizTypeLabel);
    quizTypeLabel->setText ("Add words to cardbox for quiz type:");
    quizTypeHlay->addWidget (quizTypeLabel);

    quizTypeCombo = new QComboBox (this);
    Q_CHECK_PTR (quizTypeCombo);
    quizTypeCombo->addItem ("Anagrams");
    quizTypeHlay->addWidget (quizTypeCombo);

    estimateCbox = new QCheckBox ("Estimate cardbox based on past performance",
                                  this);
    Q_CHECK_PTR (estimateCbox);
    estimateCbox->setCheckState (Qt::Checked);
    mainVlay->addWidget (estimateCbox);

    questionList = new QListWidget (this);
    Q_CHECK_PTR (questionList);
    mainVlay->addWidget (questionList);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    ZPushButton* okButton = new ZPushButton ("&OK");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    ZPushButton* cancelButton = new ZPushButton ("Cancel");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    resize (700, 500);
    setWindowTitle (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  setWords
//
//! Set the contents of the word list to be the words in a space-separated
//! string of words.
//
//! @param string the word list string
//---------------------------------------------------------------------------
void
CardboxAddDialog::setWords (const QStringList& words)
{
    QStringListIterator it (words);
    while (it.hasNext()) {
        new QListWidgetItem (it.next(), questionList);
    }
}

//---------------------------------------------------------------------------
//  getQuizType
//
//! Determine the currently chosen quiz type.
//
//! @return a string representation of the quiz type
//---------------------------------------------------------------------------
QString
CardboxAddDialog::getQuizType() const
{
    return quizTypeCombo->currentText();
}

//---------------------------------------------------------------------------
//  getEstimateCardbox
//
//! Determine whether cardboxes should be estimated based on past performance.
//
//! @return true if estimates should be used, false otherwise
//---------------------------------------------------------------------------
bool
CardboxAddDialog::getEstimateCardbox() const
{
    return (estimateCbox->checkState() == Qt::Checked);
}

//---------------------------------------------------------------------------
//  getWords
//
//! Return the list of words to be added to cardboxes.
//
//! @return the list of words
//---------------------------------------------------------------------------
QStringList
CardboxAddDialog::getWords() const
{
    QStringList words;
    for (int i = 0; i < questionList->count(); ++i) {
        words.append (questionList->item (i)->text());
    }
    return words;
}
