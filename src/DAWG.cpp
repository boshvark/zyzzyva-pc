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
DAWG::addWord (const string& w)
{
    if (!w.size())
        return;

    Node* node = top;
    Node* parentNode = 0;

    string::const_iterator c;
    for (c = w.begin(); c != w.end(); ++c) {

        // Empty node, so create a new node and link from its parent
        if (!node) {
            node = new Node (*c);
            (parentNode ? parentNode->child : top) = node;
        }

        // Nonempty node, so find the current letter in the chain
        else {
            while (node->letter != *c) {
                if (!node->next)
                    node->next = new Node (*c);
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
DAWG::containsWord (const string& w) const
{
    if (!w.size() || !top)
        return false;

    Node* node = top;
    bool eow = false;

    string::const_iterator c;
    for (c = w.begin(); c != w.end(); ++c) {
        if (!node)
            return false;

        while (node->letter != *c) {
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
