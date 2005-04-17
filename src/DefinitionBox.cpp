//---------------------------------------------------------------------------
// DefinitionBox.cpp
//
// A form for looking up and displaying word definitions.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "DefinitionBox.h"
#include "DefinitionLabel.h"
#include "Defs.h"
#include <qlayout.h>

using namespace Defs;

//---------------------------------------------------------------------------
//  DefinitionBox
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//---------------------------------------------------------------------------
DefinitionBox::DefinitionBox (QWidget* parent, const char* name)
    : QVGroupBox (parent, name)
{
    init();
}

//---------------------------------------------------------------------------
//  DefinitionBox
//
//! Constructor.
//
//! @param title the widget title
//! @param parent the parent widget
//! @param name the name of this widget
//---------------------------------------------------------------------------
DefinitionBox::DefinitionBox (const QString& title, QWidget* parent, const
                              char* name)
    : QVGroupBox (title, parent, name)
{
    init();
}

//---------------------------------------------------------------------------
//  init
//
//! Initialize the definition box by setting up the layout elements.
//---------------------------------------------------------------------------
void
DefinitionBox::init()
{
    //QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             //"mainVlay");
    //Q_CHECK_PTR (mainVlay);

    definitionLabel = new DefinitionLabel (this, "definitionLabel");
    Q_CHECK_PTR (definitionLabel);
    //mainVlay->addWidget (definitionLabel);

    //mainVlay->addStretch (1);
}

//---------------------------------------------------------------------------
//  setText
//
//! Set the label text to a string.
//
//! @param text the text string
//---------------------------------------------------------------------------
void
DefinitionBox::setText (const QString& text)
{
    definitionLabel->setText (text);
}
