//---------------------------------------------------------------------------
// CreateDatabaseThread.h
//
// A class for creating a database in the background.
//
// Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef ZYZZYVA_CREATE_DATABASE_THREAD_H
#define ZYZZYVA_CREATE_DATABASE_THREAD_H

#include <QThread>
#include <QString>
#include <map>

class WordEngine;

class CreateDatabaseThread : public QThread
{
    Q_OBJECT
    public:
    CreateDatabaseThread (QObject* parent = 0, WordEngine* e,
                          const QString& db, const QString& def)
        : QThread (parent), wordEngine (e), dbFilename (db),
          definitionFilename (def), cancelled (false) { }
    ~CreateDatabaseThread() { }

    bool getCancelled() { return cancelled; }

    public slots:
    void cancel();

    signals:
    void steps (int s);
    void progress (int p);
    void done (bool success);

    protected:
    void run();

    private:
    void runPrivate();

    WordEngine* wordEngine;
    QString dbFilename;
    QString definitionFilename;
    bool cancelled;
};

#endif // ZYZZYVA_CREATE_DATABASE_THREAD_H
