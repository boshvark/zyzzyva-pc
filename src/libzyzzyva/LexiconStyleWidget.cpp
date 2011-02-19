//---------------------------------------------------------------------------
// LexiconStyleWidget.cpp
//
// A widget for specifying a lexicon style.
//
// Copyright 2008-2011 Michael W Thelen <mthelen@gmail.com>.
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

#include "LexiconStyleWidget.h"
#include "LexiconStyle.h"
#include "MainSettings.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegExpValidator>

using namespace Defs;

const QString CONJ_AND = "and";
const QString CONJ_AND_NOT = "and not";
const int MAX_SYMBOL_LENGTH = 3;

//---------------------------------------------------------------------------
//  LexiconStyleWidget
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
LexiconStyleWidget::LexiconStyleWidget(QWidget* parent, Qt::WFlags f)
    : QWidget(parent, f)
{
    QHBoxLayout* mainHlay = new QHBoxLayout(this);
    Q_CHECK_PTR(mainHlay);
    mainHlay->setSpacing(SPACING);

    QLabel* lexiconLabel = new QLabel;
    Q_CHECK_PTR(lexiconLabel);
    lexiconLabel->setText("Lexicon:");
    mainHlay->addWidget(lexiconLabel);

    QStringList validLexicons;
    validLexicons.append(LEXICON_CD);
    validLexicons.append(LEXICON_CSW);
    validLexicons.append(LEXICON_CUSTOM);
    validLexicons.append(LEXICON_ODS4);
    validLexicons.append(LEXICON_ODS5);
    validLexicons.append(LEXICON_SWL);
    validLexicons.append(LEXICON_ZINGA);
    validLexicons.append(LEXICON_OSPD4);
    validLexicons.append(LEXICON_OSWI);
    validLexicons.append(LEXICON_OWL);
    validLexicons.append(LEXICON_OWL2);
    validLexicons.append(LEXICON_WWF);
    validLexicons.append(LEXICON_VOLOST);
    qSort(validLexicons.begin(), validLexicons.end(),
          Auxil::localeAwareLessThanQString);

    QString defaultLexicon = MainSettings::getDefaultLexicon();
    int defaultIndex = validLexicons.indexOf(defaultLexicon);
    if (defaultIndex < 0)
        defaultIndex = validLexicons.indexOf(LEXICON_OWL2);

    lexiconCombo = new QComboBox;
    Q_CHECK_PTR(lexiconCombo);
    lexiconCombo->addItems(validLexicons);
    lexiconCombo->setCurrentIndex(defaultIndex);
    mainHlay->addWidget(lexiconCombo);

    conjunctionCombo = new QComboBox;
    Q_CHECK_PTR(conjunctionCombo);
    conjunctionCombo->addItem(CONJ_AND);
    conjunctionCombo->addItem(CONJ_AND_NOT);
    conjunctionCombo->setCurrentIndex(conjunctionCombo->findText(CONJ_AND_NOT));
    mainHlay->addWidget(conjunctionCombo);

    compareLexiconCombo = new QComboBox;
    Q_CHECK_PTR(compareLexiconCombo);
    compareLexiconCombo->addItems(validLexicons);
    compareLexiconCombo->setCurrentIndex(defaultIndex);
    mainHlay->addWidget(compareLexiconCombo);

    QLabel* symbolLabel = new QLabel;
    Q_CHECK_PTR(symbolLabel);
    symbolLabel->setText("symbol:");
    mainHlay->addWidget(symbolLabel);

    symbolLine = new QLineEdit;
    Q_CHECK_PTR(symbolLine);
    symbolLine->setMaxLength(MAX_SYMBOL_LENGTH);
    symbolLine->setValidator(
        new QRegExpValidator(QRegExp("^[^\\w\\s]+$"), this));
    mainHlay->addWidget(symbolLine);

    QPushButton* deleteButton = new QPushButton;
    Q_CHECK_PTR(deleteButton);
    deleteButton->setIcon(QIcon(":/minus-icon"));
    connect(deleteButton, SIGNAL(clicked()), SIGNAL(deleteClicked()));
    mainHlay->addWidget(deleteButton);
}

//---------------------------------------------------------------------------
//  setLexiconStyle
//
//! Set the lexicon style.
//
//! @param style the lexicon style
//---------------------------------------------------------------------------
bool
LexiconStyleWidget::setLexiconStyle(const LexiconStyle& style)
{
    if (!style.isValid())
        return false;

    lexiconCombo->setCurrentIndex(lexiconCombo->findText(style.lexicon));
    QString conjStr = style.inCompareLexicon ? CONJ_AND : CONJ_AND_NOT;
    compareLexiconCombo->setCurrentIndex(
        compareLexiconCombo->findText(style.compareLexicon));
    conjunctionCombo->setCurrentIndex(conjunctionCombo->findText(conjStr));
    symbolLine->setText(style.symbol);
    return true;
}

//---------------------------------------------------------------------------
//  getLexiconStyle
//
//! Get the lexicon style.
//
//! @return the lexicon style
//---------------------------------------------------------------------------
LexiconStyle
LexiconStyleWidget::getLexiconStyle() const
{
    LexiconStyle style;
    style.lexicon = lexiconCombo->currentText();
    style.compareLexicon = compareLexiconCombo->currentText();
    style.inCompareLexicon = (conjunctionCombo->currentText() == CONJ_AND);
    style.symbol = symbolLine->text();
    return style;
}
