//---------------------------------------------------------------------------
// LexiconChooserDialog.cpp
//
// A dialog for choosing a lexicon.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include "LexiconChooserDialog.h"
#include "Auxil.h"
#include "Defs.h"
#include "ZPushButton.h"
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Choose Lexicon";

using namespace Defs;

//---------------------------------------------------------------------------
//  LexiconChooserDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
LexiconChooserDialog::LexiconChooserDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lexiconHlay = new QHBoxLayout;
    Q_CHECK_PTR (lexiconHlay);
    mainVlay->addLayout (lexiconHlay);

    QLabel* lexiconLabel = new QLabel (this);
    Q_CHECK_PTR (lexiconLabel);
    lexiconLabel->setText ("Lexicon:");
    lexiconHlay->addWidget (lexiconLabel);

    lexiconCombo = new QComboBox (this);
    Q_CHECK_PTR (lexiconCombo);
    lexiconCombo->addItem ("OWL+LWL");
    lexiconCombo->addItem ("OWL2+LWL");
    lexiconCombo->addItem ("OSWI");
    lexiconCombo->addItem ("ODS");
    lexiconCombo->addItem ("Custom");
    lexiconCombo->setCurrentIndex (lexiconCombo->findText ("OWL2+LWL"));
    lexiconHlay->addWidget (lexiconCombo);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    ZPushButton* okButton = new ZPushButton ("&OK");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    ZPushButton* cancelButton = new ZPushButton ("Cancel");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setWindowTitle (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  getLexicon
//
//! Get the chosen lexicon.
//
//! @return the chosen lexicon
//---------------------------------------------------------------------------
QString
LexiconChooserDialog::getLexicon() const
{
    return lexiconCombo->currentText();
}
