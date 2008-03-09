//---------------------------------------------------------------------------
// DefinitionDialog.cpp
//
// The dialog for word definitions.
//
// Copyright 2005, 2006, 2007, 2008 Michael W Thelen <mthelen@gmail.com>.
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
#include "DefinitionBox.h"
#include "WordEngine.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

const QString DIALOG_CAPTION_PREFIX = "Define : ";

using namespace Defs;

//---------------------------------------------------------------------------
//  DefinitionDialog
//
//! Constructor.
//
//! @param e the word engine to use for looking up definitions
//! @param lexicon the lexicon to use
//! @param word the word to define
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
DefinitionDialog::DefinitionDialog(WordEngine* e, const QString& lexicon,
                                   const QString& word, QWidget* parent,
                                   Qt::WFlags f)
    : QDialog(parent, f), engine(e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);
    Q_CHECK_PTR(mainVlay);

    bool acceptable = engine->isAcceptable(lexicon, word);
    QString wordAcceptable = acceptable ? word : word + "*";

    DefinitionBox* defBox = new DefinitionBox;
    Q_CHECK_PTR(defBox);
    defBox->setTitle(wordAcceptable);
    mainVlay->addWidget(defBox);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    Q_CHECK_PTR(buttonHlay);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    ZPushButton* closeButton = new ZPushButton("&Close");
    Q_CHECK_PTR(closeButton);
    closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setDefault(true);
    connect(closeButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(closeButton);

    setWindowTitle(DIALOG_CAPTION_PREFIX + wordAcceptable);

    QString definition = engine->getDefinition(lexicon, word);
    if (definition.isEmpty()) {
        definition = acceptable ? EMPTY_DEFINITION :
            QString("<font color=\"red\">Unacceptable</font>");
    }

    defBox->setText(Auxil::wordWrap(definition, DEFINITION_WRAP_LENGTH));
}

//---------------------------------------------------------------------------
//  ~DefinitionDialog
//
//! Destructor.
//---------------------------------------------------------------------------
DefinitionDialog::~DefinitionDialog()
{
}
