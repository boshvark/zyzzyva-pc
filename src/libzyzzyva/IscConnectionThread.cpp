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
    connect (socket, SIGNAL (bytesWritten (qint64)),
             SLOT (socketBytesWritten (qint64)));

    Rand r;
    int port = 1321 + r.rand (9);
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
    qDebug() << "*** sendMessage: " << message;
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

            QByteArray bytes;
            QString message;

            message = "0 LOGIN " + credentials;
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
        emit messageReceived (message);

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
IscConnectionThread::socketBytesWritten (qint64 bytes)
{
    qDebug() << "*** socketBytesWritten: " << bytes << " bytes";
}

//---------------------------------------------------------------------------
//  keepAliveTimeout
//
//! Called when the keep-alive timer goes off.
//---------------------------------------------------------------------------
void
IscConnectionThread::keepAliveTimeout()
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
