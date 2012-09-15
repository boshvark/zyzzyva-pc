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
    QuizQuestion getQuestion(int index);
    bool setQuestion(int index, const QuizQuestion& question);
    bool setQuestions(const QList<QuizQuestion>& questions);

    private:
    QSqlDatabase* db;
    QString dbConnectionName;
    QString dbFileName;
    Rand rng;
};

#endif // ZYZZYVA_QUIZ_DATABASE_H
