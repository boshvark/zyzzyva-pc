//---------------------------------------------------------------------------
// QuizEngine.h
//
// The engine for generating quizzes and keeping track of the user's
// performance on each quiz.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef QUIZ_ENGINE_H
#define QUIZ_ENGINE_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>
#include <set>
#include "QuizSpec.h"

class WordEngine;

class QuizEngine
{
    public:
    enum ResponseStatus {
        Correct,
        Incorrect,
        Duplicate
    };

    public:
    QuizEngine (WordEngine* e);
    ~QuizEngine() { }

    void newQuiz (const QuizSpec& spec);
    bool nextQuestion();

    ResponseStatus respond (const QString& response);
    QString getQuestion() const;
    QStringList getMissed() const;
    QuizSpec getQuizSpec() const { return quizSpec; }
    int getQuestionIndex() const { return questionIndex; }
    int numQuestions() const { return quizQuestions.size(); }
    int getQuestionTotal() const { return correctResponses.size(); }
    int getQuestionCorrect() const { return correctUserResponses.size(); }
    int getQuestionIncorrect() const { return incorrectUserResponses.size(); }
    int getQuizTotal() const { return quizTotal; }
    int getQuizCorrect() const { return quizCorrect; }
    int getQuizIncorrect() const { return quizIncorrect; }

    bool onLastQuestion() const;

    private:
    void clearQuestion();
    void prepareQuestion();

    private:
    WordEngine*       wordEngine;
    std::set<QString> correctResponses;
    std::set<QString> correctUserResponses;
    QStringList       incorrectUserResponses;

    int quizTotal;
    int quizCorrect;
    int quizIncorrect;

    bool        singleSpecQuestion;
    QuizSpec    quizSpec;
    QStringList quizQuestions;
    int         questionIndex;
};

#endif // QUIZ_ENGINE_H
