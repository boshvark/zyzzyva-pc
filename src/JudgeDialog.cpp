//---------------------------------------------------------------------------
// JudgeDialog.cpp
//
// A full-screen dialog for Word Judge functionality, in which the user can
// very easily judge the validity of one or more words.
//
// Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#include "JudgeDialog.h"
#include "DefinitionBox.h"
#include "WordEngine.h"
#include "WordTextEdit.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QPalette>
#include <QTimer>
#include <QVBoxLayout>

const int FONT_PIXEL_SIZE = 40;
const int INPUT_MARGIN = 30;
const int RESULT_BORDER_WIDTH = 15;

using namespace Defs;

//---------------------------------------------------------------------------
//  JudgeDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
JudgeDialog::JudgeDialog (WordEngine* e, QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f), engine (e)
{
    QFont formFont = qApp->font();
    formFont.setPixelSize (FONT_PIXEL_SIZE);

    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, 0);
    Q_CHECK_PTR (mainVlay);

    widgetStack = new QStackedWidget;
    Q_CHECK_PTR (widgetStack);
    mainVlay->addWidget (widgetStack);

    inputWidget = new QWidget;
    Q_CHECK_PTR (inputWidget);
    widgetStack->addWidget (inputWidget);

    QVBoxLayout* inputVlay = new QVBoxLayout (inputWidget, INPUT_MARGIN, 0);
    Q_CHECK_PTR (inputVlay);

    QLabel* instLabel = new QLabel ("Hit the Space bar or Enter key to "
                                    "separate words.  Hit the Tab key to "
                                    "judge the play.");
    Q_CHECK_PTR (instLabel);
    instLabel->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    inputVlay->addWidget (instLabel);

    inputArea = new WordTextEdit;
    Q_CHECK_PTR (inputArea);
    inputArea->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
    inputArea->setFont (formFont);
    connect (inputArea, SIGNAL (textChanged()), SLOT (textChanged()));
    inputVlay->addWidget (inputArea);

    resultWidget = new QWidget;
    Q_CHECK_PTR (resultWidget);
    resultWidget->setSizePolicy (QSizePolicy::Expanding,
                                 QSizePolicy::Expanding);
    widgetStack->addWidget (resultWidget);

    QVBoxLayout* resultVlay = new QVBoxLayout (resultWidget, 0, 0);
    Q_CHECK_PTR (resultVlay);

    resultLabel = new QLabel;
    Q_CHECK_PTR (resultLabel);
    resultLabel->setFont (formFont);
    resultLabel->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    resultLabel->setWordWrap (true);
    resultLabel->setFrameStyle (QFrame::Box | QFrame::Plain);
    resultLabel->setLineWidth (RESULT_BORDER_WIDTH);
    resultVlay->addWidget (resultLabel);

    clear();
    showFullScreen();
}

//---------------------------------------------------------------------------
//  textChanged
//
//! Called when the contents of the input area change.  Changes the input area
//! contents to upper case, and enables or disables the buttons appropriately.
//---------------------------------------------------------------------------
void
JudgeDialog::textChanged()
{
    QTextCursor cursor = inputArea->textCursor();
    int origCursorPosition = cursor.position();
    int deletedBeforeCursor = 0;

    inputArea->blockSignals (true);
    QString text = inputArea->text().upper();
    int lookIndex = 0;
    bool afterSpace = false;
    bool doJudge = false;
    for (int i = 0; i < text.length(); ++i) {
        QChar c = text.at (lookIndex);

        if (c.isLetter()) {
            afterSpace = false;
            ++lookIndex;
        }
        else if (c.isSpace() && (c != '\t') && !afterSpace) {
            text.replace (lookIndex, 1, "\n");
            afterSpace = true;
            ++lookIndex;
        }
        else {
            if (c == '\t')
                doJudge = true;
            text.remove (lookIndex, 1);
            if (i < origCursorPosition)
                ++deletedBeforeCursor;
        }
    }
    text.replace (QRegExp ("\\n+"), "\n");

    inputArea->setText (text);
    cursor.setPosition (origCursorPosition - deletedBeforeCursor);
    inputArea->setTextCursor (cursor);
    inputArea->blockSignals (false);

    if (doJudge && !text.isEmpty())
        judgeWord();
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the input area and the result area.
//---------------------------------------------------------------------------
void
JudgeDialog::clear()
{
    inputArea->clear();
    widgetStack->setCurrentWidget (inputWidget);
}

//---------------------------------------------------------------------------
//  judgeWord
//
//! Look up and display the acceptability of the word currently in the word
//! edit area.
//---------------------------------------------------------------------------
void
JudgeDialog::judgeWord()
{
    bool acceptable = true;

    QString text = inputArea->text().simplifyWhiteSpace();
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

    QPalette pal = resultLabel->palette();
    pal.setColor (QPalette::Foreground, resultColor);
    resultLabel->setPalette (pal);

    resultLabel->setText (resultStr);
    widgetStack->setCurrentWidget (resultWidget);

    QTimer::singleShot (5000, this, SLOT (clear()));
}
