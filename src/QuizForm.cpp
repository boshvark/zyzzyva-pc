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
#include "MainSettings.h"
#include "NewQuizDialog.h"
#include "Rand.h"
#include "QuizEngine.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "WordLineEdit.h"
#include "WordTableModel.h"
#include "WordTableView.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QColor>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimerEvent>
#include <QGridLayout>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

using namespace Defs;

const QString PAUSE_BUTTON = "&Pause";
const QString UNPAUSE_BUTTON = "Un&pause";

//---------------------------------------------------------------------------
//  vowelsFirstCmp
//
//! A character comparison function that sorts vowels before consonants as a
//! primary key, and alphabetically as a secondary key.
//
//! @param a character
//! @param another character
//---------------------------------------------------------------------------
bool
vowelsFirstCmp (const QChar& a, const QChar& b)
{
    if (Auxil::isVowel (a) && !Auxil::isVowel (b))
        return true;
    else if (!Auxil::isVowel (a) && Auxil::isVowel (b))
        return false;
    else
        return (a < b);
}

//---------------------------------------------------------------------------
//  consonantsFirstCmp
//
//! A character comparison function that sorts consonants before vowels as a
//! primary key, and alphabetically as a secondary key.
//
//! @param a character
//! @param another character
//---------------------------------------------------------------------------
bool
consonantsFirstCmp (const QChar& a, const QChar& b)
{
    if (Auxil::isVowel (a) && !Auxil::isVowel (b))
        return false;
    else if (!Auxil::isVowel (a) && Auxil::isVowel (b))
        return true;
    else
        return (a < b);
}

//---------------------------------------------------------------------------
//  QuizForm
//
//! Constructor.
//
//! @param we the word engine
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
QuizForm::QuizForm (WordEngine* we, QWidget* parent, Qt::WFlags f)
    : ActionForm (QuizFormType, parent, f), wordEngine (we),
    quizEngine (new QuizEngine (wordEngine)),
    numCanvasTiles (0), minCanvasTiles (7), minCanvasWidth (300),
    timerId (0), timerPaused (0),
    // FIXME: This dialog should be nonmodal!
    analyzeDialog (new AnalyzeQuizDialog (quizEngine, we, this))
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this, MARGIN, SPACING);
    Q_CHECK_PTR (mainHlay);

    QVBoxLayout* mainVlay = new QVBoxLayout (SPACING);
    Q_CHECK_PTR (mainVlay);
    mainHlay->addLayout (mainVlay);

    QHBoxLayout* topHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (topHlay);
    mainVlay->addLayout (topHlay);

    topHlay->addStretch (1);

    timerLabel = new QLabel;
    Q_CHECK_PTR (timerLabel);
    topHlay->addWidget (timerLabel);

    QHBoxLayout* quizBoxHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (quizBoxHlay);
    mainVlay->addLayout (quizBoxHlay);

    quizBoxHlay->addStretch (1);

    // Canvas for tile images - set default background color
    questionCanvas = new Q3Canvas (this);
    Q_CHECK_PTR (questionCanvas);
    setBackgroundColor (QColor (MainSettings::getQuizBackgroundColor()));

    questionCanvasView = new Q3CanvasView (questionCanvas);
    Q_CHECK_PTR (questionCanvasView);
    questionCanvasView->setVScrollBarMode (Q3ScrollView::AlwaysOff);
    questionCanvasView->setHScrollBarMode (Q3ScrollView::AlwaysOff);
    questionCanvasView->setResizePolicy (Q3ScrollView::AutoOneFit);
    questionCanvasView->setSizePolicy (QSizePolicy::Fixed,
                                       QSizePolicy::Fixed);
    quizBoxHlay->addWidget (questionCanvasView);

    quizBoxHlay->addStretch (1);

    responseView = new WordTableView (wordEngine);
    Q_CHECK_PTR (responseView);
    responseView->verticalHeader()->hide();
    mainVlay->addWidget (responseView);

    responseModel = new WordTableModel (wordEngine, this);
    Q_CHECK_PTR (responseModel);
    connect (responseModel, SIGNAL (wordsChanged()),
             responseView, SLOT (resizeAllColumnsToContents()));
    responseView->setModel (responseModel);

    // Question status
    QHBoxLayout* statusHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (statusHlay);
    mainVlay->addLayout (statusHlay);

    responseStatusLabel = new DefinitionLabel;
    Q_CHECK_PTR (responseStatusLabel);
    statusHlay->addWidget (responseStatusLabel);

    questionStatusLabel = new DefinitionLabel;
    Q_CHECK_PTR (questionStatusLabel);
    statusHlay->addWidget (questionStatusLabel);

    // Input line
    inputLine = new WordLineEdit;
    Q_CHECK_PTR (inputLine);
    WordValidator* validator = new WordValidator (inputLine);
    Q_CHECK_PTR (validator);
    inputLine->setValidator (validator);
    inputLine->setEnabled (false);
    connect (inputLine, SIGNAL (returnPressed()), SLOT (responseEntered()));
    mainVlay->addWidget (inputLine);

    // Button layout
    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QGridLayout* buttonGlay = new QGridLayout (2, 3, SPACING);
    Q_CHECK_PTR (buttonGlay);
    buttonHlay->addLayout (buttonGlay);

    // Buttons
    nextQuestionButton = new QPushButton ("&Next");
    Q_CHECK_PTR (nextQuestionButton);
    nextQuestionButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (nextQuestionButton, SIGNAL (clicked()),
             SLOT (nextQuestionClicked()));
    nextQuestionButton->setEnabled (false);
    buttonGlay->addWidget (nextQuestionButton, 0, 0, Qt::AlignHCenter);

    checkResponseButton = new QPushButton ("&Check Answers");
    Q_CHECK_PTR (checkResponseButton);
    checkResponseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (checkResponseButton, SIGNAL (clicked()),
             SLOT (checkResponseClicked()));
    checkResponseButton->setEnabled (false);
    buttonGlay->addWidget (checkResponseButton, 0, 1, Qt::AlignHCenter);

    pauseButton = new QPushButton (PAUSE_BUTTON);
    Q_CHECK_PTR (pauseButton);
    pauseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (pauseButton, SIGNAL (clicked()), SLOT (pauseClicked()));
    pauseButton->setEnabled (false);
    buttonGlay->addWidget (pauseButton, 0, 2, Qt::AlignHCenter);

    QPushButton* newQuizButton = new QPushButton ("New &Quiz...");
    Q_CHECK_PTR (newQuizButton);
    newQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (newQuizButton, SIGNAL (clicked()), SLOT (newQuizClicked()));
    buttonGlay->addWidget (newQuizButton, 1, 0, Qt::AlignHCenter);

    QPushButton* saveQuizButton = new QPushButton ("&Save Quiz...");
    Q_CHECK_PTR (saveQuizButton);
    saveQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (saveQuizButton, SIGNAL (clicked()), SLOT (saveQuizClicked()));
    buttonGlay->addWidget (saveQuizButton, 1, 1, Qt::AlignHCenter);

    analyzeButton = new QPushButton ("&Analyze Quiz...");
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
//  getStatusString
//
//! Returns the current status string.
//
//! @return the current status string
//---------------------------------------------------------------------------
QString
QuizForm::getStatusString() const
{
    return statusString;
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
        responseModel->addWord (response, WordTableModel::WordCorrect);
        //responseList->setSelected (item, true);
        //responseList->ensureItemVisible (item);
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

    updateStatusString();

    // If all correct answers have been provided, and the Quiz Auto Check
    // preference is set, act as if the Check Responses button has been
    // clicked
    if (MainSettings::getQuizAutoCheck() && 
        (quizEngine->getQuestionCorrect() == quizEngine->getQuestionTotal()))
    {
        checkResponseClicked();
    }
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
    QString filename = QFileDialog::getSaveFileName (this, "Save Quiz",
        Auxil::getQuizDir() + "/saved", "Zyzzyva Quiz Files (*.zzq)");

    if (filename.isEmpty())
        return;

    bool filenameEdited = false;
    if (!filename.endsWith (".zzq", false)) {
        filename += ".zzq";
        filenameEdited = true;
    }

    QFile file (filename);
    if (filenameEdited && file.exists()) {
        int code = QMessageBox::warning (0, "Overwrite Existing File?",
                                         "The file already exists.  "
                                         "Overwrite it?", QMessageBox::Yes,
                                         QMessageBox::No);
        if (code != QMessageBox::Yes)
            return;
    }

    // FIXME Qt4: QIODevice::Translate no longer exists!
    //if (!file.open (QIODevice::WriteOnly | QIODevice::Translate)) {
    if (!file.open (QIODevice::WriteOnly)) {
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
    NewQuizDialog* dialog = new NewQuizDialog (this);
    Q_CHECK_PTR (dialog);
    QuizSpec spec = quizEngine->getQuizSpec();
    spec.setProgress (QuizProgress());
    spec.setRandomAlgorithm (Rand::MarsagliaMwc);
    spec.setRandomSeed (0);
    spec.setRandomSeed2 (0);
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
        responseModel->addWord (*it, WordTableModel::WordMissed);
        analyzeDialog->addMissed (*it);
    }

    if ((quizEngine->numQuestions() > 0) && !quizEngine->onLastQuestion()) {
        nextQuestionButton->setEnabled (true);
        nextQuestionButton->setFocus();
    }
    else
        analyzeButton->setFocus();

    updateStatusString();
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
    setQuestionLabel (quizEngine->getQuestion());
    responseModel->clear();

    std::set<QString> correct = quizEngine->getQuestionCorrectResponses();
    std::set<QString>::iterator it;
    for (it = correct.begin(); it != correct.end(); ++it) {
        responseModel->addWord (*it, WordTableModel::WordCorrect);
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

    updateStatusString();
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
    Q3CanvasItemList canvasItems = questionCanvas->allItems();
    Q3CanvasItemList::iterator cItem;
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

    QString displayQuestion (question);
    QuizSpec quizSpec = quizEngine->getQuizSpec();
    QuizSpec::QuizType type = quizSpec.getType();

    // Anagram Quiz: Order letters according to preference
    if (type == QuizSpec::QuizAnagrams) {
        QString order = MainSettings::getQuizLetterOrder();
        int length = displayQuestion.length();

        if (order == Defs::QUIZ_LETTERS_RANDOM) {
            for (int i = 0; i < length; ++i) {
                int rnum = i + (std::rand() % (length - i));
                if (rnum == i)
                    continue;
                QChar tmp = displayQuestion[rnum];
                displayQuestion[rnum] = displayQuestion[i];
                displayQuestion[i] = tmp;
            }
        }

        else {
            QList<QChar> qchars;
            for (int i = 0; i < length; ++i)
                qchars.append (displayQuestion.at (i));

            if (order == Defs::QUIZ_LETTERS_VOWELS_FIRST) {
                qSort (qchars.begin(), qchars.end(), vowelsFirstCmp);
            }
            else if (order == Defs::QUIZ_LETTERS_CONSONANTS_FIRST) {
                qSort (qchars.begin(), qchars.end(), consonantsFirstCmp);
            }
            else if (order == Defs::QUIZ_LETTERS_ALPHA) {
                qSort (qchars.begin(), qchars.end());
            }

            char chars[MAX_WORD_LEN + 1];
            int i = 0;
            QListIterator<QChar> it (qchars);
            while (it.hasNext()) {
                chars[i] = it.next().toAscii();
                ++i;
            }
            chars[i] = 0;

            displayQuestion = chars;
        }
    }

    // Question is not an alphagram, or there are no tile images
    if (displayQuestion.contains (" ") || tileImages.empty()) {
        Q3CanvasText* text = new Q3CanvasText (displayQuestion,
                                               questionCanvas);
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
        setNumCanvasTiles (displayQuestion.length());

        QMap<QString,QImage>::iterator image;
        int x = QUIZ_TILE_MARGIN +
            ((numCanvasTiles - displayQuestion.length()) *
             (maxTileWidth + QUIZ_TILE_SPACING)) / 2;

        for (int i = 0; (i < numCanvasTiles) &&
                        (i < int (displayQuestion.length())); ++i)
        {
            QString letter = QString (displayQuestion[i]);
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
//  updateStatusString
//
//! Update the status string with the current quiz status.
//---------------------------------------------------------------------------
void
QuizForm::updateStatusString()
{
    QString status = "Question " +
        QString::number (quizEngine->getQuestionIndex() + 1) + "/" +
        QString::number (quizEngine->numQuestions());
    if (MainSettings::getQuizShowNumResponses()) {
        status += ", Correct: " +
            QString::number (quizEngine->getQuestionCorrect()) + "/" +
            QString::number (quizEngine->getQuestionTotal());
    }
    setStatusString (status);
}

//---------------------------------------------------------------------------
//  setStatusString
//
//! Set the status string, and 
//---------------------------------------------------------------------------
void
QuizForm::setStatusString (const QString& status)
{
    if (status == statusString)
        return;
    statusString = status;
    emit statusChanged (status);
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
//  setBackgroundColor
//
//! Set the background color of the quiz question area.
//
//! @param color the new color
//---------------------------------------------------------------------------
void
QuizForm::setBackgroundColor (const QColor& color)
{
    questionCanvas->setBackgroundColor (color);
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
        case QuizSpec::QuizAnagrams:
        return ((response.length() == question.length()) &&
            (wordEngine->alphagram (response) ==
             wordEngine->alphagram (question)));

        case QuizSpec::QuizHooks:
        return ((response.length() == (question.length() + 1)) &&
                ((question == response.right (question.length())) ||
                 (question == response.left (question.length()))));

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
