//---------------------------------------------------------------------------
// LookupForm.h
//
// A form for looking up words.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef LOOKUP_FORM_H
#define LOOKUP_FORM_H

#include <qlineedit.h>

class LookupForm : public QFrame
{
  Q_OBJECT
  public:
    LookupForm (QWidget* parent = 0, const char* name = 0,
                WFlags f = 0);

  private:
    QLineEdit* wordLine;
};

#endif // LOOKUP_FORM_H
