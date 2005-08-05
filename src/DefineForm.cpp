//---------------------------------------------------------------------------
// DefineForm.cpp
//
// A form for looking up and displaying word definitions.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "DefineForm.h"
#include "DefinitionBox.h"
#include "WordEngine.h"
#include "WordLineEdit.h"
#include "WordValidator.h"
#include "Auxil.h"
#include "Defs.h"
#include <qlabel.h>
#include <qlayout.h>

using namespace Defs;

//---------------------------------------------------------------------------
//  DefineForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
DefineForm::DefineForm (WordEngine* e, QWidget* parent, const char* name,
                        WFlags f)
    : ActionForm (DefineFormType, parent, name, f), engine (e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lookupHlay = new QHBoxLayout (SPACING, "lookupHlay");
    Q_CHECK_PTR (lookupHlay);
    mainVlay->addLayout (lookupHlay);

    QLabel* label = new QLabel ("Word : ", this, "label");
    Q_CHECK_PTR (label);
    lookupHlay->addWidget (label);

    wordLine = new WordLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    wordLine->setValidator (new WordValidator (wordLine));
    connect (wordLine, SIGNAL (textChanged (const QString&)),
             SLOT (wordChanged (const QString&)));
    connect (wordLine, SIGNAL (returnPressed()), SLOT (defineWord()));
    lookupHlay->addWidget (wordLine);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    defineButton = new QPushButton ("&Define", this, "defineButton");
    Q_CHECK_PTR (defineButton);
    defineButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (defineButton, SIGNAL (clicked()), SLOT (defineWord()));
    buttonHlay->addWidget (defineButton);

    QHBoxLayout* resultHlay = new QHBoxLayout (SPACING, "resultHlay");
    Q_CHECK_PTR (resultHlay);
    mainVlay->addLayout (resultHlay);

    resultBox = new DefinitionBox (this, "resultBox");
    Q_CHECK_PTR (resultBox);
    resultHlay->addWidget (resultBox);

    defineButton->setEnabled (false);
    resultBox->hide();
}

//---------------------------------------------------------------------------
//  wordChanged
//
//! Called when the contents of the input line change.  Enables or disables
//! the buttons appropriately.
//---------------------------------------------------------------------------
void
DefineForm::wordChanged (const QString& word)
{
    defineButton->setEnabled (!word.isEmpty());
}

//---------------------------------------------------------------------------
//  defineWord
//
//! Look up and display the acceptability and definition of the word currently
//! in the word edit area.
//---------------------------------------------------------------------------
void
DefineForm::defineWord()
{
    QString word = wordLine->text();
    if (word.isEmpty()) return;
    bool acceptable = engine->isAcceptable (word);
    QString resultStr = acceptable ?
                        QString ("<font color=\"blue\">Acceptable</font>") :
                        QString ("<font color=\"red\">Unacceptable</font>");

    if (acceptable) {
        QString definition = engine->getDefinition (word);
        if (definition.isEmpty())
            definition = EMPTY_DEFINITION;
        resultStr += "<br>" + definition;
    }
    else {
        word += "*";
    }

    resultBox->setText (resultStr);
    resultBox->setTitle (word);
    resultBox->show();
    wordLine->clear();
}
