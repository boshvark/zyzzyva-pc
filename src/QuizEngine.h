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
#include <set>

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

    void newQuiz (const QString& question);

    ResponseStatus respond (const QString& response);
    const QString& getQuestion() const { return question; }
    int total() const { return correctResponses.size(); }
    int correct() const { return correctUserResponses.size(); }
    int incorrect() const { return incorrectUserResponses.size(); }

  private:
    WordEngine*       wordEngine;
    QString           question;
    std::set<QString> correctResponses;
    std::set<QString> correctUserResponses;
    QStringList       incorrectUserResponses;
};

#endif // QUIZ_ENGINE_H
