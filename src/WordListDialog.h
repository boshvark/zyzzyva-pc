//---------------------------------------------------------------------------
// WordListDialog.h
//
// The dialog for editing a list of words.
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

#ifndef ZYZZYVA_WORD_LIST_DIALOG_H
#define ZYZZYVA_WORD_LIST_DIALOG_H

#include "WordEngine.h"
#include <QDialog>
#include <QLabel>
#include <QListWidget>

class WordListDialog : public QDialog
{
    Q_OBJECT
    public:
    WordListDialog (QWidget* parent = 0, Qt::WFlags f = 0);
    ~WordListDialog();

    int numWords() const { return wordList->count(); }
    void setWords (const QString& string);
    QString getWords() const;

    public slots:
    void openFileClicked();
    void clearClicked();

    private:
    void updateLabel();

    private:
    QLabel* numWordsLabel;
    QListWidget* wordList;
};

#endif // ZYZZYVA_WORD_LIST_DIALOG_H

