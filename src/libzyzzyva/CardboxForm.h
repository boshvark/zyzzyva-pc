//---------------------------------------------------------------------------
// CardboxForm.h
//
// A form for querying and editing the contents of the cardbox system.
//
// Copyright 2007 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_CARDBOX_FORM_H
#define ZYZZYVA_CARDBOX_FORM_H

#include "ActionForm.h"
#include <QLabel>
#include <QSpinBox>
#include <QSqlQueryModel>
#include <QTreeView>

class QuizDatabase;
class WordEngine;

class CardboxForm : public ActionForm
{
    Q_OBJECT
    public:
    CardboxForm (WordEngine* e, QWidget* parent = 0, Qt::WFlags f = 0);
    QIcon getIcon() const;
    QString getTitle() const;
    QString getStatusString() const;

    public slots:
    void shiftClicked();
    void refreshClicked();
    void quizSpecChanged();

    private:
    WordEngine*     wordEngine;
    QuizDatabase*   quizDatabase;
    QSqlQueryModel* cardboxCountModel;
    QSqlQueryModel* cardboxDaysModel;
    QSpinBox*       backlogSbox;
    QLabel*         backlogLabel;
    QTreeView*      cardboxCountView;
    QTreeView*      cardboxDaysView;
};

#endif // ZYZZYVA_CARDBOX_FORM_H
