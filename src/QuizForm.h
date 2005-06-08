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

#include "ActionForm.h"
#include "NewQuizDialog.h"
#include <qcanvas.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmap.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qstring.h>

class AnalyzeQuizDialog;
class DefinitionLabel;
class QuizEngine;
class WordEngine;
class WordListView;

class QuizForm : public ActionForm
{
  Q_OBJECT
  public:
    QuizForm (WordEngine* we, QWidget* parent = 0, const char* name = 0,
              WFlags f = 0);
    ~QuizForm();

  public slots:
    void newQuiz (NewQuizDialog* dialog);
    void responseEntered();
    void newQuizClicked();
    void nextQuestionClicked();
    void checkResponseClicked();
    void pauseClicked();
    void analyzeClicked();
    void setTileTheme (const QString& theme);

  private:
    void updateStats();
    void clearStats();
    void clearQuestionNum();
    void clearTimerDisplay();
    void startQuestion();
    void startNewTimer();
    void killActiveTimer();
    void pauseTimer();
    void unpauseTimer();
    void clearCanvas();
    void minimizeCanvas();
    void setNumCanvasTiles (int num);
    void setQuestionNum (int num, int total);
    void setQuestionLabel (const QString& question);
    void setQuestionStatus (int correct, int total);
    void setTimerDisplay (int seconds);
    void clearTileTheme();
    void reflowLayout();

    void timerEvent (QTimerEvent* event);

  private:
    WordEngine*   wordEngine;
    QuizEngine*   quizEngine;
    QLabel*       questionNumLabel;
    QLabel*       timerLabel;
    QWidget*      questionWidget;
    QCanvasView*  questionCanvasView;
    QCanvas*      questionCanvas;
    QLineEdit*    inputLine;
    WordListView* responseList;
    DefinitionLabel* responseStatusLabel;
    DefinitionLabel* questionStatusLabel;
    QPushButton*  nextQuestionButton;
    QPushButton*  checkResponseButton;
    QPushButton*  pauseButton;
    QPushButton*  analyzeButton;

    QMap<QString,QImage> tileImages;
    int maxTileWidth, maxTileHeight;
    int numCanvasTiles, minCanvasTiles, minCanvasWidth;
    bool useTimer;
    int timerId;
    int timerDuration, timerRemaining, timerPaused;
    QuizTimerType timerType;

    AnalyzeQuizDialog* analyzeDialog;
};

#endif // QUIZ_FORM_H
