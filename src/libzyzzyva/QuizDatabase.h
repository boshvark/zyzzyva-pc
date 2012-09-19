//---------------------------------------------------------------------------
// QuizDatabase.h
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

#ifndef ZYZZYVA_QUIZ_DATABASE_H
#define ZYZZYVA_QUIZ_DATABASE_H

#include "QuizQuestion.h"
#include "QuizResponse.h"
#include "QuizSpec.h"
#include "Rand.h"
#include <QString>

class QSqlDatabase;

class QuizDatabase
{
    public:
    QuizDatabase();
    QuizDatabase(const QString& fileName);
    ~QuizDatabase();

    bool isValid() const;
    QString getDatabaseFile() const;
    QuizSpec getQuizSpec() const;
    bool setQuizSpec(const QuizSpec& spec);
    int getNumQuestions() const;
    QuizQuestion getQuestion(int index) const;
    bool setQuestion(int index, const QuizQuestion& question) const;
    bool setQuestions(const QList<QuizQuestion>& questions) const;
    int getNumQuestionsWithStatus(QuizQuestion::Status status) const;
    QSet<int> getQuestionIndexesWithStatus(QuizQuestion::Status status) const;
    QList<QuizQuestion> getQuestionsWithStatus(QuizQuestion::Status status) const;
    int getNumCanonicalResponses() const;
    int getNumResponsesWithStatus(QuizResponse::Status status) const;
    QList<QuizResponse> getResponsesWithStatus(QuizResponse::Status status) const;
    int getNumCanonicalResponsesAtIndex(int index) const;
    QList<QuizResponse> getCanonicalResponsesAtIndex(int index) const;
    QList<QuizResponse> getResponsesAtIndex(int index) const;
    QuizResponse getResponse(int index, const QString& name) const;
    bool setResponse(int index, const QString& name,
        const QuizResponse& response) const;
    bool removeAllResponsesAtIndex(int index) const;

    private:
    QSqlDatabase* db;
    QString dbConnectionName;
    QString dbFileName;
    Rand rng;
};

#endif // ZYZZYVA_QUIZ_DATABASE_H
