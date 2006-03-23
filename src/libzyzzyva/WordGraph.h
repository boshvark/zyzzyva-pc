//---------------------------------------------------------------------------
// WordGraph.h
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

#ifndef ZYZZYVA_WORD_GRAPH_H
#define ZYZZYVA_WORD_GRAPH_H

#include "SearchSpec.h"
#include <QFile>
#include <QString>
#include <QStringList>

class WordGraph
{
    public:
    WordGraph();
    ~WordGraph();

    void clear();
    bool importDawgFile (const QString& filename, bool reverse, QString*
                         errString, quint16* expectedChecksum);
    void addWord (const QString& w);
    bool containsWord (const QString& w) const;
    QStringList search (const SearchSpec& spec) const;

    private:
    class Node {
      public:
        Node (char c = 0, bool e = false);
        ~Node();
        bool operator== (const Node& rhs);
        QChar letter;
        bool eow;
        Node* next;
        Node* child;
    };

    class TraversalState {
      public:
        TraversalState (long n, const QString& w, const QString& u)
            : node (n), word (w), unmatched (u) { }
        long node;
        QString word;
        QString unmatched;
    };

    class TraversalStateOld {
      public:
        TraversalStateOld (Node* n, const QString& w, const QString& u)
            : node (n), word (w), unmatched (u) { }
        Node* node;
        QString word;
        QString unmatched;
    };

    private:
    bool matchesSpec (QString word, const SearchSpec& spec) const;
    QString reverseString (const QString& s) const;
    long readData (long* data, long count, QFile& file);

    void addWordOld (const QString& w, bool reverse);
    bool containsWordOld (const QString& w) const;
    QStringList searchOld (const SearchSpec& spec) const;

    long* dawg;
    long* rdawg;

    bool bigEndian;

    // OLD dawg structures - only used where new DAWG is unavailable
    Node* top;
    Node* rtop;
};

#endif // ZYZZYVA_WORD_GRAPH_H
