//---------------------------------------------------------------------------
// CrosswordGameForm.cpp
//
// A form for playing a crossword game.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include "CrosswordGameForm.h"
#include "IscConnectionThread.h"
#include "Auxil.h"
#include "Defs.h"
#include <QFile>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QVBoxLayout>

#include <QtDebug>

using namespace Defs;

//---------------------------------------------------------------------------
//  CrosswordGameForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CrosswordGameForm::CrosswordGameForm (QWidget* parent, Qt::WFlags f)
    : ActionForm (CrosswordGameFormType, parent, f), iscThread (0)
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this);
    Q_CHECK_PTR (mainHlay);
    mainHlay->setMargin (MARGIN);
    mainHlay->setSpacing (SPACING);

    QVBoxLayout* boardVlay = new QVBoxLayout;
    Q_CHECK_PTR (boardVlay);
    boardVlay->setMargin (0);
    boardVlay->setSpacing (SPACING);
    mainHlay->addLayout (boardVlay);

    boardTable = new QTableWidget (this);
    Q_CHECK_PTR (boardTable);
    boardTable->setColumnCount (15);
    boardTable->setRowCount (15);
    for (int i = 0; i < 15; ++i) {
        boardTable->setColumnWidth (i, 20);
        boardTable->setRowHeight (i, 20);
    }
    boardVlay->addWidget (boardTable);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR (buttonHlay);
    boardVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    connectButton = new QPushButton ("&Connect", this);
    Q_CHECK_PTR (connectButton);
    connect (connectButton, SIGNAL (clicked()), SLOT (connectClicked()));
    buttonHlay->addWidget (connectButton);

    disconnectButton = new QPushButton ("&Disconnect", this);
    Q_CHECK_PTR (disconnectButton);
    connect (disconnectButton, SIGNAL (clicked()), SLOT (disconnectClicked()));
    disconnectButton->setEnabled (false);
    buttonHlay->addWidget (disconnectButton);

    buttonHlay->addStretch (1);

    QVBoxLayout* messageVlay = new QVBoxLayout;
    Q_CHECK_PTR (messageVlay);
    messageVlay->setMargin (0);
    messageVlay->setSpacing (SPACING);
    mainHlay->addLayout (messageVlay);

    messageArea = new QTextEdit (this);
    Q_CHECK_PTR (messageArea);
    messageArea->setReadOnly (true);
    messageVlay->addWidget (messageArea);

    inputLine = new QLineEdit (this);
    Q_CHECK_PTR (inputLine);
    connect (inputLine, SIGNAL (returnPressed()),
             SLOT (inputReturnPressed()));
    messageVlay->addWidget (inputLine);
}

//---------------------------------------------------------------------------
//  ~CrosswordGameForm
//
//! Destructor.
//---------------------------------------------------------------------------
CrosswordGameForm::~CrosswordGameForm()
{
}

//---------------------------------------------------------------------------
//  getStatusString
//
//! Returns the current status string.
//
//! @return the current status string
//---------------------------------------------------------------------------
QString
CrosswordGameForm::getStatusString() const
{
    return statusString;
}

//---------------------------------------------------------------------------
//  connectClicked
//
//! Called when the Connect button is clicked.
//---------------------------------------------------------------------------
void
CrosswordGameForm::connectClicked()
{
    if (!iscThread)
        iscThread = new IscConnectionThread (this);

    if (iscThread->isRunning())
        return;

    QFile file (Auxil::getUserConfigDir() + "/isc-creds");
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return;

    connect (iscThread, SIGNAL (messageReceived (const QString&)),
             SLOT (threadMessageReceived (const QString&)));
    connect (iscThread, SIGNAL (statusChanged (const QString&)),
             SLOT (threadStatusChanged (const QString&)));

    QByteArray credBytes = file.readLine();
    QString credStr (credBytes);
    if (!iscThread->connectToServer (credStr.simplified()))
        return;

    connectButton->setEnabled (false);
    disconnectButton->setEnabled (true);
}

//---------------------------------------------------------------------------
//  disconnectClicked
//
//! Called when the Disconnect button is clicked.
//---------------------------------------------------------------------------
void
CrosswordGameForm::disconnectClicked()
{
    if (!iscThread)
        return;

    iscThread->disconnectFromServer();
    iscThread->quit();
    delete iscThread;
    iscThread = 0;

    disconnectButton->setEnabled (false);
    connectButton->setEnabled (true);
}

//---------------------------------------------------------------------------
//  inputReturnPressed
//
//! Called when return is pressed in the input line.
//---------------------------------------------------------------------------
void
CrosswordGameForm::inputReturnPressed()
{
    QString text = inputLine->text();
    if (text.isEmpty())
        return;

    messageAppendHtml (text, QColor (0xff, 0x00, 0x00));

    iscThread->sendMessage (text);

    inputLine->clear();
}

//---------------------------------------------------------------------------
//  threadStatusChanged
//
//! Called when a status message is received from the connection thread.
//
//! @param status the status message
//---------------------------------------------------------------------------
void
CrosswordGameForm::threadStatusChanged (const QString& status)
{
    statusString = status;
    emit statusChanged (status);
}

//---------------------------------------------------------------------------
//  threadMessageReceived
//
//! Called when a message is received from the connection thread.
//
//! @param message the message
//---------------------------------------------------------------------------
void
CrosswordGameForm::threadMessageReceived (const QString& message)
{
    QString str = message.trimmed();
    QString command = str.section (" ", 0, 0).toUpper();
    QString args = str.section (" ", 1);

    // Take care of messages the GUI doesn't need to know about
    if (command == "TELL") {
        QString sender = args.section (" ", 0, 0);
        QString channel = args.section (" ", 1, 1); // ?
        QString text = args.section (" ", 2);

        messageAppendHtml (sender + " tells you: " + text,
                           QColor (0x8b, 0x00, 0x8b));
    }

    else if (command == "ASITIS") {
        messageAppendHtml (args, QColor (0x00, 0x00, 0x00));
    }

    else if (command == "SETALL") {
        // do nothing yet
    }

    else if (command == "SOUGHT") {
        // do nothing yet
    }

    else if (command == "SEEK") {
        // do nothing yet
    }

    else if (command == "UNSEEK") {
        // do nothing yet
    }

    else {
        messageAppendHtml (str, QColor (0x00, 0x00, 0xff));
    }
}

//---------------------------------------------------------------------------
//  messageAppendHtml
//
//! Append HTML to the end of the message area.
//
//! @param text the text to append
//---------------------------------------------------------------------------
void
CrosswordGameForm::messageAppendHtml (const QString& text,
                                      const QColor& color)
{
    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    int minPad = 2;
    int base = 16;
    QChar pad = '0';
    QString colorStr = QString ("#%1%2%3").arg (red, minPad, base, pad).
                                           arg (green, minPad, base, pad).
                                           arg (blue, minPad, base, pad);

    QString html = "<font color=\"" + colorStr + "\">" +
                   encodeHtmlEntities (text) + "</font><br>";
    qDebug() << "HTML: " << html;
    messageArea->insertHtml (html);

    QTextCursor cursor = messageArea->textCursor();
    cursor.movePosition (QTextCursor::End);
    messageArea->setTextCursor (cursor);
}

//---------------------------------------------------------------------------
//  encodeHtmlEntities
//
//! Encode HTML entities in a string.
//
//! @param text the string to encode
//! @return the encoded string
//---------------------------------------------------------------------------
QString
CrosswordGameForm::encodeHtmlEntities (const QString& text)
{
    QString encoded = text;
    encoded.replace ("&", "&amp;");
    encoded.replace ("<", "&lt;");
    encoded.replace (">", "&gt;");
    encoded.replace ("\n", "<br>");
    return encoded;
}
