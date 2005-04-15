//---------------------------------------------------------------------------
// MainWindow.cpp
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

#include "MainWindow.h"
#include "AboutDialog.h"
#include "DefineForm.h"
#include "HelpDialog.h"
#include "JudgeForm.h"
#include "QuizEngine.h"
#include "QuizForm.h"
#include "SearchForm.h"
#include "SettingsDialog.h"
#include "WordEngine.h"
#include "WordListViewItem.h"
#include "Auxil.h"
#include "Defs.h"
#include <qapplication.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qstatusbar.h>

const QString IMPORT_FAILURE_TITLE = "Load Failed";
const QString IMPORT_COMPLETE_TITLE = "Load Complete";
const QString DEFINE_TAB_TITLE = "Define";
const QString JUDGE_TAB_TITLE = "Judge";
const QString QUIZ_TAB_TITLE = "Quiz";
const QString SEARCH_TAB_TITLE = "Search";

const QString SETTINGS_MAIN = "/Zyzzyva";
const QString SETTINGS_GEOMETRY = "/geometry";
const QString SETTINGS_GEOMETRY_X = "/x";
const QString SETTINGS_GEOMETRY_Y = "/y";
const QString SETTINGS_GEOMETRY_WIDTH = "/width";
const QString SETTINGS_GEOMETRY_HEIGHT = "/height";

using namespace Defs;

//---------------------------------------------------------------------------
//  MainWindow
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
MainWindow::MainWindow (QWidget* parent, const char* name, WFlags f)
    : QMainWindow (parent, name, f), wordEngine (new WordEngine()),
      quizEngine (new QuizEngine (wordEngine)),
      settingsDialog (new SettingsDialog (this, "settingsDialog", true)),
      aboutDialog (new AboutDialog (this, "aboutDialog", true)),
      helpDialog (new HelpDialog (QString::null, this, "helpDialog"))
{
    QPopupMenu* filePopup = new QPopupMenu (this);
    Q_CHECK_PTR (filePopup);
    filePopup->insertItem ("&Open...", this, SLOT (importInteractive()),
                           CTRL+Key_O);
    filePopup->insertSeparator();
    filePopup->insertItem ("&Quit", qApp, SLOT (quit()));
    menuBar()->insertItem ("&File", filePopup);

    QPopupMenu* editPopup = new QPopupMenu (this);
    Q_CHECK_PTR (editPopup);
    editPopup->insertItem ("&Preferences", this, SLOT (editSettings()));
    menuBar()->insertItem ("&Edit", editPopup);

    QPopupMenu* helpPopup = new QPopupMenu (this);
    Q_CHECK_PTR (helpPopup);
    helpPopup->insertItem ("&Help", this, SLOT (displayHelp()));
    helpPopup->insertItem ("&About", this, SLOT (displayAbout()));
    menuBar()->insertItem ("&Help", helpPopup);

    tabStack = new QTabWidget (this, "tabStack");
    Q_CHECK_PTR (tabStack);
    setCentralWidget (tabStack);

    quizForm = new QuizForm (quizEngine, wordEngine, tabStack, "quizForm");
    Q_CHECK_PTR (quizForm);
    tabStack->addTab (quizForm, QUIZ_TAB_TITLE);

    searchForm = new SearchForm (wordEngine, tabStack, "searchForm");
    Q_CHECK_PTR (searchForm);
    tabStack->addTab (searchForm, SEARCH_TAB_TITLE);

    defineForm = new DefineForm (wordEngine, tabStack, "defineForm");
    Q_CHECK_PTR (defineForm);
    tabStack->addTab (defineForm, DEFINE_TAB_TITLE);

    judgeForm = new JudgeForm (wordEngine, tabStack, "judgeForm");
    Q_CHECK_PTR (judgeForm);
    tabStack->addTab (judgeForm, JUDGE_TAB_TITLE);

    messageLabel = new QLabel (this, "messageLabel");
    Q_CHECK_PTR (messageLabel);
    statusBar()->addWidget (messageLabel, 2);

    statusLabel = new QLabel (this, "statusLabel");
    Q_CHECK_PTR (statusLabel);
    statusBar()->addWidget (statusLabel, 1);
    setNumWords (0);

    readSettings (true);

    QString importFile = settingsDialog->getAutoImportFile();
    if (!importFile.isEmpty())
        import (importFile);
}

//---------------------------------------------------------------------------
//  ~MainWindow
//
//! Destructor.  Save application settings.
//---------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    writeSettings();
}

//---------------------------------------------------------------------------
//  importInteractive
//
//! Allow the user to import a word list from a file.
//---------------------------------------------------------------------------
void
MainWindow::importInteractive()
{
    QString file = QFileDialog::getOpenFileName (QDir::current().path(),
                                                 "All Files (*.*)", this,
                                                 "fileDialog",
                                                 IMPORT_CHOOSER_TITLE);
    if (file.isNull()) return;
    int imported = import (file);
    if (imported < 0) return;
    QMessageBox::information (this, IMPORT_COMPLETE_TITLE,
                              "Loaded " + QString::number (imported)
                              + " words.",
                              QMessageBox::Ok);
}

//---------------------------------------------------------------------------
//  editSettings
//
//! Allow the user to edit application settings.  If the user makes changes
//! and accepts the dialog, write the settings.  If the user rejects the
//! dialog, restore the settings after the dialog is closed.
//---------------------------------------------------------------------------
void
MainWindow::editSettings()
{
    settings.beginGroup (SETTINGS_MAIN);
    if (settingsDialog->exec() == QDialog::Accepted)
        settingsDialog->writeSettings (settings);
    else
        settingsDialog->readSettings (settings);
    settings.endGroup();
    readSettings (false);
}

//---------------------------------------------------------------------------
//  displayAbout
//
//! Display an About screen.
//---------------------------------------------------------------------------
void
MainWindow::displayAbout()
{
    aboutDialog->exec();
}

//---------------------------------------------------------------------------
//  displayHelp
//
//! Display a Help screen.
//---------------------------------------------------------------------------
void
MainWindow::displayHelp()
{
    helpDialog->showPage(Auxil::getHelpDir() + "/index.html");
}

//---------------------------------------------------------------------------
//  setNumWords
//
//! Update the label displaying the number of words loaded.
//
//! @param num the new number of words loaded
//---------------------------------------------------------------------------
void
MainWindow::setNumWords (int num)
{
    statusLabel->setText (QString::number (num) + " words loaded");
}

//---------------------------------------------------------------------------
//  readSettings
//
//! Read application settings.
//---------------------------------------------------------------------------
void
MainWindow::readSettings (bool useGeometry)
{
    settings.beginGroup (SETTINGS_MAIN);
    if (useGeometry) {
        settings.beginGroup (SETTINGS_GEOMETRY);
        int x = settings.readNumEntry (SETTINGS_GEOMETRY_X, 50);
        int y = settings.readNumEntry (SETTINGS_GEOMETRY_Y, 50);
        int w = settings.readNumEntry (SETTINGS_GEOMETRY_WIDTH, 640);
        int h = settings.readNumEntry (SETTINGS_GEOMETRY_HEIGHT, 480);
        settings.endGroup();
        setGeometry (x, y, w, h);
    }
    settingsDialog->readSettings (settings);
    settings.endGroup();

    // Main font
    QFont font;
    QString fontStr = settingsDialog->getMainFont();
    if (font.fromString (fontStr))
        qApp->setFont (font, true);
    else
        qWarning ("Cannot set font: " + fontStr);

    // Word list font
    fontStr = settingsDialog->getWordListFont();
    if (font.fromString (fontStr))
        qApp->setFont (font, true, "WordListView");
    else
        qWarning ("Cannot set font: " + fontStr);

    quizForm->setTileTheme (settingsDialog->getTileTheme());
    WordListViewItem::setSortByLength (settingsDialog->getSortByLength());
}

//---------------------------------------------------------------------------
//  writeSettings
//
//! Write application settings.
//---------------------------------------------------------------------------
void
MainWindow::writeSettings()
{
    settings.beginGroup (SETTINGS_MAIN);
    settings.beginGroup (SETTINGS_GEOMETRY);
    settings.writeEntry (SETTINGS_GEOMETRY_X, x());
    settings.writeEntry (SETTINGS_GEOMETRY_Y, y());
    settings.writeEntry (SETTINGS_GEOMETRY_WIDTH, width());
    settings.writeEntry (SETTINGS_GEOMETRY_HEIGHT, height());
    settings.endGroup();
    settingsDialog->writeSettings (settings);
    settings.endGroup();
}

//---------------------------------------------------------------------------
//  import
//
//! Import words from a file.
//
//! @return the number of imported words
//---------------------------------------------------------------------------
int
MainWindow::import (const QString& file)
{
    QString err;
    QApplication::setOverrideCursor (Qt::waitCursor);
    int imported = wordEngine->importFile (file, true, &err);
    QApplication::restoreOverrideCursor();

    if (imported < 0)
        QMessageBox::warning (this, IMPORT_FAILURE_TITLE, err);
    else
        setNumWords (imported);
    return imported;
}
