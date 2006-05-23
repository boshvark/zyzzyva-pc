//---------------------------------------------------------------------------
// JudgeDialog.h
//
// A full-screen dialog for Word Judge functionality, in which the user can
// very easily judge the validity of one or more words.
//
// Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef ZYZZYVA_JUDGE_DIALOG_H
#define ZYZZYVA_JUDGE_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QStackedWidget>
#include <QTimer>
#include <QWidget>

class WordEngine;
class WordTextEdit;

class JudgeDialog : public QDialog
{
    Q_OBJECT
    public:
    JudgeDialog (WordEngine* e, QWidget* parent = 0, Qt::WFlags f = 0);

    public slots:
    void textChanged();
    void clear();
    void judgeWord();
    void clearResultsReleaseHold();
    void displayExit();
    void clearExit();

    protected slots:
    void keyPressEvent (QKeyEvent* e);

    private:
    WordEngine*     engine;
    QStackedWidget* widgetStack;
    QWidget*        inputWidget;
    QLabel*         instLabel;
    WordTextEdit*   inputArea;
    QWidget*        resultWidget;
    QLabel*         resultLabel;
    QTimer*         resultTimer;
    QTimer*         exitTimer;
    int             clearResultsHold;
};

#endif // ZYZZYVA_JUDGE_DIALOG_H
