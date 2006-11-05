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

#include <QSqlError>

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
    rng.srand (std::time (0), Auxil::getPid());
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
//! @param updateCardbox whether to update the question in the cardbox system
//---------------------------------------------------------------------------
void
QuizDatabase::recordResponse (const QString& question, bool correct,
                              bool updateCardbox)
{
    QuestionData data = getQuestionData (question);

    if (data.valid) {
        if (correct) {
            ++data.numCorrect;
            if (data.streak < 0)
                data.streak = 1;
            else
                ++data.streak;
            data.lastCorrect = std::time (0);
        }
        else {
            ++data.numIncorrect;
            if (data.streak > 0)
                data.streak = -1;
            else
                --data.streak;
        }
        if (updateCardbox) {
            data.cardbox = correct ? data.cardbox + 1 : 0;
            data.nextScheduled = calculateNextScheduled (data.cardbox);
        }
    }

    else {
        data = QuestionData();
        data.valid = true;
        data.numCorrect = (correct ? 1 : 0);
        data.numIncorrect = (correct ? 0 : 1);
        data.streak = (correct ? 1 : -1);
        data.lastCorrect = (correct ? std::time (0) : 0);
        // XXX: Fix difficulty ratings!
        data.difficulty = 50;
        if (updateCardbox) {
            data.cardbox = correct ? 1 : 0;
            data.nextScheduled = calculateNextScheduled (data.cardbox);
        }
    }

    setQuestionData (question, data, updateCardbox);
}

//---------------------------------------------------------------------------
//  addToCardbox
//
//! Add a list of questions to the cardbox system, possibly estimating the
//! cardboxes they should be in based on past performance.
//
//! @param question the question to add to the cardbox system
//! @param estimateCardbox whether to estimate a cardbox based on past
//! performance
//---------------------------------------------------------------------------
void
QuizDatabase::addToCardbox (const QStringList& questions, bool estimateCardbox)
{
    QSqlQuery query (*db);
    query.exec ("BEGIN TRANSACTION");
    QStringListIterator it (questions);
    while (it.hasNext()) {
        addToCardbox (it.next(), estimateCardbox);
    }
    query.exec ("COMMIT TRANSACTION");
}

//---------------------------------------------------------------------------
//  addToCardbox
//
//! Add a question to the cardbox system, possibly estimating the cardbox it
//! should be in based on past performance.
//
//! @param question the question to add to the cardbox system
//! @param estimateCardbox whether to estimate a cardbox based on past
//! performance
//---------------------------------------------------------------------------
void
QuizDatabase::addToCardbox (const QString& question, bool estimateCardbox)
{
    QuestionData data = getQuestionData (question);

    if (data.valid) {
        // Question is already in the cardbox system, so leave it alone
        if (data.cardbox >= 0) {
            return;
        }

        if (estimateCardbox && (data.streak > 0))
            data.cardbox = data.streak;
        else
            data.cardbox = 0;
    }

    else {
        data = QuestionData();
        data.valid = true;
        data.cardbox = 0;
    }

    // Move scheduled time back by 16 hours, so questions in cardbox 0 will be
    // available immediately
    data.nextScheduled = calculateNextScheduled (data.cardbox);
    data.nextScheduled -= 60 * 60 * 16;

    setQuestionData (question, data, true);
}

//---------------------------------------------------------------------------
//  removeFromCardbox
//
//! Remove a list of questions from the cardbox system.
//
//! @param questions the questions to remove from the cardbox system
//---------------------------------------------------------------------------
void
QuizDatabase::removeFromCardbox (const QStringList& questions)
{
    QStringList qlist;
    QStringListIterator it (questions);
    while (it.hasNext()) {
        qlist.append ("'" + it.next() + "'");
    }
    QString queryStr = "UPDATE questions SET cardbox=NULL, "
        "next_scheduled=NULL WHERE question IN (" +
        qlist.join (", ") + ")";

    QSqlQuery query (*db);
    query.prepare (queryStr);
    query.exec();
}

//---------------------------------------------------------------------------
//  removeFromCardbox
//
//! Remove a question from the cardbox system.
//
//! @param question the question to remove from the cardbox system
//---------------------------------------------------------------------------
void
QuizDatabase::removeFromCardbox (const QString& question)
{
    QStringList qlist;
    qlist.append (question);
    removeFromCardbox (qlist);
}

//---------------------------------------------------------------------------
//  setCardbox
//
//! Place a question into a cardbox.  Add it to the cardbox system if
//! necessary.
//
//! @param question the question
//! @param cardbox the cardbox to place the question in
//---------------------------------------------------------------------------
void
QuizDatabase::setCardbox (const QString& question, int cardbox)
{
    QuestionData data = getQuestionData (question);
    data.valid = true;
    data.cardbox = cardbox;
    data.nextScheduled = calculateNextScheduled (data.cardbox);
    setQuestionData (question, data, true);
}

//---------------------------------------------------------------------------
//  rescheduleCardboxAll
//
//! Reschedule questions in the cardbox system.  If no questions are
//! specified, then all questions are rescheduled.
//
//! @param questions the list of questions to reschedule
//---------------------------------------------------------------------------
int
QuizDatabase::rescheduleCardbox (const QStringList& questions)
{
    QString queryStr = "SELECT question, cardbox, next_scheduled "
        "FROM questions WHERE cardbox NOT NULL";

    if (!questions.isEmpty()) {
        queryStr += " AND question IN (";
        QStringListIterator it (questions);
        bool firstItem = true;
        while (it.hasNext()) {
            QString question = it.next();
            if (!firstItem)
                queryStr += ", ";
            queryStr += "'" + question + "'";
            firstItem = false;
        }
        queryStr += ")";
    }

    QSqlQuery query (*db);
    query.prepare (queryStr);
    query.exec();

    QList<QString> selectedQuestions;
    QList<int> selectedCardboxes;

    while (query.next()) {
        selectedQuestions.append (query.value (0).toString());
        selectedCardboxes.append (query.value (1).toInt());
    }

    QSqlQuery updateQuery (*db);
    updateQuery.prepare ("UPDATE questions SET next_scheduled=? "
                         "WHERE question=?");

    QSqlQuery transactionQuery ("BEGIN TRANSACTION", *db);

    QListIterator<QString> it (selectedQuestions);
    QListIterator<int> jt (selectedCardboxes);
    while (it.hasNext()) {
        QString question = it.next();
        int cardbox = jt.next();
        int nextScheduled = calculateNextScheduled (cardbox);
        nextScheduled -= 60 * 60 * 16;

        updateQuery.bindValue (0, nextScheduled);
        updateQuery.bindValue (1, question);
        updateQuery.exec();
    }

    transactionQuery.exec ("END TRANSACTION");

    return selectedQuestions.size();
}

//---------------------------------------------------------------------------
//  getReadyQuestions
//
//! Get a list of questions that are ready for review, from a subset of
//! possible questions.  The questions are returned in their scheduled order.
//
//! @param questions the list of possible questions
//! @return the list of ready questions, in scheduled order
//---------------------------------------------------------------------------
QStringList
QuizDatabase::getReadyQuestions (const QStringList& questions)
{
    QStringList readyQuestions;

    unsigned int now = std::time (0);

    QString inString;
    QString question;
    foreach (question, questions) {
        if (inString.isEmpty())
            inString += "(";
        else
            inString += ",";
        inString += "'" + question + "'";
    }
    inString += ")";

    QString queryStr = "SELECT question FROM questions WHERE "
        "next_scheduled <= " + QString::number (now) + " AND "
        "question IN " + inString + " ORDER BY next_scheduled";

    QSqlQuery query (*db);
    query.prepare (queryStr);
    query.exec();

    while (query.next()) {
        readyQuestions.append (query.value (0).toString());
    }

    return readyQuestions;
}

//---------------------------------------------------------------------------
//  getQuestionData
//
//! Retrieve information about a question from the database.
//
//! @param question the question
//! @return the associated data
//---------------------------------------------------------------------------
QuizDatabase::QuestionData
QuizDatabase::getQuestionData (const QString& question)
{
    QuestionData data;

    QSqlQuery query (*db);
    query.prepare ("SELECT correct, incorrect, streak, last_correct, "
                   "difficulty, cardbox, next_scheduled "
                   "FROM questions WHERE question=?");
    query.bindValue (0, question);
    query.exec();

    if (query.next()) {
        QVariant variant = query.value (0);
        if (!variant.isNull())
            data.numCorrect = variant.toInt();

        variant = query.value (1);
        if (!variant.isNull())
            data.numIncorrect = variant.toInt();

        variant = query.value (2);
        if (!variant.isNull())
            data.streak = variant.toInt();

        variant = query.value (3);
        if (!variant.isNull())
            data.lastCorrect = variant.toInt();

        variant = query.value (4);
        if (!variant.isNull())
            data.difficulty = variant.toInt();

        variant = query.value (5);
        if (!variant.isNull())
            data.cardbox = variant.toInt();

        variant = query.value (6);
        if (!variant.isNull())
            data.nextScheduled = variant.toInt();

        data.valid = true;
    }

    return data;
}

//---------------------------------------------------------------------------
//  calculateNextScheduled
//
//! Calculate the next scheduled appearance of a question given the question's
//! new cardbox.
//
//! @param cardbox the cardbox number
//! @return the next scheduled appearance of the question
//---------------------------------------------------------------------------
int
QuizDatabase::calculateNextScheduled (int cardbox)
{
    int daySeconds = 60 * 60 * 24;
    int halfDaySeconds = daySeconds / 2;
    int numDays = 0;
    int randDays = 0;
    switch (cardbox) {
        case 0: numDays = 1; randDays = 0; break;
        case 1: numDays = 4; randDays = 1; break;
        case 2: numDays = 7; randDays = 2; break;
        case 3: numDays = 12; randDays = 3; break;
        case 4: numDays = 20; randDays = 5; break;
        case 5: numDays = 30; randDays = 7; break;
        case 6: numDays = 60; randDays = 10; break;
        case 7: numDays = 90; randDays = 15; break;
        case 8: numDays = 150; randDays = 20; break;
        case 9: numDays = 270; randDays = 30; break;
        case 10:
        default: numDays = 480; randDays = 50; break;
    }

    // Get the next scheduled time and perturb it randomly.  First, add or
    // subtract a random number of days so we don't get a bunch of questions
    // all at once on some future date.  Then add or subtract a random number
    // of seconds within 12 hours, so the future question order is somewhat
    // randomized, and each question is equally likely to occur anytime during
    // the day.  For cardbox 0, the window is only 4 hours, so the question is
    // scheduled between 8 and 16 hours in the future.
    if (randDays)
        numDays += rng.rand (randDays * 2) - randDays;
    unsigned int now = std::time (0);
    int nextSeconds = (daySeconds * numDays) - halfDaySeconds;
    int halfWindow = cardbox ? halfDaySeconds : 60 * 60 * 4;
    int randSeconds = rng.rand (2 * halfWindow) - halfWindow;
    int nextScheduled = now + nextSeconds + randSeconds;
    return nextScheduled;
}

//---------------------------------------------------------------------------
//  setQuestionData
//
//! Update information about a question in the database.
//
//! @param question the question
//! @param data the new data
//! @param updateCardbox whether to update the cardbox information
//---------------------------------------------------------------------------
void
QuizDatabase::setQuestionData (const QString& question, const QuestionData&
                               data, bool updateCardbox)
{
    QSqlQuery query (*db);
    query.prepare ("SELECT question FROM questions WHERE question=?");
    query.bindValue (0, question);
    query.exec();

    // Question data already exists, so update it
    if (query.next()) {
        int questionBindNum = 5;
        QString queryStr = "UPDATE questions SET correct=?, incorrect=?, "
            "streak=?, last_correct=?, difficulty=? ";
        if (updateCardbox) {
            queryStr += ", cardbox=?, next_scheduled=? ";
            questionBindNum = 7;
        }
        queryStr += "WHERE question=?";

        query.prepare (queryStr);
        query.bindValue (0, data.numCorrect);
        query.bindValue (1, data.numIncorrect);
        query.bindValue (2, data.streak);
        query.bindValue (3, data.lastCorrect);
        // XXX: Fix difficulty ratings!
        query.bindValue (4, data.difficulty);

        if (updateCardbox) {
            if (data.cardbox >= 0) {
                query.bindValue (5, data.cardbox);
                query.bindValue (6, data.nextScheduled);
            }
            else {
                query.bindValue (5, QVariant());
                query.bindValue (6, QVariant());
            }
            questionBindNum = 7;
        }

        query.bindValue (questionBindNum, question);
        query.exec();
    }

    // Question data does not exist, so insert it
    else {
        QString queryStr = "INSERT INTO questions (question, correct, "
                           "incorrect, streak, last_correct, difficulty";
        if (updateCardbox) {
            queryStr += ", cardbox, next_scheduled";
        }
        queryStr += ") VALUES (?, ?, ?, ?, ?, ?";
        if (updateCardbox) {
            queryStr += ", ?, ?";
        }
        queryStr += ")";

        query.prepare (queryStr);
        query.bindValue (0, question);
        query.bindValue (1, data.numCorrect);
        query.bindValue (2, data.numIncorrect);
        query.bindValue (3, data.streak);
        query.bindValue (4, data.lastCorrect);
        query.bindValue (5, data.difficulty);

        if (updateCardbox) {
            query.bindValue (6, data.cardbox);
            query.bindValue (7, data.nextScheduled);
        }

        query.exec();
    }
}
