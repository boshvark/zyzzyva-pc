//---------------------------------------------------------------------------
// ZListViewItem.h
//
// A listview item with special capabilities.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
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

