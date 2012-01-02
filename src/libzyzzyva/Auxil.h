//---------------------------------------------------------------------------
// Auxil.h
//
// Auxiliary functions.
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

#ifndef ZYZZYVA_AUXIL_H
#define ZYZZYVA_AUXIL_H

#include "LexiconStyle.h"
#include "QuizSpec.h"
#include "SearchCondition.h"
#include "SearchSet.h"
#include "WordAttribute.h"
#include "WordListFormat.h"
#include <QDate>
#include <QString>

namespace Auxil {
    bool copyDir(const QString& src, const QString& dest);
    unsigned int getPid();
    QString getAboutString();
    QString getThanksString();
    QString getRootDir();
    QString getHelpDir();
    QString getHomeDir();
    QString getQuizDir();
    QString getSearchDir();
    QString getTilesDir();
    QString getWordsDir();
    QString getUserWordsDir();
    QString getUserDir();
    QString getUserConfigDir();
    QString getLexiconPrefix(const QString& lexicon);
    QString getDatabaseFilename(const QString& lexicon);
    QString dialogWordWrap(const QString& str);
    QString wordWrap(const QString& str, int wrapLength);
    bool isVowel(QChar c);
    QString getAlphagram(const QString& word);
    QString getCanonicalSearchString(const QString& str);
    int getNumUniqueLetters(const QString& word);
    int getNumVowels(const QString& word);
    QString searchSetToString(SearchSet set);
    SearchSet stringToSearchSet(const QString& string);
    QString searchTypeToString(SearchCondition::SearchType type);
    SearchCondition::SearchType stringToSearchType(const QString& string);
    QString quizTypeToString(QuizSpec::QuizType type);
    QuizSpec::QuizType stringToQuizType(const QString& string);
    QString quizMethodToString(QuizSpec::QuizMethod method);
    QuizSpec::QuizMethod stringToQuizMethod(const QString& string);
    QString quizSourceTypeToString(QuizSpec::QuizSourceType source);
    QuizSpec::QuizSourceType stringToQuizSourceType(const QString& string);
    QString quizQuestionOrderToString(QuizSpec::QuestionOrder method);
    QuizSpec::QuestionOrder stringToQuizQuestionOrder(const QString& string);
    QString wordAttributeToString(WordAttribute attr);
    WordAttribute stringToWordAttribute(const QString& string);
    QString wordListFormatToString(WordListFormat format);
    WordListFormat stringToWordListFormat(const QString& string);
    QString lexiconStyleToString(const LexiconStyle& style);
    LexiconStyle stringToLexiconStyle(const QString& string);
    QString lexiconToOrigin(const QString& lexicon);
    QDate lexiconToDate(const QString& lexicon);
    QString lexiconToDetails(const QString& lexicon);
    QString getUpdatedLexiconName(const QString& oldLexiconName);
    bool lessThanVersion(const QString& a, const QString& b);
    bool getVersionComponents(const QString& version, int& major, int& minor,
        int& revision);
    bool localeAwareLessThanQString(const QString& a, const QString& b);
    bool localeAwareLessThanQChar(const QChar& a, const QChar& b);
}

#endif // ZYZZYVA_AUXIL_H
