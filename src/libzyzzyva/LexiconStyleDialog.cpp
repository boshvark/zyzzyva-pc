//---------------------------------------------------------------------------
// LexiconStyleDialog.cpp
//
// A dialog for selecting specialized display styles for words that belong to
// a particular combination of lexicons.
//
// Copyright 2008 Michael W Thelen <mthelen@gmail.com>.
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

#include "LexiconStyleDialog.h"
#include "LexiconStyle.h"
#include "LexiconStyleWidget.h"
#include "Defs.h"
#include <QDialogButtonBox>
#include <QLabel>

const QString DIALOG_CAPTION = "Select Lexicon Styles";
const QString INSTRUCTION_TEXT =
    "Specify the styles to be used for displaying words that belong "
    "to certain combinations of lexicons.";
const int INSERT_WIDGET_INDEX = 1;

using namespace Defs;

//---------------------------------------------------------------------------
//  LexiconStyleDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
LexiconStyleDialog::LexiconStyleDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QLabel* instructionLabel = new QLabel;
    Q_CHECK_PTR(instructionLabel);
    instructionLabel->setWordWrap(true);
    instructionLabel->setText(INSTRUCTION_TEXT);
    mainVlay->addWidget(instructionLabel);

    QDialogButtonBox* buttonBox = new QDialogButtonBox;
    Q_CHECK_PTR(buttonBox);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                  QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
    mainVlay->addWidget(buttonBox);

    setWindowTitle(DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  LexiconStyleDialog
//
//! Destructor.
//---------------------------------------------------------------------------
LexiconStyleDialog::~LexiconStyleDialog()
{
}

//---------------------------------------------------------------------------
//  getLexiconStyles
//
//! Return the list of lexicon styles.
//
//! @return a list of lexicon styles
//---------------------------------------------------------------------------
QList<LexiconStyle>
LexiconStyleDialog::getLexiconStyles() const
{
    QList<LexiconStyle> styles;
    QListIterator<LexiconStyleWidget*> it (styleWidgets);
    while (it.hasNext()) {
        LexiconStyleWidget* widget = it.next();
        styles.append(widget->getLexiconStyle());
    }
    return styles;
}

//---------------------------------------------------------------------------
//  setLexiconStyles
//
//! Set the lexicon styles to be used.
//
//! @param styles the list of styles
//---------------------------------------------------------------------------
void
LexiconStyleDialog::setLexiconStyles(const QList<LexiconStyle>& styles)
{
    qDeleteAll(styleWidgets);
    styleWidgets.clear();

    QListIterator<LexiconStyle> it (styles);
    for (int index = INSERT_WIDGET_INDEX; it.hasNext(); ++index) {
        const LexiconStyle& style = it.next();
        LexiconStyleWidget* widget = new LexiconStyleWidget;
        Q_CHECK_PTR(widget);
        widget->setLexiconStyle(style);
        styleWidgets.append(widget);
        mainVlay->insertWidget(index, widget);
    }
}
