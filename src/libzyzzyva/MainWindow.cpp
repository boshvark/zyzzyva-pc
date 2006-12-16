//---------------------------------------------------------------------------
// MainWindow.cpp
//
// The main window for the word study application.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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
#include "CardboxRescheduleDialog.h"
#include "CreateDatabaseThread.h"
#include "CrosswordGameForm.h"
#include "DefinitionDialog.h"
#include "DefineForm.h"
#include "HelpDialog.h"
#include "IntroForm.h"
#include "JudgeDialog.h"
#include "MainSettings.h"
#include "NewQuizDialog.h"
#include "QuizEngine.h"
#include "QuizForm.h"
#include "SearchForm.h"
#include "SettingsDialog.h"
#include "WordEngine.h"
#include "WordEntryDialog.h"
#include "WordVariationDialog.h"
#include "WordVariationType.h"
#include "Auxil.h"
#include "Defs.h"
#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSignalMapper>
#include <QStatusBar>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QToolBar>

#include <QtDebug>
#include "LetterBag.h"

MainWindow* MainWindow::instance = 0;

const QString APPLICATION_TITLE = "Zyzzyva";

const QString IMPORT_FAILURE_TITLE = "Load Failed";
const QString IMPORT_COMPLETE_TITLE = "Load Complete";

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
//! @param f widget flags
//---------------------------------------------------------------------------
MainWindow::MainWindow (QWidget* parent, QSplashScreen* splash, Qt::WFlags f)
    : QMainWindow (parent, f), wordEngine (new WordEngine()),
      settingsDialog (new SettingsDialog (this)),
      aboutDialog (new AboutDialog (this)),
      helpDialog (new HelpDialog (QString::null, this))
{
    if (splash) {
        splash->showMessage ("Creating interface...",
                             Qt::AlignHCenter | Qt::AlignBottom);
        qApp->processEvents();
    }

    // File Menu
    QMenu* fileMenu = menuBar()->addMenu ("&File");
    Q_CHECK_PTR (fileMenu);

    // Test action (only for testing things out)
    QAction* testAction = new QAction ("&Test", this);
    Q_CHECK_PTR (testAction);
    connect (testAction, SIGNAL (triggered()), SLOT (doTest()));
    fileMenu->addAction (testAction);

    // Save
    saveAction = new QAction ("&Save", this);
    Q_CHECK_PTR (saveAction);
    saveAction->setIcon (QIcon (":/save-icon"));
    saveAction->setEnabled (false);
    saveAction->setShortcut (QString ("Ctrl+S"));
    connect (saveAction, SIGNAL (triggered()), SLOT (doSaveAction()));
    fileMenu->addAction (saveAction);

    fileMenu->addSeparator();

    // New Introduction
    QAction* newIntroAction = new QAction ("&Welcome", this);
    Q_CHECK_PTR (newIntroAction);
    newIntroAction->setIcon (QIcon (":/help-icon"));
    connect (newIntroAction, SIGNAL (triggered()), SLOT (newIntroForm()));
    fileMenu->addAction (newIntroAction);

    // New Quiz
    QAction* newQuizAction = new QAction ("Qui&z...", this);
    Q_CHECK_PTR (newQuizAction);
    newQuizAction->setIcon (QIcon (":/quiz-icon"));
    connect (newQuizAction, SIGNAL (triggered()),
             SLOT (newQuizFormInteractive()));
    fileMenu->addAction (newQuizAction);

    // New Search
    QAction* newSearchAction = new QAction ("&Search", this);
    Q_CHECK_PTR (newSearchAction);
    newSearchAction->setIcon (QIcon (":/search-icon"));
    connect (newSearchAction, SIGNAL (triggered()), SLOT (newSearchForm()));
    fileMenu->addAction (newSearchAction);

    // New Crossword Game
    QAction* newCrosswordGameAction = new QAction ("Crossword &Game", this);
    Q_CHECK_PTR (newCrosswordGameAction);
    newCrosswordGameAction->setIcon (QIcon (":/define-icon"));
    connect (newCrosswordGameAction, SIGNAL (triggered()),
             SLOT (newCrosswordGameForm()));
    fileMenu->addAction (newCrosswordGameAction);

    // New Definition
    QAction* newDefinitionAction = new QAction ("&Definition", this);
    Q_CHECK_PTR (newDefinitionAction);
    newDefinitionAction->setIcon (QIcon (":/define-icon"));
    connect (newDefinitionAction, SIGNAL (triggered()),
             SLOT (newDefineForm()));
    fileMenu->addAction (newDefinitionAction);

    // New Word Judge
    QAction* newJudgeAction = new QAction ("Word &Judge", this);
    Q_CHECK_PTR (newJudgeAction);
    newJudgeAction->setIcon (QIcon (":/judge-icon"));
    connect (newJudgeAction, SIGNAL (triggered()), SLOT (doJudgeAction()));
    fileMenu->addAction (newJudgeAction);

    fileMenu->addSeparator();

    // Close Table
    QAction* closeTabAction = new QAction ("&Close Tab", this);
    Q_CHECK_PTR (closeTabAction);
    closeTabAction->setShortcut (tr ("Ctrl+W"));
    connect (closeTabAction, SIGNAL (triggered()), SLOT (closeCurrentTab()));
    fileMenu->addAction (closeTabAction);

    // Quit
    QAction* quitAction = new QAction ("&Quit", this);
    Q_CHECK_PTR (quitAction);
    connect (quitAction, SIGNAL (triggered()), qApp, SLOT (quit()));
    fileMenu->addAction (quitAction);

    // Edit Menu
    QMenu* editMenu = menuBar()->addMenu ("&Edit");
    Q_CHECK_PTR (editMenu);

    // Preferences
    QAction* editPrefsAction = new QAction ("&Preferences", this);
    Q_CHECK_PTR (editPrefsAction);
    editPrefsAction->setIcon (QIcon (":/preferences-icon"));
    connect (editPrefsAction, SIGNAL (triggered()), SLOT (editSettings()));
    editMenu->addAction (editPrefsAction);

    // Word Menu
    QMenu* wordMenu = menuBar()->addMenu ("&Word");
    Q_CHECK_PTR (wordMenu);

    // Word Definition
    QAction* viewDefinitionAction = new QAction ("&Definition...", this);
    Q_CHECK_PTR (viewDefinitionAction);
    connect (viewDefinitionAction, SIGNAL (triggered()),
             SLOT (viewDefinition()));
    wordMenu->addAction (viewDefinitionAction);

    QSignalMapper* wordMapper = new QSignalMapper (this);
    Q_CHECK_PTR (wordMapper);

    // Word Anagrams
    QAction* viewAnagramsAction = new QAction ("&Anagrams...", this);
    Q_CHECK_PTR (viewAnagramsAction);
    connect (viewAnagramsAction, SIGNAL (triggered()),
             wordMapper, SLOT (map()));
    wordMapper->setMapping (viewAnagramsAction, VariationAnagrams);
    wordMenu->addAction (viewAnagramsAction);

    // Word Subanagrams
    QAction* viewSubanagramsAction = new QAction ("&Subanagrams...", this);
    Q_CHECK_PTR (viewSubanagramsAction);
    connect (viewSubanagramsAction, SIGNAL (triggered()),
             wordMapper, SLOT (map()));
    wordMapper->setMapping (viewSubanagramsAction, VariationSubanagrams);
    wordMenu->addAction (viewSubanagramsAction);

    // Word Hooks
    QAction* viewHooksAction = new QAction ("&Hooks...", this);
    Q_CHECK_PTR (viewHooksAction);
    connect (viewHooksAction, SIGNAL (triggered()), wordMapper, SLOT (map()));
    wordMapper->setMapping (viewHooksAction, VariationHooks);
    wordMenu->addAction (viewHooksAction);

    // Word Extensions
    QAction* viewExtensionsAction = new QAction ("&Extensions...", this);
    Q_CHECK_PTR (viewExtensionsAction);
    connect (viewExtensionsAction, SIGNAL (triggered()),
             wordMapper, SLOT (map()));
    wordMapper->setMapping (viewExtensionsAction, VariationExtensions);
    wordMenu->addAction (viewExtensionsAction);

    // Word Anagram Hooks
    QAction* viewAnagramHooksAction = new QAction ("Anagram Hoo&ks...", this);
    Q_CHECK_PTR (viewAnagramHooksAction);
    connect (viewAnagramHooksAction, SIGNAL (triggered()),
             wordMapper, SLOT (map()));
    wordMapper->setMapping (viewAnagramHooksAction, VariationAnagramHooks);
    wordMenu->addAction (viewAnagramHooksAction);

    // Word Blank Anagrams
    QAction* viewBlankAnagramsAction = new QAction ("&Blank Anagrams...",
                                                    this);
    Q_CHECK_PTR (viewBlankAnagramsAction);
    connect (viewBlankAnagramsAction, SIGNAL (triggered()),
             wordMapper, SLOT (map()));
    wordMapper->setMapping (viewBlankAnagramsAction, VariationBlankAnagrams);
    wordMenu->addAction (viewBlankAnagramsAction);

    // Word Blank Matches
    QAction* viewBlankMatchesAction = new QAction ("Blank &Matches...", this);
    Q_CHECK_PTR (viewBlankMatchesAction);
    connect (viewBlankMatchesAction, SIGNAL (triggered()),
             wordMapper, SLOT (map()));
    wordMapper->setMapping (viewBlankMatchesAction, VariationBlankMatches);
    wordMenu->addAction (viewBlankMatchesAction);

    // Word Transpositions
    QAction* viewTranspositionsAction = new QAction ("&Transpositions...",
                                                     this);
    Q_CHECK_PTR (viewTranspositionsAction);
    connect (viewTranspositionsAction, SIGNAL (triggered()),
             wordMapper, SLOT (map()));
    wordMapper->setMapping (viewTranspositionsAction,
                            VariationTranspositions);
    wordMenu->addAction (viewTranspositionsAction);

    // Connect Word signal mappings to viewVariation
    connect (wordMapper, SIGNAL (mapped (int)), SLOT (viewVariation (int)));

    // Tools Menu
    QMenu* toolsMenu = menuBar()->addMenu ("&Tools");
    Q_CHECK_PTR (toolsMenu);

    QAction* rebuildDatabaseAction = new QAction ("Rebuild &Database...",
                                                  this);
    Q_CHECK_PTR (rebuildDatabaseAction);
    connect (rebuildDatabaseAction, SIGNAL (triggered()),
             SLOT (rebuildDatabaseRequested()));
    toolsMenu->addAction (rebuildDatabaseAction);

    QAction* rescheduleCardboxAction = new QAction ("Reschedule &Cardbox "
                                                    "Contents...", this);
    Q_CHECK_PTR (rescheduleCardboxAction);
    connect (rescheduleCardboxAction, SIGNAL (triggered()),
             SLOT (rescheduleCardboxRequested()));
    toolsMenu->addAction (rescheduleCardboxAction);

    // Help Menu
    QMenu* helpMenu = menuBar()->addMenu ("&Help");
    Q_CHECK_PTR (helpMenu);

    // Help
    QAction* helpAction = new QAction ("&Help", this);
    Q_CHECK_PTR (helpAction);
    helpAction->setIcon (QIcon (":/help-icon"));
    connect (helpAction, SIGNAL (triggered()), SLOT (displayHelp()));
    helpMenu->addAction (helpAction);

    // About
    QAction* aboutAction = new QAction ("&About", this);
    Q_CHECK_PTR (aboutAction);
    connect (aboutAction, SIGNAL (triggered()), SLOT (displayAbout()));
    helpMenu->addAction (aboutAction);

    // Tool Bar
    QToolBar* toolbar = new QToolBar;
    Q_CHECK_PTR (toolbar);
    toolbar->setIconSize (QSize (22, 22));
    toolbar->addAction (saveAction);
    toolbar->addSeparator();
    toolbar->addAction (newQuizAction);
    toolbar->addAction (newSearchAction);
    toolbar->addAction (newDefinitionAction);
    toolbar->addAction (newJudgeAction);
    toolbar->addSeparator();
    toolbar->addAction (editPrefsAction);
    toolbar->addAction (helpAction);
    addToolBar (toolbar);

    tabStack = new QTabWidget (this);
    Q_CHECK_PTR (tabStack);
    connect (tabStack, SIGNAL (currentChanged (int)),
             SLOT (currentTabChanged (int)));

    closeButton = new QToolButton (tabStack);
    Q_CHECK_PTR (closeButton);
    closeButton->setIcon (QIcon (":/close-tab-icon"));
    tabStack->setCornerWidget (closeButton);
    closeButton->hide();
    connect (closeButton, SIGNAL (clicked()), SLOT (closeCurrentTab()));

    setCentralWidget (tabStack);

    messageLabel = new QLabel;
    Q_CHECK_PTR (messageLabel);
    statusBar()->addWidget (messageLabel, 2);

    lexiconLabel = new QLabel;
    Q_CHECK_PTR (lexiconLabel);
    statusBar()->addWidget (lexiconLabel, 1);

    if (splash) {
        splash->showMessage ("Creating data files...",
                             Qt::AlignHCenter | Qt::AlignBottom);
        qApp->processEvents();
    }

    readSettings (true);

    makeUserDirs();

    setWindowTitle (APPLICATION_TITLE);
    setWindowIcon (QIcon (":/zyzzyva-32x32"));

    tryAutoImport (splash);
    setNumWords (0);

    if (!instance)
        instance = this;

    if (MainSettings::getDisplayWelcome())
        newIntroForm();

    connect (helpDialog, SIGNAL (error (const QString&)),
             SLOT (helpDialogError (const QString&)));

    QTimer::singleShot (0, this, SLOT (displayLexiconError()));
}

//---------------------------------------------------------------------------
//  fileOpenRequested
//
//! Called when a request is made to open a file.  Open the file and start
//! a quiz from it, if possible.
//
//! @param filename the filename
//---------------------------------------------------------------------------
void
MainWindow::fileOpenRequested (const QString& filename)
{
    QApplication::setOverrideCursor (Qt::WaitCursor);

    if (filename.endsWith (".zzq"))
        newQuizFromQuizFile (filename);
    else
        newQuizFromWordFile (filename);

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  processArguments
//
//! Process command-line arguments.
//
//! @param args a list of arguments
//---------------------------------------------------------------------------
void
MainWindow::processArguments (const QStringList& args)
{
    QString arg;
    foreach (arg, args)
        fileOpenRequested (arg);
}

//---------------------------------------------------------------------------
//  tryAutoImport
//
//! Try automatically importing a lexicon, if the user has enabled it in
//! preferences.
//---------------------------------------------------------------------------
void
MainWindow::tryAutoImport (QSplashScreen* splash)
{
    if (!MainSettings::getUseAutoImport())
        return;

    // FIXME: This should not be part of the MainWindow class.  Lexicons (and
    // mapping lexicons to actual files) should be handled by someone else.
    QString lexicon = MainSettings::getAutoImportLexicon();
    QString importFile;
    QString reverseImportFile;
    QString definitionFile;
    QString numWordsFile;
    QString numAnagramsFile;
    QString checksumFile;
    bool dawg = true;
    if (lexicon == "Custom") {
        importFile = MainSettings::getAutoImportFile();
        dawg = false;
    }
    else {
        QMap<QString, QString> prefixMap;
        prefixMap["OWL+LWL"] = "/north-american/owl-lwl";
        prefixMap["OWL2+LWL"] = "/north-american/owl2-lwl";
        prefixMap["OSPD4+LWL"] = "/north-american/ospd4-lwl";
        prefixMap["OSWI"] = "/british/oswi";
        prefixMap["ODS"] = "/french/ods4";

        if (prefixMap.contains (lexicon)) {
            QString prefix = Auxil::getWordsDir() + prefixMap.value (lexicon);
            importFile =        prefix + ".dwg";
            reverseImportFile = prefix + "-r.dwg";
            checksumFile =      prefix + "-checksums.txt";
        }
    }

    if (importFile.isEmpty())
        return;

    QString splashMessage = "Loading " + lexicon + " lexicon...";
    if (splash) {
        splash->showMessage (splashMessage,
                             Qt::AlignHCenter | Qt::AlignBottom);
        qApp->processEvents();
    }

    if (dawg) {
        quint16 expectedForwardChecksum = 0;
        quint16 expectedReverseChecksum = 0;

        QList<quint16> checksums = importChecksums (checksumFile);
        if (checksums.size() >= 2) {
            expectedForwardChecksum = checksums[0];
            expectedReverseChecksum = checksums[1];
        }
        else {
            // warning!
        }

        lexiconError = QString::null;

        importDawg (importFile, lexicon, false, &lexiconError,
                    &expectedForwardChecksum);
        importDawg (reverseImportFile, lexicon, true, &lexiconError,
                    &expectedReverseChecksum);
    }
    else
        importText (importFile, lexicon);

    if (splash) {
        splash->showMessage ("Loading stems...",
                             Qt::AlignHCenter | Qt::AlignBottom);
        qApp->processEvents();
    }

    importStems();
}

//---------------------------------------------------------------------------
//  importInteractive
//
//! Allow the user to import a word list from a file.
//---------------------------------------------------------------------------
void
MainWindow::importInteractive()
{
    QString file = QFileDialog::getOpenFileName (this, IMPORT_CHOOSER_TITLE,
        QDir::current().path(), "All Files (*.*)");

    if (file.isNull()) return;
    int imported = importText (file, "Custom");
    if (imported < 0) return;
    QString caption = IMPORT_COMPLETE_TITLE;
    QString message = "Loaded " + QString::number (imported) + " words.";
    message = Auxil::dialogWordWrap (message);
    QMessageBox::information (this, caption, message, QMessageBox::Ok);
}

//---------------------------------------------------------------------------
//  newQuizFormInteractive
//
//! Create a new quiz form interactively.
//---------------------------------------------------------------------------
void
MainWindow::newQuizFormInteractive()
{
    NewQuizDialog* dialog = new NewQuizDialog (wordEngine, this);
    Q_CHECK_PTR (dialog);
    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        newQuizForm (dialog->getQuizSpec());
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  newQuizFormInteractive
//
//! Create a new quiz form interactively, with the new quiz dialog initialized
//! from a quiz specification.
//---------------------------------------------------------------------------
void
MainWindow::newQuizFormInteractive (const QuizSpec& quizSpec)
{
    NewQuizDialog* dialog = new NewQuizDialog (wordEngine, this);
    Q_CHECK_PTR (dialog);
    dialog->setQuizSpec (quizSpec);
    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        newQuizForm (dialog->getQuizSpec());
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  newQuizForm
//
//! Create a new quiz form directly from a quiz specification without
//! presenting the user with a quiz spec dialog.
//
//! @param quizSpec the quiz specification
//---------------------------------------------------------------------------
void
MainWindow::newQuizForm (const QuizSpec& quizSpec)
{
    QuizForm* form = new QuizForm (wordEngine);
    Q_CHECK_PTR (form);
    form->setTileTheme (MainSettings::getTileTheme());
    bool ok = form->newQuiz (quizSpec);
    if (!ok) {
        delete form;
        return;
    }
    newTab (form);
}

//---------------------------------------------------------------------------
//  newSearchForm
//
//! Create a new search form.
//---------------------------------------------------------------------------
void
MainWindow::newSearchForm()
{
    SearchForm* form = new SearchForm (wordEngine);
    Q_CHECK_PTR (form);
    newTab (form);
}

//---------------------------------------------------------------------------
//  newCrosswordGameForm
//
//! Create a crossword game form.
//---------------------------------------------------------------------------
void
MainWindow::newCrosswordGameForm()
{
    CrosswordGameForm* form = new CrosswordGameForm;
    Q_CHECK_PTR (form);
    newTab (form);
}

//---------------------------------------------------------------------------
//  newDefineForm
//
//! Create a new word definition form.
//---------------------------------------------------------------------------
void
MainWindow::newDefineForm()
{
    DefineForm* form = new DefineForm (wordEngine);
    Q_CHECK_PTR (form);
    newTab (form);
}

//---------------------------------------------------------------------------
//  newIntroForm
//
//! Create a new introduction form.
//---------------------------------------------------------------------------
void
MainWindow::newIntroForm()
{
    IntroForm* form = new IntroForm;
    Q_CHECK_PTR (form);
    newTab (form);
}

//---------------------------------------------------------------------------
//  doSaveAction
//
//! Open a save dialog for the current tab.
//---------------------------------------------------------------------------
void
MainWindow::doSaveAction()
{
    QWidget* w = tabStack->currentWidget();
    if (!w)
        return;

    // Prompt to save changes if this is a Quiz tab
    ActionForm* form = static_cast<ActionForm*>(w);
    form->saveRequested();
}

//---------------------------------------------------------------------------
//  doJudgeAction
//
//! Open a word judge window.
//---------------------------------------------------------------------------
void
MainWindow::doJudgeAction()
{
    QString caption = "Entering Full Screen Word Judge";
    QString message = "You are now entering the full screen Word Judge mode.  "
                      "To exit full screen mode, press ESC while holding the "
                      "Shift key.";
    message = Auxil::dialogWordWrap (message);
    int code = QMessageBox::information (this, caption, message,
                                         QMessageBox::Ok | QMessageBox::Cancel,
                                         QMessageBox::Ok);

    if (code != QMessageBox::Ok)
        return;

    QApplication::setOverrideCursor (Qt::BlankCursor);
    JudgeDialog* dialog = new JudgeDialog (wordEngine, this);
    Q_CHECK_PTR (dialog);
    dialog->exec();
    delete dialog;
    QApplication::restoreOverrideCursor();
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
    QString lexicon;
    if (settingsDialog->exec() == QDialog::Accepted) {
        lexicon = MainSettings::getAutoImportLexicon();
        settingsDialog->writeSettings();
    }
    else {
        settingsDialog->readSettings();
    }
    readSettings (false);

    if (!lexicon.isEmpty() &&
        (lexicon != MainSettings::getAutoImportLexicon()))
    {
        QString caption = "Zyzzyva Restart Necessary";
        QString message = "Your lexicon settings have changed.  For the new "
            "settings to take effect, please restart Zyzzyva.";
        message = Auxil::dialogWordWrap (message);
        QMessageBox::information (this, caption, message);
    }
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
    WordEntryDialog* entryDialog = new WordEntryDialog (this);
    Q_CHECK_PTR (entryDialog);
    entryDialog->setWindowTitle ("Word Word Definition");
    entryDialog->resize (entryDialog->minimumSizeHint().width() * 2,
                         entryDialog->minimumSizeHint().height());
    int code = entryDialog->exec();
    QString word = entryDialog->getWord();
    delete entryDialog;
    if ((code != QDialog::Accepted) || word.isEmpty())
        return;

    DefinitionDialog* dialog = new DefinitionDialog (wordEngine, word, this);
    Q_CHECK_PTR (dialog);
    dialog->setAttribute (Qt::WA_DeleteOnClose);
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
        case VariationAnagrams: caption = "Word Anagrams"; break;
        case VariationSubanagrams: caption = "Word Subanagrams"; break;
        case VariationHooks: caption = "Word Hooks"; break;
        case VariationExtensions: caption = "Word Extensions"; break;
        case VariationAnagramHooks: caption = "Word Anagram Hooks"; break;
        case VariationBlankAnagrams: caption = "Word Blank Anagrams"; break;
        case VariationBlankMatches: caption = "Word Blank Matches"; break;
        case VariationTranspositions: caption = "Word Transpositions"; break;
        default: break;
    }

    WordEntryDialog* entryDialog = new WordEntryDialog (this);
    Q_CHECK_PTR (entryDialog);
    entryDialog->setWindowTitle (caption);
    entryDialog->resize (entryDialog->minimumSizeHint().width() * 2,
                         entryDialog->minimumSizeHint().height());
    int code = entryDialog->exec();
    QString word = entryDialog->getWord();
    delete entryDialog;
    if ((code != QDialog::Accepted) || word.isEmpty())
        return;

    WordVariationType type = static_cast<WordVariationType>(variation);
    WordVariationDialog* dialog = new WordVariationDialog (wordEngine, word,
                                                           type, this);
    Q_CHECK_PTR (dialog);
    dialog->setAttribute (Qt::WA_DeleteOnClose);
    dialog->show();
}

//---------------------------------------------------------------------------
//  rebuildDatabaseRequested
//
//! Called when the user requests a database rebuild.
//---------------------------------------------------------------------------
void
MainWindow::rebuildDatabaseRequested()
{
    QString lexicon = wordEngine->getLexiconName();
    QString caption = "Rebuild Database";
    QString message = "You have requested that Zyzzyva rebuild its database "
        "for the current lexicon (" + lexicon + ").  This may take several "
        "minutes.\n\nRebuild the database now?";
    message = Auxil::dialogWordWrap (message);

    int code = QMessageBox::question (this, caption, message,
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::Yes);
    if (code != QMessageBox::Yes) {
        return;
    }

    rebuildDatabase();
    connectToDatabase();
}

//---------------------------------------------------------------------------
//  rescheduleCardboxRequested
//
//! Called when the user requests to reschedule cardbox items.
//---------------------------------------------------------------------------
void
MainWindow::rescheduleCardboxRequested()
{
    CardboxRescheduleDialog* dialog = new CardboxRescheduleDialog (this);
    Q_CHECK_PTR (dialog);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        QString quizType = dialog->getQuizType();
        bool rescheduleAll = dialog->getRescheduleAll();
        SearchSpec searchSpec = dialog->getSearchSpec();
        QString lexicon = wordEngine->getLexiconName();

        QStringList words;
        if (!rescheduleAll) {
            words = wordEngine->search (searchSpec, true);
            if (words.isEmpty())
                return;
        }

        QApplication::setOverrideCursor (Qt::WaitCursor);
        int numRescheduled = rescheduleCardbox (words, lexicon, quizType);
        QApplication::restoreOverrideCursor();

        QString questionStr = numRescheduled == 1 ? QString ("question")
                                                  : QString ("questions");
        QString caption = "Cardbox Questions Rescheduled";
        QString message = QString::number (numRescheduled) + " " +
            questionStr + " rescheduled.";
        message = Auxil::dialogWordWrap (message);
        QMessageBox::information (this, caption, message);
    }

    delete dialog;
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
//  displayLexiconError
//
//! Display any lexicon errors, and ask the user whether to proceed in the
//! face of any errors.
//---------------------------------------------------------------------------
void
MainWindow::displayLexiconError()
{
    if (lexiconError.isEmpty())
        return;

    QString caption = "Lexicon Warning";
    QString message = lexiconError + "\n\nProceed anyway?";
    message = Auxil::dialogWordWrap (message);
    int code = QMessageBox::warning (this, caption, message,
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if (code != QMessageBox::Yes)
        qApp->quit();
}

//---------------------------------------------------------------------------
//  helpDialogError
//
//! Called when an error occurs in the help dialog.
//
//! @param message the error message
//---------------------------------------------------------------------------
void
MainWindow::helpDialogError (const QString& message)
{
    QString caption = "Help Display Error";
    QMessageBox::warning (this, caption, Auxil::dialogWordWrap (message));
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
    QWidget* w = tabStack->currentWidget();
    if (!w)
        return;

    // Prompt to save changes if this is a Quiz tab
    ActionForm* form = static_cast<ActionForm*>(w);
    ActionForm::ActionFormType type = form->getType();
    if (type == ActionForm::QuizFormType) {
        QuizForm* quizForm = static_cast<QuizForm*> (form);
        if (quizForm->isSaveEnabled()) {
            bool ok = quizForm->promptToSaveChanges();
            if (!ok)
                return;
        }
    }

    tabStack->removeTab (tabStack->indexOf (w));
    delete w;
    if (tabStack->count() == 0) {
        messageLabel->setText (QString::null);
        closeButton->hide();
    }
}

//---------------------------------------------------------------------------
//  currentTabChanged
//
//! Called when the current tab changes.  Sets the contents of the form
//! used for closing tabs.
//---------------------------------------------------------------------------
void
MainWindow::currentTabChanged (int)
{
    QWidget* w = tabStack->currentWidget();
    QString status;
    bool saveEnabled = false;
    if (w) {
        ActionForm* form = static_cast<ActionForm*>(w);
        ActionForm::ActionFormType type = form->getType();
        if (type == ActionForm::DefineFormType) {
            DefineForm* defineForm = static_cast<DefineForm*>(form);
            defineForm->selectInputArea();
        }
        status = form->getStatusString();
        saveEnabled = form->isSaveEnabled();
    }
    messageLabel->setText (status);
    saveAction->setEnabled (saveEnabled);
}

//---------------------------------------------------------------------------
//  tabTitleChanged
//
//! Called when the title string for a tab changes.
//
//! @param the new title status string
//---------------------------------------------------------------------------
void
MainWindow::tabTitleChanged (const QString& title)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf (form);
    tabStack->setTabText (index, title);
}

//---------------------------------------------------------------------------
//  tabStatusChanged
//
//! Called when the status string for a tab changes.
//
//! @param the new status string
//---------------------------------------------------------------------------
void
MainWindow::tabStatusChanged (const QString& status)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf (form);
    if (index == tabStack->currentIndex()) {
        messageLabel->setText (status);
        qApp->processEvents();
    }
}

//---------------------------------------------------------------------------
//  tabSaveEnabledChanged
//
//! Called when the save enabled status for a tab changes.
//
//! @param the new save enabled status
//---------------------------------------------------------------------------
void
MainWindow::tabSaveEnabledChanged (bool saveEnabled)
{
    QObject* object = sender();
    if (!object)
        return;
    ActionForm* form = static_cast<ActionForm*>(object);
    int index = tabStack->indexOf (form);
    if (index == tabStack->currentIndex()) {
        saveAction->setEnabled (saveEnabled);
    }
}

//---------------------------------------------------------------------------
//  getLexiconPrefix
//
//! Return the path associated with a lexicon name.
//
//! @param lexicon the lexicon name
//! @return the path where the lexicon data is found
//---------------------------------------------------------------------------
QString
MainWindow::getLexiconPrefix (const QString& lexicon)
{
    QMap<QString, QString> pmap;
    pmap["OWL+LWL"] = "/north-american/owl-lwl";
    pmap["OWL2+LWL"] = "/north-american/owl2-lwl";
    pmap["OSPD4+LWL"] = "/north-american/ospd4-lwl";
    pmap["OSWI"] = "/british/oswi";
    pmap["ODS"] = "/french/ods4";
    return pmap.contains (lexicon) ? pmap.value (lexicon) : QString::null;
}

//---------------------------------------------------------------------------
//  getDatabaseFilename
//
//! Return the database filename that should be used for the current lexicon.
//! Also create the db directory if it doesn't already exist.  FIXME: That
//! should be done somewhere else!
//
//! @return the database filename, or QString::null if error
//---------------------------------------------------------------------------
QString
MainWindow::getDatabaseFilename()
{
    QString lexicon = wordEngine->getLexiconName();
    if (lexicon != "Custom") {
        QString lexiconPrefix = getLexiconPrefix (lexicon);
        if (lexiconPrefix.isEmpty())
            return QString::null;
    }

    QString dbPath = Auxil::getUserDir() + "/lexicons";
    QDir dir;
    dir.mkpath (dbPath);
    return (dbPath + "/" + lexicon + ".db");
}

//---------------------------------------------------------------------------
//  connectToDatabase
//
//! Connect to the database.  Create the database if necessary.
//---------------------------------------------------------------------------
void
MainWindow::connectToDatabase()
{
    QString dbFilename = getDatabaseFilename();
    QString lexicon = wordEngine->getLexiconName();

    bool createDatabase = false;
    QFile dbFile (dbFilename);

    QString message;
    QString caption;

    if (dbFile.exists()) {
        Rand rng;
        rng.srand (std::time (0), Auxil::getPid());
        unsigned int r = rng.rand();
        QString dbConnectionName = "MainWindow" + QString::number (r);
        {
            QSqlDatabase db = QSqlDatabase::addDatabase ("QSQLITE",
                                                         dbConnectionName);
            db.setDatabaseName (dbFilename);
            bool ok = db.open();

            if (ok) {
                QString qstr = "SELECT version FROM db_version";
                QSqlQuery query (qstr, db);
                int dbVersion = 0;
                if (query.next())
                    dbVersion = query.value (0).toInt();

                if (dbVersion < CURRENT_DATABASE_VERSION) {
                    caption = "Update database?";
                    message =
                        "The database exists but is out of date.  "
                        "For certain searches and quizzes to work correctly, "
                        "Zyzzyva must create an updated database of "
                        "information about the current lexicon (" + lexicon +
                        ").  This may take several minutes, but "
                        "it will only need to be done once.\n\n"
                        "Update the database now?";
                }

            }

            else {
                caption = "Rebuild database?";
                message =
                    "The database exists but Zyzzyva cannot connect to it.  "
                    "For certain searches and quizzes to work correctly, "
                    "Zyzzyva must create a database of information about "
                    "the current lexicon (" + lexicon + ").  This may take "
                    "several minutes, but it will only need to be done "
                    "once.\n\n"
                    "Rebuild the database now?";
            }
        }

        QSqlDatabase::removeDatabase (dbConnectionName);
    }

    else {
        caption = "Create database?";
        message =
            "For certain searches and quizzes to work correctly, Zyzzyva "
            "must create a database of information about the current "
            "lexicon (" + lexicon + ").  This may take several minutes, "
            "but it will only need to be done once.\n\n"
            "This database is not needed if you only want to use the "
            "Word Judge mode.\n\n"
            "Create the database now?";
    }

    if (!message.isEmpty()) {
        message = Auxil::dialogWordWrap (message);
        int code = QMessageBox::question (this, caption, message,
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes);
        if (code != QMessageBox::Yes) {
            return;
        }
        createDatabase = true;
    }

    bool useDb = !createDatabase || rebuildDatabase();
    QString dbError;
    if (useDb) {
        bool ok = wordEngine->connectToDatabase (dbFilename, &dbError);
        if (!ok) {
            QString caption = "Database Connection Error";
            QString message = "Unable to connect to database:\n" + dbError;
            message = Auxil::dialogWordWrap (message);
            QMessageBox::warning (this, caption, message);
            return;
        }
    }

    setNumWords (wordEngine->getNumWords());
}

//---------------------------------------------------------------------------
//  rebuildDatabase
//
//! Rebuild the database for the current lexicon.  Also display a progress
//! dialog.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
MainWindow::rebuildDatabase()
{
    QString dbFilename = getDatabaseFilename();
    QString lexicon = wordEngine->getLexiconName();

    QString definitionFilename;
    if (lexicon == "Custom") {
        definitionFilename = MainSettings::getAutoImportFile();
    }
    else {
        QString lexiconPrefix = getLexiconPrefix (lexicon);
        definitionFilename = Auxil::getWordsDir() + lexiconPrefix + ".txt";
    }

    QFileInfo fileInfo (dbFilename);
    QString file = fileInfo.fileName();
    QString path = fileInfo.path();
    QString tmpDbFilename = path + "/orig-" + file;

    wordEngine->disconnectFromDatabase();
    QFile dbFile (dbFilename);
    QFile tmpDbFile (tmpDbFilename);

    bool ok = (!tmpDbFile.exists() || tmpDbFile.remove());
    if (!ok) {
        QString caption = "Cannot remove database backup file";
        QString message = "Cannot remove database backup file: " +
            tmpDbFilename + ".\nPlease close Zyzzyva, remove or rename the "
            "file, then restart Zyzzyva.";
        message = Auxil::dialogWordWrap (message);
        QMessageBox::warning (this, caption, message);
        return false;
    }

    ok = (!dbFile.exists() || dbFile.rename (tmpDbFilename));
    if (!ok) {
        QString caption = "Cannot remove database file";
        QString message = "Cannot remove original database file: " +
            dbFilename + ".\nPlease close Zyzzyva, remove or rename the "
            "file, then restart Zyzzyva.";
        message = Auxil::dialogWordWrap (message);
        QMessageBox::warning (this, caption, message);
        tmpDbFile.rename (dbFilename);
        return false;
    }

    QProgressDialog* dialog = new QProgressDialog (this);

    QLabel* dialogLabel = new QLabel ("Creating " + lexicon + " database...");
    dialog->setWindowTitle ("Creating " + lexicon + " Database");
    dialog->setLabel (dialogLabel);

    CreateDatabaseThread* thread = new CreateDatabaseThread
        (wordEngine, lexicon, dbFilename, definitionFilename, this);
    connect (thread, SIGNAL (steps (int)),
             dialog, SLOT (setMaximum (int)));
    connect (thread, SIGNAL (progress (int)),
             dialog, SLOT (setValue (int)));
    connect (dialog, SIGNAL (canceled()), thread, SLOT (cancel()));

    QApplication::setOverrideCursor (Qt::WaitCursor);

    // FIXME: return an error if something actually goes wrong!

    thread->start();
    dialog->exec();
    thread->quit();

    QApplication::restoreOverrideCursor();

    if (thread->getCancelled()) {
        QMessageBox::information (this, "Database Not Created",
                                  "Database creation cancelled.");
        if (dbFile.exists())
            dbFile.remove();
        if (tmpDbFile.exists())
            tmpDbFile.rename (dbFilename);
        return false;
    }
    else {
        QMessageBox::information (this, "Database Created",
                                  "The " + lexicon + " database was "
                                  "successfully created.");
    }

    delete thread;
    delete dialog;
    return true;
}

//---------------------------------------------------------------------------
//  rescheduleCardbox
//
//! Reschedule quiz questions in the cardbox system.
//
//! @param words a list of words to reschedule
//! @param lexicon the lexicon
//! @param quizType the quiz type
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
int
MainWindow::rescheduleCardbox (const QStringList& words, const QString&
                               lexicon, const QString& quizType) const
{
    QuizSpec::QuizType type = Auxil::stringToQuizType (quizType);
    if (type == QuizSpec::UnknownQuizType)
        return false;

    QStringList questions;
    if (!words.isEmpty()) {
        switch (type) {
            case QuizSpec::QuizAnagrams:
            case QuizSpec::QuizAnagramsWithHooks:
            case QuizSpec::QuizSubanagrams:
            case QuizSpec::QuizAnagramJumble:
            case QuizSpec::QuizSubanagramJumble: {
                QSet<QString> alphagramSet;
                QStringListIterator it (words);
                while (it.hasNext()) {
                    alphagramSet.insert (Auxil::getAlphagram (it.next()));
                }
                questions = QStringList::fromSet (alphagramSet);
            }
            break;

            default:
            questions = words;
        }
    }

    QuizDatabase db (lexicon, quizType);
    if (!db.isValid())
        return false;

    return db.rescheduleCardbox (questions);
}

//---------------------------------------------------------------------------
//  closeEvent
//
//! The event handler for widget close events, called when the main window is
//! closed.
//
//! @param event the close event
//---------------------------------------------------------------------------
void
MainWindow::closeEvent (QCloseEvent* event)
{
    // Look for unsaved quizzes
    int count = tabStack->count();
    for (int i = 0; i < count; ++i) {
        ActionForm* form = static_cast<ActionForm*> (tabStack->widget (i));
        ActionForm::ActionFormType type = form->getType();
        if (type == ActionForm::QuizFormType) {
            QuizForm* quizForm = static_cast<QuizForm*> (form);
            if (quizForm->isSaveEnabled()) {
                tabStack->setCurrentWidget (quizForm);
                bool ok = quizForm->promptToSaveChanges();
                if (!ok) {
                    event->ignore();
                    return;
                }
            }
        }
    }

    writeSettings();
    event->accept();
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
    QString label = wordEngine->getLexiconName();

    if (label.isEmpty())
        label = "No lexicon loaded";
    else if (num)
        label += " - " + QString::number (num) + " words";

    lexiconLabel->setText (label);
}

//---------------------------------------------------------------------------
//  readSettings
//
//! Read application settings.
//---------------------------------------------------------------------------
void
MainWindow::readSettings (bool useGeometry)
{
    MainSettings::readSettings();

    if (useGeometry) {
        resize (MainSettings::getMainWindowSize());
        move (MainSettings::getMainWindowPos());
    }

    // Main font
    QFont mainFont;
    QString fontStr = MainSettings::getMainFont();
    bool mainFontOk = true;
    if (fontStr.isEmpty())
        ; // do nothing
    else if (mainFont.fromString (fontStr)) {
        qApp->setFont (mainFont);
    }
    else {
        //qWarning ("Cannot set font: " + fontStr);
        mainFontOk = false;
    }

    // Word list font
    QFont font;
    fontStr = MainSettings::getWordListFont();
    if (fontStr.isEmpty())
        ; // do nothing
    else if (font.fromString (fontStr))
        qApp->setFont (font, "WordTableView");
    else {
        //qWarning ("Cannot set font: " + fontStr);
    }

    // Set word list headers back to main font
    if (mainFontOk)
        qApp->setFont (mainFont, "QHeaderView");

    // Quiz label font
    fontStr = MainSettings::getQuizLabelFont();
    if (fontStr.isEmpty()) {
        font = qApp->font();
        font.setPixelSize (60);
        qApp->setFont (font, "QuizQuestionLabel");
    }
    else if (font.fromString (fontStr))
        qApp->setFont (font, "QuizQuestionLabel");
    else {
        //qWarning ("Cannot set font: " + fontStr);
    }

    // Word input font
    fontStr = MainSettings::getWordInputFont();
    if (fontStr.isEmpty())
        ; // do nothing
    else if (font.fromString (fontStr)) {
        qApp->setFont (font, "WordLineEdit");
        qApp->setFont (font, "WordTextEdit");
    }
    else {
        //qWarning ("Cannot set font: " + fontStr);
    }

    // Definition font
    fontStr = MainSettings::getDefinitionFont();
    if (fontStr.isEmpty())
        ; // do nothing
    else if (font.fromString (fontStr)) {
        qApp->setFont (font, "DefinitionBox");
        qApp->setFont (font, "DefinitionLabel");
    }
    else {
        //qWarning ("Cannot set font: " + fontStr);
    }

    // Set tile theme and background color for all quiz forms
    // FIXME: instead, this should simply call a settingsChanged slot on all
    // forms, and each form should handle it as they see fit
    QString tileTheme = MainSettings::getTileTheme();
    QColor backgroundColor = MainSettings::getQuizBackgroundColor();
    int count = tabStack->count();
    for (int i = 0; i < count; ++i) {
        ActionForm* form = static_cast<ActionForm*> (tabStack->widget (i));
        ActionForm::ActionFormType type = form->getType();
        if (type == ActionForm::QuizFormType) {
            QuizForm* quizForm = static_cast<QuizForm*> (form);
            quizForm->setBackgroundColor (backgroundColor);
            quizForm->updateQuestionStatus();
            quizForm->updateStatusString();
            quizForm->setTileTheme (tileTheme);
        }
    }
}

//---------------------------------------------------------------------------
//  writeSettings
//
//! Write application settings.
//---------------------------------------------------------------------------
void
MainWindow::writeSettings()
{
    MainSettings::setMainWindowPos (pos());
    MainSettings::setMainWindowSize (size());
    MainSettings::setProgramVersion (ZYZZYVA_VERSION);
    MainSettings::writeSettings();
}

//---------------------------------------------------------------------------
//  newTab
//
//! Create and display a new tab.
//
//! @param form the form to display
//---------------------------------------------------------------------------
void
MainWindow::newTab (ActionForm* form)
{
    connect (form, SIGNAL (titleChanged (const QString&)),
             SLOT (tabTitleChanged (const QString&)));
    connect (form, SIGNAL (statusChanged (const QString&)),
             SLOT (tabStatusChanged (const QString&)));
    connect (form, SIGNAL (saveEnabledChanged (bool)),
             SLOT (tabSaveEnabledChanged (bool)));

    tabStack->addTab (form, form->getIcon(), form->getTitle());
    tabStack->setCurrentWidget (form);
    closeButton->show();
    currentTabChanged (0);
}

//---------------------------------------------------------------------------
//  newQuizFromQuizFile
//
//! Create a new Quiz tab and start a quiz from a saved quiz file.
//
//! @param filename the file
//---------------------------------------------------------------------------
void
MainWindow::newQuizFromQuizFile (const QString& filename)
{
    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString errorMsg;
    QuizSpec quizSpec;
    if (!quizSpec.fromXmlFile (file, &errorMsg)) {
        QMessageBox::warning (this, "Cannot Create Quiz",
                              "An error occurred while creating the quiz.\n"
                              + errorMsg);
        return;
    }

    newQuizForm (quizSpec);
}

//---------------------------------------------------------------------------
//  newQuizFromWordFile
//
//! Create a new Quiz tab and start a quiz from a list of words in a plain
//! text file.
//
//! @param filename the file
//---------------------------------------------------------------------------
void
MainWindow::newQuizFromWordFile (const QString& filename)
{
    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return;

    QStringList words;
    char* buffer = new char [MAX_INPUT_LINE_LEN];
    while (file.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplified();
        if (!line.length() || (line.at (0) == '#'))
            continue;

        QString word = line.section (' ', 0, 0).toUpper();
        if (word.isEmpty())
            continue;

        words.append (word);
    }

    SearchCondition condition;
    condition.type = SearchCondition::InWordList;
    condition.stringValue = words.join (" ");

    SearchSpec searchSpec;
    searchSpec.conditions.append (condition);

    QuizSpec quizSpec;
    quizSpec.setType (QuizSpec::QuizAnagrams);
    quizSpec.setLexicon (wordEngine->getLexiconName());
    quizSpec.setSearchSpec (searchSpec);

    newQuizForm (quizSpec);
}

//---------------------------------------------------------------------------
//  importDawg
//
//! Import words from a DAWG file.
//
//! @param file the file to import words from
//! @param lexiconName the name of the lexicon
//! @param reverse whether the DAWG contains reversed words
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
MainWindow::importDawg (const QString& file, const QString& lexiconName, bool
                        reverse, QString* errString, quint16*
                        expectedChecksum)
{
    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));
    bool ok = wordEngine->importDawgFile (file, lexiconName, reverse,
                                          errString, expectedChecksum);
    QApplication::restoreOverrideCursor();

    if (!ok) {
        // Pop up warning stating why DAWG could not be loaded.
    }

    else if (errString && !errString->isEmpty()) {
        // Pop up warning, and ask user whether to proceed.
    }



    // If failure, notify the user and bail out

    if (!reverse) {
        //setNumWords (imported);
        setWindowTitle (APPLICATION_TITLE + " - " + lexiconName);
    }
    return ok;
}

//---------------------------------------------------------------------------
//  importChecksums
//
//! Import words from a text file.
//
//! @param file the file to import words from
//! @param lexiconName the name of the lexicon
//! @return the number of imported words
//---------------------------------------------------------------------------
QList<quint16>
MainWindow::importChecksums (const QString& filename)
{
    QList<quint16> checksums;
    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return checksums;

    char* buffer = new char [MAX_INPUT_LINE_LEN];
    while (file.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        checksums.append (line.toUShort());
    }
    return checksums;
}

//---------------------------------------------------------------------------
//  makeUserDirs
//
//! Create directories for user data.
//---------------------------------------------------------------------------
void
MainWindow::makeUserDirs()
{
    QDir dir;

    // Copy predefined quizzes and searches into user data directory
    if (!dir.exists (Auxil::getQuizDir() + "/predefined")) {
        Auxil::copyDir (Auxil::getRootDir() + "/data/quiz/predefined",
                        Auxil::getQuizDir() + "/predefined");
    }
    if (!dir.exists (Auxil::getSearchDir() + "/predefined")) {
        Auxil::copyDir (Auxil::getRootDir() + "/data/search/predefined",
                        Auxil::getSearchDir() + "/predefined");
    }

    renameLexicon ("OWL", "OWL+LWL");
    renameLexicon ("OWL2", "OWL2+LWL");
    renameLexicon ("SOWPODS", "OSWI");

    dir.mkpath (Auxil::getQuizDir() + "/saved");
    dir.mkpath (Auxil::getSearchDir() + "/saved");
    dir.mkpath (Auxil::getUserWordsDir() + "/saved");
    dir.mkpath (Auxil::getUserDir() + "/judge");
}

//---------------------------------------------------------------------------
//  renameLexicon
//
//! Take care of things that need to be done when a lexicon has been renamed.
//
//! @param oldName the old lexicon name
//! @param newName the new lexicon name
//---------------------------------------------------------------------------
void
MainWindow::renameLexicon (const QString& oldName, const QString& newName)
{
    QDir dir;

    // Move quiz database
    QString oldDir = Auxil::getQuizDir() + "/data/" + oldName;
    QString newDir = Auxil::getQuizDir() + "/data/" + newName;
    if (dir.exists (oldDir) && !dir.exists (newDir)) {
        dir.rename (oldDir, newDir);
    }

    // Move lexicon database
    QString oldDb = Auxil::getUserDir() + "/lexicons/" + oldName + ".db";
    QString newDb = Auxil::getUserDir() + "/lexicons/" + newName + ".db";
    if (QFile::exists (oldDb) && !QFile::exists (newDb)) {
        QFile::rename (oldDb, newDb);
    }
}

//---------------------------------------------------------------------------
//  importText
//
//! Import words from a text file.
//
//! @param file the file to import words from
//! @param lexiconName the name of the lexicon
//! @return the number of imported words
//---------------------------------------------------------------------------
int
MainWindow::importText (const QString& file, const QString& lexiconName)
{
    QString err;
    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));
    int imported = wordEngine->importTextFile (file, lexiconName, true);
    QApplication::restoreOverrideCursor();

    setNumWords (imported);
    setWindowTitle (APPLICATION_TITLE + " - " + lexiconName);
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
    stemFiles << (Auxil::getWordsDir() + "/north-american/6-letter-stems.txt");
    stemFiles << (Auxil::getWordsDir() + "/north-american/7-letter-stems.txt");

    QString err;
    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));
    QStringList::iterator it;
    int totalImported = 0;
    for (it = stemFiles.begin(); it != stemFiles.end(); ++it) {
        int imported = wordEngine->importStems (*it, &err);
        totalImported += imported;
    }
    QApplication::restoreOverrideCursor();

    return totalImported;
}

//---------------------------------------------------------------------------
//  doTest
//
//! Test something out.
//---------------------------------------------------------------------------
void
MainWindow::doTest()
{
    LetterBag letterBag;
    qDebug() << "*** doTest!";
}
