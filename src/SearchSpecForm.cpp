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

const QString PATTERN_COMBO = "Pattern";
const QString ANAGRAM_COMBO = "Anagram";
const QString SUBANAGRAM_COMBO = "Subanagram";

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
    QHBoxLayout* mainHlay = new QHBoxLayout (this, 0, SPACING, "mainHlay");
    Q_CHECK_PTR (mainHlay);

//    QStringList types;
//    types << "Pattern Match" << "Anagram Match" << "Subanagram Match" <<
//        "Minimum Length" << "Maximum Length" << "Must Include" <<
//        "Must Exclude" << "Must Consist of" << "Must Belong to" <<
//        "Minumum Anagrams" << "Maximum Anagrams" << "Minimum Probability" <<
//        "Maximum Probability";
//    typeCbox = new QComboBox (this, "typeCbox");
//    Q_CHECK_PTR (typeCbox);
//    typeCbox->insertStringList (types);
//    mainHlay->addWidget (typeCbox);

    mainVlay = new QVBoxLayout (SPACING, "mainVlay");
    Q_CHECK_PTR (mainVlay);
    mainHlay->addLayout (mainVlay);

    addConditionForm();


//    QHBoxLayout* matchHlay = new QHBoxLayout (SPACING, "matchHlay");
//    Q_CHECK_PTR (matchHlay);
//    mainVlay->addLayout (matchHlay);
//
//    QLabel* matchLabel = new QLabel ("Match:", this, "matchLabel");
//    Q_CHECK_PTR (matchLabel);
//    matchHlay->addWidget (matchLabel);
//
//    matchCbox = new QComboBox (this, "matchCbox");
//    Q_CHECK_PTR (matchCbox);
//    matchCbox->insertItem (PATTERN_COMBO);
//    matchCbox->insertItem (ANAGRAM_COMBO);
//    matchCbox->insertItem (SUBANAGRAM_COMBO);
//    matchHlay->addWidget (matchCbox);
//
//    patternLine = new QLineEdit (this, "patternLine");
//    Q_CHECK_PTR (patternLine);
//    patternLine->setValidator (patternValidator);
//    connect (patternLine, SIGNAL (returnPressed()), SIGNAL (returnPressed()));
//    connect (patternLine, SIGNAL (textChanged (const QString&)),
//             SIGNAL (patternChanged (const QString&)));
//    matchHlay->addWidget (patternLine);
//
//    QHBoxLayout* lengthHlay = new QHBoxLayout (SPACING, "lengthHlay");
//    Q_CHECK_PTR (lengthHlay);
//    mainVlay->addLayout (lengthHlay);
//
//    QLabel* minLabel = new QLabel ("Min Length:", this, "minLabel");
//    Q_CHECK_PTR (minLabel);
//    lengthHlay->addWidget (minLabel);
//
//    minLengthSbox = new QSpinBox (this, "minLengthSbox");
//    Q_CHECK_PTR (minLengthSbox);
//    minLengthSbox->setMinValue (1);
//    minLengthSbox->setMaxValue (MAX_WORD_LEN);
//    connect (minLengthSbox, SIGNAL (valueChanged (int)),
//             SLOT (minLengthChanged (int)));
//    lengthHlay->addWidget (minLengthSbox);
//
//    QLabel* maxLabel = new QLabel ("Max Length:", this, "maxLabel");
//    Q_CHECK_PTR (maxLabel);
//    lengthHlay->addWidget (maxLabel);
//
//    maxLengthSbox = new QSpinBox (this, "maxLengthSbox");
//    Q_CHECK_PTR (maxLengthSbox);
//    maxLengthSbox->setMinValue (1);
//    maxLengthSbox->setMaxValue (MAX_WORD_LEN);
//    connect (maxLengthSbox, SIGNAL (valueChanged (int)),
//             SLOT (maxLengthChanged (int)));
//    lengthHlay->addWidget (maxLengthSbox);
//
//    QHBoxLayout* includeHlay = new QHBoxLayout (SPACING, "includeHlay");
//    Q_CHECK_PTR (includeHlay);
//    mainVlay->addLayout (includeHlay);
//
//    QLabel* includeLabel = new QLabel ("Must Include:", this, "includeLabel");
//    Q_CHECK_PTR (includeLabel);
//    includeHlay->addWidget (includeLabel);
//
//    includeLine = new QLineEdit (this, "includeLine");
//    Q_CHECK_PTR (includeLine);
//    includeLine->setValidator (letterValidator);
//    connect (includeLine, SIGNAL (returnPressed()), SIGNAL (returnPressed()));
//    includeHlay->addWidget (includeLine);
//
//    QHBoxLayout* excludeHlay = new QHBoxLayout (SPACING, "excludeHlay");
//    Q_CHECK_PTR (excludeHlay);
//    mainVlay->addLayout (excludeHlay);
//
//    QLabel* excludeLabel = new QLabel ("Must Exclude:", this, "excludeLabel");
//    Q_CHECK_PTR (excludeLabel);
//    excludeHlay->addWidget (excludeLabel);
//
//    excludeLine = new QLineEdit (this, "excludeLine");
//    Q_CHECK_PTR (excludeLine);
//    excludeLine->setValidator (letterValidator);
//    connect (excludeLine, SIGNAL (returnPressed()), SIGNAL (returnPressed()));
//    excludeHlay->addWidget (excludeLine);
//
//    QHBoxLayout* consistHlay = new QHBoxLayout (SPACING, "consistHlay");
//    mainVlay->addLayout (consistHlay);
//    Q_CHECK_PTR (consistHlay);
//
//    QLabel* consistLabel = new QLabel ("Must Consist of:", this,
//                                       "consistLabel");
//    Q_CHECK_PTR (consistLabel);
//    consistHlay->addWidget (consistLabel);
//
//    consistPctSbox = new QSpinBox (this, "consistPctSbox");
//    Q_CHECK_PTR (consistPctSbox);
//    consistPctSbox->setMinValue (0);
//    consistPctSbox->setMaxValue (100);
//    consistHlay->addWidget (consistPctSbox);
//
//    QLabel* consistPctLabel = new QLabel ("%", this, "consistPctLabel");
//    Q_CHECK_PTR (consistPctLabel);
//    consistHlay->addWidget (consistPctLabel);
//
//    consistLine = new QLineEdit (this, "consistLine");
//    Q_CHECK_PTR (consistLine);
//    consistLine->setValidator (letterValidator);
//    connect (consistLine, SIGNAL (returnPressed()), SIGNAL (returnPressed()));
//    consistHlay->addWidget (consistLine);
//
//    QLabel* setMembershipLabel = new QLabel ("Must belong to one of the "
//                                             "following sets:", this,
//                                             "setMembershipLabel");
//    Q_CHECK_PTR (setMembershipLabel);
//    mainVlay->addWidget (setMembershipLabel);
//
//    setMembershipList = new QListView (this, "setMembershipList");
//    Q_CHECK_PTR (setMembershipList);
//    setMembershipList->setSorting (-1);
//    setMembershipList->addColumn ("Set");
//    QListViewItem* item = 0;
//    item = new QListViewItem (setMembershipList, item, "Hook Words");
//    item = new QListViewItem (setMembershipList, item, "Type I Sevens");
//    //item = new QListViewItem (setMembershipList, item, "Type II Sevens");
//    //item = new QListViewItem (setMembershipList, item, "Type III Sevens");
//    item = new QListViewItem (setMembershipList, item, "Type I Eights");
//    item = new QListViewItem (setMembershipList, item,
//                              "Eights from top 7-letter stems");
//    setMembershipList->header()->hide();
//    // XXX: Fix this based on row height or font height
//    setMembershipList->setMaximumHeight (50);
//    setMembershipList->setSizePolicy (QSizePolicy::Preferred,
//                                      QSizePolicy::Fixed);
//    mainVlay->addWidget (setMembershipList);
//
//    reset();
}

//---------------------------------------------------------------------------
//  reset
//
//! Reset to defaults.
//---------------------------------------------------------------------------
void
SearchSpecForm::reset()
{
//    patternLine->setText ("");
//    includeLine->setText ("");
//    excludeLine->setText ("");
//    consistPctSbox->setValue (0);
//    consistLine->setText ("");
//    minLengthSbox->setValue (1);
//    maxLengthSbox->setValue (MAX_WORD_LEN);
//    setMembershipList->clearSelection();
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
    mainVlay->addWidget (form);
    conditionForms << form;
}
