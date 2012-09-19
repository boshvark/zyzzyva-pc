//---------------------------------------------------------------------------
// QuizDatabase.cpp
//
// A class for working with the database for a saved quiz.
//
// Copyright 2012 Boshvark Software, LLC.
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
#include "Auxil.h"
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

//const QString SQL_CREATE_QUESTIONS_TABLE_CURRENT =
//    "CREATE TABLE questions (question varchar(16), correct integer, "
//    "incorrect integer, streak integer, last_correct integer, "
//    "difficulty integer, cardbox integer, next_scheduled integer)";

//---------------------------------------------------------------------------
//  QuizDatabase
//
//! Default constructor.
//---------------------------------------------------------------------------
QuizDatabase::QuizDatabase()
    : db(0)
{
}

//---------------------------------------------------------------------------
//  QuizDatabase
//
//! Constructor.  Connect to the database specified by a lexicon and quiz
//! type.
//
//! @param fileName the database file name
//---------------------------------------------------------------------------
QuizDatabase::QuizDatabase(const QString& fileName)
    : db(0), dbFileName(fileName)
{
    // Get random connection name
    rng.srand(QDateTime::currentDateTime().toTime_t(), Auxil::getPid());
    unsigned int r = rng.rand();
    dbConnectionName = "quiz" + QString::number(r);
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE",
        dbConnectionName));
    db->setDatabaseName(dbFileName);
    //if (!db->open())
    //    return;
}

//---------------------------------------------------------------------------
//  QuizDatabase
//
//! Destructor.
//---------------------------------------------------------------------------
QuizDatabase::~QuizDatabase()
{
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
//  getDatabaseFile
//
//! Return the database file name.
//
//! @return the database file name
//---------------------------------------------------------------------------
QString
QuizDatabase::getDatabaseFile() const
{
    return dbFileName;
}

//---------------------------------------------------------------------------
//  getQuizSpec
//
//! Generate a quiz spec from the database.
//
//! @return the quiz spec
//---------------------------------------------------------------------------
QuizSpec
QuizDatabase::getQuizSpec() const
{
    QuizSpec spec;

    if (!db || (!db->isOpen() && !db->open()))
        return spec;

    QString queryStr = "SELECT lexicon, type, current_question, num_words, "
        "method, question_order FROM quiz";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return spec;
    }

    spec.setFilename(dbFileName);
    if (query.next()) {

        qDebug("QUERY SUCCESSFUL!");

        spec.setLexicon(query.value(0).toString());
        spec.setType(QuizSpec::QuizType(query.value(1).toInt()));
        spec.setQuestionIndex(query.value(2).toInt());
        spec.setNumWords(query.value(3).toInt());
        spec.setMethod(QuizSpec::QuizMethod(query.value(4).toInt()));
        spec.setQuizOrder(QuizSpec::QuizOrder(query.value(5).toInt()));
        spec.setNumQuestions(getNumQuestions());

        // ### add timer spec to quiz database
        // add new timer table in addition to quiz and questions
        //spec.setTimerSpec(...);
    }

    db->close();

    return spec;
}

//---------------------------------------------------------------------------
//  setQuizSpec
//
//! Update the database with the contents of a quiz spec.
//
//! @param quizSpec the quiz spec
//---------------------------------------------------------------------------
bool
QuizDatabase::setQuizSpec(const QuizSpec& quizSpec)
{
    QuizSpec existingSpec = getQuizSpec();

    if (!db || (!db->isOpen() && !db->open()))
        return false;

    // ### Figure out a better way to test for emptiness?
    QString queryStr;

    // Quiz data exists, so update it
    if (existingSpec.getNumQuestions()) {

        // Everything is the same, no need to update
        if ((quizSpec.getLexicon() == existingSpec.getLexicon()) &&
            (quizSpec.getType() == existingSpec.getType()) &&
            (quizSpec.getQuestionIndex() == existingSpec.getQuestionIndex()) &&
            (quizSpec.getNumWords() == existingSpec.getNumWords()) &&
            (quizSpec.getMethod() == existingSpec.getMethod()) &&
            (quizSpec.getQuizOrder() == existingSpec.getQuizOrder()))
        {
            return true;
        }

        queryStr = "UPDATE quiz SET lexicon=?, type=?, current_question=?, "
            "num_words=?, method=?, question_order=?";
    }

    else {
        queryStr = "INSERT INTO quiz (lexicon, type, current_question, "
            "num_words, method, question_order) VALUES (?, ?, ?, ?, ?, ?)";
    }

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, quizSpec.getLexicon());
    query.bindValue(bindNum++, quizSpec.getType());
    query.bindValue(bindNum++, quizSpec.getQuestionIndex());
    query.bindValue(bindNum++, quizSpec.getNumWords());
    query.bindValue(bindNum++, quizSpec.getMethod());
    query.bindValue(bindNum++, quizSpec.getQuizOrder());

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return false;
    }

    db->close();
    return true;
}

//---------------------------------------------------------------------------
//  getNumQuestions
//
//! Return the number of questions in the quiz.
//
//! @return the number of questions
//---------------------------------------------------------------------------
int
QuizDatabase::getNumQuestions() const
{
    if (!db || (!db->isOpen() && !db->open()))
        return 0;

    QString queryStr = "SELECT count(*) as count FROM questions";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return 0;
    }

    int numQuestions = 0;
    while (query.next()) {
        numQuestions = query.value(0).toInt();
        break;
    }

    db->close();
    return numQuestions;
}

//---------------------------------------------------------------------------
//  getQuestion
//
//! Return the question at an index.
//
//! @param index the question index
//! @return the question
//---------------------------------------------------------------------------
QuizQuestion
QuizDatabase::getQuestion(int index) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return QuizQuestion();

    QString queryStr = "SELECT status, name FROM questions "
        "WHERE question_index=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, index);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return QuizQuestion();
    }

    QuizQuestion question;
    if (query.next()) {
        question.setIndex(index);
        question.setStatus(QuizQuestion::Status(query.value(0).toInt()));
        question.setName(query.value(1).toString());
    }

    db->close();
    return question;
}

//---------------------------------------------------------------------------
//  setQuestion
//
//! Set the question at an index.
//
//! @param index the question index
//! @param question the question
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
QuizDatabase::setQuestion(int index, const QuizQuestion& question) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return false;

    QString queryStr;
    QList<QVariant> params;
    QuizQuestion existingQuestion = getQuestion(index);

    // Question data exists, so update it
    if (existingQuestion.isValid()) {
        queryStr ="UPDATE questions SET status=?, name=? "
            "WHERE question_index=?";
        params << question.getStatus();
        params << question.getName();
        params << index;
    }

    // Question data does not exist, so insert it
    else {
        queryStr = "INSERT INTO questions (question_index, status, name) "
            "VALUES (?, ?, ?)";
        params << index;
        params << question.getStatus();
        params << question.getName();
    }

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    QListIterator<QVariant> iParam (params);
    while (iParam.hasNext()) {
        query.bindValue(bindNum++, iParam.next());
    }

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return false;
    }

    db->close();
    return true;
}

//---------------------------------------------------------------------------
//  setQuestions
//
//! Set all the questions in a quiz.
//
//! @param questions the list of questions
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
QuizDatabase::setQuestions(const QList<QuizQuestion>& questions) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return false;

    // Delete previous quiz questions
    QString deleteQueryStr = "DELETE FROM questions";
    QSqlQuery deleteQuery (*db);
    deleteQuery.prepare(deleteQueryStr);

    if (!deleteQuery.exec()) {
        qDebug("Query failed: %s",
            deleteQuery.lastError().text().toUtf8().constData());
        db->close();
        return false;
    }

    // Begin transaction
    if (!db->transaction()) {
        qDebug("Failed to begin transaction");
        db->close();
        return false;
    }

    // Insert new quiz questions
    QString insertQueryStr = "INSERT INTO questions "
        "(question_index, status, name) VALUES (?, ?, ?)";

    QSqlQuery insertQuery (*db);
    insertQuery.prepare(insertQueryStr);

    QListIterator<QuizQuestion> iQuestion (questions);
    while (iQuestion.hasNext()) {
        const QuizQuestion& question = iQuestion.next();
        int bindNum = 0;
        insertQuery.bindValue(bindNum++, question.getIndex());
        insertQuery.bindValue(bindNum++, question.getStatus());
        insertQuery.bindValue(bindNum++, question.getName());

        if (!insertQuery.exec()) {
            qDebug("Query failed: %s",
                insertQuery.lastError().text().toUtf8().constData());
            db->close();
            return false;
        }
    }

    db->commit();
    db->close();
    return true;
}

//---------------------------------------------------------------------------
//  getNumQuestionsWithStatus
//
//! Return the number of questions with a particular status.
//
//! @param status the status
//! @return the number of questions
//---------------------------------------------------------------------------
int
QuizDatabase::getNumQuestionsWithStatus(QuizQuestion::Status status) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return 0;

    QString queryStr = "SELECT count(*) as count FROM questions "
        "WHERE status=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, status);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return 0;
    }

    int numQuestions = 0;
    if (query.next()) {
        numQuestions = query.value(0).toInt();
    }

    db->close();
    return numQuestions;
}

//---------------------------------------------------------------------------
//  getQuestionIndexesWithStatus
//
//! Return the indexes of questions with a particular status.
//
//! @param status the status
//! @return the question indexes
//---------------------------------------------------------------------------
QSet<int>
QuizDatabase::getQuestionIndexesWithStatus(QuizQuestion::Status status) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return QSet<int>();

    QString queryStr = "SELECT question_index FROM questions "
        "WHERE status=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, status);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return QSet<int>();
    }

    QSet<int> indexes;
    while (query.next()) {
        indexes.insert(query.value(0).toInt());
    }

    db->close();
    return indexes;
}

//---------------------------------------------------------------------------
//  getQuestionsWithStatus
//
//! Return the questions with a particular status.
//
//! @param status the status
//! @return the questions
//---------------------------------------------------------------------------
QList<QuizQuestion>
QuizDatabase::getQuestionsWithStatus(QuizQuestion::Status status) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return QList<QuizQuestion>();

    QString queryStr = "SELECT question_index, name FROM questions "
        "WHERE status=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, status);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return QList<QuizQuestion>();
    }

    QList<QuizQuestion> questions;
    while (query.next()) {
        QuizQuestion question;
        question.setIndex(query.value(0).toInt());
        question.setStatus(status);
        question.setName(query.value(1).toString());
        questions.append(question);
    }

    db->close();
    return questions;
}

//---------------------------------------------------------------------------
//  getNumCanonicalResponses
//
//! Return the number of canonical responses from questions that are complete.
//
//! @return the number of canonical responses
//---------------------------------------------------------------------------
int
QuizDatabase::getNumCanonicalResponses() const
{
    if (!db || (!db->isOpen() && !db->open()))
        return 0;

    QString queryStr = "SELECT count(*) as count from responses "
        "WHERE status!=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, QuizResponse::Incorrect);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return 0;
    }

    int numResponses = 0;
    if (query.next()) {
        numResponses = query.value(0).toInt();
    }

    db->close();
    return 0;
}

//---------------------------------------------------------------------------
//  getNumResponsesWithStatus
//
//! Return the number of responses with a particular status.
//
//! @param status the status
//! @return the number of responses
//---------------------------------------------------------------------------
int
QuizDatabase::getNumResponsesWithStatus(QuizResponse::Status status) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return 0;

    QString queryStr = "SELECT count(*) as count FROM responses "
        "WHERE status=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, status);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return 0;
    }

    int numResponses = 0;
    if (query.next()) {
        numResponses = query.value(0).toInt();
    }

    db->close();
    return numResponses;
}

//---------------------------------------------------------------------------
//  getResponsesWithStatus
//
//! Return the responses with a particular status.
//
//! @param status the status
//! @return the responses
//---------------------------------------------------------------------------
QList<QuizResponse>
QuizDatabase::getResponsesWithStatus(QuizResponse::Status status) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return QList<QuizResponse>();

    QString queryStr = "SELECT question_index, name FROM responses "
        "WHERE status=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, status);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return QList<QuizResponse>();
    }

    QList<QuizResponse> responses;
    while (query.next()) {
        QuizResponse response;
        response.setQuestionIndex(query.value(0).toInt());
        response.setStatus(status);
        response.setName(query.value(1).toString());
        responses.append(response);
    }

    db->close();
    return responses;
}

//---------------------------------------------------------------------------
//  getNumCanonicalResponsesAtIndex
//
//! Return the number of canonical responses for the question at a particular
//! index.
//
//! @param index the question index
//! @return the number of canonical responses
//---------------------------------------------------------------------------
int
QuizDatabase::getNumCanonicalResponsesAtIndex(int index) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return 0;

    QString queryStr = "SELECT count(*) as count from responses "
        "WHERE question_index=? AND status!=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, index);
    query.bindValue(bindNum++, QuizResponse::Incorrect);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return 0;
    }

    int numResponses = 0;
    if (query.next()) {
        numResponses = query.value(0).toInt();
    }

    db->close();
    return 0;
}

//---------------------------------------------------------------------------
//  getCanonicalResponsesAtIndex
//
//! Return the canonical responses for the question at a particular index.
//
//! @param index the question index
//! @return the canonical responses
//---------------------------------------------------------------------------
QList<QuizResponse>
QuizDatabase::getCanonicalResponsesAtIndex(int index) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return QList<QuizResponse>();

    QString queryStr = "SELECT status, name FROM responses "
        "WHERE question_index=? AND status!=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, index);
    query.bindValue(bindNum++, QuizResponse::Incorrect);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return QList<QuizResponse>();
    }

    QList<QuizResponse> responses;
    while (query.next()) {
        QuizResponse response;
        response.setQuestionIndex(index);
        response.setStatus(QuizResponse::Status(query.value(0).toInt()));
        response.setName(query.value(1).toString());
        responses.append(response);
    }

    db->close();
    return responses;
}

//---------------------------------------------------------------------------
//  getResponsesAtIndex
//
//! Return the canonical responses for the question at a particular index.
//
//! @param index the question index
//! @return the canonical responses
//---------------------------------------------------------------------------
QList<QuizResponse>
QuizDatabase::getResponsesAtIndex(int index) const
{
    if (!db || (!db->isOpen() && !db->open()))
        return QList<QuizResponse>();

    QString queryStr = "SELECT status, name FROM responses "
        "WHERE question_index=?";

    QSqlQuery query (*db);
    query.prepare(queryStr);

    int bindNum = 0;
    query.bindValue(bindNum++, index);

    if (!query.exec()) {
        qDebug("Query failed: %s", query.lastError().text().toUtf8().constData());
        db->close();
        return QList<QuizResponse>();
    }

    QList<QuizResponse> responses;
    while (query.next()) {
        QuizResponse response;
        response.setQuestionIndex(index);
        response.setStatus(QuizResponse::Status(query.value(0).toInt()));
        response.setName(query.value(1).toString());
        responses.append(response);
    }

    db->close();
    return responses;
}
