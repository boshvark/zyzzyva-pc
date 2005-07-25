//---------------------------------------------------------------------------
// WordListViewItem.cpp
//
// A listview item with special capabilities.
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

#include "WordListViewItem.h"
#include "WordListView.h"

bool WordListViewItem::sortByLength = false;

//---------------------------------------------------------------------------
//  getWord
//
//! Get the word associated with this item.
//
//! @return the word
//---------------------------------------------------------------------------
QString
WordListViewItem::getWord() const
{
    return text (WordListView::WORD_COLUMN).remove
        (WordListView::PARENT_HOOK_CHAR).remove (" ");
}

//---------------------------------------------------------------------------
//  setTextColor
//
//! Set the text color of the listview item.
//
//! @param color the text color
//---------------------------------------------------------------------------
void
WordListViewItem::setTextColor (const QColor& color)
{
    QColorGroup active = palette.active();
    QColorGroup inactive = palette.inactive();
    active.setColor (QColorGroup::Text, color);
    active.setColor (QColorGroup::Highlight, color);
    active.setColor (QColorGroup::HighlightedText, active.base());
    inactive.setColor (QColorGroup::Text, color);
    inactive.setColor (QColorGroup::Highlight, color);
    inactive.setColor (QColorGroup::HighlightedText, active.base());
    palette.setActive (active);
    palette.setInactive (inactive);
}

//---------------------------------------------------------------------------
//  setFont
//
//! Set the font of the listview item.
//
//! @param color the text color
//---------------------------------------------------------------------------
void
WordListViewItem::setFont (const QFont& f)
{
    font = f;
}

//---------------------------------------------------------------------------
//  width
//
//! Get the width of the listview item.  Overrides QListViewItem::width.
//! See the Qt documentation for details of this method.
//---------------------------------------------------------------------------
int
WordListViewItem::width (const QFontMetrics&, const QListView* lview, int c) const
{
    QFontMetrics metrics (font);
    return QListViewItem::width (metrics, lview, c);
}

//---------------------------------------------------------------------------
//  paintCell
//
//! Paint the contents of the listview item.  Overrides
//! QListViewItem::paintCell.  See the Qt documentation for details of this
//! method.
//---------------------------------------------------------------------------
void
WordListViewItem::paintCell (QPainter* p, const QColorGroup&, int column,
                          int width, int align)
{
    p->setFont (font);
    QListViewItem::paintCell (p, getColorGroup(), column, width, align);
}

//---------------------------------------------------------------------------
//  paintCell
//
//! Paint a focus indicator.  Overrides QListViewItem::paintFocus.
//! See the Qt documentation for details of this method.
//---------------------------------------------------------------------------
void
WordListViewItem::paintFocus (QPainter* p, const QColorGroup&, const QRect& r)
{
    p->setFont (font);
    QListViewItem::paintFocus (p, getColorGroup(), r);
}

//---------------------------------------------------------------------------
//  init
//
//! Initialize the listview item.  Called from all constructors.
//---------------------------------------------------------------------------
void
WordListViewItem::init()
{
    QListView* lview = listView();
    font = lview->font();
    palette = lview->palette();
}

//---------------------------------------------------------------------------
//  getColorGroup
//
//! Get the current color group to be used in painting the listview item.
//---------------------------------------------------------------------------
const QColorGroup&
WordListViewItem::getColorGroup()
{
    QListView* lview = listView();
    if (!lview->isEnabled())
        return palette.disabled();
    else if (lview->isActiveWindow())
        return palette.active();
    else
        return palette.inactive();
}

//---------------------------------------------------------------------------
//  key
//
//! Reimplementation of virtual method QListViewItem::key.  Return a key that
//! can be used for sorting this listview item by a particular column.
//
//! @param column the column index to be compared
//! @param ascending the order in which to compare (ignored)
//! @return a string by which this item can be compared
//---------------------------------------------------------------------------
QString
WordListViewItem::key (int column, bool) const
{
    return ((column == WordListView::WORD_COLUMN) ? getWord().upper()
                                                  : text (column));
}

//---------------------------------------------------------------------------
//  compare
//
//! Reimplementation of virtual method QListViewItem::compare.  Compare this
//! listview item to item I using the column COL in ASCENDING order.
//
//! @param i the listview item to compare with
//! @param col the column index to compare
//! @param ascending the order in which to compare (ignored)
//! @return < 0 if this item is less than I, 0 if they are equal, and > 0 if
//! this item is > I.
//---------------------------------------------------------------------------
int
WordListViewItem::compare (QListViewItem* i, int col, bool ascending) const
{
    if ((col != 1) || !sortByLength)
        return QListViewItem::compare (i, col, ascending);

    QString mkey = key (col, ascending);
    QString ikey = i->key (col, ascending);
    if (mkey.length() < ikey.length())
        return -1;
    else if (mkey.length() > ikey.length())
        return 1;
    else
        return mkey.compare (ikey);
}
