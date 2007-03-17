//---------------------------------------------------------------------------
// MainWindow.h
//
// The main window for the word study application.
//
// Copyright 2004, 2005, 2006, 2007 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_MAIN_WINDOW_H
#define ZYZZYVA_MAIN_WINDOW_H

#include <QCloseEvent>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>
#include <QSplashScreen>
#include <QTabWidget>
#include <QToolButton>

class AboutDialog;
class ActionForm;
class HelpDialog;
class QuizSpec;
class QuizEngine;
class WordEngine;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
    MainWindow(QWidget* parent = 0, QSplashScreen* splash = 0,
               Qt::WFlags f = 0);
    ~MainWindow() { }

    static MainWindow* getInstance() { return instance; }

    public slots:
    void fileOpenRequested(const QString& filename);
    void processArguments(const QStringList& args);
    void tryAutoImport(QSplashScreen* splash);
    void importInteractive();
    void newQuizFormInteractive();
    void newQuizFormInteractive(const QuizSpec& quizSpec);
    void newQuizForm(const QuizSpec& quizSpec);
    void newSearchForm();
    void newCrosswordGameForm();
    void newDefineForm();
    void newIntroForm();
    void newCardboxForm();
    void doSaveAction();
    void doJudgeAction();
    void editSettings();
    void viewDefinition();
    void viewVariation(int variation);
    void rebuildDatabaseRequested();
    void rescheduleCardboxRequested();
    void displayAbout();
    void displayHelp();
    void displayLexiconError();
    void helpDialogError(const QString& message);
    void closeCurrentTab();
    void currentTabChanged(int index);
    void tabTitleChanged(const QString& title);
    void tabStatusChanged(const QString& status);
    void tabSaveEnabledChanged(bool saveEnabled);

    void doTest();

    public:
    QString getLexiconPrefix(const QString& lexicon);
    QString getDatabaseFilename();
    void connectToDatabase();
    // FIXME: these probably belong with WordTableView::addToCardbox in a
    // separate class for manipulating quiz databases.  Hm, how about the
    // QuizDatabase class?
    bool rebuildDatabase();
    int rescheduleCardbox(const QStringList& words, const QString& lexicon,
                          const QString& quizType, bool shiftQuestions,
                          int backlog = 0) const;

    protected:
    virtual void closeEvent(QCloseEvent* event);

    private:
    void makeUserDirs();
    void renameLexicon(const QString& oldName, const QString& newName);
    int  importText(const QString& file, const QString& lexiconName);
    bool importDawg(const QString& file, const QString& lexiconName,
                    bool reverse = false, QString* errString = 0,
                    quint16* expectedChecksum = 0);
    QList<quint16> importChecksums(const QString& file);
    int  importStems();
    void setNumWords(int num);
    void readSettings(bool useGeometry);
    void writeSettings();
    void newTab(ActionForm* form);
    void newQuizFromQuizFile(const QString& filename);
    void newQuizFromWordFile(const QString& filename);

    private:
    WordEngine*  wordEngine;
    QTabWidget*  tabStack;
    QToolButton* closeButton;
    QLabel*      messageLabel;
    QLabel*      lexiconLabel;

    QAction*     saveAction;

    SettingsDialog* settingsDialog;
    AboutDialog*    aboutDialog;
    HelpDialog*     helpDialog;

    QString lexiconError;

    static MainWindow*  instance;
};

#endif // ZYZZYVA_MAIN_WINDOW_H
