//---------------------------------------------------------------------------
// MainWindow.h
//
// The main window for the word study application.
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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <qlabel.h>
#include <qmainwindow.h>
#include <qsettings.h>
#include <qtabwidget.h>

class AboutDialog;
class DefineForm;
class HelpDialog;
class JudgeForm;
class QuizEngine;
class QuizForm;
class SearchForm;
class WordEngine;
class SettingsDialog;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    MainWindow (QWidget* parent = 0, const char* name = 0,
                WFlags f = WType_TopLevel);
    ~MainWindow();

  public slots:
    void importInteractive();
    void editSettings();
    void displayAbout();
    void displayHelp();

  private:
    int  import (const QString& file);
    void setNumWords (int num);
    void readSettings (bool useGeometry);
    void writeSettings();

  private:
    WordEngine* wordEngine;
    QuizEngine* quizEngine;
    QTabWidget* tabStack;
    DefineForm* defineForm;
    JudgeForm*  judgeForm;
    QuizForm*   quizForm;
    SearchForm* searchForm;
    QLabel*     messageLabel;
    QLabel*     statusLabel;

    QSettings       settings;
    SettingsDialog* settingsDialog;
    AboutDialog*    aboutDialog;
    HelpDialog*     helpDialog;
};

#endif // MAIN_WINDOW_H
