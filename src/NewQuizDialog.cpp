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

    specForm = new SearchSpecForm (this, "specForm");
    Q_CHECK_PTR (specForm);
    connect (specForm, SIGNAL (patternChanged (const QString&)),
             SLOT (inputChanged (const QString&)));
    mainVlay->addWidget (specForm);

    alphagramCbox = new QCheckBox ("Use individual alphagrams as questions",
                                   this, "alphagramCbox");
    Q_CHECK_PTR (alphagramCbox);
    connect (alphagramCbox, SIGNAL (toggled (bool)),
             SLOT (alphagramsToggled (bool)));
    mainVlay->addWidget (alphagramCbox);

    randomCbox = new QCheckBox ("Randomize order", this, "randomCbox");
    Q_CHECK_PTR (randomCbox);
    randomCbox->setEnabled (false);
    mainVlay->addWidget (randomCbox);

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
// getSearchSpec
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
