//---------------------------------------------------------------------------
// DAWG.cpp
//
// A Directed Acyclic Word Graph class.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "DAWG.h"
#include <iostream>
#include <stack>

using namespace std;

//---------------------------------------------------------------------------
// DAWG
//
//! Constructor.
//---------------------------------------------------------------------------
DAWG::DAWG()
    : top (0)
{
}

//---------------------------------------------------------------------------
// ~DAWG
//
//! Destructor.
//---------------------------------------------------------------------------
DAWG::~DAWG()
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
DAWG::addWord (const QString& w)
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
        }

        // Nonempty node, so find the current letter in the chain
        else {
            while (node->letter != c) {
                if (!node->next) {
                    node->next = new Node (c);
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
DAWG::containsWord (const QString& w) const
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
DAWG::getWordsMatchingPattern (const QString& pattern) const
{
    QStringList list;

    if (pattern.isEmpty() || !top)
        return list;

    stack< pair<Node*,int> > nodeStack;
    int len = pattern.length();
    char word[len + 1];

    Node* node = top;
    QChar c;
    int i = 0;
    while (node) {
        c = pattern.at (i);

        if (c == "?") {
            if (node->next)
                nodeStack.push (make_pair (node->next, i));
        }

        else {
            while (node && node->letter != c)
                node = node->next;
        }


        if (node) {
            word[i] = node->letter;
            ++i;

            if (i == len) {
                if (node->eow) {
                    word[len] = 0;
                    list << QString (word);
                }
            }
            else
                node = node->child;
        }

        if (!node || (i == len)) {
            if (!nodeStack.size())
                break;
            node = nodeStack.top().first;
            i = nodeStack.top().second;
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
DAWG::compress()
{
}

//---------------------------------------------------------------------------
// print
//
//! Print a rudimentary ASCII representation of the graph.
//---------------------------------------------------------------------------
void
DAWG::print() const
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
DAWG::Node::Node(char c, bool e)
    : letter (c), eow (e), next (0), child (0)
{
}

//---------------------------------------------------------------------------
// ~Node
//
//! Destructor.  Destroy this node along with its next and child nodes.
//---------------------------------------------------------------------------
DAWG::Node::~Node()
{
    delete next;
    delete child;
}
