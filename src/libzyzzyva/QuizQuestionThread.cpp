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
//  run
//
//! Run the thread.
//---------------------------------------------------------------------------
void
QuizQuestionThread::run()
{
    exec();
}

//---------------------------------------------------------------------------
//  getAnswers
//
//! Retrieve answers for a quiz question.  Fetch them if necessary.
//
//! @param question the quiz question
//! @return a list of answers
//---------------------------------------------------------------------------
QStringList
QuizQuestionThread::getAnswers(const QString& question)
{
    fetchAnswers(question);
    return answers.value(question);
}

//---------------------------------------------------------------------------
//  fetchAnswers
//
//! Fetch answers to a quiz question, and store them.
//
//! @param question the quiz question
//---------------------------------------------------------------------------
void
QuizQuestionThread::fetchAnswers(const QString& question)
{

}

//---------------------------------------------------------------------------
//  removeAnswers
//
//! Remove answers for a quiz question.
//
//! @param question the quiz question
//---------------------------------------------------------------------------
void
QuizQuestionThread::removeAnswers(const QString& question)
{
    answers.remove(question);
}
