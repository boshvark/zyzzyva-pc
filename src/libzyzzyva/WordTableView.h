//---------------------------------------------------------------------------
// WordTableView.h
//
// A class derived from QTableView, used to display word lists.
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

#ifndef ZYZZYVA_WORD_TABLE_VIEW_H
#define ZYZZYVA_WORD_TABLE_VIEW_H

#include <QString>
#include <QTreeView>

class WordEngine;

class WordTableView : public QTreeView
{
    Q_OBJECT
    public:
    WordTableView (WordEngine* e, QWidget* parent = 0);
    virtual ~WordTableView() { }

    public slots:
    virtual void resizeItemsToContents();

    protected:
    virtual void contextMenuEvent (QContextMenuEvent* e);
    virtual bool viewportEvent (QEvent* e);
    virtual int sizeHintForColumn (int column) const;
    virtual int sizeHintForRow (int row) const;

    private slots:
    void viewDefinition();
    void viewVariation (int variation);
    void exportRequested();
    void createQuizRequested();

    private:
    bool exportFile (const QString& filename, QString* err) const;
    QString hookToolTipText (const QString& word, const QString& hooks,
                             bool front) const;

    private:
    WordEngine* wordEngine;

};

#endif // ZYZZYVA_WORD_TABLE_VIEW_H
