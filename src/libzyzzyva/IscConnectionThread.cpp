//---------------------------------------------------------------------------
// IscConnectionThread.cpp
//
// A class for managing an ISC connection in the background.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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

const int KEEP_ALIVE_INTERVAL = 31000;

//---------------------------------------------------------------------------
//  ~IscConnectionThread
//
//! Destructor.
//---------------------------------------------------------------------------
IscConnectionThread::~IscConnectionThread()
{
    disconnectFromServer();
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
IscConnectionThread::connectToServer(const QString& creds,
                                     QAbstractSocket::SocketError* err)
{
    credentials = creds;
    socket = new QTcpSocket(this);
    Q_CHECK_PTR(socket);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            SIGNAL(socketError(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));

    // Connect to a random port between 1321 and 1330
    Rand rng (Rand::MarsagliaMwc, QDateTime::currentDateTime().toTime_t(),
              Auxil::getPid());
    int port = 1321 + rng.rand(9);
    socket->connectToHost("66.98.172.34", port);

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

    if (socket->isValid())
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
IscConnectionThread::sendMessage(const QString& message)
{
    if (!socket)
        return;

    QString command = message.section(" ", 0, 0).toUpper();
    QString args = message.section(" ", 1);

    if (command == "SET") {
        QString subcommand = args.section(" ", 0, 0).toUpper();
        command += " " + subcommand;
        args = args.section(" ", 1);

        if (subcommand == "FINGER") {
            int index = args.section(" ", 0, 0).toInt() - 1;
            command += " " + QString::number(index);
            args = args.section(" ", 1);
        }
    }

    socket->write(encodeMessage(command + " " + args));
}

//---------------------------------------------------------------------------
//  receiveMessage
//
//! Receive a message from the server.
//
//! @param message the message
//---------------------------------------------------------------------------
void
IscConnectionThread::receiveMessage(const QString& message)
{
    if (message == "PING REPLY")
        sendMessage("PING REPLY");
    else
        emit messageReceived(message);
}

//---------------------------------------------------------------------------
//  socketStateChanged
//
//! Called when the socket state changes.
//---------------------------------------------------------------------------
void
IscConnectionThread::socketStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
        case QAbstractSocket::UnconnectedState:
        emit statusChanged("Not connected.");
        break;

        case QAbstractSocket::HostLookupState:
        emit statusChanged("Looking up hostname...");
        break;

        case QAbstractSocket::ConnectingState:
        emit statusChanged("Establishing connection...");
        break;

        case QAbstractSocket::ConnectedState: {
            emit statusChanged("Logging in...");

            sendMessage("LOGIN " + credentials);

            // Wait for SETALL
            socket->waitForReadyRead(10000);

            // Wait for SET FORMULA, BUDDIES, etc.
            socket->waitForReadyRead(10000);

            sendMessage("SOUGHT");

            socket->waitForReadyRead(10000);

            sendMessage("RESUME LOGIN");

            emit statusChanged("Connected.");

            keepAliveTimer.setInterval(KEEP_ALIVE_INTERVAL);
            connect(&keepAliveTimer, SIGNAL(timeout()),
                    SLOT(keepAliveTimeout()));
            keepAliveTimer.start();
        }
        break;

        case QAbstractSocket::BoundState:
        emit statusChanged("Bound to an address and port.");
        break;

        case QAbstractSocket::ClosingState:
        emit statusChanged("Disconnecting...");
        break;

        case QAbstractSocket::ListeningState:
        emit statusChanged("Listening...");
        break;

        default:
        emit statusChanged(QString());
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
    QStringList messages = decodeMessage(bytes);
    foreach (const QString& message, messages) {
        receiveMessage(message);
    }
}

//---------------------------------------------------------------------------
//  keepAliveTimeout
//
//! Called when the keep-alive timer goes off.
//---------------------------------------------------------------------------
void
IscConnectionThread::keepAliveTimeout()
{
    sendMessage("ALIVE");
}

//---------------------------------------------------------------------------
//  encodeMessage
//
//! Encode a message for the server by prepending two bytes indicating message
//! length, followed by "0 " and the message.
//
//! @param message the message to encode
//! @return an array of bytes to be sent to the server
//---------------------------------------------------------------------------
QByteArray
IscConnectionThread::encodeMessage(const QString& message)
{
    int length = message.length() + 2;
    unsigned char high = (length & 0xff00) >> 8;
    unsigned char low  = (length & 0x00ff);

    QByteArray bytes;
    bytes.append(high);
    bytes.append(low);
    bytes.append(QString("0 "));
    bytes.append(message);
    return bytes;
}

//---------------------------------------------------------------------------
//  decodeMessage
//
//! Decode a message from the server by interpreting the first two bytes as
//! message length, followed by the message itself.  More than one message may
//! be present in the data stream, so return a list of all messages found.
//
//! @param bytes the bytes to decode
//! @return the decoded messages
//---------------------------------------------------------------------------
QStringList
IscConnectionThread::decodeMessage(const QByteArray& bytes)
{
    int index = 0;
    QStringList messages;

    while (index < bytes.size()) {
        unsigned char high = bytes[index];
        unsigned char low  = bytes[index + 1];
        int length = (high << 8) + low - 2;
        messages.append(QString(bytes.mid(index + 4, length)));
        index += length + 4;
    }

    return messages;
}
