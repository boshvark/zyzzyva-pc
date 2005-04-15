//---------------------------------------------------------------------------
// WordListViewItem.h
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

#ifndef WORD_LIST_VIEW_ITEM_H
#define WORD_LIST_VIEW_ITEM_H

#include <qlistview.h>
#include <qpainter.h>

class WordListViewItem : public QListViewItem
{
  public:
    WordListViewItem (QListView* parent)
        : QListViewItem (parent) { init(); }

    WordListViewItem (QListViewItem* parent)
        : QListViewItem (parent) { init(); }

    WordListViewItem (QListView* parent, WordListViewItem* after)
        : QListViewItem (parent, after) { init(); }

    WordListViewItem (QListViewItem* parent, WordListViewItem* after)
        : QListViewItem (parent, after) { init(); }

    WordListViewItem (QListView* parent, QString label1,
                      QString label2 = QString::null,
                      QString label3 = QString::null,
                      QString label4 = QString::null,
                      QString label5 = QString::null,
                      QString label6 = QString::null,
                      QString label7 = QString::null,
                      QString label8 = QString::null)
        : QListViewItem (parent, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    WordListViewItem (QListViewItem* parent, QString label1,
                      QString label2 = QString::null,
                      QString label3 = QString::null,
                      QString label4 = QString::null,
                      QString label5 = QString::null,
                      QString label6 = QString::null,
                      QString label7 = QString::null,
                      QString label8 = QString::null)
        : QListViewItem (parent, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    WordListViewItem (QListView* parent, WordListViewItem* after,
                      QString label1, QString label2 = QString::null,
                      QString label3 = QString::null,
                      QString label4 = QString::null,
                      QString label5 = QString::null,
                      QString label6 = QString::null,
                      QString label7 = QString::null,
                      QString label8 = QString::null)
        : QListViewItem (parent, after, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    WordListViewItem (QListViewItem* parent, WordListViewItem* after,
                      QString label1, QString label2 = QString::null,
                      QString label3 = QString::null,
                      QString label4 = QString::null,
                      QString label5 = QString::null,
                      QString label6 = QString::null,
                      QString label7 = QString::null,
                      QString label8 = QString::null)
        : QListViewItem (parent, after, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    ~WordListViewItem() { }

    static void setSortByLength (bool b) { sortByLength = b; }
    static bool getSortByLength() { return sortByLength; }
    void setTextColor (const QColor& color);
    void setFont (const QFont& font);

    int width (const QFontMetrics& fm, const QListView* lview, int c) const;
    void paintCell (QPainter* p, const QColorGroup& cg, int column, int width,
                    int align);
    void paintFocus (QPainter* p, const QColorGroup& cg, const QRect& r);
    virtual QString key (int column, bool ascending) const;
    virtual int compare (QListViewItem* i, int col, bool ascending) const;

  private:
    void init();
    const QColorGroup& getColorGroup();
    static bool sortByLength;

  private:
    QFont font;
    QPalette palette;
};

#endif // WORD_LIST_VIEW_ITEM_H

