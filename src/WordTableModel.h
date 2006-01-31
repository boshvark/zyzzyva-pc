//---------------------------------------------------------------------------
// WordTableModel.h
//
// A model for representing word lists.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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
        WordCorrect,
        WordMissed,
        WordIncorrect,
        WordLastAdded
    };

    class WordItem {
        public:
        WordItem (const QString& w, WordType t) : word (w), type (t) { }
        QString getWord() const { return word; }
        WordType getType() const { return type; }
        void setWord (const QString& w) { word = w; }
        void setType (WordType t) { type = t; }

        bool operator== (const WordItem& other) const {
            return ((word == other.word) && (type == other.type));
        }
        bool operator< (const WordItem& other) const {
            return word < other.word;
        }

        private:
        QString word;
        WordType type;
        QString wildcard;
    };

    Q_OBJECT
    public:
    WordTableModel (WordEngine* e, QObject* parent = 0);
    ~WordTableModel();

    bool clear();
    bool addWord (const QString& word, WordType type = WordNormal, bool
                  updateLastAdded = true);
    bool addWords (const QStringList& words, WordType type = WordNormal);

    int rowCount (const QModelIndex& parent = QModelIndex()) const;
    int columnCount (const QModelIndex& parent = QModelIndex()) const;
    QVariant data (const QModelIndex& index, int role = Qt::DisplayRole)
        const;
    QVariant headerData (int section, Qt::Orientation orientation, int role =
                         Qt::DisplayRole) const;
    bool insertRows (int row, int count, const QModelIndex& parent =
                     QModelIndex());
    bool removeRows (int row, int count, const QModelIndex& parent =
                     QModelIndex());
    bool setData (const QModelIndex& index, const QVariant& value, int role =
                  Qt::EditRole);
    void sort (int column, Qt::SortOrder order = Qt::AscendingOrder);
    int getLastAddedIndex() const { return lastAddedIndex; }
    void clearLastAddedIndex();

    signals:
    void wordsChanged();

    private:
    bool isFrontHook (const QString& word) const;
    bool isBackHook (const QString& word) const;
    QString getFrontHookLetters (const QString& word) const;
    QString getBackHookLetters (const QString& word) const;

    private:
    WordEngine* wordEngine;
    QList<WordItem> wordList;
    int lastAddedIndex;

    public:
    enum {
        WILDCARD_MATCH_COLUMN = 0,
        FRONT_HOOK_COLUMN = 1,
        WORD_COLUMN = 2,
        BACK_HOOK_COLUMN = 3,
        DEFINITION_COLUMN = 4
    };

    static const QChar PARENT_HOOK_CHAR;
};

#endif // ZYZZYVA_WORD_TABLE_MODEL_H

