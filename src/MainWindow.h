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
#include <qtabwidget.h>

class LookupForm;
class WordEngine;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    MainWindow (QWidget* parent = 0, const char* name = 0,
                WFlags f = WType_TopLevel);

  public slots:
    void import();

  private:
    void setNumWords (int num);

  private:
    WordEngine* engine;
    QTabWidget* tabStack;
    LookupForm* lookupForm;
    QLabel*     messageLabel;
    QLabel*     statusLabel;
};

#endif // MAIN_WINDOW_H
