//---------------------------------------------------------------------------
// QuizSpec.h
//
// A class to represent a quiz specification.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef ZYZZYVA_QUIZ_SPEC_H
#define ZYZZYVA_QUIZ_SPEC_H

#include "QuizProgress.h"
#include "QuizTimerSpec.h"
#include "Rand.h"
#include "SearchSpec.h"
#include "Defs.h"

class QuizSpec
{
    public:
    enum QuizType {
        UnknownQuizType = 0,
        QuizPatterns,
        QuizAnagrams,
        QuizAnagramsWithHooks,
        QuizSubanagrams,
        QuizAnagramJumble,
        QuizSubanagramJumble,
        QuizHooks,
        QuizAnagramHooks,
        QuizAnagramHookMnemonics,
        QuizWordListRecall
    };

    public:
    QuizSpec() : type (QuizAnagrams), randomOrder (true), randomSeed (0),
                 randomSeed2 (0), randomAlgorithm (Rand::MarsagliaMwc) { }
    ~QuizSpec() { }

    QString asString() const;
    QDomElement asDomElement() const;
    bool fromDomElement (const QDomElement& element);

    void setLexicon (const QString& lex) { lexicon = lex; }
    void setType (QuizType t) { type = t; }
    void setSearchSpec (const SearchSpec& s) { searchSpec = s; }
    void setTimerSpec (const QuizTimerSpec& s) { timerSpec = s; }
    void setProgress (const QuizProgress& p) { progress = p; }
    void setRandomOrder (bool b) { randomOrder = b; }
    void setRandomSeed (unsigned int i) { randomSeed = i; }
    void setRandomSeed2 (unsigned int i) { randomSeed2 = i; }
    void setRandomAlgorithm (int i) { randomAlgorithm = i; }

    void addIncorrect (const QString& word) { progress.addIncorrect (word); }
    void addMissed (const QString& word) { progress.addMissed (word); }

    QString getLexicon() const { return lexicon; }
    QuizType getType() const { return type; }
    SearchSpec getSearchSpec() const { return searchSpec; }
    QuizTimerSpec getTimerSpec() const { return timerSpec; }
    QuizProgress getProgress() const { return progress; }
    bool getRandomOrder() const { return randomOrder; }
    unsigned int getRandomSeed() const { return randomSeed; }
    unsigned int getRandomSeed2() const { return randomSeed2; }
    int getRandomAlgorithm() const { return randomAlgorithm; }

    private:
    QString lexicon;
    QuizType type;
    SearchSpec searchSpec;
    QuizTimerSpec timerSpec;
    QuizProgress progress;
    bool randomOrder;
    unsigned int randomSeed;
    unsigned int randomSeed2;
    int randomAlgorithm;
};

#endif // ZYZZYVA_QUIZ_SPEC_H
