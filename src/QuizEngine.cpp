//---------------------------------------------------------------------------
// QuizEngine.cpp
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

#include "QuizEngine.h"

//---------------------------------------------------------------------------
// newQuiz
//
//! Begin a new quiz.
//
//! @param question the quiz question
//---------------------------------------------------------------------------
void
QuizEngine::newQuiz (const QString& question)
{
    correctResponses.clear();
    correctUserResponses.clear();
    incorrectUserResponses.clear();
}

//---------------------------------------------------------------------------
// respond
//
//! Enter a user response to the current quiz.
//
//! @param response the user response
//! @return the status of the response
//---------------------------------------------------------------------------
QuizEngine::ResponseStatus
QuizEngine::respond (const QString& response)
{
    if (correctResponses.find (response) == correctResponses.end()) {
        incorrectUserResponses << response;
        return Incorrect;
    }

    if (correctUserResponses.find (response) != correctUserResponses.end())
        return Duplicate;

    correctUserResponses.insert (response);
    return Correct;
}
