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
    enum QuizStatus {
        QuizStatusTemporary = 0,
        QuizStatusUnsaved = 1,
        QuizStatusSaved = 2
    };

    enum QuizType {
        UnknownQuizType = 0,
        QuizPatterns = 1,
        QuizAnagrams = 2,
        QuizAnagramsWithHooks = 3,
        QuizSubanagrams = 4,
        QuizBuild = 5,
        QuizAnagramJumble = 6,
        QuizSubanagramJumble = 7,
        QuizHooks = 8,
        QuizAnagramHooks = 9,
        QuizAnagramHookMnemonics = 10,
        QuizWordListRecall = 11
    };

    enum QuizMethod {
        QuizMethodUnknown = 0,
        QuizMethodStandard = 1,
        QuizMethodCardbox = 2
    };

    enum QuizSource {
        QuizSourceUnknown = 0,
        QuizSourceSearch = 1,
        QuizSourceCardboxReady = 2,
        QuizSourceRandomLetters = 3
    };

    enum QuizOrder {
        QuizOrderUnknown = 0,
        QuizOrderRandom = 1,
        QuizOrderAlphabetical = 2,
        QuizOrderProbability = 3,
        QuizOrderPlayability = 4,
        QuizOrderSchedule = 5,
        QuizOrderScheduleZeroFirst = 6,
        QuizOrderCardboxAscending = 7,
        QuizOrderCardboxDescending =  8,
        QuizOrderCardboxAscendingLengthAscending = 9,
        QuizOrderCardboxAscendingLengthDescending = 10
    };

    public:
    QuizSpec() : type(QuizAnagrams), method(QuizMethodStandard),
                 source(QuizSourceSearch), quizOrder(QuizOrderRandom),
                 questionIndex(0), numWords(0), numQuestions(0),
                 probNumBlanks(0), randomSeed(0), randomSeed2(0),
                 randomAlgorithm(Rand::MarsagliaMwc),
                 responseMinLength(0), responseMaxLength(0) { }
    ~QuizSpec() { }

    QString asString() const;
    QString asXml() const;
    QDomElement asDomElement() const;
    bool fromDomElement(const QDomElement& element, QString* errStr = 0);
    bool fromXmlFile(QFile& file, QString* errStr = 0);

    void setFilename(const QString& fname) { filename = fname; }
    void setLexicon(const QString& lex) { lexicon = lex; }
    void setType(QuizType t) { type = t; }
    void setMethod(QuizMethod m) { method = m; }
    void setQuizSource(QuizSource s) { source = s; }
    void setTimerSpec(const QuizTimerSpec& s) { timerSpec = s; }
    void setQuizOrder(QuizOrder o) { quizOrder = o; }
    void setQuestionIndex(int i) { questionIndex = i; }
    void setNumWords(int n) { numWords = n; }
    void setNumQuestions(int n) { numQuestions = n; }

    // Legacy
    void setSearchSpec(const SearchSpec& s) { searchSpec = s; }
    void setProgress(const QuizProgress& p) { progress = p; }
    void setProbabilityNumBlanks(int i) { probNumBlanks = i; }
    void setRandomSeed(unsigned int i) { randomSeed = i; }
    void setRandomSeed2(unsigned int i) { randomSeed2 = i; }
    void setRandomAlgorithm(int i) { randomAlgorithm = i; }
    void setResponseMinLength(int i) { responseMinLength = i; }
    void setResponseMaxLength(int i) { responseMaxLength = i; }

    void addIncorrect(const QString& word) { progress.addIncorrect(word); }
    void addMissed(const QString& word) { progress.addMissed(word); }

    QString getFilename() const { return filename; }
    QString getLexicon() const { return lexicon; }
    QuizType getType() const { return type; }
    QuizMethod getMethod() const { return method; }
    QuizSource getQuizSource() const { return source; }
    QuizTimerSpec getTimerSpec() const { return timerSpec; }
    QuizOrder getQuizOrder() const { return quizOrder; }
    int getQuestionIndex() const { return questionIndex; }
    int getNumWords() const { return numWords; }
    int getNumQuestions() const { return numQuestions; }

    // Legacy
    SearchSpec getSearchSpec() const { return searchSpec; }
    QuizProgress getProgress() const { return progress; }
    int getProbabilityNumBlanks() const { return probNumBlanks; }
    unsigned int getRandomSeed() const { return randomSeed; }
    unsigned int getRandomSeed2() const { return randomSeed2; }
    int getRandomAlgorithm() const { return randomAlgorithm; }
    int getResponseMinLength() const { return responseMinLength; }
    int getResponseMaxLength() const { return responseMaxLength; }

    private:
    QString filename;
    QString name;
    QString lexicon;
    QuizType type;
    QuizMethod method;
    QuizSource source;
    QuizTimerSpec timerSpec;
    QuizOrder quizOrder;
    int questionIndex;
    int numWords;
    int numQuestions;

    // Legacy? Currently used for loading and starting quizzes from XML files.
    // Can probably be moved out of QuizSpec class into legacy helper class.
    SearchSpec searchSpec;
    QuizProgress progress;

    int probNumBlanks;
    unsigned int randomSeed;
    unsigned int randomSeed2;
    int randomAlgorithm;
    int responseMinLength;
    int responseMaxLength;
};

#endif // ZYZZYVA_QUIZ_SPEC_H
