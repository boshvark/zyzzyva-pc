//---------------------------------------------------------------------------
// QuizDatabase.h
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

#ifndef ZYZZYVA_QUIZ_DATABASE_H
#define ZYZZYVA_QUIZ_DATABASE_H

#include "Rand.h"
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QString>

class QuizDatabase
{
    public:
    class QuestionData {
        public:
        QuestionData() : valid(false), numCorrect(0), numIncorrect(0),
                         streak(0), lastCorrect(0), difficulty(0),
                         cardbox(-1), nextScheduled(0) { }
        bool valid;
        int numCorrect;
        int numIncorrect;
        int streak;
        int lastCorrect;
        int difficulty;
        int cardbox;
        int nextScheduled;
    };

    QuizDatabase(const QString& lexicon, const QString& quizType);
    ~QuizDatabase();

    bool isValid() const;
    bool updateSchema();
    void recordResponse(const QString& question, bool correct,
                        bool updateCardbox);
    void undoLastResponse(const QString& question);
    void addToCardbox(const QStringList& questions, bool estimateCardbox,
                      int cardbox = 0);
    void addToCardbox(const QString& question, bool estimateCardbox,
                      int cardbox = 0);
    void removeFromCardbox(const QStringList& questions);
    void removeFromCardbox(const QString& question);
    void setCardbox(const QString& question, int cardbox);
    int rescheduleCardbox(const QStringList& questions);
    int shiftCardboxByBacklog(const QStringList& questions, int desiredBacklog);
    int shiftCardboxByDays(const QStringList& questions, int numDays);
    QStringList getReadyQuestions(const QStringList& questions, bool zeroFirst);
    QuestionData getQuestionData(const QString& question);
    QMap<int, int> getCardboxCounts();
    QMap<int, int> getCardboxDueCounts();
    QMap<int, int> getScheduleDayCounts();

    const QSqlDatabase* getDatabase() const;

    private:
    int calculateNextScheduled(int cardbox);
    void setQuestionData(const QString& question, const QuestionData& data,
                         bool updateCardbox);

    private:
    QString dbConnectionName;
    QSqlDatabase* db;
    Rand rng;

    QString undoQuestion;
    QuestionData undoData;
};

#endif // ZYZZYVA_QUIZ_DATABASE_H
