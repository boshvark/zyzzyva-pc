//---------------------------------------------------------------------------
// QuizEngine.h
//
// The engine for generating quizzes and keeping track of the user's
// performance on each quiz.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef QUIZ_ENGINE_H
#define QUIZ_ENGINE_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>
#include <set>
#include "MatchType.h"

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
    QuizEngine (WordEngine* e) : wordEngine (e) { }
    ~QuizEngine() { }

    void newQuiz (const QString& input, MatchType type, bool alphagrams);
    bool nextQuestion();

    ResponseStatus respond (const QString& response);
    QString getQuestion() const;
    int total() const { return correctResponses.size(); }
    int correct() const { return correctUserResponses.size(); }
    int incorrect() const { return incorrectUserResponses.size(); }
    int numQuestions() const { return quizQuestions.size(); }

    bool onLastQuestion() const;

  private:
    void clearQuestion();
    void prepareQuestion();

  private:
    WordEngine*       wordEngine;
    std::set<QString> correctResponses;
    std::set<QString> correctUserResponses;
    QStringList       incorrectUserResponses;

    QStringList       quizQuestions;
    int               questionIndex;
    MatchType         quizType;
};

#endif // QUIZ_ENGINE_H
