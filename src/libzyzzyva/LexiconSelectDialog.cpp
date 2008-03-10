//---------------------------------------------------------------------------
// LexiconSelectDialog.cpp
//
// A dialog for selecting the lexicons to be loaded and used.
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

#include "LexiconSelectDialog.h"
#include "Defs.h"
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QSet>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Select Lexicons";
const QString INSTRUCTION_TEXT = "Select the lexicons to be loaded each time "
    "the program is started, and the default lexicon to be used.";

using namespace Defs;

//---------------------------------------------------------------------------
//  LexiconSelectDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
LexiconSelectDialog::LexiconSelectDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);

    QLabel* instructionLabel = new QLabel;
    Q_CHECK_PTR(instructionLabel);
    instructionLabel->setWordWrap(true);
    instructionLabel->setText(INSTRUCTION_TEXT);
    mainVlay->addWidget(instructionLabel);

    QGridLayout* mainGlay = new QGridLayout;
    Q_CHECK_PTR(mainGlay);
    mainVlay->addLayout(mainGlay);

    QLabel* defaultLabel = new QLabel;
    Q_CHECK_PTR(defaultLabel);
    defaultLabel->setText("Default");
    mainGlay->addWidget(defaultLabel, 0, 0, Qt::AlignHCenter);

    QLabel* loadLabel = new QLabel;
    Q_CHECK_PTR(loadLabel);
    loadLabel->setText("Load");
    mainGlay->addWidget(loadLabel, 0, 1, Qt::AlignHCenter);

    QStringList validLexicons;
    validLexicons.append("OWL2+LWL");
    validLexicons.append("OSPD4+LWL");
    validLexicons.append("CSW");
    validLexicons.append("Volost");
    validLexicons.append("ODS");
    validLexicons.append("OWL+LWL");
    validLexicons.append("OSWI");

    QStringListIterator it (validLexicons);
    for (int row = 1; it.hasNext(); ++row) {
        const QString& lexicon = it.next();

        QRadioButton* radioButton = new QRadioButton;
        Q_CHECK_PTR(radioButton);
        if (row == 1)
            radioButton->setChecked(true);
        connect(radioButton, SIGNAL(clicked(bool)),
                SLOT(defaultLexiconChanged()));
        mainGlay->addWidget(radioButton, row, 0, Qt::AlignHCenter);
        lexiconRadioButtons.insert(lexicon, radioButton);

        QCheckBox* checkBox = new QCheckBox;
        Q_CHECK_PTR(checkBox);
        if (row == 1)
            checkBox->setCheckState(Qt::Checked);
        mainGlay->addWidget(checkBox, row, 1, Qt::AlignHCenter);
        lexiconCheckBoxes.insert(lexicon, checkBox);

        QLabel* label = new QLabel;
        Q_CHECK_PTR(label);
        label->setText(lexicon);
        mainGlay->addWidget(label, row, 2);
    }

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
//  LexiconSelectDialog
//
//! Destructor.
//---------------------------------------------------------------------------
LexiconSelectDialog::~LexiconSelectDialog()
{
}

//---------------------------------------------------------------------------
//  getImportLexicons
//
//! Determine which lexicons are to be imported.
//
//! @return a list of lexicons
//---------------------------------------------------------------------------
QStringList
LexiconSelectDialog::getImportLexicons() const
{
    QStringList lexicons;
    QMapIterator<QString, QCheckBox*> it (lexiconCheckBoxes);
    while (it.hasNext()) {
        it.next();
        const QString& lexicon = it.key();
        QCheckBox* checkBox = it.value();
        if (checkBox->checkState() == Qt::Checked)
            lexicons.append(lexicon);
    }
    return lexicons;
}

//---------------------------------------------------------------------------
//  getDefaultLexicon
//
//! Determine the default lexicon.
//
//! @return the default lexicon name
//---------------------------------------------------------------------------
QString
LexiconSelectDialog::getDefaultLexicon() const
{
    QMapIterator<QString, QRadioButton*> it (lexiconRadioButtons);
    while (it.hasNext()) {
        it.next();
        const QString& lexicon = it.key();
        QRadioButton* radioButton = it.value();
        if (radioButton->isChecked())
            return lexicon;
    }
    return QString();
}

//---------------------------------------------------------------------------
//  setImportLexicons
//
//! Set the lexicons that are to be imported.
//
//! @param lexicons the list of lexicons
//---------------------------------------------------------------------------
void
LexiconSelectDialog::setImportLexicons(const QStringList& lexicons)
{
    QSet<QString> lexiconSet = lexicons.toSet();
    QMapIterator<QString, QCheckBox*> it (lexiconCheckBoxes);
    while (it.hasNext()) {
        it.next();
        const QString& lexicon = it.key();
        QCheckBox* checkBox = it.value();
        checkBox->setCheckState(lexiconSet.contains(lexicon) ? Qt::Checked
                                : Qt::Unchecked);
    }
}

//---------------------------------------------------------------------------
//  setDefaultLexicon
//
//! Set the default lexicon.
//
//! @param lexicon the default lexicon name
//---------------------------------------------------------------------------
void
LexiconSelectDialog::setDefaultLexicon(const QString& lexicon)
{
    QRadioButton* radioButton = lexiconRadioButtons.value(lexicon);
    if (!radioButton)
        return;
    radioButton->setChecked(true);

    QCheckBox* checkBox = lexiconCheckBoxes.value(lexicon);
    if (!checkBox)
        return;
    checkBox->setCheckState(Qt::Checked);
    checkBox->setEnabled(false);
}

//---------------------------------------------------------------------------
//  defaultLexiconChanged
//
//! Called when the default lexicon is changed.  Select the corresponding
//! check box and disable it so it cannot be unchecked.  Also enable all other
//! check boxes.
//---------------------------------------------------------------------------
void
LexiconSelectDialog::defaultLexiconChanged()
{
    QString defaultLexicon = getDefaultLexicon();
    QMapIterator<QString, QCheckBox*> it (lexiconCheckBoxes);
    while (it.hasNext()) {
        it.next();
        const QString& lexicon = it.key();
        QCheckBox* checkBox = it.value();
        if (lexicon == defaultLexicon) {
            checkBox->setCheckState(Qt::Checked);
            checkBox->setEnabled(false);
        }
        else
            checkBox->setEnabled(true);
    }
}
