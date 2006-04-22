//---------------------------------------------------------------------------
// CreateDatabaseThread.cpp
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

    if (cancelled) {
        QFile dbFile (dbFilename);
        dbFile.remove();
    }
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

    // Find all words in the lexicon
    SearchCondition searchCondition;
    searchCondition.type = SearchCondition::PatternMatch;
    searchCondition.stringValue = "*";
    SearchSpec searchSpec;
    searchSpec.conditions.append (searchCondition);

    QStringList allWords = wordEngine->search (searchSpec, true);

    // Total number of progress steps is number of words times 4.
    // - One for finding each word's stats
    // - One for reading each word's definitions
    // - Two for inserting each word's values into the database
    // - Two total for creating indexes and cleaning up
    int numWords = allWords.size();
    int numSteps = numWords * 6 + 1;
    emit steps (numSteps);
    int stepNum = 0;

    LetterBag letterBag;
    QSet<QString> wordSet;
    QMap<int, QMultiMap<double, QString> > wordMap;
    QMap<QString, int> numAnagramsMap;

    // STEP 1: Find word stats
    QString word;
    foreach (word, allWords) {
        wordSet.insert (word);

        QMultiMap<double, QString>& comboMap = wordMap[word.length()];
        comboMap.insert (letterBag.getNumCombinations (word), word);

        QString alphagram = Auxil::getAlphagram (word);
        if (numAnagramsMap.contains (alphagram))
            ++numAnagramsMap[alphagram];
        else
            numAnagramsMap[alphagram] = 1;

        if ((stepNum % 1000) == 0) {
            if (cancelled)
                return;
            emit progress (stepNum);
        }
        ++stepNum;
    }
    stepNum = numWords;
    emit progress (stepNum);

    // STEP 2: Read word definitions
    QMap<QString, QString> definitionMap;
    QFile definitionFile (definitionFilename);
    if (definitionFile.open (QIODevice::ReadOnly | QIODevice::Text)) {
        char* buffer = new char [MAX_INPUT_LINE_LEN];
        while (definitionFile.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
            QString line (buffer);
            line = line.simplified();
            if (!line.length() || (line.at (0) == '#'))
                continue;
            QString word = line.section (' ', 0, 0).toUpper();
            QString definition = line.section (' ', 1);
            definitionMap[word] = definition;

            if ((stepNum % 1000) == 0) {
                if (cancelled) {
                    delete buffer;
                    return;
                }
                emit progress (stepNum);
            }
            ++stepNum;
        }
        delete buffer;
    }
    stepNum = numWords * 2;
    emit progress (stepNum);

    QSqlQuery transactionQuery ("BEGIN TRANSACTION", db);

    QString qstr = "CREATE TABLE words (word varchar(16), "
        "length integer, combinations integer, probability_order integer, "
        "alphagram varchar(16), num_anagrams integer, "
        "front_hooks varchar(32), back_hooks varchar(32), "
        "definition varchar(256))";

    QSqlQuery query (qstr, db);

    query.prepare ("INSERT INTO words (word, length, combinations, "
                    "probability_order, alphagram, num_anagrams, "
                    "front_hooks, back_hooks, definition) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

    QStringList letters;
    letters << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" <<
        "I" << "J" << "K" << "L" << "M" << "N" << "O" << "P" << "Q" <<
        "R" << "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";

    // STEP 3: Insert values into the database
    int numInserts = 0;
    QMapIterator<int, QMultiMap<double, QString> > it (wordMap);
    while (it.hasNext()) {
        int probOrder = 1;
        it.next();
        int length = it.key();
        QMultiMap<double, QString>& comboMap = wordMap[length];

        QMapIterator<double, QString> jt (comboMap);
        jt.toBack();
        while (jt.hasPrevious()) {
            jt.previous();
            double combinations = jt.key();
            QString word = jt.value();

            QString front, back;
            QString letter;
            foreach (letter, letters) {
                if (wordSet.contains (letter + word))
                    front += letter;
                if (wordSet.contains (word + letter))
                    back += letter;
            }

            QString alphagram = Auxil::getAlphagram (word);
            QString definition;
            if (definitionMap.contains (word))
                definition = definitionMap[word];

            query.bindValue (0, word);
            query.bindValue (1, length);
            query.bindValue (2, combinations);
            query.bindValue (3, probOrder);
            query.bindValue (4, alphagram);
            query.bindValue (5, numAnagramsMap[alphagram]);
            query.bindValue (6, front.toLower());
            query.bindValue (7, back.toLower());
            query.bindValue (8, definition);
            query.exec();

            if ((numInserts % 1000) == 0) {
                transactionQuery.exec ("END TRANSACTION");
                if (cancelled)
                    return;
                transactionQuery.exec ("BEGIN TRANSACTION");
                emit progress (stepNum);
            }
            stepNum += 2;
            ++numInserts;
            ++probOrder;
        }
    }

    transactionQuery.exec ("END TRANSACTION");
    stepNum = numWords * 4;
    if (cancelled)
        return;
    emit progress (stepNum);

    // STEP 4: Create indexes and vacuum
    query.exec ("CREATE UNIQUE INDEX word_index on words (word)");
    stepNum += (2 * numWords / 3);
    if (cancelled)
        return;
    emit progress (stepNum);
    query.exec ("CREATE INDEX length_index on words (length)");
    stepNum += (2 * numWords / 3);
    if (cancelled)
        return;
    emit progress (stepNum);
    query.exec ("VACUUM");
    stepNum += (2 * numWords / 3);
    emit progress (numSteps);
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
