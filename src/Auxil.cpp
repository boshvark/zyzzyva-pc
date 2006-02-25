//---------------------------------------------------------------------------
// Auxil.cpp
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

#include "Auxil.h"
#include <QApplication>
#include <QFile>
#include <unistd.h>

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
const QString SET_NEW_IN_OWL2_STRING = "New in OWL2";

const QString SEARCH_TYPE_PATTERN_MATCH = "Pattern Match";
const QString SEARCH_TYPE_ANAGRAM_MATCH = "Anagram Match";
const QString SEARCH_TYPE_SUBANAGRAM_MATCH = "Subanagram Match";
const QString SEARCH_TYPE_EXACT_LENGTH = "Exact Length";
const QString SEARCH_TYPE_MIN_LENGTH = "Minimum Length";
const QString SEARCH_TYPE_MAX_LENGTH = "Maximum Length";
const QString SEARCH_TYPE_TAKES_PREFIX = "Takes Prefix";
const QString SEARCH_TYPE_DOES_NOT_TAKE_PREFIX = "Does Not Take Prefix";
const QString SEARCH_TYPE_TAKES_SUFFIX = "Takes Suffix";
const QString SEARCH_TYPE_DOES_NOT_TAKE_SUFFIX = "Does Not Take Suffix";
const QString SEARCH_TYPE_MUST_INCLUDE = "Must Include";
const QString SEARCH_TYPE_MUST_EXCLUDE = "Must Exclude";
const QString SEARCH_TYPE_MUST_CONSIST = "Must Consist of";
const QString SEARCH_TYPE_MUST_BELONG = "Must Belong to";
const QString SEARCH_TYPE_IN_WORD_LIST = "Must Be in Word List";
const QString SEARCH_TYPE_NOT_IN_WORD_LIST = "Must Not Be in Word List";
const QString SEARCH_TYPE_EXACT_ANAGRAMS = "Exact Anagrams";
const QString SEARCH_TYPE_MIN_ANAGRAMS = "Minimum Anagrams";
const QString SEARCH_TYPE_MAX_ANAGRAMS = "Maximum Anagrams";
const QString SEARCH_TYPE_MIN_PROBABILITY = "Minimum Probability";
const QString SEARCH_TYPE_MAX_PROBABILITY = "Maximum Probability";

const QString QUIZ_TYPE_PATTERNS = "Patterns";
const QString QUIZ_TYPE_ANAGRAMS = "Anagrams";
const QString QUIZ_TYPE_ANAGRAMS_WITH_HOOKS = "Anagrams with Hooks";
const QString QUIZ_TYPE_SUBANAGRAMS = "Subanagrams";
const QString QUIZ_TYPE_ANAGRAM_JUMBLE = "Anagram Jumble";
const QString QUIZ_TYPE_SUBANAGRAM_JUMBLE = "Subanagram Jumble";
const QString QUIZ_TYPE_HOOKS = "Hooks";
const QString QUIZ_TYPE_ANAGRAM_HOOKS = "Anagram Hooks";
const QString QUIZ_TYPE_ANAGRAM_HOOK_MNEMONICS = "Anagram Hook Mnemonics";
const QString QUIZ_TYPE_WORD_LIST_RECALL = "Word List Recall";

//---------------------------------------------------------------------------
//  getPid
//
//! Return the process ID of the current process.
//
//! @return the process ID
//---------------------------------------------------------------------------
unsigned int
Auxil::getPid()
{
    return getpid();
}

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
    if (!aboutFile.open (QIODevice::ReadOnly | QIODevice::Text))
        return aboutString;

    aboutString.append (aboutFile.readAll());
    return aboutString;
}

//---------------------------------------------------------------------------
//  getThanksString
//
//! Return the Thanks string for the application.  Read it from a file
//! initially, and store the read value in a static variable.
//
//! @return the Thanks string
//---------------------------------------------------------------------------
QString
Auxil::getThanksString()
{
    static QString thanksString;
    if (!thanksString.isEmpty())
        return thanksString;

    QString thanksFileName = getHelpDir() + "/thanks.html";
    QFile thanksFile (thanksFileName);
    if (!thanksFile.open (QIODevice::ReadOnly | QIODevice::Text))
        return thanksString;

    thanksString.append (thanksFile.readAll());
    return thanksString;
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
//  isVowel
//
//! Determine if a character is a vowel.  Y is not considered to be a vowel.
//
//! @param c the character
//! @return true if the character is a vowel, false otherwise
//---------------------------------------------------------------------------
bool
Auxil::isVowel (QChar c)
{
    return ((c == 'A') || (c == 'E') || (c == 'I') ||
            (c == 'O') || (c == 'U'));
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
    else if (string == SET_NEW_IN_OWL2_STRING)
        return SetNewInOwl2;
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
        case SetNewInOwl2: return SET_NEW_IN_OWL2_STRING;
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
    if (string == SEARCH_TYPE_PATTERN_MATCH)
        return SearchCondition::PatternMatch;
    else if (string == SEARCH_TYPE_ANAGRAM_MATCH)
        return SearchCondition::AnagramMatch;
    else if (string == SEARCH_TYPE_SUBANAGRAM_MATCH)
        return SearchCondition::SubanagramMatch;
    else if (string == SEARCH_TYPE_EXACT_LENGTH)
        return SearchCondition::ExactLength;
    else if (string == SEARCH_TYPE_MIN_LENGTH)
        return SearchCondition::MinLength;
    else if (string == SEARCH_TYPE_MAX_LENGTH)
        return SearchCondition::MaxLength;
    else if (string == SEARCH_TYPE_TAKES_PREFIX)
        return SearchCondition::TakesPrefix;
    else if (string == SEARCH_TYPE_DOES_NOT_TAKE_PREFIX)
        return SearchCondition::DoesNotTakePrefix;
    else if (string == SEARCH_TYPE_TAKES_SUFFIX)
        return SearchCondition::TakesSuffix;
    else if (string == SEARCH_TYPE_DOES_NOT_TAKE_SUFFIX)
        return SearchCondition::DoesNotTakeSuffix;
    else if (string == SEARCH_TYPE_MUST_INCLUDE)
        return SearchCondition::MustInclude;
    else if (string == SEARCH_TYPE_MUST_EXCLUDE)
        return SearchCondition::MustExclude;
    else if (string == SEARCH_TYPE_MUST_CONSIST)
        return SearchCondition::MustConsist;
    else if (string == SEARCH_TYPE_MUST_BELONG)
        return SearchCondition::MustBelong;
    else if (string == SEARCH_TYPE_IN_WORD_LIST)
        return SearchCondition::InWordList;
    else if (string == SEARCH_TYPE_NOT_IN_WORD_LIST)
        return SearchCondition::NotInWordList;
    else if (string == SEARCH_TYPE_EXACT_ANAGRAMS)
        return SearchCondition::ExactAnagrams;
    else if (string == SEARCH_TYPE_MIN_ANAGRAMS)
        return SearchCondition::MinAnagrams;
    else if (string == SEARCH_TYPE_MAX_ANAGRAMS)
        return SearchCondition::MaxAnagrams;
    else if (string == SEARCH_TYPE_MIN_PROBABILITY)
        return SearchCondition::MinProbability;
    else if (string == SEARCH_TYPE_MAX_PROBABILITY)
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
        case SearchCondition::PatternMatch:
        return SEARCH_TYPE_PATTERN_MATCH;

        case SearchCondition::AnagramMatch:
        return SEARCH_TYPE_ANAGRAM_MATCH;

        case SearchCondition::SubanagramMatch:
        return SEARCH_TYPE_SUBANAGRAM_MATCH;

        case SearchCondition::ExactLength:
        return SEARCH_TYPE_EXACT_LENGTH;

        case SearchCondition::MinLength:
        return SEARCH_TYPE_MIN_LENGTH;

        case SearchCondition::MaxLength:
        return SEARCH_TYPE_MAX_LENGTH;

        case SearchCondition::TakesPrefix:
        return SEARCH_TYPE_TAKES_PREFIX;

        case SearchCondition::DoesNotTakePrefix:
        return SEARCH_TYPE_DOES_NOT_TAKE_PREFIX;

        case SearchCondition::TakesSuffix:
        return SEARCH_TYPE_TAKES_SUFFIX;

        case SearchCondition::DoesNotTakeSuffix:
        return SEARCH_TYPE_DOES_NOT_TAKE_SUFFIX;

        case SearchCondition::MustInclude:
        return SEARCH_TYPE_MUST_INCLUDE;

        case SearchCondition::MustExclude:
        return SEARCH_TYPE_MUST_EXCLUDE;

        case SearchCondition::MustConsist:
        return SEARCH_TYPE_MUST_CONSIST;

        case SearchCondition::MustBelong:
        return SEARCH_TYPE_MUST_BELONG;

        case SearchCondition::InWordList:
        return SEARCH_TYPE_IN_WORD_LIST;

        case SearchCondition::NotInWordList:
        return SEARCH_TYPE_NOT_IN_WORD_LIST;

        case SearchCondition::ExactAnagrams:
        return SEARCH_TYPE_EXACT_ANAGRAMS;

        case SearchCondition::MinAnagrams:
        return SEARCH_TYPE_MIN_ANAGRAMS;

        case SearchCondition::MaxAnagrams:
        return SEARCH_TYPE_MAX_ANAGRAMS;

        case SearchCondition::MinProbability:
        return SEARCH_TYPE_MIN_PROBABILITY;

        case SearchCondition::MaxProbability:
        return SEARCH_TYPE_MAX_PROBABILITY;

        default: return QString::null;
    }
}

//---------------------------------------------------------------------------
//  quizTypeToString
//
//! Convert a quiz type to a string representation.
//
//! @param t the quiz type
//! @return the string representation
//---------------------------------------------------------------------------
QString
Auxil::quizTypeToString (QuizSpec::QuizType t)
{
    switch (t) {
        case QuizSpec::QuizPatterns:
        return QUIZ_TYPE_PATTERNS;

        case QuizSpec::QuizAnagrams:
        return QUIZ_TYPE_ANAGRAMS;

        case QuizSpec::QuizAnagramsWithHooks:
        return QUIZ_TYPE_ANAGRAMS_WITH_HOOKS;

        case QuizSpec::QuizSubanagrams:
        return QUIZ_TYPE_SUBANAGRAMS;

        case QuizSpec::QuizAnagramJumble:
        return QUIZ_TYPE_ANAGRAM_JUMBLE;

        case QuizSpec::QuizSubanagramJumble:
        return QUIZ_TYPE_SUBANAGRAM_JUMBLE;

        case QuizSpec::QuizHooks:
        return QUIZ_TYPE_HOOKS;

        case QuizSpec::QuizAnagramHooks:
        return QUIZ_TYPE_ANAGRAM_HOOKS;

        case QuizSpec::QuizAnagramHookMnemonics:
        return QUIZ_TYPE_ANAGRAM_HOOK_MNEMONICS;

        case QuizSpec::QuizWordListRecall:
        return QUIZ_TYPE_WORD_LIST_RECALL;

        default: return QString::null;
    }
}

//---------------------------------------------------------------------------
//  stringToQuizType
//
//! Convert a string representation to a quiz type.
//
//! @param s the string representation
//! @return the quiz type
//---------------------------------------------------------------------------
QuizSpec::QuizType
Auxil::stringToQuizType (const QString& s)
{
    if (s == QUIZ_TYPE_PATTERNS)
        return QuizSpec::QuizPatterns;
    else if (s == QUIZ_TYPE_ANAGRAMS)
        return QuizSpec::QuizAnagrams;
    else if (s == QUIZ_TYPE_ANAGRAMS_WITH_HOOKS)
        return QuizSpec::QuizAnagramsWithHooks;
    else if (s == QUIZ_TYPE_SUBANAGRAMS)
        return QuizSpec::QuizSubanagrams;
    else if (s == QUIZ_TYPE_ANAGRAM_JUMBLE)
        return QuizSpec::QuizAnagramJumble;
    else if (s == QUIZ_TYPE_SUBANAGRAM_JUMBLE)
        return QuizSpec::QuizSubanagramJumble;
    else if (s == QUIZ_TYPE_HOOKS)
        return QuizSpec::QuizHooks;
    else if (s == QUIZ_TYPE_ANAGRAM_HOOKS)
        return QuizSpec::QuizAnagramHooks;
    else if (s == QUIZ_TYPE_ANAGRAM_HOOK_MNEMONICS)
        return QuizSpec::QuizAnagramHookMnemonics;
    else if (s == QUIZ_TYPE_WORD_LIST_RECALL)
        return QuizSpec::QuizWordListRecall;
    else
        return QuizSpec::UnknownQuizType;
}
