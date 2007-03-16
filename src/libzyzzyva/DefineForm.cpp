//---------------------------------------------------------------------------
// DefineForm.cpp
//
// A form for looking up and displaying word definitions.
//
// Copyright 2004, 2005, 2006, 2007 Michael W Thelen <mthelen@gmail.com>.
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
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

using namespace Defs;

const QString TITLE_PREFIX = "Definition";

//---------------------------------------------------------------------------
//  DefineForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
DefineForm::DefineForm(WordEngine* e, QWidget* parent, Qt::WFlags f)
    : ActionForm(DefineFormType, parent, f), engine(e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);
    Q_CHECK_PTR(mainVlay);

    QHBoxLayout* lookupHlay = new QHBoxLayout;
    lookupHlay->setSpacing(SPACING);
    Q_CHECK_PTR(lookupHlay);
    mainVlay->addLayout(lookupHlay);

    QLabel* label = new QLabel("Word : ");
    Q_CHECK_PTR(label);
    lookupHlay->addWidget(label);

    wordLine = new WordLineEdit;
    Q_CHECK_PTR(wordLine);
    wordLine->setValidator(new WordValidator(wordLine));
    connect(wordLine, SIGNAL(textChanged(const QString&)),
            SLOT(wordChanged(const QString&)));
    connect(wordLine, SIGNAL(returnPressed()), SLOT(defineWord()));
    lookupHlay->addWidget(wordLine);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    Q_CHECK_PTR(buttonHlay);
    mainVlay->addLayout(buttonHlay);

    defineButton = new ZPushButton("&Define");
    Q_CHECK_PTR(defineButton);
    defineButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(defineButton, SIGNAL(clicked()), SLOT(defineWord()));
    buttonHlay->addWidget(defineButton);

    resultBox = new DefinitionBox;
    Q_CHECK_PTR(resultBox);
    mainVlay->addWidget(resultBox, 1);

    mainVlay->addStretch(0);

    defineButton->setEnabled(false);
    resultBox->hide();
}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
DefineForm::getIcon() const
{
    return QIcon(":/define-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title string.
//
//! @return the current title string
//---------------------------------------------------------------------------
QString
DefineForm::getTitle() const
{
    return TITLE_PREFIX;
}

//---------------------------------------------------------------------------
//  getStatusString
//
//! Returns the current status string.
//
//! @return the current status string
//---------------------------------------------------------------------------
QString
DefineForm::getStatusString() const
{
    return QString::null;
}

//---------------------------------------------------------------------------
//  wordChanged
//
//! Called when the contents of the input line change.  Enables or disables
//! the buttons appropriately.
//---------------------------------------------------------------------------
void
DefineForm::wordChanged(const QString& word)
{
    defineButton->setEnabled(!word.isEmpty());
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
    if (word.isEmpty())
        return;
    bool acceptable = engine->isAcceptable(word);
    QString resultStr = acceptable ?
                        QString("<font color=\"blue\">Acceptable</font>") :
                        QString("<font color=\"red\">Unacceptable</font>");

    if (acceptable) {
        QString definition = engine->getDefinition(word);
        if (definition.isEmpty())
            definition = EMPTY_DEFINITION;
        resultStr += "<br>" + definition;
    }
    else {
        word += "*";
    }

    resultBox->setText(resultStr);
    resultBox->setTitle(word);
    resultBox->show();
    selectInputArea();
}

//---------------------------------------------------------------------------
//  selectInputArea
//
//! Give focus to the input area and select its contents.
//---------------------------------------------------------------------------
void
DefineForm::selectInputArea()
{
    wordLine->setFocus();
    wordLine->setSelection(0, wordLine->text().length());
}
