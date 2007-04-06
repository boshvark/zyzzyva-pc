//---------------------------------------------------------------------------
// SearchConditionForm.h
//
// A form for specifying a search condition.
//
// Copyright 2005, 2006, 2007 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_SEARCH_CONDITION_FORM_H
#define ZYZZYVA_SEARCH_CONDITION_FORM_H

#include "SearchCondition.h"
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QToolButton>
#include <set>

class SearchCondition;
class WordLineEdit;
class WordValidator;

class SearchConditionForm : public QWidget
{
    Q_OBJECT
    public:
    SearchConditionForm(QWidget* parent = 0, Qt::WFlags f = 0);

    SearchCondition getSearchCondition() const;
    void setSearchCondition(const SearchCondition& condition);
    bool isValid() const;

    signals:
    void returnPressed();
    void contentsChanged();
    void addClicked();
    void deleteClicked();

    public slots:
    void typeChanged(const QString&);
    void editListClicked();
    void reset();
    void selectInputArea();
    void setAddEnabled(bool enable);
    void setDeleteEnabled(bool enable);

    private:
    void setWordListString(const QString& string);
    bool matchStringIsValid(const QString& string) const;

    private:
    QCheckBox*      negationCbox;
    QComboBox*      typeCbox;
    QStackedWidget* paramStack;
    QWidget*        paramLineWidget;
    WordLineEdit*   paramLine;
    QWidget*        paramSboxWidget;
    QSpinBox*       paramMinSbox;
    QSpinBox*       paramMaxSbox;
    QCheckBox*      paramProbCbox;
    QWidget*        paramCboxWidget;
    QComboBox*      paramCbox;
    QWidget*        paramConsistWidget;
    QSpinBox*       paramConsistMinSbox;
    QSpinBox*       paramConsistMaxSbox;
    WordLineEdit*   paramConsistLine;
    QWidget*        paramWordListWidget;
    QLineEdit*      paramWordListLine;
    QString         paramWordListString;
    WordValidator*  letterValidator;
    WordValidator*  patternValidator;
    QToolButton*    addButton;
    QToolButton*    deleteButton;

    bool legacy;
};

#endif // ZYZZYVA_SEARCH_CONDITION_FORM_H
