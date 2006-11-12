//---------------------------------------------------------------------------
// WordListDialog.cpp
//
// The dialog for editing a list of words.
//
// Copyright 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include "WordListDialog.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Edit Word List";
const QString LIST_HEADER_PREFIX = "Word List : ";

using namespace Defs;

//---------------------------------------------------------------------------
//  WordListDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
WordListDialog::WordListDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    mainVlay->setMargin (MARGIN);
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);

    numWordsLabel = new QLabel;
    Q_CHECK_PTR (numWordsLabel);
    mainVlay->addWidget (numWordsLabel);

    wordList = new QListWidget (this);
    Q_CHECK_PTR (wordList);
    mainVlay->addWidget (wordList);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    ZPushButton* openFileButton = new ZPushButton ("Open &File...");
    Q_CHECK_PTR (openFileButton);
    openFileButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (openFileButton, SIGNAL (clicked()), SLOT (openFileClicked()));
    buttonHlay->addWidget (openFileButton);

    ZPushButton* clearButton = new ZPushButton ("&Clear");
    Q_CHECK_PTR (clearButton);
    clearButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (clearButton, SIGNAL (clicked()), SLOT (clearClicked()));
    buttonHlay->addWidget (clearButton);

    buttonHlay->addStretch (1);

    ZPushButton* okButton = new ZPushButton ("&OK");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    ZPushButton* cancelButton = new ZPushButton ("Cancel");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    updateLabel();
    setWindowTitle (DIALOG_CAPTION);
}

//---------------------------------------------------------------------------
//  ~WordListDialog
//
//! Destructor.
//---------------------------------------------------------------------------
WordListDialog::~WordListDialog()
{
}

//---------------------------------------------------------------------------
//  setWords
//
//! Set the contents of the word list to be the words in a space-separated
//! string of words.
//
//! @param string the word list string
//---------------------------------------------------------------------------
void
WordListDialog::setWords (const QString& string)
{
    wordList->clear();
    QStringList words = string.split (QChar (' '), QString::SkipEmptyParts);
    QStringListIterator it (words);
    while (it.hasNext())
        new QListWidgetItem (it.next(), wordList);
}

//---------------------------------------------------------------------------
//  getWords
//
//! Return the word list, with words separated by single spaces.
//
//! @return the word list string
//---------------------------------------------------------------------------
QString
WordListDialog::getWords() const
{
    QString str;
    QListWidgetItem* lookItem = 0;
    for (int i = 0; i < wordList->count(); ++i) {
        lookItem = wordList->item (i);
        if (!str.isEmpty())
            str += " ";
        str += lookItem->text();
    }
    return str;
}

//---------------------------------------------------------------------------
//  openFileClicked
//
//! Called when the Open File button is clicked.  Open a file chooser dialog.
//---------------------------------------------------------------------------
void
WordListDialog::openFileClicked()
{
    QString filename = QFileDialog::getOpenFileName (this,
        "Open Word List File", Auxil::getUserWordsDir() + "/saved",
        "Text Files (*.txt)");

    if (filename.isEmpty())
        return;

    QFile file (filename);
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        QString caption = "Error Opening Word List File";
        QString message = "Cannot open file '" + filename + "': " +
            file.errorString();
        message = Auxil::dialogWordWrap (message);
        QMessageBox::warning (this, caption, message);
        return;
    }

    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));
    QString line;
    char* buffer = new char [MAX_INPUT_LINE_LEN];
    while (file.readLine (buffer, MAX_INPUT_LINE_LEN) > 0) {
        QString line (buffer);
        line = line.simplified();
        if (!line.length() || (line.at (0) == '#'))
            continue;
        QString word = line.section (' ', 0, 0).toUpper();
        new QListWidgetItem (word, wordList);
    }
    delete[] buffer;

    QApplication::restoreOverrideCursor();

    updateLabel();
}

//---------------------------------------------------------------------------
//  clearClicked
//
//! Called when the Clear button is clicked.  Clears the word list.
//---------------------------------------------------------------------------
void
WordListDialog::clearClicked()
{
    wordList->clear();
    updateLabel();
}

//---------------------------------------------------------------------------
//  updateLabel
//
//! Update the label with the current number of words.
//---------------------------------------------------------------------------
void
WordListDialog::updateLabel()
{
    numWordsLabel->setText (LIST_HEADER_PREFIX + QString::number
                            (wordList->count()) + " words");
}
