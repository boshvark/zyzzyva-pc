//---------------------------------------------------------------------------
// SearchConditionForm.h
//
// A form for specifying a search condition.
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

#ifndef SEARCH_CONDITION_FORM_H
#define SEARCH_CONDITION_FORM_H

#include "SearchCondition.h"
#include <QComboBox>
#include <QFrame>
#include <QSpinBox>
#include <QStackedWidget>
#include <set>

class SearchCondition;
class WordLineEdit;
class WordValidator;

class SearchConditionForm : public QFrame
{
    Q_OBJECT
    public:
    SearchConditionForm (QWidget* parent = 0, Qt::WFlags f = 0);

    SearchCondition getSearchCondition() const;
    void setSearchCondition (const SearchCondition& condition);
    bool isValid() const;

    signals:
    void returnPressed();
    void contentsChanged();

    public slots:
    void typeChanged (const QString&);
    void editListClicked();
    void reset();

    private:
    void setWordListString (const QString& string);
    bool matchStringIsValid (const QString& string) const;

    private:
    QComboBox*      typeCbox;
    QStackedWidget* paramStack;
    QFrame*         paramLineFrame;
    WordLineEdit*   paramLine;
    QFrame*         paramSboxFrame;
    QSpinBox*       paramSbox;
    QFrame*         paramCboxFrame;
    QComboBox*      paramCbox;
    QFrame*         paramConsistFrame;
    QSpinBox*       paramConsistSbox;
    WordLineEdit*   paramConsistLine;
    QFrame*         paramWordListFrame;
    QLineEdit*      paramWordListLine;
    QString         paramWordListString;
    WordValidator*  letterValidator;
    WordValidator*  patternValidator;
};

#endif // SEARCH_CONDITION_FORM_H
