//---------------------------------------------------------------------------
// WordGraph.cpp
//
// A Directed Acyclic Word Graph class.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "WordGraph.h"
#include "Defs.h"
#include <qregexp.h>
#include <iostream>
#include <stack>

using namespace std;
using namespace Defs;

//---------------------------------------------------------------------------
// WordGraph
//
//! Constructor.
//---------------------------------------------------------------------------
WordGraph::WordGraph()
    : top (0)
{
}

//---------------------------------------------------------------------------
// ~WordGraph
//
//! Destructor.
//---------------------------------------------------------------------------
WordGraph::~WordGraph()
{
}

//---------------------------------------------------------------------------
// addWord
//
//! Add a word to the graph.
//
//! @param w the word to add
//---------------------------------------------------------------------------
void
WordGraph::addWord (const QString& w)
{
    if (w.isEmpty())
        return;

    Node* node = top;
    Node* parentNode = 0;
    QChar c;
    for (int i = 0; i < w.length(); ++i) {
        c = w.at (i);

        // Empty node, so create a new node and link from its parent
        if (!node) {
            node = new Node (c);
            (parentNode ? parentNode->child : top) = node;
            ++numNodes;
        }

        // Nonempty node, so find the current letter in the chain
        else {
            while (node->letter != c) {
                if (!node->next) {
                    node->next = new Node (c);
                    ++numNodes;
                }
                node = node->next;
            }
        }

        parentNode = node;
        node = node->child;
    }

    parentNode->eow = true;
}

//---------------------------------------------------------------------------
// containsWord
//
//! Determine whether the graph contains a word.
//
//! @param w the word to search for
//---------------------------------------------------------------------------
bool
WordGraph::containsWord (const QString& w) const
{
    if (w.isEmpty() || !top)
        return false;

    Node* node = top;
    bool eow = false;
    QChar c;
    for (int i = 0; i < w.length(); ++i) {
        if (!node)
            return false;

        c = w.at (i);
        while (node->letter != c) {
            if (!node->next)
                return false;
            node = node->next;
        }

        eow = node->eow;
        node = node->child;
    }

    return eow;
}

//---------------------------------------------------------------------------
// getWordsMatchingPattern
//
//! Find all acceptable words matching a pattern.
//
//! @param pattern the pattern to match
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordGraph::getWordsMatchingPattern (const QString& p) const
{
    QStringList list;

    if (p.isEmpty() || !top)
        return list;

    // Searches are simpler with no wildcard, so note whether one appears
    bool hasWildcard = p.contains ('*');

    QString pattern = p;
    pattern.replace (QRegExp ("\\*+"), "*");

    stack< pair<Node*, pair<int,int> > > nodeStack;
    int pLen = pattern.length();
    char word[MAX_WORD_LEN + 1];

    Node* node = top;
    QChar c;
    int i = 0;
    int pIndex = 0;
    int wIndex = 0;
    while (node) {
        c = pattern.at (pIndex);

        if (c == "?") {
            if (node->next)
                nodeStack.push (make_pair (node->next,
                                           make_pair (pIndex, wIndex)));
        }

        else if (c == "*") {
            if (pIndex < pLen - 1)
                nodeStack.push (make_pair (node,
                                           make_pair (pIndex + 1, wIndex)));
            if (node->next)
                nodeStack.push (make_pair (node->next,
                                           make_pair (pIndex, wIndex)));
        }

        else {
            while (node && node->letter != c)
                node = node->next;
        }

        if (node) {
            word[pIndex] = node->letter;
            ++pIndex;
            ++wIndex;

            if (pIndex == pLen) {
                if (node->eow) {
                    word[pLen] = 0;
                    list << QString (word);
                }
            }
            else
                node = node->child;
        }

        if ((!node || (pIndex == pLen)) && nodeStack.size()) {
            node = nodeStack.top().first;
            pIndex = nodeStack.top().second.first;
            wIndex = nodeStack.top().second.second;
            nodeStack.pop();
        }
    }

    return list;
}

//---------------------------------------------------------------------------
// getAnagrams
//
//! Find all anagrams in an input string.
//
//! @param input the letters to match
//! @param subanagrams whether to match subanagrams (not using all letters)
//! @return a list of acceptable anagrams
//---------------------------------------------------------------------------
QStringList
WordGraph::getAnagrams (const QString& input, bool subanagrams) const
{
    QStringList list;

    stack< pair<Node*, pair<int,QString> > > nodeStack;
    int len = input.length();
    char word[MAX_WORD_LEN + 1];
    QString unmatched = input;

    Node* node = top;
    QChar c;
    int i = 0;

    while (node) {
        c = node->letter;

        int index = unmatched.find (c);
        if (index < 0)
            index = unmatched.find ("?");

        if (index >= 0) {
            if (node->next)
                nodeStack.push (make_pair (node->next,
                                           make_pair (i, unmatched)));
            word[i] = c;
            ++i;
            unmatched.replace (index, 1, "");

            if (node->eow && (subanagrams || unmatched.isEmpty())) {
                word[i] = 0;
                list << QString (word);
            }

            node = unmatched.isEmpty() ? 0 : node->child;
        }
        else
            node = node->next;

        if (!node && nodeStack.size()) {
            node = nodeStack.top().first;
            i = nodeStack.top().second.first;
            unmatched = nodeStack.top().second.second;
            nodeStack.pop();
        }
    }

    return list;
}

//---------------------------------------------------------------------------
// compress
//
//! Compress the graph by eliminating duplication of common suffixes.
//---------------------------------------------------------------------------
void
WordGraph::compress()
{
}

//---------------------------------------------------------------------------
// print
//
//! Print a rudimentary ASCII representation of the graph.
//---------------------------------------------------------------------------
void
WordGraph::print() const
{
    if (!top) {
        cout << "(empty)" << endl;
        return;
    }

    Node* node = top;
    stack< pair<Node*,int> > nodeStack;
    int depth = 0;

    while (node) {
        cout << node->letter;
        if (node->eow)
            cout << "*";
        else
            cout << " ";

        if (!node->child) {
            cout << endl;
            while (!node->next) {
                if (!nodeStack.size())
                    break;
                node = nodeStack.top().first;
                depth = nodeStack.top().second;
                nodeStack.pop();
            }
            node = node->next;
            for (int i = 0; i < depth; ++i)
                cout << "  ";
        }

        else {
            nodeStack.push (make_pair (node, depth));
            node = node->child;
            ++depth;
        }

    }

}

//---------------------------------------------------------------------------
// Node
//
//! Constructor.
//---------------------------------------------------------------------------
WordGraph::Node::Node(char c, bool e)
    : letter (c), eow (e), next (0), child (0)
{
}

//---------------------------------------------------------------------------
// ~Node
//
//! Destructor.  Destroy this node along with its next and child nodes.
//---------------------------------------------------------------------------
WordGraph::Node::~Node()
{
    delete next;
    delete child;
}

//---------------------------------------------------------------------------
// operator==
//
//! Compare two nodes to see if they are equivalent.  Two nodes are
//! equivalent if their internal data are equal, and if their CHILD and
//! NEXT pointers are exactly the same.
//
//! @param rhs the right-hand side of the comparison
//! @return true if equivalent, false otherwise
//---------------------------------------------------------------------------
bool
WordGraph::Node::operator== (const Node& rhs)
{
    return ((letter == rhs.letter) && (eow == rhs.eow)
            && (next == rhs.next) && (child == rhs.child));
}
