//---------------------------------------------------------------------------
// WordEngine.h
//
// A class to handle the loading and searching of words.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_WORD_ENGINE_H
#define ZYZZYVA_WORD_ENGINE_H

#include "WordGraph.h"
#include <QMap>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <map>
#include <set>

class WordEngine : public QObject
{
    Q_OBJECT
    public:
    WordEngine (QObject* parent = 0)
        : QObject (parent) { }
    ~WordEngine() { }

    void clearLexicon();
    bool connectToDatabase (const QString& filename, QString* errString = 0);
    bool disconnectFromDatabase();
    int importTextFile (const QString& filename, const QString& lexName, bool
                        loadDefinitions = true, QString* errString = 0);
    bool importDawgFile (const QString& filename, const QString& lexName, bool
                         reverse = false, QString* errString = 0, quint16*
                         expectedChecksum = 0);
    int importStems (const QString& filename, QString* errString = 0);
    bool isAcceptable (const QString& word) const;
    QStringList search (const SearchSpec& spec, bool allCaps) const;
    QStringList alphagrams (const QStringList& list) const;
    int getNumWords() const;
    QString getDefinition (const QString& word) const;
    QString getLexiconName() const { return lexiconName; }
    QString getFrontHookLetters (const QString& word) const;
    QString getBackHookLetters (const QString& word) const;
    int getProbabilityOrder (const QString& word) const;
    int getMinProbabilityOrder (const QString& word) const;
    int getMaxProbabilityOrder (const QString& word) const;
    int getNumVowels (const QString& word) const;
    int getNumUniqueLetters (const QString& word) const;
    int getPointValue (const QString& word) const;

    private:
    bool matchesConditions (const QString& word, const QList<SearchCondition>&
                            conditions) const;
    bool isSetMember (const QString& word, SearchSet ss) const;
    int getNumAnagrams (const QString& word) const;
    QStringList nonGraphSearch (const SearchSpec& spec) const;
    void addDefinition (const QString& word, const QString& definition);
    QString replaceDefinitionLinks (const QString& definition, int maxDepth,
                                    bool useFollow = false) const;
    QString getSubDefinition (const QString& word, const QString& pos) const;
    QString getNewInOwl2String() const;
    QStringList databaseSearch (const SearchSpec& optimizedSpec, const
                                QStringList* wordList = 0) const;
    QStringList applyPostConditions (const SearchSpec& optimizedSpec, const
                                     QStringList& wordList) const;

    QString lexiconName;
    WordGraph graph;
    std::map<QString, std::multimap<QString, QString> > definitions;
    std::map<int, QStringList> stems;
    QMap<QString, int> numAnagramsMap;
    std::map< int, std::set<QString> > stemAlphagrams;

    QSqlDatabase db;
    QString dbConnectionName;
};

#endif // ZYZZYVA_WORD_ENGINE_H
