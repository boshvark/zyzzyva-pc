//---------------------------------------------------------------------------
// QuizQuestion.h
//
// A class to represent a quiz question.
//
// Copyright 2009-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_QUIZ_QUESTION_H
#define ZYZZYVA_QUIZ_QUESTION_H

#include <QtXml/QDomElement>
#include <QtCore/QSet>

class QuizQuestion
{
    public:
    QuizQuestion() : complete(false), correct(false) { }
    ~QuizQuestion() { }

    void setQuestion(const QString& q) { question = q; }
    void setComplete(bool b) { complete = b; }
    void setCorrect(bool b) { correct = b; }
    void setResponses(const QSet<QString>& rlist) { responses = rlist; }
    void addResponse(const QString& word);

    QString getQuestion() const { return question; }
    bool getComplete() const { return complete; }
    bool getCorrect() const { return correct; }
    QSet<QString> getResponses() const { return responses; }

    QDomElement asDomElement() const;
    bool fromDomElement(const QDomElement& element);

    private:
    QString question;
    bool complete;
    bool correct;
    QSet<QString> responses;
};

#endif // ZYZZYVA_QUIZ_QUESTION_H

