//---------------------------------------------------------------------------
// SearchSpecForm.h
//
// A form for specifying a search specification.
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

#ifndef ZYZZYVA_SEARCH_SPEC_FORM_H
#define ZYZZYVA_SEARCH_SPEC_FORM_H

#include <QFrame>
#include <QList>
#include <QRadioButton>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <set>

class SearchConditionForm;
class SearchSpec;
class ZPushButton;

class SearchSpecForm : public QFrame
{
    Q_OBJECT
    public:
    SearchSpecForm(QWidget* parent = 0, Qt::WFlags f = 0);
    SearchSpec getSearchSpec() const;
    void setSearchSpec(const SearchSpec& spec);
    bool isValid() const;

    signals:
    void returnPressed();
    void contentsChanged();

    public slots:
    void contentsChangedSlot();
    void insertConditionForm(int index);
    void removeConditionForm(int index);
    void loadSearch();
    void saveSearch();
    void selectInputArea();

    private:
    //QRadioButton* conjunctionRadio;
    ZPushButton* moreButton;
    ZPushButton* fewerButton;
    ZPushButton* loadButton;
    ZPushButton* saveButton;
    QVBoxLayout* conditionVlay;
    QList<SearchConditionForm*> conditionForms;
    QSignalMapper* addMapper;
    QSignalMapper* deleteMapper;
};

#endif // ZYZZYVA_SEARCH_SPEC_FORM_H
