//---------------------------------------------------------------------------
// QuizDatabase.cpp
//
// A class for working with database of quiz performance statistics.
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

#include "QuizDatabase.h"
#include "Rand.h"
#include "Auxil.h"
#include <QDir>
#include <QSqlQuery>
#include <QVariant>
#include <ctime>

#include <QtDebug>

const QString SQL_CREATE_QUESTIONS_TABLE_0_14_0 =
    "CREATE TABLE questions (question varchar(16), correct integer, "
    "incorrect integer, streak integer, last_correct integer, "
    "difficulty integer)";

const QString SQL_CREATE_QUESTIONS_TABLE_CURRENT =
    "CREATE TABLE questions (question varchar(16), correct integer, "
    "incorrect integer, streak integer, last_correct integer, "
    "difficulty integer, cardbox integer, next_scheduled integer)";

//---------------------------------------------------------------------------
//  QuizDatabase
//
//! Constructor.  Connect to the database specified by a lexicon and quiz
//! type.
//
//! @param lexicon the lexicon name
//! @param quizType the quiz type
//---------------------------------------------------------------------------
QuizDatabase::QuizDatabase (const QString& lexicon, const QString& quizType)
    : db (0)
{
    QString dirName = Auxil::getQuizDir() + "/data/" + lexicon;
    QDir dir (dirName);
    if (!dir.exists() && !dir.mkpath (dirName)) {
        qWarning ("Cannot create quiz stats directory\n");
        return;
    }

    QString dbFilename = dirName + "/" + quizType + ".db";

    // Get random connection name
    Rand rng (Rand::MarsagliaMwc, std::time (0), Auxil::getPid());
    unsigned int r = rng.rand();
    dbConnectionName = "quiz" + QString::number (r);
    db = new QSqlDatabase (QSqlDatabase::addDatabase ("QSQLITE",
                                                      dbConnectionName));
    db->setDatabaseName (dbFilename);
    if (!db->open())
        return;

    updateSchema();
}

//---------------------------------------------------------------------------
//  QuizDatabase
//
//! Destructor.
//---------------------------------------------------------------------------
QuizDatabase::~QuizDatabase()
{
    if (db) {
        if (db->isOpen())
            db->close();
        delete db;
        QSqlDatabase::removeDatabase (dbConnectionName);
    }
}

//---------------------------------------------------------------------------
//  isValid
//
//! Determine whether the database connection is valid.
//
//! @return true if the database is valid, false otherwise
//---------------------------------------------------------------------------
bool
QuizDatabase::isValid() const
{
    return (db && db->isValid());
}

//---------------------------------------------------------------------------
//  updateSchema
//
//! Update the quiz tables to the latest schema.
//
//! @return true if the database is valid, false otherwise
//---------------------------------------------------------------------------
bool
QuizDatabase::updateSchema()
{
    // Create table if it doesn't already exist
    QSqlQuery query (*db);
    query.exec ("SELECT name FROM sqlite_master WHERE type='table' "
                "AND name='questions'");
    if (!query.next()) {
        query.exec (SQL_CREATE_QUESTIONS_TABLE_CURRENT);
    }

    // The table already exists, so update the schema
    else {
        query.exec ("SELECT sql FROM sqlite_master WHERE type='table' "
                    "AND name='questions'");
        if (!query.next())
            return false;
        QString sql = query.value (0).toString();

        // Upgrade from 0.14.0 schema to 1.0.0 schema
        if (sql == SQL_CREATE_QUESTIONS_TABLE_0_14_0) {
            query.exec ("ALTER TABLE questions ADD COLUMN cardbox integer");
            query.exec ("ALTER TABLE questions ADD COLUMN "
                        "next_scheduled integer");
        }
    }

    // Create index on question column of questions table
    query.exec ("SELECT name FROM sqlite_master WHERE type='index' "
                "AND name='question_index' AND tbl_name='questions'");
    if (!query.next()) {
        query.exec ("CREATE UNIQUE INDEX question_index ON questions "
                    "(question)");
    }

    return true;
}

//---------------------------------------------------------------------------
//  recordResponse
//
//! Record a correct or incorrect question response.
//
//! @param question the question
//! @param correct whether the response was correct
//---------------------------------------------------------------------------
void
QuizDatabase::recordResponse (const QString& question, bool correct)
{
    QSqlQuery query (*db);
    query.prepare ("SELECT correct, incorrect, streak, last_correct, "
                   "difficulty FROM questions WHERE question=?");
    query.bindValue (0, question);
    query.exec();

    if (query.next()) {
        int numCorrect = query.value (0).toInt();
        int numIncorrect = query.value (1).toInt();
        int streak = query.value (2).toInt();
        int lastCorrect = query.value (3).toInt();
        int difficulty = query.value (4).toInt();

        if (correct) {
            ++numCorrect;
            if (streak < 0)
                streak = 1;
            else
                ++streak;
            lastCorrect = std::time (0);
        }
        else {
            ++numIncorrect;
            if (streak > 0)
                streak = -1;
            else
                --streak;
        }

        query.prepare ("UPDATE questions SET correct=?, incorrect=?, "
                       "streak=?, last_correct=?, difficulty=? "
                       "WHERE question=?");
        query.bindValue (0, numCorrect);
        query.bindValue (1, numIncorrect);
        query.bindValue (2, streak);
        query.bindValue (3, lastCorrect);
        // XXX: Fix difficulty ratings!
        query.bindValue (4, difficulty);
        query.bindValue (5, question);
        query.exec();
    }

    else {
        int numCorrect = (correct ? 1 : 0);
        int numIncorrect = (correct ? 0 : 1);
        int streak = (correct ? 1 : -1);
        int lastCorrect = (correct ? std::time (0) : 0);
        // XXX: Fix difficulty ratings!
        int difficulty = 50;
        query.prepare ("INSERT INTO questions (question, correct, "
                       "incorrect, streak, last_correct, difficulty) "
                       "VALUES (?, ?, ?, ?, ?, ?)");
        query.bindValue (0, question);
        query.bindValue (1, numCorrect);
        query.bindValue (2, numIncorrect);
        query.bindValue (3, streak);
        query.bindValue (4, lastCorrect);
        query.bindValue (5, difficulty);
        query.exec();
    }
}
