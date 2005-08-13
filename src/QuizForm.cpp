//---------------------------------------------------------------------------
// QuizForm.cpp
//
// A form for quizzing the user.
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

#include "QuizForm.h"
#include "AnalyzeQuizDialog.h"
#include "DefinitionLabel.h"
#include "ImageItem.h"
#include "NewQuizDialog.h"
#include "QuizEngine.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "WordLineEdit.h"
#include "WordListView.h"
#include "WordListViewItem.h"
#include "Auxil.h"
#include "Defs.h"
#include <qapplication.h>
#include <qcolor.h>
#include <qfiledialog.h>
#include <qhgroupbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qmessagebox.h>

using namespace Defs;

const QString PAUSE_BUTTON = "&Pause";
const QString UNPAUSE_BUTTON = "Un&pause";

//---------------------------------------------------------------------------
//  QuizForm
//
//! Constructor.
//
//! @param we the word engine
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
QuizForm::QuizForm (WordEngine* we, QWidget* parent, const char* name, WFlags
                    f)
    : ActionForm (QuizFormType, parent, name, f), wordEngine (we),
    quizEngine (new QuizEngine (wordEngine)),
    numCanvasTiles (0), minCanvasTiles (7), minCanvasWidth (300),
    timerId (0), timerPaused (0),
    analyzeDialog (new AnalyzeQuizDialog (quizEngine, we, this,
                                          "analyzeDialog", false))
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this, MARGIN, SPACING,
                                             "mainHlay");
    Q_CHECK_PTR (mainHlay);

    QVBoxLayout* mainVlay = new QVBoxLayout (SPACING, "mainVlay");
    Q_CHECK_PTR (mainVlay);
    mainHlay->addLayout (mainVlay);

    QHBoxLayout* topHlay = new QHBoxLayout (SPACING, "topHlay");
    Q_CHECK_PTR (topHlay);
    mainVlay->addLayout (topHlay);

    questionNumLabel = new QLabel (this, "questionNumLabel");
    Q_CHECK_PTR (questionNumLabel);
    topHlay->addWidget (questionNumLabel);

    topHlay->addStretch (1);

    timerLabel = new QLabel (this, "timerLabel");
    Q_CHECK_PTR (timerLabel);
    topHlay->addWidget (timerLabel);

    QHBoxLayout* quizBoxHlay = new QHBoxLayout (SPACING, "quizBoxHlay");
    Q_CHECK_PTR (quizBoxHlay);
    mainVlay->addLayout (quizBoxHlay);

    quizBoxHlay->addStretch (1);

    // Canvas for tile images - set default background color
    questionCanvas = new QCanvas (this, "questionCanvas");
    Q_CHECK_PTR (questionCanvas);
    questionCanvas->setBackgroundColor (QColor (192, 192, 192));

    questionCanvasView = new QCanvasView (questionCanvas, this,
                                          "questionCanvasView");
    Q_CHECK_PTR (questionCanvasView);
    questionCanvasView->setVScrollBarMode (QScrollView::AlwaysOff);
    questionCanvasView->setHScrollBarMode (QScrollView::AlwaysOff);
    questionCanvasView->setResizePolicy (QScrollView::AutoOneFit);
    questionCanvasView->setSizePolicy (QSizePolicy::Fixed,
                                       QSizePolicy::Fixed);
    quizBoxHlay->addWidget (questionCanvasView);

    quizBoxHlay->addStretch (1);

    responseList = new WordListView (wordEngine, this, "responseList");
    Q_CHECK_PTR (responseList);
    mainVlay->addWidget (responseList);

    // Question status
    QHBoxLayout* statusHlay = new QHBoxLayout (SPACING, "questionStatusHlay");
    Q_CHECK_PTR (statusHlay);
    mainVlay->addLayout (statusHlay);

    responseStatusLabel = new DefinitionLabel (this, "responseStatusLabel");
    Q_CHECK_PTR (responseStatusLabel);
    statusHlay->addWidget (responseStatusLabel);

    questionStatusLabel = new DefinitionLabel (this, "questionStatusLabel");
    Q_CHECK_PTR (questionStatusLabel);
    statusHlay->addWidget (questionStatusLabel);

    // Input line
    inputLine = new WordLineEdit (this, "inputLine");
    Q_CHECK_PTR (inputLine);
    WordValidator* validator = new WordValidator (inputLine);
    Q_CHECK_PTR (validator);
    inputLine->setValidator (validator);
    inputLine->setEnabled (false);
    connect (inputLine, SIGNAL (returnPressed()), SLOT (responseEntered()));
    mainVlay->addWidget (inputLine);

    // Button layout
    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QGridLayout* buttonGlay = new QGridLayout (2, 3, SPACING, "buttonGlay");
    Q_CHECK_PTR (buttonGlay);
    buttonHlay->addLayout (buttonGlay);

    // Buttons
    nextQuestionButton = new QPushButton ("&Next", this,
                                          "nextQuestionButton");
    Q_CHECK_PTR (nextQuestionButton);
    nextQuestionButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (nextQuestionButton, SIGNAL (clicked()),
             SLOT (nextQuestionClicked()));
    nextQuestionButton->setEnabled (false);
    buttonGlay->addWidget (nextQuestionButton, 0, 0, Qt::AlignHCenter);

    checkResponseButton = new QPushButton ("&Check Answers", this,
                                           "checkResponseButton");
    Q_CHECK_PTR (checkResponseButton);
    checkResponseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (checkResponseButton, SIGNAL (clicked()),
             SLOT (checkResponseClicked()));
    checkResponseButton->setEnabled (false);
    buttonGlay->addWidget (checkResponseButton, 0, 1, Qt::AlignHCenter);

    pauseButton = new QPushButton (PAUSE_BUTTON, this, "pauseButton");
    Q_CHECK_PTR (pauseButton);
    pauseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (pauseButton, SIGNAL (clicked()), SLOT (pauseClicked()));
    pauseButton->setEnabled (false);
    buttonGlay->addWidget (pauseButton, 0, 2, Qt::AlignHCenter);

    QPushButton* newQuizButton = new QPushButton ("New &Quiz...", this,
                                                  "newQuizButton");
    Q_CHECK_PTR (newQuizButton);
    newQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (newQuizButton, SIGNAL (clicked()), SLOT (newQuizClicked()));
    buttonGlay->addWidget (newQuizButton, 1, 0, Qt::AlignHCenter);

    QPushButton* saveQuizButton = new QPushButton ("&Save Quiz...", this,
                                                   "saveQuizButton");
    Q_CHECK_PTR (saveQuizButton);
    saveQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (saveQuizButton, SIGNAL (clicked()), SLOT (saveQuizClicked()));
    buttonGlay->addWidget (saveQuizButton, 1, 1, Qt::AlignHCenter);

    analyzeButton = new QPushButton ("&Analyze Quiz...", this,
                                     "analyzeButton");
    Q_CHECK_PTR (analyzeButton);
    analyzeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (analyzeButton, SIGNAL (clicked()), SLOT (analyzeClicked()));
    buttonGlay->addWidget (analyzeButton, 1, 2, Qt::AlignHCenter);

    buttonHlay->addStretch (1);
}

//---------------------------------------------------------------------------
//  ~QuizForm
//
//! Destructor.
//---------------------------------------------------------------------------
QuizForm::~QuizForm()
{
    delete quizEngine;
}

//---------------------------------------------------------------------------
//  responseEntered
//
//! Called when a response is entered into the input line.
//---------------------------------------------------------------------------
void
QuizForm::responseEntered()
{
    QString response = inputLine->text();
    if (response.isEmpty() && checkResponseButton->isEnabled()) {
        checkResponseClicked();
        return;
    }

    if (!responseMatchesQuestion (response)) {
        QMessageBox::warning (this, "Response does not match question",
                              "Sorry, your response does not match the "
                              "question.");
        return;
    }

    QuizEngine::ResponseStatus status = quizEngine->respond (response);
    QString displayResponse = response;
    QString statusStr = "";

    if (status == QuizEngine::Correct) {
        WordListViewItem* item = responseList->addWord (response);
        responseList->setSelected (item, true);
        responseList->ensureItemVisible (item);
        item->setTextColor (VALID_CORRECT_WORD_COLOR);
        statusStr = "<font color=\"blue\">Correct</font>";
        analyzeDialog->updateStats();
    }
    else if (status == QuizEngine::Incorrect) {
        displayResponse += "*";
        statusStr = "<font color=\"red\">Incorrect</font>";
        analyzeDialog->addIncorrect (response);
    }
    else if (status == QuizEngine::Duplicate) {
        statusStr = "<font color=\"purple\">Duplicate</font>";
    }
    inputLine->clear();

    // Update the stats if the stats are already shown
    if (!questionStatusLabel->text().isEmpty())
        updateStats();

    // Update the response status label
    responseStatusLabel->setText (displayResponse + " : " + statusStr);

    // Restart the timer, if the timer runs per response
    if (timerId && (timerSpec.getType() == PerResponse))
        startNewTimer();
}


//---------------------------------------------------------------------------
//  newQuiz
//
//! Start a new quiz based on a quiz specification.
//
//! @param spec the quiz specification
//---------------------------------------------------------------------------
void
QuizForm::newQuiz (const QuizSpec& spec)
{
    timerSpec = spec.getTimerSpec();
    QApplication::setOverrideCursor (Qt::waitCursor);
    quizEngine->newQuiz (spec);
    QApplication::restoreOverrideCursor();
    startQuestion();
    analyzeDialog->newQuiz (spec);
    if (quizEngine->getQuestionComplete())
        checkResponseClicked();
}

//---------------------------------------------------------------------------
//  saveQuizClicked
//
//! Called when the New Quiz button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::saveQuizClicked()
{
    pauseTimer();

    // XXX: This code is copied wholesale from NewQuizDialog::saveQuiz!
    QString filename = QFileDialog::getSaveFileName
        (Auxil::getQuizDir() + "/saved", "Zyzzyva Quiz Files (*.zzq)",
         this, "saveDialog", "Save Quiz");

    if (filename.isEmpty())
        return;

    if (!filename.endsWith (".zzq", false))
        filename += ".zzq";

    QFile file (filename);
    if (file.exists()) {
        int code = QMessageBox::warning (0, "Overwrite Existing File?",
                                         "The file already exists.  "
                                         "Overwrite it?", QMessageBox::Yes,
                                         QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    if (!file.open (IO_WriteOnly)) {
        QMessageBox::warning (this, "Error Saving Quiz",
                              "Cannot save quiz:\n" + file.errorString() +
                              ".");
        return;
    }

    QDomImplementation implementation;
    QDomDocument document (implementation.createDocumentType
                           ("zyzzyva-quiz", QString::null,
                            "http://pietdepsi.com/dtd/zyzzyva-quiz.dtd"));

    document.appendChild (quizEngine->getQuizSpec().asDomElement());

    //// XXX: There should be a programmatic way to write the <?xml?> header
    //// based on the QDomImplementation, shouldn't there?
    QTextStream stream (&file);
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        << document.toString();
}

//---------------------------------------------------------------------------
//  newQuizClicked
//
//! Called when the New Quiz button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::newQuizClicked()
{
    pauseTimer();
    NewQuizDialog* dialog = new NewQuizDialog (this, "newQuizDialog", true);
    Q_CHECK_PTR (dialog);
    QuizSpec spec = quizEngine->getQuizSpec();
    spec.setProgress (QuizProgress());
    dialog->setQuizSpec (spec);
    int code = dialog->exec();
    if (code != QDialog::Accepted) {
        unpauseTimer();
        return;
    }
    newQuiz (dialog->getQuizSpec());
}

//---------------------------------------------------------------------------
//  pauseTimer
//
//! Pause the timer.  In reality the timer keeps going, but it appears paused
//! because it has no effect while .
//---------------------------------------------------------------------------
void
QuizForm::pauseTimer()
{
    if (!timerId)
        return;
    ++timerPaused;
    pauseButton->setText (UNPAUSE_BUTTON);
}

//---------------------------------------------------------------------------
//  unpauseTimer
//
//! Unpauses the timer.
//---------------------------------------------------------------------------
void
QuizForm::unpauseTimer()
{
    if (!timerId)
        return;
    --timerPaused;
    if (timerPaused == 0)
        pauseButton->setText (PAUSE_BUTTON);
}

//---------------------------------------------------------------------------
//  nextQuestionClicked
//
//! Called when the New Question button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::nextQuestionClicked()
{
    if (!quizEngine->nextQuestion())
        QMessageBox::warning (this, "Error getting next question",
                              "Error getting next question.");
    startQuestion();
    analyzeDialog->updateStats();
}

//---------------------------------------------------------------------------
//  checkResponseClicked
//
//! Called when the Check Responses button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::checkResponseClicked()
{
    killActiveTimer();
    updateStats();
    inputLine->setText ("");
    inputLine->setEnabled (false);
    checkResponseButton->setEnabled (false);
    quizEngine->completeQuestion();
    analyzeDialog->updateStats();

    QStringList unanswered = quizEngine->getMissed();
    QStringList::iterator it;
    for (it = unanswered.begin(); it != unanswered.end(); ++it) {
        WordListViewItem* item = responseList->addWord (*it);
        item->setTextColor (VALID_MISSED_WORD_COLOR);
        analyzeDialog->addMissed (*it);
    }

    if ((quizEngine->numQuestions() > 0) && !quizEngine->onLastQuestion()) {
        nextQuestionButton->setEnabled (true);
        nextQuestionButton->setFocus();
    }
    else
        analyzeButton->setFocus();
}

//---------------------------------------------------------------------------
//  pauseClicked
//
//! Called when the Pause button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::pauseClicked()
{
    if (!timerId)
        return;

    if (pauseButton->text() == PAUSE_BUTTON)
        pauseTimer();
    else if (pauseButton->text() == UNPAUSE_BUTTON)
        unpauseTimer();
}

//---------------------------------------------------------------------------
//  analyzeClicked
//
//! Called when the Analyze button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::analyzeClicked()
{
    analyzeDialog->show();
}

//---------------------------------------------------------------------------
//  startQuestion
//
//! Update the form when starting a question.
//---------------------------------------------------------------------------
void
QuizForm::startQuestion()
{
    clearStats();
    setQuestionNum (quizEngine->getQuestionIndex() + 1,
                    quizEngine->numQuestions());
    setQuestionLabel (quizEngine->getQuestion());
    responseList->clear();

    std::set<QString> correct = quizEngine->getQuestionCorrectResponses();
    std::set<QString>::iterator it;
    for (it = correct.begin(); it != correct.end(); ++it) {
        WordListViewItem* item = responseList->addWord (*it);
        item->setTextColor (VALID_CORRECT_WORD_COLOR);
    }

    responseStatusLabel->setText ("");
    inputLine->setEnabled (true);
    checkResponseButton->setEnabled (true);
    nextQuestionButton->setEnabled (false);
    inputLine->setFocus();
    if (timerSpec.getType() == NoTimer)
        clearTimerDisplay();
    else
        startNewTimer();
}

//---------------------------------------------------------------------------
//  startNewTimer
//
//! Start a new timer, and update the form appropriately.
//---------------------------------------------------------------------------
void
QuizForm::startNewTimer()
{
    killActiveTimer();
    timerRemaining = timerSpec.getDuration();
    setTimerDisplay (timerRemaining);
    timerId = startTimer (1000);
    pauseButton->setEnabled (true);
}

//---------------------------------------------------------------------------
//  killActiveTimer
//
//! Kill the active timer, and update the form appropriately.
//---------------------------------------------------------------------------
void
QuizForm::killActiveTimer()
{
    if (!timerId)
        return;

    while (timerPaused)
        unpauseTimer();
    killTimer (timerId);
    timerId = 0;
    pauseButton->setEnabled (false);
}

//---------------------------------------------------------------------------
//  updateStats
//
//! Update the recall and precision statistics.
//---------------------------------------------------------------------------
void
QuizForm::updateStats()
{
    setQuestionStatus (quizEngine->getQuestionCorrect(),
                       quizEngine->getQuestionTotal());
}

//---------------------------------------------------------------------------
//  clearStats
//
//! Clear the recall and precision statistics.
//---------------------------------------------------------------------------
void
QuizForm::clearStats()
{
    questionStatusLabel->setText ("");
}

//---------------------------------------------------------------------------
//  clearQuestionNum
//
//! Clear the question number label.
//---------------------------------------------------------------------------
void
QuizForm::clearQuestionNum()
{
    questionNumLabel->setText ("");
}

//---------------------------------------------------------------------------
//  clearTimerDisplay
//
//! Clear the timer label.
//---------------------------------------------------------------------------
void
QuizForm::clearTimerDisplay()
{
    timerLabel->setText ("");
}

//---------------------------------------------------------------------------
//  clearCanvas
//
//! Clear the question canvas and delete all its canvas items.
//---------------------------------------------------------------------------
void
QuizForm::clearCanvas()
{
    QCanvasItemList canvasItems = questionCanvas->allItems();
    QCanvasItemList::iterator cItem;
    for (cItem = canvasItems.begin(); cItem != canvasItems.end(); ++cItem)
        delete *cItem;
    canvasItems.clear();
    minimizeCanvas();
}

//---------------------------------------------------------------------------
//  minimizeCanvas
//
//! Resize the canvas to its minimum size given the current tile theme.
//---------------------------------------------------------------------------
void
QuizForm::minimizeCanvas()
{
    if (tileImages.empty())
        questionCanvas->resize (minCanvasWidth, (2 * QUIZ_TILE_MARGIN) +
                                maxTileHeight);
    else
        setNumCanvasTiles (minCanvasTiles);
    reflowLayout();
}

//---------------------------------------------------------------------------
//  setNumCanvasTiles
//
//! Resize the canvas to be able to display a certain number of tiles.
//! @param num the number of tiles
//---------------------------------------------------------------------------
void
QuizForm::setNumCanvasTiles (int num)
{
    if (num < minCanvasTiles)
        num = minCanvasTiles;
    numCanvasTiles = num;
    int width = (2 * QUIZ_TILE_MARGIN) + (num * maxTileWidth) +
                ((num - 1) * QUIZ_TILE_SPACING);
    int height = (2 * QUIZ_TILE_MARGIN) + maxTileHeight;
    questionCanvas->resize (width, height);
}

//---------------------------------------------------------------------------
//  setQuestionNum
//
//! Set the current question number and the total number of questions.
//
//! @param num the current question number
//! @param total the total number of questions
//---------------------------------------------------------------------------
void
QuizForm::setQuestionNum (int num, int total)
{
    questionNumLabel->setText ("Question " + QString::number (num)
                               + " of " + QString::number (total));
}

//---------------------------------------------------------------------------
//  setQuestionLabel
//
//! Display the current question in the question label area.
//
//! @param question the question
//---------------------------------------------------------------------------
void
QuizForm::setQuestionLabel (const QString& question)
{
    clearCanvas();

    // Question is not an alphagram, or there are no tile images
    if (question.contains (" ") || tileImages.empty()) {
        QCanvasText* text = new QCanvasText (question, questionCanvas);
        QRect rect = text->boundingRect();
        int width = (2 * QUIZ_TILE_MARGIN) + rect.width();
        if (width < minCanvasWidth)
            width = minCanvasWidth;
        questionCanvas->resize (width, (2 * QUIZ_TILE_MARGIN) +
                                rect.height());
        text->move ((width - rect.width()) / 2, QUIZ_TILE_MARGIN);
        text->show();
        questionCanvas->setAllChanged();
    }

    else {
        setNumCanvasTiles (question.length());

        QMap<QString,QImage>::iterator image;
        int x = QUIZ_TILE_MARGIN +
            ((numCanvasTiles - question.length()) *
             (maxTileWidth + QUIZ_TILE_SPACING)) / 2;

        for (int i = 0; (i < numCanvasTiles) && (i < int (question.length()));
             ++i)
        {
            QString letter = question[i];
            if (letter == "?")
                letter = "_";
            image = tileImages.find (letter);
            if (image == tileImages.end())
                qWarning ("Did not find letter '" + letter +
                          "' in tiles map!");
            else {
                ImageItem* item = new ImageItem (*image, questionCanvas);
                item->move (x, QUIZ_TILE_MARGIN);
                item->setZ (0);
                item->show();
            }
            x += maxTileWidth + QUIZ_TILE_SPACING;
        }
    }

    reflowLayout();
}

//---------------------------------------------------------------------------
//  setQuestionStatus
//
//! Set the status of the question after the user clicks the Check button.
//
//! @param correct the number of correct responses
//! @param total the total number of correct answers
//---------------------------------------------------------------------------
void
QuizForm::setQuestionStatus (int correct, int total)
{
    questionStatusLabel->setText ("Correct: " + QString::number (correct)
                          + " of " + QString::number (total));
}

//---------------------------------------------------------------------------
//  setTimerDisplay
//
//! Display a number of seconds in the timer label.
//
//! \param seconds the number of seconds to display
//---------------------------------------------------------------------------
void
QuizForm::setTimerDisplay (int seconds)
{
    int hours = 0;
    int minutes = 0;
    if (seconds >= 3600) {
        hours = seconds / 3600;
        seconds %= 3600;
    }
    if (seconds >= 60) {
        minutes = seconds / 60;
        seconds %= 60;
    }
    QString text;
    if (hours > 0)
        text.sprintf ("%d:%02d:%02d", hours, minutes, seconds);
    else
        text.sprintf ("%02d:%02d", minutes, seconds);
    timerLabel->setText (text);
}

//---------------------------------------------------------------------------
//  clearTileTheme
//
//! Clear the current tile theme definition.
//---------------------------------------------------------------------------
void
QuizForm::clearTileTheme()
{
    tileImages.clear();
    maxTileWidth = 25;
    maxTileHeight = 25;
}

//---------------------------------------------------------------------------
//  setTileTheme
//
//! Load tile images according to a theme definition.  Find the tile images in
//! a data subdirectory named after the theme.
//
//! @param theme the name of the theme
//---------------------------------------------------------------------------
void
QuizForm::setTileTheme (const QString& theme)
{
    clearTileTheme();
    if (!theme.isEmpty()) {
        QString tilesDir = Auxil::getTilesDir();

        QStringList tilesList;
        for (char c = 'A'; c <= 'Z'; ++c)
            tilesList << QString (QChar (c));
        tilesList << "_";

        QStringList::iterator it;
        for (it = tilesList.begin(); it != tilesList.end(); ++it) {
            QImage image (tilesDir + "/" + theme + "/" + *it + ".png");
            tileImages.insert (*it, image);

            if (image.width() > maxTileWidth)
                maxTileWidth = image.width();
            if (image.height() > maxTileHeight)
                maxTileHeight = image.height();
        }
    }

    QString question = quizEngine->getQuestion();
    if (question.isEmpty())
        minimizeCanvas();
    else
        setQuestionLabel (question);
}

//---------------------------------------------------------------------------
//  reflowLayout
//
//! Force the quiz form layout to be reflowed.  Generally called after
//! resizing the canvas.  XXX: This whole method is a big kludge.  There must
//! be a better way to do this.
//---------------------------------------------------------------------------
void
QuizForm::reflowLayout()
{
    QString text = questionStatusLabel->text();
    questionStatusLabel->setText ("foo blah blah");
    questionStatusLabel->setText (text);
    questionCanvas->setAllChanged();
    questionCanvas->update();
}

//---------------------------------------------------------------------------
//  responseMatchesQuestion
//
//! Determine whether a response could possibly be a correct answer to the
//! current quiz question.
//
//! @param response the response to examine
//
//! @return true if the response could be valid, false otherwise
//---------------------------------------------------------------------------
bool
QuizForm::responseMatchesQuestion (const QString& response) const
{
    QString question = quizEngine->getQuestion();
    QuizSpec spec = quizEngine->getQuizSpec();
    switch (spec.getType()) {
        case QuizAnagrams:
        return ((response.length() == question.length()) &&
            (wordEngine->alphagram (response) ==
             wordEngine->alphagram (question)));

        default: break;
    }
    return true;
}

//---------------------------------------------------------------------------
//  timerEvent
//
//! Reimplementation of QObject::timerEvent.  Called when a timer event
//! occurs.  Decrements the time remaining, unless the timer is paused.
//
//! \param event the timer event
//---------------------------------------------------------------------------
void
QuizForm::timerEvent (QTimerEvent* event)
{
    if ((event->timerId() != timerId) || timerPaused)
        return;

    --timerRemaining;
    setTimerDisplay (timerRemaining);
    if (timerRemaining == 0)
        checkResponseClicked();
}
