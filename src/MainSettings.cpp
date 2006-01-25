//---------------------------------------------------------------------------
// MainSettings.cpp
//
// The main settings for the word study application.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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
#include <QSettings>

MainSettings* MainSettings::instance = new MainSettings();

const QString SETTINGS_MAIN = "Zyzzyva";
const QString SETTINGS_MAIN_WINDOW_POS = "main_window_pos";
const QString SETTINGS_MAIN_WINDOW_SIZE = "main_window_size";
const QString SETTINGS_IMPORT = "autoimport";
const QString SETTINGS_IMPORT_LEXICON = "autoimport_lexicon";
const QString SETTINGS_IMPORT_FILE = "autoimport_file";
const QString SETTINGS_FONT_MAIN = "font";
const QString SETTINGS_FONT_WORD_LISTS = "font_word_lists";
const QString SETTINGS_FONT_QUIZ_LABEL = "font_quiz_label";
const QString SETTINGS_FONT_DEFINITIONS = "font_definitions";
const QString SETTINGS_FONT_WORD_INPUT = "font_word_input";
const QString SETTINGS_SORT_BY_LENGTH = "wordlist_sort_by_length";
const QString SETTINGS_SHOW_HOOKS = "wordlist_show_hooks";
const QString SETTINGS_SHOW_HOOK_PARENTS = "wordlist_show_hook_parents";
const QString SETTINGS_SHOW_DEFINITIONS = "wordlist_show_definitions";
const QString SETTINGS_USE_TILE_THEME = "use_tile_theme";
const QString SETTINGS_TILE_THEME = "tile_theme";
const QString SETTINGS_QUIZ_LETTER_ORDER = "quiz_letter_order";
const QString SETTINGS_QUIZ_BACKGROUND_COLOR = "quiz_background_color";
const QString SETTINGS_QUIZ_SHOW_NUM_RESPONSES = "quiz_show_num_responses";
const QString SETTINGS_QUIZ_AUTO_CHECK = "quiz_auto_check";
const QString SETTINGS_QUIZ_AUTO_ADVANCE = "quiz_auto_advance";
const QString DEFAULT_AUTO_IMPORT_LEXICON = "OWL";
const QString DEFAULT_AUTO_IMPORT_FILE = "/north-american/twl98.txt";
const QString DEFAULT_TILE_THEME = "tan-with-border";
const QString DEFAULT_QUIZ_LETTER_ORDER = Defs::QUIZ_LETTERS_ALPHA;
const QRgb    DEFAULT_QUIZ_BACKGROUND_COLOR = qRgb (0, 0, 127);

//---------------------------------------------------------------------------
//  readSettings
//
//! Read settings.
//---------------------------------------------------------------------------
void
MainSettings::readSettings()
{
    QSettings settings;
    settings.beginGroup (SETTINGS_MAIN);

    instance->mainWindowPos =
        settings.value (SETTINGS_MAIN_WINDOW_POS, QPoint (50, 50)).toPoint();
    instance->mainWindowSize
        = settings.value (SETTINGS_MAIN_WINDOW_SIZE,
                          QSize (640, 480)).toSize();

    instance->useAutoImport = settings.value (SETTINGS_IMPORT, true).toBool();
    instance->autoImportLexicon
        = settings.value (SETTINGS_IMPORT_LEXICON, Auxil::getWordsDir() +
                          DEFAULT_AUTO_IMPORT_LEXICON).toString();

    instance->autoImportFile
        = settings.value (SETTINGS_IMPORT_FILE, Auxil::getWordsDir() +
                          DEFAULT_AUTO_IMPORT_FILE).toString();

    instance->useTileTheme
        = settings.value (SETTINGS_USE_TILE_THEME, true).toBool();
    instance->tileTheme
        = settings.value (SETTINGS_TILE_THEME, DEFAULT_TILE_THEME).toString();

    instance->quizLetterOrder
        = settings.value (SETTINGS_QUIZ_LETTER_ORDER,
                          DEFAULT_QUIZ_LETTER_ORDER).toString();

    instance->quizBackgroundColor.setRgb (
        settings.value (SETTINGS_QUIZ_BACKGROUND_COLOR,
                        QString::number (DEFAULT_QUIZ_BACKGROUND_COLOR)
                        ).toUInt());

    instance->quizShowNumResponses
        = settings.value (SETTINGS_QUIZ_SHOW_NUM_RESPONSES, true).toBool();
    instance->quizAutoCheck
        = settings.value (SETTINGS_QUIZ_AUTO_CHECK, true).toBool();
    instance->quizAutoAdvance
        = settings.value (SETTINGS_QUIZ_AUTO_ADVANCE, false).toBool();

    instance->mainFont
        = settings.value (SETTINGS_FONT_MAIN).toString();
    instance->wordListFont
        = settings.value (SETTINGS_FONT_WORD_LISTS).toString();
    instance->quizLabelFont
        = settings.value (SETTINGS_FONT_QUIZ_LABEL).toString();
    instance->wordInputFont
        = settings.value (SETTINGS_FONT_WORD_INPUT).toString();
    instance->definitionFont
        = settings.value (SETTINGS_FONT_DEFINITIONS).toString();


    instance->wordListSortByLength
        = settings.value (SETTINGS_SORT_BY_LENGTH, false).toBool();
    instance->wordListShowHooks
        = settings.value (SETTINGS_SHOW_HOOKS, true).toBool();
    instance->wordListShowHookParents
        = settings.value (SETTINGS_SHOW_HOOK_PARENTS, true).toBool();
    instance->wordListShowDefinitions
        = settings.value (SETTINGS_SHOW_DEFINITIONS, true).toBool();
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
    settings.beginGroup (SETTINGS_MAIN);
    settings.setValue (SETTINGS_MAIN_WINDOW_POS, instance->mainWindowPos);
    settings.setValue (SETTINGS_MAIN_WINDOW_SIZE, instance->mainWindowSize);

    settings.setValue (SETTINGS_IMPORT, instance->useAutoImport);
    settings.setValue (SETTINGS_IMPORT_LEXICON, instance->autoImportLexicon);
    settings.setValue (SETTINGS_IMPORT_FILE, instance->autoImportFile);
    settings.setValue (SETTINGS_USE_TILE_THEME, instance->useTileTheme);
    settings.setValue (SETTINGS_TILE_THEME, instance->tileTheme);
    settings.setValue (SETTINGS_QUIZ_LETTER_ORDER,
                       instance->quizLetterOrder);
    settings.setValue (SETTINGS_QUIZ_BACKGROUND_COLOR,
                       QString::number (instance->quizBackgroundColor.rgb()));
    settings.setValue (SETTINGS_QUIZ_SHOW_NUM_RESPONSES,
                       instance->quizShowNumResponses);
    settings.setValue (SETTINGS_QUIZ_AUTO_CHECK, instance->quizAutoCheck);
    settings.setValue (SETTINGS_QUIZ_AUTO_ADVANCE,
                       instance->quizAutoAdvance);
    settings.setValue (SETTINGS_FONT_MAIN, instance->mainFont);
    settings.setValue (SETTINGS_FONT_WORD_LISTS, instance->wordListFont);
    settings.setValue (SETTINGS_FONT_QUIZ_LABEL, instance->quizLabelFont);
    settings.setValue (SETTINGS_FONT_WORD_INPUT, instance->wordInputFont);
    settings.setValue (SETTINGS_FONT_DEFINITIONS, instance->definitionFont);
    settings.setValue (SETTINGS_SORT_BY_LENGTH,
                       instance->wordListSortByLength);
    settings.setValue (SETTINGS_SHOW_HOOKS, instance->wordListShowHooks);
    settings.setValue (SETTINGS_SHOW_HOOK_PARENTS,
                       instance->wordListShowHookParents);
    settings.setValue (SETTINGS_SHOW_DEFINITIONS,
                       instance->wordListShowDefinitions);
    settings.endGroup();
}
