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
WordEngine::importFile (const QString& filename)
{
    QFile file (filename);
    if (!file.open (IO_ReadOnly))
        qWarning ("Can't open file '" + filename + "': " + file.errorString());

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
//! @param pattern the pattern to match
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordEngine::matchPattern (const QString& pattern) const
{
    return graph.getWordsMatchingPattern (pattern);
}
