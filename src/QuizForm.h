//---------------------------------------------------------------------------
// QuizForm.h
//
// A form for quizzing the user.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef QUIZ_FORM_H
#define QUIZ_FORM_H

#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>

class WordEngine;

class QuizForm : public QFrame
{
  Q_OBJECT
  public:
    QuizForm (WordEngine* e, QWidget* parent = 0, const char* name = 0,
              WFlags f = 0);

  private:
    WordEngine*   engine;
    QLabel*       quizLabel;
};

#endif // QUIZ_FORM_H
