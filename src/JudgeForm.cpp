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
#include "DefinitionBox.h"
#include "WordEngine.h"
#include "WordTextEdit.h"
#include "WordValidator.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

using namespace Defs;

//---------------------------------------------------------------------------
//  JudgeForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
JudgeForm::JudgeForm (WordEngine* e, QWidget* parent, Qt::WFlags f)
    : ActionForm (JudgeFormType, parent, f), engine (e)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING);
    Q_CHECK_PTR (mainVlay);

    wordArea = new WordTextEdit;
    Q_CHECK_PTR (wordArea);
    wordArea->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect (wordArea, SIGNAL (textChanged()), SLOT (textChanged()));
    mainVlay->addWidget (wordArea);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    clearButton = new QPushButton ("&Clear");
    Q_CHECK_PTR (clearButton);
    clearButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (clearButton, SIGNAL (clicked()), SLOT (clear()));
    buttonHlay->addWidget (clearButton);

    judgeButton = new QPushButton ("&Judge");
    Q_CHECK_PTR (judgeButton);
    judgeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (judgeButton, SIGNAL (clicked()), SLOT (judgeWord()));
    buttonHlay->addWidget (judgeButton);

    resultBox = new DefinitionBox;
    Q_CHECK_PTR (resultBox);
    mainVlay->addWidget (resultBox, 1);

    mainVlay->addStretch (0);

    judgeButton->setEnabled (false);
    clearButton->setEnabled (false);
    resultBox->hide();
}

//---------------------------------------------------------------------------
//  getStatusString
//
//! Returns the current status string.
//
//! @return the current status string
//---------------------------------------------------------------------------
QString
JudgeForm::getStatusString() const
{
    return statusString;
}

//---------------------------------------------------------------------------
//  textChanged
//
//! Called when the contents of the input area change.  Changes the input area
//! contents to upper case, and enables or disables the buttons appropriately.
//---------------------------------------------------------------------------
void
JudgeForm::textChanged()
{
    QTextCursor cursor = wordArea->textCursor();
    QString text = wordArea->text();
    wordArea->blockSignals (true);
    wordArea->setText (text.upper());
    wordArea->setTextCursor (cursor);
    wordArea->blockSignals (false);
    bool empty = text.stripWhiteSpace().isEmpty();
    judgeButton->setEnabled (!empty);
    clearButton->setEnabled (!empty || resultBox->isShown());
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the input area and the result area.
//---------------------------------------------------------------------------
void
JudgeForm::clear()
{
    wordArea->clear();
    resultBox->hide();
    judgeButton->setEnabled (false);
    clearButton->setEnabled (false);
    statusString = QString::null;
    emit statusChanged (statusString);
}

//---------------------------------------------------------------------------
//  judgeWord
//
//! Look up and display the acceptability of the word currently in the word
//! edit area.
//---------------------------------------------------------------------------
void
JudgeForm::judgeWord()
{
    bool acceptable = true;

    QString text = wordArea->text().simplifyWhiteSpace();
    QStringList words = QStringList::split (QChar(' '), text);
    QStringList::iterator it;
    QString wordStr;
    for (it = words.begin(); it != words.end(); ++it) {
        if (acceptable && !engine->isAcceptable (*it))
            acceptable = false;
        if (!wordStr.isEmpty())
            wordStr += ", ";
        wordStr += *it;
    }

    QString resultStr = (acceptable
                 ? QString ("<font color=\"blue\">Acceptable</font>")
                 : QString ("<font color=\"red\">Unacceptable</font>"))
        + "<br>" + wordStr;

    resultBox->setText (resultStr);
    resultBox->setTitle ("The play is");
    resultBox->show();
    statusString = QString ("Play is ") +
        (acceptable ? QString ("Acceptable") : QString ("Unacceptable"));
    emit statusChanged (statusString);
}
