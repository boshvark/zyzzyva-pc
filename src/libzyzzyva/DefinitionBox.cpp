//---------------------------------------------------------------------------
// DefinitionBox.cpp
//
// A form for looking up and displaying word definitions.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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
#include "DefinitionTextEdit.h"
#include "Defs.h"
#include <QScrollArea>
#include <QVBoxLayout>

using namespace Defs;

//---------------------------------------------------------------------------
//  DefinitionBox
//
//! Constructor.
//
//! @param parent the parent widget
//---------------------------------------------------------------------------
DefinitionBox::DefinitionBox(QWidget* parent)
    : QGroupBox(parent)
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
//---------------------------------------------------------------------------
DefinitionBox::DefinitionBox(const QString& title, QWidget* parent)
    : QGroupBox(title, parent)
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
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    definitionEdit = new DefinitionTextEdit;
    Q_CHECK_PTR(definitionEdit);
    definitionEdit->setWordWrapMode(QTextOption::WordWrap);
    mainVlay->addWidget(definitionEdit);
}

//---------------------------------------------------------------------------
//  setText
//
//! Set the label text to a string.
//
//! @param text the text string
//---------------------------------------------------------------------------
void
DefinitionBox::setText(const QString& text)
{
    QString richText (text);
    richText.replace("\n", "<br>");
    definitionEdit->setText(richText);
}
