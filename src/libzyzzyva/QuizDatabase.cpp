//---------------------------------------------------------------------------
// QuizDatabase.cpp
//
// A class for working with database of quiz performance statistics.
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

#include "QuizDatabase.h"
#include "MainSettings.h"
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
QuizDatabase::QuizDatabase(const QString& lexicon, const QString& quizType)
    : db(0)
{
    QString dirName = Auxil::getQuizDir() + "/data/" + lexicon;
    QDir dir (dirName);
    if (!dir.exists() && !dir.mkpath(dirName)) {
        qWarning("Cannot create quiz stats directory\n");
        return;
    }

    QString dbFilename = dirName + "/" + quizType + ".db";

    // Get random connection name
    rng.srand(QDateTime::currentDateTime().toTime_t(), Auxil::getPid());
    unsigned int r = rng.rand();
    dbConnectionName = "quiz" + QString::number(r);
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE",
                                                    dbConnectionName));
    db->setDatabaseName(dbFilename);
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
        db = 0;
        QSqlDatabase::removeDatabase(dbConnectionName);
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
    query.exec("SELECT name FROM sqlite_master WHERE type='table' "
               "AND name='questions'");
    if (!query.next()) {
        query.exec(SQL_CREATE_QUESTIONS_TABLE_CURRENT);
    }

    // The table already exists, so update the schema
    else {
        query.exec("SELECT sql FROM sqlite_master WHERE type='table' "
                   "AND name='questions'");
        if (!query.next())
            return false;
        QString sql = query.value(0).toString();

        // Upgrade from 0.14.0 schema to 1.0.0 schema
        if (sql == SQL_CREATE_QUESTIONS_TABLE_0_14_0) {
            query.exec("ALTER TABLE questions ADD COLUMN cardbox integer");
            query.exec("ALTER TABLE questions ADD COLUMN "
                       "next_scheduled integer");
        }
    }

    // Create index on question column of questions table
    query.exec("SELECT name FROM sqlite_master WHERE type='index' "
               "AND name='question_index' AND tbl_name='questions'");
    if (!query.next()) {
        query.exec("CREATE UNIQUE INDEX question_index ON questions "
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
QuizDatabase::recordResponse(const QString& question, bool correct,
                             bool updateCardbox)
{
    QuestionData data = getQuestionData(question);
    undoQuestion = question;
    undoData = data;

    if (data.valid) {
        if (correct) {
            ++data.numCorrect;
            if (data.streak < 0)
                data.streak = 1;
            else
                ++data.streak;
            data.lastCorrect = QDateTime::currentDateTime().toTime_t();
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
            data.nextScheduled = calculateNextScheduled(data.cardbox);
        }
    }

    else {
        data = QuestionData();
        data.valid = true;
        data.numCorrect = (correct ? 1 : 0);
        data.numIncorrect = (correct ? 0 : 1);
        data.streak = (correct ? 1 : -1);
        data.lastCorrect = (correct ? QDateTime::currentDateTime().toTime_t()
                                    : 0);
        // XXX: Fix difficulty ratings!
        data.difficulty = 50;
        if (updateCardbox) {
            data.cardbox = correct ? 1 : 0;
            data.nextScheduled = calculateNextScheduled(data.cardbox);
        }
    }

    setQuestionData(question, data, updateCardbox);
}

//---------------------------------------------------------------------------
//  undoLastResponse
//
//! Undo the last question response.
//---------------------------------------------------------------------------
void
QuizDatabase::undoLastResponse(const QString& question)
{
    if (undoQuestion != question)
        return;

    setQuestionData(question, undoData, true);
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
//! @param cardbox the cardbox to place the question in, if estimateCardbox is
//! false
//---------------------------------------------------------------------------
void
QuizDatabase::addToCardbox(const QStringList& questions, bool estimateCardbox,
                           int cardbox)
{
    QSqlQuery query (*db);
    query.exec("BEGIN TRANSACTION");
    QStringListIterator it (questions);
    while (it.hasNext()) {
        addToCardbox(it.next(), estimateCardbox, cardbox);
    }
    query.exec("COMMIT TRANSACTION");
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
//! @param cardbox the cardbox to place the question in, if estimateCardbox is
//! false
//---------------------------------------------------------------------------
void
QuizDatabase::addToCardbox(const QString& question, bool estimateCardbox,
                           int cardbox)
{
    QuestionData data = getQuestionData(question);

    if (data.valid) {
        // Question is already in the cardbox system, so leave it alone
        if (data.cardbox >= 0) {
            return;
        }

        if (estimateCardbox && (data.streak > 0))
            data.cardbox = data.streak;
        else
            data.cardbox = cardbox;
    }

    else {
        data = QuestionData();
        data.valid = true;
        data.cardbox = cardbox;
    }

    // Move scheduled time back by 16 hours, so questions in cardbox 0 will be
    // available immediately
    data.nextScheduled = calculateNextScheduled(data.cardbox);
    if (!data.cardbox)
        data.nextScheduled -= 60 * 60 * 16;

    setQuestionData(question, data, true);
}

//---------------------------------------------------------------------------
//  removeFromCardbox
//
//! Remove a list of questions from the cardbox system.
//
//! @param questions the questions to remove from the cardbox system
//---------------------------------------------------------------------------
void
QuizDatabase::removeFromCardbox(const QStringList& questions)
{
    QStringList qlist;
    QStringListIterator it (questions);
    while (it.hasNext()) {
        qlist.append("'" + it.next() + "'");
    }
    QString queryStr = "UPDATE questions SET cardbox=NULL, "
        "next_scheduled=NULL WHERE question IN (" +
        qlist.join(", ") + ")";

    QSqlQuery query (*db);
    query.prepare(queryStr);
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
QuizDatabase::removeFromCardbox(const QString& question)
{
    QStringList qlist;
    qlist.append(question);
    removeFromCardbox(qlist);
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
QuizDatabase::setCardbox(const QString& question, int cardbox)
{
    QuestionData data = getQuestionData(question);
    data.valid = true;
    data.cardbox = cardbox;
    data.nextScheduled = calculateNextScheduled(data.cardbox);
    setQuestionData(question, data, true);
}

//---------------------------------------------------------------------------
//  rescheduleCardbox
//
//! Reschedule questions in the cardbox system.  If no questions are
//! specified, then all questions are rescheduled.
//
//! @param questions the list of questions to reschedule
//---------------------------------------------------------------------------
int
QuizDatabase::rescheduleCardbox(const QStringList& questions)
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
    query.prepare(queryStr);
    query.exec();

    QList<QString> selectedQuestions;
    QList<int> selectedCardboxes;

    while (query.next()) {
        selectedQuestions.append(query.value(0).toString());
        selectedCardboxes.append(query.value(1).toInt());
    }

    QSqlQuery updateQuery (*db);
    updateQuery.prepare("UPDATE questions SET next_scheduled=? "
                        "WHERE question=?");

    QSqlQuery transactionQuery ("BEGIN TRANSACTION", *db);

    QListIterator<QString> it (selectedQuestions);
    QListIterator<int> jt (selectedCardboxes);
    while (it.hasNext()) {
        QString question = it.next();
        int cardbox = jt.next();
        int nextScheduled = calculateNextScheduled(cardbox);
        nextScheduled -= 60 * 60 * 16;

        updateQuery.bindValue(0, nextScheduled);
        updateQuery.bindValue(1, question);
        updateQuery.exec();
    }

    transactionQuery.exec("END TRANSACTION");

    return selectedQuestions.size();
}

//---------------------------------------------------------------------------
//  shiftCardboxByBacklog
//
//! Shift questions in the cardbox system so that a certain number are
//! currently available from the list provided.  If no questions are
//! specified, then all questions are shifted.
//
//! @param questions the list of questions to shift
//! @param desiredBacklog the desired backlog size after shifting
//! @return the number of questions shifted
//---------------------------------------------------------------------------
int
QuizDatabase::shiftCardboxByBacklog(const QStringList& questions,
    int desiredBacklog)
{
    QString queryStr = "SELECT question, next_scheduled "
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

    queryStr += " ORDER BY next_scheduled";

    QSqlQuery query (*db);
    query.prepare(queryStr);
    query.exec();

    QList<QString> selectedQuestions;
    QList<int> selectedNextScheduled;

    int index = 1;
    int pegNextScheduled = 0;
    while (query.next()) {
        QString question = query.value(0).toString();
        int nextScheduled = query.value(1).toInt();
        selectedQuestions.append(question);
        selectedNextScheduled.append(nextScheduled);
        if (index <= desiredBacklog)
            pegNextScheduled = nextScheduled;
        ++index;
    }

    unsigned int now = QDateTime::currentDateTime().toTime_t();
    int shiftSeconds = now - pegNextScheduled;

    QSqlQuery updateQuery (*db);
    updateQuery.prepare("UPDATE questions SET next_scheduled=? "
                        "WHERE question=?");

    QSqlQuery transactionQuery ("BEGIN TRANSACTION", *db);

    QListIterator<QString> it (selectedQuestions);
    QListIterator<int> jt (selectedNextScheduled);
    while (it.hasNext()) {
        QString question = it.next();
        int nextScheduled = jt.next() + shiftSeconds;
        updateQuery.bindValue(0, nextScheduled);
        updateQuery.bindValue(1, question);
        updateQuery.exec();
    }

    transactionQuery.exec("END TRANSACTION");

    return selectedQuestions.size();
}

//---------------------------------------------------------------------------
//  shiftCardboxByDays
//
//! Shift questions in the cardbox system by a certain number of days. If no
//! questions are specified, then all questions are shifted.
//
//! @param questions the list of questions to shift
//! @param numDays the number of days to shift the questions
//! @return the number of questions shifted
//---------------------------------------------------------------------------
int
QuizDatabase::shiftCardboxByDays(const QStringList& questions,
    int numDays)
{
    QString questionClause;
    if (!questions.isEmpty()) {
        questionClause = " AND question IN (";
        bool firstItem = true;
        foreach (const QString& question, questions) {
            if (!firstItem)
                questionClause += ", ";
            questionClause += "'" + question + "'";
            firstItem = false;
        }
        questionClause += ")";
    }

    int shiftSeconds = 86400 * numDays;

    QSqlQuery updateQuery (*db);
    updateQuery.prepare(QString("UPDATE questions set next_scheduled="
        "next_scheduled+? WHERE cardbox NOT NULL%1").arg(questionClause));
    updateQuery.bindValue(0, shiftSeconds);
    if (!updateQuery.exec())
        return 0;

    return updateQuery.numRowsAffected();
}

//---------------------------------------------------------------------------
//  getReadyQuestions
//
//! Get a list of questions that are ready for review, from a subset of
//! possible questions.  The questions are returned in their scheduled order.
//
//! @param questions the list of possible questions, or empty if all questions
//! should be retrieved
//! @param zeroFirst whether to put cardbox 0 questions before all others
//! @return the list of ready questions, in scheduled order
//---------------------------------------------------------------------------
QStringList
QuizDatabase::getReadyQuestions(const QStringList& questions, bool zeroFirst)
{
    unsigned int now = QDateTime::currentDateTime().toTime_t();

    bool selectAll = questions.isEmpty();
    QSet<QString> questionSet = questions.toSet();

    QString zQueryStr = zeroFirst ? QString(" OR cardbox = 0") : QString();
    QString queryStr = "SELECT question, cardbox FROM questions WHERE "
        "next_scheduled <= " + QString::number(now) + zQueryStr +
        " ORDER BY next_scheduled";

    QSqlQuery query (*db);
    query.prepare(queryStr);
    query.exec();

    QStringList zeroQuestions;
    QStringList readyQuestions;
    while (query.next()) {
        const QString& question = query.value(0).toString();

        // Skip questions that weren't in the parameter question list
        if (!selectAll && !questionSet.contains(question))
            continue;

        int cardbox = query.value(1).toInt();
        if (zeroFirst && (cardbox == 0))
            zeroQuestions.append(question);
        else
            readyQuestions.append(question);
    }

    return zeroFirst ? zeroQuestions + readyQuestions : readyQuestions;
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
QuizDatabase::getQuestionData(const QString& question)
{
    QuestionData data;

    QSqlQuery query (*db);
    query.prepare("SELECT correct, incorrect, streak, last_correct, "
                  "difficulty, cardbox, next_scheduled "
                  "FROM questions WHERE question=?");
    query.bindValue(0, question);
    query.exec();

    if (query.next()) {
        QVariant variant = query.value(0);
        if (!variant.isNull())
            data.numCorrect = variant.toInt();

        variant = query.value(1);
        if (!variant.isNull())
            data.numIncorrect = variant.toInt();

        variant = query.value(2);
        if (!variant.isNull())
            data.streak = variant.toInt();

        variant = query.value(3);
        if (!variant.isNull())
            data.lastCorrect = variant.toInt();

        variant = query.value(4);
        if (!variant.isNull())
            data.difficulty = variant.toInt();

        variant = query.value(5);
        if (!variant.isNull())
            data.cardbox = variant.toInt();

        variant = query.value(6);
        if (!variant.isNull())
            data.nextScheduled = variant.toInt();

        data.valid = true;
    }

    return data;
}

//---------------------------------------------------------------------------
//  getCardboxCounts
//
//! Return a map of cardboxes to the number of questions in each cardbox.
//
//! @return the cardbox count map
//---------------------------------------------------------------------------
QMap<int, int>
QuizDatabase::getCardboxCounts()
{
    QMap<int, int> cardboxCounts;

    QSqlQuery query (*db);
    query.prepare("SELECT cardbox, count(*) FROM questions "
        "WHERE cardbox NOT NULL GROUP BY cardbox");
    query.exec();

    while (query.next()) {
        QVariant variant = query.value(0);
        if (variant.isNull())
            continue;
        int cardbox = variant.toInt();

        variant = query.value(1);
        if (variant.isNull())
            continue;
        int count = variant.toInt();

        cardboxCounts[cardbox] = count;
    }

    return cardboxCounts;
}


//---------------------------------------------------------------------------
//  getCardboxDueCounts
//
//! Return a map of cardboxes to the number of questions due in each cardbox.
//
//! @return the cardbox due count map
//---------------------------------------------------------------------------
QMap<int, int>
QuizDatabase::getCardboxDueCounts()
{
    QMap<int, int> cardboxDueCounts;

    QSqlQuery query (*db);
    query.prepare(
        "SELECT cardbox, count(*) as count FROM questions "
        "WHERE cardbox NOT NULL AND next_scheduled <= ? "
        "GROUP by cardbox");

    unsigned int now = QDateTime::currentDateTime().toTime_t();
    query.bindValue(0, now);
    query.exec();

    while (query.next()) {
        QVariant variant = query.value(0);
        if (variant.isNull())
            continue;
        int cardbox = variant.toInt();

        variant = query.value(1);
        if (variant.isNull())
            continue;
        int count = variant.toInt();

        cardboxDueCounts[cardbox] = count;
    }

    return cardboxDueCounts;
}

//---------------------------------------------------------------------------
//  getScheduleDayCounts
//
//! Retrieve a pointer to the database connection.
//
//! @return the database connection pointer
//---------------------------------------------------------------------------
QMap<int, int>
QuizDatabase::getScheduleDayCounts()
{
    QMap<int, int> dayCounts;

    unsigned int now = QDateTime::currentDateTime().toTime_t();
    QString queryStr =
        "SELECT round((next_scheduled - 43200.0 - %1) / 86400) AS days, "
        "count(*) FROM questions WHERE cardbox NOT NULL GROUP BY days";
    QSqlQuery query (*db);
    query.prepare(queryStr.arg(now));
    query.exec();

    while (query.next()) {
        QVariant variant = query.value(0);
        if (variant.isNull())
            continue;
        int days = variant.toInt();

        variant = query.value(1);
        if (variant.isNull())
            continue;
        int count = variant.toInt();

        dayCounts[days] = count;
    }

    return dayCounts;
}

//---------------------------------------------------------------------------
//  getDatabase
//
//! Retrieve a pointer to the database connection.
//
//! @return the database connection pointer
//---------------------------------------------------------------------------
const QSqlDatabase*
QuizDatabase::getDatabase() const
{
    return db;
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
QuizDatabase::calculateNextScheduled(int cardbox)
{
    int daySeconds = 60 * 60 * 24;
    int halfDaySeconds = daySeconds / 2;
    int halfWindow = halfDaySeconds;
    int numDays = 0;
    int randDays = 0;
    int adjustSeconds = 0;

    // Only calculate schedule if cardbox specified, otherwise schedule
    // immediately into cardbox 0
    if (cardbox >= 0) {
        QList<int> scheds = MainSettings::getCardboxScheduleList();
        QList<int> windows = MainSettings::getCardboxWindowList();
        numDays = (cardbox < scheds.count()) ? scheds[cardbox]
            : scheds.last();
        randDays = (cardbox < windows.count()) ? windows[cardbox]
            : windows.last();
    }

    // Get the next scheduled time and perturb it randomly.  First, add or
    // subtract a random number of days so we don't get a bunch of questions
    // all at once on some future date.  Then, if the question is to be
    // scheduled immediately, randomize the time within an 8-hour window
    // centered on 4 hours ago.  If the question is to be scheduled for 1 day
    // from now, use an 8-hour window centered on 12 hours from now.
    // Otherwise, use a 24-hour window so the future question order is
    // somewhat randomized, and each question is equally likely to occur
    // anytime during the day.
    if (numDays == 0) {
        adjustSeconds = 60 * 60 * 4;
        halfWindow = 60 * 60 * 4;
    }
    else if (numDays == 1) {
        adjustSeconds = halfDaySeconds;
        halfWindow = 60 * 60 * 4;
    }

    if (randDays)
        numDays += rng.rand(randDays * 2) - randDays;
    unsigned int now = QDateTime::currentDateTime().toTime_t();
    int nextSeconds = (daySeconds * numDays) - adjustSeconds;
    int randSeconds = rng.rand(2 * halfWindow) - halfWindow;
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
QuizDatabase::setQuestionData(const QString& question, const QuestionData&
                              data, bool updateCardbox)
{
    QSqlQuery query (*db);
    query.prepare("SELECT question FROM questions WHERE question=?");
    query.bindValue(0, question);
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

        query.prepare(queryStr);
        query.bindValue(0, data.numCorrect);
        query.bindValue(1, data.numIncorrect);
        query.bindValue(2, data.streak);
        query.bindValue(3, data.lastCorrect);
        // XXX: Fix difficulty ratings!
        query.bindValue(4, data.difficulty);

        if (updateCardbox) {
            if (data.cardbox >= 0) {
                query.bindValue(5, data.cardbox);
                query.bindValue(6, data.nextScheduled);
            }
            else {
                query.bindValue(5, QVariant());
                query.bindValue(6, QVariant());
            }
            questionBindNum = 7;
        }

        query.bindValue(questionBindNum, question);
        bool ok = query.exec();
        if (!ok) {
            qDebug("Update query failed: %s",
                   query.lastError().text().toUtf8().constData());
        }
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

        query.prepare(queryStr);
        query.bindValue(0, question);
        query.bindValue(1, data.numCorrect);
        query.bindValue(2, data.numIncorrect);
        query.bindValue(3, data.streak);
        query.bindValue(4, data.lastCorrect);
        query.bindValue(5, data.difficulty);

        if (updateCardbox) {
            query.bindValue(6, data.cardbox);
            query.bindValue(7, data.nextScheduled);
        }

        query.exec();
    }
}
