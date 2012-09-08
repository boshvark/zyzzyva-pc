//---------------------------------------------------------------------------
// LexiconSelectWidget.cpp
//
// A simple widget for selecting a lexicon from the available lexicons.
//
// Copyright 2008-2012 Boshvark Software, LLC.
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

#include "LexiconSelectWidget.h"
#include "MainSettings.h"
#include "Defs.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

const QString LABEL_TEXT = "Lexicon:";

using namespace Defs;

//---------------------------------------------------------------------------
//  LexiconSelectWidget
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
LexiconSelectWidget::LexiconSelectWidget(QWidget* parent, Qt::WFlags f)
    : QWidget(parent, f)
{
    QHBoxLayout* mainHlay = new QHBoxLayout(this);
    mainHlay->setSpacing(SPACING);
    mainHlay->setMargin(0);

    lexiconLabel = new QLabel;
    lexiconLabel->setText(LABEL_TEXT);
    mainHlay->addWidget(lexiconLabel);

    lexiconCombo = new QComboBox;
    mainHlay->addWidget(lexiconCombo);

    updateLexicons();
}

//---------------------------------------------------------------------------
//  LexiconSelectWidget
//
//! Destructor.
//---------------------------------------------------------------------------
LexiconSelectWidget::~LexiconSelectWidget()
{
}

//---------------------------------------------------------------------------
//  setFont
//
//! Set the font.
//
//! @param newFont the new font
//---------------------------------------------------------------------------
void
LexiconSelectWidget::setFont(const QFont& newFont)
{
    QWidget::setFont(newFont);
    lexiconCombo->setFont(font());
}

//---------------------------------------------------------------------------
//  setLabelVisible
//
//! Set the visibility of the label.
//
//! @param visible true if visible, false otherwise
//---------------------------------------------------------------------------
void
LexiconSelectWidget::setLabelVisible(bool visible)
{
    lexiconLabel->setVisible(visible);
}

//---------------------------------------------------------------------------
//  setCurrentLexicon
//
//! Select a lexicon.
//
//! @param lexicon the lexicon to select
//---------------------------------------------------------------------------
bool
LexiconSelectWidget::setCurrentLexicon(const QString& lexicon)
{
    int index = lexiconCombo->findText(lexicon);
    if (index < 0)
        return false;

    lexiconCombo->setCurrentIndex(index);
    return true;
}

//---------------------------------------------------------------------------
//  getCurrentLexicon
//
//! Determine the currently selected lexicon.
//
//! @return the current lexicon
//---------------------------------------------------------------------------
QString
LexiconSelectWidget::getCurrentLexicon() const
{
    return lexiconCombo->currentText();
}

//---------------------------------------------------------------------------
//  updateLexicons
//
//! Update the list of available lexicons.  If a lexicon is selected, try to
//! keep that lexicon selected.  Otherwise, select the default lexicon.
//---------------------------------------------------------------------------
void
LexiconSelectWidget::updateLexicons()
{
    QString selectedLexicon = lexiconCombo->currentText();
    lexiconCombo->clear();

    QStringList lexicons = MainSettings::getAutoImportLexicons();
    lexiconCombo->addItems(lexicons);

    int selectIndex = -1;
    if (!selectedLexicon.isEmpty())
        selectIndex = lexiconCombo->findText(selectedLexicon);
    if (selectIndex < 0) {
        QString defaultLexicon = MainSettings::getDefaultLexicon();
        selectIndex = lexiconCombo->findText(defaultLexicon);
    }

    lexiconCombo->setCurrentIndex((selectIndex < 0) ? 0 : selectIndex);
}
