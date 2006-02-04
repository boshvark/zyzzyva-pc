//---------------------------------------------------------------------------
// WordTableModel.cpp
//
// A model for representing word lists.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

using namespace std;

const QChar WordTableModel::PARENT_HOOK_CHAR = '-';
const QString WILDCARD_MATCH_HEADER = "?";
const QString FRONT_HOOK_HEADER = "H";
const QString WORD_HEADER = "Word";
const QString BACK_HOOK_HEADER = "H";
const QString DEFINITION_HEADER = "Definition";
const int ITEM_MARGIN = 5;
const int DEFAULT_COLUMN_WIDTH = 100;
const int NUM_COLUMNS = 5;

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
lessThan (const WordTableModel::WordItem& a,
          const WordTableModel::WordItem& b)
{
    if (MainSettings::getWordListGroupByWildcards()) {
        if (a.getWildcard() < b.getWildcard())
            return true;
        else if (a.getWildcard() > b.getWildcard())
            return false;
    }

    if (MainSettings::getWordListSortByLength()) {
        if (a.getWord().length() < b.getWord().length()) {
            return true;
        }
        else if (a.getWord().length() > b.getWord().length()) {
            return false;
        }
    }

    return (a.getWord().toUpper() < b.getWord().toUpper());
}

//---------------------------------------------------------------------------
//  WordTableModel
//
//! Constructor.
//
//! @param parent the parent object
//---------------------------------------------------------------------------
WordTableModel::WordTableModel (WordEngine* e, QObject* parent)
    : QAbstractTableModel (parent), wordEngine (e), lastAddedIndex (-1)
{
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
    bool ok = removeRows (0, rowCount());
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
WordTableModel::addWord (const WordItem& word, bool updateLastAdded)
{
    int row = rowCount();
    bool ok = insertRow (row);
    if (!ok)
        return false;

    addWordPrivate (word, row);

    sort (WORD_COLUMN);
    lastAddedIndex = updateLastAdded ? wordList.indexOf (word) : -1;
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
WordTableModel::addWords (const QList<WordItem>& words)
{
    int row = rowCount();
    bool ok = insertRows (row, words.size());
    if (!ok)
        return false;
    WordItem word;
    foreach (word, words) {
        addWordPrivate (word, row);
        ++row;
    }
    sort (WORD_COLUMN);
    lastAddedIndex = -1;
    emit wordsChanged();
    return true;
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
WordTableModel::rowCount (const QModelIndex&) const
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
WordTableModel::columnCount (const QModelIndex&) const
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
WordTableModel::data (const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ((index.row() < 0) || (index.row() >= wordList.count()))
        return QVariant();

    if (role == Qt::UserRole)
        return wordList.at (index.row()).getType();

    if ((role != Qt::DisplayRole) && (role != Qt::EditRole))
        return QVariant();

    WordItem& wordItem = wordList[index.row()];
    QString word = wordItem.getWord();
    if (word.isEmpty())
        return QString();

    QString wordUpper = word.toUpper();
    switch (index.column()) {
        case WILDCARD_MATCH_COLUMN:
        if (!MainSettings::getWordListGroupByWildcards()) {
            return QString();
        }
        return wordItem.getWildcard();

        case FRONT_HOOK_COLUMN:
        if (!MainSettings::getWordListShowHooks()) {
            return QString();
        }
        else if (!wordItem.hooksAreValid()) {
            wordItem.setHooks (getFrontHookLetters (wordUpper),
                               getBackHookLetters (wordUpper));
        }
        return wordItem.getFrontHooks();

        case BACK_HOOK_COLUMN:
        if (!MainSettings::getWordListShowHooks()) {
            return QString();
        }
        else if (!wordItem.hooksAreValid()) {
            wordItem.setHooks (getFrontHookLetters (wordUpper),
                               getBackHookLetters (wordUpper));
        }
        return wordItem.getBackHooks();

        case WORD_COLUMN:
        if (role == Qt::EditRole) {
            return wordUpper;
        }
        else if (role == Qt::DisplayRole) {
            if (MainSettings::getWordListShowHookParents()) {
                if (!wordItem.parentHooksAreValid()) {
                    wordItem.setParentHooks (isFrontHook (wordUpper),
                                             isBackHook (wordUpper));
                }
                return (wordItem.getFrontParentHook() ? PARENT_HOOK_CHAR
                                                      : QChar (' '))
                       + word
                       + (wordItem.getBackParentHook() ? PARENT_HOOK_CHAR
                                                       : QChar (' '));
            }
            else
                return word;
        }
        else
            return word;

        case DEFINITION_COLUMN:
        return MainSettings::getWordListShowDefinitions() ?
            wordEngine->getDefinition (wordUpper) : QString::null;

        default: return word;
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
WordTableModel::headerData (int section, Qt::Orientation orientation, int
                            role) const
{
    if ((section < 0)  || (section > NUM_COLUMNS - 1))
        return QVariant();

    if (orientation == Qt::Vertical)
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (section) {
            case WILDCARD_MATCH_COLUMN:
            return MainSettings::getWordListGroupByWildcards() ?
                WILDCARD_MATCH_HEADER : QString::null;

            case FRONT_HOOK_COLUMN:
            return MainSettings::getWordListShowHooks() ?
                FRONT_HOOK_HEADER : QString::null;

            case BACK_HOOK_COLUMN:
            return MainSettings::getWordListShowHooks() ?
                BACK_HOOK_HEADER : QString::null;

            case WORD_COLUMN:
            return WORD_HEADER;

            case DEFINITION_COLUMN:
            return MainSettings::getWordListShowDefinitions() ?
                DEFINITION_HEADER : QString::null;

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
WordTableModel::insertRows (int row, int count, const QModelIndex&)
{
    beginInsertRows (QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        wordList.insert (row, WordItem ("", WordNormal));
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
WordTableModel::removeRows (int row, int count, const QModelIndex&)
{
    beginRemoveRows (QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; ++i) {
        wordList.removeAt (row);
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
WordTableModel::setData (const QModelIndex& index, const QVariant& value, int
                         role)
{
    if (index.isValid() && (role == Qt::EditRole)) {
        if (index.column() == WILDCARD_MATCH_COLUMN) {
            wordList[index.row()].setWildcard (value.toString());
        }
        else if (index.column() == WORD_COLUMN) {
            WordItem& word = wordList[index.row()];
            word.setWord (value.toString());
            if (MainSettings::getWordListShowHooks()) {
                word.setHooks
                    (getFrontHookLetters (word.getWord().toUpper()),
                     getBackHookLetters (word.getWord().toUpper()));
            }
            if (MainSettings::getWordListShowHookParents()) {
                word.setParentHooks
                    (isFrontHook (word.getWord().toUpper()),
                     isBackHook (word.getWord().toUpper()));
            }
        }
        else {
            return false;
        }
        emit dataChanged (index, index);
        return true;
    }
    else if (index.isValid() && (role == Qt::UserRole)) {
        wordList[index.row()].setType (WordType (value.toInt()));
        emit dataChanged (index, index);
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
WordTableModel::sort (int, Qt::SortOrder)
{
    qSort (wordList.begin(), wordList.end(), lessThan);


    if (MainSettings::getWordListGroupByWildcards())
        markAlternates();

    emit dataChanged (index (0, 0),
                      index (wordList.size() - 1, DEFINITION_COLUMN));
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

    setData (index (lastAddedIndex, 0), WordNormal, Qt::UserRole);
    emit dataChanged (index (lastAddedIndex, 0),
                      index (lastAddedIndex, DEFINITION_COLUMN));
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
WordTableModel::addWordPrivate (const WordItem& word, int row)
{
    setData (index (row, WORD_COLUMN), word.getWord(), Qt::EditRole);
    setData (index (row, WORD_COLUMN), word.getType(), Qt::UserRole);
    setData (index (row, WILDCARD_MATCH_COLUMN), word.getWildcard(),
                    Qt::EditRole);
}

//---------------------------------------------------------------------------
//  markAlternates
//
//! Mark alternating groups of wildcard matching items as WordNormalAlternate
//! instead of WordNormal.
//---------------------------------------------------------------------------
void
WordTableModel::markAlternates()
{
    QString prevWildcard;
    bool alternate = false;
    for (int i = 0; i < wordList.size(); ++i) {
        WordItem& item = wordList[i];
        if (item.getWildcard() != prevWildcard) {
            if (!prevWildcard.isEmpty())
                alternate = !alternate;
            prevWildcard = item.getWildcard();
        }
        if (alternate && (item.getType() == WordNormal))
            item.setType (WordNormalAlternate);
    }
}

//---------------------------------------------------------------------------
//  isFrontHook
//
//! Determine whether a word is a front hook.  A word is a front hook if its
//! first letter can be removed to form a valid word.
//
//! @param word the word, assumed to be upper case
//! @return true if the word is a front hook, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::isFrontHook (const QString& word) const
{
    return wordEngine->isAcceptable (word.right (word.length() - 1));
}

//---------------------------------------------------------------------------
//  isBackHook
//
//! Determine whether a word is a back hook.  A word is a back hook if its
//! last letter can be removed to form a valid word.
//
//! @param word the word, assumed to be upper case
//! @return true if the word is a back hook, false otherwise
//---------------------------------------------------------------------------
bool
WordTableModel::isBackHook (const QString& word) const
{
    return wordEngine->isAcceptable (word.left (word.length() - 1));
}

//---------------------------------------------------------------------------
//  getFrontHookLetters
//
//! Get a string of letters that can be added to the front of a word to make
//! other valid words.
//
//! @param word the word, assumed to be upper case
//! @return a string containing lower case letters representing front hooks
//---------------------------------------------------------------------------
QString
WordTableModel::getFrontHookLetters (const QString& word) const
{
    SearchSpec spec;
    SearchCondition condition;
    condition.type = SearchCondition::PatternMatch;
    condition.stringValue = "?" + word;
    spec.conditions.append (condition);

    // Put first letter of each word in a set, for alphabetical order
    QStringList words = wordEngine->search (spec, true);
    set<QChar> letters;
    QStringList::iterator it;
    for (it = words.begin(); it != words.end(); ++it)
        letters.insert ((*it).at (0).toLower());

    QString ret;
    set<QChar>::iterator sit;
    for (sit = letters.begin(); sit != letters.end(); ++sit)
        ret += *sit;
    return ret;
}

//---------------------------------------------------------------------------
//  getBackHookLetters
//
//! Get a string of letters that can be added to the back of a word to make
//! other valid words.
//
//! @param word the word, assumed to be upper case
//! @return a string containing lower case letters representing back hooks
//---------------------------------------------------------------------------
QString
WordTableModel::getBackHookLetters (const QString& word) const
{
    SearchSpec spec;
    SearchCondition condition;
    condition.type = SearchCondition::PatternMatch;
    condition.stringValue = word + "?";
    spec.conditions.append (condition);

    // Put first letter of each word in a set, for alphabetical order
    QStringList words = wordEngine->search (spec, true);
    set<QChar> letters;
    QStringList::iterator it;
    for (it = words.begin(); it != words.end(); ++it)
        letters.insert ((*it).at ((*it).length() - 1).toLower());

    QString ret;
    set<QChar>::iterator sit;
    for (sit = letters.begin(); sit != letters.end(); ++sit)
        ret += *sit;
    return ret;
}

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
    frontParentHook = false;
    backParentHook = false;
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
WordTableModel::WordItem::setHooks (const QString& front, const QString& back)
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
WordTableModel::WordItem::setParentHooks (bool front, bool back)
{
    if (parentHooksValid)
        return;
    frontParentHook = front;
    backParentHook = back;
    parentHooksValid = true;
}
