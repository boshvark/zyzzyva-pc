//---------------------------------------------------------------------------
// LexiconStyleDialog.cpp
//
// A dialog for selecting specialized display styles for words that belong to
// a particular combination of lexicons.
//
// Copyright 2008 Michael W Thelen <mthelen@gmail.com>.
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

#include "LexiconStyleDialog.h"
#include "LexiconStyle.h"
#include "LexiconStyleWidget.h"
#include "Defs.h"
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

const QString DIALOG_CAPTION = "Select Lexicon Styles";
const QString INSTRUCTION_TEXT =
    "Specify the styles to be used for displaying words that belong "
    "to certain combinations of lexicons.";

using namespace Defs;

//---------------------------------------------------------------------------
//  LexiconStyleDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
LexiconStyleDialog::LexiconStyleDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    Q_CHECK_PTR(mainVlay);
    mainVlay->setMargin(MARGIN);
    mainVlay->setSpacing(SPACING);

    QLabel* instructionLabel = new QLabel;
    Q_CHECK_PTR(instructionLabel);
    instructionLabel->setWordWrap(true);
    instructionLabel->setText(INSTRUCTION_TEXT);
    mainVlay->addWidget(instructionLabel);

    QHBoxLayout* symbolHlay = new QHBoxLayout;
    Q_CHECK_PTR(symbolHlay);
    symbolHlay->setSpacing(SPACING);
    mainVlay->addLayout(symbolHlay);
    mainVlay->setStretchFactor(symbolHlay, 1);

    symbolVlay = new QVBoxLayout;
    Q_CHECK_PTR(symbolVlay);
    symbolVlay->setSpacing(SPACING);
    symbolHlay->addLayout(symbolVlay);

    QHBoxLayout* addButtonHlay = new QHBoxLayout;
    Q_CHECK_PTR(addButtonHlay);
    mainVlay->addLayout(addButtonHlay);

    QPushButton* addButton = new QPushButton;
    Q_CHECK_PTR(addButton);
    addButton->setText("&Add");
    addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(addButton, SIGNAL(clicked()), SLOT(addButtonClicked()));
    addButtonHlay->addWidget(addButton);

    addButtonHlay->addStretch(1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox;
    Q_CHECK_PTR(buttonBox);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                  QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
    mainVlay->addWidget(buttonBox);

    deleteMapper = new QSignalMapper(this);
    Q_CHECK_PTR(deleteMapper);
    connect(deleteMapper, SIGNAL(mapped(int)), SLOT(removeStyleWidget(int)));

    setWindowTitle(DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  LexiconStyleDialog
//
//! Destructor.
//---------------------------------------------------------------------------
LexiconStyleDialog::~LexiconStyleDialog()
{
}

//---------------------------------------------------------------------------
//  getLexiconStyles
//
//! Return the list of lexicon styles.
//
//! @return a list of lexicon styles
//---------------------------------------------------------------------------
QList<LexiconStyle>
LexiconStyleDialog::getLexiconStyles() const
{
    QList<LexiconStyle> styles;
    QListIterator<LexiconStyleWidget*> it (styleWidgets);
    while (it.hasNext()) {
        LexiconStyleWidget* widget = it.next();
        LexiconStyle style = widget->getLexiconStyle();
        if (!style.isValid())
            continue;
        styles.append(style);
    }
    return styles;
}

//---------------------------------------------------------------------------
//  setLexiconStyles
//
//! Set the lexicon styles to be used.
//
//! @param styles the list of styles
//---------------------------------------------------------------------------
void
LexiconStyleDialog::setLexiconStyles(const QList<LexiconStyle>& styles)
{
    qDeleteAll(styleWidgets);
    styleWidgets.clear();

    QListIterator<LexiconStyle> it (styles);
    for (int index = 0; it.hasNext(); ++index) {
        const LexiconStyle& style = it.next();
        insertStyleWidget(index);
        LexiconStyleWidget* widget = styleWidgets[index];
        widget->setLexiconStyle(style);
    }
}

//---------------------------------------------------------------------------
//  addButtonClicked
//
//! Called when the More button is clicked.  Add a new style widget.
//---------------------------------------------------------------------------
void
LexiconStyleDialog::addButtonClicked()
{
    insertStyleWidget(styleWidgets.count());

    // FIXME: how the frack do I resize the dialog appropriately?
    // NewQuizDialog doesn't seem to have a problem.  What am I doing wrong?
    //show();
    //adjustSize();
    //resize(width(), minimumSize().height());
    //resize(baseSize());
}

//---------------------------------------------------------------------------
//  insertStyleWidget
//
//! Insert a new style widget at the specified index.
//
//! @param index the index to insert the new widget
//---------------------------------------------------------------------------
void
LexiconStyleDialog::insertStyleWidget(int index)
{
    LexiconStyleWidget* widget = new LexiconStyleWidget;
    Q_CHECK_PTR(widget);

    connect(widget, SIGNAL(deleteClicked()), deleteMapper, SLOT(map()));
    deleteMapper->setMapping(widget, index);

    symbolVlay->insertWidget(index, widget);
    styleWidgets.insert(index, widget);
}

//---------------------------------------------------------------------------
//  removeStyleWidget
//
//! Remove the style widget at the specified index.
//
//! @param index the index of the widget to remove
//---------------------------------------------------------------------------
void
LexiconStyleDialog::removeStyleWidget(int index)
{
    if (index >= styleWidgets.count())
        return;

    // Remap signals for any widgets after the one to be deleted
    for (int i = index + 1; i < styleWidgets.size(); ++i) {
        LexiconStyleWidget* widget = styleWidgets[i];
        deleteMapper->setMapping(widget, i - 1);
    }

    delete styleWidgets[index];
    styleWidgets.removeAt(index);
}
