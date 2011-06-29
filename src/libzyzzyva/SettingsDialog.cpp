//---------------------------------------------------------------------------
// SettingsDialog.cpp
//
// The settings dialog for the word study application.
//
// Copyright 2004-2011 Michael W Thelen <mthelen@gmail.com>.
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

#include "SettingsDialog.h"
#include "MainSettings.h"
#include "LexiconSelectDialog.h"
#include "LexiconStyle.h"
#include "LexiconStyleDialog.h"
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QColorDialog>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QFontDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegExp>
#include <QSignalMapper>
#include <QVBoxLayout>

const QString DIALOG_CAPTION = "Preferences";
const QString USER_DATA_DIR_CHOOSER_TITLE = "Choose a Data Directory";
const QString JUDGE_LOG_CHOOSER_TITLE = "Choose a Log Directory";

const QString LEXICON_SEP = ", ";

const int FONT_MAIN_BUTTON = 1;
const int FONT_WORD_LISTS_BUTTON = 2;
const int FONT_QUIZ_LABEL_BUTTON = 3;
const int FONT_DEFINITIONS_BUTTON = 4;
const int FONT_WORD_INPUT_BUTTON = 5;

using namespace Defs;

//---------------------------------------------------------------------------
//  SettingsDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget* parent, Qt::WFlags f)
    : QDialog(parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout(this);

    QHBoxLayout* mainHlay = new QHBoxLayout;
    mainVlay->addLayout(mainHlay);

    navList = new QListWidget;
    navList->setMaximumWidth(150);
    navList->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(navList, SIGNAL(currentTextChanged(const QString&)),
            SLOT(navTextChanged(const QString&)));
    mainHlay->addWidget(navList);

    QVBoxLayout* navStackVlay = new QVBoxLayout;
    navStackVlay->setMargin(0);
    mainHlay->addLayout(navStackVlay);

    navStack = new QStackedWidget;
    navStackVlay->addWidget(navStack);

    navStackVlay->addStretch(1);

    QHBoxLayout* restoreDefaultHlay = new QHBoxLayout;
    navStackVlay->addLayout(restoreDefaultHlay);

    restoreDefaultHlay->addStretch(1);

    // ### Change this, OK, Cancel, to QDialogButtonBox
    QPushButton* restoreDefaultsButton = new QPushButton;
    restoreDefaultsButton->setText("&Restore Defaults");
    connect(restoreDefaultsButton, SIGNAL(clicked()),
            SLOT(restoreDefaultsClicked()));
    restoreDefaultHlay->addWidget(restoreDefaultsButton);

    // General Prefs
    generalPrefWidget = new QWidget;
    navStack->addWidget(generalPrefWidget);

    QVBoxLayout* generalPrefVlay = new QVBoxLayout(generalPrefWidget);
    generalPrefVlay->setMargin(0);

    QLabel* generalPrefLabel = new QLabel(MainSettings::GENERAL_PREFS_GROUP);
    generalPrefLabel->setFrameShape(QFrame::StyledPanel);
    generalPrefVlay->addWidget(generalPrefLabel);

    QGroupBox* autoImportGbox = new QGroupBox("Lexicon");
    generalPrefVlay->addWidget(autoImportGbox);
    generalPrefVlay->setStretchFactor(autoImportGbox, 1);

    QVBoxLayout* autoImportVlay = new QVBoxLayout(autoImportGbox);
    autoImportVlay->setMargin(MARGIN);
    autoImportVlay->setSpacing(SPACING);

    QHBoxLayout* autoImportLexiconHlay = new QHBoxLayout;
    autoImportLexiconHlay->setSpacing(SPACING);
    autoImportVlay->addLayout(autoImportLexiconHlay);

    autoImportCbox = new QCheckBox("Load lexicons: ");
    connect(autoImportCbox, SIGNAL(toggled(bool)),
            SLOT(autoImportCboxToggled(bool)));
    autoImportLexiconHlay->addWidget(autoImportCbox);

    autoImportButton = new ZPushButton;
    autoImportButton->setText("Edit...");
    autoImportButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(autoImportButton, SIGNAL(clicked()),
            SLOT(chooseLexiconsClicked()));
    autoImportLexiconHlay->addWidget(autoImportButton);

    autoImportLabel = new QLabel;
    autoImportVlay->addWidget(autoImportLabel);

    autoImportCustomWidget = new QWidget;
    autoImportVlay->addWidget(autoImportCustomWidget);

    QHBoxLayout* autoImportCustomHlay = new QHBoxLayout(autoImportCustomWidget);
    autoImportCustomHlay->setMargin(0);
    autoImportCustomHlay->setSpacing(SPACING);

    QLabel* autoImportCustomLabel = new QLabel("Custom:");
    autoImportCustomHlay->addWidget(autoImportCustomLabel);

    autoImportCustomLine = new QLineEdit;
    autoImportCustomLine->setReadOnly(true);
    autoImportCustomHlay->addWidget(autoImportCustomLine);

    QGroupBox* userDataDirGbox = new QGroupBox("Data Directory");
    generalPrefVlay->addWidget(userDataDirGbox);
    generalPrefVlay->setStretchFactor(userDataDirGbox, 1);

    QVBoxLayout* userDataDirVlay = new QVBoxLayout(userDataDirGbox);
    userDataDirVlay->setMargin(MARGIN);
    userDataDirVlay->setSpacing(SPACING);

    QHBoxLayout* userDataDirHlay = new QHBoxLayout;
    userDataDirHlay->setSpacing(SPACING);
    userDataDirVlay->addLayout(userDataDirHlay);

    QLabel* userDataDirLabel = new QLabel("Location:");
    userDataDirHlay->addWidget(userDataDirLabel);

    userDataDirLine = new QLineEdit;
    connect(userDataDirLine, SIGNAL(textChanged(const QString&)),
            SLOT(userDataDirChanged(const QString&)));
    userDataDirHlay->addWidget(userDataDirLine);

    ZPushButton* userDataDirBrowseButton = new ZPushButton("Browse...");
    connect(userDataDirBrowseButton, SIGNAL(clicked()),
            SLOT(userDataDirBrowseButtonClicked()));
    userDataDirHlay->addWidget(userDataDirBrowseButton);

    userDataDirMoveCbox = new QCheckBox("Move data after saving preferences");
    userDataDirMoveCbox->setEnabled(false);
    userDataDirVlay->addWidget(userDataDirMoveCbox);

    // FIXME: Come back to this when you figure out how to actually move a
    // directory!
    userDataDirMoveCbox->hide();

    QGroupBox* displayWelcomeGbox = new QGroupBox("Welcome");
    generalPrefVlay->addWidget(displayWelcomeGbox);
    generalPrefVlay->setStretchFactor(displayWelcomeGbox, 1);

    QVBoxLayout* displayWelcomeVlay = new QVBoxLayout(displayWelcomeGbox);
    displayWelcomeVlay->setMargin(MARGIN);
    displayWelcomeVlay->setSpacing(SPACING);

    displayWelcomeCbox = new QCheckBox("Display Welcome on startup");
    displayWelcomeVlay->addWidget(displayWelcomeCbox);

    generalPrefVlay->addStretch(2);

    // Quiz Prefs
    quizPrefWidget = new QWidget;
    navStack->addWidget(quizPrefWidget);

    QVBoxLayout* quizPrefVlay = new QVBoxLayout(quizPrefWidget);
    quizPrefVlay->setMargin(0);

    QLabel* quizPrefLabel = new QLabel(MainSettings::QUIZ_PREFS_GROUP);
    quizPrefLabel->setFrameShape(QFrame::StyledPanel);
    quizPrefVlay->addWidget(quizPrefLabel);

    QGroupBox* quizQuestionGbox = new QGroupBox("Question Display");
    quizPrefVlay->addWidget(quizQuestionGbox);
    quizPrefVlay->setStretchFactor(quizQuestionGbox, 1);

    QVBoxLayout* quizQuestionVlay = new QVBoxLayout(quizQuestionGbox);
    quizQuestionVlay->setMargin(MARGIN);
    quizQuestionVlay->setSpacing(SPACING);

    themeCbox = new QCheckBox("Use tile images");
    connect(themeCbox, SIGNAL(toggled(bool)), SLOT(themeCboxToggled(bool)));
    quizQuestionVlay->addWidget(themeCbox);

    QHBoxLayout* themeHlay = new QHBoxLayout;
    themeHlay->setSpacing(SPACING);
    quizQuestionVlay->addLayout(themeHlay);

    themeLabel = new QLabel("Tile Theme:");
    themeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    themeHlay->addWidget(themeLabel);

    themeCombo = new QComboBox;
    themeHlay->addWidget(themeCombo);

    QHBoxLayout* letterOrderHlay = new QHBoxLayout;
    quizQuestionVlay->addLayout(letterOrderHlay);

    QLabel* letterOrderLabel = new QLabel("Letter Order:");
    letterOrderLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    letterOrderHlay->addWidget(letterOrderLabel);

    letterOrderCombo = new QComboBox;
    letterOrderHlay->addWidget(letterOrderCombo);

    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_ALPHA);
    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_RANDOM);
    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_VOWELS_FIRST);
    letterOrderCombo->addItem(Defs::QUIZ_LETTERS_CONSONANTS_FIRST);
    letterOrderCombo->setCurrentIndex(0);

    QHBoxLayout* quizBackgroundColorHlay = new QHBoxLayout;
    quizQuestionVlay->addLayout(quizBackgroundColorHlay);

    QLabel* quizBackgroundColorLabel = new QLabel("Background Color:");
    quizBackgroundColorHlay->addWidget(quizBackgroundColorLabel);

    quizBackgroundColorLine = new QLineEdit;
    quizBackgroundColorLine->setReadOnly(true);
    quizBackgroundColorLine->setEnabled(false);
    quizBackgroundColorHlay->addWidget(quizBackgroundColorLine);

    ZPushButton* quizBackgroundColorButton = new ZPushButton("Choose...");
    connect(quizBackgroundColorButton, SIGNAL(clicked()),
            SLOT(chooseQuizBackgroundColorButtonClicked()));
    quizBackgroundColorHlay->addWidget(quizBackgroundColorButton);

    QGroupBox* quizBehaviorGbox = new QGroupBox("Quiz Behavior");
    quizPrefVlay->addWidget(quizBehaviorGbox);
    quizPrefVlay->setStretchFactor(quizBehaviorGbox, 1);

    QVBoxLayout* quizBehaviorVlay = new QVBoxLayout(quizBehaviorGbox);
    quizBehaviorVlay->setMargin(MARGIN);
    quizBehaviorVlay->setSpacing(SPACING);

    quizUseFlashcardModeCbox = new QCheckBox("Start in Flashcard Mode");
    quizBehaviorVlay->addWidget(quizUseFlashcardModeCbox);

    quizShowNumResponsesCbox =
        new QCheckBox("Show number of responses in status bar");
    quizBehaviorVlay->addWidget(quizShowNumResponsesCbox);

    quizShowQuestionStatsCbox = new QCheckBox("Show question statistics");
    quizBehaviorVlay->addWidget(quizShowQuestionStatsCbox);

    quizRequireLexiconSymbolsCbox =
        new QCheckBox("Require lexicon symbols for correct response");
    quizBehaviorVlay->addWidget(quizRequireLexiconSymbolsCbox);

    quizAutoCheckCbox =
        new QCheckBox("End question after all correct responses");
    connect(quizAutoCheckCbox, SIGNAL(toggled(bool)),
            SLOT(autoCheckCboxToggled(bool)));
    quizBehaviorVlay->addWidget(quizAutoCheckCbox);

    quizAutoAdvanceCbox =
        new QCheckBox("Go to next question after all correct responses");
    quizBehaviorVlay->addWidget(quizAutoAdvanceCbox);

    quizAutoEndAfterIncorrectCbox =
        new QCheckBox("End question after an incorrect response");
    quizBehaviorVlay->addWidget(quizAutoEndAfterIncorrectCbox);

    quizMarkMissedAfterIncorrectCbox =
        new QCheckBox("Mark question as missed after an incorrect response");
    quizBehaviorVlay->addWidget(quizMarkMissedAfterIncorrectCbox);

    quizMarkMissedAfterTimerCbox =
        new QCheckBox("Mark question as missed after timer expires");
    quizBehaviorVlay->addWidget(quizMarkMissedAfterTimerCbox);

    quizCycleAnswersCbox =
        new QCheckBox("Cycle answers after ending a question");
    quizBehaviorVlay->addWidget(quizCycleAnswersCbox);

    QHBoxLayout* timeoutDisableInputHlay = new QHBoxLayout;
    timeoutDisableInputHlay->setMargin(0);
    quizBehaviorVlay->addLayout(timeoutDisableInputHlay);

    quizTimeoutDisableInputCbox =
        new QCheckBox("After timer expires, disable input for:");
    connect(quizTimeoutDisableInputCbox, SIGNAL(toggled(bool)),
            SLOT(timeoutDisableInputCboxToggled(bool)));
    timeoutDisableInputHlay->addWidget(quizTimeoutDisableInputCbox);

    quizTimeoutDisableInputSbox = new QSpinBox;
    quizTimeoutDisableInputSbox->setMinimum(1);
    quizTimeoutDisableInputSbox->setMaximum(10000);
    quizTimeoutDisableInputSbox->setSuffix(" milliseconds");
    timeoutDisableInputHlay->addWidget(quizTimeoutDisableInputSbox);

    quizPrefVlay->addStretch(2);

    // Search Prefs
    probabilityPrefWidget = new QWidget;
    navStack->addWidget(probabilityPrefWidget);

    QVBoxLayout* probabilityPrefVlay = new QVBoxLayout(probabilityPrefWidget);
    probabilityPrefVlay->setMargin(0);

    QLabel* probabilityPrefLabel =
        new QLabel(MainSettings::PROBABILITY_PREFS_GROUP);
    probabilityPrefLabel->setFrameShape(QFrame::StyledPanel);
    probabilityPrefVlay->addWidget(probabilityPrefLabel);

    QGroupBox* probabilityGbox = new QGroupBox("Probability");
    probabilityPrefVlay->addWidget(probabilityGbox);
    probabilityPrefVlay->setStretchFactor(probabilityGbox, 1);

    QVBoxLayout* probabilityVlay = new QVBoxLayout(probabilityGbox);
    probabilityVlay->setMargin(MARGIN);
    probabilityVlay->setSpacing(SPACING);

    QHBoxLayout* probBlanksHlay = new QHBoxLayout;
    probBlanksHlay->setMargin(0);
    probabilityVlay->addLayout(probBlanksHlay);

    QLabel* probBlanksLabel = new QLabel;
    probBlanksLabel->setText("Default number of blanks when "
                                   "calculating probability:");
    probBlanksHlay->addWidget(probBlanksLabel);

    probBlanksSbox = new QSpinBox;
    probBlanksSbox->setMinimum(0);
    probBlanksSbox->setMaximum(Defs::MAX_BLANKS);
    probBlanksHlay->addWidget(probBlanksSbox);

    probabilityPrefVlay->addStretch(2);

    // Cardbox Prefs
    cardboxPrefWidget = new QWidget;
    navStack->addWidget(cardboxPrefWidget);

    QVBoxLayout* cardboxPrefVlay = new QVBoxLayout(cardboxPrefWidget);
    cardboxPrefVlay->setMargin(0);

    QLabel* cardboxPrefLabel = new QLabel(MainSettings::CARDBOX_PREFS_GROUP);
    cardboxPrefLabel->setFrameShape(QFrame::StyledPanel);
    cardboxPrefVlay->addWidget(cardboxPrefLabel);

    QGroupBox* cardboxScheduleGbox = new QGroupBox("Schedule");
    cardboxPrefVlay->addWidget(cardboxScheduleGbox);
    cardboxPrefVlay->setStretchFactor(cardboxScheduleGbox, 1);

    QGridLayout* cardboxScheduleGlay = new QGridLayout(cardboxScheduleGbox);
    cardboxScheduleGlay->setMargin(MARGIN);
    cardboxScheduleGlay->setSpacing(SPACING);

    int numColsPerCardbox = 4;
    int cardboxesPerCol = 8;
    int maxCardbox = 15;

    QLabel* cardboxScheduleLabel = new QLabel;
    cardboxScheduleLabel->setWordWrap(true);
    cardboxScheduleLabel->setText(
        "Questions in each cardbox are scheduled for the specified number of "
        "days in the future, plus or minus the associated window of days.");
    cardboxScheduleGlay->addWidget(cardboxScheduleLabel, 0, 0, 1,
        numColsPerCardbox * ((maxCardbox + 1) / cardboxesPerCol));

    for (int i = 0; i <= maxCardbox; ++i) {
        int col = (i / cardboxesPerCol) * numColsPerCardbox;
        int row = (i % cardboxesPerCol) + 1;

        QLabel* label = new QLabel;
        QString numStr = QString::number(i);
        if (i == maxCardbox)
            numStr += "+";
        label->setText(QString("Cardbox %1:").arg(numStr));
        cardboxScheduleGlay->addWidget(label, row, col);

        QSpinBox* scheduleSbox = new QSpinBox;
        scheduleSbox->setMinimum(0);
        scheduleSbox->setMaximum(9999);
        cardboxScheduleGlay->addWidget(scheduleSbox, row, col + 1);
        cardboxScheduleSboxList.append(scheduleSbox);

        QLabel* plusMinusLabel = new QLabel;
        plusMinusLabel->setTextFormat(Qt::RichText);
        plusMinusLabel->setText("&#177;");
        cardboxScheduleGlay->addWidget(plusMinusLabel, row, col + 2);

        QSpinBox* windowSbox = new QSpinBox;
        windowSbox->setMinimum(0);
        windowSbox->setMaximum(9999);
        cardboxScheduleGlay->addWidget(windowSbox, row, col + 3);
        cardboxWindowSboxList.append(windowSbox);
    }

    cardboxPrefVlay->addStretch(2);

    // Word Judge Prefs
    judgePrefWidget = new QWidget;
    navStack->addWidget(judgePrefWidget);

    QVBoxLayout* judgePrefVlay = new QVBoxLayout(judgePrefWidget);
    judgePrefVlay->setMargin(0);

    QLabel* judgePrefLabel = new QLabel(MainSettings::JUDGE_PREFS_GROUP);
    judgePrefLabel->setFrameShape(QFrame::StyledPanel);
    judgePrefVlay->addWidget(judgePrefLabel);

    QGroupBox* judgeGbox = new QGroupBox;
    judgePrefVlay->addWidget(judgeGbox);
    judgePrefVlay->setStretchFactor(judgeGbox, 1);

    QVBoxLayout* judgeVlay = new QVBoxLayout(judgeGbox);
    judgeVlay->setMargin(MARGIN);
    judgeVlay->setSpacing(SPACING);

    judgeSaveLogCbox = new QCheckBox("Save judge results to log files");
    connect(judgeSaveLogCbox, SIGNAL(toggled(bool)),
            SLOT(judgeSaveLogCboxToggled(bool)));
    judgeVlay->addWidget(judgeSaveLogCbox);

    judgeLogDirWidget = new QWidget;
    judgeVlay->addWidget(judgeLogDirWidget);

    QHBoxLayout* judgeLogDirHlay = new QHBoxLayout(judgeLogDirWidget);
    judgeLogDirHlay->setMargin(0);
    judgeLogDirHlay->setSpacing(SPACING);

    judgeLogDirLabel = new QLabel;
    judgeLogDirHlay->addWidget(judgeLogDirLabel);

    judgePrefVlay->addStretch(2);

    // Signal mapper for the Choose Font buttons
    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)),
            SLOT(chooseFontButtonClicked(int)));

    // Font Prefs
    fontPrefWidget = new QWidget;
    navStack->addWidget(fontPrefWidget);

    QVBoxLayout* fontPrefVlay = new QVBoxLayout(fontPrefWidget);
    fontPrefVlay->setMargin(0);

    QLabel* fontPrefLabel = new QLabel(MainSettings::FONT_PREFS_GROUP);
    fontPrefLabel->setFrameShape(QFrame::StyledPanel);
    fontPrefVlay->addWidget(fontPrefLabel);

    QGroupBox* fontGbox = new QGroupBox;
    fontPrefVlay->addWidget(fontGbox);
    fontPrefVlay->setStretchFactor(fontGbox, 1);

    QGridLayout* fontGlay = new QGridLayout(fontGbox);
    fontGlay->setMargin(MARGIN);
    fontGlay->setSpacing(SPACING);

    // Main font
    int row = 0;
    QLabel* fontMainLabel = new QLabel("Main:");
    fontGlay->addWidget(fontMainLabel, row, 0, Qt::AlignLeft);

    fontMainLine = new QLineEdit;
    fontMainLine->setReadOnly(true);
    fontMainLine->setText(this->font().toString());
    fontMainLine->home(false);
    fontGlay->addWidget(fontMainLine, row, 1);

    ZPushButton* chooseFontMainButton = new ZPushButton("Choose...");
    connect(chooseFontMainButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(chooseFontMainButton, FONT_MAIN_BUTTON);
    fontGlay->addWidget(chooseFontMainButton, row, 2);

    // Word list font
    ++row;
    QLabel* fontWordListLabel = new QLabel("Word Tables:");
    fontGlay->addWidget(fontWordListLabel, row, 0, Qt::AlignLeft);

    fontWordListLine = new QLineEdit;
    fontWordListLine->setReadOnly(true);
    fontWordListLine->setText(this->font().toString());
    fontWordListLine->home(false);
    fontGlay->addWidget(fontWordListLine, row, 1);

    ZPushButton* chooseFontWordListButton = new ZPushButton("Choose...");
    connect(chooseFontWordListButton, SIGNAL(clicked()), signalMapper,
            SLOT(map()));
    signalMapper->setMapping(chooseFontWordListButton, FONT_WORD_LISTS_BUTTON);
    fontGlay->addWidget(chooseFontWordListButton, row, 2);

    // Quiz label font
    ++row;
    QLabel* fontQuizLabelLabel = new QLabel("Quiz Questions:");
    fontGlay->addWidget(fontQuizLabelLabel, row, 0, Qt::AlignLeft);

    fontQuizLabelLine = new QLineEdit;
    fontQuizLabelLine->setReadOnly(true);
    fontQuizLabelLine->setText(this->font().toString());
    fontQuizLabelLine->home(false);
    fontGlay->addWidget(fontQuizLabelLine, row, 1);

    ZPushButton* chooseFontQuizLabelButton = new ZPushButton("Choose...");
    connect(chooseFontQuizLabelButton, SIGNAL(clicked()), signalMapper,
            SLOT(map()));
    signalMapper->setMapping(chooseFontQuizLabelButton, FONT_QUIZ_LABEL_BUTTON);
    fontGlay->addWidget(chooseFontQuizLabelButton, row, 2);

    // Input font
    ++row;
    QLabel* fontInputLabel = new QLabel("Word Input:");
    fontGlay->addWidget(fontInputLabel, row, 0, Qt::AlignLeft);

    fontWordInputLine = new QLineEdit;
    fontWordInputLine->setReadOnly(true);
    fontWordInputLine->setText(this->font().toString());
    fontWordInputLine->home(false);
    fontGlay->addWidget(fontWordInputLine, row, 1);

    ZPushButton* chooseFontInputButton = new ZPushButton("Choose...");
    connect(chooseFontInputButton, SIGNAL(clicked()), signalMapper,
            SLOT(map()));
    signalMapper->setMapping(chooseFontInputButton, FONT_WORD_INPUT_BUTTON);
    fontGlay->addWidget(chooseFontInputButton, row, 2);

    // Definition font
    ++row;
    QLabel* fontDefinitionLabel = new QLabel("Definitions:");
    fontGlay->addWidget(fontDefinitionLabel, row, 0, Qt::AlignLeft);

    fontDefinitionLine = new QLineEdit;
    fontDefinitionLine->setReadOnly(true);
    fontDefinitionLine->setText(this->font().toString());
    fontDefinitionLine->home(false);
    fontGlay->addWidget(fontDefinitionLine, row, 1);

    ZPushButton* chooseFontDefinitionButton = new ZPushButton("Choose...");
    connect(chooseFontDefinitionButton, SIGNAL(clicked()), signalMapper,
            SLOT(map()));
    signalMapper->setMapping(chooseFontDefinitionButton,
                             FONT_DEFINITIONS_BUTTON);
    fontGlay->addWidget(chooseFontDefinitionButton, row, 2);

    fontPrefVlay->addStretch(2);

    // Word List Prefs
    wordListPrefWidget = new QWidget;
    navStack->addWidget(wordListPrefWidget);

    QVBoxLayout* wordListPrefVlay = new QVBoxLayout(wordListPrefWidget);
    wordListPrefVlay->setMargin(0);

    QLabel* wordListPrefLabel = new QLabel(MainSettings::WORD_LIST_PREFS_GROUP);
    wordListPrefLabel->setFrameShape(QFrame::StyledPanel);
    wordListPrefVlay->addWidget(wordListPrefLabel);

    QGroupBox* wordListDisplayGbox = new QGroupBox("Display Columns");
    wordListPrefVlay->addWidget(wordListDisplayGbox);
    wordListPrefVlay->setStretchFactor(wordListDisplayGbox, 1);

    QVBoxLayout* wordListDisplayVlay = new QVBoxLayout(wordListDisplayGbox);
    wordListDisplayVlay->setMargin(MARGIN);
    wordListDisplayVlay->setSpacing(SPACING);

    showProbabilityOrderCbox = new QCheckBox("Show probability order");
    wordListDisplayVlay->addWidget(showProbabilityOrderCbox);

    showPlayabilityOrderCbox = new QCheckBox("Show playability order");
    wordListDisplayVlay->addWidget(showPlayabilityOrderCbox);

    showHooksCbox = new QCheckBox("Show hooks");
    wordListDisplayVlay->addWidget(showHooksCbox);

    showHookParentsCbox = new QCheckBox("Show inner hooks");
    connect(showHookParentsCbox, SIGNAL(toggled(bool)),
            SLOT(showHookParentsCboxToggled(bool)));
    wordListDisplayVlay->addWidget(showHookParentsCbox);

    useHookParentHyphensCbox = new QCheckBox("Display inner hooks as hyphens");
    wordListDisplayVlay->addWidget(useHookParentHyphensCbox);

    showDefinitionCbox = new QCheckBox("Show definitions");
    wordListDisplayVlay->addWidget(showDefinitionCbox);

    lowerCaseWildcardsCbox =
        new QCheckBox("Use lower-case for wildcard matches");
    wordListDisplayVlay->addWidget(lowerCaseWildcardsCbox);

    lexiconStyleCbox = new QCheckBox("Show lexicon symbols");
    connect(lexiconStyleCbox, SIGNAL(toggled(bool)),
            SLOT(lexiconStyleCboxToggled(bool)));
    wordListDisplayVlay->addWidget(lexiconStyleCbox);

    QHBoxLayout* lexiconStyleHlay = new QHBoxLayout;
    wordListDisplayVlay->addLayout(lexiconStyleHlay);

    lexiconStyleLabel = new QLabel;
    lexiconStyleHlay->addWidget(lexiconStyleLabel);

    lexiconStyleButton = new ZPushButton;
    lexiconStyleButton->setText("Edit...");
    lexiconStyleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(lexiconStyleButton, SIGNAL(clicked()),
            SLOT(chooseLexiconStyleClicked()));
    lexiconStyleHlay->addWidget(lexiconStyleButton);

    QGroupBox* wordListSortGbox = new QGroupBox("Sorting and Grouping");
    wordListPrefVlay->addWidget(wordListSortGbox);
    wordListPrefVlay->setStretchFactor(wordListSortGbox, 1);

    QVBoxLayout* wordListSortVlay = new QVBoxLayout(wordListSortGbox);
    wordListSortVlay->setMargin(MARGIN);
    wordListSortVlay->setSpacing(SPACING);

    lengthSortCbox = new QCheckBox("Sort by word length");
    wordListSortVlay->addWidget(lengthSortCbox);

    anagramGroupCbox = new QCheckBox("Group by anagrams");
    wordListSortVlay->addWidget(anagramGroupCbox);

    wordListPrefVlay->addStretch(2);

    // Button layout
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing(SPACING);
    mainVlay->addLayout(buttonHlay);

    buttonHlay->addStretch(1);

    ZPushButton* okButton = new ZPushButton("OK");
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), SLOT(accept()));
    buttonHlay->addWidget(okButton);

    ZPushButton* cancelButton = new ZPushButton("Cancel");
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
    buttonHlay->addWidget(cancelButton);

    // Add nav list items
    new QListWidgetItem(MainSettings::GENERAL_PREFS_GROUP, navList);
    new QListWidgetItem(MainSettings::QUIZ_PREFS_GROUP, navList);
    new QListWidgetItem(MainSettings::PROBABILITY_PREFS_GROUP, navList);
    new QListWidgetItem(MainSettings::CARDBOX_PREFS_GROUP, navList);
    new QListWidgetItem(MainSettings::JUDGE_PREFS_GROUP, navList);
    new QListWidgetItem(MainSettings::FONT_PREFS_GROUP, navList);
    new QListWidgetItem(MainSettings::WORD_LIST_PREFS_GROUP, navList);
    navList->setCurrentRow(0);

    setWindowTitle(DIALOG_CAPTION);
    readSettings();

    quizBackgroundColor = MainSettings::getQuizBackgroundColor();
    QPalette colorPalette (quizBackgroundColor);
    quizBackgroundColorLine->setPalette(colorPalette);
}

//---------------------------------------------------------------------------
//  ~SettingsDialog
//
//! Destructor.  Save application settings.
//---------------------------------------------------------------------------
SettingsDialog::~SettingsDialog()
{
}

//---------------------------------------------------------------------------
//  readSettings
//
//! Read settings.
//---------------------------------------------------------------------------
void
SettingsDialog::readSettings()
{
    MainSettings::readSettings();
    refreshSettings();
}

//---------------------------------------------------------------------------
//  refreshSettings
//
//! Refresh settings from the main settings object.
//---------------------------------------------------------------------------
void
SettingsDialog::refreshSettings()
{
    bool autoImport = MainSettings::getUseAutoImport();
    autoImportCbox->setChecked(autoImport);

    QStringList autoImportLexicons = MainSettings::getAutoImportLexicons();
    QString defaultLexicon = MainSettings::getDefaultLexicon();
    setImportLexicons(autoImportLexicons, defaultLexicon);

    QString autoImportFile = MainSettings::getAutoImportFile();
    autoImportCustomLine->setText(autoImportFile);

    origUserDataDir = MainSettings::getUserDataDir();
    userDataDirLine->setText(origUserDataDir);

    displayWelcomeCbox->setChecked(MainSettings::getDisplayWelcome());

    fillThemeCombo();
    bool useTileTheme = MainSettings::getUseTileTheme();
    themeCbox->setChecked(useTileTheme);
    if (themeCbox->isEnabled()) {
        QString tileTheme = MainSettings::getTileTheme();
        QString themeStr;
        for (int i = 0; i < themeCombo->count(); ++i) {
            themeStr = themeCombo->itemText(i);
            if (themeStr == tileTheme) {
                themeCombo->setCurrentIndex(i);
                break;
            }
        }
        themeCboxToggled(useTileTheme);
    }

    // Quiz letter order
    int letterOrderIndex =
        letterOrderCombo->findText(MainSettings::getQuizLetterOrder());
    if (letterOrderIndex < 0) {
        letterOrderIndex = letterOrderCombo->findText(Defs::QUIZ_LETTERS_ALPHA);
    }
    if (letterOrderIndex < 0)
        letterOrderIndex = 0;
    letterOrderCombo->setCurrentIndex(letterOrderIndex);

    quizBackgroundColor = MainSettings::getQuizBackgroundColor();
    QPalette quizBackgroundPalette (quizBackgroundColor);
    quizBackgroundColorLine->setPalette(quizBackgroundPalette);

    quizUseFlashcardModeCbox->setChecked(
        MainSettings::getQuizUseFlashcardMode());
    quizShowNumResponsesCbox->setChecked(
        MainSettings::getQuizShowNumResponses());
    quizShowQuestionStatsCbox->setChecked(
        MainSettings::getQuizShowQuestionStats());
    quizRequireLexiconSymbolsCbox->setChecked(
        MainSettings::getQuizRequireLexiconSymbols());
    bool autoCheck = MainSettings::getQuizAutoCheck();
    quizAutoCheckCbox->setChecked(autoCheck);
    quizAutoAdvanceCbox->setChecked(MainSettings::getQuizAutoAdvance());
    quizAutoEndAfterIncorrectCbox->setChecked(
        MainSettings::getQuizAutoEndAfterIncorrect());
    quizMarkMissedAfterIncorrectCbox->setChecked(
        MainSettings::getQuizMarkMissedAfterIncorrect());
    quizMarkMissedAfterTimerCbox->setChecked(
        MainSettings::getQuizMarkMissedAfterTimerExpires());
    quizCycleAnswersCbox->setChecked(MainSettings::getQuizCycleAnswers());
    quizTimeoutDisableInputCbox->setChecked(
        MainSettings::getQuizTimeoutDisableInput());
    quizTimeoutDisableInputSbox->setValue(
        MainSettings::getQuizTimeoutDisableInputMillisecs());
    autoCheckCboxToggled(autoCheck);

    probBlanksSbox->setValue(
        MainSettings::getProbabilityNumBlanks());

    judgeSaveLogCbox->setChecked(MainSettings::getJudgeSaveLog());

    // Cardbox schedules
    QList<int> cardboxSchedules = MainSettings::getCardboxScheduleList();
    int i = 0;
    int lastSched = 0;
    foreach (QSpinBox* sbox, cardboxScheduleSboxList) {
        int sched = (i >= cardboxSchedules.count()) ? lastSched
            : cardboxSchedules[i];
        sbox->setValue(sched);
        lastSched = sched;
        ++i;
    }

    // Cardbox windows
    QList<int> cardboxWindows = MainSettings::getCardboxWindowList();
    i = 0;
    int lastWindow = 0;
    foreach (QSpinBox* sbox, cardboxWindowSboxList) {
        int window = (i >= cardboxWindows.count()) ? lastWindow
            : cardboxWindows[i];
        sbox->setValue(window);
        lastWindow = window;
        ++i;
    }

    // Main font
    fontMainLine->setText(MainSettings::getMainFont());
    fontMainLine->home(false);

    // Word list font
    fontWordListLine->setText(MainSettings::getWordListFont());
    fontWordListLine->home(false);

    // Quiz label font
    fontQuizLabelLine->setText(MainSettings::getQuizLabelFont());
    fontQuizLabelLine->home(false);

    // Word input font
    fontWordInputLine->setText(MainSettings::getWordInputFont());
    fontWordInputLine->home(false);

    // Definition font
    fontDefinitionLine->setText(MainSettings::getDefinitionFont());
    fontDefinitionLine->home(false);

    lengthSortCbox->setChecked(MainSettings::getWordListSortByLength());
    anagramGroupCbox->setChecked(MainSettings::getWordListGroupByAnagrams());
    showProbabilityOrderCbox->setChecked(
        MainSettings::getWordListShowProbabilityOrder());
    showPlayabilityOrderCbox->setChecked(
        MainSettings::getWordListShowPlayabilityOrder());
    showHooksCbox->setChecked(MainSettings::getWordListShowHooks());
    bool showHookParents = MainSettings::getWordListShowHookParents();
    showHookParentsCbox->setChecked(showHookParents);
    useHookParentHyphensCbox->setChecked(
        MainSettings::getWordListUseHookParentHyphens());
    showDefinitionCbox->setChecked(MainSettings::getWordListShowDefinitions());
    lowerCaseWildcardsCbox->setChecked(
        MainSettings::getWordListLowerCaseWildcards());
    showHookParentsCboxToggled(showHookParents);

    bool useLexiconStyles = MainSettings::getWordListUseLexiconStyles();
    lexiconStyleCbox->setChecked(useLexiconStyles);
    lexiconStyleCboxToggled(useLexiconStyles);

    setLexiconStyles(MainSettings::getWordListLexiconStyles());

    updateJudgeLogDirLabel();
}

//---------------------------------------------------------------------------
//  writeSettings
//
//! Write settings.
//---------------------------------------------------------------------------
void
SettingsDialog::writeSettings()
{
    MainSettings::setUseAutoImport(autoImportCbox->isChecked());

    QStringList importLexicons;
    QString defaultLexicon;
    getImportLexicons(importLexicons, defaultLexicon);
    MainSettings::setAutoImportLexicons(importLexicons);
    MainSettings::setDefaultLexicon(defaultLexicon);
    MainSettings::setAutoImportFile(autoImportCustomLine->text());
    MainSettings::setDisplayWelcome(displayWelcomeCbox->isChecked());
    MainSettings::setUserDataDir(userDataDirLine->text());
    MainSettings::setUseTileTheme(themeCbox->isChecked());
    MainSettings::setTileTheme(themeCombo->currentText());
    MainSettings::setQuizLetterOrder(letterOrderCombo->currentText());
    MainSettings::setQuizBackgroundColor(quizBackgroundColor);
    MainSettings::setQuizUseFlashcardMode(
        quizUseFlashcardModeCbox->isChecked());
    MainSettings::setQuizShowNumResponses(
        quizShowNumResponsesCbox->isChecked());
    MainSettings::setQuizShowQuestionStats(
        quizShowQuestionStatsCbox->isChecked());
    MainSettings::setQuizRequireLexiconSymbols(
        quizRequireLexiconSymbolsCbox->isChecked());
    MainSettings::setQuizAutoCheck(quizAutoCheckCbox->isChecked());
    MainSettings::setQuizAutoAdvance(quizAutoAdvanceCbox->isChecked());
    MainSettings::setQuizAutoEndAfterIncorrect(
        quizAutoEndAfterIncorrectCbox->isChecked());
    MainSettings::setQuizMarkMissedAfterIncorrect(
        quizMarkMissedAfterIncorrectCbox->isChecked());
    MainSettings::setQuizMarkMissedAfterTimerExpires(
        quizMarkMissedAfterTimerCbox->isChecked());
    MainSettings::setQuizCycleAnswers(quizCycleAnswersCbox->isChecked());
    MainSettings::setQuizTimeoutDisableInput(
        quizTimeoutDisableInputCbox->isChecked());
    MainSettings::setQuizTimeoutDisableInputMillisecs(
        quizTimeoutDisableInputSbox->value());

    MainSettings::setProbabilityNumBlanks(probBlanksSbox->value());

    QList<int> cardboxSchedules;
    foreach (QSpinBox* sbox, cardboxScheduleSboxList)
        cardboxSchedules.append(sbox->value());
    MainSettings::setCardboxScheduleList(cardboxSchedules);

    QList<int> cardboxWindows;
    foreach (QSpinBox* sbox, cardboxWindowSboxList)
        cardboxWindows.append(sbox->value());
    MainSettings::setCardboxWindowList(cardboxWindows);

    MainSettings::setJudgeSaveLog(judgeSaveLogCbox->isChecked());
    MainSettings::setMainFont(fontMainLine->text());
    MainSettings::setWordListFont(fontWordListLine->text());
    MainSettings::setQuizLabelFont(fontQuizLabelLine->text());
    MainSettings::setWordInputFont(fontWordInputLine->text());
    MainSettings::setDefinitionFont(fontDefinitionLine->text());
    MainSettings::setWordListSortByLength(lengthSortCbox->isChecked());
    MainSettings::setWordListGroupByAnagrams(anagramGroupCbox->isChecked());
    MainSettings::setWordListShowProbabilityOrder(
        showProbabilityOrderCbox->isChecked());
    MainSettings::setWordListShowPlayabilityOrder(
        showPlayabilityOrderCbox->isChecked());
    MainSettings::setWordListShowHooks(showHooksCbox->isChecked());
    MainSettings::setWordListShowHookParents(showHookParentsCbox->isChecked());
    MainSettings::setWordListUseHookParentHyphens(
        useHookParentHyphensCbox->isChecked());
    MainSettings::setWordListShowDefinitions(showDefinitionCbox->isChecked());
    MainSettings::setWordListLowerCaseWildcards(
        lowerCaseWildcardsCbox->isChecked());
    MainSettings::setWordListLexiconStyles(getLexiconStyles());
    MainSettings::writeSettings();
}

//---------------------------------------------------------------------------
//  navTextChanged
//
//! Slot called when the current item in the navigation list changes.  Raise
//! the corresponding frame in the stacked widget.
//
//! @param text the current text in the navigation list
//---------------------------------------------------------------------------
void
SettingsDialog::navTextChanged(const QString& text)
{
    if (text == MainSettings::GENERAL_PREFS_GROUP)
        navStack->setCurrentWidget(generalPrefWidget);
    else if (text == MainSettings::QUIZ_PREFS_GROUP)
        navStack->setCurrentWidget(quizPrefWidget);
    else if (text == MainSettings::PROBABILITY_PREFS_GROUP)
        navStack->setCurrentWidget(probabilityPrefWidget);
    else if (text == MainSettings::CARDBOX_PREFS_GROUP)
        navStack->setCurrentWidget(cardboxPrefWidget);
    else if (text == MainSettings::JUDGE_PREFS_GROUP)
        navStack->setCurrentWidget(judgePrefWidget);
    else if (text == MainSettings::FONT_PREFS_GROUP)
        navStack->setCurrentWidget(fontPrefWidget);
    else if (text == MainSettings::WORD_LIST_PREFS_GROUP)
        navStack->setCurrentWidget(wordListPrefWidget);
}

//---------------------------------------------------------------------------
//  restoreDefaultsClicked
//
//! Slot called when the Restore Defaults button is clicked.
//---------------------------------------------------------------------------
void
SettingsDialog::restoreDefaultsClicked()
{
    MainSettings::restoreDefaults(navList->selectedItems().first()->text());
    refreshSettings();
}

//---------------------------------------------------------------------------
//  autoImportCboxToggled
//
//! Slot called when the Auto Import check box is toggled.  Enable or
//! disable the auto-import file edit area.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::autoImportCboxToggled(bool on)
{
    autoImportLabel->setEnabled(on);
    autoImportButton->setEnabled(on);

    QStringList lexicons;
    QString defaultLexicon;
    getImportLexicons(lexicons, defaultLexicon);
    autoImportCustomWidget->setEnabled(lexicons.contains(LEXICON_CUSTOM));
}

//---------------------------------------------------------------------------
//  autoImportLexiconActivated
//
//! Slot called when the Auto Import Lexicon combo box is activated.  Enable
//! or disable the auto-import custom file edit area.
//
//! @param text the activated text in the combo box
//---------------------------------------------------------------------------
void
SettingsDialog::autoImportLexiconActivated(const QString& text)
{
    bool customActive = (text == LEXICON_CUSTOM);
    autoImportCustomWidget->setEnabled(customActive);
}

//---------------------------------------------------------------------------
//  chooseLexiconsClicked
//
//! Slot called when the Choose Lexicons button is clicked.  Display the
//! lexicon selection dialog.
//---------------------------------------------------------------------------
void
SettingsDialog::chooseLexiconsClicked()
{
    QString defaultLexicon;
    QStringList importLexicons;
    getImportLexicons(importLexicons, defaultLexicon);

    LexiconSelectDialog* dialog = new LexiconSelectDialog(this);
    dialog->setImportLexicons(importLexicons);
    dialog->setDefaultLexicon(defaultLexicon);
    dialog->setCustomLexiconFile(autoImportCustomLine->text());

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        QStringList lexicons = dialog->getImportLexicons();
        QString defaultLexicon = dialog->getDefaultLexicon();
        setImportLexicons(lexicons, defaultLexicon);
        autoImportCustomLine->setText(dialog->getCustomLexiconFile());
    }

    delete dialog;
}

//---------------------------------------------------------------------------
//  userDataDirBrowseButtonClicked
//
//! Slot called when the User Data Directory Browse button is clicked.  Create
//! a directory chooser dialog and place the name of the chosen directory in
//! the user data directory line edit.
//---------------------------------------------------------------------------
void
SettingsDialog::userDataDirBrowseButtonClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
        USER_DATA_DIR_CHOOSER_TITLE, Auxil::getUserDir());

    if (!dir.isNull())
        userDataDirLine->setText(dir);
}

//---------------------------------------------------------------------------
//  userDataDirChanged
//
//! Slot called when the User Data Directory preference is changed.  Enable
//! the checkbox that determines whether the data directory should be moved
//! immediately.
//---------------------------------------------------------------------------
void
SettingsDialog::userDataDirChanged(const QString& text)
{
    if (text == origUserDataDir) {
        userDataDirMoveCbox->setCheckState(Qt::Unchecked);
        userDataDirMoveCbox->setEnabled(false);
    }

    else if (!userDataDirMoveCbox->isEnabled()) {
        userDataDirMoveCbox->setEnabled(true);
        userDataDirMoveCbox->setCheckState(Qt::Checked);
    }

    updateJudgeLogDirLabel();
}

//---------------------------------------------------------------------------
//  autoCheckCboxToggled
//
//! Slot called when the Auto Check check box is toggled.  Enable or disable
//! the auto-advance check box.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::autoCheckCboxToggled(bool on)
{
    quizAutoAdvanceCbox->setEnabled(on);
}

//---------------------------------------------------------------------------
//  themeCboxToggled
//
//! Slot called when the Tile Theme check box is toggled.  Enable or disable
//! the tile theme selection area.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::themeCboxToggled(bool on)
{
    themeLabel->setEnabled(on);
    themeCombo->setEnabled(on);
}

//---------------------------------------------------------------------------
//  judgeSaveLogCboxToggled
//
//! Slot called when the Save Judge Log check box is toggled.  Enable or
//! disable the judge log directory line edit.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::judgeSaveLogCboxToggled(bool on)
{
    judgeLogDirWidget->setEnabled(on);
}

//---------------------------------------------------------------------------
//  showHookParentsCboxToggled
//
//! Slot called when the Show Inner Hooks check box is toggled. Enable or
//! disable the Show Inner Hooks As Hyphens check box.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::showHookParentsCboxToggled(bool on)
{
    useHookParentHyphensCbox->setEnabled(on);
}

//---------------------------------------------------------------------------
//  chooseFontButtonClicked
//
//! Slot called when a Choose Font button is clicked.  Create a font chooser
//! dialog and place the name of the chosen font in the corresponding font
//! line edit.
//
//! @param button the identifier of the button that was clicked
//---------------------------------------------------------------------------
void
SettingsDialog::chooseFontButtonClicked(int button)
{
    bool ok = false;

    QLineEdit* lineEdit = 0;
    switch (button) {
        case FONT_MAIN_BUTTON:        lineEdit = fontMainLine; break;
        case FONT_WORD_LISTS_BUTTON:  lineEdit = fontWordListLine; break;
        case FONT_QUIZ_LABEL_BUTTON:  lineEdit = fontQuizLabelLine; break;
        case FONT_WORD_INPUT_BUTTON:  lineEdit = fontWordInputLine; break;
        case FONT_DEFINITIONS_BUTTON: lineEdit = fontDefinitionLine; break;
        default: return;
    }

    QFont currentFont = this->font();
    currentFont.fromString(lineEdit->text());
    QFont newFont = QFontDialog::getFont(&ok, currentFont, this);
    if (ok) {
        lineEdit->setText(newFont.toString());
        lineEdit->home(false);
    }
}

//---------------------------------------------------------------------------
//  chooseQuizBackgroundColorButtonClicked
//
//! Slot called when a Choose button is clicked to choose a quiz background
//! color.  Create a color chooser dialog and place the name of the chosen
//! color in the color line edit.
//---------------------------------------------------------------------------
void
SettingsDialog::chooseQuizBackgroundColorButtonClicked()
{
    QColor color =
        QColorDialog::getColor(MainSettings::getQuizBackgroundColor());
    if (color.isValid()) {
        QPalette palette (color);
        quizBackgroundColorLine->setPalette(palette);
        quizBackgroundColor = color;
    }
}

//---------------------------------------------------------------------------
//  timeoutDisableInputCboxToggled
//
//! Slot called when the Disable Input When Timer Expires check box is
//! toggled.  Enable or disable the duration spin box.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::timeoutDisableInputCboxToggled(bool on)
{
    quizTimeoutDisableInputSbox->setEnabled(on);
}

//---------------------------------------------------------------------------
//  lexiconStyleCboxToggled
//
//! Slot called when the Lexicon Style check box is toggled.  Enable or
//! disable the lexicon style edit area.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::lexiconStyleCboxToggled(bool on)
{
    lexiconStyleLabel->setEnabled(on);
    lexiconStyleButton->setEnabled(on);
}

//---------------------------------------------------------------------------
//  chooseLexiconStyleClicked
//
//! Slot called when the Choose Lexicon Style button is clicked.  Display the
//! lexicon style selection dialog.
//---------------------------------------------------------------------------
void
SettingsDialog::chooseLexiconStyleClicked()
{
    QList<LexiconStyle> styles = getLexiconStyles();
    LexiconStyleDialog* dialog = new LexiconStyleDialog(this);
    dialog->setLexiconStyles(styles);

    int code = dialog->exec();
    if (code == QDialog::Accepted) {
        setLexiconStyles(dialog->getLexiconStyles());
    }
    delete dialog;
}

//---------------------------------------------------------------------------
//  setImportLexicons
//
//! Set the contents of the Lexicon Import label with a list of lexicons to
//! import and a default lexicon.
//
//! @param lexicons the list of lexicons to import
//! @param defaultLexicon the default lexicon
//---------------------------------------------------------------------------
void
SettingsDialog::setImportLexicons(const QStringList& lexicons, const QString&
                                  defaultLexicon)
{
    QString importText;
    QStringListIterator it (lexicons);
    while (it.hasNext()) {
        const QString& lexicon = it.next();

        if (!importText.isEmpty())
            importText += LEXICON_SEP;

        importText += (lexicon == defaultLexicon) ?
            QString("<b>%1</b>").arg(lexicon) : lexicon;
    }
    autoImportLabel->setText(importText);
    autoImportCboxToggled(autoImportCbox->isChecked());
}

//---------------------------------------------------------------------------
//  getImportLexicons
//
//! Determine the import lexicons and default lexicon from the contents of the
//! import lexicon label.
//
//! @param lexicons return the list of import lexicons
//! @param defaultlexicon return the default lexicon
//---------------------------------------------------------------------------
void
SettingsDialog::getImportLexicons(QStringList& lexicons, QString&
                                  defaultLexicon) const
{
    QRegExp defaultRegex ("<b>(\\S+)</b>");
    QString autoImportText = autoImportLabel->text();
    if (autoImportText.contains(defaultRegex)) {
        defaultLexicon = defaultRegex.cap(1);
        autoImportText.replace(defaultRegex, "\\1");
    }
    lexicons = autoImportText.split(LEXICON_SEP);
}

//---------------------------------------------------------------------------
//  setLexiconStyles
//
//! Set the contents of the Lexicon Style label with a list of lexicon styles.
//
//! @param styles the list of lexicon styles
//---------------------------------------------------------------------------
void
SettingsDialog::setLexiconStyles(const QList<LexiconStyle>& styles)
{
    QString str;
    QListIterator<LexiconStyle> it (styles);
    while (it.hasNext()) {
        const LexiconStyle& style = it.next();
        if (!str.isEmpty())
            str += "\n";
        str += Auxil::lexiconStyleToString(style);
    }
    lexiconStyleLabel->setText(str);
}

//---------------------------------------------------------------------------
//  getLexiconStyles
//
//! Determine the lexicon styles from the contents of the lexicon style label.
//
//! @return the list of lexicon styles
//---------------------------------------------------------------------------
QList<LexiconStyle>
SettingsDialog::getLexiconStyles() const
{
    QList<LexiconStyle> styles;
    foreach (const QString& str, lexiconStyleLabel->text().split(QChar('\n'))) {
        LexiconStyle style = Auxil::stringToLexiconStyle(str);
        if (!style.isValid())
            continue;
        styles.append(style);
    }
    return styles;
}

//---------------------------------------------------------------------------
//  fillThemeCombo
//
//! Fill the Tile Theme combo box with the list of available tile themes.
//---------------------------------------------------------------------------
void
SettingsDialog::fillThemeCombo()
{
    themeCombo->clear();

    QDir themeDir (Auxil::getTilesDir());
    QStringList themes =
        themeDir.entryList(QDir::Dirs, QDir::Name).filter(QRegExp("^[^\\.]"));

    bool enableThemes = (themes.count() > 0);
    themeCbox->setEnabled(enableThemes);
    if (enableThemes) {
        themeCombo->addItems(themes);
    }
    else {
        themeLabel->setEnabled(false);
        themeCombo->setEnabled(false);
    }
}

//---------------------------------------------------------------------------
//  updateJudgeLogDirLabel
//
//! Update the Judge Log Directory label based on the latest setting for the
//! local user directory.
//---------------------------------------------------------------------------
void
SettingsDialog::updateJudgeLogDirLabel()
{
    judgeLogDirLabel->setText("Log directory: " +
        QDir::cleanPath(userDataDirLine->text()) + "/judge");
}
