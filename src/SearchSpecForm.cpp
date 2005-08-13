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
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qmessagebox.h>

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
    : QFrame (parent, name, f), visibleForms (0)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, SPACING, "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QButtonGroup* radioGroup = new QButtonGroup (this, "radioGroup");
    Q_CHECK_PTR (radioGroup);
    radioGroup->hide();

    QFrame* radioFrame = new QFrame (this, "radioFrame");
    Q_CHECK_PTR (radioFrame);
    mainVlay->addWidget (radioFrame);

    //QHBoxLayout* radioHlay = new QHBoxLayout (radioFrame, 0, SPACING, "radioHlay");
    //Q_CHECK_PTR (radioHlay);

    //conjunctionRadio = new QRadioButton ("Match all of the following",
    //                                     radioFrame, "conjunctionRadio");
    //Q_CHECK_PTR (conjunctionRadio);
    //conjunctionRadio->setChecked (true);
    //radioGroup->insert (conjunctionRadio, 1);
    //radioHlay->addWidget (conjunctionRadio);

    //QRadioButton* disjunctionRadio = new QRadioButton (
    //    "Match any of the following", radioFrame, "disjunctionRadio");
    //Q_CHECK_PTR (disjunctionRadio);
    //radioGroup->insert (disjunctionRadio, 1);
    //radioHlay->addWidget (disjunctionRadio);

    QHBoxLayout* conditionHlay = new QHBoxLayout (SPACING, "conditionHlay");
    Q_CHECK_PTR (conditionHlay);
    mainVlay->addLayout (conditionHlay);
    mainVlay->setStretchFactor (conditionHlay, 1);

    conditionVlay = new QVBoxLayout (SPACING, "conditionVlay");
    Q_CHECK_PTR (conditionVlay);
    conditionHlay->addLayout (conditionVlay);

    addConditionForms();

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    moreButton = new QPushButton ("&More", this, "moreButton");
    Q_CHECK_PTR (moreButton);
    moreButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (moreButton, SIGNAL (clicked()), SLOT (addConditionForm()));
    buttonHlay->addWidget (moreButton);

    fewerButton = new QPushButton ("Fe&wer", this, "fewerButton");
    Q_CHECK_PTR (fewerButton);
    fewerButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (fewerButton, SIGNAL (clicked()), SLOT (removeConditionForm()));
    buttonHlay->addWidget (fewerButton);

    buttonHlay->addStretch (1);

    loadButton = new QPushButton ("L&oad Search...", this, "loadButton");
    Q_CHECK_PTR (loadButton);
    loadButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (loadButton, SIGNAL (clicked()), SLOT (loadSearch()));
    buttonHlay->addWidget (loadButton);

    saveButton = new QPushButton ("S&ave Search...", this, "saveButton");
    Q_CHECK_PTR (saveButton);
    saveButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (saveButton, SIGNAL (clicked()), SLOT (saveSearch()));
    buttonHlay->addWidget (saveButton);

    connect (this, SIGNAL (contentsChanged()), SLOT (contentsChangedSlot()));
    fewerButton->setEnabled (false);
    saveButton->setEnabled (false);
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
    //spec.conjunction = conjunctionRadio->isChecked();
    int i = 0;
    QValueList<SearchConditionForm*>::const_iterator it;
    for (it = conditionForms.begin();
         (i < visibleForms) && (it != conditionForms.end()); ++it, ++i)
    {
        if ((*it)->isValid())
            spec.conditions << (*it)->getSearchCondition();
    }
    return spec;
}

//---------------------------------------------------------------------------
//  setSearchSpec
//
//! Set the contents of the form according to the contents of a search spec.
//
//! @param spec the search spec
//---------------------------------------------------------------------------
void
SearchSpecForm::setSearchSpec (const SearchSpec& spec)
{
    if (spec.conditions.empty())
        return;

    visibleForms = 0;
    //conjunctionRadio->setChecked (spec->conjunction);
    QValueList<SearchCondition>::const_iterator cit = spec.conditions.begin();
    QValueList<SearchConditionForm*>::iterator fit;
    for (fit = conditionForms.begin(); fit != conditionForms.end(); ++fit) {
        if (cit == spec.conditions.end())
            (*fit)->hide();
        else {
            (*fit)->setSearchCondition (*cit);
            (*fit)->show();
            ++cit;
            ++visibleForms;
        }
    }

    fewerButton->setEnabled (visibleForms > 1);
    moreButton->setEnabled (visibleForms < MAX_CONDITIONS);
}

//---------------------------------------------------------------------------
//  isValid
//
//! Determine whether the input in all visible search condition forms is
//! valid.
//
//! @return true if valid, false otherwise 
//---------------------------------------------------------------------------
bool
SearchSpecForm::isValid() const
{
    QValueList<SearchConditionForm*>::const_iterator it =
        conditionForms.begin();
    for (int i = 0; (i < visibleForms) && (it != conditionForms.end());
         ++i, ++it)
    {
        if (!(*it)->isValid())
            return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//  contentsChangedSlot
//
//! Called when the contents of the form change.  Enables or disables the Save
//! button appropriately.
//---------------------------------------------------------------------------
void
SearchSpecForm::contentsChangedSlot()
{
    saveButton->setEnabled (isValid());
}

//---------------------------------------------------------------------------
//  addConditionForm
//
//! Add a condition form to the bottom of the layout.
//---------------------------------------------------------------------------
void
SearchSpecForm::addConditionForm()
{
    SearchConditionForm* form = conditionForms[visibleForms];
    form->blockSignals (true);
    form->reset();
    form->show();
    form->blockSignals (false);
    ++visibleForms;

    fewerButton->setEnabled (visibleForms > 1);
    if (visibleForms == MAX_CONDITIONS)
        moreButton->setEnabled (false);

    contentsChanged();
}

//---------------------------------------------------------------------------
//  removeConditionForm
//
//! Remove a condition form from the bottom of the layout.
//---------------------------------------------------------------------------
void
SearchSpecForm::removeConditionForm()
{
    if (visibleForms <= 1)
        return;

    conditionForms[visibleForms - 1]->hide();
    --visibleForms;

    moreButton->setEnabled (true);
    if (visibleForms == 1)
        fewerButton->setEnabled (false);

    contentsChanged();
}

//---------------------------------------------------------------------------
//  loadSearch
//
//! Load a search spec from a file.
//---------------------------------------------------------------------------
void
SearchSpecForm::loadSearch()
{
    QString filename = QFileDialog::getOpenFileName
        (Auxil::getSearchDir(), "Zyzzyva Search Files (*.zzs)", this,
         "loadSearchDialog", "Load Search");
    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open (IO_ReadOnly | IO_Translate)) {
        QMessageBox::warning (this, "Error Opening Search File",
                              "Cannot open file '" + filename + "': " +
                              file.errorString());
        return;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QApplication::setOverrideCursor (Qt::waitCursor);
    QDomDocument document;
    bool success = document.setContent (&file, false, &errorMsg, &errorLine,
                                        &errorColumn);
    QApplication::restoreOverrideCursor();

    if (!success) {
        QMessageBox::warning (this, "Error in Search File",
                              "Error in search file, line " +
                              QString::number (errorLine) + ", column " +
                              QString::number (errorColumn) + ": " + 
                              errorMsg);
        return;
    }

    SearchSpec spec;
    if (!spec.fromDomElement (document.documentElement())) {
        QMessageBox::warning (this, "Error in Search File",
                              "Error in search file.");
        return;
    }

    setSearchSpec (spec);
    contentsChanged();
}

//---------------------------------------------------------------------------
//  saveSearch
//
//! Save the current search spec to a file.
//---------------------------------------------------------------------------
void
SearchSpecForm::saveSearch()
{
    QString filename = QFileDialog::getSaveFileName
        (Auxil::getSearchDir() + "/saved", "Zyzzyva Search Files (*.zzs)",
         this, "saveDialog", "Save Search");

    if (filename.isEmpty())
        return;

    if (!filename.endsWith (".zzs", false))
        filename += ".zzs";

    QFile file (filename);
    if (file.exists()) {
        int code = QMessageBox::warning (0, "Overwrite Existing File?",
                                         "The file already exists.  "
                                         "Overwrite it?", QMessageBox::Yes,
                                         QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    if (!file.open (IO_WriteOnly | IO_Translate)) {
        QMessageBox::warning (this, "Error Saving Search",
                              "Cannot save search:\n" + file.errorString() +
                              ".");
        return;
    }

    QDomImplementation implementation;
    QDomDocument document (implementation.createDocumentType
                           ("zyzzyva-search", QString::null,
                            "http://pietdepsi.com/dtd/zyzzyva-search.dtd"));

    document.appendChild (getSearchSpec().asDomElement());

    // XXX: There should be a programmatic way to write the <?xml?> header
    // based on the QDomImplementation, shouldn't there?
    QTextStream stream (&file);
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        << document.toString();
}

//---------------------------------------------------------------------------
//  addConditionForms
//
//! Add condition forms to the layout and hide all but one.
//---------------------------------------------------------------------------
void
SearchSpecForm::addConditionForms()
{
    for (int i = 0; i < MAX_CONDITIONS; ++i) {
        SearchConditionForm* form = new SearchConditionForm (this, "form");
        Q_CHECK_PTR (form);
        connect (form, SIGNAL (returnPressed()), SIGNAL (returnPressed()));
        connect (form, SIGNAL (contentsChanged()), SIGNAL (contentsChanged()));
        conditionVlay->addWidget (form);
        conditionForms << form;
        if (i)
            form->hide();
        else
            form->show();
    }
    visibleForms = 1;
}
