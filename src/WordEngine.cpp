//---------------------------------------------------------------------------
// WordEngine.cpp
//
// A class to handle the loading and searching of words.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "WordEngine.h"
#include <qfile.h>

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
