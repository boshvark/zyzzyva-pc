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
#include "DefinitionDialog.h"
#include "DefineForm.h"
#include "HelpDialog.h"
#include "JudgeForm.h"
#include "NewQuizDialog.h"
#include "QuizEngine.h"
#include "QuizForm.h"
#include "SearchForm.h"
#include "SettingsDialog.h"
#include "WordEngine.h"
#include "WordEntryDialog.h"
#include "WordListViewItem.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
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
      settingsDialog (new SettingsDialog (this, "settingsDialog", true)),
      aboutDialog (new AboutDialog (this, "aboutDialog", true)),
      helpDialog (new HelpDialog (QString::null, this, "helpDialog"))
{
    QPopupMenu* filePopup = new QPopupMenu (this);
    Q_CHECK_PTR (filePopup);
    filePopup->insertItem ("New Qui&z", this, SLOT (newQuizForm()));
    filePopup->insertItem ("New &Search", this, SLOT (newSearchForm()));
    filePopup->insertItem ("New &Definition", this, SLOT (newDefineForm()));
    filePopup->insertItem ("New Word &Judge", this, SLOT (newJudgeForm()));
    filePopup->insertSeparator();
    filePopup->insertItem ("&Open...", this, SLOT (importInteractive()),
                           CTRL+Key_O);
    filePopup->insertSeparator();
    filePopup->insertItem ("&Quit", qApp, SLOT (quit()));
    menuBar()->insertItem ("&File", filePopup);

    QPopupMenu* editPopup = new QPopupMenu (this);
    Q_CHECK_PTR (editPopup);
    editPopup->insertItem ("&Preferences", this, SLOT (editSettings()));
    menuBar()->insertItem ("&Edit", editPopup);

    QPopupMenu* viewPopup = new QPopupMenu (this);
    Q_CHECK_PTR (viewPopup);
    viewPopup->insertItem ("&Definition...", this, SLOT (viewDefinition()));
    viewPopup->insertItem ("&Anagrams...", this, SLOT (viewVariation (int)),
                           0, VariationAnagrams);
    viewPopup->insertItem ("&Subanagrams...", this, SLOT (viewVariation
                                                          (int)),
                           0, VariationSubanagrams);
    viewPopup->insertItem ("&Hooks...", this, SLOT (viewVariation (int)),
                           0, VariationHooks);
    viewPopup->insertItem ("&Extensions...", this, SLOT (viewVariation (int)),
                           0, VariationExtensions);
    viewPopup->insertItem ("Anagram Hoo&ks...", this, SLOT (viewVariation
                                                            (int)),
                           0, VariationAnagramHooks);
    viewPopup->insertItem ("&Blank Anagrams...", this, SLOT (viewVariation
                                                             (int)),
                           0, VariationBlankAnagrams);
    viewPopup->insertItem ("Blank &Matches...", this, SLOT (viewVariation
                                                            (int)),
                           0, VariationBlankMatches);
    viewPopup->insertItem ("&Transpositions...", this, SLOT (viewVariation
                                                             (int)),
                           0, VariationTranspositions);
    menuBar()->insertItem ("&View", viewPopup);

    QPopupMenu* helpPopup = new QPopupMenu (this);
    Q_CHECK_PTR (helpPopup);
    helpPopup->insertItem ("&Help", this, SLOT (displayHelp()));
    helpPopup->insertItem ("&About", this, SLOT (displayAbout()));
    menuBar()->insertItem ("&Help", helpPopup);

    tabStack = new QTabWidget (this, "tabStack");
    Q_CHECK_PTR (tabStack);

    closeButton = new QToolButton (tabStack, "closeButton");
    Q_CHECK_PTR (closeButton);
    closeButton->setUsesTextLabel (true);
    closeButton->setTextLabel ("X", false);
    tabStack->setCornerWidget (closeButton);
    closeButton->hide();
    connect (closeButton, SIGNAL (clicked()), SLOT (closeCurrentTab()));

    setCentralWidget (tabStack);

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

    importStems();
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
//  newQuizForm
//
//! Create a new quiz form.
//---------------------------------------------------------------------------
void
MainWindow::newQuizForm()
{
    NewQuizDialog* dialog = new NewQuizDialog (this, "newQuizDialog", true);
    Q_CHECK_PTR (dialog);
    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        QuizForm* form = new QuizForm (wordEngine, tabStack, "quizForm");
        Q_CHECK_PTR (form);
        form->newQuiz (dialog);
        form->setTileTheme (settingsDialog->getTileTheme());
        newTab (form, QUIZ_TAB_TITLE);
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  newSearchForm
//
//! Create a new search form.
//---------------------------------------------------------------------------
void
MainWindow::newSearchForm()
{
    SearchForm* form = new SearchForm (wordEngine, tabStack, "searchForm");
    Q_CHECK_PTR (form);
    newTab (form, SEARCH_TAB_TITLE);
}

//---------------------------------------------------------------------------
//  newDefineForm
//
//! Create a new word definition form.
//---------------------------------------------------------------------------
void
MainWindow::newDefineForm()
{
    DefineForm* form = new DefineForm (wordEngine, tabStack, "defineForm");
    Q_CHECK_PTR (form);
    newTab (form, DEFINE_TAB_TITLE);
}

//---------------------------------------------------------------------------
//  newJudgeForm
//
//! Create a new word judgment form.
//---------------------------------------------------------------------------
void
MainWindow::newJudgeForm()
{
    JudgeForm* form = new JudgeForm (wordEngine, tabStack, "judgeForm");
    Q_CHECK_PTR (form);
    newTab (form, JUDGE_TAB_TITLE);
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
//  viewDefinition
//
//! Allow the user to view the definition of a word.  Display a dialog asking
//! the user for the word.
//---------------------------------------------------------------------------
void
MainWindow::viewDefinition()
{
    WordEntryDialog* entryDialog = new WordEntryDialog (this,
                                                        "wordEntryDialog",
                                                        true);
    Q_CHECK_PTR (entryDialog);
    entryDialog->setCaption ("View Word Definition");
    int code = entryDialog->exec();
    QString word = entryDialog->getWord();
    delete entryDialog;
    if ((code != QDialog::Accepted) || word.isEmpty())
        return;

    DefinitionDialog* dialog = new DefinitionDialog (wordEngine, word, this,
                                                     "dialog", false,
                                                     Qt::WDestructiveClose);
    Q_CHECK_PTR (dialog);
    dialog->show();
}

//---------------------------------------------------------------------------
//  viewVariation
//
//! Allow the user to view variations of a word.  Display a dialog asking the
//! user for the word.
//---------------------------------------------------------------------------
void
MainWindow::viewVariation (int variation)
{
    QString caption;
    switch (variation) {
        case VariationAnagrams: caption = "View Anagrams"; break;
        case VariationSubanagrams: caption = "View Subanagrams"; break;
        case VariationHooks: caption = "View Hooks"; break;
        case VariationExtensions: caption = "View Extensions"; break;
        case VariationAnagramHooks: caption = "View Anagram Hooks"; break;
        case VariationBlankAnagrams: caption = "View Blank Anagrams"; break;
        case VariationBlankMatches: caption = "View Blank Matches"; break;
        case VariationTranspositions: caption = "View Transpositions"; break;
        default: break;
    }

    WordEntryDialog* entryDialog = new WordEntryDialog (this,
                                                        "wordEntryDialog",
                                                        true);
    Q_CHECK_PTR (entryDialog);
    entryDialog->setCaption (caption);
    int code = entryDialog->exec();
    QString word = entryDialog->getWord();
    delete entryDialog;
    if ((code != QDialog::Accepted) || word.isEmpty())
        return;

    WordVariationType type = static_cast<WordVariationType>(variation);
    WordVariationDialog* dialog = new WordVariationDialog (wordEngine, word,
                                                           type, this,
                                                           "dialog", false,
                                                           Qt::WDestructiveClose);
    Q_CHECK_PTR (dialog);
    dialog->show();
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
//  closeCurrentTab
//
//! Close the currently open tab.  If no other tabs exist, hide the button
//! used for closing tabs.
//---------------------------------------------------------------------------
void
MainWindow::closeCurrentTab()
{
    QWidget* w = tabStack->currentPage();
    tabStack->removePage (w);
    delete w;
    if (tabStack->count() == 0)
        closeButton->hide();
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
    QFont mainFont;
    QString fontStr = settingsDialog->getMainFont();
    bool mainFontOk = true;
    if (mainFont.fromString (fontStr))
        qApp->setFont (mainFont, true);
    else {
        qWarning ("Cannot set font: " + fontStr);
        mainFontOk = false;
    }

    // Word list font
    QFont font;
    fontStr = settingsDialog->getWordListFont();
    if (font.fromString (fontStr))
        qApp->setFont (font, true, "WordListView");
    else
        qWarning ("Cannot set font: " + fontStr);

    // Set word list headers back to main font
    if (mainFontOk)
        qApp->setFont (mainFont, true, "QHeader");

    // Quiz label font
    // XXX: Reinstate this once it's know how to change the font of canvas
    // text items via QApplication::setFont
    //fontStr = settingsDialog->getQuizLabelFont();
    //if (font.fromString (fontStr))
    //    // FIXME: How to get QCanvasText items to update their font?
    //    qApp->setFont (font, true, "QCanvasText");
    //else
    //    qWarning ("Cannot set font: " + fontStr);

    // Definition font
    fontStr = settingsDialog->getDefinitionFont();
    if (font.fromString (fontStr)) {
        qApp->setFont (font, true, "DefinitionBox");
        qApp->setFont (font, true, "DefinitionLabel");
    }
    else
        qWarning ("Cannot set font: " + fontStr);

    // Set tile theme for all quiz forms
    QString tileTheme = settingsDialog->getTileTheme();
    int count = tabStack->count();
    for (int i = 0; i < count; ++i) {
        QuizForm* quizForm = dynamic_cast<QuizForm*> (tabStack->page (i));
        if (quizForm)
            quizForm->setTileTheme (tileTheme);
    }

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
//  newTab
//
//! Create and display a new tab.
//
//! @param widget the widget to display
//! @param title the title of the tab
//---------------------------------------------------------------------------
void
MainWindow::newTab (QWidget* widget, const QString& title)
{
    tabStack->addTab (widget, title);
    tabStack->showPage (widget);
    closeButton->show();
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

//---------------------------------------------------------------------------
//  importStems
//
//! Import stems.  XXX: Right now this is hard-coded to load certain North
//! American stems.  Should be more flexible.
//
//! @return the number of imported stems
//---------------------------------------------------------------------------
int
MainWindow::importStems()
{
    QStringList stemFiles;
    stemFiles << Auxil::getWordsDir() + "/north-american/6-letter-stems.txt";
    stemFiles << Auxil::getWordsDir() + "/north-american/7-letter-stems.txt";

    QString err;
    QApplication::setOverrideCursor (Qt::waitCursor);
    QStringList::iterator it;
    int totalImported = 0;
    for (it = stemFiles.begin(); it != stemFiles.end(); ++it) {
        int imported = wordEngine->importStems (*it, &err);
        totalImported += imported;
    }
    QApplication::restoreOverrideCursor();

    return totalImported;
}
