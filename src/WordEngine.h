//---------------------------------------------------------------------------
// WordEngine.h
//
// A class to handle the loading and searching of words.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef WORD_ENGINE_H
#define WORD_ENGINE_H

#include "WordGraph.h"
#include <qstring.h>
#include <qstringlist.h>

class WordEngine
{
  public:
    WordEngine() { }
    ~WordEngine() { }

    int importFile (const QString& filename, QString* errString = 0);
    bool isAcceptable (const QString& word) const;
    QStringList matchPattern (const QString& input) const;
    QStringList matchAnagram (const QString& input) const;
    QStringList matchSubanagram (const QString& input) const;

  private:
    WordGraph graph;
};

#endif // WORD_ENGINE_H
