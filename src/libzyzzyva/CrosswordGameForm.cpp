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
#include "Defs.h"
#include <QHBoxLayout>
#include <QTextCursor>
#include <QVBoxLayout>

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
    : ActionForm (CrosswordGameFormType, parent, f), socket (0)
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
    if (socket)
        socket->disconnectFromHost();
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
//  inputReturnPressed
//
//! Called when the Connect button is clicked.
//---------------------------------------------------------------------------
void
CrosswordGameForm::connectClicked()
{
    socket = new QTcpSocket (this);
    Q_CHECK_PTR (socket);
    connect (socket, SIGNAL (stateChanged (QAbstractSocket::SocketState)),
             SLOT (socketStateChanged (QAbstractSocket::SocketState)));
    connect (socket, SIGNAL (readyRead()), SLOT (socketReadyRead()));
    socket->connectToHost ("pietdepsi.com", 21);
    connectButton->setEnabled (false);
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

    messageAppendHtml ("<font color=\"red\">" + text + "</font><br>");
    inputLine->clear();
}

//---------------------------------------------------------------------------
//  socketStateChanged
//
//! Called when the socket state changes.
//---------------------------------------------------------------------------
void
CrosswordGameForm::socketStateChanged (QAbstractSocket::SocketState state)
{
    qDebug ("*** socketStateChanged");
    switch (state) {
        case QAbstractSocket::UnconnectedState:
        qDebug ("UnconnectedState: The socket is not connected.");
        break;

        case QAbstractSocket::HostLookupState:
        qDebug ("HostLookupState: The socket is performing a host "
                "name lookup.");
        break;

        case QAbstractSocket::ConnectingState:
        qDebug ("ConnectingState: The socket has started establishing "
                "a connection.");
        break;

        case QAbstractSocket::ConnectedState:
        qDebug ("ConnectedState: A connection is established.");
        break;

        case QAbstractSocket::BoundState:
        qDebug ("BoundState: The socket is bound to an address and port "
                "(for servers).");
        break;

        case QAbstractSocket::ClosingState:
        qDebug ("ClosingState: The socket is about to close (data may "
                "still be waiting to be written).");
        break;

        case QAbstractSocket::ListeningState:
        qDebug ("ListeningState: for internal use only.");
        break;

        default:
        qDebug ("*** UNKNOWN STATE!");
        break;
    }
}

//---------------------------------------------------------------------------
//  socketReadyRead
//
//! Called when the socket has data ready to read.
//---------------------------------------------------------------------------
void
CrosswordGameForm::socketReadyRead()
{
    qDebug ("*** socketReadyRead");
    while (socket->canReadLine()) {
        QByteArray byteArray = socket->readLine (8092);
        QString line (byteArray);
        messageAppendHtml ("<font color=\"blue\">" + line + "</font><br>");
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
CrosswordGameForm::messageAppendHtml (const QString& text)
{
    messageArea->insertHtml ("<font color=\"red\">" + text + "</font><br>");
    QTextCursor cursor = messageArea->textCursor();
    cursor.movePosition (QTextCursor::End);
    messageArea->setTextCursor (cursor);
}
