//---------------------------------------------------------------------------
// NewQuizDialog.h
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef NEW_QUIZ_DIALOG_H
#define NEW_QUIZ_DIALOG_H

#include <qdialog.h>
#include <qradiobutton.h>

class NewQuizDialog : public QDialog
{
  Q_OBJECT
  public:
    NewQuizDialog (QWidget* parent = 0, const char* name = 0,
                   bool modal = false, WFlags f = 0);

    ~NewQuizDialog() { }

  private:
    QRadioButton* patternButton;
    QRadioButton* anagramButton;
    QRadioButton* subanagramButton;
};

#endif // NEW_QUIZ_DIALOG_H

