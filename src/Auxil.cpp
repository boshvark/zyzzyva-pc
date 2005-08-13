//---------------------------------------------------------------------------
// Auxil.cpp
//
// Auxiliary functions.
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

#include "Auxil.h"
#include <qapplication.h>
#include <qfile.h>

const QString SET_UNKNOWN_STRING = "Unknown";
const QString SET_HOOK_WORDS_STRING = "Hook Words";
const QString SET_FRONT_HOOKS_STRING = "Front Hooks";
const QString SET_BACK_HOOKS_STRING = "Back Hooks";
const QString SET_TYPE_ONE_SEVENS_STRING = "Type I Sevens";
const QString SET_TYPE_TWO_SEVENS_STRING = "Type II Sevens";
const QString SET_TYPE_THREE_SEVENS_STRING = "Type III Sevens";
const QString SET_TYPE_ONE_EIGHTS_STRING = "Type I Eights";
const QString SET_EIGHTS_FROM_SEVEN_STEMS_STRING =
              "Eights From Seven-Letter Stems";

const QString PATTERN_MATCH_TYPE = "Pattern Match";
const QString ANAGRAM_MATCH_TYPE = "Anagram Match";
const QString SUBANAGRAM_MATCH_TYPE = "Subanagram Match";
const QString EXACT_LENGTH_TYPE = "Exact Length";
const QString MIN_LENGTH_TYPE = "Minimum Length";
const QString MAX_LENGTH_TYPE = "Maximum Length";
const QString MUST_INCLUDE_TYPE = "Must Include";
const QString MUST_EXCLUDE_TYPE = "Must Exclude";
const QString MUST_CONSIST_TYPE = "Must Consist of";
const QString MUST_BELONG_TYPE = "Must Belong to";
const QString IN_WORD_LIST_TYPE = "Must Be in Word List";
const QString EXACT_ANAGRAMS_TYPE = "Exact Anagrams";
const QString MIN_ANAGRAMS_TYPE = "Minimum Anagrams";
const QString MAX_ANAGRAMS_TYPE = "Maximum Anagrams";
const QString MIN_PROBABILITY_TYPE = "Minimum Probability";
const QString MAX_PROBABILITY_TYPE = "Maximum Probability";

//---------------------------------------------------------------------------
//  getAboutString
//
//! Return the About string for the application.  Read it from a file
//! initially, and store the read value in a static variable.
//
//! @return the About string
//---------------------------------------------------------------------------
QString
Auxil::getAboutString()
{
    static QString aboutString;
    if (!aboutString.isEmpty())
        return aboutString;

    QString aboutFileName = getHelpDir() + "/about.html";
    QFile aboutFile (aboutFileName);
    if (!aboutFile.open (IO_ReadOnly | IO_Translate))
        return aboutString;

    aboutString.append (aboutFile.readAll());
    return aboutString;
}

//---------------------------------------------------------------------------
//  getHelpDir
//
//! Return the top-level directory containing the help documentation.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getHelpDir()
{
    return qApp->applicationDirPath() + "/data/help";
}

//---------------------------------------------------------------------------
//  getQuizDir
//
//! Return the top-level directory containing subdirectories with quiz
//! specifications.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getQuizDir()
{
    return qApp->applicationDirPath() + "/data/quiz";
}

//---------------------------------------------------------------------------
//  getSearchDir
//
//! Return the top-level directory containing subdirectories with search
//! specifications.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getSearchDir()
{
    return qApp->applicationDirPath() + "/data/search";
}

//---------------------------------------------------------------------------
//  getTilesDir
//
//! Return the top-level directory containing subdirectories with tiles
//! images.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getTilesDir()
{
    return qApp->applicationDirPath() + "/data/tiles";
}

//---------------------------------------------------------------------------
//  getWordsDir
//
//! Return the top-level directory containing subdirectories with word lists.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getWordsDir()
{
    return qApp->applicationDirPath() + "/data/words";
}

//---------------------------------------------------------------------------
//  wordWrap
//
//! Wrap a string so that no line is longer than a certain length.
//
//! @param str the string to wrap
//! @param wrapLength the maximum length of a line
//! @return the wrapped string
//---------------------------------------------------------------------------
QString
Auxil::wordWrap (const QString& str, int wrapLength)
{
    int strLen = str.length();
    if (strLen <= wrapLength)
        return str;

    QChar c;
    QString wrappedStr = str;
    int lastSpace = 0;
    int lastNewline = 0;
    for (int i = 0; i < strLen; ++i) {
        c = wrappedStr.at (i);
        if (c == '\n')
            lastNewline = i;
        if (c.isSpace())
            lastSpace = i;

        if ((i - lastNewline) == wrapLength) {
            wrappedStr[lastSpace] = '\n';
            lastNewline = lastSpace;
        }
    }
    return wrappedStr;
}

//---------------------------------------------------------------------------
//  stringToSearchSet
//
//! Return a search set enumeration value corresponding to a string.
//
//! @param string the string
//! @return the corresponding search set value
//---------------------------------------------------------------------------
SearchSet
Auxil::stringToSearchSet (const QString& string)
{
    if (string == SET_HOOK_WORDS_STRING)
        return SetHookWords;
    else if (string == SET_FRONT_HOOKS_STRING)
        return SetFrontHooks;
    else if (string == SET_BACK_HOOKS_STRING)
        return SetBackHooks;
    else if (string == SET_TYPE_ONE_SEVENS_STRING)
        return SetTypeOneSevens;
    else if (string == SET_TYPE_TWO_SEVENS_STRING)
        return SetTypeTwoSevens;
    else if (string == SET_TYPE_THREE_SEVENS_STRING)
        return SetTypeThreeSevens;
    else if (string == SET_TYPE_ONE_EIGHTS_STRING)
        return SetTypeOneEights;
    else if (string == SET_EIGHTS_FROM_SEVEN_STEMS_STRING)
        return SetEightsFromSevenLetterStems;
    else
        return UnknownSearchSet;
}

//---------------------------------------------------------------------------
//  searchSetToString
//
//! Return a string corresponding to a search set enumeration value.
//
//! @param ss the search set value
//! @return the corresponding string
//---------------------------------------------------------------------------
QString
Auxil::searchSetToString (SearchSet ss)
{
    switch (ss) {
        case SetHookWords: return SET_HOOK_WORDS_STRING;
        case SetFrontHooks: return SET_FRONT_HOOKS_STRING;
        case SetBackHooks: return SET_BACK_HOOKS_STRING;
        case SetTypeOneSevens: return SET_TYPE_ONE_SEVENS_STRING;
        case SetTypeTwoSevens: return SET_TYPE_TWO_SEVENS_STRING;
        case SetTypeThreeSevens: return SET_TYPE_THREE_SEVENS_STRING;
        case SetTypeOneEights: return SET_TYPE_ONE_EIGHTS_STRING;
        case SetEightsFromSevenLetterStems:
            return SET_EIGHTS_FROM_SEVEN_STEMS_STRING;
        default: return SET_UNKNOWN_STRING;
    }
}

//---------------------------------------------------------------------------
//  stringToSearchType
//
//! Return a search type enumeration value corresponding to a string.
//
//! @param string the string
//! @return the corresponding search type value
//---------------------------------------------------------------------------
SearchCondition::SearchType
Auxil::stringToSearchType (const QString& string)
{
    if (string == PATTERN_MATCH_TYPE)
        return SearchCondition::PatternMatch;
    else if (string == ANAGRAM_MATCH_TYPE)
        return SearchCondition::AnagramMatch;
    else if (string == SUBANAGRAM_MATCH_TYPE)
        return SearchCondition::SubanagramMatch;
    else if (string == EXACT_LENGTH_TYPE)
        return SearchCondition::ExactLength;
    else if (string == MIN_LENGTH_TYPE)
        return SearchCondition::MinLength;
    else if (string == MAX_LENGTH_TYPE)
        return SearchCondition::MaxLength;
    else if (string == MUST_INCLUDE_TYPE)
        return SearchCondition::MustInclude;
    else if (string == MUST_EXCLUDE_TYPE)
        return SearchCondition::MustExclude;
    else if (string == MUST_CONSIST_TYPE)
        return SearchCondition::MustConsist;
    else if (string == MUST_BELONG_TYPE)
        return SearchCondition::MustBelong;
    else if (string == IN_WORD_LIST_TYPE)
        return SearchCondition::InWordList;
    else if (string == EXACT_ANAGRAMS_TYPE)
        return SearchCondition::ExactAnagrams;
    else if (string == MIN_ANAGRAMS_TYPE)
        return SearchCondition::MinAnagrams;
    else if (string == MAX_ANAGRAMS_TYPE)
        return SearchCondition::MaxAnagrams;
    else if (string == MIN_PROBABILITY_TYPE)
        return SearchCondition::MinProbability;
    else if (string == MAX_PROBABILITY_TYPE)
        return SearchCondition::MaxProbability;
    return SearchCondition::UnknownSearchType;
}

//---------------------------------------------------------------------------
//  searchTypeToString
//
//! Return a string corresponding to a search type enumeration value.
//
//! @param type the search type value
//! @return the corresponding string
//---------------------------------------------------------------------------
QString
Auxil::searchTypeToString (SearchCondition::SearchType type)
{
    switch (type) {
        case SearchCondition::PatternMatch:    return PATTERN_MATCH_TYPE;
        case SearchCondition::AnagramMatch:    return ANAGRAM_MATCH_TYPE;
        case SearchCondition::SubanagramMatch: return SUBANAGRAM_MATCH_TYPE;
        case SearchCondition::ExactLength:     return EXACT_LENGTH_TYPE;
        case SearchCondition::MinLength:       return MIN_LENGTH_TYPE;
        case SearchCondition::MaxLength:       return MAX_LENGTH_TYPE;
        case SearchCondition::MustInclude:     return MUST_INCLUDE_TYPE;
        case SearchCondition::MustExclude:     return MUST_EXCLUDE_TYPE;
        case SearchCondition::MustConsist:     return MUST_CONSIST_TYPE;
        case SearchCondition::MustBelong:      return MUST_BELONG_TYPE;
        case SearchCondition::InWordList:      return IN_WORD_LIST_TYPE;
        case SearchCondition::ExactAnagrams:   return EXACT_ANAGRAMS_TYPE;
        case SearchCondition::MinAnagrams:     return MIN_ANAGRAMS_TYPE;
        case SearchCondition::MaxAnagrams:     return MAX_ANAGRAMS_TYPE;
        case SearchCondition::MinProbability:  return MIN_PROBABILITY_TYPE;
        case SearchCondition::MaxProbability:  return MAX_PROBABILITY_TYPE;
        default: return QString::null;
    }
}
