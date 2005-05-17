//---------------------------------------------------------------------------
// WordEntryDialog.cpp
//
// A dialog for getting a word from the user.
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

#include "WordEntryDialog.h"
#include "WordValidator.h"
#include "Defs.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

const QString DIALOG_CAPTION = "Enter a word";

using namespace Defs;

//---------------------------------------------------------------------------
//  WordEntryDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
WordEntryDialog::WordEntryDialog (QWidget* parent, const char* name, bool
                                  modal, WFlags f)
    : QDialog (parent, name, modal, f),
    wordValidator (new WordValidator (this, "wordValidator"))
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lineHlay = new QHBoxLayout (SPACING, "lineHlay");
    Q_CHECK_PTR (lineHlay);
    mainVlay->addLayout (lineHlay);

    QLabel* label = new QLabel ("Word:", this, "label");
    Q_CHECK_PTR (label);
    lineHlay->addWidget (label);

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    wordLine->setValidator (wordValidator);
    lineHlay->addWidget (wordLine);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QPushButton* okButton = new QPushButton ("OK", this, "okButton");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel", this,
                                                 "cancelButton");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setCaption (DIALOG_CAPTION);
    //resize (minimumSizeHint().width(), 500);
}

//---------------------------------------------------------------------------
//  ~WordEntryDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordEntryDialog::~WordEntryDialog()
{
    delete wordValidator;
}
