//---------------------------------------------------------------------------
// Auxil.cpp
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
const QString SET_HIGH_FIVES_STRING = "High Fives";
const QString SET_TYPE_ONE_SEVENS_STRING = "Type I Sevens";
const QString SET_TYPE_TWO_SEVENS_STRING = "Type II Sevens";
const QString SET_TYPE_THREE_SEVENS_STRING = "Type III Sevens";
const QString SET_TYPE_ONE_EIGHTS_STRING = "Type I Eights";
const QString SET_TYPE_TWO_EIGHTS_STRING = "Type II Eights";
const QString SET_TYPE_THREE_EIGHTS_STRING = "Type III Eights";
const QString SET_EIGHTS_FROM_SEVEN_STEMS_STRING =
              "Eights From Seven-Letter Stems";

// Obsolete search set strings
const QString SET_OLD_NEW_IN_OWL2_STRING = "New in OWL2";
const QString SET_OLD_NEW_IN_CSW_STRING = "New in CSW";

const QString SEARCH_TYPE_PATTERN_MATCH = "Pattern Match";
const QString SEARCH_TYPE_ANAGRAM_MATCH = "Anagram Match";
const QString SEARCH_TYPE_SUBANAGRAM_MATCH = "Subanagram Match";
const QString SEARCH_TYPE_LENGTH = "Length";
const QString SEARCH_TYPE_PREFIX = "Takes Prefix";
const QString SEARCH_TYPE_SUFFIX = "Takes Suffix";
const QString SEARCH_TYPE_INCLUDE_LETTERS = "Includes Letters";
const QString SEARCH_TYPE_CONSIST_OF = "Consists of";
const QString SEARCH_TYPE_BELONG_TO_GROUP = "Belongs to Group";
const QString SEARCH_TYPE_IN_LEXICON = "In Lexicon";
const QString SEARCH_TYPE_IN_WORD_LIST = "In Word List";
const QString SEARCH_TYPE_NUM_ANAGRAMS = "Number of Anagrams";
const QString SEARCH_TYPE_NUM_VOWELS = "Number of Vowels";
const QString SEARCH_TYPE_NUM_UNIQUE_LETTERS = "Number of Unique Letters";
const QString SEARCH_TYPE_POINT_VALUE = "Point Value";
const QString SEARCH_TYPE_PROBABILITY = "Probability";
const QString SEARCH_TYPE_PROBABILITY_ORDER = "Probability Order";
const QString SEARCH_TYPE_LIMIT_BY_PROBABILITY_ORDER =
              "Limit by Probability Order";
const QString SEARCH_TYPE_PLAYABILITY_ORDER = "Playability Order";
const QString SEARCH_TYPE_LIMIT_BY_PLAYABILITY_ORDER =
              "Limit by Playability Order";
const QString SEARCH_TYPE_PART_OF_SPEECH = "Part of Speech";
const QString SEARCH_TYPE_DEFINITION = "Definition";

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
const QString QUIZ_TYPE_BUILD = "Build";
const QString QUIZ_TYPE_ANAGRAM_JUMBLE = "Anagram Jumble";
const QString QUIZ_TYPE_SUBANAGRAM_JUMBLE = "Subanagram Jumble";
const QString QUIZ_TYPE_HOOKS = "Hooks";
const QString QUIZ_TYPE_ANAGRAM_HOOKS = "Anagram Hooks";
const QString QUIZ_TYPE_ANAGRAM_HOOK_MNEMONICS = "Anagram Hook Mnemonics";
const QString QUIZ_TYPE_WORD_LIST_RECALL = "Word List Recall";

const QString QUIZ_METHOD_STANDARD = "Standard";
const QString QUIZ_METHOD_CARDBOX = "Cardbox";

const QString QUIZ_SOURCE_SEARCH = "search";
const QString QUIZ_SOURCE_CARDBOX_READY = "cardbox-ready";
const QString QUIZ_SOURCE_RANDOM_LETTERS = "random-letters";

const QString QUIZ_ORDER_UNKNOWN = "Unknown";
const QString QUIZ_ORDER_RANDOM = "Random";
const QString QUIZ_ORDER_ALPHABETICAL = "Alphabetical";
const QString QUIZ_ORDER_PROBABILITY = "Probability";
const QString QUIZ_ORDER_PLAYABILITY = "Playability";
const QString QUIZ_ORDER_SCHEDULE = "Schedule";
const QString QUIZ_ORDER_SCHEDULE_ZERO_FIRST = "Schedule (Cardbox 0 First)";

const QString WORD_ATTR_WORD = "Word";
const QString WORD_ATTR_DEFINITION = "Definition";
const QString WORD_ATTR_FRONT_HOOKS = "Front Hooks";
const QString WORD_ATTR_BACK_HOOKS = "Back Hooks";
const QString WORD_ATTR_INNER_HOOKS = "Inner Hooks";
const QString WORD_ATTR_LEXICON_SYMBOLS = "Lexicon Symbols";
const QString WORD_ATTR_PROBABILITY_ORDER = "Probability Order";
const QString WORD_ATTR_PLAYABILITY_ORDER = "Playability Order";
const QString WORD_ATTR_FRONT_EXTENSIONS = "Front Extensions";
const QString WORD_ATTR_BACK_EXTENSIONS = "Back Extensions";
const QString WORD_ATTR_DOUBLE_EXTENSIONS = "Double Extensions";
const QString WORD_ATTR_ALPHAGRAM = "Alphagram";

const QString WORD_LIST_FORMAT_ONE_PER_LINE = "One Word Per Line";
const QString WORD_LIST_FORMAT_QUESTION_ANSWER = "Anagram Question/Answer";
const QString WORD_LIST_FORMAT_TWO_COLUMN = "Anagram Two Column";
const QString WORD_LIST_FORMAT_DISTINCT_ALPHAGRAMS = "Distinct Alphagrams";

using namespace Defs;

//---------------------------------------------------------------------------
//  localeAwareLessThanQString
//
//! A comparison function that compares QString objects in a locale-aware way.
//
//! @param a the first string to compare
//! @param b the second string to compare
//! @return true if a is less than b
//---------------------------------------------------------------------------
bool
Auxil::localeAwareLessThanQString(const QString& a, const QString& b)
{
    return (QString::localeAwareCompare(a, b) < 0);
}

//---------------------------------------------------------------------------
//  localeAwareLessThanQChar
//
//! A comparison function that compares QChar objects in a locale-aware way.
//
//! @param a the first char to compare
//! @param b the second char to compare
//! @return true if a is less than b
//---------------------------------------------------------------------------
bool
Auxil::localeAwareLessThanQChar(const QChar& a, const QChar& b)
{
    return (QString::localeAwareCompare(a, b) < 0);
}

//---------------------------------------------------------------------------
//  copyDir
//
//! Copy a directory from one location to another.
//
//! @poram src the source directory
//! @param dest the destination directory
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
Auxil::copyDir(const QString& src, const QString& dest)
{
    QDir srcDir (src);
    QDir destDir (dest);
    if (!srcDir.isReadable())
        return false;

    destDir.mkpath(dest);
    QFileInfoList entries = srcDir.entryInfoList();
    QListIterator<QFileInfo> it (entries);
    while (it.hasNext()) {
        const QFileInfo& finfo = it.next();
        if ((finfo.fileName() == ".") || (finfo.fileName() == ".."))
            continue;
        else if (finfo.isDir()) {
            copyDir(finfo.filePath(),
                    destDir.absoluteFilePath(finfo.fileName()));
        }
        else if (finfo.isFile() && finfo.isReadable()) {
            QFile file (finfo.filePath());
            file.copy(destDir.absoluteFilePath(finfo.fileName()));
        }
        else
            return false;
    }
    return true;
}

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
    if (!aboutFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return aboutString;

    aboutString.append(aboutFile.readAll());
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
    if (!thanksFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return thanksString;

    thanksString.append(thanksFile.readAll());
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
        if (dir.exists("zyzzyva.top")) {
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
    home.replace(QRegExp("/+$"), "");
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
//  getLexiconPrefix
//
//! Return the path associated with a lexicon name.
//
//! @param lexicon the lexicon name
//! @return the path where the lexicon data is found
//---------------------------------------------------------------------------
QString
Auxil::getLexiconPrefix(const QString& lexicon)
{
    static QMap<QString, QString> pmap;
    if (pmap.isEmpty()) {
        pmap[LEXICON_OWL] = "/North-American/OWL";
        pmap[LEXICON_OWL2] = "/North-American/OWL2";
        pmap[LEXICON_OWL2_1] = "/North-American/OWL2.1";
        pmap[LEXICON_OSPD4] = "/North-American/OSPD4";
        pmap[LEXICON_OSPD4_1] = "/North-American/OSPD4.1";
        pmap[LEXICON_WWF] = "/North-American/WWF";
        pmap[LEXICON_VOLOST] = "/Antarctic/Volost";
        pmap[LEXICON_OSWI] = "/British/OSWI";
        pmap[LEXICON_CSW07] = "/British/CSW07";
        pmap[LEXICON_CSW12] = "/British/CSW12";
        pmap[LEXICON_CD] = "/British/CD";
        pmap[LEXICON_ODS4] = "/French/ODS4";
        pmap[LEXICON_ODS5] = "/French/ODS5";
        pmap[LEXICON_FISE2009] = "/Spanish/FISE2009";
        pmap[LEXICON_ZINGA] = "/Italian/ZINGA";
    }
    return pmap.value(lexicon);
}

//---------------------------------------------------------------------------
//  getDatabaseFilename
//
//! Return the database filename that should be used for a lexicon.  Also
//! create the db directory if it doesn't already exist.  FIXME: That should
//! be done somewhere else!
//
//! @param lexicon the lexicon name
//! @return the database filename, or empty string if error
//---------------------------------------------------------------------------
QString
Auxil::getDatabaseFilename(const QString& lexicon)
{
    if (lexicon != LEXICON_CUSTOM) {
        QString lexiconPrefix = getLexiconPrefix(lexicon);
        if (lexiconPrefix.isEmpty())
            return QString();
    }

    QString dbPath = getUserDir() + "/lexicons";
    QDir dir;
    dir.mkpath(dbPath);
    return (dbPath + "/" + lexicon + ".db");
}

//---------------------------------------------------------------------------
//  dialogWordWrap
//
//! Wrap a string so it is appropriate for display in a dialog.
//
//! @param str the string to wrap
//! @return the wrapped string
//---------------------------------------------------------------------------
QString
Auxil::dialogWordWrap(const QString& str)
{
#if defined Z_OSX
    return str;
#else
    return wordWrap(str, DIALOG_LINE_LENGTH);
#endif
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
Auxil::wordWrap(const QString& str, int wrapLength)
{
    int strLen = str.length();
    if (strLen <= wrapLength)
        return str;

    QChar c;
    QString wrappedStr = str;
    int lastSpace = 0;
    int lastNewline = 0;
    for (int i = 0; i < strLen; ++i) {
        c = wrappedStr.at(i);
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
Auxil::isVowel(QChar c)
{
    // XXX: This really should be done in a more rigorous way
    switch (c.unicode()) {
        case 'A': case 'E': case 'I': case 'O': case 'U':
        case 192: case 193: case 194: case 195: case 196: case 197: case 198:
        case 200: case 201: case 202: case 203: case 204: case 205: case 206:
        case 207: case 210: case 211: case 212: case 213: case 214: case 216:
        case 217: case 218: case 219: case 220: case 221:
        return true;

        default: return false;
    }
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
Auxil::getAlphagram(const QString& word)
{
    int wordLength = word.length();
    if (wordLength <= 1)
        return word;

    // Get characters
    QString alphagram;
    QList<QChar> chars;
    for (int i = 0; i < word.length(); ++i) {
        chars.append(word[i]);
    }
    qSort(chars.begin(), chars.end(), localeAwareLessThanQChar);
    foreach (const QChar& c, chars)
        alphagram.append(c);

    //qDebug("Alphagram: |%s|", alphagram.toUtf8().constData());
    return alphagram;

    // Old, fast alphagram code - get back to this if possible
    //char chars[wordLength + 1];
    //int charsPlaced = 0;

    //for (int i = 0; i < wordLength; ++i) {
    //    char c = word.at(i).toAscii();
    //    int j = 0;
    //    while ((j < charsPlaced) && (c >= chars[j]))
    //        ++j;
    //    for (int k = charsPlaced; k > j; --k)
    //        chars[k] = chars[k - 1];
    //    chars[j] = c;
    //    ++charsPlaced;
    //}
    //chars[charsPlaced] = 0;

    //return QString(chars);
}

//---------------------------------------------------------------------------
//  getCanonicalSearchString
//
//! Transform a search string into its canonical version. Replace '.' with '?'
//! and '@' with '*'.
//
//! @param str the search string
//! @return the canonical search string
//---------------------------------------------------------------------------
QString
Auxil::getCanonicalSearchString(const QString& str)
{
    QString canonical (str);
    canonical.replace(QChar('.'), QChar('?'));
    canonical.replace(QChar('@'), QChar('*'));
    return canonical;
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
Auxil::getNumUniqueLetters(const QString& word)
{
    int numUniqueLetters = 0;
    QString alphagram = getAlphagram(word);
    QChar c;
    for (int i = 0; i < alphagram.length(); ++i) {
        QChar d = alphagram.at(i);
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
Auxil::getNumVowels(const QString& word)
{
    int numVowels = 0;
    for (int i = 0; i < word.length(); ++i) {
        if (isVowel(word.at(i)))
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
Auxil::stringToSearchSet(const QString& string)
{
    if (string == SET_HOOK_WORDS_STRING)
        return SetHookWords;
    else if (string == SET_FRONT_HOOKS_STRING)
        return SetFrontHooks;
    else if (string == SET_BACK_HOOKS_STRING)
        return SetBackHooks;
    else if (string == SET_HIGH_FIVES_STRING)
        return SetHighFives;
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

    // Obsolete search sets
    else if (string == SET_OLD_NEW_IN_OWL2_STRING)
        return SetOldNewInOwl2;
    else if (string == SET_OLD_NEW_IN_CSW_STRING)
        return SetOldNewInCsw;
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
Auxil::searchSetToString(SearchSet ss)
{
    switch (ss) {
        case SetHookWords: return SET_HOOK_WORDS_STRING;
        case SetFrontHooks: return SET_FRONT_HOOKS_STRING;
        case SetBackHooks: return SET_BACK_HOOKS_STRING;
        case SetHighFives: return SET_HIGH_FIVES_STRING;
        case SetTypeOneSevens: return SET_TYPE_ONE_SEVENS_STRING;
        case SetTypeTwoSevens: return SET_TYPE_TWO_SEVENS_STRING;
        case SetTypeThreeSevens: return SET_TYPE_THREE_SEVENS_STRING;
        case SetTypeOneEights: return SET_TYPE_ONE_EIGHTS_STRING;
        case SetTypeTwoEights: return SET_TYPE_TWO_EIGHTS_STRING;
        case SetTypeThreeEights: return SET_TYPE_THREE_EIGHTS_STRING;
        case SetEightsFromSevenLetterStems:
            return SET_EIGHTS_FROM_SEVEN_STEMS_STRING;

        // Obsolete search sets
        case SetOldNewInOwl2: return SET_OLD_NEW_IN_OWL2_STRING;
        case SetOldNewInCsw: return SET_OLD_NEW_IN_CSW_STRING;
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
Auxil::stringToSearchType(const QString& string)
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
    else if (string == SEARCH_TYPE_IN_LEXICON)
        return SearchCondition::InLexicon;
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
    else if (string == SEARCH_TYPE_PLAYABILITY_ORDER)
        return SearchCondition::PlayabilityOrder;
    else if (string == SEARCH_TYPE_LIMIT_BY_PLAYABILITY_ORDER)
        return SearchCondition::LimitByPlayabilityOrder;
    else if (string == SEARCH_TYPE_PART_OF_SPEECH)
        return SearchCondition::PartOfSpeech;
    else if (string == SEARCH_TYPE_DEFINITION)
        return SearchCondition::Definition;

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
Auxil::searchTypeToString(SearchCondition::SearchType type)
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

        case SearchCondition::InLexicon:
        return SEARCH_TYPE_IN_LEXICON;

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

        case SearchCondition::PlayabilityOrder:
        return SEARCH_TYPE_PLAYABILITY_ORDER;

        case SearchCondition::LimitByPlayabilityOrder:
        return SEARCH_TYPE_LIMIT_BY_PLAYABILITY_ORDER;

        case SearchCondition::PartOfSpeech:
        return SEARCH_TYPE_PART_OF_SPEECH;

        case SearchCondition::Definition:
        return SEARCH_TYPE_DEFINITION;

        default: return QString();
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
Auxil::quizTypeToString(QuizSpec::QuizType t)
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

        case QuizSpec::QuizBuild:
        return QUIZ_TYPE_BUILD;

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

        default: return QString();
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
Auxil::stringToQuizType(const QString& s)
{
    if (s == QUIZ_TYPE_PATTERNS)
        return QuizSpec::QuizPatterns;
    else if (s == QUIZ_TYPE_ANAGRAMS)
        return QuizSpec::QuizAnagrams;
    else if (s == QUIZ_TYPE_ANAGRAMS_WITH_HOOKS)
        return QuizSpec::QuizAnagramsWithHooks;
    else if (s == QUIZ_TYPE_SUBANAGRAMS)
        return QuizSpec::QuizSubanagrams;
    else if (s == QUIZ_TYPE_BUILD)
        return QuizSpec::QuizBuild;
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
Auxil::quizMethodToString(QuizSpec::QuizMethod m)
{
    switch (m) {
        case QuizSpec::StandardQuizMethod:
        return QUIZ_METHOD_STANDARD;

        case QuizSpec::CardboxQuizMethod:
        return QUIZ_METHOD_CARDBOX;

        default: return QString();
    }
}

//---------------------------------------------------------------------------
//  stringToQuizMethod
//
//! Convert a string representation to a quiz method.
//
//! @param s the string representation
//! @return the quiz type
//---------------------------------------------------------------------------
QuizSpec::QuizMethod
Auxil::stringToQuizMethod(const QString& s)
{
    if (s == QUIZ_METHOD_STANDARD)
        return QuizSpec::StandardQuizMethod;
    else if (s == QUIZ_METHOD_CARDBOX)
        return QuizSpec::CardboxQuizMethod;
    else
        return QuizSpec::UnknownQuizMethod;
}

//---------------------------------------------------------------------------
//  quizSourceTypeToString
//
//! Convert a quiz source type to a string representation.
//
//! @param m the quiz source type
//! @return the string representation
//---------------------------------------------------------------------------
QString
Auxil::quizSourceTypeToString(QuizSpec::QuizSourceType s)
{
    switch (s) {
        case QuizSpec::SearchSource:
        return QUIZ_SOURCE_SEARCH;

        case QuizSpec::CardboxReadySource:
        return QUIZ_SOURCE_CARDBOX_READY;

        case QuizSpec::RandomLettersSource:
        return QUIZ_SOURCE_RANDOM_LETTERS;

        default: return QString();
    }
}

//---------------------------------------------------------------------------
//  stringToQuizSourceType
//
//! Convert a string representation to a quiz source type.
//
//! @param s the string representation
//! @return the quiz type
//---------------------------------------------------------------------------
QuizSpec::QuizSourceType
Auxil::stringToQuizSourceType(const QString& s)
{
    if (s == QUIZ_SOURCE_SEARCH)
        return QuizSpec::SearchSource;
    else if (s == QUIZ_SOURCE_CARDBOX_READY)
        return QuizSpec::CardboxReadySource;
    else if (s == QUIZ_SOURCE_RANDOM_LETTERS)
        return QuizSpec::RandomLettersSource;
    else
        return QuizSpec::UnknownSource;
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
Auxil::quizQuestionOrderToString(QuizSpec::QuestionOrder o)
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

        case QuizSpec::PlayabilityOrder:
        return QUIZ_ORDER_PLAYABILITY;

        case QuizSpec::ScheduleOrder:
        return QUIZ_ORDER_SCHEDULE;

        case QuizSpec::ScheduleZeroFirstOrder:
        return QUIZ_ORDER_SCHEDULE_ZERO_FIRST;

        default: return QString();
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
Auxil::stringToQuizQuestionOrder(const QString& s)
{
    if (s == QUIZ_ORDER_RANDOM)
        return QuizSpec::RandomOrder;
    else if (s == QUIZ_ORDER_ALPHABETICAL)
        return QuizSpec::AlphabeticalOrder;
    else if (s == QUIZ_ORDER_PROBABILITY)
        return QuizSpec::ProbabilityOrder;
    else if (s == QUIZ_ORDER_PLAYABILITY)
        return QuizSpec::PlayabilityOrder;
    else if (s == QUIZ_ORDER_SCHEDULE)
        return QuizSpec::ScheduleOrder;
    else if (s == QUIZ_ORDER_SCHEDULE_ZERO_FIRST)
        return QuizSpec::ScheduleZeroFirstOrder;
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
Auxil::wordAttributeToString(WordAttribute attr)
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

        case WordAttrLexiconSymbols:
        return WORD_ATTR_LEXICON_SYMBOLS;

        case WordAttrProbabilityOrder:
        return WORD_ATTR_PROBABILITY_ORDER;

        case WordAttrPlayabilityOrder:
        return WORD_ATTR_PLAYABILITY_ORDER;

        case WordAttrFrontExtensions:
        return WORD_ATTR_FRONT_EXTENSIONS;

        case WordAttrBackExtensions:
        return WORD_ATTR_BACK_EXTENSIONS;

        case WordAttrDoubleExtensions:
        return WORD_ATTR_DOUBLE_EXTENSIONS;

        case WordAttrAlphagram:
        return WORD_ATTR_ALPHAGRAM;

        default: return QString();
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
Auxil::stringToWordAttribute(const QString& s)
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
    else if (s == WORD_ATTR_LEXICON_SYMBOLS)
        return WordAttrLexiconSymbols;
    else if (s == WORD_ATTR_PROBABILITY_ORDER)
        return WordAttrProbabilityOrder;
    else if (s == WORD_ATTR_PLAYABILITY_ORDER)
        return WordAttrPlayabilityOrder;
    else if (s == WORD_ATTR_FRONT_EXTENSIONS)
        return WordAttrFrontExtensions;
    else if (s == WORD_ATTR_BACK_EXTENSIONS)
        return WordAttrBackExtensions;
    else if (s == WORD_ATTR_DOUBLE_EXTENSIONS)
        return WordAttrDoubleExtensions;
    else if (s == WORD_ATTR_ALPHAGRAM)
        return WordAttrAlphagram;
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
Auxil::wordListFormatToString(WordListFormat format)
{
    switch (format) {
        case WordListOnePerLine:
        return WORD_LIST_FORMAT_ONE_PER_LINE;

        case WordListAnagramQuestionAnswer:
        return WORD_LIST_FORMAT_QUESTION_ANSWER;

        case WordListAnagramTwoColumn:
        return WORD_LIST_FORMAT_TWO_COLUMN;

        case WordListDistinctAlphagrams:
        return WORD_LIST_FORMAT_DISTINCT_ALPHAGRAMS;

        default: return QString();
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
Auxil::stringToWordListFormat(const QString& s)
{
    if (s == WORD_LIST_FORMAT_ONE_PER_LINE)
        return WordListOnePerLine;
    else if (s == WORD_LIST_FORMAT_QUESTION_ANSWER)
        return WordListAnagramQuestionAnswer;
    else if (s == WORD_LIST_FORMAT_TWO_COLUMN)
        return WordListAnagramTwoColumn;
    else if (s == WORD_LIST_FORMAT_DISTINCT_ALPHAGRAMS)
        return WordListDistinctAlphagrams;
    else
        return WordListInvalid;
}

//---------------------------------------------------------------------------
//  lexiconStyleToString
//
//! Convert a lexicon style to a string representation.
//
//! @param style the lexicon style
//! @return the string representation
//---------------------------------------------------------------------------
QString
Auxil::lexiconStyleToString(const LexiconStyle& style)
{
    return style.lexicon + " and " +
        (style.inCompareLexicon ? QString() : QString("not ")) +
        style.compareLexicon + ": symbol " +
        style.symbol;
}

//---------------------------------------------------------------------------
//  stringToLexiconStyle
//
//! Convert a string representation to a lexicon style.
//
//! @param s the string representation
//! @return the lexicon style
//---------------------------------------------------------------------------
LexiconStyle
Auxil::stringToLexiconStyle(const QString& s)
{
    QRegExp regex("^(\\S+)\\s+and\\s+(not\\s+)?(\\S+):\\s+symbol\\s+(\\S+)");
    int pos = regex.indexIn(s);
    if (pos < 0)
        return LexiconStyle();

    LexiconStyle style;
    style.lexicon = regex.cap(1);
    style.compareLexicon = regex.cap(3);
    style.inCompareLexicon = regex.cap(2).isEmpty();
    style.symbol = regex.cap(4);
    return style;
}

//---------------------------------------------------------------------------
//  lexiconToOrigin
//
//! Convert a lexicon name to the origin of the lexicon.
//
//! @param s the lexicon name
//! @return the lexicon origin
//---------------------------------------------------------------------------
QString
Auxil::lexiconToOrigin(const QString& lexicon)
{
    if (lexicon == LEXICON_OWL)
        return "North American";
    if (lexicon == LEXICON_OWL2)
        return "North American";
    if (lexicon == LEXICON_OWL2_1)
        return "North American";
    if (lexicon == LEXICON_OSPD4)
        return "North American (School)";
    if (lexicon == LEXICON_OSPD4_1)
        return "North American (School)";
    if (lexicon == LEXICON_WWF)
        return "Words With Friends";
    if (lexicon == LEXICON_OSWI)
        return "British";
    if (lexicon == LEXICON_CSW07)
        return "British (WESPA HarperCollins copyright 2007)";
    if (lexicon == LEXICON_CSW12)
        return "British (WESPA HarperCollins copyright 2011)";
    if (lexicon == LEXICON_CD)
        return "British (Contest)";
    if (lexicon == LEXICON_ODS4)
        return "French";
    if (lexicon == LEXICON_ODS5)
        return "French";
    if (lexicon == LEXICON_FISE2009)
        return "Spanish";
    if (lexicon == LEXICON_ZINGA)
        return "Italian";
    if (lexicon == LEXICON_VOLOST)
        return "Antarctic";
    return QString();
}

//---------------------------------------------------------------------------
//  lexiconToDate
//
//! Convert a lexicon name to the date the lexicon was last updated.
//
//! @param s the lexicon name
//! @return the date the lexicon was last modified
//---------------------------------------------------------------------------
QDate
Auxil::lexiconToDate(const QString& lexicon)
{
    if (lexicon == LEXICON_OWL)
        return QDate(2003, 7, 1);
    if (lexicon == LEXICON_OWL2)
        return QDate(2006, 3, 1);
    if (lexicon == LEXICON_OWL2_1)
        return QDate(2014, 8, 9);
    if (lexicon == LEXICON_OSPD4)
        return QDate(2006, 3, 1);
    if (lexicon == LEXICON_OSPD4_1)
        return QDate(2014, 4, 10);
    if (lexicon == LEXICON_WWF)
        return QDate(2011, 2, 1);
    if (lexicon == LEXICON_OSWI)
        return QDate(2002, 1, 14);
    if (lexicon == LEXICON_CSW07)
        return QDate(2007, 5, 15);
    if (lexicon == LEXICON_CSW12)
        return QDate(2012, 1, 1);
    if (lexicon == LEXICON_CD)
        return QDate(2010, 1, 1);
    if (lexicon == LEXICON_ODS4)
        return QDate(2004, 1, 1);
    if (lexicon == LEXICON_ODS5)
        return QDate(2008, 1, 1);
    if (lexicon == LEXICON_FISE2009)
        return QDate(2009, 4, 1);
    if (lexicon == LEXICON_ZINGA)
        return QDate(2010, 1, 1);
    if (lexicon == LEXICON_VOLOST)
        return QDate(2007, 4, 1);
    return QDate();
}

//---------------------------------------------------------------------------
//  lexiconToDetails
//
//! Convert a lexicon name to the details about the lexicon.
//
//! @param s the lexicon name
//! @return the details about the lexicon
//---------------------------------------------------------------------------
QString
Auxil::lexiconToDetails(const QString& lexicon)
{
    if (lexicon == LEXICON_CSW07) {
        return "The WESPA HarperCollins copyright 2007 word list (CSW07) "
            "is copyright of HarperCollins and used with permission.";
    }
    if (lexicon == LEXICON_CSW12) {
        return "The WESPA HarperCollins copyright 2011 word list (CSW12) "
            "is copyright of HarperCollins and used with permission.";
    }
    return QString();
}

//---------------------------------------------------------------------------
//  getUpdatedLexiconName
//
//! Get the most up-to-date lexicon name for old lexicons that may have been
//! renamed.
//
//! @param oldLexiconName the old lexicon name
//! @return the current lexicon name
//---------------------------------------------------------------------------
QString
Auxil::getUpdatedLexiconName(const QString& oldLexiconName)
{
    if (oldLexiconName == LEXICON_OLD_OWL)
        return LEXICON_OWL;
    else if (oldLexiconName == LEXICON_OLD_OWL2)
        return LEXICON_OWL2;
    else if (oldLexiconName == LEXICON_OLD_OSPD4)
        return LEXICON_OSPD4;
    else if (oldLexiconName == LEXICON_OLD_SOWPODS)
        return LEXICON_OSWI;
    else if (oldLexiconName == LEXICON_OLD_ODS)
        return LEXICON_ODS4;
    else if (oldLexiconName == LEXICON_OLD_CSW)
        return LEXICON_CSW07;
    else if ((oldLexiconName == LEXICON_CUSTOM) ||
             (oldLexiconName == LEXICON_OWL) ||
             (oldLexiconName == LEXICON_OWL2) ||
             (oldLexiconName == LEXICON_OWL2_1) ||
             (oldLexiconName == LEXICON_WWF) ||
             (oldLexiconName == LEXICON_CSW07) ||
             (oldLexiconName == LEXICON_CSW12) ||
             (oldLexiconName == LEXICON_CD) ||
             (oldLexiconName == LEXICON_OSPD4) ||
             (oldLexiconName == LEXICON_OSPD4_1) ||
             (oldLexiconName == LEXICON_OSWI) ||
             (oldLexiconName == LEXICON_ODS4) ||
             (oldLexiconName == LEXICON_ODS5) ||
             (oldLexiconName == LEXICON_FISE2009) ||
             (oldLexiconName == LEXICON_ZINGA) ||
             (oldLexiconName == LEXICON_VOLOST))
    {
        return oldLexiconName;
    }

    return QString();
}

//---------------------------------------------------------------------------
//  lessThanVersion
//
//! Test whether one version string is less than another version string.
//
//! @param a the version to test
//! @param b the version to compare against
//! @return true if a is less than b, false otherwise
//---------------------------------------------------------------------------
bool
Auxil::lessThanVersion(const QString& a, const QString& b)
{
    if (a == b)
        return false;

    int amaj = 0;
    int bmaj = 0;
    int amin = 0;
    int bmin = 0;
    int arev = 0;
    int brev = 0;

    bool aok = getVersionComponents(a, amaj, amin, arev);
    bool bok = getVersionComponents(b, bmaj, bmin, brev);

    if (!aok)
        return bok;
    else if (!bok)
        return false;

    if (amaj < bmaj)
        return true;
    else if (amaj > bmaj)
        return false;

    if (amin < bmin)
        return true;
    else if (amin > bmin)
        return false;

    return (arev < brev);
}

//---------------------------------------------------------------------------
//  getVersionComponents
//
//! Get major, minor, and revision components of a version string.
//
//! @param version the version string
//! @param[out] major return the major component
//! @param[out] minor return the minor component
//! @param[out] revision return the revision component
//! @return true if valid version string, false otherwise
//---------------------------------------------------------------------------
bool
Auxil::getVersionComponents(const QString& version, int& major, int& minor,
    int& revision)
{
    static QRegExp re("^(\\d+)\\.(\\d+)\\.(\\d+)$");

    if (!re.exactMatch(version))
        return false;

    major = re.capturedTexts().at(1).toInt();
    minor = re.capturedTexts().at(2).toInt();
    revision = re.capturedTexts().at(3).toInt();
    return true;
}
