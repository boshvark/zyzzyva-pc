//---------------------------------------------------------------------------
// WordEntryDialog.cpp
//
// A dialog for getting a word from the user.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

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
WordEntryDialog::WordEntryDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f),
    wordValidator (new WordValidator (this))
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING);
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lineHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (lineHlay);
    mainVlay->addLayout (lineHlay);

    QLabel* label = new QLabel ("Word:");
    Q_CHECK_PTR (label);
    lineHlay->addWidget (label);

    wordLine = new QLineEdit;
    Q_CHECK_PTR (wordLine);
    wordLine->setValidator (wordValidator);
    lineHlay->addWidget (wordLine);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QPushButton* okButton = new QPushButton ("OK");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel");
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
