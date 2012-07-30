//---------------------------------------------------------------------------
// JudgeSelectDialog.cpp
//
// A dialog for selecting a word judge lexicon.
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

#include "JudgeSelectDialog.h"
#include "LexiconSelectWidget.h"
#include "Auxil.h"
#include "Defs.h"
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Entering Full Screen Word Judge";

using namespace Defs;

//---------------------------------------------------------------------------
//  JudgeSelectDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
JudgeSelectDialog::JudgeSelectDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(20);
    mainVlay->setSpacing(20);

    QLabel* instructionLabel = new QLabel;
    QString message = "You are now entering the full screen Word Judge mode.\n"
                      "To exit full screen mode, press ESC while holding the "
                      "Shift key.\n\n"
                      "Please choose a lexicon for the Word Judge.";
    message = Auxil::dialogWordWrap(message);
    instructionLabel->setAlignment(Qt::AlignHCenter);
    instructionLabel->setText(message);
    mainVlay->addWidget(instructionLabel);

    lexiconWidget = new LexiconSelectWidget;
    QFont font = lexiconWidget->font();
    font.setPointSize(font.pointSize() + 20);
    lexiconWidget->setFont(font);
    lexiconWidget->resize(lexiconWidget->sizeHint());
    mainVlay->addWidget(lexiconWidget);

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
//  ~JudgeSelectDialog
//
//! Destructor.
//---------------------------------------------------------------------------
JudgeSelectDialog::~JudgeSelectDialog()
{
}

//---------------------------------------------------------------------------
//  getLexicon
//
//! Return the selected lexicon.
//
//! @return the selected lexicon
//---------------------------------------------------------------------------
QString
JudgeSelectDialog::getLexicon() const
{
    return lexiconWidget->getCurrentLexicon();
}
