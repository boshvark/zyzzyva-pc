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
#include <qpushbutton.h>

class NewQuizDialog;
class QuizEngine;

class QuizForm : public QFrame
{
  Q_OBJECT
  public:
    QuizForm (QuizEngine* e, QWidget* parent = 0, const char* name = 0,
              WFlags f = 0);

  public slots:
    void responseEntered();
    void newQuizClicked();
    void nextQuestionClicked();
    void checkResponseClicked();

  private:
    void updateForm (bool showStats);
    void updateStats();
    void clearStats();
    void setRecall (int correct, int total);
    void setPrecision (int correct, int total);

  private:
    QuizEngine*   engine;
    QLabel*       questionLabel;
    QLineEdit*    inputLine;
    QListBox*     responseList;
    QLabel*       recallLabel;
    QLabel*       precisionLabel;
    QLabel*       responseStatusLabel;
    QPushButton*  nextQuestionButton;

    NewQuizDialog* newQuizDialog;
};

#endif // QUIZ_FORM_H
