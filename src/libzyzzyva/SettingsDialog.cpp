//---------------------------------------------------------------------------
// SettingsDialog.cpp
//
// The settings dialog for the word study application.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mthelen@gmail.com>.
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
#include "ZPushButton.h"
#include "Auxil.h"
#include "Defs.h"
#include <QColorDialog>
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

const QString GENERAL_PREFS_ITEM = "General";
const QString QUIZ_PREFS_ITEM = "Quiz";
const QString JUDGE_PREFS_ITEM = "Word Judge";
const QString FONT_PREFS_ITEM = "Fonts";
const QString WORD_LIST_PREFS_ITEM = "Word Tables";

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
SettingsDialog::SettingsDialog (QWidget* parent, Qt::WFlags f)
    : QDialog (parent, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this);
    Q_CHECK_PTR (mainVlay);

    QHBoxLayout* mainHlay = new QHBoxLayout;
    Q_CHECK_PTR (mainHlay);
    mainVlay->addLayout (mainHlay);

    navList = new QListWidget;
    Q_CHECK_PTR (navList);
    navList->setMaximumWidth (150);
    navList->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect (navList, SIGNAL (currentTextChanged (const QString&)),
             SLOT (navTextChanged (const QString&)));
    mainHlay->addWidget (navList);

    navStack = new QStackedWidget;
    Q_CHECK_PTR (navStack);
    mainHlay->addWidget (navStack);

    // General Prefs
    generalPrefWidget = new QWidget;
    Q_CHECK_PTR (generalPrefWidget);
    navStack->addWidget (generalPrefWidget);

    QVBoxLayout* generalPrefVlay = new QVBoxLayout (generalPrefWidget);
    Q_CHECK_PTR (generalPrefVlay);
    generalPrefVlay->setMargin (0);

    QLabel* generalPrefLabel = new QLabel (GENERAL_PREFS_ITEM);
    Q_CHECK_PTR (generalPrefLabel);
    generalPrefLabel->setFrameShape (QFrame::StyledPanel);
    generalPrefVlay->addWidget (generalPrefLabel);

    QGroupBox* autoImportGbox = new QGroupBox ("Lexicon");
    Q_CHECK_PTR (autoImportGbox);
    generalPrefVlay->addWidget (autoImportGbox);
    generalPrefVlay->setStretchFactor (autoImportGbox, 1);

    QVBoxLayout* autoImportVlay = new QVBoxLayout (autoImportGbox);
    Q_CHECK_PTR (autoImportVlay);
    autoImportVlay->setMargin (MARGIN);
    autoImportVlay->setSpacing (SPACING);

    QHBoxLayout* autoImportLexiconHlay = new QHBoxLayout;
    autoImportLexiconHlay->setSpacing (SPACING);
    Q_CHECK_PTR (autoImportLexiconHlay);
    autoImportVlay->addLayout (autoImportLexiconHlay);

    autoImportCbox = new QCheckBox ("Automatically load a lexicon:");
    Q_CHECK_PTR (autoImportCbox);
    connect (autoImportCbox, SIGNAL (toggled (bool)),
             SLOT (autoImportCboxToggled (bool)));
    autoImportLexiconHlay->addWidget (autoImportCbox);

    autoImportLexiconCombo = new QComboBox;
    Q_CHECK_PTR (autoImportLexiconCombo);
    autoImportLexiconCombo->addItem ("OWL+LWL");
    autoImportLexiconCombo->addItem ("OWL2+LWL");
    autoImportLexiconCombo->addItem ("SOWPODS");
    autoImportLexiconCombo->addItem ("ODS");
    autoImportLexiconCombo->addItem ("Custom");
    autoImportLexiconCombo->setCurrentIndex
        (autoImportLexiconCombo->findText ("OWL2+LWL"));
    connect (autoImportLexiconCombo, SIGNAL (activated (const QString&)),
             SLOT (autoImportLexiconActivated (const QString&)));
    autoImportLexiconHlay->addWidget (autoImportLexiconCombo);

    autoImportCustomWidget = new QWidget;
    Q_CHECK_PTR (autoImportCustomWidget);
    autoImportVlay->addWidget (autoImportCustomWidget);

    QHBoxLayout* autoImportCustomHlay = new QHBoxLayout
        (autoImportCustomWidget);
    Q_CHECK_PTR (autoImportCustomHlay);
    autoImportCustomHlay->setMargin (0);
    autoImportCustomHlay->setSpacing (SPACING);

    QLabel* autoImportCustomLabel = new QLabel ("Custom:");
    Q_CHECK_PTR (autoImportCustomLabel);
    autoImportCustomHlay->addWidget (autoImportCustomLabel);

    autoImportCustomLine = new QLineEdit;
    Q_CHECK_PTR (autoImportCustomLine);
    autoImportCustomHlay->addWidget (autoImportCustomLine);

    ZPushButton* autoImportBrowseButton = new ZPushButton ("Browse...");
    Q_CHECK_PTR (autoImportBrowseButton);
    connect (autoImportBrowseButton, SIGNAL (clicked()),
             SLOT (autoImportBrowseButtonClicked()));
    autoImportCustomHlay->addWidget (autoImportBrowseButton);

    QGroupBox* userDataDirGbox = new QGroupBox ("Data Directory");
    Q_CHECK_PTR (userDataDirGbox);
    generalPrefVlay->addWidget (userDataDirGbox);
    generalPrefVlay->setStretchFactor (userDataDirGbox, 1);

    QVBoxLayout* userDataDirVlay = new QVBoxLayout (userDataDirGbox);
    Q_CHECK_PTR (userDataDirVlay);
    userDataDirVlay->setMargin (MARGIN);
    userDataDirVlay->setSpacing (SPACING);

    QHBoxLayout* userDataDirHlay = new QHBoxLayout;
    Q_CHECK_PTR (userDataDirHlay);
    userDataDirHlay->setSpacing (SPACING);
    userDataDirVlay->addLayout (userDataDirHlay);

    QLabel* userDataDirLabel = new QLabel ("Location:");
    Q_CHECK_PTR (userDataDirLabel);
    userDataDirHlay->addWidget (userDataDirLabel);

    userDataDirLine = new QLineEdit;
    Q_CHECK_PTR (userDataDirLine);
    connect (userDataDirLine, SIGNAL (textChanged (const QString&)),
             SLOT (userDataDirChanged (const QString&)));
    userDataDirHlay->addWidget (userDataDirLine);

    ZPushButton* userDataDirBrowseButton = new ZPushButton ("Browse...");
    Q_CHECK_PTR (userDataDirBrowseButton);
    connect (userDataDirBrowseButton, SIGNAL (clicked()),
             SLOT (userDataDirBrowseButtonClicked()));
    userDataDirHlay->addWidget (userDataDirBrowseButton);

    userDataDirMoveCbox = new QCheckBox ("Move data after saving preferences");
    Q_CHECK_PTR (userDataDirMoveCbox);
    userDataDirMoveCbox->setEnabled (false);
    userDataDirVlay->addWidget (userDataDirMoveCbox);

    // FIXME: Come back to this when you figure out how to actually move a
    // directory!
    userDataDirMoveCbox->hide();

    QGroupBox* displayWelcomeGbox = new QGroupBox ("Welcome");
    Q_CHECK_PTR (displayWelcomeGbox);
    generalPrefVlay->addWidget (displayWelcomeGbox);
    generalPrefVlay->setStretchFactor (displayWelcomeGbox, 1);

    QVBoxLayout* displayWelcomeVlay = new QVBoxLayout (displayWelcomeGbox);
    Q_CHECK_PTR (displayWelcomeVlay);
    displayWelcomeVlay->setMargin (MARGIN);
    displayWelcomeVlay->setSpacing (SPACING);

    displayWelcomeCbox = new QCheckBox ("Display Welcome on startup");
    Q_CHECK_PTR (displayWelcomeCbox);
    displayWelcomeVlay->addWidget (displayWelcomeCbox);

    generalPrefVlay->addStretch (2);

    // Quiz Prefs
    quizPrefWidget = new QWidget;
    Q_CHECK_PTR (quizPrefWidget);
    navStack->addWidget (quizPrefWidget);

    QVBoxLayout* quizPrefVlay = new QVBoxLayout (quizPrefWidget);
    Q_CHECK_PTR (quizPrefVlay);
    quizPrefVlay->setMargin (0);

    QLabel* quizPrefLabel = new QLabel (QUIZ_PREFS_ITEM);
    Q_CHECK_PTR (quizPrefLabel);
    quizPrefLabel->setFrameShape (QFrame::StyledPanel);
    quizPrefVlay->addWidget (quizPrefLabel);

    QGroupBox* quizQuestionGbox = new QGroupBox ("Question Display");
    Q_CHECK_PTR (quizQuestionGbox);
    quizPrefVlay->addWidget (quizQuestionGbox);
    quizPrefVlay->setStretchFactor (quizQuestionGbox, 1);

    QVBoxLayout* quizQuestionVlay = new QVBoxLayout (quizQuestionGbox);
    Q_CHECK_PTR (quizQuestionVlay);
    quizQuestionVlay->setMargin (MARGIN);
    quizQuestionVlay->setSpacing (SPACING);

    themeCbox = new QCheckBox ("Use tile images");
    Q_CHECK_PTR (themeCbox);
    connect (themeCbox, SIGNAL (toggled (bool)),
             SLOT (themeCboxToggled (bool)));
    quizQuestionVlay->addWidget (themeCbox);

    QHBoxLayout* themeHlay = new QHBoxLayout;
    themeHlay->setSpacing (SPACING);
    Q_CHECK_PTR (themeHlay);
    quizQuestionVlay->addLayout (themeHlay);

    themeLabel = new QLabel ("Tile Theme:");
    Q_CHECK_PTR (themeLabel);
    themeLabel->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    themeHlay->addWidget (themeLabel);

    themeCombo = new QComboBox;
    Q_CHECK_PTR (themeCombo);
    themeHlay->addWidget (themeCombo);

    QHBoxLayout* letterOrderHlay = new QHBoxLayout;
    Q_CHECK_PTR (letterOrderHlay);
    quizQuestionVlay->addLayout (letterOrderHlay);

    QLabel* letterOrderLabel = new QLabel ("Letter Order:");
    Q_CHECK_PTR (letterOrderLabel);
    letterOrderLabel->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    letterOrderHlay->addWidget (letterOrderLabel);

    letterOrderCombo = new QComboBox;
    Q_CHECK_PTR (letterOrderCombo);
    letterOrderHlay->addWidget (letterOrderCombo);

    letterOrderCombo->addItem (Defs::QUIZ_LETTERS_ALPHA);
    letterOrderCombo->addItem (Defs::QUIZ_LETTERS_RANDOM);
    letterOrderCombo->addItem (Defs::QUIZ_LETTERS_VOWELS_FIRST);
    letterOrderCombo->addItem (Defs::QUIZ_LETTERS_CONSONANTS_FIRST); 
    letterOrderCombo->setCurrentIndex (0);

    QHBoxLayout* quizBackgroundColorHlay = new QHBoxLayout;
    Q_CHECK_PTR (quizBackgroundColorHlay);
    quizQuestionVlay->addLayout (quizBackgroundColorHlay);

    QLabel* quizBackgroundColorLabel = new QLabel ("Background Color:");
    Q_CHECK_PTR (quizBackgroundColorLabel);
    quizBackgroundColorHlay->addWidget (quizBackgroundColorLabel);

    quizBackgroundColorLine = new QLineEdit;
    Q_CHECK_PTR (quizBackgroundColorLine);
    quizBackgroundColorLine->setReadOnly (true);
    quizBackgroundColorLine->setEnabled (false);
    quizBackgroundColorHlay->addWidget (quizBackgroundColorLine);

    ZPushButton* quizBackgroundColorButton = new ZPushButton ("Choose...");
    Q_CHECK_PTR (quizBackgroundColorButton);
    connect (quizBackgroundColorButton, SIGNAL (clicked()),
             SLOT (chooseQuizBackgroundColorButtonClicked()));
    quizBackgroundColorHlay->addWidget (quizBackgroundColorButton);

    QGroupBox* quizBehaviorGbox = new QGroupBox ("Quiz Behavior");
    Q_CHECK_PTR (quizBehaviorGbox);
    quizPrefVlay->addWidget (quizBehaviorGbox);
    quizPrefVlay->setStretchFactor (quizBehaviorGbox, 1);

    QVBoxLayout* quizBehaviorVlay = new QVBoxLayout (quizBehaviorGbox);
    Q_CHECK_PTR (quizBehaviorVlay);
    quizBehaviorVlay->setMargin (MARGIN);
    quizBehaviorVlay->setSpacing (SPACING);

    quizUseFlashcardModeCbox = new QCheckBox ("Start in Flashcard Mode");
    Q_CHECK_PTR (quizUseFlashcardModeCbox);
    quizBehaviorVlay->addWidget (quizUseFlashcardModeCbox);

    quizShowNumResponsesCbox = new QCheckBox ("Show number of responses "
                                              "in status bar");
    Q_CHECK_PTR (quizShowNumResponsesCbox);
    quizBehaviorVlay->addWidget (quizShowNumResponsesCbox);

    quizAutoCheckCbox = new QCheckBox
        ("End question after all correct responses");
    Q_CHECK_PTR (quizAutoCheckCbox);
    connect (quizAutoCheckCbox, SIGNAL (toggled (bool)),
             SLOT (autoCheckCboxToggled (bool)));
    quizBehaviorVlay->addWidget (quizAutoCheckCbox);

    quizAutoAdvanceCbox = new QCheckBox
        ("Go to next question after all correct responses");
    Q_CHECK_PTR (quizAutoAdvanceCbox);
    quizBehaviorVlay->addWidget (quizAutoAdvanceCbox);

    quizAutoEndAfterIncorrectCbox = new QCheckBox
        ("End question after an incorrect response");
    Q_CHECK_PTR (quizAutoEndAfterIncorrectCbox);
    quizBehaviorVlay->addWidget (quizAutoEndAfterIncorrectCbox);

    quizCycleAnswersCbox = new QCheckBox
        ("Cycle answers after ending a question");
    Q_CHECK_PTR (quizCycleAnswersCbox);
    quizBehaviorVlay->addWidget (quizCycleAnswersCbox);

    quizPrefVlay->addStretch (2);

    // Word Judge Prefs
    judgePrefWidget = new QWidget;
    Q_CHECK_PTR (judgePrefWidget);
    navStack->addWidget (judgePrefWidget);

    QVBoxLayout* judgePrefVlay = new QVBoxLayout (judgePrefWidget);
    Q_CHECK_PTR (judgePrefVlay);
    judgePrefVlay->setMargin (0);

    QLabel* judgePrefLabel = new QLabel (JUDGE_PREFS_ITEM);
    Q_CHECK_PTR (judgePrefLabel);
    judgePrefLabel->setFrameShape (QFrame::StyledPanel);
    judgePrefVlay->addWidget (judgePrefLabel);

    QGroupBox* judgeGbox = new QGroupBox;
    Q_CHECK_PTR (judgeGbox);
    judgePrefVlay->addWidget (judgeGbox);
    judgePrefVlay->setStretchFactor (judgeGbox, 1);

    QVBoxLayout* judgeVlay = new QVBoxLayout (judgeGbox);
    Q_CHECK_PTR (judgeVlay);
    judgeVlay->setMargin (MARGIN);
    judgeVlay->setSpacing (SPACING);

    judgeSaveLogCbox = new QCheckBox ("Save judge results to log files");
    Q_CHECK_PTR (judgeSaveLogCbox);
    connect (judgeSaveLogCbox, SIGNAL (toggled (bool)),
             SLOT (judgeSaveLogCboxToggled (bool)));
    judgeVlay->addWidget (judgeSaveLogCbox);

    judgeLogDirWidget = new QWidget;
    Q_CHECK_PTR (judgeLogDirWidget);
    judgeVlay->addWidget (judgeLogDirWidget);

    QHBoxLayout* judgeLogDirHlay = new QHBoxLayout (judgeLogDirWidget);
    Q_CHECK_PTR (judgeLogDirHlay);
    judgeLogDirHlay->setMargin (0);
    judgeLogDirHlay->setSpacing (SPACING);

    QLabel* judgeLogDirLabel = new QLabel ("Log directory:");
    Q_CHECK_PTR (judgeLogDirLabel);
    judgeLogDirHlay->addWidget (judgeLogDirLabel);

    judgeLogDirLine = new QLineEdit;
    Q_CHECK_PTR (judgeLogDirLine);
    judgeLogDirHlay->addWidget (judgeLogDirLine);

    ZPushButton* judgeLogBrowseButton = new ZPushButton ("Browse...");
    Q_CHECK_PTR (judgeLogBrowseButton);
    connect (judgeLogBrowseButton, SIGNAL (clicked()),
             SLOT (judgeLogBrowseButtonClicked()));
    judgeLogDirHlay->addWidget (judgeLogBrowseButton);

    judgePrefVlay->addStretch (2);

    // Signal mapper for the Choose Font buttons
    QSignalMapper* signalMapper = new QSignalMapper (this);
    Q_CHECK_PTR (signalMapper);
    connect (signalMapper, SIGNAL (mapped (int)),
             SLOT (chooseFontButtonClicked (int)));

    // Font Prefs
    fontPrefWidget = new QWidget;
    Q_CHECK_PTR (fontPrefWidget);
    navStack->addWidget (fontPrefWidget);

    QVBoxLayout* fontPrefVlay = new QVBoxLayout (fontPrefWidget);
    Q_CHECK_PTR (fontPrefVlay);
    fontPrefVlay->setMargin (0);

    QLabel* fontPrefLabel = new QLabel (FONT_PREFS_ITEM);
    Q_CHECK_PTR (fontPrefLabel);
    fontPrefLabel->setFrameShape (QFrame::StyledPanel);
    fontPrefVlay->addWidget (fontPrefLabel);

    QGroupBox* fontGbox = new QGroupBox;
    Q_CHECK_PTR (fontGbox);
    fontPrefVlay->addWidget (fontGbox);
    fontPrefVlay->setStretchFactor (fontGbox, 1);

    QGridLayout* fontGlay = new QGridLayout (fontGbox);
    fontGlay->setMargin (MARGIN);
    fontGlay->setSpacing (SPACING);
    Q_CHECK_PTR (fontGlay);

    // Main font
    int row = 0;
    QLabel* fontMainLabel = new QLabel ("Main:");
    Q_CHECK_PTR (fontMainLabel);
    fontGlay->addWidget (fontMainLabel, row, 0, Qt::AlignLeft);

    fontMainLine = new QLineEdit;
    Q_CHECK_PTR (fontMainLine);
    fontMainLine->setReadOnly (true);
    fontMainLine->setText (this->font().toString());
    fontMainLine->home (false);
    fontGlay->addWidget (fontMainLine, row, 1);

    ZPushButton* chooseFontMainButton = new ZPushButton ("Choose...");
    Q_CHECK_PTR (chooseFontMainButton);
    connect (chooseFontMainButton, SIGNAL (clicked()), signalMapper,
             SLOT (map()));
    signalMapper->setMapping (chooseFontMainButton, FONT_MAIN_BUTTON);
    fontGlay->addWidget (chooseFontMainButton, row, 2);

    // Word list font
    ++row;
    QLabel* fontWordListLabel = new QLabel ("Word Tables:");
    Q_CHECK_PTR (fontWordListLabel);
    fontGlay->addWidget (fontWordListLabel, row, 0, Qt::AlignLeft);

    fontWordListLine = new QLineEdit;
    Q_CHECK_PTR (fontWordListLine);
    fontWordListLine->setReadOnly (true);
    fontWordListLine->setText (this->font().toString());
    fontWordListLine->home (false);
    fontGlay->addWidget (fontWordListLine, row, 1);

    ZPushButton* chooseFontWordListButton = new ZPushButton ("Choose...");
    Q_CHECK_PTR (chooseFontWordListButton);
    connect (chooseFontWordListButton, SIGNAL (clicked()), signalMapper,
             SLOT (map()));
    signalMapper->setMapping (chooseFontWordListButton,
                              FONT_WORD_LISTS_BUTTON);
    fontGlay->addWidget (chooseFontWordListButton, row, 2);

    // Quiz label font
    ++row;
    QLabel* fontQuizLabelLabel = new QLabel ("Quiz Questions:");
    Q_CHECK_PTR (fontQuizLabelLabel);
    fontGlay->addWidget (fontQuizLabelLabel, row, 0, Qt::AlignLeft);

    fontQuizLabelLine = new QLineEdit;
    Q_CHECK_PTR (fontQuizLabelLine);
    fontQuizLabelLine->setReadOnly (true);
    fontQuizLabelLine->setText (this->font().toString());
    fontQuizLabelLine->home (false);
    fontGlay->addWidget (fontQuizLabelLine, row, 1);

    ZPushButton* chooseFontQuizLabelButton = new ZPushButton ("Choose...");
    Q_CHECK_PTR (chooseFontQuizLabelButton);
    connect (chooseFontQuizLabelButton, SIGNAL (clicked()), signalMapper,
             SLOT (map()));
    signalMapper->setMapping (chooseFontQuizLabelButton,
                              FONT_QUIZ_LABEL_BUTTON);
    fontGlay->addWidget (chooseFontQuizLabelButton, row, 2);

    // Input font
    ++row;
    QLabel* fontInputLabel = new QLabel ("Word Input:");
    Q_CHECK_PTR (fontInputLabel);
    fontGlay->addWidget (fontInputLabel, row, 0, Qt::AlignLeft);

    fontWordInputLine = new QLineEdit;
    Q_CHECK_PTR (fontWordInputLine);
    fontWordInputLine->setReadOnly (true);
    fontWordInputLine->setText (this->font().toString());
    fontWordInputLine->home (false);
    fontGlay->addWidget (fontWordInputLine, row, 1);

    ZPushButton* chooseFontInputButton = new ZPushButton ("Choose...");
    Q_CHECK_PTR (chooseFontInputButton);
    connect (chooseFontInputButton, SIGNAL (clicked()), signalMapper,
             SLOT (map()));
    signalMapper->setMapping (chooseFontInputButton, FONT_WORD_INPUT_BUTTON);
    fontGlay->addWidget (chooseFontInputButton, row, 2);

    // Definition font
    ++row;
    QLabel* fontDefinitionLabel = new QLabel ("Definitions:");
    Q_CHECK_PTR (fontDefinitionLabel);
    fontGlay->addWidget (fontDefinitionLabel, row, 0, Qt::AlignLeft);

    fontDefinitionLine = new QLineEdit;
    Q_CHECK_PTR (fontDefinitionLine);
    fontDefinitionLine->setReadOnly (true);
    fontDefinitionLine->setText (this->font().toString());
    fontDefinitionLine->home (false);
    fontGlay->addWidget (fontDefinitionLine, row, 1);

    ZPushButton* chooseFontDefinitionButton = new ZPushButton ("Choose...");
    Q_CHECK_PTR (chooseFontDefinitionButton);
    connect (chooseFontDefinitionButton, SIGNAL (clicked()), signalMapper,
             SLOT (map()));
    signalMapper->setMapping (chooseFontDefinitionButton,
                              FONT_DEFINITIONS_BUTTON);
    fontGlay->addWidget (chooseFontDefinitionButton, row, 2);

    fontPrefVlay->addStretch (2);

    // Word List Prefs
    wordListPrefWidget = new QWidget;
    Q_CHECK_PTR (wordListPrefWidget);
    navStack->addWidget (wordListPrefWidget);

    QVBoxLayout* wordListPrefVlay = new QVBoxLayout (wordListPrefWidget);
    Q_CHECK_PTR (wordListPrefVlay);
    wordListPrefVlay->setMargin (0);

    QLabel* wordListPrefLabel = new QLabel (WORD_LIST_PREFS_ITEM);
    Q_CHECK_PTR (wordListPrefLabel);
    wordListPrefLabel->setFrameShape (QFrame::StyledPanel);
    wordListPrefVlay->addWidget (wordListPrefLabel);

    QGroupBox* wordListDisplayGbox = new QGroupBox ("Display Columns");
    Q_CHECK_PTR (wordListDisplayGbox);
    wordListPrefVlay->addWidget (wordListDisplayGbox);
    wordListPrefVlay->setStretchFactor (wordListDisplayGbox, 1);

    QVBoxLayout* wordListDisplayVlay = new QVBoxLayout (wordListDisplayGbox);
    Q_CHECK_PTR (wordListDisplayVlay);
    wordListDisplayVlay->setMargin (MARGIN);
    wordListDisplayVlay->setSpacing (SPACING);

    showProbabilityOrderCbox = new QCheckBox ("Show probability order");
    Q_CHECK_PTR (showProbabilityOrderCbox);
    wordListDisplayVlay->addWidget (showProbabilityOrderCbox);

    showHooksCbox = new QCheckBox ("Show hooks");
    Q_CHECK_PTR (showHooksCbox);
    wordListDisplayVlay->addWidget (showHooksCbox);

    showHookParentsCbox = new QCheckBox ("Show inner hooks");
    Q_CHECK_PTR (showHookParentsCbox);
    wordListDisplayVlay->addWidget (showHookParentsCbox);

    showDefinitionCbox = new QCheckBox ("Show definitions");
    Q_CHECK_PTR (showDefinitionCbox);
    wordListDisplayVlay->addWidget (showDefinitionCbox);

    QGroupBox* wordListSortGbox = new QGroupBox ("Sorting and Grouping");
    Q_CHECK_PTR (wordListSortGbox);
    wordListPrefVlay->addWidget (wordListSortGbox);
    wordListPrefVlay->setStretchFactor (wordListSortGbox, 1);

    QVBoxLayout* wordListSortVlay = new QVBoxLayout (wordListSortGbox);
    Q_CHECK_PTR (wordListSortVlay);
    wordListSortVlay->setMargin (MARGIN);
    wordListSortVlay->setSpacing (SPACING);

    lengthSortCbox = new QCheckBox ("Sort by word length");
    Q_CHECK_PTR (lengthSortCbox);
    wordListSortVlay->addWidget (lengthSortCbox);

    anagramGroupCbox = new QCheckBox ("Group by anagrams");
    Q_CHECK_PTR (anagramGroupCbox);
    wordListSortVlay->addWidget (anagramGroupCbox);

    wordListPrefVlay->addStretch (2);

    // Button layout
    QHBoxLayout* buttonHlay = new QHBoxLayout;
    buttonHlay->setSpacing (SPACING);
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    ZPushButton* okButton = new ZPushButton ("OK");
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

    // Add nav list items
    new QListWidgetItem (GENERAL_PREFS_ITEM, navList);
    new QListWidgetItem (QUIZ_PREFS_ITEM, navList);
    new QListWidgetItem (JUDGE_PREFS_ITEM, navList);
    new QListWidgetItem (FONT_PREFS_ITEM, navList);
    new QListWidgetItem (WORD_LIST_PREFS_ITEM, navList);
    navList->setCurrentRow (0);

    setWindowTitle (DIALOG_CAPTION);
    readSettings();

    quizBackgroundColor = MainSettings::getQuizBackgroundColor();
    QPalette colorPalette (quizBackgroundColor);
    quizBackgroundColorLine->setPalette (colorPalette);
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

    bool autoImport = MainSettings::getUseAutoImport();
    autoImportCbox->setChecked (autoImport);
    autoImportCboxToggled (autoImport);

    QString autoImportLexicon = MainSettings::getAutoImportLexicon();
    int index = autoImportLexiconCombo->findText (autoImportLexicon);
    autoImportLexiconCombo->setCurrentIndex (index >= 0 ? index : 0);
    autoImportLexiconActivated (autoImportLexiconCombo->currentText());

    QString autoImportFile = MainSettings::getAutoImportFile();
    autoImportCustomLine->setText (autoImportFile);

    origUserDataDir = MainSettings::getUserDataDir();
    userDataDirLine->setText (origUserDataDir);

    displayWelcomeCbox->setChecked (MainSettings::getDisplayWelcome());

    fillThemeCombo();
    bool useTileTheme = MainSettings::getUseTileTheme();
    themeCbox->setChecked (useTileTheme);
    if (themeCbox->isEnabled()) {
        QString tileTheme = MainSettings::getTileTheme();
        QString themeStr;
        for (int i = 0; i < themeCombo->count(); ++i) {
            themeStr = themeCombo->itemText (i);
            if (themeStr == tileTheme) {
                themeCombo->setCurrentIndex (i);
                break;
            }
        }
        themeCboxToggled (useTileTheme);
    }

    // Quiz letter order
    int letterOrderIndex = letterOrderCombo->findText
        (MainSettings::getQuizLetterOrder());
    if (letterOrderIndex < 0) {
        letterOrderIndex = letterOrderCombo->findText
            (Defs::QUIZ_LETTERS_ALPHA);
    }
    if (letterOrderIndex < 0)
        letterOrderIndex = 0;
    letterOrderCombo->setCurrentIndex (letterOrderIndex);

    quizBackgroundColor = MainSettings::getQuizBackgroundColor();
    QPalette quizBackgroundPalette (quizBackgroundColor);
    quizBackgroundColorLine->setPalette (quizBackgroundPalette);

    quizUseFlashcardModeCbox->setChecked
        (MainSettings::getQuizUseFlashcardMode());
    quizShowNumResponsesCbox->setChecked
        (MainSettings::getQuizShowNumResponses());
    bool autoCheck = MainSettings::getQuizAutoCheck();
    quizAutoCheckCbox->setChecked (autoCheck);
    quizAutoAdvanceCbox->setChecked (MainSettings::getQuizAutoAdvance());
    quizAutoEndAfterIncorrectCbox->setChecked
        (MainSettings::getQuizAutoEndAfterIncorrect());
    quizCycleAnswersCbox->setChecked (MainSettings::getQuizCycleAnswers());
    autoCheckCboxToggled (autoCheck);

    judgeSaveLogCbox->setChecked (MainSettings::getJudgeSaveLog());
    judgeLogDirLine->setText (MainSettings::getJudgeLogDir());

    // Main font
    fontMainLine->setText (MainSettings::getMainFont());
    fontMainLine->home (false);

    // Word list font
    fontWordListLine->setText (MainSettings::getWordListFont());
    fontWordListLine->home (false);

    // Quiz label font
    fontQuizLabelLine->setText (MainSettings::getQuizLabelFont());
    fontQuizLabelLine->home (false);

    // Word input font
    fontWordInputLine->setText (MainSettings::getWordInputFont());
    fontWordInputLine->home (false);

    // Definition font
    fontDefinitionLine->setText (MainSettings::getDefinitionFont());
    fontDefinitionLine->home (false);

    lengthSortCbox->setChecked (MainSettings::getWordListSortByLength());
    anagramGroupCbox->setChecked
        (MainSettings::getWordListGroupByAnagrams());
    showProbabilityOrderCbox->setChecked
        (MainSettings::getWordListShowProbabilityOrder());
    showHooksCbox->setChecked (MainSettings::getWordListShowHooks());
    showHookParentsCbox->setChecked
        (MainSettings::getWordListShowHookParents());
    showDefinitionCbox->setChecked
        (MainSettings::getWordListShowDefinitions());
}

//---------------------------------------------------------------------------
//  writeSettings
//
//! Write settings.
//---------------------------------------------------------------------------
void
SettingsDialog::writeSettings()
{
    MainSettings::setUseAutoImport (autoImportCbox->isChecked());
    MainSettings::setAutoImportLexicon
        (autoImportLexiconCombo->currentText());
    MainSettings::setAutoImportFile (autoImportCustomLine->text());
    MainSettings::setDisplayWelcome (displayWelcomeCbox->isChecked());
    MainSettings::setUserDataDir (userDataDirLine->text());
    MainSettings::setUseTileTheme (themeCbox->isChecked());
    MainSettings::setTileTheme (themeCombo->currentText());
    MainSettings::setQuizLetterOrder (letterOrderCombo->currentText());
    MainSettings::setQuizBackgroundColor (quizBackgroundColor);
    MainSettings::setQuizUseFlashcardMode
        (quizUseFlashcardModeCbox->isChecked());
    MainSettings::setQuizShowNumResponses
        (quizShowNumResponsesCbox->isChecked());
    MainSettings::setQuizAutoCheck (quizAutoCheckCbox->isChecked());
    MainSettings::setQuizAutoAdvance (quizAutoAdvanceCbox->isChecked());
    MainSettings::setQuizAutoEndAfterIncorrect
        (quizAutoEndAfterIncorrectCbox->isChecked());
    MainSettings::setQuizCycleAnswers (quizCycleAnswersCbox->isChecked());
    MainSettings::setJudgeSaveLog (judgeSaveLogCbox->isChecked());
    MainSettings::setJudgeLogDir (judgeLogDirLine->text());
    MainSettings::setMainFont (fontMainLine->text());
    MainSettings::setWordListFont (fontWordListLine->text());
    MainSettings::setQuizLabelFont (fontQuizLabelLine->text());
    MainSettings::setWordInputFont (fontWordInputLine->text());
    MainSettings::setDefinitionFont (fontDefinitionLine->text());
    MainSettings::setWordListSortByLength (lengthSortCbox->isChecked());
    MainSettings::setWordListGroupByAnagrams
        (anagramGroupCbox->isChecked());
    MainSettings::setWordListShowProbabilityOrder
        (showProbabilityOrderCbox->isChecked());
    MainSettings::setWordListShowHooks (showHooksCbox->isChecked());
    MainSettings::setWordListShowHookParents
        (showHookParentsCbox->isChecked());
    MainSettings::setWordListShowDefinitions
        (showDefinitionCbox->isChecked());
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
SettingsDialog::navTextChanged (const QString& text)
{
    if (text == GENERAL_PREFS_ITEM)
        navStack->setCurrentWidget (generalPrefWidget);
    else if (text == QUIZ_PREFS_ITEM)
        navStack->setCurrentWidget (quizPrefWidget);
    else if (text == JUDGE_PREFS_ITEM)
        navStack->setCurrentWidget (judgePrefWidget);
    else if (text == FONT_PREFS_ITEM)
        navStack->setCurrentWidget (fontPrefWidget);
    else if (text == WORD_LIST_PREFS_ITEM)
        navStack->setCurrentWidget (wordListPrefWidget);
}

//---------------------------------------------------------------------------
//  autoImportBrowseButtonClicked
//
//! Slot called when the Auto Import Browse button is clicked.  Create a file
//! chooser dialog and place the name of the chosen file in the auto-import
//! line edit.
//---------------------------------------------------------------------------
void
SettingsDialog::autoImportBrowseButtonClicked()
{
    QString file = QFileDialog::getOpenFileName (this, IMPORT_CHOOSER_TITLE,
        Auxil::getWordsDir(), "All Files (*.*)");

    if (!file.isNull())
        autoImportCustomLine->setText (file);
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
SettingsDialog::autoImportCboxToggled (bool on)
{
    autoImportLexiconCombo->setEnabled (on);
    if (on)
        autoImportLexiconActivated (autoImportLexiconCombo->currentText());
    else
        autoImportCustomWidget->setEnabled (false);
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
SettingsDialog::autoImportLexiconActivated (const QString& text)
{
    bool customActive = (text == "Custom");
    autoImportCustomWidget->setEnabled (customActive);
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
    QString dir = QFileDialog::getExistingDirectory
        (this, USER_DATA_DIR_CHOOSER_TITLE, Auxil::getUserDir());

    if (!dir.isNull())
        userDataDirLine->setText (dir);
}

//---------------------------------------------------------------------------
//  userDataDirChanged
//
//! Slot called when the User Data Directory preference is changed.  Enable
//! the checkbox that determines whether the data directory should be moved
//! immediately.
//---------------------------------------------------------------------------
void
SettingsDialog::userDataDirChanged (const QString& text)
{
    if (text == origUserDataDir) {
        userDataDirMoveCbox->setCheckState (Qt::Unchecked);
        userDataDirMoveCbox->setEnabled (false);
    }

    else if (!userDataDirMoveCbox->isEnabled()) {
        userDataDirMoveCbox->setEnabled (true);
        userDataDirMoveCbox->setCheckState (Qt::Checked);
    }
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
SettingsDialog::autoCheckCboxToggled (bool on)
{
    quizAutoAdvanceCbox->setEnabled (on);
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
SettingsDialog::themeCboxToggled (bool on)
{
    themeLabel->setEnabled (on);
    themeCombo->setEnabled (on);
}

//---------------------------------------------------------------------------
//  judgeLogBrowseButtonClicked
//
//! Slot called when the Judge Log Browse button is clicked.  Create a
//! directory chooser dialog and place the name of the chosen directory in the
//! judge log line edit.
//---------------------------------------------------------------------------
void
SettingsDialog::judgeLogBrowseButtonClicked()
{
    QString dir = QFileDialog::getExistingDirectory (this,
                                                     JUDGE_LOG_CHOOSER_TITLE,
                                                     Auxil::getRootDir() +
                                                     "/data/judge");
    if (!dir.isEmpty()) {
        dir = dir.replace (QRegExp ("/+$"), "");
        judgeLogDirLine->setText (dir);
    }
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
SettingsDialog::judgeSaveLogCboxToggled (bool on)
{
    judgeLogDirWidget->setEnabled (on);
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
SettingsDialog::chooseFontButtonClicked (int button)
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
    currentFont.fromString (lineEdit->text());
    QFont newFont = QFontDialog::getFont (&ok, currentFont, this);
    if (ok) {
        lineEdit->setText (newFont.toString());
        lineEdit->home (false);
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
    QColor color = QColorDialog::getColor
        (MainSettings::getQuizBackgroundColor());
    if (color.isValid()) {
        QPalette palette (color);
        quizBackgroundColorLine->setPalette (palette);
        quizBackgroundColor = color;
    }
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
    QStringList themes = themeDir.entryList (QDir::Dirs, QDir::Name).filter
        (QRegExp ("^[^\\.]"));

    bool enableThemes = (themes.count() > 0);
    themeCbox->setEnabled (enableThemes);
    if (enableThemes) {
        themeCombo->addItems (themes);
    }
    else {
        themeLabel->setEnabled (false);
        themeCombo->setEnabled (false);
    }
}
