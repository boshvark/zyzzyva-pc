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
#include <QApplication>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>

using namespace Defs;

const int MAX_CONDITIONS = 8;

//---------------------------------------------------------------------------
//  SearchSpecForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
SearchSpecForm::SearchSpecForm (QWidget* parent, Qt::WFlags f)
    : QFrame (parent, f), visibleForms (0)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, SPACING);
    Q_CHECK_PTR (mainVlay);

    //QButtonGroup* radioGroup = new QButtonGroup (this);
    //Q_CHECK_PTR (radioGroup);
    //radioGroup->hide();

    //QFrame* radioFrame = new QFrame;
    //Q_CHECK_PTR (radioFrame);
    //mainVlay->addWidget (radioFrame);

    //QHBoxLayout* radioHlay = new QHBoxLayout (radioFrame, 0, SPACING);
    //Q_CHECK_PTR (radioHlay);

    //conjunctionRadio = new QRadioButton ("Match all of the following");
    //Q_CHECK_PTR (conjunctionRadio);
    //conjunctionRadio->setChecked (true);
    //radioGroup->insert (conjunctionRadio, 1);
    //radioHlay->addWidget (conjunctionRadio);

    //QRadioButton* disjunctionRadio = new QRadioButton
    //    ("Match any of the following");
    //Q_CHECK_PTR (disjunctionRadio);
    //radioGroup->insert (disjunctionRadio, 1);
    //radioHlay->addWidget (disjunctionRadio);

    QHBoxLayout* conditionHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (conditionHlay);
    mainVlay->addLayout (conditionHlay);
    mainVlay->setStretchFactor (conditionHlay, 1);

    conditionVlay = new QVBoxLayout (SPACING);
    Q_CHECK_PTR (conditionVlay);
    conditionHlay->addLayout (conditionVlay);

    addConditionForms();

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    moreButton = new QPushButton ("&More");
    Q_CHECK_PTR (moreButton);
    moreButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (moreButton, SIGNAL (clicked()), SLOT (addConditionForm()));
    buttonHlay->addWidget (moreButton);

    fewerButton = new QPushButton ("Fe&wer");
    Q_CHECK_PTR (fewerButton);
    fewerButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (fewerButton, SIGNAL (clicked()), SLOT (removeConditionForm()));
    buttonHlay->addWidget (fewerButton);

    buttonHlay->addStretch (1);

    loadButton = new QPushButton ("L&oad Search...");
    Q_CHECK_PTR (loadButton);
    loadButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (loadButton, SIGNAL (clicked()), SLOT (loadSearch()));
    buttonHlay->addWidget (loadButton);

    saveButton = new QPushButton ("S&ave Search...");
    Q_CHECK_PTR (saveButton);
    saveButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (saveButton, SIGNAL (clicked()), SLOT (saveSearch()));
    buttonHlay->addWidget (saveButton);

    connect (this, SIGNAL (contentsChanged()), SLOT (contentsChangedSlot()));
    fewerButton->setEnabled (false);
    saveButton->setEnabled (false);

    setSizePolicy (QSizePolicy::Preferred, QSizePolicy::Minimum);
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
    QListIterator<SearchConditionForm*> it (conditionForms);
    while (it.hasNext() && (i < visibleForms)) {
        const SearchConditionForm* form = it.next();
        if (form->isValid())
            spec.conditions.append (form->getSearchCondition());
        ++i;
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
    QListIterator<SearchCondition> cit (spec.conditions);
    QMutableListIterator<SearchConditionForm*> fit (conditionForms);
    while (fit.hasNext()) {
        SearchConditionForm* form = fit.next();
        if (!cit.hasNext())
            form->hide();
        else {
            form->setSearchCondition (cit.next());
            form->show();
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
    QListIterator<SearchConditionForm*> it (conditionForms);
    int i = 0;
    while (it.hasNext() && (i < visibleForms)) {
        if (!(it.next()->isValid()))
            return false;
        ++i;
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
    QString filename = QFileDialog::getOpenFileName (this, "Load Search",
        Auxil::getSearchDir(), "Zyzzyva Search Files (*.zzs)");

    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
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
    QString filename = QFileDialog::getSaveFileName (this, "Save Search",
        Auxil::getSearchDir() + "/saved", "Zyzzyva Search Files (*.zzs)");

    if (filename.isEmpty())
        return;

    bool filenameEdited = false;
    if (!filename.endsWith (".zzs", false)) {
        filename += ".zzs";
        filenameEdited = true;
    }

    QFile file (filename);
    if (filenameEdited && file.exists()) {
        int code = QMessageBox::warning (0, "Overwrite Existing File?",
                                         "The file already exists.  "
                                         "Overwrite it?", QMessageBox::Yes,
                                         QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    if (!file.open (QIODevice::WriteOnly | QIODevice::Text)) {
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
        SearchConditionForm* form = new SearchConditionForm;
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
