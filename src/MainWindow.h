//---------------------------------------------------------------------------
// MainWindow.h
//
// The main window for the word study application.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <qlabel.h>
#include <qmainwindow.h>
#include <qsettings.h>
#include <qtabwidget.h>

class JudgeForm;
class SearchForm;
class WordEngine;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    MainWindow (QWidget* parent = 0, const char* name = 0,
                WFlags f = WType_TopLevel);
    ~MainWindow();

  public slots:
    void import();

  private:
    void setNumWords (int num);
    void readSettings();
    void writeSettings() const;

  private:
    WordEngine* engine;
    QTabWidget* tabStack;
    JudgeForm*  judgeForm;
    SearchForm* searchForm;
    QLabel*     messageLabel;
    QLabel*     statusLabel;
    QSettings   settings;
};

#endif // MAIN_WINDOW_H
