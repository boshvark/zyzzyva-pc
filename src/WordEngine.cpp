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
        ++ imported;
    }

    // Insert the stem list into the map, or append to an existing stem list
    map<int, QStringList>::iterator it = stems.find (length);
    if (it == stems.end())
        stems.insert (make_pair (length, words));
    else
        it->second += words;

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
    // Allow a smart compiler to optimize by returing directly from
    // WordGraph::search unless we need to manipulate the return list
    if ((!allCaps || !spec.pattern.contains (QRegExp ("[\\*\\?]"))) &&
        spec.setMemberships.empty())
    {
        return graph.search (spec);
    }

    QStringList wordList = graph.search (spec);

    // Check set membership
    if (!spec.setMemberships.empty()) {
        QStringList::iterator it;
        std::set<SearchSet>::const_iterator sit;
        for (it = wordList.begin(); it != wordList.end();) {
            bool match = false;
            for (sit = spec.setMemberships.begin();
                 sit != spec.setMemberships.end();
                 ++sit)
            {
                if (isSetMember ((*it).upper(), *sit)) {
                    match = true;
                    break;
                }
            }
            if (match)
                ++it;
            else
                it = wordList.erase (it);
        }
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



        default: return false;
    }
}
