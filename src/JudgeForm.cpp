//---------------------------------------------------------------------------
// JudgeForm.cpp
//
// A form for looking up words.
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

#include "JudgeForm.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "Defs.h"
#include <qlayout.h>

using namespace Defs;

//---------------------------------------------------------------------------
// JudgeForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
JudgeForm::JudgeForm (WordEngine* e, QWidget* parent, const char* name,
                        WFlags f)
    : QFrame (parent, name, f), engine (e)
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

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    wordLine->setValidator (new WordValidator (wordLine));
    connect (wordLine, SIGNAL (returnPressed()), SLOT (lookupWord()));
    lookupHlay->addWidget (wordLine);

    QHBoxLayout* resultHlay = new QHBoxLayout (SPACING, "resultHlay");
    mainVlay->addLayout (resultHlay);
    resultLabel = new QLabel (this, "resultLabel");
    resultHlay->addWidget (resultLabel, 0, Qt::AlignHCenter);
}

//---------------------------------------------------------------------------
// lookupWord
//
//! Look up and display the acceptability of the word currently in the word
//! edit area.
//---------------------------------------------------------------------------
void
JudgeForm::lookupWord()
{
    QString word = wordLine->text();
    if (word.isEmpty()) return;
    QString statusStr = engine->isAcceptable (word) ?
                        QString ("<font color=\"blue\">Acceptable</font>") :
                        QString ("<font color=\"red\">Unacceptable</font>");
    resultLabel->setText (word + " : " + statusStr);
}
