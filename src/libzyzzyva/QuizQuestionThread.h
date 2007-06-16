//---------------------------------------------------------------------------
// QuizQuestionThread.h
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

#ifndef ZYZZYVA_QUIZ_QUESTION_THREAD_H
#define ZYZZYVA_QUIZ_QUESTION_THREAD_H

#include <QMap>
#include <QString>
#include <QThread>

#include "QuizSpec.h"

class WordEngine;

class QuizQuestionThread : public QThread
{
    Q_OBJECT
    public:
    QuizQuestionThread(WordEngine* e, const QuizSpec& qs, QObject* parent = 0)
        : QThread(parent), wordEngine(e), quizSpec(qs) { }
    ~QuizQuestionThread() { }

    QStringList getAnswers(const QString& question);
    void fetchAnswers(const QString& question);
    void removeAnswers(const QString& question);

    protected:
    void run();

    WordEngine* wordEngine;
    QuizSpec quizSpec;
    QMap<QString, QStringList> answers;
};

#endif // ZYZZYVA_QUIZ_QUESTION_THREAD_H
