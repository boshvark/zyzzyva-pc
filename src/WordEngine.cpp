//---------------------------------------------------------------------------
// WordEngine.cpp
//
// A class to handle the loading and searching of words.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
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
#include <qfile.h>
#include <qregexp.h>
#include <qvaluevector.h>
#include <set>

using namespace Defs;
using std::set;
using std::map;
using std::make_pair;

const int MAX_INPUT_LINE_LEN = 640;

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
    if (!file.open (IO_ReadOnly)) {
        if (errString)
            *errString = "Can't open file '" + filename + "': "
            + file.errorString();
        return -1;
    }

    int imported = 0;
    QString line;
    while (file.readLine (line, MAX_INPUT_LINE_LEN) > 0) {
        line = line.simplifyWhiteSpace();
        if (!line.length() || (line.at (0) == '#'))
            continue;
        QString word = line.section (' ', 0, 0);
        graph.addWord (word);
        if (loadDefinitions) {
            QString definition = line.section (' ', 1);
            if (!definition.isEmpty())
                definitions.insert (make_pair (word, definition));
        }
        ++imported;
    }

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
    if (!file.open (IO_ReadOnly)) {
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
    QString line;
    while (file.readLine (line, MAX_INPUT_LINE_LEN) > 0) {
        line = line.simplifyWhiteSpace();
        if (!line.length() || (line.at (0) == '#'))
            continue;
        QString word = line.section (' ', 0, 0);

        if (!length)
            length = word.length();

        if (word.length() != length)
            continue;

        words << word;
        alphagrams.insert (alphagram (word));
        ++imported;
    }

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
    QStringList wordList = graph.search (spec);

    // Check special conditions
    QStringList::iterator wit;
    for (wit = wordList.begin(); wit != wordList.end();) {
        if (matchesConditions (*wit, spec.conditions))
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
    QValueVector<QChar> qchars;

    char chars[MAX_WORD_LEN + 1];
    int wordLength = word.length();
    for (int i = 0; (i < wordLength) && (i < MAX_WORD_LEN); ++i) {
        qchars.push_back (word.at (i));
    }

    qHeapSort (qchars);

    int i = 0;
    QValueVector<QChar>::iterator it;
    for (it = qchars.begin(); it != qchars.end(); ++it, ++i) {
        chars[i] = *it;
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
    map<QString,QString>::const_iterator it = definitions.find (word);
    return ((it == definitions.end()) ? QString::null : it->second);
}

//---------------------------------------------------------------------------
//  matchesConditions
//
//! Test whether a word matches certain conditions.  Not all conditions in the
//! list are tested.  Only set membership and number of anagrams are currently
//! tested.
//
//! @param word the word to be tested
//! @param conditions the list of conditions to test
//! @return true if the word matches all special conditions, false otherwise
//---------------------------------------------------------------------------
bool
WordEngine::matchesConditions (const QString& word, const
                               QValueList<SearchCondition>& conditions) const
{
    QValueList<SearchCondition>::const_iterator it;
    for (it = conditions.begin(); it != conditions.end(); ++it) {

        switch ((*it).type) {

            case SearchCondition::MustBelong: {
                SearchSet searchSet = Auxil::stringToSearchSet
                    ((*it).stringValue);
                if (searchSet == UnknownSearchSet)
                    continue;
                if (!isSetMember (word.upper(), searchSet))
                    return false;
            }
            break;

            case SearchCondition::ExactAnagrams:
            if (numAnagrams (word) != (*it).intValue)
                return false;
            break;

            case SearchCondition::MinAnagrams:
            if (numAnagrams (word) < (*it).intValue)
                return false;
            break;

            case SearchCondition::MaxAnagrams:
            if (numAnagrams (word) > (*it).intValue)
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


        case SetTypeOneSevens: {
            if (word.length() != 7)
                return false;

            std::map< int, std::set<QString> >::const_iterator it =
                stemAlphagrams.find (word.length() - 1);
            if (it == stemAlphagrams.end())
                return false;

            const std::set<QString>& alphaset = it->second;
            QString agram = alphagram (word);
            for (int i = 0; i < agram.length(); ++i) {
                if (alphaset.find (agram.left (i) +
                                   agram.right (agram.length() - i - 1))
                    != alphaset.end())
                {
                    return true;
                }
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
                for (int i = 0; i < agram.length(); ++i) {
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
    spec.conditions << condition;
    return search (spec, true).size();
}
