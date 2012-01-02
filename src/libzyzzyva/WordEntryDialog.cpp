//---------------------------------------------------------------------------
// WordEntryDialog.cpp
//
// A dialog for getting a word from the user.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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

#include "WordEntryDialog.h"
#include "LexiconSelectWidget.h"
#include "WordValidator.h"
#include "ZPushButton.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Enter a word";

using namespace Defs;

//---------------------------------------------------------------------------
//  WordEntryDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
WordEntryDialog::WordEntryDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f),
    wordValidator(new WordValidator(this))
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    lexiconWidget = new LexiconSelectWidget;
    Q_CHECK_PTR(lexiconWidget);
    mainVlay->addWidget(lexiconWidget);

    QHBoxLayout* lineHlay = new QHBoxLayout;
    Q_CHECK_PTR(lineHlay);
    lineHlay->setSpacing(SPACING);
    mainVlay->addLayout(lineHlay);

    QLabel* label = new QLabel("Word:");
    Q_CHECK_PTR(label);
    lineHlay->addWidget(label);

    wordLine = new QLineEdit;
    Q_CHECK_PTR(wordLine);
    wordLine->setValidator(wordValidator);
    lineHlay->addWidget(wordLine);

    // OK/Cancel buttons
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR(buttonHlay);
    buttonHlay->setSpacing(SPACING);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    ZPushButton* okButton = new ZPushButton("OK");
    Q_CHECK_PTR(okButton);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    Q_CHECK_PTR(cancelButton);
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    setWindowTitle(DIALOG_CAPTION);
    wordLine->setFocus();
}

//---------------------------------------------------------------------------
//  ~WordEntryDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordEntryDialog::~WordEntryDialog()
{
    delete wordValidator;
}

//---------------------------------------------------------------------------
//  getLexicon
//
//! Return the selected lexicon.
//
//! @return the selected lexicon
//---------------------------------------------------------------------------
QString
WordEntryDialog::getLexicon() const
{
    return lexiconWidget->getCurrentLexicon();
}
