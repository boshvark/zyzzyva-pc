//---------------------------------------------------------------------------
// CardboxForm.h
//
// A form for querying and editing the contents of the cardbox system.
//
// Copyright 2007-2012 Boshvark Software, LLC.
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
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSqlQueryModel>
#include <QTimer>
#include <QTreeWidget>

class LexiconSelectWidget;
class WordEngine;

class CardboxForm : public ActionForm
{
    Q_OBJECT
    public:
    CardboxForm(WordEngine* e, QWidget* parent = 0, Qt::WFlags f = 0);
    QIcon getIcon() const;
    QString getTitle() const;
    QString getStatusString() const;
    QString getDetailsString() const;

    public slots:
    void lexiconActivated(const QString& lexicon);
    void refreshClicked();
    void questionDataClicked();

    private:
    WordEngine*     wordEngine;
    //QuizStatsDatabase*   quizDatabase;
    LexiconSelectWidget* lexiconWidget;
    QComboBox*      quizTypeCombo;
    //QSqlQueryModel* cardboxCountModel;
    //QSqlQueryModel* cardboxDaysModel;
    //QSqlTableModel* cardboxContentsModel;
    //QTreeView*      cardboxCountView;
    //QTreeView*      cardboxDaysView;
    //QTreeView*      cardboxContentsView;
    QTreeWidget*    cardboxCountTree;
    QTreeWidget*    cardboxDaysTree;
    QLineEdit*      questionLine;
    QLabel*         questionDataLabel;

    //QTimer refreshTimer;
    QString detailsString;
};

#endif // ZYZZYVA_CARDBOX_FORM_H
