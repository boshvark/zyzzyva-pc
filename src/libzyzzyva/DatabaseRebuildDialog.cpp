//---------------------------------------------------------------------------
// DatabaseRebuildDialog.cpp
//
// A dialog for specifying lexicon databases to be rebuilt.
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

#include "DatabaseRebuildDialog.h"
#include "LexiconSelectWidget.h"
#include "Auxil.h"
#include "Defs.h"
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Rebuild Lexicon Database";

using namespace Defs;

//---------------------------------------------------------------------------
//  DatabaseRebuildDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
DatabaseRebuildDialog::DatabaseRebuildDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QLabel* instructionLabel = new QLabel;
    Q_CHECK_PTR(instructionLabel);
    QString message = "Please choose the lexicon database to rebuild.\n"
        "This may take several minutes.";
    message = Auxil::dialogWordWrap(message);
    instructionLabel->setText(message);
    mainVlay->addWidget(instructionLabel);

    rebuildAllButton = new QRadioButton;
    Q_CHECK_PTR(rebuildAllButton);
    rebuildAllButton->setText("Rebuild databases for all lexicons");
    rebuildAllButton->setChecked(true);
    connect(rebuildAllButton, SIGNAL(toggled(bool)),
            SLOT(rebuildAllToggled(bool)));
    mainVlay->addWidget(rebuildAllButton);

    QRadioButton* rebuildSingleButton = new QRadioButton;
    Q_CHECK_PTR(rebuildSingleButton);
    rebuildSingleButton->setText("Rebuild database for a single lexicon");
    mainVlay->addWidget(rebuildSingleButton);

    lexiconWidget = new LexiconSelectWidget;
    Q_CHECK_PTR(lexiconWidget);
    lexiconWidget->setEnabled(false);
    mainVlay->addWidget(lexiconWidget);

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
//  ~DatabaseRebuildDialog
//
//! Destructor.
//---------------------------------------------------------------------------
DatabaseRebuildDialog::~DatabaseRebuildDialog()
{
}

//---------------------------------------------------------------------------
//  getRebuildAll
//
//! Determine whether all databases should be rebuilt for all lexicons.
//
//! @return true if Rebuild All is selected, false otherwise
//---------------------------------------------------------------------------
bool
DatabaseRebuildDialog::getRebuildAll() const
{
    return rebuildAllButton->isChecked();
}

//---------------------------------------------------------------------------
//  getLexicon
//
//! Return the selected lexicon.
//
//! @return the selected lexicon
//---------------------------------------------------------------------------
QString
DatabaseRebuildDialog::getLexicon() const
{
    return lexiconWidget->getCurrentLexicon();
}

//---------------------------------------------------------------------------
//  rebuildAllToggled
//
//! Called when the Rebuild All button is toggled.
//
//! @param on whether the button is checked
//---------------------------------------------------------------------------
void
DatabaseRebuildDialog::rebuildAllToggled(bool on)
{
    lexiconWidget->setEnabled(!on);
}
