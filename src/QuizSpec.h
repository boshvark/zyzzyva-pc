//---------------------------------------------------------------------------
// QuizSpec.h
//
// A class to represent a quiz specification.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef QUIZ_SPEC_H
#define QUIZ_SPEC_H

#include "QuizProgress.h"
#include "QuizTimerSpec.h"
#include "SearchSpec.h"
#include "Defs.h"

enum QuizType {
    UnknownQuizType = 0,
    QuizPatterns,
    QuizAnagrams,
    QuizSubanagrams,
    QuizHooks,
    QuizAnagramHooks,
    QuizAnagramHookMnemonics
};

class QuizSpec
{
    public:
    QuizSpec() : type (QuizAnagrams), useList (false), randomOrder (true)
        { }
    ~QuizSpec() { }

    QString asString() const;
    QDomElement asDomElement() const;
    bool fromDomElement (const QDomElement& element);

    void setType (QuizType t) { type = t; }
    void setSearchSpec (const SearchSpec& s) { searchSpec = s; }
    void setTimerSpec (const QuizTimerSpec& s) { timerSpec = s; }
    void setProgress (const QuizProgress& p) { progress = p; }
    void setUseList (bool b) { useList = b; }
    void setRandomOrder (bool b) { randomOrder = b; }

    QuizType getType() const { return type; }
    SearchSpec getSearchSpec() const { return searchSpec; }
    QuizTimerSpec getTimerSpec() const { return timerSpec; }
    QuizProgress getProgress() const { return progress; }
    bool getUseList() const { return useList; }
    bool getRandomOrder() const { return randomOrder; }

    private:
    QuizType type;
    SearchSpec searchSpec;
    QuizTimerSpec timerSpec;
    QuizProgress progress;
    bool useList;
    bool randomOrder;
};

#endif // QUIZ_SPEC_H
