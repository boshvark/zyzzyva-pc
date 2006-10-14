//---------------------------------------------------------------------------
// CreateDatabaseThread.cpp
//
// A class for creating a database in the background.
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

#include "CreateDatabaseThread.h"
#include "LetterBag.h"
#include "WordEngine.h"
#include "Auxil.h"
#include "Defs.h"
#include <QtSql>

using namespace Defs;

//---------------------------------------------------------------------------
//  run
//
//! Create the database.
//---------------------------------------------------------------------------
void
CreateDatabaseThread::run()
{
    runPrivate();
    QSqlDatabase::removeDatabase ("threaddb");
}

//---------------------------------------------------------------------------
//  runPrivate
//
//! Create the database.
//---------------------------------------------------------------------------
void
CreateDatabaseThread::runPrivate()
{
    // Create empty database
    QSqlDatabase db = QSqlDatabase::addDatabase ("QSQLITE", "threaddb");
    db.setDatabaseName (dbFilename);
    if (!db.open())
        return;

    // Start at 1% progress
    emit (steps (100));
    emit (progress (1));

    // Total number of progress steps is number of words times 4.
    // - One for finding each word's stats
    // - One for reading each word's definitions
    // - Two for inserting each word's values into the database
    // - Two total for creating indexes
    int numWords = wordEngine->getNumWords();
    int baseProgress = numWords * 4 / 99;
    int numSteps = numWords * 4 + baseProgress + 1;
    emit steps (numSteps);
    int stepNum = baseProgress;
    emit progress (stepNum);

    createTables (db);

    createIndexes (db);

    insertWords (db, stepNum);

    updateProbabilityOrder (db, stepNum);

    updateDefinitions (db, stepNum);

    emit progress (numSteps);
}

//---------------------------------------------------------------------------
//  createTables
//
//! Create the database tables.
//
//! @param db the database
//---------------------------------------------------------------------------
void
CreateDatabaseThread::createTables (QSqlDatabase& db)
{
    QSqlQuery query (db);

    query.exec ("CREATE TABLE words (word varchar(16), "
        "length integer, combinations integer, probability_order integer, "
        "min_probability_order integer, max_probability_order integer, "
        "alphagram varchar(16), num_anagrams integer, "
        "front_hooks varchar(32), back_hooks varchar(32), "
        "definition varchar(256))");

    query.exec ("CREATE TABLE db_version (version integer)");
    query.exec ("INSERT into db_version (version) VALUES (1)");

    query.exec ("CREATE TABLE lexicon_date (date date)");
    query.prepare ("INSERT into lexicon_date (date) VALUES (?)");
    query.bindValue (0, Auxil::lexiconToDate (lexiconName));
    query.exec();
}

//---------------------------------------------------------------------------
//  createIndexes
//
//! Create the database indexes.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::createIndexes (QSqlDatabase& db)
{
    QSqlQuery query (db);
    query.exec ("CREATE UNIQUE INDEX word_index on words (word)");
    if (cancelled)
        return;
    query.exec ("CREATE INDEX length_index on words (length)");
    if (cancelled)
        return;
}

//---------------------------------------------------------------------------
//  insertWords
//
//! Insert words into the database.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::insertWords (QSqlDatabase& db, int& stepNum)
{
    LetterBag letterBag;
    QStringList letters;
    letters << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" <<
        "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" <<
        "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";

    SearchCondition searchCondition;
    searchCondition.type = SearchCondition::Length;
    SearchSpec searchSpec;
    searchSpec.conditions.append (searchCondition);

    QSqlQuery transactionQuery (db);
    QSqlQuery query (db);

    QMap<QString, int> numAnagramsMap;

    for (int length = 1; length <= MAX_WORD_LEN; ++length) {

        searchSpec.conditions[0].minValue = length;
        searchSpec.conditions[0].maxValue = length;

        QStringList words = wordEngine->search (searchSpec, true);

        query.prepare ("INSERT INTO words (word, length, combinations, "
                       "alphagram, front_hooks, back_hooks) "
                       "VALUES (?, ?, ?, ?, ?, ?)");

        transactionQuery.exec ("BEGIN TRANSACTION");

        // Insert words with length, combinations, hooks
        QString word;
        foreach (word, words) {
            double combinations = letterBag.getNumCombinations (word);

            QString alphagram = Auxil::getAlphagram (word);
            if (numAnagramsMap.contains (alphagram))
                ++numAnagramsMap[alphagram];
            else
                numAnagramsMap[alphagram] = 1;

            QString front, back;
            QString letter;
            foreach (letter, letters) {
                if (wordEngine->isAcceptable (letter + word))
                    front += letter;
                if (wordEngine->isAcceptable (word + letter))
                    back += letter;
            }

            query.bindValue (0, word);
            query.bindValue (1, length);
            query.bindValue (2, combinations);
            query.bindValue (3, alphagram);
            query.bindValue (4, front.toLower());
            query.bindValue (5, back.toLower());
            query.exec();

            if ((stepNum % 1000) == 0) {
                if (cancelled) {
                    transactionQuery.exec ("END TRANSACTION");
                    return;
                }
                emit progress (stepNum);
            }
            ++stepNum;
        }
        transactionQuery.exec ("END TRANSACTION");

        // Update number of anagrams
        query.prepare ("UPDATE words SET num_anagrams=? WHERE word=?");

        transactionQuery.exec ("BEGIN TRANSACTION");
        foreach (word, words) {
            QString alphagram = Auxil::getAlphagram (word);
            query.bindValue (0, numAnagramsMap[alphagram]);
            query.bindValue (1, word);
            query.exec();

            if ((stepNum % 1000) == 0) {
                if (cancelled) {
                    transactionQuery.exec ("END TRANSACTION");
                    return;
                }
                emit progress (stepNum);
            }
            ++stepNum;
        }
        transactionQuery.exec ("END TRANSACTION");
    }
}

//---------------------------------------------------------------------------
//  updateProbabilityOrder
//
//! Update probability order of words in the database.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::updateProbabilityOrder (QSqlDatabase& db, int& stepNum)
{
    QSqlQuery transactionQuery ("BEGIN TRANSACTION", db);

    QSqlQuery selectQuery (db);
    selectQuery.prepare ("SELECT word, combinations FROM words WHERE length=? "
                         "ORDER BY combinations DESC");

    QSqlQuery updateQuery (db);
    updateQuery.prepare ("UPDATE words SET probability_order=?, "
                         "min_probability_order=?, max_probability_order=? "
                         "WHERE word=?");

    for (int length = 1; length <= MAX_WORD_LEN; ++length) {
        selectQuery.bindValue (0, length);
        selectQuery.exec();

        QMap<QString, QString> equalWordMap;

        int probOrder = 1;
        int minProbOrder = 1;
        double prevCombinations = 0;

        while (selectQuery.next()) {
            QString word = selectQuery.value (0).toString();
            double combinations = selectQuery.value (1).toDouble();

            // Update probability ranges
            if ((combinations != prevCombinations) && !equalWordMap.empty()) {
                int maxProbOrder = minProbOrder + equalWordMap.size() - 1;
                QMapIterator<QString, QString> it (equalWordMap);
                while (it.hasNext()) {
                    it.next();
                    QString equalWord = it.value();

                    // Update probability order and range of this word
                    updateQuery.bindValue (0, probOrder);
                    updateQuery.bindValue (1, minProbOrder);
                    updateQuery.bindValue (2, maxProbOrder);
                    updateQuery.bindValue (3, equalWord);
                    updateQuery.exec();

                    if ((stepNum % 1000) == 0) {
                        transactionQuery.exec ("END TRANSACTION");
                        if (cancelled)
                            return;
                        transactionQuery.exec ("BEGIN TRANSACTION");
                        emit progress (stepNum);
                    }

                    ++probOrder;
                    ++stepNum;
                }
                minProbOrder = probOrder;
                equalWordMap.clear();
            }

            // Sort words by alphagram
            QString radix = Auxil::getAlphagram (word) + word;
            equalWordMap.insert (radix, word);

            prevCombinations = combinations;
        }

        int maxProbOrder = minProbOrder + equalWordMap.size() - 1;
        QMapIterator<QString, QString> it (equalWordMap);
        while (it.hasNext()) {
            it.next();
            QString equalWord = it.value();
            updateQuery.bindValue (0, probOrder);
            updateQuery.bindValue (1, minProbOrder);
            updateQuery.bindValue (2, maxProbOrder);
            updateQuery.bindValue (3, equalWord);
            updateQuery.exec();
        }
    }

    transactionQuery.exec ("END TRANSACTION");
}

//---------------------------------------------------------------------------
//  updateDefinitions
//
//! Update definitions of words in the database.
//
//! @param db the database
//! @param stepNum the current step number
//---------------------------------------------------------------------------
void
CreateDatabaseThread::updateDefinitions (QSqlDatabase& db, int& stepNum)
{
    QSqlQuery transactionQuery ("BEGIN TRANSACTION", db);

    QSqlQuery query (db);
    query.prepare ("UPDATE words SET definition=? WHERE word=?");

    QMap<QString, QString> definitionMap;
    QFile definitionFile (definitionFilename);
    if (definitionFile.open (QIODevice::ReadOnly | QIODevice::Text)) {
        char* buffer = new char [MAX_INPUT_LINE_LEN];
        while (definitionFile.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
            QString line (buffer);
            line = line.simplified();
            if (!line.length() || (line.at (0) == '#')) {
                if ((stepNum % 1000) == 0) {
                    transactionQuery.exec ("END TRANSACTION");
                    if (cancelled) {
                        delete buffer;
                        return;
                    }
                    transactionQuery.exec ("BEGIN TRANSACTION");
                    emit progress (stepNum);
                }
                ++stepNum;
                continue;
            }
            QString word = line.section (' ', 0, 0).toUpper();
            QString definition = line.section (' ', 1);

            query.bindValue (0, definition);
            query.bindValue (1, word);
            query.exec();

            if ((stepNum % 1000) == 0) {
                transactionQuery.exec ("END TRANSACTION");
                if (cancelled) {
                    delete buffer;
                    return;
                }
                transactionQuery.exec ("BEGIN TRANSACTION");
                emit progress (stepNum);
            }
            ++stepNum;
        }
        delete buffer;
    }

    transactionQuery.exec ("END TRANSACTION");
}

//---------------------------------------------------------------------------
//  cancel
//
//! Cancel the creation of this database.  Set the cancelled flag so the
//! operation will be stopped and the database will be deleted.
//---------------------------------------------------------------------------
void
CreateDatabaseThread::cancel()
{
    cancelled = true;
}
