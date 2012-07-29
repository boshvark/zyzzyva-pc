//---------------------------------------------------------------------------
// CreateDatabaseThread.h
//
// A class for creating a database in the background.
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

#ifndef ZYZZYVA_CREATE_DATABASE_THREAD_H
#define ZYZZYVA_CREATE_DATABASE_THREAD_H

#include <QMap>
#include <QString>
#include <QSqlDatabase>
#include <QThread>

class WordEngine;

class CreateDatabaseThread : public QThread
{
    Q_OBJECT
    public:
    CreateDatabaseThread(WordEngine* e, const QString& lex, const QString& db,
                         const QString& def, QObject* parent = 0)
        : QThread(parent), wordEngine(e), lexiconName(lex),
          dbFilename(db), definitionFilename(def), cancelled(false) { }
    ~CreateDatabaseThread() { }

    bool getCancelled() { return cancelled; }
    QString getError() { return error; }

    public slots:
    void cancel();
    void cleanup();

    signals:
    void steps(int s);
    void progress(int p);
    void done(bool success);

    protected:
    void run();

    private:
    void runPrivate();
    void createTables(QSqlDatabase& db);
    void createIndexes(QSqlDatabase& db);
    void insertVersion(QSqlDatabase& db);
    void insertWords(QSqlDatabase& db, int& stepNum);
    void updatePlayabilityOrder(QSqlDatabase& db, int& stepNum);
    void updateProbabilityOrder(QSqlDatabase& db, int& stepNum);
    void updateDefinitions(QSqlDatabase& db, int& stepNum);
    void updateDefinitionLinks(QSqlDatabase& db, int& stepNum);

    void getDefinitions(QSqlDatabase& db, int& stepNum);
    QString replaceDefinitionLinks(const QString& definition, int maxDepth,
        QSet<QString>* alreadyReplaced = 0, bool useFollow = false) const;
    QString getSubDefinition(const QString& word, const QString& pos) const;
    int importPlayability(const QString& filename, QMap<QString, qint64>&
                          playabilityMap) const;

    WordEngine* wordEngine;
    QString lexiconName;
    QString dbFilename;
    QString definitionFilename;
    bool cancelled;
    QString error;
    QMap<QString, QString> definitions;
};

#endif // ZYZZYVA_CREATE_DATABASE_THREAD_H
