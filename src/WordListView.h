//---------------------------------------------------------------------------
// WordListView.h
//
// A customized list view.
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

#ifndef WORD_LIST_VIEW_H
#define WORD_LIST_VIEW_H

#include <qevent.h>
#include <qlistview.h>

class WordEngine;
class WordListViewItem;

class WordListView : public QListView
{
    Q_OBJECT
    public:
    WordListView (WordEngine* e, QWidget* parent = 0, const char* name = 0,
                  WFlags f = 0);
    ~WordListView();

    void setFont (const QFont& font);
    WordListViewItem* addWord (const QString& word);
    void resetColumnWidths();

    public slots:
    void clear();
    void doReturnPressed (QListViewItem* item);
    void doPopupMenu (QListViewItem* item, const QPoint& point, int);
    void exportRequested();
    void createQuizRequested();
    void displayDefinition (const QString& word);
    void showHooks (bool show);
    void showDefinitions (bool show);

    private:
    void contextMenuEvent (QContextMenuEvent* e);
    bool exportFile (const QString& filename, QString* err = 0) const;

    private:
    WordEngine* wordEngine;
    bool hidden[4];
};

#endif // WORD_LIST_VIEW_H

