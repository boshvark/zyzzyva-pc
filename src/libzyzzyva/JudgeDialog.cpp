//---------------------------------------------------------------------------
// JudgeDialog.cpp
//
// A full-screen dialog for Word Judge functionality, in which the user can
// very easily judge the validity of one or more words.
//
// Copyright 2006, 2007, 2008 Michael W Thelen <mthelen@gmail.com>.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "JudgeDialog.h"
#include "DefinitionBox.h"
#include "MainSettings.h"
#include "WordEngine.h"
#include "WordTextEdit.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPalette>
#include <QTextCursor>
#include <QTextStream>
#include <QVBoxLayout>

const int FORM_FONT_PIXEL_SIZE = 55;
const int TITLE_FONT_PIXEL_SIZE = 40;
const int INSTRUCTION_FONT_PIXEL_SIZE = 40;
const int EXIT_FONT_PIXEL_SIZE = 30;
const int INPUT_MARGIN = 30;
const int RESULT_BORDER_WIDTH = 30;
const int RESULT_SPACING = 100;
const int CLEAR_INPUT_DELAY = 15000;
const int CLEAR_RESULTS_DELAY = 10000;
const int CLEAR_RESULTS_MIN_DELAY = 500;
const int EXIT_TIMEOUT = 5000;
const int NUM_KEYPRESSES_TO_EXIT = 10;
const int NUM_KEYPRESSES_TO_DISPLAY = 5;
const int CLEAR_EXIT_DELAY = 5000;
const int MAX_JUDGE_WORDS = 8;

const QString INSTRUCTION_MESSAGE = "1. CHALLENGER: Enter words, "
                                    "separated by SPACE or ENTER.\n"
                                    "2. OPPONENT: Press TAB to judge the play.\n"
                                    "3. Press any key to clear the results.";

using namespace Defs;

//---------------------------------------------------------------------------
//  JudgeDialog
//
//! Constructor.
//
//! @param e the word engine
//! @param lexi the lexicon to use
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
JudgeDialog::JudgeDialog(WordEngine* e, const QString& lex, QWidget* parent,
                         Qt::WFlags f)
    : QDialog(parent, f), engine(e), lexicon(lex), clearResultsHold(0)
{
    QFont formFont = qApp->font();
    formFont.setPixelSize(FORM_FONT_PIXEL_SIZE);

    QFont titleFont = qApp->font();
    titleFont.setPixelSize(TITLE_FONT_PIXEL_SIZE);

    QFont instructionFont = qApp->font();
    instructionFont.setPixelSize(INSTRUCTION_FONT_PIXEL_SIZE);

    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(0);
    mainVlay->setSpacing(SPACING);
    Q_CHECK_PTR(mainVlay);

    widgetStack = new QStackedWidget;
    Q_CHECK_PTR(widgetStack);
    mainVlay->addWidget(widgetStack);

    inputWidget = new QWidget;
    Q_CHECK_PTR(inputWidget);
    widgetStack->addWidget(inputWidget);

    QVBoxLayout* inputVlay = new QVBoxLayout(inputWidget);
    inputVlay->setMargin(INPUT_MARGIN);
    inputVlay->setSpacing(20);
    Q_CHECK_PTR(inputVlay);

    instLabel = new QLabel(INSTRUCTION_MESSAGE);
    Q_CHECK_PTR(instLabel);
    instLabel->setFont(instructionFont);
    instLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    instLabel->setWordWrap(true);
    inputVlay->addWidget(instLabel);

    inputArea = new WordTextEdit;
    Q_CHECK_PTR(inputArea);
    inputArea->setContextMenuPolicy(Qt::NoContextMenu);
    inputArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    inputArea->setFont(formFont);
    connect(inputArea, SIGNAL(textChanged()), SLOT(textChanged()));
    inputVlay->addWidget(inputArea);

    resultWidget = new QFrame;
    Q_CHECK_PTR(resultWidget);
    resultWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    resultWidget->setFrameStyle(QFrame::Box | QFrame::Plain);
    resultWidget->setLineWidth(RESULT_BORDER_WIDTH);
    widgetStack->addWidget(resultWidget);

    QVBoxLayout* resultVlay = new QVBoxLayout(resultWidget);
    resultVlay->setMargin(0);
    resultVlay->setSpacing(RESULT_SPACING);
    Q_CHECK_PTR(resultVlay);

    resultVlay->addStretch(1);

    resultPixmapLabel = new QLabel;
    Q_CHECK_PTR(resultPixmapLabel);
    resultPixmapLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    resultVlay->addWidget(resultPixmapLabel);

    resultLabel = new QLabel;
    Q_CHECK_PTR(resultLabel);
    resultLabel->setFont(formFont);
    resultLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    resultLabel->setWordWrap(true);
    resultVlay->addWidget(resultLabel);

    resultVlay->addStretch(1);

    QHBoxLayout* titleHlay = new QHBoxLayout;
    Q_CHECK_PTR(titleHlay);
    inputVlay->addLayout(titleHlay);

    QLabel* programLabel = new QLabel("Zyzzyva Word Judge\n"
                                      "Version " + ZYZZYVA_VERSION);
    Q_CHECK_PTR(programLabel);
    programLabel->setFont(titleFont);
    titleHlay->addWidget(programLabel);

    titleHlay->addStretch(1);

    QDate date = Auxil::lexiconToDate(lexicon);
    QString dateStr;
    if (date.isValid())
        dateStr = date.toString("MMMM d, yyyy");
    QLabel* lexiconLabel = new QLabel("Lexicon: " + lexicon + "\n" + dateStr);
    Q_CHECK_PTR(lexiconLabel);
    lexiconLabel->setFont(titleFont);
    lexiconLabel->setAlignment(Qt::AlignRight);
    titleHlay->addWidget(lexiconLabel);

    inputTimer = new QTimer(this);
    Q_CHECK_PTR(inputTimer);
    connect(inputTimer, SIGNAL(timeout()), SLOT(clearInput()));

    resultTimer = new QTimer(this);
    Q_CHECK_PTR(resultTimer);
    connect(resultTimer, SIGNAL(timeout()), SLOT(clearResults()));

    exitTimer = new QTimer(this);
    Q_CHECK_PTR(exitTimer);
    connect(exitTimer, SIGNAL(timeout()), SLOT(clearExit()));

    clearResults();
    showFullScreen();
}

//---------------------------------------------------------------------------
//  textChanged
//
//! Called when the contents of the input area change.  Changes the input area
//! contents to upper case, and enables or disables the buttons appropriately.
//---------------------------------------------------------------------------
void
JudgeDialog::textChanged()
{
    QTextCursor cursor = inputArea->textCursor();
    int origCursorPosition = cursor.position();
    int deletedBeforeCursor = 0;

    inputArea->blockSignals(true);
    QString text = inputArea->toPlainText().toUpper();
    int lookIndex = 0;
    int wordLength = 0;
    int numWords = 0;
    bool afterSpace = false;
    bool doJudge = false;
    for (int i = 0; i < text.length(); ++i) {
        QChar c = text.at(lookIndex);

        if (c.isLetter()) {
            if (wordLength == 0)
                ++numWords;

            if (numWords <= MAX_JUDGE_WORDS) {
                afterSpace = false;
                ++wordLength;
                if (wordLength > MAX_WORD_LEN) {
                    text.insert(lookIndex, "\n");
                    afterSpace = true;
                    wordLength = 0;
                    if (i < origCursorPosition)
                        --deletedBeforeCursor;
                }
            }

            // Disallow more than MAX_JUDGE_WORDS by whiting them out
            else {
                text.replace(lookIndex, 1, "\n");
            }
            ++lookIndex;
        }
        else if ((lookIndex > 0) && c.isSpace() && (c != '\t') && !afterSpace) {
            text.replace(lookIndex, 1, "\n");
            afterSpace = true;
            ++lookIndex;
            wordLength = 0;
        }
        else {
            if (c == '\t')
                doJudge = true;
            text.remove(lookIndex, 1);
            if (i < origCursorPosition)
                ++deletedBeforeCursor;
            wordLength = 0;
        }
    }
    text.replace(QRegExp("\\n+"), "\n");

    inputArea->setPlainText(text);
    int position = origCursorPosition - deletedBeforeCursor;
    if (position > text.length())
        position = text.length();
    cursor.setPosition(position);
    inputArea->setTextCursor(cursor);
    inputArea->blockSignals(false);

    if (!text.isEmpty()) {
        if (doJudge)
            judgeWord();
        else
            inputTimer->start(CLEAR_INPUT_DELAY);
    }
}

//---------------------------------------------------------------------------
//  clearResults
//
//! Clear the input area and the result area.
//---------------------------------------------------------------------------
void
JudgeDialog::clearResults()
{
    resultTimer->stop();
    exitTimer->stop();
    clearExit();
    inputArea->clear();
    widgetStack->setCurrentWidget(inputWidget);
}

//---------------------------------------------------------------------------
//  judgeWord
//
//! Look up and display the acceptability of the word currently in the word
//! edit area.
//---------------------------------------------------------------------------
void
JudgeDialog::judgeWord()
{
    bool acceptable = true;

    QString text = inputArea->toPlainText().simplified();
    QStringList words = text.split(QChar(' '));
    QStringList acceptableWords;
    QStringList unacceptableWords;
    QStringList::iterator it;
    QString wordStr;
    for (it = words.begin(); it != words.end(); ++it) {
        bool wordAcceptable = engine->isAcceptable(lexicon, *it);

        if (wordAcceptable)
            acceptableWords.append(*it);
        else
            unacceptableWords.append(*it);

        if (!wordAcceptable)
            acceptable = false;
        if (!wordStr.isEmpty())
            wordStr += ", ";
        wordStr += *it;
    }

    QString resultStr;
    QColor resultColor;
    QPixmap resultPixmap;
    if (acceptable) {
        resultStr = "<font color=\"#00bb00\">YES, the play is "
                    "<b>ACCEPTABLE</b></font>";
        resultColor = QColor(0, 204, 0);
        resultPixmap.load(":/judge-acceptable");
    }
    else {
        resultStr = "<font color=\"red\">NO, the play is "
                    "<b>UNACCEPTABLE</b></font>";
        resultColor = Qt::red;
        resultPixmap.load(":/judge-unacceptable");
    }
    resultStr += "<br><br><font color=\"black\">" + wordStr + "</font>";

    QPalette pal = resultWidget->palette();
    pal.setColor(QPalette::Foreground, resultColor);
    resultWidget->setPalette(pal);
    resultLabel->setPalette(pal);

    ++clearResultsHold;

    resultLabel->setText(resultStr);
    if (!resultPixmap.isNull())
        resultPixmapLabel->setPixmap(resultPixmap);
    widgetStack->setCurrentWidget(resultWidget);

    QTimer::singleShot(CLEAR_RESULTS_MIN_DELAY, this,
                       SLOT(clearResultsReleaseHold()));

    resultTimer->start(CLEAR_RESULTS_DELAY);

    if (!MainSettings::getJudgeSaveLog())
        return;

    QString logDirName = MainSettings::getUserDataDir() + "/judge/" + lexicon;
    QDir logDir (logDirName);

    if (!logDir.exists() && !logDir.mkdir(logDirName)) {
        qWarning("Cannot create judge log directory\n");
        return;
    }

    // Log challenge results to files
    if (!acceptableWords.empty()) {
        QFile file (logDirName + "/acceptable.txt");
        file.open(QIODevice::Append | QIODevice::Text);
        QTextStream stream (&file);
        foreach (QString word, acceptableWords) {
            stream << word;
            endl(stream);
        }
    }

    if (!unacceptableWords.empty()) {
        QFile file (logDirName + "/unacceptable.txt");
        file.open(QIODevice::Append | QIODevice::Text);
        QTextStream stream (&file);
        foreach (QString word, unacceptableWords) {
            stream << word;
            endl(stream);
        }
    }

    QFile file (logDirName + "/challenge.txt");
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream stream (&file);
    stream << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")
        << (acceptable ? "acceptable   +++" : "unacceptable ---");
    foreach (QString word, words) {
        stream << " " << word;
    }
    endl(stream);
}

//---------------------------------------------------------------------------
//  clearResultsReleaseHold
//
//! Release a hold on clearing the results display.
//---------------------------------------------------------------------------
void
JudgeDialog::clearResultsReleaseHold()
{
    --clearResultsHold;
}

//---------------------------------------------------------------------------
//  displayExit
//
//! Display instructions for exiting full screen mode.
//---------------------------------------------------------------------------
void
JudgeDialog::displayExit()
{
    instLabel->setText(INSTRUCTION_MESSAGE + "\nTo exit, hold SHIFT and "
                       "press the ESC key.");
    exitTimer->start(CLEAR_EXIT_DELAY);
}

//---------------------------------------------------------------------------
//  clearInput
//
//! Clear the input area.
//---------------------------------------------------------------------------
void
JudgeDialog::clearInput()
{
    inputTimer->stop();
    inputArea->clear();
}

//---------------------------------------------------------------------------
//  clearExit
//
//! Clear instructions for exiting full screen mode.
//---------------------------------------------------------------------------
void
JudgeDialog::clearExit()
{
    instLabel->setText(INSTRUCTION_MESSAGE);
}

//---------------------------------------------------------------------------
//  keyPressEvent
//
//! Receive key press events for the widget.  Reimplemented from QWidget.
//
//! @param event the key press event
//---------------------------------------------------------------------------
void
JudgeDialog::keyPressEvent(QKeyEvent* event)
{
    if (!event)
        return;

    bool cleared = false;
    if ((widgetStack->currentWidget() == resultWidget) && !clearResultsHold) {
        clearResults();
        cleared = true;
    }

    if (event->key() == Qt::Key_Escape) {

        Qt::KeyboardModifiers modifiers = event->modifiers();
        if (modifiers & Qt::ShiftModifier)
            accept();
        else if (!cleared)
            displayExit();
    }
    else
        event->ignore();
}
