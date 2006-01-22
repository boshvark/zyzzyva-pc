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
#include <QTimer>
#include <QVBoxLayout>

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
    formFont.setPixelSize (40);

    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING);
    Q_CHECK_PTR (mainVlay);

    widgetStack = new QStackedWidget;
    Q_CHECK_PTR (widgetStack);
    mainVlay->addWidget (widgetStack);

    wordArea = new WordTextEdit;
    Q_CHECK_PTR (wordArea);
    wordArea->setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Fixed);
    wordArea->setFont (formFont);
    connect (wordArea, SIGNAL (textChanged()), SLOT (textChanged()));
    widgetStack->addWidget (wordArea);

    resultGbox = new QGroupBox;
    Q_CHECK_PTR (resultGbox);
    resultGbox->setFont (formFont);
    widgetStack->addWidget (resultGbox);

    QHBoxLayout* resultHlay = new QHBoxLayout (resultGbox, MARGIN, SPACING);
    Q_CHECK_PTR (resultHlay);

    resultLabel = new QLabel;
    Q_CHECK_PTR (resultLabel);
    resultLabel->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    resultLabel->setWordWrap (true);
    resultHlay->addWidget (resultLabel);

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
    QTextCursor cursor = wordArea->textCursor();
    int origCursorPosition = cursor.position();
    int deletedBeforeCursor = 0;

    wordArea->blockSignals (true);
    QString text = wordArea->text().upper();
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

    wordArea->setText (text);
    cursor.setPosition (origCursorPosition - deletedBeforeCursor);
    wordArea->setTextCursor (cursor);
    wordArea->blockSignals (false);

    if (doJudge)
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
    wordArea->clear();
    widgetStack->setCurrentWidget (wordArea);
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
                 ? QString ("<font color=\"blue\">YES, the play is "
                            "ACCEPTABLE</font>")
                 : QString ("<font color=\"red\">NO, the play is "
                            "UNACCEPTABLE</font>"))
        + "<br><br>" + wordStr;

    resultLabel->setText (resultStr);
    widgetStack->setCurrentWidget (resultGbox);

    QTimer::singleShot (5000, this, SLOT (clear()));
}
