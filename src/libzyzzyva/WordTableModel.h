//---------------------------------------------------------------------------
// WordTableModel.h
//
// A model for representing word lists.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_WORD_TABLE_MODEL_H
#define ZYZZYVA_WORD_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QChar>
#include <QStringList>

class WordEngine;

class WordTableModel : public QAbstractTableModel
{
    public:
    enum WordType {
        WordNormal = 0,
        WordNormalAlternate,
        WordCorrect,
        WordMissed,
        WordIncorrect,
        WordLastAdded
    };

    enum {
        WordTypeRole = Qt::UserRole,
        PlayabilityValueRole = Qt::UserRole + 1
    };

    class WordItem {
        public:
        WordItem() : type(WordNormal) { init(); }
        WordItem(const QString& w, WordType t = WordNormal,
                 const QString& wc = QString())
            : word(w), type(t), wildcard(wc) { init(); }
        void init();
        QString getWord() const { return word; }
        WordType getType() const { return type; }
        QString getWildcard() const { return wildcard; }
        QString getFrontHooks() const { return frontHooks; }
        QString getBackHooks() const { return backHooks; }
        bool getFrontParentHook() const { return frontParentHook; }
        bool getBackParentHook() const { return backParentHook; }
        int getProbabilityOrder() const { return probabilityOrder; }
        qint64 getPlayabilityValue() const { return playabilityValue; }
        int getPlayabilityOrder() const { return playabilityOrder; }
        QString getLexiconSymbols() const { return lexiconSymbols; }
        void setWord(const QString& w) { word = w; }
        void setType(WordType t) { type = t; }
        void setWildcard(const QString& w) { wildcard = w; }
        void setProbabilityOrder(int p);
        void setPlayabilityValue(qint64 p);
        void setPlayabilityOrder(int p);
        void setLexiconSymbols(const QString& s);

        void setHooks(const QString& front, const QString& back);
        void setParentHooks(bool front, bool back);
        bool hooksAreValid() const { return hooksValid; }
        bool parentHooksAreValid() const { return parentHooksValid; }
        bool probabilityOrderIsValid() const { return probabilityOrderValid; }
        bool playabilityOrderIsValid() const { return playabilityOrderValid; }
        bool lexiconSymbolsAreValid() const { return lexiconSymbolsValid; }

        bool operator==(const WordItem& other) const {
            return ((word == other.word) && (type == other.type));
        }
        bool operator< (const WordItem& other) const {
            return word < other.word;
        }

        private:
        bool hooksValid;
        bool parentHooksValid;
        bool probabilityOrderValid;
        bool playabilityOrderValid;
        bool lexiconSymbolsValid;
        QString word;
        WordType type;
        int probabilityOrder;
        qint64 playabilityValue;
        int playabilityOrder;
        QString wildcard;
        QString frontHooks;
        QString backHooks;
        bool frontParentHook;
        bool backParentHook;
        QString lexiconSymbols;
    };

    Q_OBJECT
    public:
    WordTableModel(WordEngine* e, QObject* parent = 0);
    ~WordTableModel();

    bool clear();
    bool addWord(const WordItem& word, bool updateLastAdded = true);
    bool addWords(const QList<WordItem>& words);
    bool removeWord(const QString& word);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role =
                        Qt::DisplayRole) const;
    bool insertRows(int row, int count, const QModelIndex& parent =
                    QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent =
                    QModelIndex());
    bool setData(const QModelIndex& index, const QVariant& value, int role =
                 Qt::EditRole);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    void reverse();
    QString getLexicon() const { return lexicon; }
    void setLexicon(const QString& lex) { lexicon = lex; }
    int getLastAddedIndex() const { return lastAddedIndex; }
    void setProbabilityNumBlanks(int numBlanks) { probNumBlanks = numBlanks; }
    int getProbabilityNumBlanks() const { return probNumBlanks; }
    void clearLastAddedIndex();

    signals:
    void wordsChanged();

    private:
    void addWordPrivate(const WordItem& word, int row);
    void markAlternates();

    private:
    WordEngine* wordEngine;
    QString lexicon;
    mutable QList<WordItem> wordList;
    int probNumBlanks;
    int lastAddedIndex;

    public:
    enum {
        WILDCARD_MATCH_COLUMN = 0,
        PROBABILITY_ORDER_COLUMN = 1,
        PLAYABILITY_ORDER_COLUMN = 2,
        FRONT_HOOK_COLUMN = 3,
        WORD_COLUMN = 4,
        BACK_HOOK_COLUMN = 5,
        DEFINITION_COLUMN = 6,
        NUM_COLUMNS = 7
    };

    static const QChar PARENT_HOOK_CHAR;
    static const int ITEM_XPADDING;
    static const int ITEM_YPADDING;
};

#endif // ZYZZYVA_WORD_TABLE_MODEL_H

