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

const int MAX_INPUT_LINE_LEN = 640;

//---------------------------------------------------------------------------
// importFile
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
    QString word, line;
    while (file.readLine (line, MAX_INPUT_LINE_LEN) > 0) {
        line = line.simplifyWhiteSpace();
        QString word = line.section (' ', 0, 0);
        graph.addWord (word);
        if (loadDefinitions)
            definitions.insert (std::make_pair (word, line.section (' ', 1)));
        ++imported;
    }

    return imported;
}

//---------------------------------------------------------------------------
// isAcceptable
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
// search
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
    if (!allCaps || !spec.pattern.contains (QRegExp ("[\\*\\?]")))
        return graph.search (spec);

    QStringList wordList = graph.search (spec);
    QStringList::iterator it;
    for (it = wordList.begin(); it != wordList.end(); ++it)
        *it = (*it).upper();
    return wordList;
}

//---------------------------------------------------------------------------
// alphagrams
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
// alphagram
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
