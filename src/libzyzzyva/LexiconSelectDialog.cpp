//---------------------------------------------------------------------------
// LexiconSelectDialog.cpp
//
// A dialog for selecting the lexicons to be loaded and used.
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

#include "LexiconSelectDialog.h"
#include "Auxil.h"
#include "Defs.h"
#include <QDialogButtonBox>
#include <QFileDialog>
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

    QLabel* instructionLabel = new QLabel;
    instructionLabel->setWordWrap(true);
    instructionLabel->setText(INSTRUCTION_TEXT);
    mainVlay->addWidget(instructionLabel);

    QGridLayout* mainGlay = new QGridLayout;
    mainVlay->addLayout(mainGlay);

    QLabel* defaultHeaderLabel = new QLabel;
    defaultHeaderLabel->setText("Default");
    mainGlay->addWidget(defaultHeaderLabel, 0, 0, Qt::AlignHCenter);

    QLabel* loadHeaderLabel = new QLabel;
    loadHeaderLabel->setText("Load");
    mainGlay->addWidget(loadHeaderLabel, 0, 1, Qt::AlignHCenter);

    QLabel* lexiconHeaderLabel = new QLabel;
    lexiconHeaderLabel->setText("Lexicon");
    mainGlay->addWidget(lexiconHeaderLabel, 0, 2, Qt::AlignHCenter);

    QLabel* originHeaderLabel = new QLabel;
    originHeaderLabel->setText("Origin");
    mainGlay->addWidget(originHeaderLabel, 0, 3, Qt::AlignHCenter);

    QLabel* dateHeaderLabel = new QLabel;
    dateHeaderLabel->setText("Date");
    mainGlay->addWidget(dateHeaderLabel, 0, 4, Qt::AlignHCenter);

    QStringList validLexicons;
    validLexicons.append(LEXICON_OWL2);
    validLexicons.append(LEXICON_OSPD4);
    validLexicons.append(LEXICON_CSW12);
    validLexicons.append(LEXICON_CSW07);
    validLexicons.append(LEXICON_WWF);
    validLexicons.append(LEXICON_CD);
    validLexicons.append(LEXICON_ODS5);
    validLexicons.append(LEXICON_FISE2009);
    validLexicons.append(LEXICON_SWL);
    validLexicons.append(LEXICON_ZINGA);
    validLexicons.append(LEXICON_VOLOST);
    validLexicons.append(LEXICON_ODS4);
    validLexicons.append(LEXICON_OWL);
    validLexicons.append(LEXICON_OSWI);
    validLexicons.append(LEXICON_CUSTOM);

    QStringListIterator it (validLexicons);
    for (int row = 1; it.hasNext(); ++row) {
        const QString& lexicon = it.next();

        QRadioButton* radioButton = new QRadioButton;
        if (row == 1)
            radioButton->setChecked(true);
        connect(radioButton, SIGNAL(clicked(bool)),
                SLOT(defaultLexiconChanged()));
        mainGlay->addWidget(radioButton, row, 0, Qt::AlignHCenter);
        lexiconRadioButtons.insert(lexicon, radioButton);

        QCheckBox* checkBox = new QCheckBox;
        if (row == 1)
            checkBox->setCheckState(Qt::Checked);
        mainGlay->addWidget(checkBox, row, 1, Qt::AlignHCenter);
        if (lexicon == LEXICON_CUSTOM) {
            connect(checkBox, SIGNAL(stateChanged(int)),
                    SLOT(customStateChanged(int)));
        }
        lexiconCheckBoxes.insert(lexicon, checkBox);

        QLabel* lexiconLabel = new QLabel;
        lexiconLabel->setText(lexicon);
        mainGlay->addWidget(lexiconLabel, row, 2);

        // Create custom lexicon chooser for custom lexicon, or origin/date
        // labels for other lexicons
        if (lexicon == LEXICON_CUSTOM) {
            customLexiconLine = new QLineEdit;
            customLexiconLine->setEnabled(false);
            mainGlay->addWidget(customLexiconLine, row, 3);

            customLexiconButton = new QPushButton;
            customLexiconButton->setText("Browse...");
            customLexiconButton->setSizePolicy(QSizePolicy::Fixed,
                                               QSizePolicy::Fixed);
            customLexiconButton->setEnabled(false);
            connect(customLexiconButton, SIGNAL(clicked()),
                    SLOT(customBrowseButtonClicked()));
            mainGlay->addWidget(customLexiconButton, row, 4);
        }
        else {
            QString lexiconOrigin = Auxil::lexiconToOrigin(lexicon);
            if (!lexiconOrigin.isEmpty()) {
                QLabel* originLabel = new QLabel;
                originLabel->setText(lexiconOrigin);
                mainGlay->addWidget(originLabel, row, 3);
            }

            QDate lexiconDate = Auxil::lexiconToDate(lexicon);
            if (lexiconDate.isValid()) {
                QLabel* dateLabel = new QLabel;
                dateLabel->setText(lexiconDate.toString("MMMM d, yyyy"));
                mainGlay->addWidget(dateLabel, row, 4);
            }
        }
    }

    mainVlay->addStretch(1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox;
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
//  getCustomLexiconFile
//
//! Determine the file to be used as a custom lexicon.
//
//! @return the custom lexicon file
//---------------------------------------------------------------------------
QString
LexiconSelectDialog::getCustomLexiconFile() const
{
    return customLexiconLine->text();
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
//  setCustomLexiconFile
//
//! Set the file to be used as a custom lexicon.
//
//! @param filename the custom lexicon file
//---------------------------------------------------------------------------
void
LexiconSelectDialog::setCustomLexiconFile(const QString& filename)
{
    customLexiconLine->setText(filename);
}

//---------------------------------------------------------------------------
//  customStateChanged
//
//! Called when the state of the Custom checkbox changes.  Enable or disable
//! the custom line and button appropriately.
//
//! @param state the new checkbox state
//---------------------------------------------------------------------------
void
LexiconSelectDialog::customStateChanged(int state)
{
    bool enable = (state == Qt::Checked);
    customLexiconLine->setEnabled(enable);
    customLexiconButton->setEnabled(enable);
}

//---------------------------------------------------------------------------
//  customBrowseButtonClicked
//
//! Called when the Browse button for the custom lexicon is clicked.
//---------------------------------------------------------------------------
void
LexiconSelectDialog::customBrowseButtonClicked()
{
    QString file = QFileDialog::getOpenFileName(this, IMPORT_CHOOSER_TITLE,
        Auxil::getWordsDir(), "All Files (*.*)");

    if (!file.isNull())
        customLexiconLine->setText(file);
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
