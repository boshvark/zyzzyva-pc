//---------------------------------------------------------------------------
// MainSettings.cpp
//
// The main settings for the word study application.
//
// Copyright 2005, 2006, 2007, 2008 Michael W Thelen <mthelen@gmail.com>.
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
const QString SETTINGS_SHOW_HOOKS = "wordlist_show_hooks";
const QString SETTINGS_SHOW_HOOK_PARENTS = "wordlist_show_hook_parents";
const QString SETTINGS_SHOW_DEFINITIONS = "wordlist_show_definitions";
const QString SETTINGS_USE_LEXICON_STYLES = "wordlist_use_lexicon_styles";
const QString SETTINGS_LEXICON_STYLES = "wordlist_lexicon_styles";
const QString SETTINGS_USE_TILE_THEME = "use_tile_theme";
const QString SETTINGS_TILE_THEME = "tile_theme";
const QString SETTINGS_QUIZ_LETTER_ORDER = "quiz_letter_order";
const QString SETTINGS_QUIZ_BACKGROUND_COLOR = "quiz_background_color";
const QString SETTINGS_QUIZ_USE_FLASHCARD_MODE = "quiz_use_flashcard_mode";
const QString SETTINGS_QUIZ_SHOW_NUM_RESPONSES = "quiz_show_num_responses";
const QString SETTINGS_QUIZ_SHOW_QUESTION_STATS = "quiz_show_question_stat";
const QString SETTINGS_QUIZ_AUTO_CHECK = "quiz_auto_check";
const QString SETTINGS_QUIZ_AUTO_ADVANCE = "quiz_auto_advance";
const QString SETTINGS_QUIZ_AUTO_END_AFTER_INCORRECT
    = "quiz_auto_end_after_incorrect";
const QString SETTINGS_QUIZ_MARK_MISSED_AFTER_INCORRECT
    = "quiz_mark_missed_after_incorrect";
const QString SETTINGS_QUIZ_CYCLE_ANSWERS = "quiz_cycle_answers";
const QString SETTINGS_QUIZ_RECORD_STATS = "quiz_record_stats";
const QString SETTINGS_LETTER_DISTRIBUTION = "letter_distribution";
const QString SETTINGS_JUDGE_SAVE_LOG = "judge_save_log";
const QString DEFAULT_DEFAULT_LEXICON = "OWL2+LWL";
const QString DEFAULT_TILE_THEME = "tan-with-border";
const QString DEFAULT_QUIZ_LETTER_ORDER = Defs::QUIZ_LETTERS_ALPHA;
const QRgb    DEFAULT_QUIZ_BACKGROUND_COLOR = qRgb(0, 0, 127);
const QString DEFAULT_LEXICON_STYLES =
    "OWL2+LWL and not OWL+LWL: symbol %\n"
    "OWL2+LWL and not OSPD4+LWL: symbol !\n"
    "CSW and not OWL2+LWL: symbol #\n"
    "CSW and not OSWI: symbol ^";
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

    instance->useAutoImport = settings.value(SETTINGS_IMPORT, true).toBool();

    // Get default lexicon, either from current setting or old one
    instance->defaultLexicon
        = settings.value(SETTINGS_DEFAULT_LEXICON).toString();
    if (instance->defaultLexicon.isEmpty()) {
        instance->defaultLexicon
            = settings.value(SETTINGS_IMPORT_LEXICON).toString();
        if (instance->defaultLexicon.isEmpty())
            instance->defaultLexicon = DEFAULT_DEFAULT_LEXICON;
    }

    instance->autoImportLexicons
        = settings.value(SETTINGS_IMPORT_LEXICONS).toStringList();

    // Kludge to update the names of renamed lexicons
    if (instance->defaultLexicon == "OWL")
        instance->defaultLexicon = "OWL+LWL";
    else if (instance->defaultLexicon == "OWL2")
        instance->defaultLexicon = "OWL2+LWL";
    else if (instance->defaultLexicon == "SOWPODS")
        instance->defaultLexicon = "OSWI";

    instance->autoImportFile
        = settings.value(SETTINGS_IMPORT_FILE).toString();

    instance->displayWelcome
        = settings.value(SETTINGS_DISPLAY_WELCOME, true).toBool();

    QString defaultUserDir = Auxil::getHomeDir() + "/.zyzzyva";
    instance->userDataDir = QDir::cleanPath(
        settings.value(SETTINGS_USER_DATA_DIR, defaultUserDir).toString());

    instance->useTileTheme
        = settings.value(SETTINGS_USE_TILE_THEME, true).toBool();
    instance->tileTheme
        = settings.value(SETTINGS_TILE_THEME, DEFAULT_TILE_THEME).toString();

    instance->quizLetterOrder
        = settings.value(SETTINGS_QUIZ_LETTER_ORDER,
                         DEFAULT_QUIZ_LETTER_ORDER).toString();

    instance->quizBackgroundColor.setRgb(
        settings.value(SETTINGS_QUIZ_BACKGROUND_COLOR,
                       QString::number(DEFAULT_QUIZ_BACKGROUND_COLOR)).toUInt());

    instance->quizUseFlashcardMode
        = settings.value(SETTINGS_QUIZ_USE_FLASHCARD_MODE, false).toBool();
    instance->quizShowNumResponses
        = settings.value(SETTINGS_QUIZ_SHOW_NUM_RESPONSES, true).toBool();
    instance->quizShowQuestionStats
        = settings.value(SETTINGS_QUIZ_SHOW_QUESTION_STATS, true).toBool();
    instance->quizAutoCheck
        = settings.value(SETTINGS_QUIZ_AUTO_CHECK, true).toBool();
    instance->quizAutoAdvance
        = settings.value(SETTINGS_QUIZ_AUTO_ADVANCE, false).toBool();
    instance->quizAutoEndAfterIncorrect
        = settings.value(SETTINGS_QUIZ_AUTO_END_AFTER_INCORRECT,
                         false).toBool();
    instance->quizMarkMissedAfterIncorrect
        = settings.value(SETTINGS_QUIZ_MARK_MISSED_AFTER_INCORRECT,
                         true).toBool();
    instance->quizCycleAnswers
        = settings.value(SETTINGS_QUIZ_CYCLE_ANSWERS, true).toBool();
    instance->quizRecordStats
        = settings.value(SETTINGS_QUIZ_RECORD_STATS, true).toBool();

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
        = settings.value(SETTINGS_SORT_BY_LENGTH, false).toBool();
    instance->wordListGroupByAnagrams
        = settings.value(SETTINGS_GROUP_BY_ALPHAGRAMS, true).toBool();
    instance->wordListShowProbabilityOrder
        = settings.value(SETTINGS_SHOW_PROBABILITY_ORDER, true).toBool();
    instance->wordListShowHooks
        = settings.value(SETTINGS_SHOW_HOOKS, true).toBool();
    instance->wordListShowHookParents
        = settings.value(SETTINGS_SHOW_HOOK_PARENTS, true).toBool();
    instance->wordListShowDefinitions
        = settings.value(SETTINGS_SHOW_DEFINITIONS, true).toBool();

    instance->wordListUseLexiconStyles
        = settings.value(SETTINGS_USE_LEXICON_STYLES, true).toBool();

    QString lexiconStyleStr
        = settings.value(SETTINGS_LEXICON_STYLES,
                         DEFAULT_LEXICON_STYLES).toString();
    instance->wordListLexiconStyles.clear();
    QString str;
    foreach (str, lexiconStyleStr.split(QChar('\n'))) {
        qDebug("Read string: |%s|", str.toUtf8().constData());
        LexiconStyle style = Auxil::stringToLexiconStyle(str);
        if (!style.isValid())
            continue;
        instance->wordListLexiconStyles.append(style);
        qDebug("Read style:  |%s|",
               Auxil::lexiconStyleToString(style).toUtf8().constData());
    }

    instance->letterDistribution
        = settings.value(SETTINGS_LETTER_DISTRIBUTION,
                         DEFAULT_LETTER_DISTRIBUTION).toString();

    instance->judgeSaveLog
        = settings.value(SETTINGS_JUDGE_SAVE_LOG, true).toBool();

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
    settings.setValue(SETTINGS_QUIZ_AUTO_CHECK, instance->quizAutoCheck);
    settings.setValue(SETTINGS_QUIZ_AUTO_ADVANCE,
                      instance->quizAutoAdvance);
    settings.setValue(SETTINGS_QUIZ_AUTO_END_AFTER_INCORRECT,
                      instance->quizAutoEndAfterIncorrect);
    settings.setValue(SETTINGS_QUIZ_MARK_MISSED_AFTER_INCORRECT,
                      instance->quizMarkMissedAfterIncorrect);
    settings.setValue(SETTINGS_QUIZ_CYCLE_ANSWERS,
                      instance->quizCycleAnswers);
    settings.setValue(SETTINGS_QUIZ_RECORD_STATS,
                      instance->quizRecordStats);
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
    settings.setValue(SETTINGS_SHOW_HOOKS, instance->wordListShowHooks);
    settings.setValue(SETTINGS_SHOW_HOOK_PARENTS,
                      instance->wordListShowHookParents);
    settings.setValue(SETTINGS_SHOW_DEFINITIONS,
                      instance->wordListShowDefinitions);
    settings.setValue(SETTINGS_LETTER_DISTRIBUTION,
                      instance->letterDistribution);
    settings.setValue(SETTINGS_JUDGE_SAVE_LOG, instance->judgeSaveLog);
    settings.endGroup();
}
