//---------------------------------------------------------------------------
// MainWindow.cpp
//
// The main window for the word study application.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "MainWindow.h"
#include "LookupForm.h"
#include "WordEngine.h"
#include <qapplication.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmessagebox.h>

const QString ANAGRAM_MODE = "Anagram";
const QString ADJUDICATE_MODE = "Adjudicate";
const QString RECALL_LIST_MODE = "Recall List";
const QString CHOOSE_BUTTON_TEXT = "Choose...";
const QString CHOOSER_TITLE = "Choose a Word List";
const QString IMPORT_COMPLETE_TITLE = "Import Complete";

//---------------------------------------------------------------------------
// MainWindow
//
//! Constructor.
//
//! @param parent the parent widget
//! @name the name of this widget
//! @f widget flags
//---------------------------------------------------------------------------
MainWindow::MainWindow (QWidget* parent, const char* name, WFlags f)
    : QMainWindow (parent, name, f), engine (new WordEngine())
{

    QFrame* frame = new QFrame (this);
    setCentralWidget (frame);

    QPopupMenu* filePopup = new QPopupMenu (this);
    Q_CHECK_PTR (filePopup);
    filePopup->insertItem ("&Import...", this, SLOT (import()), CTRL+Key_I);
    filePopup->insertSeparator();
    filePopup->insertItem ("&Quit", qApp, SLOT (quit()));
    menuBar()->insertItem ("&File", filePopup);

    QVBoxLayout* mainVlay = new QVBoxLayout (frame, 0, 0, "mainVlay");
    Q_CHECK_PTR (mainVlay);
//    QHBoxLayout* modeHlay = new QHBoxLayout (0, "mainHlay");
//    Q_CHECK_PTR (modeHlay);
//    mainVlay->addLayout (modeHlay);
//    QLabel* modeLabel = new QLabel ("Mode:", frame, "modeLabel");
//    Q_CHECK_PTR (modeLabel);
//    modeHlay->addWidget (modeLabel);
//    modeCombo = new QComboBox (frame, "modeCombo");
//    Q_CHECK_PTR (modeCombo);
//    modeCombo->insertItem (ANAGRAM_MODE);
//    modeCombo->insertItem (RECALL_LIST_MODE);
//    modeHlay->addWidget (modeCombo);

//    QHBoxLayout* fileHlay = new QHBoxLayout (0, "fileHlay");
//    Q_CHECK_PTR (fileHlay);
//    mainVlay->addLayout (fileHlay);
//    QLabel* fileLabel = new QLabel ("Word List:", frame, "fileLabel");
//    Q_CHECK_PTR (fileLabel);
//    fileHlay->addWidget (fileLabel);

    lookupForm = new LookupForm (frame, "lookupForm");
    mainVlay->addWidget (lookupForm);
}

//---------------------------------------------------------------------------
// import
//
//! Allow the user to import a word list from a file.
//---------------------------------------------------------------------------
void
MainWindow::import()
{
    QString file = QFileDialog::getOpenFileName (QDir::current().path(),
                                                 "All Files (*.*)", this,
                                                 "fileDialog",
                                                 CHOOSER_TITLE);
    if (file.isNull()) return;
    int imported = engine->importFile (file);

    QMessageBox::information (this, IMPORT_COMPLETE_TITLE,
                              "Imported " + QString::number (imported)
                              + " words.",
                              QMessageBox::Ok);
}
