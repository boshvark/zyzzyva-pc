//---------------------------------------------------------------------------
// DAWG.h
//
// A Directed Acyclic Word Graph class.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef DAWG_H
#define DAWG_H

#include <qstring.h>
#include <qstringlist.h>

class DAWG
{
  public:
    DAWG();
    ~DAWG();

    void addWord (const QString& w);
    bool containsWord (const QString& w) const;
    QStringList getWordsMatchingPattern (const QString& pattern) const;
    QStringList getAnagrams (const QString& input, bool subanagrams) const;
    void compress();
    void print() const;

  private:
    class Node {
      public:
        Node(char c = 0, bool e = false);
        ~Node();
        char letter;
        bool eow;
        Node* next;
        Node* child;
    };

    Node* top;
};

#endif // DAWG_H
