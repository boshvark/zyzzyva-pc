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
#include <qvaluevector.h>
#include <set>

using namespace Defs;
using std::set;

const int READLINE_BYTES = 64;

//---------------------------------------------------------------------------
// importFile
//
//! Import words from a file.  The file is assumed to be in plain text format,
//! containing one word per line.
//
//! @param filename the name of the file to import
//! @return the number of words imported
//---------------------------------------------------------------------------
int
WordEngine::importFile (const QString& filename, QString* errString)
{
    QFile file (filename);
    if (!file.open (IO_ReadOnly)) {
        if (errString)
            *errString = "Can't open file '" + filename + "': "
            + file.errorString();
        return -1;
    }

    int imported = 0;
    QString word;
    while (file.readLine (word, READLINE_BYTES) > 0) {
        graph.addWord (word.stripWhiteSpace());
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
// matchPattern
//
//! Find all acceptable words matching a pattern.
//
//! @param input the pattern to match
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordEngine::matchPattern (const QString& input) const
{
    return graph.getWordsMatchingPattern (input);
}

//---------------------------------------------------------------------------
// matchAnagram
//
//! Find all acceptable anagrams of a string of letters.
//
//! @param input the letters to match
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordEngine::matchAnagram (const QString& input) const
{
    return graph.getAnagrams (input, false);
}

//---------------------------------------------------------------------------
// matchSubanagram
//
//! Find all acceptable anagrams and subanagrams of a string of letters.
//
//! @param input the letters to match
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordEngine::matchSubanagram (const QString& input) const
{
    return graph.getAnagrams (input, true);
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
    for (int i = 0; (i < word.length()) && (i < MAX_WORD_LEN); ++i) {
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
