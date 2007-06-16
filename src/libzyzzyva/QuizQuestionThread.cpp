//---------------------------------------------------------------------------
// QuizQuestionThread.cpp
//
// A class for caching quiz question answers for quick retrieval.
//
// Copyright 2007 Michael W Thelen <mthelen@gmail.com>.
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

#include "QuizQuestionThread.h"
#include "WordEngine.h"
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

//---------------------------------------------------------------------------
//  ~QuizQuestionThread
//
//! Destructor.
//---------------------------------------------------------------------------
QuizQuestionThread::~QuizQuestionThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}

//---------------------------------------------------------------------------
//  run
//
//! Run the thread.
//---------------------------------------------------------------------------
void
QuizQuestionThread::run()
{
    forever {
        if (queue.isEmpty()) {
            mutex.lock();
            condition.wait(&mutex);
            mutex.unlock();
        }

        if (abort)
            return;

        mutex.lock();
        QString question = queue.dequeue();
        mutex.unlock();

        QStringList answerList = getAnswers(question);
        emit fetchedAnswers(question, answerList);
    }
}

//---------------------------------------------------------------------------
//  setQuizSpec
//
//! Set the quiz specification to be used.  Also clear the answer cache.
//
//! @param qs the new quiz spec
//---------------------------------------------------------------------------
void
QuizQuestionThread::setQuizSpec(const QuizSpec& qs)
{
    mutex.lock();
    quizSpec = qs;
    mutex.unlock();
}

//---------------------------------------------------------------------------
//  fetchAnswers
//
//! Add a question to the list of questions for which answers are requested.
//! Wake up the thread to start retrieving  the answers.
//
//! @param question the quiz question
//---------------------------------------------------------------------------
void
QuizQuestionThread::fetchAnswers(const QString& question)
{
    mutex.lock();
    queue.enqueue(question);
    mutex.unlock();

    if (!isRunning()) {
        start();
    }
    else {
        condition.wakeOne();
    }
}

//---------------------------------------------------------------------------
//  getAnswers
//
//! Get answers to a quiz question.
//
//! @param question the quiz question
//! @return the list of answers
//---------------------------------------------------------------------------
QStringList
QuizQuestionThread::getAnswers(const QString& question)
{
    mutex.lock();
    QuizSpec::QuizType type = quizSpec.getType();
    mutex.unlock();

    QString realQuestion = question;
    realQuestion.replace("_", "?");

    QStringList answerList;

    if (type == QuizSpec::QuizWordListRecall)
        answerList = wordEngine->search(quizSpec.getSearchSpec(), true);
    else if (type == QuizSpec::QuizBuild) {
        int min = quizSpec.getResponseMinLength();
        int max = quizSpec.getResponseMaxLength();
        int qlen = realQuestion.length();

        if (min <= qlen) {
            SearchSpec spec;
            SearchCondition condition;
            condition.type = SearchCondition::Length;
            condition.minValue = min;
            condition.maxValue = qlen;
            spec.conditions.append(condition);
            condition = SearchCondition();
            condition.type = SearchCondition::SubanagramMatch;
            condition.stringValue = realQuestion;
            spec.conditions.append(condition);
            answerList += wordEngine->search(spec, true);
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
            condition.stringValue = realQuestion + "*";
            spec.conditions.append(condition);
            answerList += wordEngine->search(spec, true);
        }
    }
    else if ((type == QuizSpec::QuizAnagrams) ||
             (type == QuizSpec::QuizAnagramsWithHooks))
    {
        SearchCondition condition;
        condition.type = SearchCondition::AnagramMatch;
        condition.stringValue = realQuestion;
        SearchSpec spec;
        spec.conditions.append(condition);
        answerList = wordEngine->search(spec, true);
    }
    else if (type == QuizSpec::QuizHooks) {
        SearchCondition condition;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "?" + realQuestion;
        SearchSpec spec;
        spec.conditions.append(condition);
        answerList = wordEngine->search(spec, true);

        spec.conditions.clear();
        condition.stringValue = realQuestion + "?";
        spec.conditions.append(condition);
        answerList += wordEngine->search(spec, true);
    }

    return answerList;
}
