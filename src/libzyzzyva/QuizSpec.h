//---------------------------------------------------------------------------
// QuizSpec.h
//
// A class to represent a quiz specification.
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

#ifndef ZYZZYVA_QUIZ_SPEC_H
#define ZYZZYVA_QUIZ_SPEC_H

#include "QuizProgress.h"
#include "QuizTimerSpec.h"
#include "Rand.h"
#include "SearchSpec.h"
#include <QFile>
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
        QuizBuild,
        QuizAnagramJumble,
        QuizSubanagramJumble,
        QuizHooks,
        QuizAnagramHooks,
        QuizAnagramHookMnemonics,
        QuizWordListRecall
    };

    enum QuizMethod {
        UnknownQuizMethod = 0,
        StandardQuizMethod,
        CardboxQuizMethod
    };

    enum QuizSourceType {
        UnknownSource = 0,
        SearchSource,
        CardboxReadySource,
        RandomLettersSource
    };

    enum QuestionOrder {
        UnknownOrder = 0,
        RandomOrder,
        AlphabeticalOrder,
        ProbabilityOrder,
        PlayabilityOrder,
        ScheduleOrder,
        ScheduleZeroFirstOrder,
    };

    public:
    QuizSpec() : type(QuizAnagrams), method(StandardQuizMethod),
                 sourceType(SearchSource), questionOrder(RandomOrder),
                 probNumBlanks(0), randomSeed(0), randomSeed2(0),
                 randomAlgorithm(Rand::MarsagliaMwc),
                 responseMinLength(0), responseMaxLength(0) { }
    ~QuizSpec() { }

    QString asString() const;
    QString asXml() const;
    QDomElement asDomElement() const;
    bool fromDomElement(const QDomElement& element, QString* errStr = 0);
    bool fromXmlFile(QFile& file, QString* errStr = 0);

    void setLexicon(const QString& lex) { lexicon = lex; }
    void setType(QuizType t) { type = t; }
    void setMethod(QuizMethod m) { method = m; }
    void setQuizSourceType(QuizSourceType s) { sourceType = s; }
    void setSearchSpec(const SearchSpec& s) { searchSpec = s; }
    void setTimerSpec(const QuizTimerSpec& s) { timerSpec = s; }
    void setProgress(const QuizProgress& p) { progress = p; }
    void setQuestionOrder(QuestionOrder o) { questionOrder = o; }
    void setProbabilityNumBlanks(int i) { probNumBlanks = i; }
    void setRandomSeed(unsigned int i) { randomSeed = i; }
    void setRandomSeed2(unsigned int i) { randomSeed2 = i; }
    void setRandomAlgorithm(int i) { randomAlgorithm = i; }
    void setResponseMinLength(int i) { responseMinLength = i; }
    void setResponseMaxLength(int i) { responseMaxLength = i; }
    void setFilename(const QString& fname) { filename = fname; }

    void addIncorrect(const QString& word) { progress.addIncorrect(word); }
    void addMissed(const QString& word) { progress.addMissed(word); }

    QString getLexicon() const { return lexicon; }
    QuizType getType() const { return type; }
    QuizMethod getMethod() const { return method; }
    QuizSourceType getQuizSourceType() const { return sourceType; }
    SearchSpec getSearchSpec() const { return searchSpec; }
    QuizTimerSpec getTimerSpec() const { return timerSpec; }
    QuizProgress getProgress() const { return progress; }
    QuestionOrder getQuestionOrder() const { return questionOrder; }
    int getProbabilityNumBlanks() const { return probNumBlanks; }
    unsigned int getRandomSeed() const { return randomSeed; }
    unsigned int getRandomSeed2() const { return randomSeed2; }
    int getRandomAlgorithm() const { return randomAlgorithm; }
    int getResponseMinLength() const { return responseMinLength; }
    int getResponseMaxLength() const { return responseMaxLength; }
    QString getFilename() const { return filename; }

    private:
    QString lexicon;
    QuizType type;
    QuizMethod method;
    QuizSourceType sourceType;
    SearchSpec searchSpec;
    QuizTimerSpec timerSpec;
    QuizProgress progress;
    QuestionOrder questionOrder;
    int probNumBlanks;
    unsigned int randomSeed;
    unsigned int randomSeed2;
    int randomAlgorithm;
    int responseMinLength;
    int responseMaxLength;
    QString filename;
};

#endif // ZYZZYVA_QUIZ_SPEC_H
