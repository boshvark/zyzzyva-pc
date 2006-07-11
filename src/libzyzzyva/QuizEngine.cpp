//---------------------------------------------------------------------------
// QuizEngine.cpp
//
// The engine for generating quizzes and keeping track of the user's
// performance on each quiz.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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
#include "LetterBag.h"
#include "WordEngine.h"
#include "Auxil.h"
#include <cstdlib>
#include <ctime>

//---------------------------------------------------------------------------
//  probabilityCmp
//
//! A comparison function that sorts strings based on probability as a primary
//! key, and alphabetically as a secondary key.
//
//! @param a a string
//! @param b another string
//---------------------------------------------------------------------------
bool
probabilityCmp (const QString& a, const QString& b)
{
    // FIXME: There has to be a more efficient way than this!
    LetterBag letterBag;
    double ca = letterBag.getNumCombinations (a);
    double cb = letterBag.getNumCombinations (b);

    if (ca > cb)
        return true;
    else if (cb > ca)
        return false;
    else
        return (a < b);
}

//---------------------------------------------------------------------------
//  QuizEngine
//
//! Constructor.
//---------------------------------------------------------------------------
QuizEngine::QuizEngine (WordEngine* e)
    : wordEngine (e), quizTotal (0), quizCorrect (0), quizIncorrect (0),
    questionIndex (0)
{
}

//---------------------------------------------------------------------------
//  newQuiz
//
//! Begin a new quiz.
//
//! @param input the group of symbols forming the basis of the quiz
//! @return true if the quiz was started successfully, false otherwise;
//---------------------------------------------------------------------------
bool
QuizEngine::newQuiz (const QuizSpec& spec)
{
    QuizSpec::QuizType type = spec.getType();

    QStringList questions;

    // Anagram Quiz: The search spec is used to select the list of words.
    // Their alphagrams are used as quiz questions, and their anagrams are
    // used as quiz answers.
    if ((type == QuizSpec::QuizAnagrams) ||
        (type == QuizSpec::QuizAnagramsWithHooks))
    {
        questions = wordEngine->search (spec.getSearchSpec(), true);
        questions = wordEngine->alphagrams (questions);
    }

    else if (type == QuizSpec::QuizHooks) {
        questions = wordEngine->search (spec.getSearchSpec(), true);
    }

    else if (type == QuizSpec::QuizWordListRecall) {
        questions << spec.getSearchSpec().asString();
    }

    if (questions.isEmpty())
        return false;

    quizSpec = spec;
    quizQuestions = questions;

    switch (quizSpec.getQuestionOrder()) {
        case QuizSpec::AlphabeticalOrder:
        // do nothing - the questions are already in alphabetical order
        break;

        case QuizSpec::RandomOrder: {
            unsigned int seed = spec.getRandomSeed();
            if (!seed)
                seed = std::time (0);
            unsigned int seed2 = spec.getRandomSeed2();
            if (!seed2)
                seed2 = Auxil::getPid();
            rng.setAlgorithm (spec.getRandomAlgorithm());
            rng.srand (seed, seed2);
            quizSpec.setRandomSeed (seed);
            quizSpec.setRandomSeed2 (seed2);

            // XXX: We need a Shuffle class to handle shuffling using various
            // algorithms!
            QString tmp;
            int num = quizQuestions.size();
            for (int i = 0; i < num - 1; ++i) {
                unsigned int randnum = rng.rand (num - i - 1);
                int rnum = i + randnum;
                if (rnum == i)
                    continue;
                tmp = quizQuestions[rnum];
                quizQuestions[rnum] = quizQuestions[i];
                quizQuestions[i] = tmp;
            }
        }
        break;

        case QuizSpec::ProbabilityOrder: {
            qSort (quizQuestions.begin(), quizQuestions.end(),
                   probabilityCmp);
        }

        default: break;
    }

    // Restore quiz progress
    QuizProgress progress = spec.getProgress();
    questionIndex = progress.getQuestion();
    quizCorrect = progress.getNumCorrect();
    quizIncorrect = progress.getNumIncorrect();
    quizTotal = quizCorrect + progress.getNumMissed();

    prepareQuestion();

    // Add correct user responses from saved quiz state, and adjust the total
    // number of quiz answers by subtracting that number of responses.  This
    // is necessary because we used the quizCorrect total (including these
    // correct responses) as a base for the quizTotal calculation earlier.
    correctUserResponses = progress.getQuestionCorrect();
    if (!correctUserResponses.empty())
        quizTotal -= correctUserResponses.size();

    return true;
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

    // Update progress
    QuizProgress progress = quizSpec.getProgress();
    progress.setQuestion (questionIndex);
    progress.setCorrect (quizCorrect);
    progress.setQuestionComplete (false);
    progress.clearQuestionCorrect();
    QStringList missed = getMissed();
    QStringList::iterator it;
    for (it = missed.begin(); it != missed.end(); ++it)
        progress.addMissed (*it);

    for (it = incorrectUserResponses.begin(); it !=
         incorrectUserResponses.end(); ++it)
    {
        progress.addIncorrect (*it);
    }
    quizSpec.setProgress (progress);

    prepareQuestion();
    return true;
}

//---------------------------------------------------------------------------
//  completeQuestion
//
//! Mark the current question as completed.
//---------------------------------------------------------------------------
void
QuizEngine::completeQuestion()
{
    QuizProgress progress = quizSpec.getProgress();
    progress.setQuestionComplete (true);
    quizSpec.setProgress (progress);
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
    QString word (response);

    if (quizSpec.getType() == QuizSpec::QuizAnagramsWithHooks) {
        QStringList sections = response.split (":");
        if (sections.size() != 3) {
            addQuestionIncorrect (response);
            return Incorrect;
        }

        QString frontHooks = Auxil::getAlphagram (sections.at (0));
        word = sections.at (1);
        QString backHooks = Auxil::getAlphagram (sections.at (2));

        if ((frontHooks.toLower() != wordEngine->getFrontHookLetters (word))
            || (backHooks.toLower() != wordEngine->getBackHookLetters (word)))
        {
            addQuestionIncorrect (response);
            return Incorrect;
        }
    }

    if (correctResponses.find (word) == correctResponses.end()) {
        addQuestionIncorrect (response);
        return Incorrect;
    }

    if (correctUserResponses.find (word) != correctUserResponses.end())
        return Duplicate;

    addQuestionCorrect (word);
    return Correct;
}

//---------------------------------------------------------------------------
//  markQuestionAsMissed
//
//! Mark the current question as missed.
//---------------------------------------------------------------------------
void
QuizEngine::markQuestionAsMissed()
{
    // Remove any correct answers the user may have already had
    int numCorrect = correctUserResponses.size();
    quizCorrect -= numCorrect;

    correctUserResponses.clear();

    QuizProgress progress = quizSpec.getProgress();
    progress.clearQuestionCorrect();
    progress.setCorrect (progress.getNumCorrect() - numCorrect);
    quizSpec.setProgress (progress);
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
    return (questionIndex >= quizQuestions.size()) ? QString::null:
        quizQuestions.at (questionIndex);
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
    QuizSpec::QuizType type = quizSpec.getType();

    if (type == QuizSpec::QuizWordListRecall)
        answers = wordEngine->search (quizSpec.getSearchSpec(), true);
    else if ((type == QuizSpec::QuizAnagrams) ||
             (type == QuizSpec::QuizAnagramsWithHooks))
    {
        SearchCondition condition;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = question;
        SearchSpec spec;
        spec.conditions.append (condition);
        answers = wordEngine->search (spec, true);
    }
    else if (type == QuizSpec::QuizHooks) {
        SearchCondition condition;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "?" + question;
        SearchSpec spec;
        spec.conditions.append (condition);
        answers = wordEngine->search (spec, true);

        spec.conditions.clear();
        condition.stringValue = question + "?";
        spec.conditions.append (condition);
        answers += wordEngine->search (spec, true);
    }

    QStringList::iterator it;
    for (it = answers.begin(); it != answers.end(); ++it) {
        correctResponses.insert (*it);
    }
    quizTotal += correctResponses.size();
}

//---------------------------------------------------------------------------
//  addQuestionCorrect
//
//! Add a correct user response to the current question.
//
//! @param response the correct response
//---------------------------------------------------------------------------
void
QuizEngine::addQuestionCorrect (const QString& response)
{
    correctUserResponses.insert (response);
    ++quizCorrect;
    QuizProgress progress = quizSpec.getProgress();
    progress.addQuestionCorrect (response);
    progress.setCorrect (progress.getNumCorrect() + 1);
    quizSpec.setProgress (progress);
}

//---------------------------------------------------------------------------
//  addQuestionIncorrect
//
//! Add an incorrect user response to the current question.
//
//! @param response the incorrect response
//---------------------------------------------------------------------------
void
QuizEngine::addQuestionIncorrect (const QString& response)
{
    incorrectUserResponses << response;
    ++quizIncorrect;
    QuizProgress progress = quizSpec.getProgress();
    progress.addIncorrect (response);
    quizSpec.setProgress (progress);
}
