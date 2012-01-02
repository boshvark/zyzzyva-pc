//---------------------------------------------------------------------------
// QuizEngine.h
//
// The engine for generating quizzes and keeping track of the user's
// performance on each quiz.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_QUIZ_ENGINE_H
#define ZYZZYVA_QUIZ_ENGINE_H

#include "QuizSpec.h"
#include "Rand.h"
#include <QSet>
#include <QString>
#include <QStringList>

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
    QuizEngine(WordEngine* e);
    ~QuizEngine() { }

    bool newQuiz(const QuizSpec& spec);
    bool nextQuestion();
    void completeQuestion();

    ResponseStatus respond(const QString& response,
                           bool lexiconSymbols = false);
    void markQuestionAsCorrect();
    void markQuestionAsMissed();
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
    QSet<QString> getQuestionCorrectResponses() const {
        return correctUserResponses; }
    QStringList getQuestionIncorrectResponses() const {
        return incorrectUserResponses; }

    bool getQuestionComplete() const {
        return quizSpec.getProgress().getQuestionComplete(); }
    bool onLastQuestion() const;
    void setQuizSpecFilename(const QString& filename) {
        quizSpec.setFilename(filename);
    }

    private:
    void clearQuestion();
    void prepareQuestion();
    void addQuestionCorrect(const QString& response);
    void addQuestionIncorrect(const QString& response);
    QMap<QChar, QString> parseHookSymbols(const QString& str);

    private:
    WordEngine*   wordEngine;
    QSet<QString> correctResponses;
    QSet<QString> correctUserResponses;
    QStringList   incorrectUserResponses;

    int quizTotal;
    int quizCorrect;
    int quizIncorrect;

    Rand        rng;
    QuizSpec    quizSpec;
    QStringList quizQuestions;
    int         questionIndex;
};

#endif // ZYZZYVA_QUIZ_ENGINE_H
