//---------------------------------------------------------------------------
// AboutDialog.cpp
//
// The about dialog.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#include "AboutDialog.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "About Zyzzyva";

using namespace Defs;

//---------------------------------------------------------------------------
//  AboutDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
AboutDialog::AboutDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    QLabel* label = new QLabel;
    Q_CHECK_PTR (label);
    label->setPalette (QPalette (QColor (255, 255, 255)));
    label->setFrameShape (QLabel::StyledPanel);
    label->setFrameShadow (QLabel::Sunken);
    label->setLineWidth (2);
    label->setMargin (2);
    label->setAlignment (Qt::AlignHCenter);
    label->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    label->setText (Auxil::getAboutString()); 
    mainVlay->addWidget (label);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    ZPushButton* closeButton = new ZPushButton ("&Close");
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setDefault (true);
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    setWindowTitle (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  ~AboutDialog
//
//! Destructor.
//---------------------------------------------------------------------------
AboutDialog::~AboutDialog()
{
}
