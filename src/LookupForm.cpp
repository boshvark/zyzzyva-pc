//---------------------------------------------------------------------------
// LookupForm.cpp
//
// A form for looking up words.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "LookupForm.h"
#include <qlabel.h>
#include <qlayout.h>

LookupForm::LookupForm (QWidget* parent, const char* name, WFlags f)
    : QFrame (parent, name, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, 0, "mainVlay");
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* lookupHlay = new QHBoxLayout (0, "lookupHlay");
    Q_CHECK_PTR (lookupHlay);
    mainVlay->addLayout (lookupHlay);

    QLabel* label = new QLabel ("Look Up : ", this, "label");
    Q_CHECK_PTR (label);
    lookupHlay->addWidget (label);

    wordLine = new QLineEdit (this, "wordLine");
    Q_CHECK_PTR (wordLine);
    lookupHlay->addWidget (wordLine);
}
