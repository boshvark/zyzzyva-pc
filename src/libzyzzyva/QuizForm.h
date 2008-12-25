//---------------------------------------------------------------------------
// QuizForm.h
//
// A form for quizzing the user.
//
// Copyright 2004, 2005, 2006, 2007, 2008 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_QUIZ_FORM_H
#define ZYZZYVA_QUIZ_FORM_H

#include "ActionForm.h"
#include "QuizTimerSpec.h"
#include "QuizDatabase.h"
#include "QuizSpec.h"
#include <QCheckBox>
#include <QComboBox>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QSpinBox>
#include <QStackedWidget>
#include <QString>
#include <QTimer>
#include <QTimerEvent>

class AnalyzeQuizDialog;
class DefinitionLabel;
class QuizCanvas;
class QuizDatabase;
class QuizEngine;
class QuizQuestionLabel;
class WordEngine;
class WordLineEdit;
class WordTableModel;
class WordTableView;
class WordValidator;
class ZPushButton;

class QuizForm : public ActionForm
{
    Q_OBJECT
    public:
    QuizForm(WordEngine* we, QWidget* parent = 0, Qt::WFlags f = 0);
    ~QuizForm();
    QIcon getIcon() const;
    QString getTitle() const;
    QString getStatusString() const;
    bool isSaveCapable() const { return true; }
    bool isSaveEnabled() const;

    // Reimplemented virtual methods
    public slots:
    void saveRequested(bool saveAs = false);
    void selectInputArea();

    public slots:
    bool newQuiz(const QuizSpec& spec);
    void responseEntered();
    void letterOrderClicked();
    void newQuizClicked();
    void nextQuestionClicked();
    void checkResponseClicked();
    void markMissedClicked();
    void pauseClicked();
    void analyzeClicked();
    void cardboxMoveClicked();
    void flashcardStateChanged(int state);
    void lexiconSymbolStateChanged(int state);
    void setTileTheme(const QString& theme);
    void setBackgroundColor(const QColor& color);
    void updateStatusString();
    void updateQuestionDisplay();
    void updateQuestionStatus();
    void startDisplayingCorrectAnswers();
    void stopDisplayingCorrectAnswers();
    void displayNextCorrectAnswer();
    void enableAndSelectInputArea();
    bool promptToSaveChanges();

    protected:
    void keyPressEvent(QKeyEvent* event);

    private:
    void updateStats();
    void clearStats();
    void clearTimerDisplay();
    void startQuestion();
    void startNewTimer();
    void killActiveTimer();
    void pauseTimer();
    void unpauseTimer();
    void markMissed();
    void markCorrect();
    void moveToCardbox(int cardbox);
    void clearCanvas();
    void minimizeCanvas();
    void setNumCanvasTiles(int num);
    void setQuestionLabel(const QString& question, const QString& order =
                          QString());
    void setCorrectStatus(int correct, int total);
    void setQuestionStatus(const QuizDatabase::QuestionData& data);
    void setStatusString(const QString& status);
    void setTimerDisplay(int seconds);
    void setQuizName(const QString& name);
    void setQuizNameFromFilename(const QString& filename);
    void setUnsavedChanges(bool b);
    void clearTileTheme();
    void reflowLayout();
    bool responseMatchesQuestion(const QString& response) const;
    void connectToDatabase(const QString& lexicon, const QString& quizType);
    void disconnectDatabase();
    void recordQuestionStats(bool correct);
    bool customLetterOrderAllowed(QuizSpec::QuizType quizType) const;
    void updateValidatorOptions();

    void timerEvent(QTimerEvent* event);

    private:
    WordEngine*   wordEngine;
    QuizEngine*   quizEngine;
    QLabel*       quizTypeLabel;
    QLabel*       timerLabel;
    QLabel*       quizNameLabel;
    QStackedWidget* questionStack;
    QuizCanvas*   questionCanvas;
    QuizQuestionLabel* questionLabel;
    QLabel*       questionSpecLabel;
    WordLineEdit* inputLine;
    WordValidator* inputValidator;
    WordTableView* responseView;
    WordTableModel* responseModel;
    DefinitionLabel* responseStatusLabel;
    DefinitionLabel* correctStatusLabel;
    DefinitionLabel* cardboxStatusLabel;
    DefinitionLabel* questionStatsLabel;
    QWidget*      inputWidget;
    QLabel*       letterOrderLabel;
    QCheckBox*    flashcardCbox;
    QCheckBox*    lexiconSymbolCbox;
    ZPushButton*  nextQuestionButton;
    ZPushButton*  checkResponseButton;
    ZPushButton*  markMissedButton;
    ZPushButton*  pauseButton;
    ZPushButton*  newQuizButton;
    ZPushButton*  saveQuizButton;
    ZPushButton*  analyzeButton;
    QWidget*      cardboxMoveWidget;
    QSpinBox*     cardboxMoveSbox;
    QWidget*      letterOrderWidget;
    QComboBox*    letterOrderCombo;

    int timerId;
    int timerRemaining, timerPaused;
    QuizTimerSpec timerSpec;
    QString statusString;
    QString tileTheme;
    bool checkBringsJudgment;
    bool recordStatsBlocked;
    bool unsavedChanges;
    int questionMarkedStatus;

    QuizDatabase* db;
    QuizDatabase::QuestionData origQuestionData;

    QTimer* displayAnswerTimer;
    int currentDisplayAnswer;

    AnalyzeQuizDialog* analyzeDialog;

    enum {
        QuestionNotMarked = 0,
        QuestionMarkedMissed = 1,
        QuestionMarkedCorrect = 2
    };
};

#endif // ZYZZYVA_QUIZ_FORM_H
