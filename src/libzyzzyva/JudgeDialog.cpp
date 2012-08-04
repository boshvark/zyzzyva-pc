//---------------------------------------------------------------------------
// JudgeDialog.cpp
//
// A full-screen dialog for Word Judge functionality, in which the user can
// very easily judge the validity of one or more words.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPalette>
#include <QTextCursor>
#include <QTextStream>
#include <QVBoxLayout>

// How many pixels to display for every 20 pixels of screen height
const int FONT_PIXEL_DIVIDER = 20;
const double FORM_FONT_PIXEL_SIZE = 1.5;
const double LEXICON_FONT_PIXEL_SIZE = 1.0;
const double TITLE_FONT_PIXEL_SIZE = 1.0;
const double INSTRUCTION_FONT_PIXEL_SIZE = 1.0;
const double PASSWORD_FONT_PIXEL_SIZE = 1.0;
const double COUNT_FONT_PIXEL_SIZE = 5.0;
const double EXIT_FONT_PIXEL_SIZE = 0.75;
const double PASSWORD_MARGIN = 0.75;
const double PASSWORD_SPACING = 1.5;
const double COUNT_MARGIN = 0.75;
const double COUNT_SPACING = 0.5;
const double INPUT_MARGIN = 0.75;
const double INPUT_SPACING = 0.5;
const double RESULT_BORDER_WIDTH = 0.75;
const double RESULT_MARGIN = 0.75;
const double RESULT_SPACING = 0.75;

const int CLEAR_COUNT_DELAY = 750;
const int CLEAR_PASSWORD_DELAY = 10000;
const int CLEAR_INCORRECT_PASSWORD_DELAY = 1500;
const int CLEAR_INPUT_DELAY = 15000;
const int CLEAR_RESULTS_DELAY = 10000;
const int CLEAR_RESULTS_MIN_DELAY = 500;
const int EXIT_TIMEOUT = 5000;
const int NUM_KEYPRESSES_TO_EXIT = 10;
const int NUM_KEYPRESSES_TO_DISPLAY = 5;
const int CLEAR_EXIT_DELAY = 5000;
const int MAX_JUDGE_WORDS = 8;

const QString PASSWORD_INSTRUCTION_MESSAGE =
    "Enter password to exit Word Judge.";
const QString NO_PASSWORD_INSTRUCTION_MESSAGE =
    "Press Enter to exit Word Judge.";

const QString INCORRECT_PASSWORD_MESSAGE = "Sorry, incorrect password.";

const QString COUNT_INSTRUCTION_MESSAGE = "CHALLENGER:\n"
    "How many words would you like to challenge?";

const QString INSTRUCTION_MESSAGE =
    "1. CHALLENGER: Enter %1 word%2.\n"
    "2. OPPONENT: Press TAB to judge the play.\n"
    "3. Press any key to clear the results.";

using namespace Defs;

//---------------------------------------------------------------------------
//  JudgeDialog
//
//! Constructor.
//
//! @param e the word engine
//! @param lex the lexicon to use
//! @param pass the password to use
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
JudgeDialog::JudgeDialog(WordEngine* e, const QString& lex,
    const QString& pass, QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f), engine(e), lexicon(lex), password(pass), count(0),
    clearResultsHold(0), fontMultiplier(0)
{
    countTimer = new QTimer(this);
    connect(countTimer, SIGNAL(timeout()), SLOT(displayInput()));

    passwordTimer = new QTimer(this);
    connect(passwordTimer, SIGNAL(timeout()), SLOT(displayCount()));

    inputTimer = new QTimer(this);
    connect(inputTimer, SIGNAL(timeout()), SLOT(displayCount()));

    resultTimer = new QTimer(this);
    connect(resultTimer, SIGNAL(timeout()), SLOT(clearResults()));

    exitTimer = new QTimer(this);
    connect(exitTimer, SIGNAL(timeout()), SLOT(clearExit()));

    //const int screenWidth = QApplication::desktop()->width();
    const int screenHeight = QApplication::desktop()->height();
    fontMultiplier = screenHeight / FONT_PIXEL_DIVIDER;

    QFont formFont = qApp->font();
    formFont.setPixelSize(int(FORM_FONT_PIXEL_SIZE * fontMultiplier));

    QFont instructionFont = qApp->font();
    instructionFont.setPixelSize(int(INSTRUCTION_FONT_PIXEL_SIZE * fontMultiplier));

    QFont countFont = qApp->font();
    countFont.setPixelSize(int(COUNT_FONT_PIXEL_SIZE * fontMultiplier));
    countFont.setBold(true);

    QVBoxLayout* mainVlay = new QVBoxLayout(this);
    mainVlay->setMargin(0);
    mainVlay->setSpacing(SPACING);

    widgetStack = new QStackedWidget;
    connect(widgetStack, SIGNAL(currentChanged(int)),
            SLOT(currentChanged(int)));
    mainVlay->addWidget(widgetStack);

    // Count screen
    countWidget = new QWidget;
    widgetStack->addWidget(countWidget);

    QVBoxLayout* countVlay = new QVBoxLayout(countWidget);
    countVlay->setMargin(int(COUNT_MARGIN * fontMultiplier));
    countVlay->setSpacing(int(COUNT_SPACING * fontMultiplier));

    countVlay->addStretch(1);

    countInstLabel = new QLabel(COUNT_INSTRUCTION_MESSAGE);
    countInstLabel->setFont(instructionFont);
    countInstLabel->setAlignment(Qt::AlignHCenter);
    countInstLabel->setWordWrap(true);
    countVlay->addWidget(countInstLabel);

    countLabel = new QLabel;
    countLabel->setFont(countFont);
    countLabel->setAlignment(Qt::AlignHCenter);
    countVlay->addWidget(countLabel);

    countVlay->addStretch(1);

    QWidget* countTitleWidget = createTitleWidget();
    countVlay->addWidget(countTitleWidget);

    // Password screen
    passwordWidget = new QWidget;
    widgetStack->addWidget(passwordWidget);

    QVBoxLayout* passwordVlay = new QVBoxLayout(passwordWidget);
    passwordVlay->setMargin(int(PASSWORD_MARGIN * fontMultiplier));
    passwordVlay->setSpacing(int(PASSWORD_SPACING * fontMultiplier));

    passwordVlay->addStretch(1);

    passwordLabel = new QLabel;
    passwordLabel->setFont(instructionFont);
    passwordLabel->setAlignment(Qt::AlignHCenter);
    passwordVlay->addWidget(passwordLabel);

    const int screenWidth = QApplication::desktop()->width();
    QHBoxLayout* passwordLineHlay = new QHBoxLayout;
    passwordLineHlay->setMargin(0);
    passwordVlay->addLayout(passwordLineHlay);

    passwordLineHlay->addSpacing(screenWidth / 4);

    QFontMetrics instructionFontMetrics (instructionFont);
    passwordLine = new QLineEdit;
    passwordLine->setFont(instructionFont);
    passwordLine->setEchoMode(QLineEdit::Password);
    passwordLine->setMinimumSize(0, instructionFontMetrics.height());
    connect(passwordLine, SIGNAL(textChanged(const QString&)),
        SLOT(passwordTextChanged()));
    connect(passwordLine, SIGNAL(returnPressed()),
        SLOT(passwordReturnPressed()));
    passwordLineHlay->addWidget(passwordLine);

    passwordLineHlay->addSpacing(screenWidth / 4);

    passwordResultLabel = new QLabel;
    QFont passwordResultFont = instructionFont;
    QPalette passwordResultPalette = passwordResultLabel->palette();
    passwordResultPalette.setColor(QPalette::Foreground, Qt::red);
    passwordResultLabel->setAlignment(Qt::AlignHCenter);
    passwordResultLabel->setFont(passwordResultFont);
    passwordResultLabel->setPalette(passwordResultPalette);
    passwordVlay->addWidget(passwordResultLabel);

    passwordVlay->addStretch(1);

    // Input screen
    inputWidget = new QWidget;
    widgetStack->addWidget(inputWidget);

    QVBoxLayout* inputVlay = new QVBoxLayout(inputWidget);
    inputVlay->setMargin(int(INPUT_MARGIN * fontMultiplier));
    inputVlay->setSpacing(int(INPUT_SPACING * fontMultiplier));

    inputInstLabel = new QLabel;
    inputInstLabel->setFont(instructionFont);
    inputInstLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    inputInstLabel->setWordWrap(true);
    inputVlay->addWidget(inputInstLabel);

    inputArea = new WordTextEdit;
    inputArea->setContextMenuPolicy(Qt::NoContextMenu);
    inputArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    inputArea->setFont(formFont);
    connect(inputArea, SIGNAL(textChanged()), SLOT(textChanged()));
    inputVlay->addWidget(inputArea);

    // Results screen
    resultWidget = new QFrame;
    resultWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    resultWidget->setFrameStyle(QFrame::Box | QFrame::Plain);
    resultWidget->setLineWidth(int(RESULT_BORDER_WIDTH * fontMultiplier));
    widgetStack->addWidget(resultWidget);

    QVBoxLayout* resultVlay = new QVBoxLayout(resultWidget);
    resultVlay->setMargin(int(RESULT_MARGIN * fontMultiplier));
    resultVlay->setSpacing(int(RESULT_SPACING * fontMultiplier));

    //resultVlay->addStretch(1);

    resultPixmapLabel = new QLabel;
    resultPixmapLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    resultVlay->addWidget(resultPixmapLabel);
    resultVlay->setStretchFactor(resultPixmapLabel, 1);

    resultLabel = new QLabel;
    resultLabel->setFont(formFont);
    resultLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    resultLabel->setWordWrap(true);
    resultLabel->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Expanding);
    resultVlay->addWidget(resultLabel);

    QFont lexiconFont = qApp->font();
    lexiconFont.setPixelSize(int(LEXICON_FONT_PIXEL_SIZE * fontMultiplier));

    resultLexiconLabel = new QLabel;
    resultLexiconLabel->setFont(lexiconFont);
    resultLexiconLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    resultLexiconLabel->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Fixed);
    resultVlay->addWidget(resultLexiconLabel);

    QWidget* inputTitleWidget = createTitleWidget();
    inputVlay->addWidget(inputTitleWidget);

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
    int tabIndex = -1;
    bool tabDeletedBeforeCursor = false;
    int maxWords = (count > MAX_JUDGE_WORDS ? MAX_JUDGE_WORDS : count);
    for (int i = 0; lookIndex < text.length(); ++i) {
        QChar c = text.at(lookIndex);

        if (c.isLetter()) {
            if (wordLength == 0)
                ++numWords;

            if (numWords <= maxWords) {
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

            // Disallow more than max words by whiting them out
            else {
                text.replace(lookIndex, 1, "\n");
            }
            ++lookIndex;
        }
        else if ((lookIndex > 0) && c.isSpace() && (c != '\t') && !afterSpace)
        {
            text.replace(lookIndex, 1, "\n");
            afterSpace = true;
            ++lookIndex;
            wordLength = 0;
        }
        else {
            if (c == '\t')
                tabIndex = lookIndex;
            else
                wordLength = 0;

            text.remove(lookIndex, 1);
            if (i < origCursorPosition) {
                if (c == '\t')
                    tabDeletedBeforeCursor = true;
                ++deletedBeforeCursor;
            }
        }
    }

    // Treat the tab character as word separator if fewer than N words are
    // present
    if ((numWords < maxWords) && (tabIndex > 0)) {
        if (tabIndex >= text.length()) {
            text.append("\n");
            if (tabDeletedBeforeCursor)
                --deletedBeforeCursor;
        }
        else if ((tabIndex > 0) && (text.at(tabIndex - 1).isLetter()) &&
            text.at(tabIndex).isLetter())
        {
            text.insert(tabIndex, "\n");
            ++numWords;
            if (tabDeletedBeforeCursor)
                --deletedBeforeCursor;
        }
        tabIndex = -1;
    }

    text.replace(QRegExp("\\n+"), "\n");

    inputArea->setPlainText(text);
    int position = origCursorPosition - deletedBeforeCursor;
    if (position > text.length())
        position = text.length();
    cursor.setPosition(position);
    inputArea->setTextCursor(cursor);
    inputArea->blockSignals(false);

    bool doJudge = (tabIndex >= 0) && (numWords == count);
    if (!text.isEmpty()) {
        if (doJudge)
            judgeWord();
        else
            inputTimer->start(CLEAR_INPUT_DELAY);
    }
}

//---------------------------------------------------------------------------
//  currentChanged
//
//! Called when the current widget is changed.
//
//! @param index the index of the current widget
//---------------------------------------------------------------------------
void
JudgeDialog::currentChanged(int)
{
    countTimer->stop();
    inputTimer->stop();
    resultTimer->stop();
}

//---------------------------------------------------------------------------
//  clearResults
//
//! Clear the result area.
//---------------------------------------------------------------------------
void
JudgeDialog::clearResults()
{
    displayCount();
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
    resultLexiconLabel->setPalette(pal);

    ++clearResultsHold;

    resultLabel->setText(resultStr);
    resultLexiconLabel->setText("<br><br><font color=\"black\">Lexicon: " +
        lexicon + "</font>");
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
        foreach (const QString& word, acceptableWords) {
            stream << word;
            endl(stream);
        }
    }

    if (!unacceptableWords.empty()) {
        QFile file (logDirName + "/unacceptable.txt");
        file.open(QIODevice::Append | QIODevice::Text);
        QTextStream stream (&file);
        foreach (const QString& word, unacceptableWords) {
            stream << word;
            endl(stream);
        }
    }

    QFile file (logDirName + "/challenge.txt");
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream stream (&file);
    stream << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ")
        << (acceptable ? "acceptable   +++" : "unacceptable ---");
    foreach (const QString& word, words) {
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
//  displayCount
//
//! Display the word count screen.
//---------------------------------------------------------------------------
void
JudgeDialog::displayCount()
{
    count = 0;
    countLabel->clear();
    widgetStack->setCurrentWidget(countWidget);
}

//---------------------------------------------------------------------------
//  displayPassword
//
//! Display the password area.
//---------------------------------------------------------------------------
void
JudgeDialog::displayPassword()
{
    passwordLine->clear();
    passwordResultLabel->clear();

    if (password.isEmpty()) {
        passwordLabel->setText(NO_PASSWORD_INSTRUCTION_MESSAGE);
        passwordLine->hide();
        passwordResultLabel->hide();
    }
    else {
        passwordLabel->setText(PASSWORD_INSTRUCTION_MESSAGE);
        passwordLine->setReadOnly(false);
        passwordLine->show();
        passwordLine->setFocus();
        passwordResultLabel->show();
    }

    widgetStack->setCurrentWidget(passwordWidget);
    passwordTimer->start(CLEAR_PASSWORD_DELAY);
}

//---------------------------------------------------------------------------
//  displayIncorrectPassword
//
//! Display the incorrect password message.
//---------------------------------------------------------------------------
void
JudgeDialog::displayIncorrectPassword()
{
    passwordLine->setReadOnly(true);
    passwordResultLabel->setText(INCORRECT_PASSWORD_MESSAGE);
    QTimer::singleShot(CLEAR_INCORRECT_PASSWORD_DELAY, this,
        SLOT(displayCount()));
}

//---------------------------------------------------------------------------
//  displayInput
//
//! Display the input area.
//---------------------------------------------------------------------------
void
JudgeDialog::displayInput()
{
    inputInstLabel->setText(getInstructionMessage());
    inputArea->clear();
    inputArea->setFocus();
    widgetStack->setCurrentWidget(inputWidget);
    inputTimer->start(CLEAR_INPUT_DELAY);
}

////---------------------------------------------------------------------------
////  displayExit
////
////! Display instructions for exiting full screen mode.
////---------------------------------------------------------------------------
//void
//JudgeDialog::displayExit()
//{
//    countInstLabel->setText(COUNT_INSTRUCTION_MESSAGE +
//        "\n[ To exit, hold SHIFT and press the ESC key. ]");
//    exitTimer->start(CLEAR_EXIT_DELAY);
//}

//---------------------------------------------------------------------------
//  clearInput
//
//! Clear the input area.
//---------------------------------------------------------------------------
void
JudgeDialog::clearInput()
{
    inputArea->clear();
    inputTimer->start(CLEAR_INPUT_DELAY);
}

//---------------------------------------------------------------------------
//  clearExit
//
//! Clear instructions for exiting full screen mode.
//---------------------------------------------------------------------------
void
JudgeDialog::clearExit()
{
    exitTimer->stop();
    countInstLabel->setText(COUNT_INSTRUCTION_MESSAGE);
}

//---------------------------------------------------------------------------
//  passwordTextChanged
//
//! Called when the text of the password input area is changed. Restart the
//! password clear timer.
//---------------------------------------------------------------------------
void
JudgeDialog::passwordTextChanged()
{
    passwordTimer->start(CLEAR_PASSWORD_DELAY);
}

//---------------------------------------------------------------------------
//  passwordReturnPressed
//
//! Called when Return is pressed in the password input area. Validate the
//! password and exit if appropriate.
//---------------------------------------------------------------------------
void
JudgeDialog::passwordReturnPressed()
{
    passwordTimer->stop();
    QString text = passwordLine->text();
    if (password.isEmpty() || (text == password))
        accept();
    else
        displayIncorrectPassword();
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

    QWidget* currentWidget = widgetStack->currentWidget();

    if ((currentWidget == resultWidget) && !clearResultsHold) {
        clearResults();
    }

    else if (currentWidget == countWidget) {
        if (!countTimer->isActive()) {
            switch (event->key()) {
                case Qt::Key_1: count = 1; break;
                case Qt::Key_2: count = 2; break;
                case Qt::Key_3: count = 3; break;
                case Qt::Key_4: count = 4; break;
                case Qt::Key_5: count = 5; break;
                case Qt::Key_6: count = 6; break;
                case Qt::Key_7: count = 7; break;
                case Qt::Key_8: count = 8; break;
                default: break;
            }

            if (count) {
                clearExit();
                countLabel->setText(QString::number(count));
                countTimer->start(CLEAR_COUNT_DELAY);
            }
        }
    }

    else if (currentWidget == passwordWidget) {
        if (password.isEmpty() && ((event->key() == Qt::Key_Return) ||
            (event->key() == Qt::Key_Enter)))
        {
            accept();
        }
    }

    if (event->key() == Qt::Key_Escape) {
        if (currentWidget == countWidget) {
        //Qt::KeyboardModifiers modifiers = event->modifiers();
        //if (modifiers & Qt::ShiftModifier) {
            displayPassword();
        //}
        //else if ((currentWidget == countWidget) && !cleared) {
        //    displayExit();
        //}
        }
        else if (currentWidget == passwordWidget) {
            passwordTimer->stop();
            displayCount();
        }
        else if (currentWidget == inputWidget) {
            QString text = inputArea->toPlainText().simplified();
            if (text.isEmpty())
                displayCount();
            else
                clearInput();
        }
    }
    else
        event->ignore();
}

//---------------------------------------------------------------------------
//  getInstructionMessage
//
//! Get an instruction message, properly parameterized for the number of words
//! to be judged.
//
//! @return the message
//---------------------------------------------------------------------------
QString
JudgeDialog::getInstructionMessage()
{
    return INSTRUCTION_MESSAGE.arg(count).arg(
        count == 1 ? QString() : QString("s, separated by SPACE or ENTER"));
}

//---------------------------------------------------------------------------
//  createTitleWidget
//
//! Create a title widget containing version and lexicon information, etc.
//
//! @return the newly created widget
//---------------------------------------------------------------------------
QWidget*
JudgeDialog::createTitleWidget()
{
    QFont titleFont = qApp->font();
    titleFont.setPixelSize(int(TITLE_FONT_PIXEL_SIZE * fontMultiplier));

    QWidget* widget = new QWidget;

    QHBoxLayout* titleHlay = new QHBoxLayout(widget);

    QLabel* programLabel = new QLabel("Zyzzyva Word Judge\n"
                                      "Version " + ZYZZYVA_VERSION);
    programLabel->setFont(titleFont);
    programLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    titleHlay->addWidget(programLabel);
    titleHlay->setStretchFactor(programLabel, 1);

    QPixmap pixmap (":/zyzzyva-128x128");
    QLabel* pixmapLabel = new QLabel;
    pixmapLabel->setPixmap(pixmap);
    pixmapLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    titleHlay->addWidget(pixmapLabel);

    QDate date = Auxil::lexiconToDate(lexicon);
    QString dateStr;
    if (date.isValid())
        dateStr = date.toString("MMMM d, yyyy");
    QLabel* lexiconLabel = new QLabel("Lexicon: " + lexicon + "\n" + dateStr);
    lexiconLabel->setFont(titleFont);
    lexiconLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    titleHlay->addWidget(lexiconLabel);
    titleHlay->setStretchFactor(lexiconLabel, 1);

    return widget;
}
