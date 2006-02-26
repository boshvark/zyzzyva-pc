//---------------------------------------------------------------------------
// WordGraph.cpp
//
// A Directed Acyclic Word Graph class.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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
#include <QList>
#include <QRegExp>
#include <iostream>
#include <map>
#include <stack>

using namespace std;
using namespace Defs;

//---------------------------------------------------------------------------
//  WordGraph
//
//! Constructor.
//---------------------------------------------------------------------------
WordGraph::WordGraph()
    : top (0), rtop (0)
{
}

//---------------------------------------------------------------------------
//  ~WordGraph
//
//! Destructor.
//---------------------------------------------------------------------------
WordGraph::~WordGraph()
{
}

//---------------------------------------------------------------------------
//  clear
//
//! Remove all nodes from the graph.
//---------------------------------------------------------------------------
void
WordGraph::clear()
{
}

//---------------------------------------------------------------------------
//  addWord
//
//! Add a word to the graph.  Represent the word in both forward and reverse
//! order for quick lookup in either direction.
//
//! @param w the word to add
//---------------------------------------------------------------------------
void
WordGraph::addWord (const QString& w)
{
    if (w.isEmpty())
        return;

    addWord (w, false);
    addWord (w, true);
}

//---------------------------------------------------------------------------
//  containsWord
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
//  search
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
    if (!top || spec.conditions.empty())
        return wordList;

    QList<SearchCondition> matchConditions;
    int maxLength = MAX_WORD_LEN;
    QString excludeLetters;
    int numWildcardConditions = 0;

    QListIterator<SearchCondition> it (spec.conditions);
    while (it.hasNext()) {
        const SearchCondition& condition = it.next();

        switch (condition.type) {
            case SearchCondition::PatternMatch:
            case SearchCondition::AnagramMatch:
            case SearchCondition::SubanagramMatch:
            matchConditions.append (condition); 
            if (condition.stringValue.contains ("?") ||
                condition.stringValue.contains ("["))
                ++numWildcardConditions;
            break;

            case SearchCondition::Length:
            if (condition.maxValue < maxLength)
                maxLength = condition.maxValue;
            break;

            case SearchCondition::IncludeLetters:
            if (condition.negated)
                excludeLetters += condition.stringValue;
            break;

            default: break;
        }
    }

    // Only replace wildcard matches with lower case letters if there is
    // exactly one pattern using wildcards
    // XXX: Commented out because it may be a reasonable default to use the
    // lower case lettering as matched by the first such condition
    //bool wildcardLower = (numWildcardConditions == 1);
    bool wildcardLower = true;

    // If no match condition was specified, search for all words matching the
    // other conditions
    if (matchConditions.empty()) {
        SearchCondition condition;
        condition.type = SearchCondition::PatternMatch;
        condition.stringValue = "*";
        matchConditions.append (condition);
    }

    map<QString, QString> finalWordSet;
    map<QString, QString>::iterator sit;
    int conditionNum = 0;

    // Search for each condition separately, and take the conjunction or
    // disjunction of the result sets
    QListIterator<SearchCondition> mit (matchConditions);
    while (mit.hasNext()) {
        const SearchCondition& condition = mit.next();
        QString unmatched = condition.stringValue;

        // Use set to eliminate duplicates since patterns with wildcards may
        // match the same word in more than one way
        map<QString, QString> wordSet;
        stack <TraversalState> states;
        QString word;

        bool wildcard = false;
        bool reversePattern = false;

        // If Pattern match is unspecified, change it to a single wildcard
        // character.  Also, remove any redundant wildcards.
        if (condition.type == SearchCondition::PatternMatch) {
            if (unmatched.isEmpty())
                unmatched = "*";
            else
                unmatched.replace (QRegExp ("\\*+"), "*");

            if ((unmatched.left (1) == "*") && (unmatched.right (1) != "*")) {
                unmatched = reverseString (unmatched);
                reversePattern = true;
            }
        }

        // If Anagram or Subanagram match contains a wildcard, note it and remove
        // the wildcard character from the match pattern.  Also move character
        // classes to the end of the string so they will be seen last if
        // moving sequentially through the string looking for matches.
        else if ((condition.type == SearchCondition::AnagramMatch) ||
                 (condition.type == SearchCondition::SubanagramMatch))
        {
            wildcard = unmatched.contains ('*');
            if (wildcard)
                unmatched = unmatched.replace ('*', "");

            QRegExp re ("\\[[^\\]]*\\][A-Z]");
            int pos = 0;
            while ((pos = re.indexIn (unmatched, pos)) >= 0) {
                unmatched = unmatched.left (re.pos()) +
                    unmatched.right (unmatched.length() -
                                    (re.pos() + re.matchedLength()) + 1) +
                    unmatched.mid (re.pos(), re.matchedLength() - 1);
                pos += re.matchedLength();
            }
        }

        Node* node = (reversePattern ? rtop : top);

        // Traverse the tree looking for matches
        while (node) {

            // Stop if word is at max length
            if (int (word.length()) < maxLength) {
                QString origWord = word;
                QString origUnmatched = unmatched;

                QString match;
                int closeIndex = 0;

                // Get the next character in the Pattern match.  Allow a
                // wildcard to match the empty string.
                if ((condition.type == SearchCondition::PatternMatch) &&
                    (!unmatched.isEmpty()))
                {
                    match = unmatched.at (0);
                    if (match == "*")
                        states.push (TraversalState (node, word,
                            unmatched.right (unmatched.length() - 1)));

                    else if (match == "[") {
                        closeIndex = unmatched.indexOf (']', 0);
                        match = unmatched.mid (1, closeIndex);
                    }
                }

                // Traverse next nodes, looking for matches
                for (; node; node = node->next) {
                    if (excludeLetters.contains (node->letter))
                        continue;

                    unmatched = origUnmatched;
                    word = origWord;

                    // Special processing for Pattern match
                    if (condition.type == SearchCondition::PatternMatch) {

                        // A node matches wildcard characters or its own
                        // letter
                        bool matchLetter = match.contains (node->letter);
                        bool matchNegated = match.contains ("^");
                        QChar c = (match.contains ("]") || (match == "?"))
                            ? node->letter.toLower() : node->letter;

                        if ((match == "*") || (match == "?") ||
                            (matchLetter ^ matchNegated))
                            word += c;
                        else
                            continue;

                        // If this node matches, push its child on the stack
                        // to be traversed later
                        if (node->child) {
                            if (match == "*")
                                states.push (TraversalState (node->child,
                                                             word,
                                                             unmatched));

                            states.push (TraversalState (node->child, word,
                                unmatched.right (unmatched.length() -
                                                 closeIndex - 1)));
                        }

                        // If end of word and end of pattern, put the word in
                        // the list.  If we are searching the reverse list,
                        // reverse the word first.
                        QString wordUpper = word.toUpper();
                        if (reversePattern)
                            wordUpper = reverseString (wordUpper);

                        if (node->eow &&
                            ((int (unmatched.length()) == closeIndex + 1) ||
                            ((int (unmatched.length()) == closeIndex + 2) &&
                             (QChar (unmatched.at (closeIndex + 1)) == '*'))) &&
                            matchesSpec (wordUpper, spec) &&
                            !wordSet.count (wordUpper))
                        {
                            wordSet.insert (make_pair (wordUpper,
                                reversePattern ? reverseString (word)
                                               : word));
                        }
                    }

                    // Special processing for Anagram or Subanagram match
                    else if
                        ((condition.type == SearchCondition::AnagramMatch) ||
                         (condition.type == SearchCondition::SubanagramMatch))
                    {
                        // Find the current letter in the pattern.  First,
                        // prefer to match the letter itself.  Second, prefer
                        // to match the letter as part of a character class.
                        // If the letter matches more than one character
                        // class, match the first one and push traversal
                        // states for each of the others that is matched.
                        // Character classes are guaranteed to be at the end
                        // of the search string, so once you're in a character
                        // class, you're always in a character class.
                        int len = unmatched.length();
                        bool inGroup = false;
                        bool found = false;
                        bool negated = false;
                        int matchStart = -1;
                        int matchEnd = -1;
                        int groupStart = -1;
                        bool wildcardMatch = false;
                        for (int i = 0; i < len; ++i) {
                            QChar c = unmatched.at (i);

                            if (c == '[') {
                                inGroup = true;
                                negated = false;
                                groupStart = i;
                            }

                            else if (inGroup) {
                                if (c == '^')
                                    negated = true;

                                else if (c == ']') {
                                    if (found ^ negated) {
                                        if (matchEnd < 0) {
                                            matchStart = groupStart;
                                            matchEnd = i;
                                            wildcardMatch = true;
                                        }

                                        else if (node->child) {
                                            states.push ( TraversalState
                                                (node->child, word +
                                                 node->letter,
                                                 unmatched.left (groupStart) +
                                                 unmatched.right
                                                 (unmatched.length() - i -
                                                  1)));
                                        }
                                    }
                                    inGroup = false;
                                    found = false;
                                    negated = false;
                                }

                                else if (c == node->letter)
                                    found = true;
                            }

                            // Matched the character itself
                            else if (c == node->letter) {
                                found = true;
                                matchStart = i;
                                matchEnd = i;
                                break;
                            }
                        }

                        // Try to match the current letter against the
                        // pattern.  If the letter doesn't match exactly,
                        // match a ? char.
                        //int index = unmatched.find (node->letter);
                        found = (matchStart >= 0);
                        if (!found) {
                            matchStart = matchEnd = unmatched.indexOf ("?");
                            found = (matchStart >= 0);
                            wildcardMatch = true;
                        }

                        // If this letter matched or a wildcard was specified,
                        // keep traversing after possibly adding the current
                        // word.
                        if (found || wildcard) {
                            word += (found && !wildcardMatch) ? node->letter
                                : node->letter.toLower();

                            if (found)
                                unmatched.replace (matchStart,
                                                   matchEnd - matchStart + 1,
                                                   "");

                            if (node->child &&
                                (wildcard || !unmatched.isEmpty()))
                            {
                                states.push (TraversalState (node->child,
                                                             word,
                                                             unmatched));
                            }

                            QString wordUpper = word.toUpper();
                            if (node->eow &&
                                ((condition.type ==
                                  SearchCondition::SubanagramMatch) ||
                                  unmatched.isEmpty()) &&
                                  matchesSpec (wordUpper, spec) &&
                                  !wordSet.count (wordUpper))
                            {
                                wordSet.insert (make_pair (wordUpper, word));
                            }
                        }
                    }
                }
            }

            // Done traversing next nodes, pop a child off the stack
            node = 0;
            if (states.size()) {
                TraversalState state = states.top();
                node = state.node;
                unmatched = state.unmatched;
                word = state.word;
                states.pop();
            }
        }

        // Take conjunction or disjunction with final result set
        if (!conditionNum) {
            finalWordSet = wordSet;
        }

        else if (spec.conjunction) {
            map<QString, QString> conjunctionSet;
            for (sit = wordSet.begin(); sit != wordSet.end(); ++sit) {
                map<QString, QString>::iterator found =
                    finalWordSet.find (sit->first);
                if (found != finalWordSet.end())
                    conjunctionSet.insert (*found);
            }
            if (conjunctionSet.empty())
                return wordList;
            finalWordSet = conjunctionSet;
        }

        else {
            for (sit = wordSet.begin(); sit != wordSet.end(); ++sit) {
                finalWordSet.insert (*sit);
            }
        }

        ++conditionNum;
    }

    // Transform word set into word list and return it
    for (sit = finalWordSet.begin(); sit != finalWordSet.end(); ++sit) {
        wordList << (wildcardLower ? sit->second : sit->first);
    }

    return wordList;
}

//---------------------------------------------------------------------------
//  compress
//
//! Compress the graph by eliminating duplication of common suffixes.
//---------------------------------------------------------------------------
void
WordGraph::compress()
{
}

//---------------------------------------------------------------------------
//  print
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
        cout << node->letter.toAscii();
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
//  matchesSpec
//
//! Determine whether a word matches a search specification.  Only the
//! following attributes are checked: Include Letters, Consist Letters/Pct,
//! Min Length, Min Anagrams, Max Anagrams, Min Probability, Max Probability.
//! All other attributes are assumed to have been checked in the course of
//! finding the word to be checked.
//---------------------------------------------------------------------------
bool
WordGraph::matchesSpec (QString word, const SearchSpec& spec) const
{
    QListIterator<SearchCondition> it (spec.conditions);
    while (it.hasNext()) {
        const SearchCondition& condition = it.next();

        switch (condition.type) {
            case SearchCondition::Length:
            if ((int (word.length()) < condition.minValue) ||
                (int (word.length()) > condition.maxValue))
                return false;
            break;

            case SearchCondition::IncludeLetters: {
                QString tmpWord = word;
                int includeLen = condition.stringValue.length();
                for (int i = 0; i < includeLen; ++i) {
                    int index = tmpWord.indexOf (condition.stringValue.at (i));
                    if ((index < 0) ^ condition.negated)
                        return false;
                    tmpWord.replace (index, 1, "");
                }
            }
            break;

            case SearchCondition::ConsistOf:
            if ((condition.minValue > 0) || (condition.maxValue < 100)) {
                int wordLen = word.length();
                int consist = 0;
                for (int i = 0; i < wordLen; ++i) {
                    if (condition.stringValue.contains (word.at (i)))
                        ++consist;
                }
                int consistPct = (consist * 100) / wordLen;
                if ((consistPct < condition.minValue) ||
                    (consistPct > condition.maxValue))
                    return false;
            }
            break;

            // XXX: Implement these!
            //case SearchCondition::NumAnagrams:
            //case SearchCondition::Probability:

            default: break;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
//  addWord
//
//! Add a word at a location in the graph.
//
//! @param w the word to add
//! @param reverse true if the word should be reversed and added to the
//! reverse list
//---------------------------------------------------------------------------
void
WordGraph::addWord (const QString& w, bool reverse)
{
    QString word = (reverse ? reverseString (w) : w);
    Node* node = (reverse ? rtop : top);
    Node* parentNode = 0;
    QChar c;
    int wordLength = word.length();
    for (int i = 0; i < wordLength; ++i) {
        c = word.at (i);

        // Empty node, so create a new node and link from its parent
        if (!node) {
            node = new Node (c.toAscii());
            (parentNode ? parentNode->child : (reverse ? rtop : top)) = node;
            ++numNodes;
        }

        // Nonempty node, so find the current letter in the chain
        else {
            while (node->letter != c) {
                if (!node->next) {
                    node->next = new Node (c.toAscii());
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
//  reverseString
//
//! Reverse the characters of a string or pattern.  Character classes, e.g.
//! "[ABCDE]" are treated as single characters and left intact.
//
//! @param s the string to reverse
//! @return the reversed string
//---------------------------------------------------------------------------
QString
WordGraph::reverseString (const QString& s) const
{
    QString reverse;
    QString tmp;
    bool inCharClass = false;
    for (int i = 0; i < s.length(); ++i) {
        QChar c = s[i];

        if (c == ']') {
            reverse.prepend (c);
            reverse.prepend (tmp);
            tmp.clear();
            inCharClass = false;
        }

        else if (c == '[') {
            tmp.append (c);
            inCharClass = true;
        }

        else if (inCharClass) {
            tmp.append (c);
        }

        else {
            reverse.prepend (c);
        }
    }

    if (inCharClass)
        reverse.prepend (tmp);

    return reverse;
}

//---------------------------------------------------------------------------
//  Node
//
//! Constructor.
//---------------------------------------------------------------------------
WordGraph::Node::Node(char c, bool e)
    : letter (c), eow (e), next (0), child (0)
{
}

//---------------------------------------------------------------------------
//  ~Node
//
//! Destructor.  Destroy this node along with its next and child nodes.
//---------------------------------------------------------------------------
WordGraph::Node::~Node()
{
    delete next;
    delete child;
}

//---------------------------------------------------------------------------
//  operator==
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
