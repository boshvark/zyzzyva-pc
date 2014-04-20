//---------------------------------------------------------------------------
// MainSettings.cpp
//
// The main settings for the word study application.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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

#include "MainSettings.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QRect>
#include <QSettings>

MainSettings* MainSettings::instance = new MainSettings();

const QString MainSettings::GENERAL_PREFS_GROUP = "General";
const QString MainSettings::SEARCH_PREFS_GROUP = "Search";
const QString MainSettings::QUIZ_PREFS_GROUP = "Quiz";
const QString MainSettings::PROBABILITY_PREFS_GROUP = "Probability";
const QString MainSettings::CARDBOX_PREFS_GROUP = "Cardbox";
const QString MainSettings::JUDGE_PREFS_GROUP = "Word Judge";
const QString MainSettings::FONT_PREFS_GROUP = "Fonts";
const QString MainSettings::WORD_LIST_PREFS_GROUP = "Word Tables";

const QString SETTINGS_MAIN = "Zyzzyva";
const QString SETTINGS_PROGRAM_VERSION = "program_version";
const QString SETTINGS_MAIN_WINDOW_POS = "main_window_pos";
const QString SETTINGS_MAIN_WINDOW_SIZE = "main_window_size";
const QString SETTINGS_IMPORT = "autoimport";
const QString SETTINGS_IMPORT_LEXICON = "autoimport_lexicon";
const QString SETTINGS_IMPORT_LEXICONS = "autoimport_lexicons";
const QString SETTINGS_DEFAULT_LEXICON = "default_lexicon";
const QString SETTINGS_IMPORT_FILE = "autoimport_file";
const QString SETTINGS_DISPLAY_WELCOME = "display_welcome";
const QString SETTINGS_USER_DATA_DIR = "user_data_dir";
const QString SETTINGS_FONT_MAIN = "font";
const QString SETTINGS_FONT_WORD_LISTS = "font_word_lists";
const QString SETTINGS_FONT_QUIZ_LABEL = "font_quiz_label";
const QString SETTINGS_FONT_DEFINITIONS = "font_definitions";
const QString SETTINGS_FONT_WORD_INPUT = "font_word_input";
const QString SETTINGS_SORT_BY_LENGTH = "wordlist_sort_by_length";
const QString SETTINGS_GROUP_BY_ALPHAGRAMS = "wordlist_group_by_alphagrams";
const QString SETTINGS_SHOW_PROBABILITY_ORDER
    = "wordlist_show_probability_order";
const QString SETTINGS_SHOW_PLAYABILITY_ORDER
    = "wordlist_show_playability_order";
const QString SETTINGS_SHOW_HOOKS = "wordlist_show_hooks";
const QString SETTINGS_SHOW_HOOK_PARENTS = "wordlist_show_hook_parents";
const QString SETTINGS_USE_HOOK_PARENT_HYPHENS
    = "wordlist_use_hook_parent_hyphens";
const QString SETTINGS_SHOW_DEFINITIONS = "wordlist_show_definitions";
const QString SETTINGS_LOWER_CASE_WILDCARDS = "wordlist_lower_case_wildcards";
const QString SETTINGS_USE_LEXICON_STYLES = "wordlist_use_lexicon_styles";
const QString SETTINGS_LEXICON_STYLES = "wordlist_lexicon_styles";
const QString SETTINGS_USE_TILE_THEME = "use_tile_theme";
const QString SETTINGS_TILE_THEME = "tile_theme";
const QString SETTINGS_SEARCH_SELECT_INPUT = "search_select_input";
const QString SETTINGS_QUIZ_LETTER_ORDER = "quiz_letter_order";
const QString SETTINGS_QUIZ_BACKGROUND_COLOR = "quiz_background_color";
const QString SETTINGS_QUIZ_USE_FLASHCARD_MODE = "quiz_use_flashcard_mode";
const QString SETTINGS_QUIZ_SHOW_NUM_RESPONSES = "quiz_show_num_responses";
const QString SETTINGS_QUIZ_SHOW_QUESTION_STATS = "quiz_show_question_stat";
const QString SETTINGS_QUIZ_REQUIRE_LEXICON_SYMBOLS
    = "quiz_require_lexicon_symbols";
const QString SETTINGS_QUIZ_AUTO_CHECK = "quiz_auto_check";
const QString SETTINGS_QUIZ_AUTO_ADVANCE = "quiz_auto_advance";
const QString SETTINGS_QUIZ_AUTO_END_AFTER_INCORRECT
    = "quiz_auto_end_after_incorrect";
const QString SETTINGS_QUIZ_MARK_MISSED_AFTER_INCORRECT
    = "quiz_mark_missed_after_incorrect";
const QString SETTINGS_QUIZ_MARK_MISSED_AFTER_TIMER_EXPIRES
    = "quiz_mark_missed_after_timer_expires";
const QString SETTINGS_QUIZ_CYCLE_ANSWERS = "quiz_cycle_answers";
const QString SETTINGS_QUIZ_TIMEOUT_DISABLE_INPUT = "quiz_timeout_disable_input";
const QString SETTINGS_QUIZ_TIMEOUT_DISABLE_INPUT_MSECS
    = "quiz_timeout_disable_input_msecs";
const QString SETTINGS_QUIZ_RECORD_STATS = "quiz_record_stats";
const QString SETTINGS_PROBABILITY_NUM_BLANKS = "probability_num_blanks";
const QString SETTINGS_CARDBOX_SCHEDULES = "cardbox_schedules";
const QString SETTINGS_CARDBOX_WINDOWS = "cardbox_windows";
const QString SETTINGS_LETTER_DISTRIBUTION = "letter_distribution";
const QString SETTINGS_JUDGE_SAVE_LOG = "judge_save_log";

const bool    DEFAULT_AUTO_IMPORT = true;
const QString DEFAULT_DEFAULT_LEXICON = Defs::LEXICON_OWL2;
const bool    DEFAULT_DISPLAY_WELCOME = true;
const QString DEFAULT_USER_DATA_DIR = Auxil::getHomeDir() + "/Zyzzyva";
const bool    DEFAULT_USE_TILE_THEME = true;
const QString DEFAULT_TILE_THEME = "tan-with-border";
const bool    DEFAULT_SEARCH_SELECT_INPUT = true;
const QString DEFAULT_QUIZ_LETTER_ORDER = Defs::QUIZ_LETTERS_ALPHA;
const QRgb    DEFAULT_QUIZ_BACKGROUND_COLOR = qRgb(0, 0, 127);
const bool    DEFAULT_QUIZ_USE_FLASHCARD_MODE = false;
const bool    DEFAULT_QUIZ_SHOW_NUM_RESPONSES = true;
const bool    DEFAULT_QUIZ_SHOW_QUESTION_STATS = true;
const bool    DEFAULT_QUIZ_REQUIRE_LEXICON_SYMBOLS = false;
const bool    DEFAULT_QUIZ_AUTO_CHECK = true;
const bool    DEFAULT_QUIZ_AUTO_ADVANCE = false;
const bool    DEFAULT_QUIZ_AUTO_END_AFTER_INCORRECT = false;
const bool    DEFAULT_QUIZ_MARK_MISSED_AFTER_INCORRECT = true;
const bool    DEFAULT_QUIZ_MARK_MISSED_AFTER_TIMER_EXPIRES = false;
const bool    DEFAULT_QUIZ_CYCLE_ANSWERS = true;
const bool    DEFAULT_QUIZ_TIMEOUT_DISABLE_INPUT = true;
const int     DEFAULT_QUIZ_TIMEOUT_DISABLE_INPUT_MSECS = 750;
const bool    DEFAULT_QUIZ_RECORD_STATS = true;
const int     DEFAULT_PROBABILITY_NUM_BLANKS = 2;
const QString DEFAULT_CARDBOX_SCHEDULES = "1 4 7 12 20 30 60 90 150 270 480";
const QString DEFAULT_CARDBOX_WINDOWS = "0 1 2 3 5 7 10 15 20 30 50";
const bool    DEFAULT_JUDGE_SAVE_LOG = true;
const bool    DEFAULT_SORT_BY_LENGTH = false;
const bool    DEFAULT_GROUP_BY_ANAGRAMS = true;
const bool    DEFAULT_SHOW_PROBABILITY_ORDER = true;
const bool    DEFAULT_SHOW_PLAYABILITY_ORDER = false;
const bool    DEFAULT_SHOW_HOOKS = true;
const bool    DEFAULT_SHOW_HOOK_PARENTS = true;
const bool    DEFAULT_USE_HOOK_PARENT_HYPHENS = false;
const bool    DEFAULT_SHOW_DEFINITIONS = true;
const bool    DEFAULT_LOWER_CASE_WILDCARDS = false;
const bool    DEFAULT_USE_LEXICON_STYLES = true;
const QString DEFAULT_LEXICON_STYLES = QString(
    "%1 and not %2: symbol +\n"
    "%3 and not %4: symbol !\n"
    "%5 and not %6: symbol +\n"
    "%7 and not %8: symbol !\n"
    "%9 and not %10: symbol #\n"
    "%11 and not %12: symbol #\n"
    "%13 and not %14: symbol +\n"
    "%15 and not %16: symbol ^\n"
    "%17 and not %18: symbol +\n"
    "%19 and not %20: symbol +")
    .arg(Defs::LEXICON_OWL2).arg(Defs::LEXICON_OWL)
    .arg(Defs::LEXICON_OWL2).arg(Defs::LEXICON_OSPD4)
    .arg(Defs::LEXICON_OWL2_1).arg(Defs::LEXICON_OWL)
    .arg(Defs::LEXICON_OWL2_1).arg(Defs::LEXICON_OSPD4_1)
    .arg(Defs::LEXICON_CSW07).arg(Defs::LEXICON_OWL2)
    .arg(Defs::LEXICON_CSW12).arg(Defs::LEXICON_OWL2)
    .arg(Defs::LEXICON_CSW12).arg(Defs::LEXICON_CSW07)
    .arg(Defs::LEXICON_CSW07).arg(Defs::LEXICON_CSW12)
    .arg(Defs::LEXICON_CSW07).arg(Defs::LEXICON_OSWI)
    .arg(Defs::LEXICON_ODS5).arg(Defs::LEXICON_ODS4);
const QString DEFAULT_LETTER_DISTRIBUTION = "A:9 B:2 C:2 D:4 E:12 F:2 G:3 "
    "H:2 I:9 J:1 K:1 L:4 M:2 N:6 O:8 P:2 Q:1 R:6 S:4 T:6 U:4 V:2 W:2 X:1 "
    "Y:2 Z:1 _:2";

//---------------------------------------------------------------------------
//  readSettings
//
//! Read settings.
//---------------------------------------------------------------------------
void
MainSettings::readSettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_MAIN);

    QPoint defaultPos (50, 50);
    QSize defaultSize (640, 480);

    // Get desktop geometry and validate position, size settings to make sure
    // the window will appear on-screen
    // XXX: This may have problems with virtual desktops, need to test
    QRect srect = qApp->desktop()->availableGeometry();
    QPoint pos = settings.value(SETTINGS_MAIN_WINDOW_POS, defaultPos).toPoint();
    QSize size = settings.value(SETTINGS_MAIN_WINDOW_SIZE, defaultSize).toSize();

    // Validate and correct window position
    if ((pos.x() < 0) || (pos.y() < 0) ||
        (pos.x() > srect.width()) || (pos.y() > srect.height()))
    {
        pos = defaultPos;
    }

    // Validate and correct window size
    if ((size.width() < 0) || (size.height() < 0) ||
        (size.width() > srect.width()) || (size.height() > srect.height()))
    {
        size = defaultSize;
    }

    instance->mainWindowPos = pos;
    instance->mainWindowSize = size;

    instance->programVersion
        = settings.value(SETTINGS_PROGRAM_VERSION).toString();

    instance->useAutoImport =
        settings.value(SETTINGS_IMPORT, DEFAULT_AUTO_IMPORT).toBool();

    // Get default lexicon, either from current setting or old one
    instance->defaultLexicon
        = settings.value(SETTINGS_DEFAULT_LEXICON).toString();
    if (instance->defaultLexicon.isEmpty()) {
        instance->defaultLexicon
            = settings.value(SETTINGS_IMPORT_LEXICON).toString();
        if (instance->defaultLexicon.isEmpty())
            instance->defaultLexicon = DEFAULT_DEFAULT_LEXICON;
    }
    instance->defaultLexicon =
        Auxil::getUpdatedLexiconName(instance->defaultLexicon);

    instance->autoImportLexicons
        = settings.value(SETTINGS_IMPORT_LEXICONS).toStringList();
    QMutableListIterator<QString> it (instance->autoImportLexicons);
    while (it.hasNext()) {
        const QString& lexicon = it.next();
        QString updatedLexicon = Auxil::getUpdatedLexiconName(lexicon);
        if (updatedLexicon.isEmpty())
            it.remove();
        else
            it.setValue(updatedLexicon);
    }

    instance->autoImportFile
        = settings.value(SETTINGS_IMPORT_FILE).toString();

    instance->displayWelcome
        = settings.value(SETTINGS_DISPLAY_WELCOME,
                         DEFAULT_DISPLAY_WELCOME).toBool();

    instance->userDataDir = QDir::cleanPath(
        settings.value(SETTINGS_USER_DATA_DIR,
                       DEFAULT_USER_DATA_DIR).toString());

    instance->useTileTheme
        = settings.value(SETTINGS_USE_TILE_THEME,
                         DEFAULT_USE_TILE_THEME).toBool();
    instance->tileTheme
        = settings.value(SETTINGS_TILE_THEME, DEFAULT_TILE_THEME).toString();

    instance->searchSelectInput
        = settings.value(SETTINGS_SEARCH_SELECT_INPUT,
                         DEFAULT_SEARCH_SELECT_INPUT).toBool();

    instance->quizLetterOrder
        = settings.value(SETTINGS_QUIZ_LETTER_ORDER,
                         DEFAULT_QUIZ_LETTER_ORDER).toString();

    instance->quizBackgroundColor.setRgb(
        settings.value(SETTINGS_QUIZ_BACKGROUND_COLOR,
                       QString::number(DEFAULT_QUIZ_BACKGROUND_COLOR)).toUInt());

    instance->quizUseFlashcardMode
        = settings.value(SETTINGS_QUIZ_USE_FLASHCARD_MODE,
                         DEFAULT_QUIZ_USE_FLASHCARD_MODE).toBool();
    instance->quizShowNumResponses
        = settings.value(SETTINGS_QUIZ_SHOW_NUM_RESPONSES,
                         DEFAULT_QUIZ_SHOW_NUM_RESPONSES).toBool();
    instance->quizShowQuestionStats
        = settings.value(SETTINGS_QUIZ_SHOW_QUESTION_STATS,
                         DEFAULT_QUIZ_SHOW_QUESTION_STATS).toBool();
    instance->quizRequireLexiconSymbols
        = settings.value(SETTINGS_QUIZ_REQUIRE_LEXICON_SYMBOLS,
                         DEFAULT_QUIZ_REQUIRE_LEXICON_SYMBOLS).toBool();
    instance->quizAutoCheck
        = settings.value(SETTINGS_QUIZ_AUTO_CHECK,
                         DEFAULT_QUIZ_AUTO_CHECK).toBool();
    instance->quizAutoAdvance
        = settings.value(SETTINGS_QUIZ_AUTO_ADVANCE,
                         DEFAULT_QUIZ_AUTO_ADVANCE).toBool();
    instance->quizAutoEndAfterIncorrect
        = settings.value(SETTINGS_QUIZ_AUTO_END_AFTER_INCORRECT,
                         DEFAULT_QUIZ_AUTO_END_AFTER_INCORRECT).toBool();
    instance->quizMarkMissedAfterIncorrect
        = settings.value(SETTINGS_QUIZ_MARK_MISSED_AFTER_INCORRECT,
                         DEFAULT_QUIZ_MARK_MISSED_AFTER_INCORRECT).toBool();
    instance->quizMarkMissedAfterTimerExpires
        = settings.value(SETTINGS_QUIZ_MARK_MISSED_AFTER_TIMER_EXPIRES,
                         DEFAULT_QUIZ_MARK_MISSED_AFTER_TIMER_EXPIRES).toBool();
    instance->quizCycleAnswers
        = settings.value(SETTINGS_QUIZ_CYCLE_ANSWERS,
                         DEFAULT_QUIZ_CYCLE_ANSWERS).toBool();
    instance->quizTimeoutDisableInput
        = settings.value(SETTINGS_QUIZ_TIMEOUT_DISABLE_INPUT,
                         DEFAULT_QUIZ_TIMEOUT_DISABLE_INPUT).toBool();
    instance->quizTimeoutDisableInputMillisecs
        = settings.value(SETTINGS_QUIZ_TIMEOUT_DISABLE_INPUT_MSECS,
                         DEFAULT_QUIZ_TIMEOUT_DISABLE_INPUT_MSECS).toInt();

    instance->quizRecordStats
        = settings.value(SETTINGS_QUIZ_RECORD_STATS,
                         DEFAULT_QUIZ_RECORD_STATS).toBool();

    instance->probabilityNumBlanks
        = settings.value(SETTINGS_PROBABILITY_NUM_BLANKS,
                         DEFAULT_PROBABILITY_NUM_BLANKS).toInt();

    instance->setCardboxScheduleList(
        settings.value(SETTINGS_CARDBOX_SCHEDULES,
                       DEFAULT_CARDBOX_SCHEDULES).toString());

    instance->setCardboxWindowList(
        settings.value(SETTINGS_CARDBOX_WINDOWS,
                       DEFAULT_CARDBOX_WINDOWS).toString());

    instance->judgeSaveLog
        = settings.value(SETTINGS_JUDGE_SAVE_LOG,
                         DEFAULT_JUDGE_SAVE_LOG).toBool();

    instance->mainFont
        = settings.value(SETTINGS_FONT_MAIN).toString();
    instance->wordListFont
        = settings.value(SETTINGS_FONT_WORD_LISTS).toString();
    instance->quizLabelFont
        = settings.value(SETTINGS_FONT_QUIZ_LABEL).toString();
    instance->wordInputFont
        = settings.value(SETTINGS_FONT_WORD_INPUT).toString();
    instance->definitionFont
        = settings.value(SETTINGS_FONT_DEFINITIONS).toString();

    instance->wordListSortByLength
        = settings.value(SETTINGS_SORT_BY_LENGTH,
                         DEFAULT_SORT_BY_LENGTH).toBool();
    instance->wordListGroupByAnagrams
        = settings.value(SETTINGS_GROUP_BY_ALPHAGRAMS,
                         DEFAULT_GROUP_BY_ANAGRAMS).toBool();
    instance->wordListShowProbabilityOrder
        = settings.value(SETTINGS_SHOW_PROBABILITY_ORDER,
                         DEFAULT_SHOW_PROBABILITY_ORDER).toBool();
    instance->wordListShowPlayabilityOrder
        = settings.value(SETTINGS_SHOW_PLAYABILITY_ORDER,
                         DEFAULT_SHOW_PLAYABILITY_ORDER).toBool();
    instance->wordListShowHooks
        = settings.value(SETTINGS_SHOW_HOOKS, DEFAULT_SHOW_HOOKS).toBool();
    instance->wordListShowHookParents
        = settings.value(SETTINGS_SHOW_HOOK_PARENTS,
                         DEFAULT_SHOW_HOOK_PARENTS).toBool();
    instance->wordListUseHookParentHyphens
        = settings.value(SETTINGS_USE_HOOK_PARENT_HYPHENS,
                         DEFAULT_USE_HOOK_PARENT_HYPHENS).toBool();
    instance->wordListShowDefinitions
        = settings.value(SETTINGS_SHOW_DEFINITIONS,
                         DEFAULT_SHOW_DEFINITIONS).toBool();
    instance->wordListLowerCaseWildcards
        = settings.value(SETTINGS_LOWER_CASE_WILDCARDS,
                         DEFAULT_LOWER_CASE_WILDCARDS).toBool();
    instance->wordListUseLexiconStyles
        = settings.value(SETTINGS_USE_LEXICON_STYLES,
                         DEFAULT_USE_LEXICON_STYLES).toBool();

    instance->setWordListLexiconStyles(
        settings.value(SETTINGS_LEXICON_STYLES,
                       DEFAULT_LEXICON_STYLES).toString());

    instance->letterDistribution
        = settings.value(SETTINGS_LETTER_DISTRIBUTION,
                         DEFAULT_LETTER_DISTRIBUTION).toString();

    settings.endGroup();
}

//---------------------------------------------------------------------------
//  writeSettings
//
//! Write settings.
//---------------------------------------------------------------------------
void
MainSettings::writeSettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_MAIN);
    settings.setValue(SETTINGS_MAIN_WINDOW_POS, instance->mainWindowPos);
    settings.setValue(SETTINGS_MAIN_WINDOW_SIZE, instance->mainWindowSize);

    settings.setValue(SETTINGS_PROGRAM_VERSION, instance->programVersion);
    settings.setValue(SETTINGS_IMPORT, instance->useAutoImport);
    settings.setValue(SETTINGS_IMPORT_LEXICONS, instance->autoImportLexicons);
    settings.setValue(SETTINGS_DEFAULT_LEXICON, instance->defaultLexicon);
    settings.setValue(SETTINGS_IMPORT_FILE, instance->autoImportFile);
    settings.setValue(SETTINGS_DISPLAY_WELCOME, instance->displayWelcome);
    settings.setValue(SETTINGS_USER_DATA_DIR, instance->userDataDir);
    settings.setValue(SETTINGS_USE_TILE_THEME, instance->useTileTheme);
    settings.setValue(SETTINGS_TILE_THEME, instance->tileTheme);
    settings.setValue(SETTINGS_SEARCH_SELECT_INPUT,
                      instance->searchSelectInput);
    settings.setValue(SETTINGS_QUIZ_LETTER_ORDER,
                      instance->quizLetterOrder);
    settings.setValue(SETTINGS_QUIZ_BACKGROUND_COLOR,
                      QString::number(instance->quizBackgroundColor.rgb()));
    settings.setValue(SETTINGS_QUIZ_USE_FLASHCARD_MODE,
                      instance->quizUseFlashcardMode);
    settings.setValue(SETTINGS_QUIZ_SHOW_NUM_RESPONSES,
                      instance->quizShowNumResponses);
    settings.setValue(SETTINGS_QUIZ_SHOW_QUESTION_STATS,
                      instance->quizShowQuestionStats);
    settings.setValue(SETTINGS_QUIZ_REQUIRE_LEXICON_SYMBOLS,
                      instance->quizRequireLexiconSymbols);
    settings.setValue(SETTINGS_QUIZ_AUTO_CHECK, instance->quizAutoCheck);
    settings.setValue(SETTINGS_QUIZ_AUTO_ADVANCE,
                      instance->quizAutoAdvance);
    settings.setValue(SETTINGS_QUIZ_AUTO_END_AFTER_INCORRECT,
                      instance->quizAutoEndAfterIncorrect);
    settings.setValue(SETTINGS_QUIZ_MARK_MISSED_AFTER_INCORRECT,
                      instance->quizMarkMissedAfterIncorrect);
    settings.setValue(SETTINGS_QUIZ_MARK_MISSED_AFTER_TIMER_EXPIRES,
                      instance->quizMarkMissedAfterTimerExpires);
    settings.setValue(SETTINGS_QUIZ_CYCLE_ANSWERS,
                      instance->quizCycleAnswers);
    settings.setValue(SETTINGS_QUIZ_TIMEOUT_DISABLE_INPUT,
                      instance->quizTimeoutDisableInput);
    settings.setValue(SETTINGS_QUIZ_TIMEOUT_DISABLE_INPUT_MSECS,
                      instance->quizTimeoutDisableInputMillisecs);
    settings.setValue(SETTINGS_QUIZ_RECORD_STATS,
                      instance->quizRecordStats);

    settings.setValue(SETTINGS_PROBABILITY_NUM_BLANKS,
                      instance->probabilityNumBlanks);

    QString schedStr;
    foreach (int sched, instance->cardboxScheduleList) {
        if (!schedStr.isEmpty())
            schedStr += ' ';
        schedStr += QString::number(sched);
    }
    settings.setValue(SETTINGS_CARDBOX_SCHEDULES, schedStr);

    QString windowStr;
    foreach (int window, instance->cardboxWindowList) {
        if (!windowStr.isEmpty())
            windowStr += ' ';
        windowStr += QString::number(window);
    }
    settings.setValue(SETTINGS_CARDBOX_WINDOWS, windowStr);


    settings.setValue(SETTINGS_FONT_MAIN, instance->mainFont);
    settings.setValue(SETTINGS_FONT_WORD_LISTS, instance->wordListFont);
    settings.setValue(SETTINGS_FONT_QUIZ_LABEL, instance->quizLabelFont);
    settings.setValue(SETTINGS_FONT_WORD_INPUT, instance->wordInputFont);
    settings.setValue(SETTINGS_FONT_DEFINITIONS, instance->definitionFont);
    settings.setValue(SETTINGS_SORT_BY_LENGTH,
                      instance->wordListSortByLength);
    settings.setValue(SETTINGS_GROUP_BY_ALPHAGRAMS,
                      instance->wordListGroupByAnagrams);
    settings.setValue(SETTINGS_SHOW_PROBABILITY_ORDER,
                      instance->wordListShowProbabilityOrder);
    settings.setValue(SETTINGS_SHOW_PLAYABILITY_ORDER,
                      instance->wordListShowPlayabilityOrder);
    settings.setValue(SETTINGS_SHOW_HOOKS, instance->wordListShowHooks);
    settings.setValue(SETTINGS_SHOW_HOOK_PARENTS,
                      instance->wordListShowHookParents);
    settings.setValue(SETTINGS_USE_HOOK_PARENT_HYPHENS,
                      instance->wordListUseHookParentHyphens);
    settings.setValue(SETTINGS_SHOW_DEFINITIONS,
                      instance->wordListShowDefinitions);
    settings.setValue(SETTINGS_LOWER_CASE_WILDCARDS,
                      instance->wordListLowerCaseWildcards);
    settings.setValue(SETTINGS_USE_LEXICON_STYLES,
                      instance->wordListUseLexiconStyles);

    QString lexiconStyleStr;
    QListIterator<LexiconStyle> it (instance->wordListLexiconStyles);
    while (it.hasNext()) {
        const LexiconStyle& style = it.next();
        if (!lexiconStyleStr.isEmpty())
            lexiconStyleStr += "\n";
        lexiconStyleStr += Auxil::lexiconStyleToString(style);
    }
    settings.setValue(SETTINGS_LEXICON_STYLES, lexiconStyleStr);

    settings.setValue(SETTINGS_LETTER_DISTRIBUTION,
                      instance->letterDistribution);
    settings.setValue(SETTINGS_JUDGE_SAVE_LOG, instance->judgeSaveLog);
    settings.endGroup();
}

//---------------------------------------------------------------------------
//  restoreDefaults
//
//! Restore default settings for a settings group.
//!
//! @param group the settings group, use empty string to restore defaults for
//! all groups
//---------------------------------------------------------------------------
void
MainSettings::restoreDefaults(const QString& group)
{
    if (group.isEmpty() || (group == GENERAL_PREFS_GROUP)) {
        instance->useAutoImport = DEFAULT_AUTO_IMPORT;
        instance->defaultLexicon = DEFAULT_DEFAULT_LEXICON;
        instance->autoImportLexicons = QStringList(DEFAULT_DEFAULT_LEXICON);
        instance->autoImportFile = QString();
        instance->displayWelcome = DEFAULT_DISPLAY_WELCOME;
        instance->userDataDir = DEFAULT_USER_DATA_DIR;
    }

    if (group.isEmpty() || (group == SEARCH_PREFS_GROUP)) {
        instance->searchSelectInput = DEFAULT_SEARCH_SELECT_INPUT;
    }

    if (group.isEmpty() || (group == QUIZ_PREFS_GROUP)) {
        instance->useTileTheme = DEFAULT_USE_TILE_THEME;
        instance->tileTheme = DEFAULT_TILE_THEME;
        instance->quizLetterOrder = DEFAULT_QUIZ_LETTER_ORDER;
        instance->quizBackgroundColor.setRgb(DEFAULT_QUIZ_BACKGROUND_COLOR);
        instance->quizUseFlashcardMode = DEFAULT_QUIZ_USE_FLASHCARD_MODE;
        instance->quizShowNumResponses = DEFAULT_QUIZ_SHOW_NUM_RESPONSES;
        instance->quizShowQuestionStats = DEFAULT_QUIZ_SHOW_QUESTION_STATS;
        instance->quizRequireLexiconSymbols =
            DEFAULT_QUIZ_REQUIRE_LEXICON_SYMBOLS;
        instance->quizAutoCheck = DEFAULT_QUIZ_AUTO_CHECK;
        instance->quizAutoAdvance = DEFAULT_QUIZ_AUTO_ADVANCE;
        instance->quizAutoEndAfterIncorrect =
            DEFAULT_QUIZ_AUTO_END_AFTER_INCORRECT;
        instance->quizMarkMissedAfterIncorrect =
            DEFAULT_QUIZ_MARK_MISSED_AFTER_INCORRECT;
        instance->quizMarkMissedAfterTimerExpires =
            DEFAULT_QUIZ_MARK_MISSED_AFTER_TIMER_EXPIRES;
        instance->quizCycleAnswers = DEFAULT_QUIZ_CYCLE_ANSWERS;
        instance->quizTimeoutDisableInput =
            DEFAULT_QUIZ_TIMEOUT_DISABLE_INPUT;
        instance->quizTimeoutDisableInputMillisecs =
            DEFAULT_QUIZ_TIMEOUT_DISABLE_INPUT_MSECS;
        instance->quizRecordStats = DEFAULT_QUIZ_RECORD_STATS;
    }

    if (group.isEmpty() || (group == PROBABILITY_PREFS_GROUP)) {
        instance->probabilityNumBlanks = DEFAULT_PROBABILITY_NUM_BLANKS;
    }

    if (group.isEmpty() || (group == CARDBOX_PREFS_GROUP)) {
        instance->setCardboxScheduleList(DEFAULT_CARDBOX_SCHEDULES);
        instance->setCardboxWindowList(DEFAULT_CARDBOX_WINDOWS);
    }

    if (group.isEmpty() || (group == JUDGE_PREFS_GROUP)) {
        instance->judgeSaveLog = DEFAULT_JUDGE_SAVE_LOG;
    }

    if (group.isEmpty() || (group == FONT_PREFS_GROUP)) {
        instance->mainFont = QString();
        instance->wordListFont = QString();
        instance->quizLabelFont = QString();
        instance->wordInputFont = QString();
        instance->definitionFont = QString();
    }

    if (group.isEmpty() || (group == WORD_LIST_PREFS_GROUP)) {
        instance->wordListSortByLength = DEFAULT_SORT_BY_LENGTH;
        instance->wordListGroupByAnagrams = DEFAULT_GROUP_BY_ANAGRAMS;
        instance->wordListShowProbabilityOrder =
            DEFAULT_SHOW_PROBABILITY_ORDER;
        instance->wordListShowPlayabilityOrder =
            DEFAULT_SHOW_PLAYABILITY_ORDER;
        instance->wordListShowHooks = DEFAULT_SHOW_HOOKS;
        instance->wordListShowHookParents = DEFAULT_SHOW_HOOK_PARENTS;
        instance->wordListUseHookParentHyphens =
            DEFAULT_USE_HOOK_PARENT_HYPHENS;
        instance->wordListShowDefinitions = DEFAULT_SHOW_DEFINITIONS;
        instance->wordListLowerCaseWildcards = DEFAULT_LOWER_CASE_WILDCARDS;
        instance->wordListUseLexiconStyles = DEFAULT_USE_LEXICON_STYLES;
        instance->setWordListLexiconStyles(DEFAULT_LEXICON_STYLES);
    }

    // ### Not user-visible yet
    instance->letterDistribution = DEFAULT_LETTER_DISTRIBUTION;
}

//---------------------------------------------------------------------------
//  setCardboxScheduleList
//
//! Set the cardbox schedule list from a string.
//!
//! @param str the string
//---------------------------------------------------------------------------
void
MainSettings::setCardboxScheduleList(const QString& str)
{
    cardboxScheduleList.clear();
    foreach (const QString& str, str.split(QChar(' '))) {
        cardboxScheduleList.append(str.toInt());
    }
}

//---------------------------------------------------------------------------
//  setCardboxWindowList
//
//! Set the cardbox window list from a string.
//!
//! @param str the string
//---------------------------------------------------------------------------
void
MainSettings::setCardboxWindowList(const QString& str)
{
    cardboxWindowList.clear();
    foreach (const QString& str, str.split(QChar(' '))) {
        cardboxWindowList.append(str.toInt());
    }
}

//---------------------------------------------------------------------------
//  setWordListLexiconStyles
//
//! Set the word list lexicon styles from a string.
//!
//! @param str the string
//---------------------------------------------------------------------------
void
MainSettings::setWordListLexiconStyles(const QString& str)
{
    wordListLexiconStyles.clear();
    foreach (const QString& s, str.split(QChar('\n'))) {
        LexiconStyle style = Auxil::stringToLexiconStyle(s);
        if (!style.isValid())
            continue;
        wordListLexiconStyles.append(style);
    }
}
