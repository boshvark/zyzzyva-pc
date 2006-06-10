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
#include "Auxil.h"
#include "Defs.h"
#include <QFile>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QVBoxLayout>

#include <QtDebug>

using namespace Defs;

const int KEEP_ALIVE_INTERVAL = 31000;

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
    connect (socket, SIGNAL (bytesWritten (qint64)),
             SLOT (socketBytesWritten (qint64)));
    //socket->connectToHost ("pietdepsi.com", 21);
    socket->connectToHost ("66.98.172.34", 1327);
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
    qDebug() << "*** socketStateChanged";
    switch (state) {
        case QAbstractSocket::UnconnectedState:
        qDebug() << "UnconnectedState: The socket is not connected.";
        break;

        case QAbstractSocket::HostLookupState:
        qDebug() << "HostLookupState: The socket is performing a host "
                    "name lookup.";
        break;

        case QAbstractSocket::ConnectingState:
        qDebug() << "ConnectingState: The socket has started establishing "
                    "a connection.";
        break;

        case QAbstractSocket::ConnectedState: {
            qDebug() << "ConnectedState: A connection is established.";

            QByteArray bytes;
            QString message;

            QFile file (Auxil::getUserConfigDir() + "/isc-creds");
            if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
                return;

            QByteArray credBytes = file.readLine();
            QString credStr (credBytes);
            credStr = credStr.simplified();

            message = "0 LOGIN " + credStr;
            bytes.append (char (0x00));
            bytes.append (char (0x2c));
            bytes.append (message);
            qDebug() << "Message: |" << message << "|";
            socket->write (bytes);

            // Wait for SETALL
            socket->waitForReadyRead (10000);
            // Wait for SET FORMULA, BUDDIES, etc.
            socket->waitForReadyRead (10000);

            message = "0 SOUGHT";
            bytes.clear();
            bytes.append (char (0x00));
            bytes.append (char (0x08));
            bytes.append (message);
            qDebug() << "Message: |" << message << "|";
            socket->write (bytes);

            socket->waitForReadyRead (10000);
            message = "0 RESUME LOGIN";
            bytes.clear();
            bytes.append (char (0x00));
            bytes.append (char (0x0e));
            bytes.append (message);
            qDebug() << "Message: |" << message << "|";
            socket->write (bytes);

            keepAliveTimer.setInterval (KEEP_ALIVE_INTERVAL);
            connect (&keepAliveTimer, SIGNAL (timeout()),
                     SLOT (keepAliveTimeout()));
            keepAliveTimer.start();
        }
        break;

        case QAbstractSocket::BoundState:
        qDebug() << "BoundState: The socket is bound to an address and port "
                    "(for servers).";
        break;

        case QAbstractSocket::ClosingState:
        qDebug() << "ClosingState: The socket is about to close (data may "
                    "still be waiting to be written).";
        break;

        case QAbstractSocket::ListeningState:
        qDebug() << "ListeningState: for internal use only.";
        break;

        default:
        qDebug() << "*** UNKNOWN STATE!";
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
    qDebug() << "*** socketReadyRead";
    QByteArray byteArray = socket->readAll();

    QString message;
    for (int i = 0; i < byteArray.size(); ++i) {
        QChar c (byteArray[i]);
        if (c.isPrint()) {
            message = byteArray.right (byteArray.size() - i);
            break;
        }
    }

    qDebug() << "Read " << byteArray.size() << " bytes: |" << message << "|";

    if (!byteArray.isEmpty())
        messageAppendHtml ("<font color=\"blue\">" + message +
                            "</font><br>");

    if (message.contains ("0 PING REPLY")) {
        QByteArray bytes;
        QString response = "0 PING REPLY";
        bytes.append (char (0x00));
        bytes.append (char (0x0c));
        bytes.append (response);
        qDebug() << "Bytes: |" << bytes << "|";
        socket->write (bytes);
    }
}

//---------------------------------------------------------------------------
//  socketBytesWritten
//
//! Called when data has been written to the socket.
//---------------------------------------------------------------------------
void
CrosswordGameForm::socketBytesWritten (qint64 bytes)
{
    qDebug() << "*** socketBytesWritten: " << bytes << " bytes";
}

//---------------------------------------------------------------------------
//  keepAliveTimeout
//
//! Called when the keep-alive timer goes off.
//---------------------------------------------------------------------------
void
CrosswordGameForm::keepAliveTimeout()
{
    qDebug() << "*** Keep alive timeout!";

    if (!socket)
        return;

    QByteArray bytes;
    QString response = "0 ALIVE";
    bytes.append (char (0x00));
    bytes.append (char (0x07));
    bytes.append (response);
    qDebug() << "Bytes: |" << bytes << "|";
    socket->write (bytes);
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
