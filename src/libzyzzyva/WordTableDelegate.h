//---------------------------------------------------------------------------
// WordTableDelegate.h
//
// A class derived from QItemDelegate, used to render items in a word list.
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

#ifndef ZYZZYVA_WORD_TABLE_DELEGATE_H
#define ZYZZYVA_WORD_TABLE_DELEGATE_H

#include <QString>
#include <QItemDelegate>

class WordEngine;

class WordTableDelegate : public QItemDelegate
{
    Q_OBJECT
    public:
    static const int ITEM_XPADDING;

    public:
    WordTableDelegate (QWidget* parent = 0);
    virtual ~WordTableDelegate() { }

    protected:
    virtual void paint (QPainter* painter, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const;

};

#endif // ZYZZYVA_WORD_TABLE_DELEGATE_H
