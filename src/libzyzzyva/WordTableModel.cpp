//---------------------------------------------------------------------------
// WordTableModel.cpp
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

#include "WordTableModel.h"
#include "WordEngine.h"
#include "MainSettings.h"
#include "Auxil.h"
#include <QBrush>

using namespace std;

const QChar WordTableModel::PARENT_HOOK_CHAR = QChar(0x00b7);
const QChar PARENT_HOOK_HYPHEN_CHAR = '-';
const QString WILDCARD_MATCH_HEADER = "?";
const QString PROBABILITY_ORDER_HEADER = "Prob-%1";
const QString PLAYABILITY_ORDER_HEADER = "Play";
const QString FRONT_HOOK_HEADER = "<";
const QString WORD_HEADER = "Word";
const QString BACK_HOOK_HEADER = ">";
const QString DEFINITION_HEADER = "Definition";
const int ITEM_MARGIN = 5;
const int DEFAULT_COLUMN_WIDTH = 100;

const QColor VALID_NORMAL_WORD_FOREGROUND = Qt::black;
const QColor VALID_NORMAL_WORD_BACKGROUND = Qt::white;
const QColor VALID_NORMAL_ALTERNATE_FOREGROUND = Qt::black;
const QColor VALID_NORMAL_ALTERNATE_BACKGROUND = Qt::lightGray;
const QColor VALID_CORRECT_WORD_FOREGROUND = Qt::black;
const QColor VALID_CORRECT_WORD_BACKGROUND = Qt::white;
const QColor VALID_MISSED_WORD_FOREGROUND = Qt::black;
const QColor VALID_MISSED_WORD_BACKGROUND = Qt::cyan;
const QColor LAST_ADDED_WORD_FOREGROUND = Qt::black;
const QColor LAST_ADDED_WORD_BACKGROUND = Qt::green;
const QColor INVALID_WORD_FOREGROUND = Qt::black;
const QColor INVALID_WORD_BACKGROUND = Qt::red;

//---------------------------------------------------------------------------
//  lessThan
//
//! A comparison function that compares by lexical value.
//
//! @param a the first word item to compare
//! @param b the second word item to compare
//! @return true if a is lexically less than b
//---------------------------------------------------------------------------
bool
lessThan(const WordTableModel::WordItem& a,
         const WordTableModel::WordItem& b)
{
    if (MainSettings::getWordListSortByPlayabilityOrder()) {
        qint64 aPlayValue = a.getPlayabilityValue();
        qint64 bPlayValue = b.getPlayabilityValue();

        // High playability values compare as less
        if (bPlayValue < aPlayValue)
            return true;
        else if (aPlayValue < bPlayValue)
            return false;

        int aPlayOrder = a.getPlayabilityOrder();
        int bPlayOrder = b.getPlayabilityOrder();
        if (aPlayOrder < bPlayOrder)
            return true;
        else if (bPlayOrder < aPlayOrder)
            return false;
    }

    if (MainSettings::getWordListSortByLength()) {
        int aLen = a.getWord().length();
        int bLen = b.getWord().length();
        bool reverse = MainSettings::getWordListSortByReverseLength();
        if ((!reverse && (aLen < bLen)) || (reverse && (aLen > bLen)))
            return true;
        else if ((!reverse && (bLen < aLen)) || (reverse && (bLen > aLen)))
            return false;
    }

    if (MainSettings::getWordListGroupByAnagrams()) {
        QString aa = Auxil::getAlphagram(a.getWord().toUpper());
        QString ab = Auxil::getAlphagram(b.getWord().toUpper());
        int compare = QString::localeAwareCompare(aa, ab);
        if (compare < 0)
            return true;
        else if (compare > 0)
            return false;
    }

    if (MainSettings::getWordListSortByProbabilityOrder()) {
        int aProbOrder = a.getProbabilityOrder();
        int bProbOrder = b.getProbabilityOrder();
        if (aProbOrder < bProbOrder)
            return true;
        else if (bProbOrder < aProbOrder)
            return false;
    }

    return (QString::localeAwareCompare(a.getWord().toUpper(),
                                        b.getWord().toUpper()) < 0);
}

//---------------------------------------------------------------------------
//  WordTableModel
//
//! Constructor.
//
//! @param e the word engine
//! @param parent the parent object
//---------------------------------------------------------------------------
WordTableModel::WordTableModel(WordEngine* e, QObject* parent)
    : QAbstractTableModel(parent), wordEngine(e), probNumBlanks(0),
      lastAddedIndex(-1)
{
    probNumBlanks = MainSettings::getProbabilityNumBlanks();
}

//---------------------------------------------------------------------------
//  ~WordTableModel
//
//! Destructor.
//---------------------------------------------------------------------------
WordTableModel::~WordTableModel()
{
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the word list.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::clear()
{
    bool ok = removeRows(0, rowCount());
    emit wordsChanged();
    return ok;
}

//---------------------------------------------------------------------------
//  addWord
//
//! Add a word to the model.
//
//! @param word the word item to add
//! @param updateLastAdded whether to update the last added index
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::addWord(const WordItem& word, bool updateLastAdded)
{
    int row = rowCount();
    bool ok = insertRow(row);
    if (!ok)
        return false;

    addWordPrivate(word, row);

    sort(WORD_COLUMN);
    lastAddedIndex = updateLastAdded ? wordList.indexOf(word) : -1;
    emit wordsChanged();
    return true;
}

//---------------------------------------------------------------------------
//  addWords
//
//! Add a list of words to the model.
//
//! @param words the word items to add
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::addWords(const QList<WordItem>& words)
{
    int row = rowCount();
    bool ok = insertRows(row, words.size());
    if (!ok)
        return false;
    foreach (const WordItem& word, words) {
        addWordPrivate(word, row);
        ++row;
    }
    sort(WORD_COLUMN);
    lastAddedIndex = -1;
    emit wordsChanged();
    return true;
}

//---------------------------------------------------------------------------
//  removeWord
//
//! Add a word to the model.
//
//! @param word the word to remove
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::removeWord(const QString& word)
{
    int count = 0;
    int start = 0;
    for (int i = 0; i < rowCount(); ++i) {
        const WordItem& item = wordList.at(i);
        if (item.getWord() == word) {
            if (!count)
                start = i;
            ++count;

            if (i < lastAddedIndex)
                --lastAddedIndex;
            else if (i == lastAddedIndex)
                lastAddedIndex = -1;
        }
        else if (count) {
            bool ok = removeRows(start, count);
            emit wordsChanged();
            return ok;
        }
    }

    if (count) {
        bool ok = removeRows(start, count);
        emit wordsChanged();
        return ok;
    }

    return false;
}

//---------------------------------------------------------------------------
//  rowCount
//
//! Return the number of rows under the given parent.  Reimplemented from
//! QAbstractTableModel.
//
//! @param parent the parent index
//! @return the number of rows
//---------------------------------------------------------------------------
int
WordTableModel::rowCount(const QModelIndex&) const
{
    return wordList.count();
}

//---------------------------------------------------------------------------
//  columnCount
//
//! Return the number of columns for the given parent.  Reimplemented from
//! QAbstractTableModel.
//
//! @param parent the parent index
//! @return the number of columns
//---------------------------------------------------------------------------
int
WordTableModel::columnCount(const QModelIndex&) const
{
    return NUM_COLUMNS;
}

//---------------------------------------------------------------------------
//  data
//
//! Return the data stored under the given role for the item referred to by
//! the index.  Reimplemented from QAbstractTableModel.
//
//! @param index the index
//! @param role the display role
//! @return the appropriate header data, or an empty object if invalid
//---------------------------------------------------------------------------
QVariant
WordTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((index.row() < 0) || (index.row() >= wordList.count()))
        return QVariant();

    WordItem& wordItem = wordList[index.row()];
    WordType type = (lastAddedIndex == index.row()) ? WordLastAdded
        : wordItem.getType();

    switch (role) {
        case WordTypeRole:
        return type;

        case PlayabilityValueRole:
        if (!wordItem.playabilityOrderIsValid()) {
            QString wordUpper = wordItem.getWord().toUpper();
            qint64 pv = wordEngine->getPlayabilityValue(lexicon, wordUpper);
            if (pv)
                wordItem.setPlayabilityValue(pv);
            int po = wordEngine->getPlayabilityOrder(lexicon, wordUpper);
            if (po)
                wordItem.setPlayabilityOrder(po);
        }
        return wordItem.getPlayabilityValue();

        case Qt::ForegroundRole: {
            QColor color;
            switch (type) {
                case WordTableModel::WordNormal:
                color = VALID_NORMAL_WORD_FOREGROUND;
                break;
                case WordTableModel::WordNormalAlternate:
                color = VALID_NORMAL_ALTERNATE_FOREGROUND;
                break;
                case WordTableModel::WordCorrect:
                color = VALID_CORRECT_WORD_FOREGROUND;
                break;
                case WordTableModel::WordMissed:
                color = VALID_MISSED_WORD_FOREGROUND;
                break;
                case WordTableModel::WordIncorrect:
                color = INVALID_WORD_FOREGROUND;
                break;
                case WordTableModel::WordLastAdded:
                color = LAST_ADDED_WORD_FOREGROUND;
                break;
                default: break;
            }
            return QBrush(color);
        }

        case Qt::BackgroundRole: {
            QColor color;
            switch (type) {
                case WordTableModel::WordNormal:
                color = VALID_NORMAL_WORD_BACKGROUND;
                break;
                case WordTableModel::WordNormalAlternate:
                color = VALID_NORMAL_ALTERNATE_BACKGROUND;
                break;
                case WordTableModel::WordCorrect:
                color = VALID_CORRECT_WORD_BACKGROUND;
                break;
                case WordTableModel::WordMissed:
                color = VALID_MISSED_WORD_BACKGROUND;
                break;
                case WordTableModel::WordIncorrect:
                color = INVALID_WORD_BACKGROUND;
                break;
                case WordTableModel::WordLastAdded:
                color = LAST_ADDED_WORD_BACKGROUND;
                break;
                default: break;
            }
            return QBrush(color);
        }

        case Qt::TextAlignmentRole: {
            int flags = Qt::AlignVCenter;
            switch (index.column()) {
                case WordTableModel::FRONT_HOOK_COLUMN:
                case WordTableModel::PROBABILITY_ORDER_COLUMN:
                case WordTableModel::PLAYABILITY_ORDER_COLUMN:
                flags |= Qt::AlignRight;
                break;

                default:
                flags |= Qt::AlignLeft;
                break;
            }
            return flags;
        }

        case Qt::DisplayRole:
        case Qt::EditRole: {
            QString word = wordItem.getWord();
            if (word.isEmpty())
                return QString();

            QString wordUpper = word.toUpper();
            switch (index.column()) {
                case WILDCARD_MATCH_COLUMN:
                return wordItem.getWildcard();

                case PROBABILITY_ORDER_COLUMN: {
                    if (!MainSettings::getWordListShowProbabilityOrder()) {
                        return QString();
                    }

                    if (!wordItem.probabilityOrderIsValid()) {
                        int p = wordEngine->getProbabilityOrder(
                            lexicon, wordUpper, probNumBlanks);
                        if (p)
                            wordItem.setProbabilityOrder(p);
                    }

                    int probOrder = wordItem.getProbabilityOrder();
                    return (probOrder ? QString::number(probOrder) : QString());
                }

                case PLAYABILITY_ORDER_COLUMN: {
                    if (!MainSettings::getWordListShowPlayabilityOrder()) {
                        return QString();
                    }

                    if (!wordItem.playabilityOrderIsValid()) {
                        qint64 pv = wordEngine->getPlayabilityValue(
                            lexicon, wordUpper);
                        if (pv)
                            wordItem.setPlayabilityValue(pv);
                        int po = wordEngine->getPlayabilityOrder(
                            lexicon, wordUpper);
                        if (po)
                            wordItem.setPlayabilityOrder(po);
                    }

                    int playOrder = wordItem.getPlayabilityOrder();
                    return (playOrder ? QString::number(playOrder) : QString());
                }

                case FRONT_HOOK_COLUMN:
                if (!MainSettings::getWordListShowHooks()) {
                    return QString();
                }
                else if (!wordItem.hooksAreValid()) {
                    wordItem.setHooks(
                        wordEngine->getFrontHookLetters(lexicon, wordUpper),
                        wordEngine->getBackHookLetters(lexicon, wordUpper));
                }
                return wordItem.getFrontHooks();

                case BACK_HOOK_COLUMN:
                if (!MainSettings::getWordListShowHooks()) {
                    return QString();
                }
                else if (!wordItem.hooksAreValid()) {
                    wordItem.setHooks(
                        wordEngine->getFrontHookLetters(lexicon, wordUpper),
                        wordEngine->getBackHookLetters(lexicon, wordUpper));
                }
                return wordItem.getBackHooks();

                case WORD_COLUMN:
                if (role == Qt::EditRole) {
                    return wordUpper;
                }
                else if (role == Qt::DisplayRole) {
                    QString str (word);
                    if (MainSettings::getWordListShowHookParents()) {
                        if (!wordItem.parentHooksAreValid()) {
                            wordItem.setParentHooks(
                                wordEngine->getIsFrontHook(lexicon, wordUpper),
                                wordEngine->getIsBackHook(lexicon, wordUpper));
                        }
                        QChar hookChar =
                            (MainSettings::getWordListUseHookParentHyphens() ?
                                PARENT_HOOK_HYPHEN_CHAR : PARENT_HOOK_CHAR);
                        str = (wordItem.getFrontParentHook() ? hookChar
                               : QChar(' '))
                            + str
                            + (wordItem.getBackParentHook() ? hookChar
                               : QChar(' '));
                    }
                    if (MainSettings::getWordListUseLexiconStyles()) {
                        if (!wordItem.lexiconSymbolsAreValid()) {
                            wordItem.setLexiconSymbols(
                                wordEngine->getLexiconSymbols(lexicon,
                                                              wordUpper));
                        }
                        str += wordItem.getLexiconSymbols();
                    }
                    return str;
                }
                else
                    return word;

                case DEFINITION_COLUMN:
                return MainSettings::getWordListShowDefinitions() ?
                    wordEngine->getDefinition(lexicon, wordUpper) :
                    QString();

                default:
                return word;
            }
        }

        default:
        return QVariant();
    }

}

//---------------------------------------------------------------------------
//  headerData
//
//! Return the data for the given role and section in the header with the
//! specified orientation.  Reimplemented from QAbstractTableModel.
//
//! @param section the sectionindex
//! @param orientation the header orientation
//! @param role the display role
//! @return the appropriate header data, or an empty object if invalid
//---------------------------------------------------------------------------
QVariant
WordTableModel::headerData(int section, Qt::Orientation orientation, int
                           role) const
{
    if ((section < 0)  || (section > NUM_COLUMNS - 1))
        return QVariant();

    if (orientation == Qt::Vertical)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (section) {
            case WILDCARD_MATCH_COLUMN:
            return MainSettings::getWordListGroupByAnagrams() ?
                WILDCARD_MATCH_HEADER : QString();

            case PROBABILITY_ORDER_COLUMN:
            return MainSettings::getWordListShowProbabilityOrder() ?
                PROBABILITY_ORDER_HEADER.arg(probNumBlanks) : QString();

            case PLAYABILITY_ORDER_COLUMN:
            return MainSettings::getWordListShowPlayabilityOrder() ?
                PLAYABILITY_ORDER_HEADER : QString();

            case FRONT_HOOK_COLUMN:
            return MainSettings::getWordListShowHooks() ?
                FRONT_HOOK_HEADER : QString();

            case BACK_HOOK_COLUMN:
            return MainSettings::getWordListShowHooks() ?
                BACK_HOOK_HEADER : QString();

            case WORD_COLUMN:
            return WORD_HEADER;

            case DEFINITION_COLUMN:
            return MainSettings::getWordListShowDefinitions() ?
                DEFINITION_HEADER : QString();

            default:
            return "Unknown";
        }
    }
    else
        return QVariant();
}

//---------------------------------------------------------------------------
//  insertRows
//
//! Insert rows into the model before the given row.  Reimplemented from
//! QAbstractTableModel.
//
//! @param row the row number to begin inserting rows
//! @param count the number of rows to insert
//! @param parent the parent index
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::insertRows(int row, int count, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        wordList.insert(row, WordItem(QString(), WordNormal));
    }

    endInsertRows();
    return true;
}

//---------------------------------------------------------------------------
//  removeRows
//
//! Remove rows starting with the given row under parent from the model.
//! Reimplemented from QAbstractTableModel.
//
//! @param row the row number to begin removing rows
//! @param count the number of rows to remove
//! @param parent the parent index
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::removeRows(int row, int count, const QModelIndex&)
{
    if (wordList.empty())
        return true;

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        wordList.removeAt(row);
    }

    endRemoveRows();
    return true;
}

//---------------------------------------------------------------------------
//  setData
//
//! Set the role data for the item at index to value.  Reimplemented from
//! QAbstractTableModel.
//
//! @param index the index
//! @param value the value to set
//! @param role the role for which to set data
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::setData(const QModelIndex& index, const QVariant& value, int
                        role)
{
    if (index.isValid() && (role == Qt::EditRole)) {
        if (index.column() == WILDCARD_MATCH_COLUMN) {
            wordList[index.row()].setWildcard(value.toString());
        }
        else if (index.column() == WORD_COLUMN) {
            WordItem& word = wordList[index.row()];
            word.setWord(value.toString());
            QString wordUpper = word.getWord().toUpper();
            if (MainSettings::getWordListShowHooks()) {
                word.setHooks(
                    wordEngine->getFrontHookLetters(lexicon, wordUpper),
                    wordEngine->getBackHookLetters(lexicon, wordUpper));
            }
            if (MainSettings::getWordListShowHookParents()) {
                word.setParentHooks(
                    wordEngine->getIsFrontHook(lexicon, wordUpper),
                    wordEngine->getIsBackHook(lexicon, wordUpper));
            }
            if (MainSettings::getWordListUseLexiconStyles()) {
                word.setLexiconSymbols(
                    wordEngine->getLexiconSymbols(lexicon, wordUpper));
            }
        }
        else if (index.column() == PROBABILITY_ORDER_COLUMN) {
            wordList[index.row()].setProbabilityOrder(value.toInt());
        }
        else if (index.column() == PLAYABILITY_ORDER_COLUMN) {
            wordList[index.row()].setPlayabilityOrder(value.toInt());
        }
        else {
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }
    else if (index.isValid() && (role == WordTypeRole)) {
        wordList[index.row()].setType(WordType(value.toInt()));
        emit dataChanged(index, index);
        return true;
    }
    else if (index.isValid() && (role == PlayabilityValueRole)) {
        wordList[index.row()].setPlayabilityValue(value.toLongLong());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
//  sort
//
//! Sort the model by column in the given order.  Reimplemented from
//! QAbstractItemModel.  NOTE: Both parameters are currently ignored.
//
//! @param column the column to sort by
//! @param order the sort order
//---------------------------------------------------------------------------
void
WordTableModel::sort(int, Qt::SortOrder)
{
    qSort(wordList.begin(), wordList.end(), lessThan);

    if (MainSettings::getWordListGroupByAnagrams())
        markAlternates();

    emit dataChanged(index(0, 0),
                     index(wordList.size() - 1, DEFINITION_COLUMN));
}

//---------------------------------------------------------------------------
//  reverse
//
//! Reverse the items in the model.
//---------------------------------------------------------------------------
void
WordTableModel::reverse()
{
    int iForward = 0;
    int iReverse = wordList.size() - 1;
    for (; iForward < iReverse; ++iForward, --iReverse) {
        wordList.swap(iForward, iReverse);
    }

    emit dataChanged(index(0, 0),
        index(wordList.size() - 1, DEFINITION_COLUMN));
}

//---------------------------------------------------------------------------
//  clearLastAddedIndex
//
//! Remove the designation of whichever item was last added.
//---------------------------------------------------------------------------
void
WordTableModel::clearLastAddedIndex()
{
    if (lastAddedIndex < 0)
        return;

    setData(index(lastAddedIndex, 0), WordNormal, WordTypeRole);
    emit dataChanged(index(lastAddedIndex, 0),
                     index(lastAddedIndex, DEFINITION_COLUMN));
    lastAddedIndex = -1;
}

//---------------------------------------------------------------------------
//  addWordPrivate
//
//! Add a word to the model.
//
//! @param word the word item to add
//! @param row the row whose data to set to the word
//---------------------------------------------------------------------------
void
WordTableModel::addWordPrivate(const WordItem& word, int row)
{
    setData(index(row, WORD_COLUMN), word.getWord(), Qt::EditRole);
    setData(index(row, WORD_COLUMN), word.getType(), WordTypeRole);
    setData(index(row, WORD_COLUMN), word.getPlayabilityValue(),
            PlayabilityValueRole);
    setData(index(row, WILDCARD_MATCH_COLUMN), word.getWildcard(),
                  Qt::EditRole);
    if (word.probabilityOrderIsValid()) {
        setData(index(row, PROBABILITY_ORDER_COLUMN),
                word.getProbabilityOrder(), Qt::EditRole);
    }
    if (word.playabilityOrderIsValid()) {
        setData(index(row, PLAYABILITY_ORDER_COLUMN),
                word.getPlayabilityOrder(), Qt::EditRole);
    }
}

//---------------------------------------------------------------------------
//  markAlternates
//
//! Mark alternating groups of alphagram matching items as WordNormalAlternate
//! instead of WordNormal.
//---------------------------------------------------------------------------
void
WordTableModel::markAlternates()
{
    QString prevAlphagram;
    bool alternate = false;
    for (int i = 0; i < wordList.size(); ++i) {
        WordItem& item = wordList[i];
        QString alphagram = Auxil::getAlphagram(item.getWord().toUpper());
        if (alphagram != prevAlphagram) {
            if (!prevAlphagram.isEmpty())
                alternate = !alternate;
            prevAlphagram = alphagram;
        }
        if (alternate && (item.getType() == WordNormal))
            item.setType(WordNormalAlternate);
    }
}

////---------------------------------------------------------------------------
////  isFrontHook
////
////! Determine whether a word is a front hook.  A word is a front hook if its
////! first letter can be removed to form a valid word.
////
////! @param word the word, assumed to be upper case
////! @return true if the word is a front hook, false otherwise
////---------------------------------------------------------------------------
//bool
//WordTableModel::isFrontHook(const QString& word) const
//{
//    return wordEngine->isAcceptable(lexicon, word.right(word.length() - 1));
//}
//
////---------------------------------------------------------------------------
////  isBackHook
////
////! Determine whether a word is a back hook.  A word is a back hook if its
////! last letter can be removed to form a valid word.
////
////! @param word the word, assumed to be upper case
////! @return true if the word is a back hook, false otherwise
////---------------------------------------------------------------------------
//bool
//WordTableModel::isBackHook(const QString& word) const
//{
//    return wordEngine->isAcceptable(lexicon, word.left(word.length() - 1));
//}
//
////---------------------------------------------------------------------------
////  getLexiconSymbols
////
////! Get lexicon symbols to be appended to a word to denote that the word
////! belongs to certain lexicon combinations.
////
////! @param word the word, assumed to be upper case
////! @return true a string containing all applicable lexicon symbols
////---------------------------------------------------------------------------
//QString
//WordTableModel::getLexiconSymbols(const QString& word) const
//{
//    QString symbolStr;
//
//    // FIXME: instead of querying settings and iterating for every word, the
//    // WordTableModel should cache a styles to be applied, and the cache
//    // should be updated whenever settings are changed
//    QListIterator<LexiconStyle> it (MainSettings::getWordListLexiconStyles());
//    while (it.hasNext()) {
//        const LexiconStyle& style = it.next();
//        if (style.lexicon != lexicon)
//            continue;
//
//        bool acceptable = wordEngine->isAcceptable(style.compareLexicon, word);
//        if (!(acceptable ^ style.inCompareLexicon))
//            symbolStr += style.symbol;
//    }
//    return symbolStr;
//}

//---------------------------------------------------------------------------
//  init
//
//! Initialize a word item.
//---------------------------------------------------------------------------
void
WordTableModel::WordItem::init()
{
    hooksValid = false;
    parentHooksValid = false;
    probabilityOrderValid = false;
    playabilityOrderValid = false;
    lexiconSymbolsValid = false;
    frontParentHook = false;
    backParentHook = false;
    probabilityOrder = 0;
    playabilityValue = 0;
    playabilityOrder = 0;
}

//---------------------------------------------------------------------------
//  setProbabilityOrder
//
//! Set probability order of a word item.
//
//! @param p the probability order
//---------------------------------------------------------------------------
void
WordTableModel::WordItem::setProbabilityOrder(int p)
{
    probabilityOrder = p;
    probabilityOrderValid = true;
}

//---------------------------------------------------------------------------
//  setPlayabilityValue
//
//! Set playability value of a word item.
//
//! @param p the playability value
//---------------------------------------------------------------------------
void
WordTableModel::WordItem::setPlayabilityValue(qint64 p)
{
    playabilityValue = p;
}

//---------------------------------------------------------------------------
//  setPlayabilityOrder
//
//! Set playability order of a word item.
//
//! @param p the playability order
//---------------------------------------------------------------------------
void
WordTableModel::WordItem::setPlayabilityOrder(int p)
{
    playabilityOrder = p;
    playabilityOrderValid = true;
}

//---------------------------------------------------------------------------
//  setLexiconSymbols
//
//! Set lexicon symbols of a word item.
//
//! @param s the lexicon symbols
//---------------------------------------------------------------------------
void
WordTableModel::WordItem::setLexiconSymbols(const QString& s)
{
    lexiconSymbols = s;
    lexiconSymbolsValid = true;
}

//---------------------------------------------------------------------------
//  setHooks
//
//! Set front and back hooks of a word item.
//
//! @param front the front hooks
//! @param back the back hooks
//---------------------------------------------------------------------------
void
WordTableModel::WordItem::setHooks(const QString& front, const QString& back)
{
    if (hooksValid)
        return;
    frontHooks = front;
    backHooks = back;
    hooksValid = true;
}

//---------------------------------------------------------------------------
//  setParentHooks
//
//! Set front and back parent hooks of a word item.
//
//! @param front the front parent hook
//! @param back the back parent hook
//---------------------------------------------------------------------------
void
WordTableModel::WordItem::setParentHooks(bool front, bool back)
{
    if (parentHooksValid)
        return;
    frontParentHook = front;
    backParentHook = back;
    parentHooksValid = true;
}
