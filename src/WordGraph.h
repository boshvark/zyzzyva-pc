//---------------------------------------------------------------------------
// WordGraph.h
//
// A Directed Acyclic Word Graph class.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef WORD_GRAPH_H
#define WORD_GRAPH_H

#include <qstring.h>
#include <qstringlist.h>

class WordGraph
{
  public:
    WordGraph();
    ~WordGraph();

    void addWord (const QString& w);
    bool containsWord (const QString& w) const;
    QStringList getWordsMatchingPattern (const QString& pattern) const;
    QStringList getAnagrams (const QString& input, bool subanagrams) const;
    void compress();
    void print() const;
    int getNumNodes() { return numNodes; }

  private:
    class Node {
      public:
        Node (char c = 0, bool e = false);
        ~Node();
        bool operator== (const Node& rhs);
        char letter;
        bool eow;
        Node* next;
        Node* child;
    };

    Node* top;
    int numNodes;
};

#endif // WORD_GRAPH_H
