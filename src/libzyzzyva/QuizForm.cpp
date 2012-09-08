//---------------------------------------------------------------------------
// QuizForm.cpp
//
// A form for quizzing the user.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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
#include "QuizStatsDatabase.h"
#include "WordEngine.h"
#include "WordValidator.h"
#include "WordLineEdit.h"
#include "WordTableModel.h"
#include "WordTableView.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QtWidgets/QApplication>
#include <QtGui/QColor>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimerEvent>
#include <QtWidgets/QGridLayout>
#include <QtCore/QTextStream>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

using namespace Defs;

const QString TITLE_PREFIX = "Quiz";
const QString PAUSE_BUTTON = "&Pause";
const QString UNPAUSE_BUTTON = "Un&pause";
const QString MARK_MISSED_BUTTON = "&Mark as Missed";
const QString MARK_CORRECT_BUTTON = "&Mark as Correct";
const int TITLE_FONT_PIXEL_SIZE = 20;

//---------------------------------------------------------------------------
//  alphabeticalCmp
//
//! A character comparison function that sorts alphabetically.  For some
//! reason, qSort doesn't seem to do this correctly?
//
//! @param a a character
//! @param b another character
//---------------------------------------------------------------------------
bool
alphabeticalCmp(const QChar& a, const QChar& b)
{
    return Auxil::localeAwareLessThanQChar(a, b);
}

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
vowelsFirstCmp(const QChar& a, const QChar& b)
{
    if (Auxil::isVowel(a) && !Auxil::isVowel(b))
        return true;
    else if (!Auxil::isVowel(a) && Auxil::isVowel(b))
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
consonantsFirstCmp(const QChar& a, const QChar& b)
{
    if (Auxil::isVowel(a) && !Auxil::isVowel(b))
        return false;
    else if (!Auxil::isVowel(a) && Auxil::isVowel(b))
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
QuizForm::QuizForm(WordEngine* we, QWidget* parent, Qt::WFlags f)
    : ActionForm(QuizFormType, parent, f), wordEngine(we),
    quizEngine(new QuizEngine(wordEngine)), timerId(0), timerPaused(0),
    checkBringsJudgment(true), recordStatsBlocked(false),
    unsavedChanges(false), cardboxQuiz(false),
    questionMarkedStatus(QuestionNotMarked), quizStatsDatabase(0),
    // FIXME: This dialog should be nonmodal!
    analyzeDialog(new AnalyzeQuizDialog(quizEngine, we, this,
                                        Qt::WindowMinMaxButtonsHint))
{
    QFont titleFont = qApp->font();
    titleFont.setPixelSize(TITLE_FONT_PIXEL_SIZE);

    QHBoxLayout* mainHlay = new QHBoxLayout(this);
    mainHlay->setMargin(MARGIN);
    mainHlay->setSpacing(SPACING);

    QVBoxLayout* mainVlay = new QVBoxLayout;
    mainVlay->setSpacing(SPACING);
    mainHlay->addLayout(mainVlay);

    QHBoxLayout* topHlay = new QHBoxLayout;
    topHlay->setSpacing(SPACING);
    mainVlay->addLayout(topHlay);

    quizTypeLabel = new QLabel;
    quizTypeLabel->setFont(titleFont);
    quizTypeLabel->setAlignment(Qt::AlignLeft);
    topHlay->addWidget(quizTypeLabel);

    timerLabel = new QLabel;
    timerLabel->setFont(titleFont);
    timerLabel->setAlignment(Qt::AlignHCenter);
    topHlay->addWidget(timerLabel);

    quizNameLabel = new QLabel;
    quizNameLabel->setFont(titleFont);
    quizNameLabel->setAlignment(Qt::AlignRight);
    quizNameLabel->setText("Unnamed Quiz");
    topHlay->addWidget(quizNameLabel);

    QHBoxLayout* quizBoxHlay = new QHBoxLayout;
    quizBoxHlay->setSpacing(SPACING);
    mainVlay->addLayout(quizBoxHlay);

    quizBoxHlay->addStretch(1);

    questionStack = new QStackedWidget;
    questionStack->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    quizBoxHlay->addWidget(questionStack);

    // Canvas for tile images - set default background color
    questionCanvas = new QuizCanvas;
    questionCanvas->setPalette(
        QPalette(MainSettings::getQuizBackgroundColor()));
    questionCanvas->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    questionStack->addWidget(questionCanvas);

    questionLabel = new QuizQuestionLabel;
    questionLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    questionStack->addWidget(questionLabel);

    quizBoxHlay->addStretch(1);

    questionSpecLabel = new QLabel;
    questionSpecLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    questionStack->addWidget(questionSpecLabel);

    responseView = new WordTableView(wordEngine);
    mainVlay->addWidget(responseView);

    responseModel = new WordTableModel(wordEngine, this);
    connect(responseModel, SIGNAL(wordsChanged()),
            responseView, SLOT(resizeItemsToContents()));
    responseView->setModel(responseModel);

    // Correct status
    QHBoxLayout* correctStatusHlay = new QHBoxLayout;
    correctStatusHlay->setSpacing(SPACING);
    mainVlay->addLayout(correctStatusHlay);

    responseStatusLabel = new DefinitionLabel;
    correctStatusHlay->addWidget(responseStatusLabel);

    correctStatusLabel = new DefinitionLabel;
    correctStatusHlay->addWidget(correctStatusLabel);

    // Cardbox status
    QHBoxLayout* cardboxStatusHlay = new QHBoxLayout;
    cardboxStatusHlay->setSpacing(SPACING);
    mainVlay->addLayout(cardboxStatusHlay);

    cardboxStatusLabel = new DefinitionLabel;
    cardboxStatusHlay->addWidget(cardboxStatusLabel);

    // Question stats
    QHBoxLayout* questionStatsHlay = new QHBoxLayout;
    questionStatsHlay->setSpacing(SPACING);
    questionStatsHlay->setMargin(0);
    mainVlay->addLayout(questionStatsHlay);

    questionStatsLabel = new DefinitionLabel;
    questionStatsHlay->addWidget(questionStatsLabel);

    inputWidget = new QWidget;
    mainVlay->addWidget(inputWidget);

    QVBoxLayout* inputVlay = new QVBoxLayout(inputWidget);
    inputVlay->setMargin(0);

    // Input line
    inputLine = new WordLineEdit;
    inputValidator = new WordValidator(inputLine);
    inputLine->setValidator(inputValidator);
    inputLine->setEnabled(false);
    connect(inputLine, SIGNAL(returnPressed()), SLOT(responseEntered()));
    inputVlay->addWidget(inputLine);

    // Button layout
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    inputVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    QGridLayout* buttonGlay = new QGridLayout;
    buttonGlay->setSpacing(SPACING);
    buttonHlay->addLayout(buttonGlay);

    // Buttons
    nextQuestionButton = new ZPushButton("&Next");
    nextQuestionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(nextQuestionButton, SIGNAL(clicked()), SLOT(nextQuestionClicked()));
    nextQuestionButton->setEnabled(false);
    buttonGlay->addWidget(nextQuestionButton, 0, 0, Qt::AlignHCenter);

    checkResponseButton = new ZPushButton("&Check Answers");
    checkResponseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(checkResponseButton, SIGNAL(clicked()),
            SLOT(checkResponseClicked()));
    checkResponseButton->setEnabled(false);
    buttonGlay->addWidget(checkResponseButton, 0, 1, Qt::AlignHCenter);

    markMissedButton = new ZPushButton(MARK_MISSED_BUTTON);
    markMissedButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(markMissedButton, SIGNAL(clicked()), SLOT(markMissedClicked()));
    buttonGlay->addWidget(markMissedButton, 0, 2, Qt::AlignHCenter);

    newQuizButton = new ZPushButton("New &Quiz...");
    newQuizButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(newQuizButton, SIGNAL(clicked()), SLOT(newQuizClicked()));
    buttonGlay->addWidget(newQuizButton, 1, 0, Qt::AlignHCenter);

    saveQuizButton = new ZPushButton("&Save Quiz");
    saveQuizButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(saveQuizButton, SIGNAL(clicked()), SLOT(saveRequested()));
    buttonGlay->addWidget(saveQuizButton, 1, 1, Qt::AlignHCenter);

    pauseButton = new ZPushButton(PAUSE_BUTTON);
    pauseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(pauseButton, SIGNAL(clicked()), SLOT(pauseClicked()));
    pauseButton->setEnabled(false);
    buttonGlay->addWidget(pauseButton, 1, 2, Qt::AlignHCenter);

    flashcardCbox = new QCheckBox("Flashcard M&ode");
    connect(flashcardCbox, SIGNAL(stateChanged(int)),
            SLOT(flashcardStateChanged(int)));
    buttonGlay->addWidget(flashcardCbox, 2, 0, Qt::AlignHCenter);

    lexiconSymbolCbox = new QCheckBox("Require le&xicon symbols");
    connect(lexiconSymbolCbox, SIGNAL(stateChanged(int)),
            SLOT(lexiconSymbolStateChanged(int)));
    buttonGlay->addWidget(lexiconSymbolCbox, 2, 1, Qt::AlignHCenter);

    analyzeButton = new ZPushButton("Analy&ze Quiz...");
    analyzeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(analyzeButton, SIGNAL(clicked()), SLOT(analyzeClicked()));
    buttonGlay->addWidget(analyzeButton, 2, 2, Qt::AlignHCenter);

    letterOrderWidget = new QWidget;
    buttonGlay->addWidget(letterOrderWidget, 3, 0, 1, 2, Qt::AlignCenter);

    QHBoxLayout* letterOrderHlay = new QHBoxLayout(letterOrderWidget);
    letterOrderHlay->setMargin(0);
    letterOrderHlay->setSpacing(SPACING);

    letterOrderLabel = new QLabel("Letter order:");
    letterOrderHlay->addWidget(letterOrderLabel);

    letterOrderCombo = new QComboBox;
    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_ALPHA);
    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_RANDOM);
    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_VOWELS_FIRST);
    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_CONSONANTS_FIRST);
    QString defaultOrder = MainSettings::getQuizLetterOrder();
    letterOrderCombo->setCurrentIndex(letterOrderCombo->findText(defaultOrder));
    letterOrderHlay->addWidget(letterOrderCombo);

    ZPushButton* letterOrderButton = new ZPushButton("&Reorder");
    letterOrderButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(letterOrderButton, SIGNAL(clicked()), SLOT(letterOrderClicked()));
    letterOrderHlay->addWidget(letterOrderButton);

    cardboxMoveWidget = new QWidget;
    buttonGlay->addWidget(cardboxMoveWidget, 3, 2, Qt::AlignCenter);

    QHBoxLayout* cardboxMoveHlay = new QHBoxLayout(cardboxMoveWidget);
    cardboxMoveHlay->setMargin(0);
    cardboxMoveHlay->setSpacing(SPACING);

    QLabel* cardboxMoveLabel = new QLabel;
    cardboxMoveLabel->setText("Cardbox:");
    cardboxMoveHlay->addWidget(cardboxMoveLabel);

    cardboxMoveSbox = new QSpinBox;
    cardboxMoveSbox->setMinimum(1);
    cardboxMoveSbox->setMaximum(15);
    cardboxMoveHlay->addWidget(cardboxMoveSbox);

    ZPushButton* cardboxMoveButton = new ZPushButton("Mo&ve");
    cardboxMoveButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cardboxMoveButton, SIGNAL(clicked()), SLOT(cardboxMoveClicked()));
    cardboxMoveHlay->addWidget(cardboxMoveButton);

    buttonHlay->addStretch(1);

    displayAnswerTimer = new QTimer(this);
    connect(displayAnswerTimer, SIGNAL(timeout()),
            SLOT(displayNextCorrectAnswer()));
}

//---------------------------------------------------------------------------
//  ~QuizForm
//
//! Destructor.
//---------------------------------------------------------------------------
QuizForm::~QuizForm()
{
    disconnectDatabase();
    delete quizEngine;
}

//---------------------------------------------------------------------------
//  getIcon
//
//! Returns the current icon.
//
//! @return the current icon
//---------------------------------------------------------------------------
QIcon
QuizForm::getIcon() const
{
    return QIcon(":/quiz-icon");
}

//---------------------------------------------------------------------------
//  getTitle
//
//! Returns the current title.
//
//! @return the current title
//---------------------------------------------------------------------------
QString
QuizForm::getTitle() const
{
    QString title = TITLE_PREFIX;
    QString quizName = quizNameLabel->text();
    if (!quizName.isEmpty())
        title += " : " + quizName;
    return title;
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
//  getDetailsString
//
//! Returns the current details string.
//
//! @return the current details string
//---------------------------------------------------------------------------
QString
QuizForm::getDetailsString() const
{
    return detailsString;
}

//---------------------------------------------------------------------------
//  isSaveEnabled
//
//! Determine whether the save action should be enabled for this form.
//
//! @return true if save should be enabled, false otherwise
//---------------------------------------------------------------------------
bool
QuizForm::isSaveEnabled() const
{
    return unsavedChanges;
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

    if (!responseMatchesQuestion(response)) {
        pauseTimer();
        QString caption = "Response does not match question";
        QString message = "Sorry, your response does not match the question.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        unpauseTimer();
        return;
    }

    QuizEngine::ResponseStatus status = quizEngine->respond(response,
        (lexiconSymbolCbox->checkState() == Qt::Checked));
    QString displayResponse = response;
    QString statusStr;

    if (quizEngine->getQuizSpec().getMethod() != QuizSpec::CardboxQuizMethod)
        setUnsavedChanges(true);

    if (status == QuizEngine::Correct) {
        if (response.contains(":"))
            response = response.section(":", 1, 1);
        response.replace(QRegExp("[\\W_\\d]+"), QString());

        // FIXME: Probably not the right way to get alphabetical sorting
        // instead of alphagram sorting
        bool origGroupByAnagrams = MainSettings::getWordListGroupByAnagrams();
        MainSettings::setWordListGroupByAnagrams(false);
        responseModel->addWord(
            WordTableModel::WordItem(response, WordTableModel::WordCorrect),
            true);
        MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);

        responseView->scrollTo(responseModel->sibling(
            responseModel->getLastAddedIndex(), 0, QModelIndex()));
        statusStr = "<font color=\"blue\">Correct</font>";
        analyzeDialog->updateStats();
        inputLine->clear();
    }
    else if (status == QuizEngine::Incorrect) {
        statusStr = "<font color=\"red\">Incorrect</font>";
        analyzeDialog->addIncorrect(response);
        selectInputArea();
    }
    else if (status == QuizEngine::Duplicate) {
        statusStr = "<font color=\"purple\">Duplicate</font>";
        inputLine->clear();
    }

    // Update the stats if the stats are already shown
    if (!correctStatusLabel->text().isEmpty())
        updateStats();

    // Update the response status label
    responseStatusLabel->setText(displayResponse + " : " + statusStr);

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
QuizForm::newQuiz(const QuizSpec& spec)
{
    disconnectDatabase();

    timerSpec = spec.getTimerSpec();
    cardboxQuiz = (spec.getMethod() == QuizSpec::CardboxQuizMethod);

    // Enable or disable Alpha and Random buttons depending on Quiz type
    letterOrderWidget->setEnabled(customLetterOrderAllowed(spec.getType()));

    cardboxMoveWidget->setEnabled(
        spec.getMethod() == QuizSpec::CardboxQuizMethod);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool ok = quizEngine->newQuiz(spec);
    QApplication::restoreOverrideCursor();

    if (!ok) {
        QString caption = "No Questions Found";
        QString message;
        if (spec.getMethod() == QuizSpec::CardboxQuizMethod) {
            message = "No matching questions are ready for review.  "
                      "Please modify your search criteria, "
                      "or add more words to the Cardbox system.";
        }
        else {
            message = "No matching questions were found.  "
                      "Please modify your search criteria.";
        }
        message = Auxil::dialogWordWrap(message);

        QMessageBox::warning(MainWindow::getInstance(), caption, message);
        return false;
    }

    if (spec.getMethod() == QuizSpec::CardboxQuizMethod)
        cardboxStatusLabel->show();
    else
        cardboxStatusLabel->hide();

    // Restore incorrect and missed words from quiz progress
    analyzeDialog->newQuiz(spec);

    // Connect to database before starting the first question
    QString lexicon = spec.getLexicon();
    QString quizType = Auxil::quizTypeToString(spec.getType());
    connectToDatabase(lexicon, quizType);

    quizTypeLabel->setText(lexicon + " - " + quizType);
    responseModel->setLexicon(lexicon);
    inputLine->clear();
    startQuestion();

    // If the question is complete, don't record stats when restoring it
    if (quizEngine->getQuestionComplete()) {
        recordStatsBlocked = true;
        checkResponseClicked();
        recordStatsBlocked = false;
    }

    flashcardCbox->setChecked(MainSettings::getQuizUseFlashcardMode());
    lexiconSymbolCbox->setCheckState(
        MainSettings::getQuizRequireLexiconSymbols()
        ? Qt::Checked : Qt::Unchecked);

    updateValidatorOptions();
    inputValidator->setLexicon(spec.getLexicon());

    setQuizNameFromFilename(spec.getFilename());

    // Update number of blanks in response list probability
    int probNumBlanks = MainSettings::getProbabilityNumBlanks();
    if (spec.getQuestionOrder() == QuizSpec::ProbabilityOrder) {
        probNumBlanks = spec.getProbabilityNumBlanks();
    }
    else {
        SearchSpec searchSpec = spec.getSearchSpec();
        foreach (const SearchCondition& condition, searchSpec.conditions) {
            if ((condition.type == SearchCondition::ProbabilityOrder) ||
                (condition.type == SearchCondition::LimitByProbabilityOrder))
            {
                probNumBlanks = condition.intValue;
                break;
            }
        }
    }
    responseModel->setProbabilityNumBlanks(probNumBlanks);

    QTimer::singleShot(0, this, SLOT(selectInputArea()));

    setDetailsString(Auxil::lexiconToDetails(lexicon));
    setUnsavedChanges(spec.getFilename().isEmpty());
    return true;
}

//---------------------------------------------------------------------------
//  saveRequested
//
//! Called when the New Quiz button is clicked.
//
//! @param saveAs true if the user should be prompted for a filename
//---------------------------------------------------------------------------
void
QuizForm::saveRequested(bool saveAs)
{
    pauseTimer();

    // XXX: This code is copied mostly from NewQuizDialog::saveQuiz!

    // Try saving in the same location as the spec's current filename
    QString startDir = Auxil::getQuizDir() + "/saved";
    QString currentFilename = quizEngine->getQuizSpec().getFilename();
    if (!currentFilename.isEmpty()) {
        currentFilename.remove("/[^/]+$");
        startDir = currentFilename;
    }

    QString filename = quizEngine->getQuizSpec().getFilename();
    bool filenameEdited = false;

    if (filename.isEmpty() || saveAs) {
        filename = QFileDialog::getSaveFileName(this, "Save Quiz", startDir,
                                               "Zyzzyva Quiz Files (*.zzq)");

        if (filename.isEmpty()) {
            unpauseTimer();
            return;
        }

        if (!filename.endsWith(".zzq", Qt::CaseInsensitive)) {
            filename += ".zzq";
            filenameEdited = true;
        }
    }

    QFile file (filename);
    if (filenameEdited && file.exists()) {
        QString caption = "Overwrite Existing File?";
        QString message = "The file already exists.  Overwrite it?";
        message = Auxil::dialogWordWrap(message);
        int code = QMessageBox::warning(0, caption, message,
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No);
        if (code != QMessageBox::Yes) {
            unpauseTimer();
            return;
        }
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString caption = "Error Saving Quiz";
        QString message = "Cannot save quiz:\n" + file.errorString() + ".";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
        unpauseTimer();
        return;
    }

    QDomImplementation implementation;
    QDomDocument document (implementation.createDocumentType("zyzzyva-quiz",
        QString(), "http://boshvark.com/dtd/zyzzyva-quiz.dtd"));

    document.appendChild(quizEngine->getQuizSpec().asDomElement());

    //// XXX: There should be a programmatic way to write the <?xml?> header
    //// based on the QDomImplementation, shouldn't there?
    QTextStream stream (&file);
    stream << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        << document.toString();

    quizEngine->setQuizSpecFilename(filename);
    setQuizNameFromFilename(filename);
    setUnsavedChanges(false);

    selectInputArea();
    setStatusString("Quiz saved successfully.");
    QTimer::singleShot(2000, this, SLOT(updateStatusString()));
    unpauseTimer();
}

//---------------------------------------------------------------------------
//  selectInputArea
//
//! Give focus to a text input area if possible.  Otherwise give focus to
//! Check Answers button, or the Next button.
//---------------------------------------------------------------------------
void
QuizForm::selectInputArea()
{
    if (inputLine->isVisible() && inputLine->isEnabled()) {
        inputLine->setFocus();
        inputLine->setSelection(0, inputLine->text().length());
    }
    else if (checkResponseButton->isEnabled()) {
        checkResponseButton->setFocus();
    }
    else {
        nextQuestionButton->setFocus();
    }
}

//---------------------------------------------------------------------------
//  letterOrderClicked
//
//! Called when the Reorder button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::letterOrderClicked()
{
    setQuestionLabel(quizEngine->getQuestion(),
                     letterOrderCombo->currentText());
    selectInputArea();
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

    if (!promptToSaveChanges())
        return;

    NewQuizDialog* dialog = new NewQuizDialog(this);
    QuizSpec spec = quizEngine->getQuizSpec();
    spec.setProgress(QuizProgress());
    spec.setRandomAlgorithm(Rand::MarsagliaMwc);
    spec.setRandomSeed(0);
    spec.setRandomSeed2(0);
    dialog->setQuizSpec(spec);
    int code = dialog->exec();
    if (code != QDialog::Accepted) {
        selectInputArea();
        unpauseTimer();
        return;
    }
    newQuiz(dialog->getQuizSpec());
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
    pauseButton->setText(UNPAUSE_BUTTON);
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
        pauseButton->setText(PAUSE_BUTTON);
}

//---------------------------------------------------------------------------
//  markMissed
//
//! Mark the current question as missed.
//---------------------------------------------------------------------------
void
QuizForm::markMissed()
{
    quizEngine->markQuestionAsMissed();
    responseModel->clear();
    markMissedButton->setText(MARK_CORRECT_BUTTON);
    bool old = checkBringsJudgment;
    checkBringsJudgment = true;
    questionMarkedStatus = QuestionMarkedMissed;
    quizStatsDatabase->undoLastResponse(quizEngine->getQuestion());
    checkResponseClicked();
    checkBringsJudgment = old;
}

//---------------------------------------------------------------------------
//  markCorrect
//
//! Mark the current question as correct.
//---------------------------------------------------------------------------
void
QuizForm::markCorrect()
{
    QStringList missed = quizEngine->getMissed();
    QStringListIterator it (missed);
    while (it.hasNext()) {
        analyzeDialog->removeMissed(it.next());
    }

    QStringList incorrect = quizEngine->getQuestionIncorrectResponses();
    foreach (const QString& response, incorrect) {
        analyzeDialog->removeIncorrect(response);
    }

    bool old = checkBringsJudgment;
    checkBringsJudgment = false;
    questionMarkedStatus = QuestionMarkedCorrect;
    quizStatsDatabase->undoLastResponse(quizEngine->getQuestion());
    checkResponseClicked();
    checkBringsJudgment = old;
    quizEngine->markQuestionAsCorrect();
    analyzeDialog->updateStats();
}

//---------------------------------------------------------------------------
//  moveToCardbox
//
//! Move the current question to a specific cardbox.
//
//! @param cardbox the cardbox to move the question to
//---------------------------------------------------------------------------
void
QuizForm::moveToCardbox(int cardbox)
{
    if (quizEngine->getQuestionCorrect() != quizEngine->getQuestionTotal())
        markCorrect();

    quizStatsDatabase->setCardbox(quizEngine->getQuestion(), cardbox);
    updateQuestionStatus();
}

//---------------------------------------------------------------------------
//  nextQuestionClicked
//
//! Called when the New Question button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::nextQuestionClicked()
{
    if (!quizEngine->nextQuestion()) {
        QString caption = "Error getting next question";
        QString message = "Error getting next question.";
        message = Auxil::dialogWordWrap(message);
        QMessageBox::warning(this, caption, message);
    }
    startQuestion();
    analyzeDialog->updateStats();
    if (quizEngine->getQuizSpec().getMethod() != QuizSpec::CardboxQuizMethod)
        setUnsavedChanges(true);
}

//---------------------------------------------------------------------------
//  checkResponseClicked
//
//! Called when the Check Responses button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::checkResponseClicked()
{
    const QuizSpec& quizSpec = quizEngine->getQuizSpec();
    QuizSpec::QuizType quizType = quizSpec.getType();

    // Mark question missed if all correct answers were given, but an
    // incorrect answer was also given
    if ((questionMarkedStatus == QuestionNotMarked) &&
        MainSettings::getQuizMarkMissedAfterIncorrect() &&
        (quizType != QuizSpec::QuizWordListRecall) &&
        (quizEngine->getQuestionIncorrect() > 0) &&
        (quizEngine->getQuestionCorrect() == quizEngine->getQuestionTotal()))
    {
        markMissed();
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // If checking responses does not bring judgment, then feed all correct
    // responses to the quiz engine
    if (!checkBringsJudgment) {
        QStringList unanswered = quizEngine->getMissed();
        QStringListIterator it (unanswered);
        while (it.hasNext()) {
            QString word = it.next();
            QString response = word;
            QString lexicon = quizSpec.getLexicon();
            bool lexiconSymbols =
                (lexiconSymbolCbox->checkState() == Qt::Checked);

            if (lexiconSymbols) {
                response += wordEngine->getLexiconSymbols(lexicon, word);
            }

            if (quizType == QuizSpec::QuizAnagramsWithHooks) {
                QString frontHooks =
                    wordEngine->getFrontHookLetters(lexicon, word).toUpper();
                QString backHooks =
                    wordEngine->getBackHookLetters(lexicon, word).toUpper();
                if (!lexiconSymbols) {
                    frontHooks.replace(QRegExp("[\\W_\\d]+"), QString());
                    backHooks.replace(QRegExp("[\\W_\\d]+"), QString());
                }
                response = frontHooks + ":" + response + ":" + backHooks;
            }

            quizEngine->respond(response, lexiconSymbols);

            // FIXME: Probably not the right way to get alphabetical sorting
            // instead of alphagram sorting
            bool origGroupByAnagrams
                = MainSettings::getWordListGroupByAnagrams();
            MainSettings::setWordListGroupByAnagrams(false);
            responseModel->removeWord(word);
            responseModel->addWord(
                WordTableModel::WordItem(word, WordTableModel::WordCorrect),
                true);
            MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);
        }
    }

    killActiveTimer();
    updateStats();
    inputLine->setText(QString());
    inputLine->setEnabled(false);
    checkResponseButton->setEnabled(false);
    quizEngine->completeQuestion();
    analyzeDialog->updateStats();

    if (quizEngine->getQuizSpec().getMethod() != QuizSpec::CardboxQuizMethod)
        setUnsavedChanges(true);

    // FIXME: Count Incorrect answers (not just Missed) as incorrect when
    // recording stats
    bool questionCorrect = true;
    QStringList unanswered = quizEngine->getMissed();
    if (questionMarkedStatus == QuestionMarkedMissed)
        questionCorrect = false;
    else if (questionMarkedStatus == QuestionMarkedCorrect)
        questionCorrect = true;
    else
        questionCorrect = unanswered.isEmpty();

    if (questionCorrect) {
        responseModel->clearLastAddedIndex();
        markMissedButton->setText(MARK_MISSED_BUTTON);
    }
    else {
        QStringList::iterator it;
        QList<WordTableModel::WordItem> wordItems;
        for (it = unanswered.begin(); it != unanswered.end(); ++it) {
            wordItems.append(
                WordTableModel::WordItem(*it, WordTableModel::WordMissed));
        }
        // FIXME: Probably not the right way to get alphabetical sorting
        // instead of alphagram sorting
        bool origGroupByAnagrams = MainSettings::getWordListGroupByAnagrams();
        MainSettings::setWordListGroupByAnagrams(false);
        responseModel->addWords(wordItems);
        MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);

        analyzeDialog->addMissed(unanswered);
        questionCorrect = false;

        markMissedButton->setText(MARK_CORRECT_BUTTON);
    }

    if ((quizEngine->numQuestions() > 0) && !quizEngine->onLastQuestion()) {
        nextQuestionButton->setEnabled(true);
        nextQuestionButton->setFocus();
    }
    else
        analyzeButton->setFocus();

    updateStatusString();

    // For Anagram quizzes, display the correct answer in the question area
    if (MainSettings::getQuizCycleAnswers() &&
        ((quizType == QuizSpec::QuizAnagrams) ||
         (quizType == QuizSpec::QuizAnagramsWithHooks)))
    {
        startDisplayingCorrectAnswers();
    }

    if (MainSettings::getQuizRecordStats() && !recordStatsBlocked)
        recordQuestionStats(questionCorrect);

    if (quizStatsDatabase && quizStatsDatabase->isValid() &&
        (MainSettings::getQuizShowQuestionStats() ||
        (quizEngine->getQuizSpec().getMethod() == QuizSpec::CardboxQuizMethod)))
    {
        updateQuestionStatus();
    }

    QApplication::restoreOverrideCursor();
}

//---------------------------------------------------------------------------
//  markMissedClicked
//
//! Called when the Mark as Missed button is clicked.  Mark the question as
//! missed or correct, as appropriate based on the current state of the
//! button.
//---------------------------------------------------------------------------
void
QuizForm::markMissedClicked()
{
    if (markMissedButton->text() == MARK_MISSED_BUTTON)
        markMissed();
    else
        markCorrect();

    selectInputArea();
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

    selectInputArea();
}

//---------------------------------------------------------------------------
//  analyzeClicked
//
//! Called when the Analyze button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::analyzeClicked()
{
    if (analyzeDialog->isVisible())
        return;

    analyzeDialog->show();
    selectInputArea();
}

//---------------------------------------------------------------------------
//  cardboxMoveClicked
//
//! Called when the Move to Cardbox button is clicked.
//---------------------------------------------------------------------------
void
QuizForm::cardboxMoveClicked()
{
    int cardbox = cardboxMoveSbox->value();
    moveToCardbox(cardbox);
    selectInputArea();
}

//---------------------------------------------------------------------------
//  flashcardStateChanged
//
//! Called when the state of the Flashcard Mode checkbox changes.
//
//! @param state the new checkbox state
//---------------------------------------------------------------------------
void
QuizForm::flashcardStateChanged(int state)
{
    if (state == Qt::Checked) {
        checkBringsJudgment = false;
        inputLine->clear();
        inputLine->hide();
        responseStatusLabel->hide();
        correctStatusLabel->hide();
        if (checkResponseButton->isEnabled())
            checkResponseButton->setFocus();
        else
            nextQuestionButton->setFocus();
    }
    else if (state == Qt::Unchecked) {
        checkBringsJudgment = true;
        inputLine->show();
        responseStatusLabel->show();
        correctStatusLabel->show();
        inputLine->setFocus();
    }
}

//---------------------------------------------------------------------------
//  lexiconSymbolStateChanged
//
//! Called when the state of the Require Lexicon Symbols checkbox changes.
//
//! @param state the new checkbox state
//---------------------------------------------------------------------------
void
QuizForm::lexiconSymbolStateChanged(int)
{
    updateValidatorOptions();
}

//---------------------------------------------------------------------------
//  startQuestion
//
//! Update the form when starting a question.
//---------------------------------------------------------------------------
void
QuizForm::startQuestion()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    stopDisplayingCorrectAnswers();
    clearStats();
    updateQuestionDisplay();
    responseModel->clear();
    questionMarkedStatus = QuestionNotMarked;

    QString question = quizEngine->getQuestion();
    origQuestionData = quizStatsDatabase->getQuestionData(question);

    QSet<QString> correct = quizEngine->getQuestionCorrectResponses();
    if (!correct.isEmpty()) {
        QList<WordTableModel::WordItem> wordItems;
        QSetIterator<QString> it (correct);
        while (it.hasNext()) {
            QString word = it.next();
            wordItems.append(
                WordTableModel::WordItem(word, WordTableModel::WordCorrect));
        }

        // FIXME: Probably not the right way to get alphabetical sorting
        // instead of alphagram sorting
        bool origGroupByAnagrams = MainSettings::getWordListGroupByAnagrams();
        MainSettings::setWordListGroupByAnagrams(false);
        responseModel->addWords(wordItems);
        MainSettings::setWordListGroupByAnagrams(origGroupByAnagrams);
    }

    responseStatusLabel->setText(QString());
    checkResponseButton->setEnabled(true);
    markMissedButton->setText(MARK_MISSED_BUTTON);
    nextQuestionButton->setEnabled(false);
    inputLine->setEnabled(true);
    selectInputArea();
    if (timerSpec.getType() == NoTimer)
        clearTimerDisplay();
    else
        startNewTimer();

    updateStatusString();

    if (quizEngine->getQuizSpec().getMethod() != QuizSpec::CardboxQuizMethod)
        setUnsavedChanges(true);

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
    setTimerDisplay(timerRemaining);
    timerId = startTimer(1000);
    pauseButton->setEnabled(true);
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
    killTimer(timerId);
    timerId = 0;
    pauseButton->setEnabled(false);
}

//---------------------------------------------------------------------------
//  updateStats
//
//! Update the recall and precision statistics.
//---------------------------------------------------------------------------
void
QuizForm::updateStats()
{
    setCorrectStatus(quizEngine->getQuestionCorrect(),
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
    correctStatusLabel->setText(QString());
    cardboxStatusLabel->setText(QString());
    questionStatsLabel->setText(QString());
}

//---------------------------------------------------------------------------
//  clearTimerDisplay
//
//! Clear the timer label.
//---------------------------------------------------------------------------
void
QuizForm::clearTimerDisplay()
{
    timerLabel->setText(QString());
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
QuizForm::setQuestionLabel(const QString& question, const QString& order)
{
    QString displayQuestion (question);
    QuizSpec quizSpec = quizEngine->getQuizSpec();
    QuizSpec::QuizType type = quizSpec.getType();

    // Order letters except for certain quiz types
    if (customLetterOrderAllowed(type)) {
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
                qchars.append(displayQuestion.at(i));

            if (letterOrder == Defs::QUIZ_LETTERS_VOWELS_FIRST) {
                qSort(qchars.begin(), qchars.end(), vowelsFirstCmp);
            }
            else if (letterOrder == Defs::QUIZ_LETTERS_CONSONANTS_FIRST) {
                qSort(qchars.begin(), qchars.end(), consonantsFirstCmp);
            }
            else if (letterOrder == Defs::QUIZ_LETTERS_ALPHA) {
                qSort(qchars.begin(), qchars.end(), alphabeticalCmp);
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

    QWidget* currentWidget = questionStack->currentWidget();
    if (currentWidget == questionCanvas) {
        questionCanvas->setText(displayQuestion);
        questionLabel->setText(QString());
        questionSpecLabel->setText(QString());
    }
    else if (currentWidget == questionLabel) {
        questionLabel->setText(displayQuestion);
        questionSpecLabel->setText(QString());
        clearCanvas();
    }
    else if (currentWidget == questionSpecLabel) {
        questionSpecLabel->setText(displayQuestion);
        questionLabel->setText(QString());
        clearCanvas();
    }

    reflowLayout();
}

//---------------------------------------------------------------------------
//  setCorrectStatus
//
//! Set the correctness status of the question after the user clicks the Check
//! button.
//
//! @param correct the number of correct responses
//! @param total the total number of correct answers
//---------------------------------------------------------------------------
void
QuizForm::setCorrectStatus(int correct, int total)
{
    correctStatusLabel->setText("Correct: " + QString::number(correct) +
                                " of " + QString::number(total));
}

//---------------------------------------------------------------------------
//  setQuestionStatus
//
//! Set the status of the question.
//
//! @param correct the number of correct responses
//! @param total the total number of correct answers
//---------------------------------------------------------------------------
void
QuizForm::setQuestionStatus(const QuizStatsDatabase::QuestionData& data)
{
    if (!data.valid || !quizEngine->getQuestionComplete())
        return;

    if (quizEngine->getQuizSpec().getMethod() == QuizSpec::CardboxQuizMethod) {
        QDateTime nextDate;
        nextDate.setTime_t(data.nextScheduled);
        QDateTime now = QDateTime::currentDateTime();
        int numDays = now.daysTo(nextDate);

        int origCardbox = origQuestionData.cardbox;

        QString format = "yyyy-MM-dd hh:mm:ss";
        QString text = "<b>Old Cardbox:</b> " + QString::number(origCardbox) +
            ", <b>New Cardbox:</b> " + QString::number(data.cardbox) +
            ", <b>Next Scheduled:</b> " + nextDate.toString(format) +
            " (" + QString::number(numDays) + " day" +
            (numDays == 1 ? QString() : QString("s")) + ")";

        cardboxStatusLabel->setText(text);
    }

    if (MainSettings::getQuizShowQuestionStats()) {
        int correct = data.numCorrect;
        int incorrect = data.numIncorrect;
        int total = correct + incorrect;
        double pct = total ? (correct * 100.0) / total : 0;

        QString streak;
        if (data.streak == 0)
            streak = "none";
        else if (data.streak > 0)
            streak = QString::number(data.streak) + " correct";
        else if (data.streak < 0)
            streak = QString::number(-data.streak) + " incorrect";

        QString lastCorrect = "never";
        if (data.lastCorrect) {

            QDateTime lastCorrectDate;
            lastCorrectDate.setTime_t(data.lastCorrect);
            QDateTime now = QDateTime::currentDateTime();
            int numDays = now.daysTo(lastCorrectDate);

            QString format = "yyyy-MM-dd hh:mm:ss";
            QString delta;
            QString daysStr = (abs(numDays) == 1 ? QString("day")
                                                 : QString("days"));
            if (numDays == 0)
                delta = "today";
            else if (numDays < 0)
                delta = QString::number(-numDays) + " " + daysStr + " ago";
            else if (numDays > 0)
                delta = QString::number(numDays) + " " + daysStr + " from now";

            lastCorrect = lastCorrectDate.toString(format) + " (" + delta + ")";
        }

        QString text = "<b>Overall:</b> " +
            QString::number(correct) + "/" + QString::number(total) + " (" +
            QString::number(pct, 'f', 1) + "%), <b>Streak:</b> " + streak +
            ", <b>Last Correct:</b> " + lastCorrect;

        questionStatsLabel->setText(text);
        questionStatsLabel->show();
    }
    else {
        questionStatsLabel->hide();
    }
}

//---------------------------------------------------------------------------
//  updateStatusString
//
//! Update the status string with the current quiz status.
//---------------------------------------------------------------------------
void
QuizForm::updateStatusString()
{
    QString status;

    //if (quizEngine->getQuizSpec().getMethod() == QuizSpec::CardboxQuizMethod) {
    //    status = "Questions remaining: " +
    //        QString::number(quizEngine->numQuestions());
    //}

    //else {
        status = "Question " +
            QString::number(quizEngine->getQuestionIndex() + 1) + "/" +
            QString::number(quizEngine->numQuestions());
    //}

    if (MainSettings::getQuizShowNumResponses()) {
        status += ", Correct: " +
            QString::number(quizEngine->getQuestionCorrect()) + "/" +
            QString::number(quizEngine->getQuestionTotal());
    }

    setStatusString(status);
}

//---------------------------------------------------------------------------
//  updateQuestionDisplay
//
//! Display the question using the appropriate widget.
//---------------------------------------------------------------------------
void
QuizForm::updateQuestionDisplay()
{
    bool wordListRecall =
        (quizEngine->getQuizSpec().getType() == QuizSpec::QuizWordListRecall);

    QWidget* newWidget = questionCanvas;

    if (!MainSettings::getUseTileTheme() || wordListRecall
        || !questionCanvas->hasTileImages())
    {
        newWidget = (wordListRecall ? questionSpecLabel : questionLabel);
    }

    questionStack->setCurrentWidget(newWidget);
    setQuestionLabel(quizEngine->getQuestion());
}

//---------------------------------------------------------------------------
//  updateQuestionStatus
//
//! Update the display of the current question's status, including cardbox
//! status and question stats.
//---------------------------------------------------------------------------
void
QuizForm::updateQuestionStatus()
{
    QString question = quizEngine->getQuestion();
    QuizStatsDatabase::QuestionData data =
        quizStatsDatabase->getQuestionData(question);
    if (data.valid)
        setQuestionStatus(data);
}

//---------------------------------------------------------------------------
//  startDisplayingCorrectAnswers
//
//! Start displaying correct answers in the quiz question display area.
//---------------------------------------------------------------------------
void
QuizForm::startDisplayingCorrectAnswers()
{
    questionCanvas->setDragDropEnabled(false);
    currentDisplayAnswer = -1;
    displayNextCorrectAnswer();
    if (responseModel->rowCount() > 1)
        displayAnswerTimer->start(5000);
}

//---------------------------------------------------------------------------
//  stopDisplayingCorrectAnswers
//
//! Stop displaying correct answers in the quiz question display area.
//---------------------------------------------------------------------------
void
QuizForm::stopDisplayingCorrectAnswers()
{
    if (displayAnswerTimer->isActive())
        displayAnswerTimer->stop();
    questionCanvas->setDragDropEnabled(true);
}

//---------------------------------------------------------------------------
//  displayNextCorrectAnswer
//
//! Display the next correct answer in the quiz question display area.
//---------------------------------------------------------------------------
void
QuizForm::displayNextCorrectAnswer()
{
    ++currentDisplayAnswer;
    if (currentDisplayAnswer >= responseModel->rowCount())
        currentDisplayAnswer = 0;

    QString answer = responseModel->data(
        responseModel->index(currentDisplayAnswer, WordTableModel::WORD_COLUMN),
        Qt::EditRole).toString();

    setQuestionLabel(answer, "foo");
}

//---------------------------------------------------------------------------
//  enableAndSelectInputArea
//
//! Enable the input area and buttons, and give focus to an input widget.
//---------------------------------------------------------------------------
void
QuizForm::enableAndSelectInputArea()
{
    inputWidget->setEnabled(true);
    selectInputArea();
}

//---------------------------------------------------------------------------
//  promptToSaveChanges
//
//! Prompt the user to save any unsaved changes in this quiz.
//
//! @return true if everything is okay and can proceed normally, false if the
//! action generating this request should be cancelled
//---------------------------------------------------------------------------
bool
QuizForm::promptToSaveChanges()
{
    if (!unsavedChanges || cardboxQuiz)
        return true;

    pauseTimer();

    QString quizName = quizNameLabel->text();
    if (!quizName.isEmpty())
        quizName = "(\"" + quizName + "\") ";

    QString caption = "Save Quiz?";
    QString message = "This quiz " + quizName + "has unsaved changes.  "
        "Would you like to save it?";
    message = Auxil::dialogWordWrap(message);
    int code = QMessageBox::warning(this, caption, message,
                                    QMessageBox::Yes | QMessageBox::No |
                                    QMessageBox::Cancel, QMessageBox::Yes);

    bool ok = false;
    switch (code) {
        case QMessageBox::No:
        ok = true;
        break;

        case QMessageBox::Yes:
        saveRequested();

        // If the quiz spec does not have a filename, the save was cancelled
        ok = !quizEngine->getQuizSpec().getFilename().isEmpty();
        break;

        default:
        ok = false;
        break;
    }

    unpauseTimer();
    return ok;
}

//---------------------------------------------------------------------------
//  keyPressEvent
//
//! Called when the quiz form receives a key press event.
//---------------------------------------------------------------------------
void
QuizForm::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::NoModifier) {
        switch (event->key()) {
            case Qt::Key_N: nextQuestionButton->animateClick(); return;
            case Qt::Key_C: checkResponseButton->animateClick(); return;
            case Qt::Key_M: markMissedButton->animateClick(); return;
            case Qt::Key_P: pauseButton->animateClick(); return;
            case Qt::Key_Q: newQuizButton->animateClick(); return;
            case Qt::Key_S: saveQuizButton->animateClick(); return;
            case Qt::Key_Z: analyzeButton->animateClick(); return;
            case Qt::Key_O: flashcardCbox->toggle(); return;
            case Qt::Key_X: lexiconSymbolCbox->toggle(); return;
            default: break;
        }
    }
    else if (event->modifiers() & Qt::ControlModifier) {
        int cardbox = 0;
        switch (event->key()) {
            case Qt::Key_1: cardbox = 1; break;
            case Qt::Key_2: cardbox = 2; break;
            case Qt::Key_3: cardbox = 3; break;
            case Qt::Key_4: cardbox = 4; break;
            case Qt::Key_5: cardbox = 5; break;
            case Qt::Key_6: cardbox = 6; break;
            case Qt::Key_7: cardbox = 7; break;
            case Qt::Key_8: cardbox = 8; break;
            case Qt::Key_9: cardbox = 9; break;
            case Qt::Key_0: cardbox = 10; break;
        }

        if (cardbox) {
            QuizSpec quizSpec = quizEngine->getQuizSpec();
            if (quizSpec.getMethod() == QuizSpec::CardboxQuizMethod) {
                moveToCardbox(cardbox);
            }
        }

    }
    event->ignore();
}

//---------------------------------------------------------------------------
//  setStatusString
//
//! Set the status string.
//---------------------------------------------------------------------------
void
QuizForm::setStatusString(const QString& status)
{
    if (status == statusString)
        return;
    statusString = status;
    emit statusChanged(status);
}

//---------------------------------------------------------------------------
//  setDetailsString
//
//! Set the details string.
//---------------------------------------------------------------------------
void
QuizForm::setDetailsString(const QString& details)
{
    if (details == detailsString)
        return;
    detailsString = details;
    emit detailsChanged(details);
}

//---------------------------------------------------------------------------
//  setTimerDisplay
//
//! Display a number of seconds in the timer label.
//
//! @param seconds the number of seconds to display
//---------------------------------------------------------------------------
void
QuizForm::setTimerDisplay(int seconds)
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
        text.sprintf("%d:%02d:%02d", hours, minutes, seconds);
    else
        text.sprintf("%02d:%02d", minutes, seconds);
    timerLabel->setText(text);
}

//---------------------------------------------------------------------------
//  setQuizName
//
//! Set the quiz name.
//
//! @param name the name of the quiz
//---------------------------------------------------------------------------
void
QuizForm::setQuizName(const QString& name)
{
    quizNameLabel->setText(name);
    emit titleChanged(getTitle());
}

//---------------------------------------------------------------------------
//  setQuizName
//
//! Set the quiz name from a filename.
//
//! @param filename the filename
//---------------------------------------------------------------------------
void
QuizForm::setQuizNameFromFilename(const QString& filename)
{
    QString quizName (filename);
    quizName.remove(QRegExp("^.*/"));
    quizName.remove(QRegExp("\\.zzq$"));
    setQuizName(quizName);
}

//---------------------------------------------------------------------------
//  setUnsavedChanges
//
//! Set the unsaved changes flag, and enable or disable the Save Quiz button
//! as appropriate.
//
//! @param b whether there are unsaved changes
//---------------------------------------------------------------------------
void
QuizForm::setUnsavedChanges(bool b)
{
    unsavedChanges = b;
    saveQuizButton->setEnabled(b);
    emit saveEnabledChanged(b);
}

//---------------------------------------------------------------------------
//  setBackgroundColor
//
//! Set the background color of the quiz question area.
//
//! @param color the new color
//---------------------------------------------------------------------------
void
QuizForm::setBackgroundColor(const QColor& color)
{
    questionCanvas->setPalette(QPalette(color));
}

//---------------------------------------------------------------------------
//  setTileTheme
//
//! Set canvas tile images according to a theme definition.
//
//! @param theme the name of the theme
//---------------------------------------------------------------------------
void
QuizForm::setTileTheme(const QString& theme)
{
    if ((questionStack->currentWidget() == questionCanvas)
        && (theme != tileTheme))
    {
        questionCanvas->setTileTheme(theme);
        updateQuestionDisplay();
    }
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
    QString text = correctStatusLabel->text();
    correctStatusLabel->setText("foo blah blah");
    correctStatusLabel->setText(text);
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
QuizForm::responseMatchesQuestion(const QString& response) const
{
    QString question = quizEngine->getQuestion();
    QuizSpec spec = quizEngine->getQuizSpec();
    QRegExp wordRegex ("[^\\W_]+");
    switch (spec.getType()) {
        case QuizSpec::QuizAnagrams: {
            if (wordRegex.indexIn(response) < 0)
                return false;
            QString word = wordRegex.cap(0);
            return ((word.length() == question.length()) &&
                (Auxil::getAlphagram(word) ==
                Auxil::getAlphagram(question)));
        }

        case QuizSpec::QuizAnagramsWithHooks: {
            if (response.count(QChar(':')) != 2)
                return false;
            QString wordSection = response.section(":", 1, 1);
            if (wordRegex.indexIn(wordSection) < 0)
                return false;
            QString word = wordRegex.cap(0);
            return ((word.length() == question.length()) &&
                (Auxil::getAlphagram(word) ==
                 Auxil::getAlphagram(question)));
        }

        case QuizSpec::QuizHooks: {
            if (wordRegex.indexIn(response) < 0)
                return false;
            QString word = wordRegex.cap(0);
            return ((word.length() == (question.length() + 1)) &&
                    ((question == word.right(question.length())) ||
                    (question == word.left(question.length()))));
        }

        default: break;
    }
    return true;
}

//---------------------------------------------------------------------------
//  connectToDatabase
//
//! Connect to the database.  Create the database if necessary.
//
//! @param lexicon the lexicon name
//! @param quizType the quiz type
//---------------------------------------------------------------------------
void
QuizForm::connectToDatabase(const QString& lexicon, const QString& quizType)
{
    if (quizStatsDatabase && quizStatsDatabase->isValid())
        return;

    quizStatsDatabase = new QuizStatsDatabase(lexicon, quizType);
    if (!quizStatsDatabase->isValid()) {
        delete quizStatsDatabase;
        quizStatsDatabase = 0;
    }
}

//---------------------------------------------------------------------------
//  disconnectDatabase
//
//! Disconnect from the database.
//---------------------------------------------------------------------------
void
QuizForm::disconnectDatabase()
{
    delete quizStatsDatabase;
    quizStatsDatabase = 0;
}

//---------------------------------------------------------------------------
//  recordQuestionStats
//
//! Record statistics for the current quiz question.
//
//! @param correct whether to record a correct or incorrect response
//---------------------------------------------------------------------------
void
QuizForm::recordQuestionStats(bool correct)
{
    if (!quizStatsDatabase || !quizStatsDatabase->isValid())
        return;

    if (quizEngine->getQuizSpec().getMethod() != QuizSpec::CardboxQuizMethod)
        setUnsavedChanges(true);

    QuizSpec::QuizMethod method = quizEngine->getQuizSpec().getMethod();
    bool updateCardbox = (method == QuizSpec::CardboxQuizMethod);
    quizStatsDatabase->recordResponse(quizEngine->getQuestion(), correct,
        updateCardbox);
}

//---------------------------------------------------------------------------
//  customLetterOrderAllowed
//
//! Determine whether order of quiz question letters is allowed for a certain
//! quiz type.
//
//! @param quizType the quiz type
//! @return true if letter ordering is allowed, false otherwise
//---------------------------------------------------------------------------
bool
QuizForm::customLetterOrderAllowed(QuizSpec::QuizType quizType) const
{
    return ((quizType != QuizSpec::QuizPatterns) &&
            (quizType != QuizSpec::QuizHooks) &&
            (quizType != QuizSpec::QuizWordListRecall));
}

//---------------------------------------------------------------------------
//  updateValidatorOptions
//
//! Update the validator options
//! quiz type.
//---------------------------------------------------------------------------
void
QuizForm::updateValidatorOptions()
{
    QuizSpec quizSpec = quizEngine->getQuizSpec();

    int validatorOptions = WordValidator::None;
    if (quizSpec.getType() == QuizSpec::QuizAnagramsWithHooks)
        validatorOptions |= WordValidator::AllowHooks;

    if (lexiconSymbolCbox->checkState() == Qt::Checked)
        validatorOptions |= WordValidator::AllowLexiconSymbols;

    inputValidator->setOptions(validatorOptions);
}

//---------------------------------------------------------------------------
//  timerEvent
//
//! Reimplementation of QObject::timerEvent.  Called when a timer event
//! occurs.  Decrements the time remaining, unless the timer is paused.
//
//! @param event the timer event
//---------------------------------------------------------------------------
void
QuizForm::timerEvent(QTimerEvent* event)
{
    if ((event->timerId() != timerId) || timerPaused)
        return;

    --timerRemaining;
    setTimerDisplay(timerRemaining);
    if (timerRemaining == 0) {

        if (MainSettings::getQuizMarkMissedAfterTimerExpires()) {
            markMissed();
        }
        else {
            bool old = checkBringsJudgment;
            checkBringsJudgment = true;
            checkResponseClicked();
            checkBringsJudgment = old;
        }

        if (MainSettings::getQuizTimeoutDisableInput()) {
            int msecs = MainSettings::getQuizTimeoutDisableInputMillisecs();
            inputWidget->setEnabled(false);
            QTimer::singleShot(msecs, this, SLOT(enableAndSelectInputArea()));
        }
    }
}
