//---------------------------------------------------------------------------
// CardboxRemoveDialog.cpp
//
// A dialog for removing words from the cardbox system.
//
// Copyright 2006-2011 Michael W Thelen <mthelen@gmail.com>.
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

#include "CardboxRemoveDialog.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Remove Words from Cardbox";

using namespace Defs;

//---------------------------------------------------------------------------
//  CardboxRemoveDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CardboxRemoveDialog::CardboxRemoveDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QHBoxLayout* quizTypeHlay = new QHBoxLayout;
    mainVlay->addLayout(quizTypeHlay);

    QLabel* quizTypeLabel = new QLabel(this);
    quizTypeLabel->setText("Remove words from cardbox for quiz type:");
    quizTypeHlay->addWidget(quizTypeLabel);

    quizTypeCombo = new QComboBox(this);
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizAnagrams));
    quizTypeCombo->addItem(
        Auxil::quizTypeToString(QuizSpec::QuizAnagramsWithHooks));
    quizTypeCombo->addItem(Auxil::quizTypeToString(QuizSpec::QuizHooks));
    quizTypeHlay->addWidget(quizTypeCombo);

    questionList = new QListWidget(this);
    mainVlay->addWidget(questionList);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    mainVlay->addLayout(buttonHlay);

    ZPushButton* okButton = new ZPushButton("&OK");
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    setWindowTitle(DIALOG_CAPTION);
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
CardboxRemoveDialog::setWords(const QStringList& words)
{
    QStringListIterator it (words);
    while (it.hasNext()) {
        new QListWidgetItem(it.next(), questionList);
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
CardboxRemoveDialog::getQuizType() const
{
    return quizTypeCombo->currentText();
}

//---------------------------------------------------------------------------
//  getWords
//
//! Return the list of words to be added to cardboxes.
//
//! @return the list of words
//---------------------------------------------------------------------------
QStringList
CardboxRemoveDialog::getWords() const
{
    QStringList words;
    for (int i = 0; i < questionList->count(); ++i) {
        words.append(questionList->item(i)->text());
    }
    return words;
}
