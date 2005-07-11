//---------------------------------------------------------------------------
// SearchSpecForm.h
//
// A form for specifying a search specification.
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

#ifndef SEARCH_SPEC_FORM_H
#define SEARCH_SPEC_FORM_H

#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qvaluelist.h>
#include <set>

class SearchConditionForm;
class SearchSpec;

class SearchSpecForm : public QFrame
{
    Q_OBJECT
    public:
    SearchSpecForm (QWidget* parent = 0, const char* name = 0, WFlags f = 0);
    SearchSpec getSearchSpec() const;
    void setSearchSpec (const SearchSpec& spec);
    bool isValid() const;

    signals:
    void returnPressed();
    void contentsChanged();

    public slots:
    void contentsChangedSlot();
    void addConditionForm();
    void removeConditionForm();
    void loadSearch();
    void saveSearch();

    private:
    void addConditionForms();

    private:
    //QRadioButton* conjunctionRadio;
    int visibleForms;
    QPushButton* moreButton;
    QPushButton* fewerButton;
    QPushButton* loadButton;
    QPushButton* saveButton;
    QVBoxLayout* conditionVlay;
    QValueList<SearchConditionForm*> conditionForms;
};

#endif // SEARCH_SPEC_FORM_H
