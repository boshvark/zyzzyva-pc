//---------------------------------------------------------------------------
// IscConnectionThread.cpp
//
// A class for managing an ISC connection in the background.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "IscConnectionThread.h"
#include "Rand.h"
#include "Auxil.h"

#include <QtDebug>

const int KEEP_ALIVE_INTERVAL = 31000;

//---------------------------------------------------------------------------
//  ~IscConnectionThread
//
//! Destructor.
//---------------------------------------------------------------------------
IscConnectionThread::~IscConnectionThread()
{
    if (socket)
        socket->disconnectFromHost();
    delete socket;
}

//---------------------------------------------------------------------------
//  run
//
//! Connect to ISC and manage the connection.
//---------------------------------------------------------------------------
void
IscConnectionThread::run()
{
    exec();
}

//---------------------------------------------------------------------------
//  connectToServer
//
//! Connect to the server.
//
//! @param creds login credentials
//! @param err return in case of error
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
IscConnectionThread::connectToServer (const QString& creds,
                                      QAbstractSocket::SocketError* err)
{
    credentials = creds;
    socket = new QTcpSocket (this);
    Q_CHECK_PTR (socket);
    connect (socket, SIGNAL (error()), SLOT (socketError()));
    connect (socket, SIGNAL (stateChanged (QAbstractSocket::SocketState)),
             SLOT (socketStateChanged (QAbstractSocket::SocketState)));
    connect (socket, SIGNAL (readyRead()), SLOT (socketReadyRead()));

    // Connect to a random port between 1321 and 1330
    Rand rng (Rand::MarsagliaMwc, std::time (0), Auxil::getPid());
    int port = 1321 + rng.rand (9);
    socket->connectToHost ("66.98.172.34", port);

    if (socketHadError) {
        if (err)
            *err = socket->error();
        delete socket;
        socket = 0;
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
//  disconnectFromServer
//
//! Disconnect from the server.
//---------------------------------------------------------------------------
void
IscConnectionThread::disconnectFromServer()
{
    if (!socket)
        return;

    socket->disconnectFromHost();
    delete socket;
    socket = 0;
}

//---------------------------------------------------------------------------
//  sendMessage
//
//! Send a message to the server.
//
//! @param message the message
//---------------------------------------------------------------------------
void
IscConnectionThread::sendMessage (const QString& message)
{
    if (!socket)
        return;

    QString str = message.simplified();
    QString command = str.section (" ", 0, 0).toUpper();
    QString args = str.section (" ", 1);
    socket->write (encodeMessage (command + " " + args));
}

//---------------------------------------------------------------------------
//  receiveMessage
//
//! Receive a message from the server.
//
//! @param message the message
//---------------------------------------------------------------------------
void
IscConnectionThread::receiveMessage (const QString& message)
{
    if (message == "PING REPLY")
        sendMessage ("PING REPLY");
    else
        emit messageReceived (message);
}

//---------------------------------------------------------------------------
//  socketError
//
//! Called when the socket encounters an error.
//---------------------------------------------------------------------------
void
IscConnectionThread::socketError()
{

}

//---------------------------------------------------------------------------
//  socketStateChanged
//
//! Called when the socket state changes.
//---------------------------------------------------------------------------
void
IscConnectionThread::socketStateChanged (QAbstractSocket::SocketState state)
{
    switch (state) {
        case QAbstractSocket::UnconnectedState:
        emit statusChanged ("Not connected.");
        break;

        case QAbstractSocket::HostLookupState:
        emit statusChanged ("Looking up hostname...");
        break;

        case QAbstractSocket::ConnectingState:
        emit statusChanged ("Establishing connection...");
        break;

        case QAbstractSocket::ConnectedState: {
            emit statusChanged ("Logging in...");

            sendMessage ("LOGIN " + credentials);

            // Wait for SETALL
            socket->waitForReadyRead (10000);

            // Wait for SET FORMULA, BUDDIES, etc.
            socket->waitForReadyRead (10000);

            sendMessage ("SOUGHT");

            socket->waitForReadyRead (10000);

            sendMessage ("RESUME LOGIN");

            emit statusChanged ("Connected.");

            keepAliveTimer.setInterval (KEEP_ALIVE_INTERVAL);
            connect (&keepAliveTimer, SIGNAL (timeout()),
                     SLOT (keepAliveTimeout()));
            keepAliveTimer.start();
        }
        break;

        case QAbstractSocket::BoundState:
        emit statusChanged ("Bound to an address and port.");
        break;

        case QAbstractSocket::ClosingState:
        emit statusChanged ("Disconnecting...");
        break;

        case QAbstractSocket::ListeningState:
        emit statusChanged ("Listening...");
        break;

        default:
        emit statusChanged ("");
        break;
    }
}

//---------------------------------------------------------------------------
//  socketReadyRead
//
//! Called when the socket has data ready to read.
//---------------------------------------------------------------------------
void
IscConnectionThread::socketReadyRead()
{
    QByteArray bytes = socket->readAll();
    QString message = decodeMessage (bytes);

    if (!message.isEmpty())
        receiveMessage (message);
}

//---------------------------------------------------------------------------
//  keepAliveTimeout
//
//! Called when the keep-alive timer goes off.
//---------------------------------------------------------------------------
void
IscConnectionThread::keepAliveTimeout()
{
    sendMessage ("ALIVE");
}

//---------------------------------------------------------------------------
//  encodeMessage
//
//! Encode a message for the server by prepending a null character followed by
//! another byte indicating message length, followed by "0 " and the message.
//
//! @param message the message to encode
//! @return an array of bytes to be sent to the server
//---------------------------------------------------------------------------
QByteArray
IscConnectionThread::encodeMessage (const QString& message)
{
    int length = message.length() + 2;
    QByteArray bytes;
    bytes.append (char (0x00));
    bytes.append (char (length));
    bytes.append (QString ("0 "));
    bytes.append (message);
    return bytes;
}

//---------------------------------------------------------------------------
//  decodeMessage
//
//! Decode a message from the server by removing a null character followed by
//! another byte indicating message length, followed by "0 ".
//
//! @param bytes the bytes to decode
//! @return the decoded message
//---------------------------------------------------------------------------
QString
IscConnectionThread::decodeMessage (const QByteArray& bytes)
{
    int length = bytes[1] - 2;
    return QString (bytes.mid (4, length));
}
