//---------------------------------------------------------------------------
// NewQuizDialog.cpp
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "NewQuizDialog.h"
#include "Defs.h"

#include <qbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>

const QString DIALOG_CAPTION = "New Quiz";

using namespace Defs;

//---------------------------------------------------------------------------
// NewQuizDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
NewQuizDialog::NewQuizDialog (QWidget* parent, const char* name,
                              bool modal, WFlags f)
    : QDialog (parent, name, modal, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QButtonGroup* optionGroup = new QButtonGroup (3, QButtonGroup::Vertical,
                                                  "Quiz Type", this);
    Q_CHECK_PTR (optionGroup);
    optionGroup->setExclusive (true);
    mainVlay->addWidget (optionGroup);

    patternButton = new QRadioButton ("Pattern", optionGroup,
                                      "patternButton");
    Q_CHECK_PTR (patternButton);
    patternButton->setChecked (true);
    anagramButton = new QRadioButton ("Anagram", optionGroup,
                                      "anagramButton");
    Q_CHECK_PTR (anagramButton);
    subanagramButton = new QRadioButton ("Subanagram", optionGroup,
                                         "subanagramButton");
    Q_CHECK_PTR (subanagramButton);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QPushButton* okButton = new QPushButton ("OK", this, "okButton");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel", this,
                                                 "cancelButton");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setCaption (DIALOG_CAPTION);
}
