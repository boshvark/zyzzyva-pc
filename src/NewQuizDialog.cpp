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
#include "WordValidator.h"
#include "Defs.h"

#include <qbuttongroup.h>
#include <qlayout.h>

const QString DIALOG_CAPTION = "New Quiz";

using namespace Defs;

//---------------------------------------------------------------------------
// NewQuizDialog
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

    QHBoxLayout* mainHlay = new QHBoxLayout (SPACING, "mainHlay");
    Q_CHECK_PTR (mainHlay);
    mainVlay->addLayout (mainHlay);

    QVBoxLayout* optionVlay = new QVBoxLayout (SPACING, "optionVlay");
    Q_CHECK_PTR (optionVlay);
    mainHlay->addLayout (optionVlay);

    QButtonGroup* optionGroup = new QButtonGroup (3, QButtonGroup::Vertical,
                                                  "Quiz Type", this);
    Q_CHECK_PTR (optionGroup);
    optionGroup->setExclusive (true);
    optionVlay->addWidget (optionGroup);

    patternButton = new QRadioButton ("Pattern", optionGroup,
                                      "patternButton");
    Q_CHECK_PTR (patternButton);
    patternButton->setChecked (true);
    anagramButton = new QRadioButton ("Anagram", optionGroup,
                                      "anagramButton");
    Q_CHECK_PTR (anagramButton);
    subanagramButton = new QRadioButton ("Subanagram", optionGroup,
                                         "subanagramButton");
    Q_CHECK_PTR (subanagramButton);

    QVBoxLayout* inputVlay = new QVBoxLayout (SPACING, "inputVlay");
    Q_CHECK_PTR (inputVlay);
    mainHlay->addLayout (inputVlay);

    alphagramCbox = new QCheckBox ("Use alphagrams as questions", this,
                                   "alphagramCbox");
    Q_CHECK_PTR (alphagramCbox);
    connect (alphagramCbox, SIGNAL (toggled (bool)),
             SLOT (alphagramsToggled (bool)));
    inputVlay->addWidget (alphagramCbox);

    randomCbox = new QCheckBox ("Randomize order", this, "randomCbox");
    Q_CHECK_PTR (randomCbox);
    randomCbox->setEnabled (false);
    inputVlay->addWidget (randomCbox);

    inputLine = new QLineEdit (this, "inputLine");
    Q_CHECK_PTR (inputLine);
    connect (inputLine, SIGNAL (textChanged (const QString&)),
             SLOT (inputChanged (const QString&)));
    inputVlay->addWidget (inputLine);

    WordValidator* validator = new WordValidator (inputLine);
    Q_CHECK_PTR (validator);
    validator->setOptions (WordValidator::AllowQuestionMarks);
    inputLine->setValidator (validator);

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

    setCaption (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
// getQuizType
//
//! Get the quiz type.
//
//! @return the quiz type
//---------------------------------------------------------------------------
MatchType
NewQuizDialog::getQuizType() const
{
    if (patternButton->isChecked())
        return Pattern;
    else if (anagramButton->isChecked())
        return Anagram;
    else if (subanagramButton->isChecked())
        return Subanagram;
    return UnknownMatchType;
}

//---------------------------------------------------------------------------
// getQuizAlphagrams
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
// getQuizRandomOrder
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
// getQuizString
//
//! Get the quiz string (pattern, letters, etc).
//
//! @return the quiz string
//---------------------------------------------------------------------------
QString
NewQuizDialog::getQuizString() const
{
    return inputLine->text();
}

//---------------------------------------------------------------------------
// alphagramsToggled
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
// inputChanged
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
