//---------------------------------------------------------------------------
// ZListViewItem.h
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

#ifndef ZLISTVIEWITEM_H
#define ZLISTVIEWITEM_H

#include <qlistview.h>
#include <qpainter.h>

class ZListViewItem : public QListViewItem
{
  public:
    ZListViewItem (QListView* parent)
        : QListViewItem (parent) { init(); }

    ZListViewItem (QListViewItem* parent)
        : QListViewItem (parent) { init(); }

    ZListViewItem (QListView* parent, ZListViewItem* after)
        : QListViewItem (parent, after) { init(); }

    ZListViewItem (QListViewItem* parent, ZListViewItem* after)
        : QListViewItem (parent, after) { init(); }

    ZListViewItem (QListView* parent, QString label1,
                   QString label2=QString::null, QString label3=QString::null,
                   QString label4=QString::null, QString label5=QString::null,
                   QString label6=QString::null, QString label7=QString::null,
                   QString label8=QString::null)
        : QListViewItem (parent, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    ZListViewItem (QListViewItem* parent, QString label1,
                   QString label2=QString::null, QString label3=QString::null,
                   QString label4=QString::null, QString label5=QString::null,
                   QString label6=QString::null, QString label7=QString::null,
                   QString label8=QString::null)
        : QListViewItem (parent, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    ZListViewItem (QListView* parent, ZListViewItem* after,
                   QString label1, QString label2=QString::null,
                   QString label3=QString::null, QString label4=QString::null,
                   QString label5=QString::null, QString label6=QString::null,
                   QString label7=QString::null, QString label8=QString::null)
        : QListViewItem (parent, after, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    ZListViewItem (QListViewItem* parent, ZListViewItem* after,
                   QString label1, QString label2 = QString::null,
                   QString label3=QString::null, QString label4=QString::null,
                   QString label5=QString::null, QString label6=QString::null,
                   QString label7=QString::null, QString label8=QString::null)
        : QListViewItem (parent, after, label1, label2, label3, label4, label5,
                         label6, label7, label8) { init(); }

    ~ZListViewItem() { }

    void setTextColor (const QColor& color);
    void setFont (const QFont& font);

    int width (const QFontMetrics& fm, const QListView* lview, int c) const;
    void paintCell (QPainter* p, const QColorGroup& cg, int column, int width,
                    int align);
    void paintFocus (QPainter* p, const QColorGroup& cg, const QRect& r);

  private:
    void init();
    const QColorGroup& getColorGroup();

  private:
    QFont font;
    QPalette palette;
};

#endif // ZLISTVIEWITEM_H

