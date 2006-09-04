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

    helloLabel = new QLabel;
    Q_CHECK_PTR (helloLabel);
    helloLabel->setAlignment (Qt::AlignHCenter);
    helloLabel->setText ("Hello!");
    mainVlay->addWidget (helloLabel);

    resize (700, 500);
    setWindowTitle (DIALOG_CAPTION);
}
