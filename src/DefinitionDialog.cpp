//---------------------------------------------------------------------------
// DefinitionDialog.cpp
//
// The dialog for word definitions.
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

#include "DefinitionDialog.h"
#include "WordEngine.h"
#include "Defs.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>

const QString DIALOG_CAPTION_PREFIX = "Define : ";
const int     WORD_WRAP_LENGTH = 80;

using namespace Defs;

//---------------------------------------------------------------------------
// DefinitionDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
DefinitionDialog::DefinitionDialog (WordEngine* e, const QString& word,
                                    QWidget* parent, const char* name, bool
                                    modal, WFlags f)
    : QDialog (parent, name, modal, f), engine (e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QVGroupBox* groupBox = new QVGroupBox (this, "groupBox");
    Q_CHECK_PTR (groupBox);
    groupBox->setTitle (word);
    mainVlay->addWidget (groupBox);

    QLabel* label = new QLabel (groupBox, "label");
    Q_CHECK_PTR (label);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QPushButton* closeButton = new QPushButton ("&Close", this, "closeButton");
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setDefault (true);
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    setCaption (DIALOG_CAPTION_PREFIX + word);

    QString definition = engine->getDefinition (word);
    if (definition.isEmpty())
        definition = EMPTY_DEFINITION;

    label->setText (wordWrap (definition, WORD_WRAP_LENGTH));
}

//---------------------------------------------------------------------------
// ~DefinitionDialog
//
//! Destructor.
//---------------------------------------------------------------------------
DefinitionDialog::~DefinitionDialog()
{
}

//---------------------------------------------------------------------------
// wordWrap
//
//! Wrap a string so that no line is longer than a certain length.
//
//! @param str the string to wrap
//! @param wrapLength the maximum length of a line
//! @return the wrapped string
//---------------------------------------------------------------------------
QString
DefinitionDialog::wordWrap (const QString& str, int wrapLength) const
{
    int strLen = str.length();
    if (strLen <= wrapLength)
        return str;

    QChar c;
    QString wrappedStr = str;
    int lastSpace = 0;
    int lastNewline = 0;
    for (int i = 0; i < strLen; ++i) {
        c = wrappedStr.at (i);
        if (c == '\n')
            lastNewline = i;
        if (c.isSpace())
            lastSpace = i;

        if ((i - lastNewline) == wrapLength) {
            wrappedStr[lastSpace] = '\n';
            lastNewline = lastSpace;
        }
    }
    return wrappedStr;
}
