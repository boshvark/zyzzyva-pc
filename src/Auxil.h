//---------------------------------------------------------------------------
// Auxil.h
//
// Auxiliary functions.
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

#ifndef ZYZZYVA_AUXIL_H
#define ZYZZYVA_AUXIL_H

#include "QuizSpec.h"
#include "SearchCondition.h"
#include "SearchSet.h"
#include <QDate>
#include <QString>

namespace Auxil {
    unsigned int getPid();
    QString getAboutString();
    QString getThanksString();
    QString getRootDir();
    QString getHelpDir();
    QString getQuizDir();
    QString getSearchDir();
    QString getTilesDir();
    QString getWordsDir();
    QString getUserWordsDir();
    QString getUserDir();
    QString wordWrap (const QString& str, int wrapLength);
    bool isVowel (QChar c);
    QString getAlphagram (const QString& word);
    QString searchSetToString (SearchSet set);
    SearchSet stringToSearchSet (const QString& string);
    QString searchTypeToString (SearchCondition::SearchType type);
    SearchCondition::SearchType stringToSearchType (const QString& string);
    QString quizTypeToString (QuizSpec::QuizType type);
    QuizSpec::QuizType stringToQuizType (const QString& string);
    QDate lexiconToDate (const QString& lexicon);
}

#endif // ZYZZYVA_AUXIL_H
