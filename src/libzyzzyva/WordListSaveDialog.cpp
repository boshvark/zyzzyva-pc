//---------------------------------------------------------------------------
// WordListSaveDialog.cpp
//
// The dialog for saving a list of words.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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

#include "WordListSaveDialog.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Save Word List";

using namespace Defs;

//---------------------------------------------------------------------------
//  WordListSaveDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
WordListSaveDialog::WordListSaveDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QHBoxLayout* formatHlay = new QHBoxLayout;
    Q_CHECK_PTR(formatHlay);
    formatHlay->setSpacing(SPACING);
    mainVlay->addLayout(formatHlay);

    QLabel* formatLabel = new QLabel("Format:");
    Q_CHECK_PTR(formatLabel);
    formatHlay->addWidget(formatLabel);

    formatCombo = new QComboBox(this);
    Q_CHECK_PTR(formatCombo);
    formatCombo->addItem(Auxil::wordListFormatToString(WordListOnePerLine));
    formatCombo->addItem(
        Auxil::wordListFormatToString(WordListAnagramQuestionAnswer));
    formatCombo->addItem(
        Auxil::wordListFormatToString(WordListAnagramTwoColumn));
    formatCombo->addItem(
        Auxil::wordListFormatToString(WordListDistinctAlphagrams));
    connect(formatCombo, SIGNAL(activated(const QString&)),
        SLOT(formatActivated(const QString&)));
    formatHlay->addWidget(formatCombo);

    QHBoxLayout* listHlay = new QHBoxLayout;
    Q_CHECK_PTR(listHlay);
    listHlay->setSpacing(SPACING);
    mainVlay->addLayout(listHlay);

    QVBoxLayout* unselectedVlay = new QVBoxLayout;
    Q_CHECK_PTR(unselectedVlay);
    unselectedVlay->setSpacing(SPACING);
    listHlay->addLayout(unselectedVlay);

    QLabel* unselectedLabel = new QLabel("Available attributes:");
    Q_CHECK_PTR(unselectedLabel);
    unselectedVlay->addWidget(unselectedLabel);

    unselectedAttrList = new QListWidget(this);
    Q_CHECK_PTR(unselectedAttrList);
    unselectedAttrList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    unselectedAttrList->setDragEnabled(true);
    unselectedAttrList->setAcceptDrops(true);
    unselectedAttrList->setDropIndicatorShown(true);
    unselectedAttrList->setDragDropMode(QAbstractItemView::InternalMove);
    connect(unselectedAttrList, SIGNAL(itemSelectionChanged()),
            SLOT(unselectedSelectionChanged()));
    unselectedVlay->addWidget(unselectedAttrList);

    QVBoxLayout* arrowVlay = new QVBoxLayout;
    Q_CHECK_PTR(arrowVlay);
    listHlay->addLayout(arrowVlay);

    arrowVlay->addStretch(1);

    selectButton = new QToolButton(this);
    Q_CHECK_PTR(selectButton);
    selectButton->setIcon(QIcon(":/right-arrow-icon"));
    connect(selectButton, SIGNAL(clicked()), SLOT(selectClicked()));
    arrowVlay->addWidget(selectButton);

    deselectButton = new QToolButton(this);
    Q_CHECK_PTR(deselectButton);
    deselectButton->setIcon(QIcon(":/left-arrow-icon"));
    connect(deselectButton, SIGNAL(clicked()), SLOT(deselectClicked()));
    arrowVlay->addWidget(deselectButton);

    arrowVlay->addStretch(1);

    QVBoxLayout* selectedVlay = new QVBoxLayout;
    Q_CHECK_PTR(selectedVlay);
    selectedVlay->setSpacing(SPACING);
    listHlay->addLayout(selectedVlay);

    QLabel* selectedLabel = new QLabel("Selected attributes:");
    Q_CHECK_PTR(selectedLabel);
    selectedVlay->addWidget(selectedLabel);

    selectedAttrList = new QListWidget(this);
    Q_CHECK_PTR(selectedAttrList);
    selectedAttrList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    selectedAttrList->setDragEnabled(true);
    selectedAttrList->setAcceptDrops(true);
    selectedAttrList->setDropIndicatorShown(true);
    selectedAttrList->setDragDropMode(QAbstractItemView::InternalMove);
    connect(selectedAttrList, SIGNAL(itemSelectionChanged()),
            SLOT(selectedSelectionChanged()));
    selectedVlay->addWidget(selectedAttrList);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR(buttonHlay);
    buttonHlay->setSpacing(SPACING);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    ZPushButton* okButton = new ZPushButton("&OK");
    Q_CHECK_PTR(okButton);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    Q_CHECK_PTR(cancelButton);
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    selectButton->setEnabled(false);
    deselectButton->setEnabled(false);
    initializeLists();
    setWindowTitle(DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  ~WordListSaveDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordListSaveDialog::~WordListSaveDialog()
{
}

//---------------------------------------------------------------------------
//  getSelectedAttributes
//
//! Get a list of selected word attributes in order.
//
//! @return the ordered list of selected attributes
//---------------------------------------------------------------------------
QList<WordAttribute>
WordListSaveDialog::getSelectedAttributes() const
{
    QList<WordAttribute> attributes;
    for (int i = 0; i < selectedAttrList->count(); ++i) {
        QString str = selectedAttrList->item(i)->text();
        WordAttribute attr = Auxil::stringToWordAttribute(str);
        if (attr != WordAttrInvalid)
            attributes.append(attr);
    }
    return attributes;
}

//---------------------------------------------------------------------------
//  getWordListFormat
//
//! Get the selected word list format.
//
//! @return the word list format
//---------------------------------------------------------------------------
WordListFormat
WordListSaveDialog::getWordListFormat() const
{
    return Auxil::stringToWordListFormat(formatCombo->currentText());
}

//---------------------------------------------------------------------------
//  formatActivated
//
//! Called when a format is activated in the dropdown.
//
//! @param format the activated format
//---------------------------------------------------------------------------
void
WordListSaveDialog::formatActivated(const QString& format)
{
    bool enable = (format !=
        Auxil::wordListFormatToString(WordListDistinctAlphagrams));

    unselectedAttrList->setEnabled(enable);
    selectedAttrList->setEnabled(enable);
    selectButton->setEnabled(enable);
    deselectButton->setEnabled(enable);
}

//---------------------------------------------------------------------------
//  selectClicked
//
//! Called when the select button is clicked.
//---------------------------------------------------------------------------
void
WordListSaveDialog::selectClicked()
{
    moveSelection(unselectedAttrList, selectedAttrList);
}

//---------------------------------------------------------------------------
//  deselectClicked
//
//! Called when the deselect button is clicked.
//---------------------------------------------------------------------------
void
WordListSaveDialog::deselectClicked()
{
    moveSelection(selectedAttrList, unselectedAttrList);
}

//---------------------------------------------------------------------------
//  unselectedSelectionChanged
//
//! Called when the selection in the unselected list changes.  Enable or
//! disable the select button as appropriate.
//---------------------------------------------------------------------------
void
WordListSaveDialog::unselectedSelectionChanged()
{
    selectButton->setEnabled(!unselectedAttrList->selectedItems().empty());
}

//---------------------------------------------------------------------------
//  selectedSelectionChanged
//
//! Called when the selection in the selected list changes.  Enable or
//! disable the deselect button as appropriate.
//---------------------------------------------------------------------------
void
WordListSaveDialog::selectedSelectionChanged()
{
    deselectButton->setEnabled(!selectedAttrList->selectedItems().empty());
}

//---------------------------------------------------------------------------
//  initializeLists
//
//! Initialize the lists of selected and unselected attributes.
//---------------------------------------------------------------------------
void
WordListSaveDialog::initializeLists()
{
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrDefinition));
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrFrontHooks));
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrBackHooks));
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrInnerHooks));
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrLexiconSymbols));
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrProbabilityOrder));
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrPlayabilityOrder));
    unselectedAttrList->addItem(
        Auxil::wordAttributeToString(WordAttrAlphagram));

    selectedAttrList->addItem(Auxil::wordAttributeToString(WordAttrWord));
}

//---------------------------------------------------------------------------
//  moveSelection
//
//! Move selected items from one list widget to another
//
//! @param src the source list widget
//! @param dest the destination list widget
//---------------------------------------------------------------------------
void
WordListSaveDialog::moveSelection(QListWidget* src, QListWidget* dest)
{
    // FIXME: use selectedItems??  May be much easier, if they're sorted.

    // Build a sorted list of selected rows
    QItemSelectionModel* selectionModel = src->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
    QListIterator<QModelIndex> it (selectedIndexes);
    QList<int> selectedRows;
    while (it.hasNext()) {
        selectedRows.append(it.next().row());
    }

    // Move rows in reverse sorted order so row numbers are not messed up
    qSort(selectedRows.begin(), selectedRows.end(), qGreater<int>());
    QListIterator<int> jt (selectedRows);
    int insertRow = dest->count();
    while (jt.hasNext()) {
        dest->insertItem(insertRow, src->takeItem(jt.next()));
    }
}
