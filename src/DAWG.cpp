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

DAWG::DAWG()
    : top (0)
{
}

DAWG::~DAWG()
{
}

void
DAWG::addWord (const string& w)
{
    Node* node = top;
    Node* parentNode = 0;

    string::const_iterator c;
    for (c = w.begin(); c != w.end(); ++c) {
        //cerr << "Char: |" << *c << "|" << endl;

        // Empty node, so create a new node and link from its parent
        if (!node) {
            //cerr << "Adding child node |" << *c << "|" << endl;
            node = new Node (*c);
            (parentNode ? parentNode->child : top) = node;
        }

        // Nonempty node, so find the current letter in the chain
        else {
            while (node->letter != *c) {
                if (node->next)
                    node = node->next;
                else {
                    //cerr << "Adding next node |" << *c << "|" << endl;
                    node->next = new Node (*c);
                    node = node->next;
                }
            }
        }

        parentNode = node;
        node = node->child;
    }

    parentNode->eow = true;
}

bool
DAWG::containsWord (const string& w) const
{
    return true;
}


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

DAWG::Node::Node(char c, bool e)
    : letter (c), eow (e), next (0), child (0)
{
}

DAWG::Node::~Node()
{
    delete next;
    delete child;
}
