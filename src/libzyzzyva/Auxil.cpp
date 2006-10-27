//---------------------------------------------------------------------------
// Auxil.cpp
//
// Auxiliary functions.
//
// Copyright 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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
#include "MainSettings.h"
#include "Defs.h"
#include <QApplication>
#include <QDir>
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
const QString SET_TYPE_TWO_EIGHTS_STRING = "Type II Eights";
const QString SET_TYPE_THREE_EIGHTS_STRING = "Type III Eights";
const QString SET_EIGHTS_FROM_SEVEN_STEMS_STRING =
              "Eights From Seven-Letter Stems";
const QString SET_NEW_IN_OWL2_STRING = "New in OWL2";

const QString SEARCH_TYPE_PATTERN_MATCH = "Pattern Match";
const QString SEARCH_TYPE_ANAGRAM_MATCH = "Anagram Match";
const QString SEARCH_TYPE_SUBANAGRAM_MATCH = "Subanagram Match";
const QString SEARCH_TYPE_LENGTH = "Length";
const QString SEARCH_TYPE_PREFIX = "Takes Prefix";
const QString SEARCH_TYPE_SUFFIX = "Takes Suffix";
const QString SEARCH_TYPE_INCLUDE_LETTERS = "Includes Letters";
const QString SEARCH_TYPE_CONSIST_OF = "Consists of";
const QString SEARCH_TYPE_BELONG_TO_GROUP = "Belongs to Group";
const QString SEARCH_TYPE_IN_WORD_LIST = "In Word List";
const QString SEARCH_TYPE_NUM_ANAGRAMS = "Number of Anagrams";
const QString SEARCH_TYPE_NUM_VOWELS = "Number of Vowels";
const QString SEARCH_TYPE_NUM_UNIQUE_LETTERS = "Number of Unique Letters";
const QString SEARCH_TYPE_POINT_VALUE = "Point Value";
const QString SEARCH_TYPE_PROBABILITY = "Probability";
const QString SEARCH_TYPE_PROBABILITY_ORDER = "Probability Order";
const QString SEARCH_TYPE_LIMIT_BY_PROBABILITY_ORDER =
              "Limit by Probability Order";

// Obsolete search condition strings
const QString SEARCH_TYPE_OLD_EXACT_LENGTH = "Exact Length";
const QString SEARCH_TYPE_OLD_MIN_LENGTH = "Minimum Length";
const QString SEARCH_TYPE_OLD_MAX_LENGTH = "Maximum Length";
const QString SEARCH_TYPE_OLD_DOES_NOT_TAKE_PREFIX = "Does Not Take Prefix";
const QString SEARCH_TYPE_OLD_DOES_NOT_TAKE_SUFFIX = "Does Not Take Suffix";
const QString SEARCH_TYPE_OLD_MUST_INCLUDE = "Must Include";
const QString SEARCH_TYPE_OLD_MUST_EXCLUDE = "Must Exclude";
const QString SEARCH_TYPE_OLD_MUST_CONSIST = "Must Consist of";
const QString SEARCH_TYPE_OLD_MUST_BELONG = "Must Belong to";
const QString SEARCH_TYPE_OLD_IN_WORD_LIST = "Must Be in Word List";
const QString SEARCH_TYPE_OLD_NOT_IN_WORD_LIST = "Must Not Be in Word List";
const QString SEARCH_TYPE_OLD_EXACT_ANAGRAMS = "Exact Anagrams";
const QString SEARCH_TYPE_OLD_MIN_ANAGRAMS = "Minimum Anagrams";
const QString SEARCH_TYPE_OLD_MAX_ANAGRAMS = "Maximum Anagrams";

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

const QString QUIZ_METHOD_STANDARD = "Standard";
const QString QUIZ_METHOD_CARDBOX = "Cardbox";

const QString QUIZ_ORDER_UNKNOWN = "Unknown";
const QString QUIZ_ORDER_RANDOM = "Random";
const QString QUIZ_ORDER_ALPHABETICAL = "Alphabetical";
const QString QUIZ_ORDER_PROBABILITY = "Probability";
const QString QUIZ_ORDER_SCHEDULE = "Schedule";

const QString WORD_ATTR_WORD = "Word";
const QString WORD_ATTR_DEFINITION = "Definition";
const QString WORD_ATTR_FRONT_HOOKS = "Front Hooks";
const QString WORD_ATTR_BACK_HOOKS = "Back Hooks";
const QString WORD_ATTR_INNER_HOOKS = "Inner Hooks";
const QString WORD_ATTR_PROBABILITY_ORDER = "Probability Order";
const QString WORD_ATTR_FRONT_EXTENSIONS = "Front Extensions";
const QString WORD_ATTR_BACK_EXTENSIONS = "Back Extensions";
const QString WORD_ATTR_DOUBLE_EXTENSIONS = "Double Extensions";

const QString WORD_LIST_FORMAT_ONE_PER_LINE = "One Word Per Line";
const QString WORD_LIST_FORMAT_QUESTION_ANSWER = "Anagram Question/Answer";

using namespace Defs;

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
//  getRootDir
//
//! Return the top-level directory containing everything Zyzzyva needs.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getRootDir()
{
    static QString rootDir;

    if (!rootDir.isEmpty())
        return rootDir;

    rootDir = qApp->applicationDirPath();
    QDir dir (rootDir);

    // Search in the application dir path first, then up directories until a
    // directory is found that contains a zyzzyva.top file.
    while (true) {
        if (dir.exists ("zyzzyva.top")) {
            rootDir = dir.absolutePath();
            return rootDir;
        }
        if (!dir.cdUp())
            break;
    }

    return rootDir;
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
    return getRootDir() + "/data/help";
}

//---------------------------------------------------------------------------
//  getHomeDir
//
//! Return the user's home directory.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getHomeDir()
{
    QString home = QDir::homePath();
    home.replace (QRegExp ("/+$"), "");
    return home;
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
    return getUserDir() + "/quiz";
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
    return getUserDir() + "/search";
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
    return getRootDir() + "/data/tiles";
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
    return getRootDir() + "/data/words";
}

//---------------------------------------------------------------------------
//  getUserWordsDir
//
//! Return the top-level user directory for word lists.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getUserWordsDir()
{
    return getUserDir() + "/words";
}

//---------------------------------------------------------------------------
//  getUserDir
//
//! Return the top-level directory where user data can be saved.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getUserDir()
{
    return MainSettings::getUserDataDir();
}

//---------------------------------------------------------------------------
//  getUserConfigDir
//
//! Return the top-level user directory for configuration.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getUserConfigDir()
{
    return getUserDir() + "/config";
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
//  getAlphagram
//
//! Transform a string into its alphagram.
//
//! @param word the word
//! @return the alphagram
//---------------------------------------------------------------------------
QString
Auxil::getAlphagram (const QString& word)
{
    int wordLength = word.length();
    char chars[wordLength + 1];
    int charsPlaced = 0;

    for (int i = 0; i < wordLength; ++i) {
        char c = word.at (i).toAscii();
        int j = 0;
        while ((j < charsPlaced) && (c >= chars[j]))
            ++j;
        for (int k = charsPlaced; k > j; --k)
            chars[k] = chars[k - 1];
        chars[j] = c;
        ++charsPlaced;
    }
    chars[charsPlaced] = 0;

    return QString (chars);
}

//---------------------------------------------------------------------------
//  getNumUniqueLetters
//
//! Determine the number of unique letters in a word.
//
//! @param word the word
//! @return the number of unique letters
//---------------------------------------------------------------------------
int
Auxil::getNumUniqueLetters (const QString& word)
{
    int numUniqueLetters = 0;
    QString alphagram = getAlphagram (word);
    QChar c;
    for (int i = 0; i < alphagram.length(); ++i) {
        QChar d = alphagram.at (i);
        if (d != c)
            ++numUniqueLetters;
        c = d;
    }
    return numUniqueLetters;
}

//---------------------------------------------------------------------------
//  getNumVowels
//
//! Determine the number of vowels in a word.
//
//! @param word the word
//! @return the number of vowels
//---------------------------------------------------------------------------
int
Auxil::getNumVowels (const QString& word)
{
    int numVowels = 0;
    for (int i = 0; i < word.length(); ++i) {
        if (isVowel (word.at (i)))
            ++numVowels;
    }
    return numVowels;
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
    else if (string == SET_TYPE_TWO_EIGHTS_STRING)
        return SetTypeTwoEights;
    else if (string == SET_TYPE_THREE_EIGHTS_STRING)
        return SetTypeThreeEights;
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
        case SetTypeTwoEights: return SET_TYPE_TWO_EIGHTS_STRING;
        case SetTypeThreeEights: return SET_TYPE_THREE_EIGHTS_STRING;
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
    else if (string == SEARCH_TYPE_LENGTH)
        return SearchCondition::Length;
    else if (string == SEARCH_TYPE_PREFIX)
        return SearchCondition::Prefix;
    else if (string == SEARCH_TYPE_SUFFIX)
        return SearchCondition::Suffix;
    else if (string == SEARCH_TYPE_INCLUDE_LETTERS)
        return SearchCondition::IncludeLetters;
    else if (string == SEARCH_TYPE_CONSIST_OF)
        return SearchCondition::ConsistOf;
    else if (string == SEARCH_TYPE_BELONG_TO_GROUP)
        return SearchCondition::BelongToGroup;
    else if (string == SEARCH_TYPE_IN_WORD_LIST)
        return SearchCondition::InWordList;
    else if (string == SEARCH_TYPE_NUM_ANAGRAMS)
        return SearchCondition::NumAnagrams;
    else if (string == SEARCH_TYPE_NUM_VOWELS)
        return SearchCondition::NumVowels;
    else if (string == SEARCH_TYPE_NUM_UNIQUE_LETTERS)
        return SearchCondition::NumUniqueLetters;
    else if (string == SEARCH_TYPE_POINT_VALUE)
        return SearchCondition::PointValue;
    else if (string == SEARCH_TYPE_PROBABILITY)
        return SearchCondition::Probability;
    else if (string == SEARCH_TYPE_PROBABILITY_ORDER)
        return SearchCondition::ProbabilityOrder;
    else if (string == SEARCH_TYPE_LIMIT_BY_PROBABILITY_ORDER)
        return SearchCondition::LimitByProbabilityOrder;

    // Obsolete search condition strings
    else if (string == SEARCH_TYPE_OLD_EXACT_LENGTH)
        return SearchCondition::OldExactLength;
    else if (string == SEARCH_TYPE_OLD_MIN_LENGTH)
        return SearchCondition::OldMinLength;
    else if (string == SEARCH_TYPE_OLD_MAX_LENGTH)
        return SearchCondition::OldMaxLength;
    else if (string == SEARCH_TYPE_OLD_DOES_NOT_TAKE_PREFIX)
        return SearchCondition::OldDoesNotTakePrefix;
    else if (string == SEARCH_TYPE_OLD_DOES_NOT_TAKE_SUFFIX)
        return SearchCondition::OldDoesNotTakeSuffix;
    else if (string == SEARCH_TYPE_OLD_MUST_INCLUDE)
        return SearchCondition::IncludeLetters;
    else if (string == SEARCH_TYPE_OLD_MUST_EXCLUDE)
        return SearchCondition::OldMustExclude;
    else if (string == SEARCH_TYPE_OLD_MUST_CONSIST)
        return SearchCondition::ConsistOf;
    else if (string == SEARCH_TYPE_OLD_MUST_BELONG)
        return SearchCondition::BelongToGroup;
    else if (string == SEARCH_TYPE_OLD_IN_WORD_LIST)
        return SearchCondition::InWordList;
    else if (string == SEARCH_TYPE_OLD_NOT_IN_WORD_LIST)
        return SearchCondition::OldNotInWordList;
    else if (string == SEARCH_TYPE_OLD_EXACT_ANAGRAMS)
        return SearchCondition::OldExactAnagrams;
    else if (string == SEARCH_TYPE_OLD_MIN_ANAGRAMS)
        return SearchCondition::OldMinAnagrams;
    else if (string == SEARCH_TYPE_OLD_MAX_ANAGRAMS)
        return SearchCondition::OldMaxAnagrams;

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

        case SearchCondition::Length:
        return SEARCH_TYPE_LENGTH;

        case SearchCondition::Prefix:
        return SEARCH_TYPE_PREFIX;

        case SearchCondition::Suffix:
        return SEARCH_TYPE_SUFFIX;

        case SearchCondition::IncludeLetters:
        return SEARCH_TYPE_INCLUDE_LETTERS;

        case SearchCondition::ConsistOf:
        return SEARCH_TYPE_CONSIST_OF;

        case SearchCondition::BelongToGroup:
        return SEARCH_TYPE_BELONG_TO_GROUP;

        case SearchCondition::InWordList:
        return SEARCH_TYPE_IN_WORD_LIST;

        case SearchCondition::NumAnagrams:
        return SEARCH_TYPE_NUM_ANAGRAMS;

        case SearchCondition::NumVowels:
        return SEARCH_TYPE_NUM_VOWELS;

        case SearchCondition::NumUniqueLetters:
        return SEARCH_TYPE_NUM_UNIQUE_LETTERS;

        case SearchCondition::PointValue:
        return SEARCH_TYPE_POINT_VALUE;

        case SearchCondition::Probability:
        return SEARCH_TYPE_PROBABILITY;

        case SearchCondition::ProbabilityOrder:
        return SEARCH_TYPE_PROBABILITY_ORDER;

        case SearchCondition::LimitByProbabilityOrder:
        return SEARCH_TYPE_LIMIT_BY_PROBABILITY_ORDER;

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

//---------------------------------------------------------------------------
//  quizMethodToString
//
//! Convert a quiz method to a string representation.
//
//! @param m the quiz method
//! @return the string representation
//---------------------------------------------------------------------------
QString
Auxil::quizMethodToString (QuizSpec::QuizMethod m)
{
    switch (m) {
        case QuizSpec::StandardQuizMethod:
        return QUIZ_METHOD_STANDARD;

        case QuizSpec::CardboxQuizMethod:
        return QUIZ_METHOD_CARDBOX;

        default: return QString::null;
    }
}

//---------------------------------------------------------------------------
//  stringToQuizMethod
//
//! Convert a string representation to a quiz type.
//
//! @param s the string representation
//! @return the quiz type
//---------------------------------------------------------------------------
QuizSpec::QuizMethod
Auxil::stringToQuizMethod (const QString& s)
{
    if (s == QUIZ_METHOD_STANDARD)
        return QuizSpec::StandardQuizMethod;
    else if (s == QUIZ_METHOD_CARDBOX)
        return QuizSpec::CardboxQuizMethod;
    else
        return QuizSpec::UnknownQuizMethod;
}

//---------------------------------------------------------------------------
//  quizQuestionOrderToString
//
//! Convert a quiz question order to a string representation.
//
//! @param o the quiz question order
//! @return the string representation
//---------------------------------------------------------------------------
QString
Auxil::quizQuestionOrderToString (QuizSpec::QuestionOrder o)
{
    switch (o) {
        case QuizSpec::UnknownOrder:
        return QUIZ_ORDER_UNKNOWN;

        case QuizSpec::RandomOrder:
        return QUIZ_ORDER_RANDOM;

        case QuizSpec::AlphabeticalOrder:
        return QUIZ_ORDER_ALPHABETICAL;

        case QuizSpec::ProbabilityOrder:
        return QUIZ_ORDER_PROBABILITY;

        case QuizSpec::ScheduleOrder:
        return QUIZ_ORDER_SCHEDULE;

        default: return QString::null;
    }
}

//---------------------------------------------------------------------------
//  stringToQuizQuestionOrder
//
//! Convert a string representation to a quiz question order.
//
//! @param s the string representation
//! @return the quiz question order
//---------------------------------------------------------------------------
QuizSpec::QuestionOrder
Auxil::stringToQuizQuestionOrder (const QString& s)
{
    if (s == QUIZ_ORDER_RANDOM)
        return QuizSpec::RandomOrder;
    else if (s == QUIZ_ORDER_ALPHABETICAL)
        return QuizSpec::AlphabeticalOrder;
    else if (s == QUIZ_ORDER_PROBABILITY)
        return QuizSpec::ProbabilityOrder;
    else if (s == QUIZ_ORDER_SCHEDULE)
        return QuizSpec::ScheduleOrder;
    else
        return QuizSpec::UnknownOrder;
}

//---------------------------------------------------------------------------
//  wordAttributeToString
//
//! Convert a word attribute to a string representation.
//
//! @param attr the word attribute
//! @return the string representation
//---------------------------------------------------------------------------
QString
Auxil::wordAttributeToString (WordAttribute attr)
{
    switch (attr) {
        case WordAttrWord:
        return WORD_ATTR_WORD;

        case WordAttrDefinition:
        return WORD_ATTR_DEFINITION;

        case WordAttrFrontHooks:
        return WORD_ATTR_FRONT_HOOKS;

        case WordAttrBackHooks:
        return WORD_ATTR_BACK_HOOKS;

        case WordAttrInnerHooks:
        return WORD_ATTR_INNER_HOOKS;

        case WordAttrProbabilityOrder:
        return WORD_ATTR_PROBABILITY_ORDER;

        case WordAttrFrontExtensions:
        return WORD_ATTR_FRONT_EXTENSIONS;

        case WordAttrBackExtensions:
        return WORD_ATTR_BACK_EXTENSIONS;

        case WordAttrDoubleExtensions:
        return WORD_ATTR_DOUBLE_EXTENSIONS;

        default: return QString::null;
    }
}

//---------------------------------------------------------------------------
//  stringToWordAttribute
//
//! Convert a string representation to a word attribute.
//
//! @param s the string representation
//! @return the word attribute
//---------------------------------------------------------------------------
WordAttribute
Auxil::stringToWordAttribute (const QString& s)
{
    if (s == WORD_ATTR_WORD)
        return WordAttrWord;
    else if (s == WORD_ATTR_DEFINITION)
        return WordAttrDefinition;
    else if (s == WORD_ATTR_FRONT_HOOKS)
        return WordAttrFrontHooks;
    else if (s == WORD_ATTR_BACK_HOOKS)
        return WordAttrBackHooks;
    else if (s == WORD_ATTR_INNER_HOOKS)
        return WordAttrInnerHooks;
    else if (s == WORD_ATTR_PROBABILITY_ORDER)
        return WordAttrProbabilityOrder;
    else if (s == WORD_ATTR_FRONT_EXTENSIONS)
        return WordAttrFrontExtensions;
    else if (s == WORD_ATTR_BACK_EXTENSIONS)
        return WordAttrBackExtensions;
    else if (s == WORD_ATTR_DOUBLE_EXTENSIONS)
        return WordAttrDoubleExtensions;
    else
        return WordAttrInvalid;
}

//---------------------------------------------------------------------------
//  wordListFormatToString
//
//! Convert a word list file format to a string representation.
//
//! @param format the word list file format
//! @return the string representation
//---------------------------------------------------------------------------
QString
Auxil::wordListFormatToString (WordListFormat format)
{
    switch (format) {
        case WordListOnePerLine:
        return WORD_LIST_FORMAT_ONE_PER_LINE;

        case WordListAnagramQuestionAnswer:
        return WORD_LIST_FORMAT_QUESTION_ANSWER;

        default: return QString::null;
    }
}

//---------------------------------------------------------------------------
//  stringToWordListFormat
//
//! Convert a string representation to a word attribute.
//
//! @param s the string representation
//! @return the word list format
//---------------------------------------------------------------------------
WordListFormat
Auxil::stringToWordListFormat (const QString& s)
{
    if (s == WORD_LIST_FORMAT_ONE_PER_LINE)
        return WordListOnePerLine;
    else if (s == WORD_LIST_FORMAT_QUESTION_ANSWER)
        return WordListAnagramQuestionAnswer;
    else
        return WordListInvalid;
}

//---------------------------------------------------------------------------
//  lexiconToDate
//
//! Convert a lexicon name to the date the lexicon was last updated
//
//! @param s the lexicon name
//! @return the date the lexicon was last modified
//---------------------------------------------------------------------------
QDate
Auxil::lexiconToDate (const QString& lexicon)
{
    if (lexicon == "OWL+LWL")
        return QDate (2003, 7, 1);
    if (lexicon == "OWL2+LWL")
        return QDate (2006, 3, 1);
    if (lexicon == "SOWPODS")
        return QDate (2002, 1, 14);
    return QDate();
}
