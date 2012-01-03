//---------------------------------------------------------------------------
// WordEngine.h
//
// A class to handle the loading and searching of words.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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
#include <QMultiMap>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <stdint.h>

class WordEngine : public QObject
{
    Q_OBJECT
    public:
    class ValueOrder {
        public:
        ValueOrder() : valueOrder(0), minValueOrder(0), maxValueOrder(0) { }
        public:
        int valueOrder;
        int minValueOrder;
        int maxValueOrder;
    };

    class WordInfo {
        public:
        WordInfo() : numVowels(0), numUniqueLetters(0), numAnagrams(0),
            pointValue(0) { }
        ~WordInfo() { }

        bool isValid() const { return !word.isEmpty(); }

        public:
        QString word;
        int numVowels;
        int numUniqueLetters;
        int numAnagrams;
        int pointValue;
        QString frontHooks;
        QString backHooks;
        bool isFrontHook;
        bool isBackHook;
        QString lexiconSymbols;
        QString definition;
        qint64 playability;
        ValueOrder playabilityOrder;
        QMap<int, ValueOrder> blankProbabilityOrder;
    };

    class LexiconData {
        public:
        LexiconData() : graph(0), db(0) { }

        public:
        QString name;
        QString lexiconFile;
        QMap<QString, QMultiMap<QString, QString> > definitions;
        QMap<int, QStringList> stems;
        QMap<QString, int> numAnagramsMap;
        QMap<QString, qint64> playabilityMap;
        QMap<int, QSet<QString> > stemAlphagrams;
        mutable QMap<QString, WordInfo> wordCache;
        WordGraph* graph;
        QSqlDatabase* db;
        QString dbConnectionName;
    };

    public:
    WordEngine(QObject* parent = 0)
        : QObject(parent) { }
    ~WordEngine() { }

    bool connectToDatabase(const QString& lexicon, const QString& filename,
                           QString* errString = 0);
    bool disconnectFromDatabase(const QString& lexicon);
    bool databaseIsConnected(const QString& lexicon) const;
    int importTextFile(const QString& lexicon, const QString& filename, bool
                       loadDefinitions = true, QString* errString = 0);
    bool importDawgFile(const QString& lexicon, const QString& filename, bool
                        reverse = false, QString* errString = 0, quint16*
                        expectedChecksum = 0);
    int importStems(const QString& lexicon, const QString& filename,
                    QString* errString = 0);
    bool lexiconIsLoaded(const QString& lexicon) const;
    bool isAcceptable(const QString& lexicon, const QString& word) const;
    QStringList search(const QString& lexicon, const SearchSpec& spec,
                       bool allCaps) const;
    QStringList wordGraphSearch(const QString& lexicon, const SearchSpec&
                                spec) const;
    QStringList alphagrams(const QStringList& strList) const;
    int getNumWords(const QString& lexicon) const;
    QString getLexiconFile(const QString& lexicon) const;
    WordInfo getWordInfo(const QString& lexicon, const QString& word) const;
    QString getDefinition(const QString& lexicon, const QString& word,
                          bool replaceLinks = true) const;
    QString getFrontHookLetters(const QString& lexicon, const QString& word)
        const;
    QString getBackHookLetters(const QString& lexicon, const QString& word)
        const;
    qint64 getPlayabilityValue(const QString& lexicon, const QString& word)
        const;
    int getPlayabilityOrder(const QString& lexicon, const QString& word)
        const;
    int getMinPlayabilityOrder(const QString& lexicon, const QString& word)
        const;
    int getMaxPlayabilityOrder(const QString& lexicon, const QString& word)
        const;
    int getProbabilityOrder(const QString& lexicon, const QString& word,
                            int numBlanks) const;
    int getMinProbabilityOrder(const QString& lexicon, const QString& word,
                               int numBlanks) const;
    int getMaxProbabilityOrder(const QString& lexicon, const QString& word,
                               int numBlanks) const;
    // XXX: Lexicon parameter doesn't really make sense here, but it is
    // retained to use word info caching
    int getNumVowels(const QString& lexicon, const QString& word) const;
    int getNumUniqueLetters(const QString& lexicon, const QString& word) const;
    int getPointValue(const QString& lexicon, const QString& word) const;
    bool getIsFrontHook(const QString& lexicon, const QString& word) const;
    bool getIsBackHook(const QString& lexicon, const QString& word) const;
    QString getLexiconSymbols(const QString& lexicon, const QString& word) const;

    void addToCache(const QString& lexicon, const QStringList& words) const;

    private:
    enum ConditionPhase {
        UnknownPhase = 0,
        WordGraphPhase,
        DatabasePhase,
        PostConditionPhase
    };

    private:
    void clearCache(const QString& lexicon) const;
    bool matchesPostConditions(const QString& lexicon, const QString& word,
                               const QList<SearchCondition>& conditions) const;
    bool isSetMember(const QString& lexicon, const QString& word,
                     SearchSet ss) const;
    int getNumAnagrams(const QString& lexicon, const QString& word) const;
    QStringList nonGraphSearch(const QString& lexicon,
                               const SearchSpec& spec) const;
    void addDefinition(const QString& lexicon, const QString& word,
                       const QString& definition);
    QStringList databaseSearch(const QString& lexicon, const SearchSpec&
                               optimizedSpec, const QStringList* wordList = 0)
                               const;
    QStringList applyPostConditions(const QString& lexicon, const SearchSpec&
                                    optimizedSpec, const QStringList&
                                    wordList) const;
    ConditionPhase getConditionPhase(const SearchCondition& condition) const;

    private:
    QMap<QString, LexiconData*> lexiconData;
};

#endif // ZYZZYVA_WORD_ENGINE_H
