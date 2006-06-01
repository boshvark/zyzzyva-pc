//---------------------------------------------------------------------------
// AboutDialog.cpp
//
// The about dialog.
//
// Copyright 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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
#include <QTabWidget>
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

    QTabWidget* tabStack = new QTabWidget (this);
    Q_CHECK_PTR (tabStack);
    mainVlay->addWidget (tabStack);

    QLabel* aboutLabel = new QLabel;
    Q_CHECK_PTR (aboutLabel);
    aboutLabel->setPalette (QPalette (QColor (255, 255, 255)));
    aboutLabel->setFrameShape (QLabel::StyledPanel);
    aboutLabel->setFrameShadow (QLabel::Sunken);
    aboutLabel->setLineWidth (2);
    aboutLabel->setMargin (2);
    aboutLabel->setAlignment (Qt::AlignHCenter);
    aboutLabel->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    aboutLabel->setText (Auxil::getAboutString());
    tabStack->addTab (aboutLabel, "Zyzzyva");

    QLabel* thanksLabel = new QLabel;
    Q_CHECK_PTR (thanksLabel);
    thanksLabel->setPalette (QPalette (QColor (255, 255, 255)));
    thanksLabel->setFrameShape (QLabel::StyledPanel);
    thanksLabel->setFrameShadow (QLabel::Sunken);
    thanksLabel->setLineWidth (2);
    thanksLabel->setMargin (2);
    thanksLabel->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    thanksLabel->setText (Auxil::getThanksString());
    thanksLabel->setWordWrap (true);
    tabStack->addTab (thanksLabel, "Thanks");

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
