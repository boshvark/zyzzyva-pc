//---------------------------------------------------------------------------
// QuizEngine.cpp
//
// The engine for generating quizzes and keeping track of the user's
// performance on each quiz.
//
// Copyright 2004, 2005, 2006, 2007, 2008 Michael W Thelen <mthelen@gmail.com>.
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
#include "QuizDatabase.h"
#include "LetterBag.h"
#include "WordEngine.h"
#include "Auxil.h"
#include <cstdlib>

//---------------------------------------------------------------------------
//  probabilityCmp
//
//! A comparison function that sorts strings based on probability as a primary
//! key, and alphabetically as a secondary key.
//
//! @param a a string/combination pair
//! @param b another string/combination pair
//---------------------------------------------------------------------------
bool
probabilityCmp(const QPair<QString, double>& a, const QPair<QString, double>& b)
{
    if (a.second > b.second)
        return true;
    else if (b.second > a.second)
        return false;
    else
        return (a.first < b.first);
}

//---------------------------------------------------------------------------
//  QuizEngine
//
//! Constructor.
//---------------------------------------------------------------------------
QuizEngine::QuizEngine(WordEngine* e)
    : wordEngine(e), quizTotal(0), quizCorrect(0), quizIncorrect(0),
    questionIndex(0)
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
QuizEngine::newQuiz(const QuizSpec& spec)
{
    QStringList questions;
    QString lexicon = spec.getLexicon();

    if (spec.getQuizSourceType() == QuizSpec::RandomLettersSource) {
        LetterBag bag;

        // FIXME: get rid of all this hard-coded junk!

        for (int i = 0; i < 1000; ++i) {
            QString question = Auxil::getAlphagram(bag.lookRandomLetters(7));
            quizQuestions.append(question);
        }

        quizSpec = spec;
    }

    else if (spec.getQuizSourceType() == QuizSpec::CardboxReadySource) {
        QString quizType = Auxil::quizTypeToString(spec.getType());
        QuizDatabase* db = new QuizDatabase(lexicon, quizType);
        if (!db->isValid()) {
            delete db;
            return false;
        }
        bool zeroFirst = (spec.getQuestionOrder() ==
                          QuizSpec::ScheduleZeroFirstOrder);
        quizQuestions = db->getReadyQuestions(QStringList(), zeroFirst);
        delete db;
        if (quizQuestions.isEmpty())
            return false;
        quizSpec = spec;
    }

    else {
        // Anagram Quiz: The search spec is used to select the list of words.
        // Their alphagrams are used as quiz questions, and their anagrams are
        // used as quiz answers.
        QuizSpec::QuizType type = spec.getType();
        if ((type == QuizSpec::QuizAnagrams) ||
            (type == QuizSpec::QuizAnagramsWithHooks))
        {
            questions = wordEngine->search(lexicon, spec.getSearchSpec(), true);
            questions = wordEngine->alphagrams(questions);
        }

        else if (type == QuizSpec::QuizHooks) {
            questions = wordEngine->search(lexicon, spec.getSearchSpec(), true);
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
                    seed = QDateTime::currentDateTime().toTime_t();
                unsigned int seed2 = spec.getRandomSeed2();
                if (!seed2)
                    seed2 = Auxil::getPid();
                rng.setAlgorithm(spec.getRandomAlgorithm());
                rng.srand(seed, seed2);
                quizSpec.setRandomSeed(seed);
                quizSpec.setRandomSeed2(seed2);

                // XXX: We need a Shuffle class to handle shuffling using various
                // algorithms!
                QString tmp;
                int num = quizQuestions.size();
                for (int i = 0; i < num - 1; ++i) {
                    unsigned int randnum = rng.rand(num - i - 1);
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
                LetterBag letterBag;
                QList<QPair<QString, double> > questionPairs;

                foreach (QString question, quizQuestions) {
                    double combos = letterBag.getNumCombinations(question);
                    questionPairs.append(qMakePair(question, combos));
                }

                qSort(questionPairs.begin(), questionPairs.end(),
                      probabilityCmp);

                quizQuestions.clear();
                QListIterator<QPair<QString, double> > it (questionPairs);
                while (it.hasNext()) {
                    const QPair<QString, double>& questionPair = it.next();
                    quizQuestions.append(questionPair.first);
                }
            }
            break;

            case QuizSpec::ScheduleOrder:
            case QuizSpec::ScheduleZeroFirstOrder: {
                QString lexicon = spec.getLexicon();
                QString quizType = Auxil::quizTypeToString(spec.getType());
                QuizDatabase* db = new QuizDatabase(lexicon, quizType);
                if (!db->isValid()) {
                    delete db;
                    return false;
                }

                bool zeroFirst = (quizSpec.getQuestionOrder() ==
                                  QuizSpec::ScheduleZeroFirstOrder);
                quizQuestions = db->getReadyQuestions(quizQuestions, zeroFirst);
                delete db;

                if (quizQuestions.isEmpty())
                    return false;
            }
            break;

            default: break;
        }
    }

    // Restore quiz progress
    QuizProgress progress;
    if (spec.getMethod() != QuizSpec::CardboxQuizMethod)
        progress = spec.getProgress();

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
    // FIXME: For now, start a new quiz every time the user advances to the
    // next question during a cardbox quiz.  This works, but can be very slow
    // when the user has limited the quiz with a search specification.  Do
    // something smarter here!
    //if (quizSpec.getMethod() == QuizSpec::CardboxQuizMethod) {
    //    return newQuiz(quizSpec);
    //}

    if (onLastQuestion())
        return false;

    ++questionIndex;

    // Update progress
    QuizProgress progress = quizSpec.getProgress();
    progress.setQuestion(questionIndex);
    progress.setCorrect(quizCorrect);
    progress.setQuestionComplete(false);
    progress.clearQuestionCorrect();
    QStringList missed = getMissed();
    QStringList::iterator it;
    for (it = missed.begin(); it != missed.end(); ++it)
        progress.addMissed(*it);

    for (it = incorrectUserResponses.begin(); it !=
         incorrectUserResponses.end(); ++it)
    {
        progress.addIncorrect(*it);
    }
    quizSpec.setProgress(progress);

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
    progress.setQuestionComplete(true);
    quizSpec.setProgress(progress);
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
QuizEngine::respond(const QString& response)
{
    QString word (response);

    if (quizSpec.getType() == QuizSpec::QuizAnagramsWithHooks) {
        QStringList sections = response.split(":");
        if (sections.size() != 3) {
            addQuestionIncorrect(response);
            return Incorrect;
        }

        QString frontHooks = Auxil::getAlphagram(sections.at(0));
        word = sections.at(1);
        QString backHooks = Auxil::getAlphagram(sections.at(2));

        QString lexicon = quizSpec.getLexicon();
        QString frontAnswers = wordEngine->getFrontHookLetters(lexicon, word);
        QString backAnswers = wordEngine->getBackHookLetters(lexicon, word);

        if ((frontHooks.toLower() != frontAnswers) ||
            (backHooks.toLower() != backAnswers))
        {
            addQuestionIncorrect(response);
            return Incorrect;
        }
    }

    if (!correctResponses.contains(word)) {
        addQuestionIncorrect(response);
        return Incorrect;
    }

    if (correctUserResponses.contains(word))
        return Duplicate;

    addQuestionCorrect(word);
    return Correct;
}

//---------------------------------------------------------------------------
//  markQuestionAsCorrect
//
//! Mark the current question as correct.
//---------------------------------------------------------------------------
void
QuizEngine::markQuestionAsCorrect()
{
    // Remove any incorrect answers the user may have already had
    int numIncorrect = incorrectUserResponses.count();
    quizIncorrect -= numIncorrect;

    QuizProgress progress = quizSpec.getProgress();
    QStringListIterator it (incorrectUserResponses);
    while (it.hasNext()) {
        progress.removeIncorrect(it.next());
    }
    incorrectUserResponses.clear();

    QStringList missed = getMissed();
    QStringListIterator jt (missed);
    while (jt.hasNext()) {
        QString word = jt.next();
        correctUserResponses.insert(word);
        progress.addQuestionCorrect(word);
        if (progress.getQuestionComplete()) {
            progress.removeMissed(word);
        }
    }

    quizSpec.setProgress(progress);
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
    int numCorrect = correctUserResponses.count();
    quizCorrect -= numCorrect;

    correctUserResponses.clear();

    QuizProgress progress = quizSpec.getProgress();
    progress.clearQuestionCorrect();
    progress.setCorrect(progress.getNumCorrect() - numCorrect);
    quizSpec.setProgress(progress);
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
    return (questionIndex >= quizQuestions.size()) ? QString():
        quizQuestions.at(questionIndex);
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
    QStringList missedWords;
    QSetIterator<QString> it (correctResponses);
    while (it.hasNext()) {
        QString word = it.next();
        if (!correctUserResponses.contains(word))
            missedWords.append(word);
    }
    return missedWords;
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
    QString question = getQuestion().replace("_", "?");

    QStringList answers;
    QuizSpec::QuizType type = quizSpec.getType();
    QString lexicon = quizSpec.getLexicon();

    if (type == QuizSpec::QuizWordListRecall)
        answers = wordEngine->search(lexicon, quizSpec.getSearchSpec(), true);
    else if (type == QuizSpec::QuizBuild) {
        int min = quizSpec.getResponseMinLength();
        int max = quizSpec.getResponseMaxLength();
        int qlen = question.length();

        if (min <= qlen) {
            SearchSpec spec;
            SearchCondition condition;
            condition.type = SearchCondition::Length;
            condition.minValue = min;
            condition.maxValue = qlen;
            spec.conditions.append(condition);
            condition = SearchCondition();
            condition.type = SearchCondition::SubanagramMatch;
            condition.stringValue = question;
            spec.conditions.append(condition);
            answers += wordEngine->search(lexicon, spec, true);
        }

        if (max > qlen) {
            SearchSpec spec;
            SearchCondition condition;
            condition.type = SearchCondition::Length;
            condition.minValue = qlen + 1;
            condition.maxValue = max;
            spec.conditions.append(condition);
            condition = SearchCondition();
            condition.type = SearchCondition::AnagramMatch;
            condition.stringValue = question + "*";
            spec.conditions.append(condition);
            answers += wordEngine->search(lexicon, spec, true);
        }
    }
    else if ((type == QuizSpec::QuizAnagrams) ||
             (type == QuizSpec::QuizAnagramsWithHooks))
    {
        SearchCondition condition;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = question;
        SearchSpec spec;
        spec.conditions.append(condition);
        answers = wordEngine->search(lexicon, spec, true);
    }
    else if (type == QuizSpec::QuizHooks) {
        SearchCondition condition;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "?" + question;
        SearchSpec spec;
        spec.conditions.append(condition);
        answers = wordEngine->search(lexicon, spec, true);

        spec.conditions.clear();
        condition.stringValue = question + "?";
        spec.conditions.append(condition);
        answers += wordEngine->search(lexicon, spec, true);
    }

    correctResponses += answers.toSet();
    quizTotal += correctResponses.count();
}

//---------------------------------------------------------------------------
//  addQuestionCorrect
//
//! Add a correct user response to the current question.
//
//! @param response the correct response
//---------------------------------------------------------------------------
void
QuizEngine::addQuestionCorrect(const QString& response)
{
    correctUserResponses.insert(response);
    ++quizCorrect;
    QuizProgress progress = quizSpec.getProgress();
    progress.addQuestionCorrect(response);
    progress.setCorrect(progress.getNumCorrect() + 1);
    quizSpec.setProgress(progress);
}

//---------------------------------------------------------------------------
//  addQuestionIncorrect
//
//! Add an incorrect user response to the current question.
//
//! @param response the incorrect response
//---------------------------------------------------------------------------
void
QuizEngine::addQuestionIncorrect(const QString& response)
{
    incorrectUserResponses << response;
    ++quizIncorrect;
    QuizProgress progress = quizSpec.getProgress();
    progress.addIncorrect(response);
    quizSpec.setProgress(progress);
}
