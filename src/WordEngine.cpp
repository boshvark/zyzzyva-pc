//---------------------------------------------------------------------------
// WordEngine.cpp
//
// A class to handle the loading and searching of words.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#include "WordEngine.h"
#include "Auxil.h"
#include "Defs.h"
#include <QFile>
#include <QRegExp>
#include <set>

const int MAX_DEFINITION_LINKS = 3;

using namespace Defs;
using std::set;
using std::map;
using std::make_pair;
using std::multimap;

//---------------------------------------------------------------------------
//  importFile
//
//! Import words from a file.  The file is assumed to be in plain text format,
//! containing one word per line.
//
//! @param filename the name of the file to import
//! @param loadDefinitions whether to load word definitions
//! @param errString returns the error string in case of error
//! @return the number of words imported
//---------------------------------------------------------------------------
int
WordEngine::importFile (const QString& filename, bool loadDefinitions,
                        QString* errString)
{
    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        if (errString)
            *errString = "Can't open file '" + filename + "': "
            + file.errorString();
        return 0;
    }

    QRegExp posRegex (QString ("\\[(\\w+)"));

    int imported = 0;
    char* buffer = new char [MAX_INPUT_LINE_LEN];
    while (file.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplifyWhiteSpace();
        if (!line.length() || (line.at (0) == '#'))
            continue;
        QString word = line.section (' ', 0, 0);
        graph.addWord (word);
        if (loadDefinitions) {
            QString definition = line.section (' ', 1);
            if (!definition.isEmpty()) {
                multimap<QString, QString> defMap;
                QStringList defs = definition.split (" / ");
                QString def;
                foreach (def, defs) {
                    QString pos;
                    if (posRegex.indexIn (def, 0) >= 0) {
                        pos = posRegex.cap (1);
                    }
                    defMap.insert (make_pair (pos, def));
                }
                definitions.insert (make_pair (word, defMap));
            }
        }
        ++imported;
    }

    delete[] buffer;
    return imported;
}

//---------------------------------------------------------------------------
//  importStems
//
//! Import stems from a file.  The file is assumed to be in plain text format,
//! containing one stem per line.  The file is also assumed to contain stems
//! of equal length.  All stems of different length than the first stem will
//! be discarded.
//
//! @param filename the name of the file to import
//! @param errString returns the error string in case of error
//! @return the number of stems imported
//---------------------------------------------------------------------------
int
WordEngine::importStems (const QString& filename, QString* errString)
{
    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        if (errString)
            *errString = "Can't open file '" + filename + "': "
            + file.errorString();
        return -1;
    }

    // XXX: At some point, may want to consider allowing words of varying
    // lengths to be in the same file?
    QStringList words;
    std::set<QString> alphagrams;
    int imported = 0;
    int length = 0;
    char* buffer = new char [MAX_INPUT_LINE_LEN];
    while (file.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplifyWhiteSpace();
        if (!line.length() || (line.at (0) == '#'))
            continue;
        QString word = line.section (' ', 0, 0);

        if (!length)
            length = word.length();

        if (length != int (word.length()))
            continue;

        words << word;
        alphagrams.insert (alphagram (word));
        ++imported;
    }
    delete[] buffer;

    // Insert the stem list into the map, or append to an existing stem list
    map<int, QStringList>::iterator it = stems.find (length);
    if (it == stems.end()) {
        stems.insert (make_pair (length, words));
        stemAlphagrams.insert (make_pair (length, alphagrams));
    }
    else {
        it->second += words;
        std::map< int, std::set<QString> >::iterator it =
            stemAlphagrams.find (length);
        std::set<QString>::iterator sit;
        for (sit = alphagrams.begin(); sit != alphagrams.end(); ++sit)
            (it->second).insert (*sit);
    }

    return imported;
}

//---------------------------------------------------------------------------
//  isAcceptable
//
//! Determine whether a word is acceptable.
//
//! @param word the word to look up
//! @return true if acceptable, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::isAcceptable (const QString& word) const
{
    return graph.containsWord (word);
}

//---------------------------------------------------------------------------
//  search
//
//! Search for acceptable words matching a search specification.
//
//! @param spec the search specification
//! @param allCaps whether to ensure the words in the list are all caps
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordEngine::search (const SearchSpec& spec, bool allCaps) const
{
    SearchSpec optimizedSpec = spec;
    optimizedSpec.optimize();

    // Big optimization if the only conditions are InWordList conditions:
    // don't even do a search, just test each word in the search condition for
    // acceptability.
    bool wordListsOnly = true;
    QListIterator<SearchCondition> cit (spec.conditions);
    while (cit.hasNext()) {
        if (cit.next().type != SearchCondition::InWordList) {
            wordListsOnly = false;
            break;
        }
    }
    if (wordListsOnly)
        return wordListOnlySearch (optimizedSpec);

    QStringList wordList = graph.search (optimizedSpec);

    // Check special postconditions
    QStringList::iterator wit;
    for (wit = wordList.begin(); wit != wordList.end();) {
        if (matchesConditions (*wit, optimizedSpec.conditions))
            ++wit;
        else
            wit = wordList.erase (wit);
    }

    // Convert to all caps if necessary
    if (allCaps) {
        QStringList::iterator it;
        for (it = wordList.begin(); it != wordList.end(); ++it)
            *it = (*it).upper();
    }
    return wordList;
}

//---------------------------------------------------------------------------
//  alphagrams
//
//! Transform a list of strings into a list of alphagrams of those strings.
//! The created list may be shorter than the original list.
//
//! @param list the list of strings
//! @return a list of alphagrams
//---------------------------------------------------------------------------
QStringList
WordEngine::alphagrams (const QStringList& list) const
{
    QStringList alphagrams;
    QStringList::const_iterator it;

    // Insert into a set first, to remove duplicates
    set<QString> seen;
    for (it = list.begin(); it != list.end(); ++it) {
        seen.insert (alphagram (*it));
    }

    set<QString>::iterator sit;
    for (sit = seen.begin(); sit != seen.end(); ++sit) {
        alphagrams << *sit;
    }
    return alphagrams;
}

//---------------------------------------------------------------------------
//  alphagram
//
//! Transform a string into its alphagram.
//
//! @param list the list of strings
//! @return a list of alphagrams
//---------------------------------------------------------------------------
QString
WordEngine::alphagram (const QString& word) const
{
    QList<QChar> qchars;

    char chars[MAX_WORD_LEN + 1];
    int wordLength = word.length();
    for (int i = 0; (i < wordLength) && (i < MAX_WORD_LEN); ++i) {
        qchars.append (word.at (i));
    }

    qSort (qchars);

    int i = 0;
    QListIterator<QChar> it (qchars);
    while (it.hasNext()) {
        chars[i] = it.next().toAscii();
        ++i;
    }
    chars[i] = 0;

    return QString (chars);
}

//---------------------------------------------------------------------------
//  getDefinition
//
//! Return the definition associated with a word.
//
//! @param word the word whose definition to look up
//! @return the definition, or QString::null if no definition
//---------------------------------------------------------------------------
QString
WordEngine::getDefinition (const QString& word) const
{
    map<QString, multimap<QString, QString> >::const_iterator it =
        definitions.find (word);
    if (it == definitions.end())
        return QString::null;

    const multimap<QString, QString>& mmap = it->second;
    multimap<QString, QString>::const_iterator mit;
    QString definition;
    for (mit = mmap.begin(); mit != mmap.end(); ++mit) {
        if (!definition.isEmpty())
            definition += " / ";
        definition += replaceDefinitionLinks (mit->second,
                                              MAX_DEFINITION_LINKS);
    }
    return definition;
}

//---------------------------------------------------------------------------
//  matchesConditions
//
//! Test whether a word matches certain conditions.  Not all conditions in the
//! list are tested.  Only the conditions that cannot be easily tested in
//! WordGraph::search are tested here.
//
//! @param word the word to be tested
//! @param conditions the list of conditions to test
//! @return true if the word matches all special conditions, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::matchesConditions (const QString& word, const
                               QList<SearchCondition>& conditions) const
{
    QString wordUpper = word.upper();
    QListIterator<SearchCondition> it (conditions);
    while (it.hasNext()) {
        const SearchCondition& condition = it.next();

        switch (condition.type) {

            case SearchCondition::TakesPrefix:
            if (!isAcceptable (condition.stringValue + wordUpper))
                return false;
            break;

            case SearchCondition::DoesNotTakePrefix:
            if (isAcceptable (condition.stringValue + wordUpper))
                return false;
            break;

            case SearchCondition::TakesSuffix:
            if (!isAcceptable (wordUpper + condition.stringValue))
                return false;
            break;

            case SearchCondition::DoesNotTakeSuffix:
            if (isAcceptable (wordUpper + condition.stringValue))
                return false;
            break;

            case SearchCondition::MustBelong: {
                SearchSet searchSet = Auxil::stringToSearchSet
                    (condition.stringValue);
                if (searchSet == UnknownSearchSet)
                    continue;
                if (!isSetMember (wordUpper, searchSet))
                    return false;
            }
            break;

            case SearchCondition::InWordList:
            if (!condition.stringValue.contains
                (QRegExp ("\\b" + wordUpper + "\\b")))
                return false;
            break;

            case SearchCondition::ExactAnagrams:
            if (numAnagrams (wordUpper) != condition.intValue)
                return false;
            break;

            case SearchCondition::MinAnagrams:
            if (numAnagrams (wordUpper) < condition.intValue)
                return false;
            break;

            case SearchCondition::MaxAnagrams:
            if (numAnagrams (wordUpper) > condition.intValue)
                return false;
            break;

            default: break;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
//  isSetMember
//
//! Determine whether a word is a member of a set.  Assumes the word has
//! already been determined to be acceptable.
//
//! @param word the word to look up
//! @param ss the search set
//! @return true if a member of the set, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::isSetMember (const QString& word, SearchSet ss) const
{
    switch (ss) {
        case SetHookWords:
        return (isAcceptable (word.left (word.length() - 1)) ||
                isAcceptable (word.right (word.length() - 1)));

        case SetFrontHooks:
        return isAcceptable (word.right (word.length() - 1));

        case SetBackHooks:
        return isAcceptable (word.left (word.length() - 1));

        case SetTypeOneSevens: {
            if (word.length() != 7)
                return false;

            std::map< int, std::set<QString> >::const_iterator it =
                stemAlphagrams.find (word.length() - 1);
            if (it == stemAlphagrams.end())
                return false;

            const std::set<QString>& alphaset = it->second;
            QString agram = alphagram (word);
            std::set<QString>::const_iterator ait;
            for (int i = 0; i < int (agram.length()); ++i) {
                ait = alphaset.find (agram.left (i) +
                                     agram.right (agram.length() - i - 1));
                if (ait != alphaset.end())
                    return true;
            }
            return false;
        }

        case SetTypeOneEights: {
            if (word.length() != 8)
                return false;

            std::map< int, std::set<QString> >::const_iterator it =
                stemAlphagrams.find (word.length() - 2);
            if (it == stemAlphagrams.end())
                return false;

            QString agram = alphagram (word);

            // Compare the letters of the word with the letters of each
            // alphagram, ensuring that no more than two letters in the word
            // are missing from the alphagram.
            const std::set<QString>& alphaset = it->second;
            std::set<QString>::const_iterator ait;
            for (ait = alphaset.begin(); ait != alphaset.end(); ++ait) {
                QString setAlphagram = *ait;
                int missing = 0;
                int saIndex = 0;
                for (int i = 0; i < int (agram.length()); ++i) {
                    if (agram.at (i) == setAlphagram.at (saIndex))
                        ++saIndex;
                    else
                        ++missing;
                    if (missing > 2)
                        break;
                }
                if (missing <= 2)
                    return true;
            }
            return false;
        }

        default: return false;
    }
}

//---------------------------------------------------------------------------
//  numAnagrams
//
//! Determine the number of valid anagrams of a word.
//
//! @param word the word
//! @return the number of valid anagrams
//---------------------------------------------------------------------------
int
WordEngine::numAnagrams (const QString& word) const
{
    SearchSpec spec;
    SearchCondition condition;
    condition.type = SearchCondition::AnagramMatch;
    condition.stringValue = word;
    spec.conditions.append (condition);
    return search (spec, true).size();
}

//---------------------------------------------------------------------------
//  wordListOnlySearch
//
//! Search for valid words matching In Word List conditions in a search spec.
//
//! @param spec the search specification
//
//! @return a list of acceptable words matching the In Word List conditions
//---------------------------------------------------------------------------
QStringList
WordEngine::wordListOnlySearch (const SearchSpec& spec) const
{
    QStringList wordList;
    std::set<QString> finalWordSet;
    std::set<QString>::iterator sit;
    int conditionNum = 0;

    QListIterator<SearchCondition> it (spec.conditions);
    while (it.hasNext()) {
        const SearchCondition& condition = it.next();
        if (condition.type != SearchCondition::InWordList)
            continue;
        QStringList words = QStringList::split (" ", condition.stringValue);

        std::set<QString> wordSet;
        QStringList::iterator wit;
        for (wit = words.begin(); wit != words.end(); ++wit) {
            if (isAcceptable (*wit))
                wordSet.insert (*wit);
        }

        if (!conditionNum) {
            finalWordSet = wordSet;
        }

        else if (spec.conjunction) {
            set<QString> conjunctionSet;
            for (sit = wordSet.begin(); sit != wordSet.end(); ++sit) {
                if (finalWordSet.find (*sit) != finalWordSet.end())
                    conjunctionSet.insert (*sit);
            }
            if (conjunctionSet.empty())
                return wordList;
            finalWordSet = conjunctionSet;
        }

        else {
            for (sit = wordSet.begin(); sit != wordSet.end(); ++sit) {
                finalWordSet.insert (*sit);
            }
        }

        ++conditionNum;
    }

    // Transform word set into word list and return it
    for (sit = finalWordSet.begin(); sit != finalWordSet.end(); ++sit) {
        wordList << *sit;
    }

    return wordList;
}

//---------------------------------------------------------------------------
//  replaceDefinitionLinks
//
//! Replace links in a definition with the definitions of the words they are
//! linked to.  A string is assumed to have a maximum of one link.  Links may
//! be followed recursively to the maximum depth specified.
//
//! @param definition the definition with links to be replaced
//! @param maxDepth the maximum number of recursive links to replace
//! @param useFollow true if the "follow" replacement should be used
//
//! @return a string with links replaced
//---------------------------------------------------------------------------
QString
WordEngine::replaceDefinitionLinks (const QString& definition, int maxDepth,
                                    bool useFollow) const
{
    QRegExp followRegex (QString ("\\{(\\w+)=(\\w+)\\}"));
    QRegExp replaceRegex (QString ("\\<(\\w+)=(\\w+)\\>"));

    // Try to match the follow regex and the replace regex.  If a follow regex
    // is ever matched, then the "follow" replacements should always be used,
    // even if the "replace" regex is matched in a later iteration.
    QRegExp* matchedRegex = 0;
    int index = followRegex.indexIn (definition, 0);
    if (index >= 0) {
        matchedRegex = &followRegex;
        useFollow = true;
    }
    else {
        index = replaceRegex.indexIn (definition, 0);
        matchedRegex = &replaceRegex;
    }

    if (index < 0)
        return definition;

    QString modified (definition);
    QString word = matchedRegex->cap (1);
    QString pos = matchedRegex->cap (2);

    QString replacement;
    if (!maxDepth) {
        replacement = word;
    }
    else {
        QString upper = word.toUpper();
        if (useFollow) {
            replacement = (matchedRegex == &followRegex) ?
                word + " (" + getSubDefinition (upper, pos) + ")" :
                getSubDefinition (upper, pos);
        }
        else {
            replacement = upper + ", " + getSubDefinition (upper, pos);
        }
    }

    modified.replace (index, matchedRegex->matchedLength(), replacement);
    return maxDepth ? replaceDefinitionLinks (modified, maxDepth - 1,
                                              useFollow) : modified;
}

//---------------------------------------------------------------------------
//  getSubDefinition
//
//! Return the definition associated with a word and a part of speech.  If
//! more than one definition is given for a part of speech, pick the first
//! one.
//
//! @param word the word
//! @param pos the part of speech
//
//! @return the definition substring
//---------------------------------------------------------------------------
QString
WordEngine::getSubDefinition (const QString& word, const QString& pos) const
{
    std::map<QString, std::multimap<QString, QString> >::const_iterator it =
        definitions.find (word);
    if (it == definitions.end())
        return QString::null;

    const multimap<QString, QString>& mmap = it->second;
    multimap<QString, QString>::const_iterator mit = mmap.find (pos);
    if (mit == mmap.end())
        return QString::null;

    return mit->second.left (mit->second.indexOf (" ["));
}
