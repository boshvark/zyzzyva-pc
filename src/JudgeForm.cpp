//---------------------------------------------------------------------------
// JudgeForm.cpp
//
// A form for looking up words.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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
#include "JudgeDialog.h"
#include "DefinitionLabel.h"
#include "WordEngine.h"
#include "WordTextEdit.h"
#include "WordValidator.h"
#include "ZPushButton.h"
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

    judgeButton = new ZPushButton ("&Judge");
    Q_CHECK_PTR (judgeButton);
    judgeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (judgeButton, SIGNAL (clicked()), SLOT (judgeWord()));
    buttonHlay->addWidget (judgeButton);

    clearButton = new ZPushButton ("&Clear");
    Q_CHECK_PTR (clearButton);
    clearButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (clearButton, SIGNAL (clicked()), SLOT (clear()));
    buttonHlay->addWidget (clearButton);

    ZPushButton* fullScreenButton = new ZPushButton ("Full &Screen");
    Q_CHECK_PTR (fullScreenButton);
    fullScreenButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (fullScreenButton, SIGNAL (clicked()), SLOT (doFullScreen()));
    buttonHlay->addWidget (fullScreenButton);

    resultBox = new QGroupBox;
    Q_CHECK_PTR (resultBox);
    mainVlay->addWidget (resultBox, 1);

    QVBoxLayout* resultVlay = new QVBoxLayout (resultBox, MARGIN, SPACING);
    Q_CHECK_PTR (resultVlay);

    resultLabel = new DefinitionLabel;
    Q_CHECK_PTR (resultLabel);
    resultLabel->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    resultVlay->addWidget (resultLabel);

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
    int origCursorPosition = cursor.position();
    int deletedBeforeCursor = 0;

    wordArea->blockSignals (true);
    QString text = wordArea->text().upper();
    int lookIndex = 0;
    bool afterSpace = false;
    for (int i = 0; i < text.length(); ++i) {
        QChar c = text.at (lookIndex);

        if (c.isLetter()) {
            afterSpace = false;
            ++lookIndex;
        }
        else if ((lookIndex > 0) && c.isSpace() && !afterSpace) {
            text.replace (lookIndex, 1, "\n");
            afterSpace = true;
            ++lookIndex;
        }
        else {
            text.remove (lookIndex, 1);
            if (i < origCursorPosition)
                ++deletedBeforeCursor;
        }
    }
    text.replace (QRegExp ("\\n+"), "\n");

    wordArea->setText (text);
    cursor.setPosition (origCursorPosition - deletedBeforeCursor);
    wordArea->setTextCursor (cursor);
    wordArea->blockSignals (false);

    text = wordArea->text();
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

    QString resultStr;
    QColor resultColor;
    if (acceptable) {
        resultStr = "<font color=\"blue\">YES, the play is ACCEPTABLE</font>";
        resultColor = Qt::blue;
    }
    else {
        resultStr = "<font color=\"red\">NO, the play is UNACCEPTABLE</font>";
        resultColor = Qt::red;
    }
    resultStr += "<br><br>" + wordStr;

    resultLabel->setText (resultStr);
    resultBox->show();
    statusString = QString ("Play is ") +
        (acceptable ? QString ("ACCEPTABLE") : QString ("UNACCEPTABLE"));
    emit statusChanged (statusString);
}

//---------------------------------------------------------------------------
//  doFullScreen
//
//! Enter full-screen Word Judge mode using a JudgeDialog.
//---------------------------------------------------------------------------
void
JudgeForm::doFullScreen()
{
    JudgeDialog* dialog = new JudgeDialog (engine, this);
    Q_CHECK_PTR (dialog);
    dialog->exec();
    delete dialog;
}
