//---------------------------------------------------------------------------
// QuizForm.h
//
// A form for quizzing the user.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef QUIZ_FORM_H
#define QUIZ_FORM_H

#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
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
    void clearQuestionNum();
    void setQuestionNum (int num, int total);
    void setRecall (int correct, int total);
    void setPrecision (int correct, int total);
    void setTotalRecall (int correct, int total);
    void setTotalPrecision (int correct, int total);
    QString percentString (int numerator, int denominator) const;

  private:
    QuizEngine*   engine;
    QLabel*       questionNumLabel;
    QLabel*       questionLabel;
    QLineEdit*    inputLine;
    QListView*    responseList;
    QLabel*       recallLabel;
    QLabel*       precisionLabel;
    QLabel*       totalRecallLabel;
    QLabel*       totalPrecisionLabel;
    QLabel*       responseStatusLabel;
    QPushButton*  nextQuestionButton;
    QPushButton*  checkResponseButton;

    NewQuizDialog* newQuizDialog;
};

#endif // QUIZ_FORM_H
