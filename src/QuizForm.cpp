//---------------------------------------------------------------------------
// QuizForm.cpp
//
// A form for quizzing the user.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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
#include "MainSettings.h"
#include "MainWindow.h"
#include "NewQuizDialog.h"
#include "Rand.h"
#include "QuizCanvas.h"
#include "QuizEngine.h"
#include "QuizQuestionLabel.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "WordLineEdit.h"
#include "WordTableModel.h"
#include "WordTableView.h"
#include "ZPushButton.h"
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
//! @param a a character
//! @param b another character
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
    timerId (0), timerPaused (0), checkBringsJudgment (true),
    // FIXME: This dialog should be nonmodal!
    analyzeDialog (new AnalyzeQuizDialog (quizEngine, we, this,
                                          Qt::WindowMinMaxButtonsHint))
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this);
    mainHlay->setMargin (MARGIN);
    mainHlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainHlay);

    QVBoxLayout* mainVlay = new QVBoxLayout;
    mainVlay->setSpacing (SPACING);
    Q_CHECK_PTR (mainVlay);
    mainHlay->addLayout (mainVlay);

    QHBoxLayout* topHlay = new QHBoxLayout;
    topHlay->setSpacing (SPACING);
    Q_CHECK_PTR (topHlay);
    mainVlay->addLayout (topHlay);

    quizTypeLabel = new QLabel;
    Q_CHECK_PTR (quizTypeLabel);
    topHlay->addWidget (quizTypeLabel);

    topHlay->addStretch (1);

    timerLabel = new QLabel;
    Q_CHECK_PTR (timerLabel);
    topHlay->addWidget (timerLabel);

    QHBoxLayout* quizBoxHlay = new QHBoxLayout;
    quizBoxHlay->setSpacing (SPACING);
    Q_CHECK_PTR (quizBoxHlay);
    mainVlay->addLayout (quizBoxHlay);

    quizBoxHlay->addStretch (1);

    questionStack = new QStackedWidget;
    Q_CHECK_PTR (questionStack);
    questionStack->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    quizBoxHlay->addWidget (questionStack);

    // Canvas for tile images - set default background color
    questionCanvas = new QuizCanvas;
    Q_CHECK_PTR (questionCanvas);
    questionCanvas->setPalette
        (QPalette (MainSettings::getQuizBackgroundColor()));
    questionCanvas->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    questionStack->addWidget (questionCanvas);

    questionLabel = new QuizQuestionLabel;
    Q_CHECK_PTR (questionLabel);
    questionLabel->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    questionStack->addWidget (questionLabel);

    quizBoxHlay->addStretch (1);

    questionSpecLabel = new QLabel;
    Q_CHECK_PTR (questionSpecLabel);
    questionSpecLabel->setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);
    questionStack->addWidget (questionSpecLabel);

    QHBoxLayout* letterOrderHlay = new QHBoxLayout;
    Q_CHECK_PTR (letterOrderHlay);
    mainVlay->addLayout (letterOrderHlay);

    letterOrderHlay->addStretch (1);

    letterOrderLabel = new QLabel ("Letter order:");
    Q_CHECK_PTR (letterOrderLabel);
    letterOrderHlay->addWidget (letterOrderLabel);

    alphaOrderButton = new ZPushButton ("&Alpha");
    Q_CHECK_PTR (alphaOrderButton);
    alphaOrderButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (alphaOrderButton, SIGNAL (clicked()),
             SLOT (alphaOrderClicked()));
    letterOrderHlay->addWidget (alphaOrderButton);

    randomOrderButton = new ZPushButton ("&Random");
    Q_CHECK_PTR (randomOrderButton);
    randomOrderButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (randomOrderButton, SIGNAL (clicked()),
             SLOT (randomOrderClicked()));
    letterOrderHlay->addWidget (randomOrderButton);

    letterOrderHlay->addStretch (1);

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
    QHBoxLayout* statusHlay = new QHBoxLayout;
    statusHlay->setSpacing (SPACING);
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
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QGridLayout* buttonGlay = new QGridLayout;
    buttonGlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonGlay);
    buttonHlay->addLayout (buttonGlay);

    // Buttons
    nextQuestionButton = new ZPushButton ("&Next");
    Q_CHECK_PTR (nextQuestionButton);
    nextQuestionButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (nextQuestionButton, SIGNAL (clicked()),
             SLOT (nextQuestionClicked()));
    nextQuestionButton->setEnabled (false);
    buttonGlay->addWidget (nextQuestionButton, 0, 0, Qt::AlignHCenter);

    checkResponseButton = new ZPushButton ("&Check Answers");
    Q_CHECK_PTR (checkResponseButton);
    checkResponseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (checkResponseButton, SIGNAL (clicked()),
             SLOT (checkResponseClicked()));
    checkResponseButton->setEnabled (false);
    buttonGlay->addWidget (checkResponseButton, 0, 1, Qt::AlignHCenter);

    markMissedButton = new ZPushButton ("Mark as &Missed");
    Q_CHECK_PTR (markMissedButton);
    markMissedButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (markMissedButton, SIGNAL (clicked()),
             SLOT (markMissedClicked()));
    buttonGlay->addWidget (markMissedButton, 0, 2, Qt::AlignHCenter);

    pauseButton = new ZPushButton (PAUSE_BUTTON);
    Q_CHECK_PTR (pauseButton);
    pauseButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (pauseButton, SIGNAL (clicked()), SLOT (pauseClicked()));
    pauseButton->setEnabled (false);
    buttonGlay->addWidget (pauseButton, 0, 3, Qt::AlignHCenter);

    flashcardCbox = new QCheckBox ("Flashcard M&ode");
    Q_CHECK_PTR (flashcardCbox);
    connect (flashcardCbox, SIGNAL (stateChanged (int)),
             SLOT (flashcardStateChanged (int)));
    buttonGlay->addWidget (flashcardCbox, 1, 0, Qt::AlignHCenter);

    ZPushButton* newQuizButton = new ZPushButton ("New &Quiz...");
    Q_CHECK_PTR (newQuizButton);
    newQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (newQuizButton, SIGNAL (clicked()), SLOT (newQuizClicked()));
    buttonGlay->addWidget (newQuizButton, 1, 1, Qt::AlignHCenter);

    ZPushButton* saveQuizButton = new ZPushButton ("&Save Quiz...");
    Q_CHECK_PTR (saveQuizButton);
    saveQuizButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (saveQuizButton, SIGNAL (clicked()), SLOT (saveQuizClicked()));
    buttonGlay->addWidget (saveQuizButton, 1, 2, Qt::AlignHCenter);

    analyzeButton = new ZPushButton ("Analy&ze Quiz...");
    Q_CHECK_PTR (analyzeButton);
    analyzeButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (analyzeButton, SIGNAL (clicked()), SLOT (analyzeClicked()));
    buttonGlay->addWidget (analyzeButton, 1, 3, Qt::AlignHCenter);

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
        responseModel->addWord (WordTableModel::WordItem
                                (response, WordTableModel::WordCorrect),
                                true);
        responseView->scrollTo (responseModel->sibling
                                (responseModel->getLastAddedIndex(), 0,
                                 QModelIndex()));
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
        if (MainSettings::getQuizAutoAdvance() &&
            nextQuestionButton->isEnabled())
        {
            nextQuestionClicked();
        }
    }

    // End question immediately after an incorrect response if the Quiz Auto
    // End After Incorrect preference is set
    else if (MainSettings::getQuizAutoEndAfterIncorrect() &&
             (status == QuizEngine::Incorrect))
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
//! @return true if the quiz is successfully started, false otherwise
//---------------------------------------------------------------------------
bool
QuizForm::newQuiz (const QuizSpec& spec)
{
    timerSpec = spec.getTimerSpec();

    // Enable or disable Alpha and Random buttons depending on Quiz type
    bool enableLetterOrder = false;
    switch (spec.getType()) {
        case QuizSpec::QuizAnagrams:
        case QuizSpec::QuizSubanagrams:
        case QuizSpec::QuizAnagramJumble:
        case QuizSpec::QuizSubanagramJumble:
        case QuizSpec::QuizAnagramHooks:
        enableLetterOrder = true;
        break;

        default:
        enableLetterOrder = false;
        break;
    }
    letterOrderLabel->setEnabled (enableLetterOrder);
    alphaOrderButton->setEnabled (enableLetterOrder);
    randomOrderButton->setEnabled (enableLetterOrder);

    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));
    bool ok = quizEngine->newQuiz (spec);
    QApplication::restoreOverrideCursor();

    if (!ok) {
        QMessageBox::warning (MainWindow::getInstance(), "Cannot Start Quiz",
                              "The quiz cannot be started because no "
                              "questions were found.");
        return false;
    }

    quizTypeLabel->setText (Auxil::quizTypeToString
                            (quizEngine->getQuizSpec().getType()));
    startQuestion();
    analyzeDialog->newQuiz (spec);
    if (quizEngine->getQuestionComplete())
        checkResponseClicked();

    flashcardCbox->setChecked (MainSettings::getQuizUseFlashcardMode());

    return true;
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
    if (!filename.endsWith (".zzq", Qt::CaseInsensitive)) {
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

    if (!file.open (QIODevice::WriteOnly | QIODevice::Text)) {
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
//  alphaOrderClicked
//
//! Called when the Alpha button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::alphaOrderClicked()
{
    setQuestionLabel (quizEngine->getQuestion(), Defs::QUIZ_LETTERS_ALPHA);
}

//---------------------------------------------------------------------------
//  randomOrderClicked
//
//! Called when the Random button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::randomOrderClicked()
{
    setQuestionLabel (quizEngine->getQuestion(), Defs::QUIZ_LETTERS_RANDOM);
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
    NewQuizDialog* dialog = new NewQuizDialog (wordEngine, this);
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
    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));

    // If checking responses does not bring judgment, then feed all correct
    // responses to the quiz engine
    if (!checkBringsJudgment) {
        QStringList unanswered = quizEngine->getMissed();
        QStringListIterator it (unanswered);
        while (it.hasNext()) {
            QString word = it.next();
            quizEngine->respond (word);
            responseModel->addWord (WordTableModel::WordItem
                                    (word, WordTableModel::WordCorrect),
                                    true);
        }
    }

    killActiveTimer();
    updateStats();
    inputLine->setText ("");
    inputLine->setEnabled (false);
    checkResponseButton->setEnabled (false);
    quizEngine->completeQuestion();
    analyzeDialog->updateStats();

    // Disable the Mark as Missed button if all responses were missed
    if (quizEngine->getQuestionCorrect() == 0)
        markMissedButton->setEnabled (false);

    QStringList unanswered = quizEngine->getMissed();
    if (unanswered.empty()) {
        responseModel->clearLastAddedIndex();
    }
    else {
        QStringList::iterator it;
        QList<WordTableModel::WordItem> wordItems;
        for (it = unanswered.begin(); it != unanswered.end(); ++it) {
            wordItems.append (WordTableModel::WordItem
                              (*it, WordTableModel::WordMissed));
        }
        responseModel->addWords (wordItems);
        analyzeDialog->addMissed (unanswered);
    }

    if ((quizEngine->numQuestions() > 0) && !quizEngine->onLastQuestion()) {
        nextQuestionButton->setEnabled (true);
        nextQuestionButton->setFocus();
    }
    else
        analyzeButton->setFocus();

    updateStatusString();

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  markMissedClicked
//
//! Called when the Mark as Missed button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::markMissedClicked()
{
    quizEngine->markQuestionAsMissed();
    responseModel->clear();
    markMissedButton->setEnabled (false);
    bool old = checkBringsJudgment;
    checkBringsJudgment = true;
    checkResponseClicked();
    checkBringsJudgment = old;
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
//  flashcardStateChanged
//
//! Called when the state of the Flashcard Mode checkbox changes.
//
//! @param state the new checkbox state
//---------------------------------------------------------------------------
void
QuizForm::flashcardStateChanged (int state)
{
    if (state == Qt::Checked) {
        checkBringsJudgment = false;
        inputLine->clear();
        inputLine->hide();
        responseStatusLabel->hide();
        questionStatusLabel->hide();
        if (checkResponseButton->isEnabled())
            checkResponseButton->setFocus();
        else
            nextQuestionButton->setFocus();
    }
    else if (state == Qt::Unchecked) {
        checkBringsJudgment = true;
        inputLine->show();
        responseStatusLabel->show();
        questionStatusLabel->show();
        inputLine->setFocus();
    }
}

//---------------------------------------------------------------------------
//  startQuestion
//
//! Update the form when starting a question.
//---------------------------------------------------------------------------
void
QuizForm::startQuestion()
{
    QApplication::setOverrideCursor (QCursor (Qt::WaitCursor));

    clearStats();
    setQuestionLabel (quizEngine->getQuestion());
    responseModel->clear();

    std::set<QString> correct = quizEngine->getQuestionCorrectResponses();
    if (!correct.empty()) {
        std::set<QString>::iterator it;
        QList<WordTableModel::WordItem> wordItems;
        for (it = correct.begin(); it != correct.end(); ++it) {
            wordItems.append (WordTableModel::WordItem
                                (*it, WordTableModel::WordCorrect));
        }
        responseModel->addWords (wordItems);
    }

    responseStatusLabel->setText ("");
    checkResponseButton->setEnabled (true);
    markMissedButton->setEnabled (true);
    nextQuestionButton->setEnabled (false);
    inputLine->setEnabled (true);
    if (inputLine->isVisible())
        inputLine->setFocus();
    if (timerSpec.getType() == NoTimer)
        clearTimerDisplay();
    else
        startNewTimer();

    updateStatusString();

    QApplication::restoreOverrideCursor();
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
    questionCanvas->clear();
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
    questionCanvas->setToMinimumSize();
    reflowLayout();
}

//---------------------------------------------------------------------------
//  setQuestionLabel
//
//! Display the current question in the question label area.
//
//! @param question the question
//---------------------------------------------------------------------------
void
QuizForm::setQuestionLabel (const QString& question, const QString& order)
{
    clearCanvas();

    QString displayQuestion (question);
    QuizSpec quizSpec = quizEngine->getQuizSpec();
    QuizSpec::QuizType type = quizSpec.getType();

    // Anagram Quiz: Order letters according to preference
    if (type == QuizSpec::QuizAnagrams) {
        QString letterOrder (order);
        if (letterOrder.isEmpty())
            letterOrder = MainSettings::getQuizLetterOrder();
        int length = displayQuestion.length();

        if (letterOrder == Defs::QUIZ_LETTERS_RANDOM) {
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

            if (letterOrder == Defs::QUIZ_LETTERS_VOWELS_FIRST) {
                qSort (qchars.begin(), qchars.end(), vowelsFirstCmp);
            }
            else if (letterOrder == Defs::QUIZ_LETTERS_CONSONANTS_FIRST) {
                qSort (qchars.begin(), qchars.end(), consonantsFirstCmp);
            }
            else if (letterOrder == Defs::QUIZ_LETTERS_ALPHA) {
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
    if (!MainSettings::getUseTileTheme() || displayQuestion.contains (" ")
        || !questionCanvas->hasTileImages())
    {
        QLabel* label = (quizSpec.getType() == QuizSpec::QuizWordListRecall) ?
            questionSpecLabel : questionLabel;
        label->setText (displayQuestion);
        questionStack->setCurrentWidget (label);
    }

    else {
        questionCanvas->setText (displayQuestion);
        questionStack->setCurrentWidget (questionCanvas);
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
//! Set the status string.
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
//  setBackgroundColor
//
//! Set the background color of the quiz question area.
//
//! @param color the new color
//---------------------------------------------------------------------------
void
QuizForm::setBackgroundColor (const QColor& color)
{
    questionCanvas->setPalette (QPalette (color));
}

//---------------------------------------------------------------------------
//  setTileTheme
//
//! Set canvas tile images according to a theme definition.
//
//! @param theme the name of the theme
//---------------------------------------------------------------------------
void
QuizForm::setTileTheme (const QString& theme)
{
    if (theme == tileTheme)
        return;
    questionCanvas->setTileTheme (theme);
    QString question = quizEngine->getQuestion();
    if (question.isEmpty())
        minimizeCanvas();
    else
        setQuestionLabel (question);
    tileTheme = theme;
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
