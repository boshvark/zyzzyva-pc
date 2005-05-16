//---------------------------------------------------------------------------
// WordVariationDialog.cpp
//
// A dialog for displaying variations of a word, such as hooks, anagrams, etc.
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

#include "WordVariationDialog.h"
#include "Defs.h"
#include "WordEngine.h"
#include "WordListView.h"
#include "WordListViewItem.h"
#include <qlayout.h>

using namespace Defs;

//---------------------------------------------------------------------------
//  WordVariationDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
WordVariationDialog::WordVariationDialog (WordEngine* we, const QString& word,
                                          WordVariationType variation,
                                          QWidget* parent, const char* name,
                                          bool modal, WFlags f)
    : QDialog (parent, name, modal, f), wordEngine (we)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* labelHlay = new QHBoxLayout (SPACING, "labelHlay");
    Q_CHECK_PTR (labelHlay);
    mainVlay->addLayout (labelHlay);

    labelHlay->addStretch (1);

    wordLabel = new QLabel (this, "wordLabel");
    Q_CHECK_PTR (wordLabel);
    labelHlay->addWidget (wordLabel);

    labelHlay->addStretch (1);

    QHBoxLayout* listHlay = new QHBoxLayout (SPACING, "listHlay");
    Q_CHECK_PTR (listHlay);
    mainVlay->addLayout (listHlay);

    frontList = new WordListView (wordEngine, this, "frontList");
    Q_CHECK_PTR (frontList);
    frontList->setResizeMode (QListView::LastColumn);
    frontList->addColumn ("Front Hooks");
    listHlay->addWidget (frontList);

    backList = new WordListView (wordEngine, this, "backList");
    Q_CHECK_PTR (backList);
    backList->setResizeMode (QListView::LastColumn);
    backList->addColumn ("Back Hooks");
    listHlay->addWidget (backList);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    closeButton = new QPushButton ("&Close", this, "closeButton");
    Q_CHECK_PTR (closeButton);
    closeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setAutoDefault (false);
    closeButton->setFocus();
    connect (closeButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (closeButton);

    setWordVariation (word, variation);
    resize (minimumSizeHint().width(), 500);
}

//---------------------------------------------------------------------------
//  ~WordVariationDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordVariationDialog::~WordVariationDialog()
{
}

//---------------------------------------------------------------------------
//  setWordVariation
//
//! Display the front and back hooks of a word.
//
//! @param word the word whose hooks are displayed
//! @param variation the variation of the word to display
//---------------------------------------------------------------------------
void
WordVariationDialog::setWordVariation (const QString& word, WordVariationType
                                       variation)
{
    QString title = "Hook words for: " + word;
    setCaption (title);
    wordLabel->setText (title);

    // Front hooks
    SearchSpec spec;
    spec.pattern = "?" + word;
    spec.type = Pattern;
    QStringList hooks = wordEngine->search (spec, true);
    QStringList::iterator it;
    for (it = hooks.begin(); it != hooks.end(); ++it)
        new WordListViewItem (frontList, *it);

    // Back hooks
    spec.pattern = word + "?";
    hooks = wordEngine->search (spec, true);
    for (it = hooks.begin(); it != hooks.end(); ++it)
        new WordListViewItem (backList, *it);
}
