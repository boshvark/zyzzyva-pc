//---------------------------------------------------------------------------
// WordListSaveDialog.cpp
//
// The dialog for editing a list of words.
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

#include "WordListSaveDialog.h"
#include "ZPushButton.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Save Word List";

using namespace Defs;

//---------------------------------------------------------------------------
//  WordListSaveDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
WordListSaveDialog::WordListSaveDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* listHlay = new QHBoxLayout;
    listHlay->setSpacing (SPACING);
    Q_CHECK_PTR (listHlay);
    mainVlay->addLayout (listHlay);

    fullAttrList = new QListWidget (this);
    Q_CHECK_PTR (fullAttrList);
    listHlay->addWidget (fullAttrList);

    selectedAttrList = new QListWidget (this);
    Q_CHECK_PTR (selectedAttrList);
    listHlay->addWidget (selectedAttrList);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

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

    setWindowTitle (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  ~WordListSaveDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordListSaveDialog::~WordListSaveDialog()
{
}
