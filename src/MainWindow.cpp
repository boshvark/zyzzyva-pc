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
#include <qdir.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmenubar.h>

const QString ANAGRAM_MODE = "Anagram";
const QString RECALL_LIST_MODE = "Recall List";
const QString CHOOSE_BUTTON_TEXT = "Choose...";
const QString CHOOSER_TITLE = "Choose a Word List";

MainWindow::MainWindow (QWidget* parent, const char* name, WFlags f)
    : QMainWindow (parent, name, f)
{
    QFrame* frame = new QFrame (this);
    setCentralWidget (frame);


    QPopupMenu* filePopup = new QPopupMenu (this);
    Q_CHECK_PTR (filePopup);
    filePopup->insertItem ("&Open...", this, SLOT (Open()), CTRL+Key_O);
    menuBar()->insertItem ("&File", filePopup);

    QVBoxLayout* mainVlay = new QVBoxLayout (frame, 0, 0, "mainVlay");
    Q_CHECK_PTR (mainVlay);
    QHBoxLayout* modeHlay = new QHBoxLayout (0, "mainHlay");
    Q_CHECK_PTR (modeHlay);
    mainVlay->addLayout (modeHlay);
    QLabel* modeLabel = new QLabel ("Mode:", frame, "modeLabel");
    Q_CHECK_PTR (modeLabel);
    modeHlay->addWidget (modeLabel);
    modeCombo = new QComboBox (frame, "modeCombo");
    Q_CHECK_PTR (modeCombo);
    modeCombo->insertItem (ANAGRAM_MODE);
    modeCombo->insertItem (RECALL_LIST_MODE);
    modeHlay->addWidget (modeCombo);

    QHBoxLayout* fileHlay = new QHBoxLayout (0, "fileHlay");
    Q_CHECK_PTR (fileHlay);
    mainVlay->addLayout (fileHlay);
    QLabel* fileLabel = new QLabel ("Word List:", frame, "fileLabel");
    Q_CHECK_PTR (fileLabel);
    fileHlay->addWidget (fileLabel);
}

void
MainWindow::open()
{
    QString file = QFileDialog::getOpenFileName (QDir::current().path(),
                                                 "All Files (*.*)", this,
                                                 "fileDialog",
                                                 CHOOSER_TITLE);
    if (file.isNull()) return;
    qDebug (file);
}
