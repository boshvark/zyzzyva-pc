//---------------------------------------------------------------------------
// SearchSpecForm.cpp
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

#include "SearchSpecForm.h"
#include "SearchSpec.h"
#include "SearchConditionForm.h"
#include "Auxil.h"
#include "Defs.h"
#include <qlayout.h>

using namespace Defs;

const int MAX_CONDITIONS = 8;

//---------------------------------------------------------------------------
//  SearchSpecForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchSpecForm::SearchSpecForm (QWidget* parent, const char* name, WFlags f)
    : QFrame (parent, name, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, SPACING, "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* conditionHlay = new QHBoxLayout (SPACING, "conditionHlay");
    Q_CHECK_PTR (conditionHlay);
    mainVlay->addLayout (conditionHlay);
    mainVlay->setStretchFactor (conditionHlay, 1);

    conditionVlay = new QVBoxLayout (SPACING, "conditionVlay");
    Q_CHECK_PTR (conditionVlay);
    conditionHlay->addLayout (conditionVlay);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    moreButton = new QPushButton ("&More", this, "moreButton");
    Q_CHECK_PTR (moreButton);
    moreButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (moreButton, SIGNAL (clicked()), SLOT (addConditionForm()));
    buttonHlay->addWidget (moreButton);

    fewerButton = new QPushButton ("&Fewer", this, "fewerButton");
    Q_CHECK_PTR (fewerButton);
    fewerButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (fewerButton, SIGNAL (clicked()), SLOT (removeConditionForm()));
    buttonHlay->addWidget (fewerButton);

    addConditionForm();
    fewerButton->setEnabled (false);
}

//---------------------------------------------------------------------------
//  getSearchSpec
//
//! Get a SearchSpec object corresponding to the values in the form.
//
//! @return the search spec
//---------------------------------------------------------------------------
SearchSpec
SearchSpecForm::getSearchSpec() const
{
    SearchSpec spec;
    spec.conjunction = true;
    QValueList<SearchConditionForm*>::const_iterator it;
    for (it = conditionForms.begin(); it != conditionForms.end(); ++it) {
        if ((*it)->isValid())
            spec.conditions << (*it)->getSearchCondition();
    }
    return spec;
}

//---------------------------------------------------------------------------
//  addConditionForm
//
//! Add a condition form to the bottom of the layout.
//---------------------------------------------------------------------------
void
SearchSpecForm::addConditionForm()
{
    SearchConditionForm* form = new SearchConditionForm (this, "form");
    Q_CHECK_PTR (form);
    conditionVlay->addWidget (form);
    conditionForms << form;
    form->show();

    fewerButton->setEnabled (true);
    if (conditionForms.size() == MAX_CONDITIONS)
        moreButton->setEnabled (false);
}

//---------------------------------------------------------------------------
//  removeConditionForm
//
//! Remove a condition form from the bottom of the layout.
//---------------------------------------------------------------------------
void
SearchSpecForm::removeConditionForm()
{
    if (conditionForms.size() <= 1)
        return;

    SearchConditionForm* form = conditionForms.last();
    conditionForms.pop_back();
    conditionVlay->remove (form);
    delete form;

    moreButton->setEnabled (true);
    if (conditionForms.size() == 1)
        fewerButton->setEnabled (false);
}
