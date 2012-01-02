//---------------------------------------------------------------------------
// QuizTimerSpec.h
//
// A class to represent a quiz timer specification.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_QUIZ_TIMER_SPEC_H
#define ZYZZYVA_QUIZ_TIMER_SPEC_H

#include <QDomElement>
#include <QString>

enum QuizTimerType {
    NoTimer = 0,
    PerQuestion,
    PerResponse
};

class QuizTimerSpec
{
    public:
    QuizTimerSpec() : type(NoTimer), duration(0) { }
    ~QuizTimerSpec() { }

    QString asString() const;
    QDomElement asDomElement() const;
    bool fromDomElement(const QDomElement& element);

    void setType(QuizTimerType t) { type = t; }
    void setDuration(int d) { duration = d; }

    QuizTimerType getType() const { return type; }
    int getDuration() const { return duration; }

    private:
    QString typeToString(const QuizTimerType t) const;
    QuizTimerType stringToType(const QString& s) const;

    private:
    QuizTimerType type;
    int duration;
};

#endif // ZYZZYVA_QUIZ_TIMER_SPEC_H
