//---------------------------------------------------------------------------
// IscConnectionThread.h
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

#ifndef ZYZZYVA_ISC_CONNECTION_THREAD_H
#define ZYZZYVA_ISC_CONNECTION_THREAD_H

#include <QStringList>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

class IscConnectionThread : public QThread
{
    Q_OBJECT
    public:
    IscConnectionThread(QObject* parent = 0)
        : QThread(parent), socket(0) { }
    ~IscConnectionThread();

    bool connectToServer(const QString& creds,
                         QAbstractSocket::SocketError* err = 0);
    void disconnectFromServer();

    signals:
    void messageReceived(const QString& message);
    void statusChanged(const QString& status);
    void socketError(QAbstractSocket::SocketError error);

    public slots:
    void sendMessage(const QString& message);
    void receiveMessage(const QString& message);

    private slots:
    void socketStateChanged(QAbstractSocket::SocketState state);
    void socketReadyRead();
    void keepAliveTimeout();
    QByteArray encodeMessage(const QString& message);
    QStringList decodeMessage(const QByteArray& bytes);

    protected:
    void run();

    private:
    QTcpSocket* socket;
    QTimer keepAliveTimer;
    QString credentials;
    bool socketHadError;
};

#endif // ZYZZYVA_ISC_CONNECTION_THREAD_H
