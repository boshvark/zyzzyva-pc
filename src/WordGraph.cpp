//---------------------------------------------------------------------------
// WordGraph.cpp
//
// A Directed Acyclic Word Graph class.
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

#include "WordGraph.h"
#include "Defs.h"
#include <qregexp.h>
#include <iostream>
#include <stack>
#include <set>

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
    int wordLength = w.length();
    for (int i = 0; i < wordLength; ++i) {
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
    int wordLength = w.length();
    for (int i = 0; i < wordLength; ++i) {
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
// search
//
//! Search for acceptable words matching a search specification.
//
//! @param spec the search specification
//! @return a list of acceptable words
//---------------------------------------------------------------------------
QStringList
WordGraph::search (const SearchSpec& spec) const
{
    QStringList wordList;

    // Pattern match is specified
    if (spec.type == Pattern) {

        // Must use set to eliminate duplicates since patterns with wildcards
        // may match the same word in more than one way
        set<QString> wordSet;

        if (!top)
            return wordList;

        QString pattern = spec.pattern;
        if (pattern.isEmpty())
            pattern = "*";
        else
            pattern.replace (QRegExp ("\\*+"), "*");

        stack< pair<Node*, pair<int,QString> > > nodeStack;
        int pLen = pattern.length();
        QString word;

        Node* node = top;
        QChar c;
        int pIndex = 0;
        while (node) {
            QString origWord = word;
            c = pattern.at (pIndex);

            // Allow wildcard to match empty string
            if (c == "*")
                nodeStack.push (make_pair (node, make_pair (pIndex + 1, word)));

            // Traverse next nodes, looking for matches
            while (node) {
                bool match = false;
                word = origWord;

                // A node matches wildcard characters or its own letter
                if ((c == "*") || (c == "?") || (c == node->letter)) {
                    word += node->letter;
                    match = true;
                }

                // If this node matches, push its child on the stack to be
                // traversed later
                if (match && node->child) {
                    if (c == "*")
                        nodeStack.push (make_pair (node->child,
                                                   make_pair (pIndex, word)));

                    nodeStack.push (make_pair (node->child,
                                               make_pair (pIndex + 1, word)));
                }

                // If end of word and end of pattern, put the word in the list
                if (match && node->eow &&
                    ((pIndex == pLen - 1) ||
                    ((pIndex == pLen - 2) &&
                     (QChar (pattern.at (pIndex + 1)) == "*"))))
                    wordSet.insert (word);

                node = node->next;
            }

            // Done traversing next nodes, pop a child off the stack
            if (nodeStack.size()) {
                node = nodeStack.top().first;
                pIndex = nodeStack.top().second.first;
                word = nodeStack.top().second.second;
                nodeStack.pop();
            }
        }

        // Build word list from word set
        set<QString>::iterator it;
        for (it = wordSet.begin(); it != wordSet.end(); ++it)
            wordList << *it;
    }

    else if ((spec.type == Anagram) || (spec.type == Subanagram)) {
        bool subanagrams = (spec.type == Subanagram);

        stack< pair<Node*, pair<int,QString> > > nodeStack;
        char word[MAX_WORD_LEN + 1];
        QString unmatched = spec.pattern;
        bool wildcard = false;

        // Note if any wildcard chars are present, then get rid of them
        if (unmatched.contains ('*')) {
            wildcard = true;
            unmatched = unmatched.replace ('*', "");
        }

        Node* node = top;
        QChar c;
        int i = 0;

        while (node) {
            c = node->letter;

            int index = unmatched.find (c);
            if (index < 0)
                index = unmatched.find ("?");

            if ((index >= 0) || wildcard) {
                if (node->next)
                    nodeStack.push (make_pair (node->next,
                                               make_pair (i, unmatched)));
                word[i] = c;
                ++i;
                if (index >= 0)
                    unmatched.replace (index, 1, "");

                if (node->eow && (subanagrams || unmatched.isEmpty())) {
                    word[i] = 0;
                    wordList << QString (word);
                }

                node = (unmatched.isEmpty() && !wildcard) ? 0 : node->child;
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
    }

    return wordList;
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
