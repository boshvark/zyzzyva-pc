//---------------------------------------------------------------------------
// QuizEngine.cpp
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

#include "QuizEngine.h"
#include "WordEngine.h"
#include <cstdlib>
#include <ctime>

QuizEngine::QuizEngine (WordEngine* e)
    : wordEngine (e)
{
    std::time_t now = std::time (0);
    std::srand (now);
}

//---------------------------------------------------------------------------
//  newQuiz
//
//! Begin a new quiz.
//
//! @param input the group of symbols forming the basis of the quiz
//---------------------------------------------------------------------------
void
QuizEngine::newQuiz (const QuizSpec& spec)
{
    quizSpec = spec;
    quizQuestions.clear();

    // If alphagrams is not specified, then the quiz will be a single question
    // requiring the entire search results of the search spec as an answer.
    singleSpecQuestion = !quizSpec.alphagrams;

    // When using alphagrams, always change quiz type to Anagram.  The pattern
    // is used to select the list of alphagrams, then anagrams are used as
    // quiz answers.
    if (!singleSpecQuestion) {
        quizQuestions = wordEngine->search (quizSpec.searchSpec, true);
        quizQuestions = wordEngine->alphagrams (quizQuestions);

        // Do a random shuffle
        if (quizSpec.randomOrder) {
            QString tmp;
            int num = quizQuestions.size();
            for (int i = 0; i < num ; ++i) {
                int rnum = i + (std::rand() % (num - i));
                if (rnum == i)
                    continue;
                tmp = quizQuestions[rnum];
                quizQuestions[rnum] = quizQuestions[i];
                quizQuestions[i] = tmp;
            }
        }
    }

    else {
        quizQuestions << spec.asString();
    }

    questionIndex = 0;
    quizTotal = 0;
    quizCorrect = 0;
    quizIncorrect = 0;
    prepareQuestion();
}

//---------------------------------------------------------------------------
//  nextQuestion
//
//! Go to the next question in the quiz.
//
//! @return true if there are more questions, false if there are no more
//---------------------------------------------------------------------------
bool
QuizEngine::nextQuestion()
{
    if (onLastQuestion())
        return false;

    ++questionIndex;
    prepareQuestion();
    return true;
}

//---------------------------------------------------------------------------
//  respond
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
        ++quizIncorrect;
        return Incorrect;
    }

    if (correctUserResponses.find (response) != correctUserResponses.end())
        return Duplicate;

    correctUserResponses.insert (response);
    ++quizCorrect;
    return Correct;
}

//---------------------------------------------------------------------------
//  getQuestion
//
//! Get the question string for the current question.
//
//! @return the current question string
//---------------------------------------------------------------------------
QString
QuizEngine::getQuestion() const
{
    QStringList::const_iterator it = quizQuestions.at (questionIndex);
    return it == quizQuestions.end() ? QString::null : *it;
}

//---------------------------------------------------------------------------
//  getMissed
//
//! Get a list of correct responses that have not been answered yet.
//
//! @return the unanswered responses
//---------------------------------------------------------------------------
QStringList
QuizEngine::getMissed() const
{
    QStringList responses;
    std::set<QString>::const_iterator it, uit;
    for (it = correctResponses.begin(); it != correctResponses.end(); ++it) {
        uit = correctUserResponses.find (*it);
        if (uit == correctUserResponses.end())
            responses << *it;
    }

    return responses;
}

//---------------------------------------------------------------------------
//  onLastQuestion
//
//! Determine whether the current quiz is on the last question.
//
//! @return true if on the last question, false otherwise
//---------------------------------------------------------------------------
bool
QuizEngine::onLastQuestion() const
{
    return (questionIndex == int(quizQuestions.size() - 1));
}

//---------------------------------------------------------------------------
//  clearQuestion
//
//! Clear all answers and user responses.
//---------------------------------------------------------------------------
void
QuizEngine::clearQuestion()
{
    correctResponses.clear();
    correctUserResponses.clear();
    incorrectUserResponses.clear();
}

//---------------------------------------------------------------------------
//  prepareQuestion
//
//! Get the answers to the current question.
//---------------------------------------------------------------------------
void
QuizEngine::prepareQuestion()
{
    clearQuestion();
    QString question = getQuestion();

    QStringList answers;

    if (singleSpecQuestion)
        answers = wordEngine->search (quizSpec.searchSpec, true);
    else {
        SearchSpec spec;
        spec.pattern = question;
        spec.type = Anagram;
        answers = wordEngine->search (spec, true);
    }

    QStringList::iterator it;
    for (it = answers.begin(); it != answers.end(); ++it) {
        correctResponses.insert (*it);
    }
    quizTotal += correctResponses.size();
}
