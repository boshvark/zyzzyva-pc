//---------------------------------------------------------------------------
// WordTableDelegate.cpp
//
// A class derived from QItemDelegate, used to render items in a word list.
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

#include "WordTableDelegate.h"
#include "WordTableModel.h"
#include "Defs.h"
#include <QModelIndex>
#include <QPainter>

using namespace std;

const QColor VALID_NORMAL_WORD_FOREGROUND = Qt::black;
const QColor VALID_NORMAL_WORD_BACKGROUND = Qt::white;
const QColor VALID_CORRECT_WORD_FOREGROUND = Qt::black;
const QColor VALID_CORRECT_WORD_BACKGROUND = Qt::white;
const QColor VALID_MISSED_WORD_FOREGROUND = Qt::black;
const QColor VALID_MISSED_WORD_BACKGROUND = Qt::green;
const QColor INVALID_WORD_FOREGROUND = Qt::black;
const QColor INVALID_WORD_BACKGROUND = Qt::red;

//---------------------------------------------------------------------------
//  WordTableDelegate
//
//! Constructor.
//
//! @param parent the parent object
//---------------------------------------------------------------------------
WordTableDelegate::WordTableDelegate (QWidget* parent)
    : QItemDelegate (parent)
{
}

//---------------------------------------------------------------------------
//  paint
//
//! Paint an item in a word list.
//
//! @param painter the painter to use
//! @param option style option
//! @param index the model index corresponding to the item
//---------------------------------------------------------------------------
void
WordTableDelegate::paint (QPainter* painter, const QStyleOptionViewItem&
                          option, const QModelIndex& index) const
{
    WordTableModel::WordType type =
        WordTableModel::WordType (index.model()->data (index,
                                                       Qt::UserRole).toInt());

    QColor color;

    // Draw background
    switch (type) {
        case WordTableModel::WordNormal:
        color = VALID_NORMAL_WORD_BACKGROUND;
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
        default: break;
    }
    painter->setBrush (QBrush (color));
    painter->drawRect (option.rect);

    // Draw foreground
    switch (type) {
        case WordTableModel::WordNormal:
        color = VALID_NORMAL_WORD_FOREGROUND;
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
        default: break;
    }
    painter->setPen (color);
    painter->drawText (option.rect,
                       Qt::AlignLeft | Qt::AlignVCenter,
                       index.model()->data (index,
                                            Qt::DisplayRole).toString());
}
