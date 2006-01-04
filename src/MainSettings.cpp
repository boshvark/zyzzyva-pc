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

const QString SETTINGS_MAIN = "/Zyzzyva";
const QString SETTINGS_GEOMETRY = "/geometry";
const QString SETTINGS_GEOMETRY_X = "/x";
const QString SETTINGS_GEOMETRY_Y = "/y";
const QString SETTINGS_GEOMETRY_WIDTH = "/width";
const QString SETTINGS_GEOMETRY_HEIGHT = "/height";
const QString SETTINGS_IMPORT = "/autoimport";
const QString SETTINGS_IMPORT_FILE = "/autoimport_file";
const QString SETTINGS_FONT_MAIN = "/font";
const QString SETTINGS_FONT_WORD_LISTS = "/font_word_lists";
const QString SETTINGS_FONT_QUIZ_LABEL = "/font_quiz_label";
const QString SETTINGS_FONT_DEFINITIONS = "/font_definitions";
const QString SETTINGS_FONT_WORD_INPUT = "/font_word_input";
const QString SETTINGS_SORT_BY_LENGTH = "/wordlist_sort_by_length";
const QString SETTINGS_SHOW_HOOKS = "/wordlist_show_hooks";
const QString SETTINGS_SHOW_HOOK_PARENTS = "/wordlist_show_hook_parents";
const QString SETTINGS_SHOW_DEFINITIONS = "/wordlist_show_definitions";
const QString SETTINGS_USE_TILE_THEME = "/use_tile_theme";
const QString SETTINGS_TILE_THEME = "/tile_theme";
const QString SETTINGS_QUIZ_LETTER_ORDER = "/quiz_letter_order";
const QString SETTINGS_QUIZ_BACKGROUND_COLOR = "/quiz_background_color";
const QString SETTINGS_QUIZ_SHOW_NUM_RESPONSES = "/quiz_show_num_responses";
const QString SETTINGS_QUIZ_AUTO_CHECK = "/quiz_auto_check";
const QString DEFAULT_AUTO_IMPORT_FILE = "/north-american/twl98.txt";
const QString DEFAULT_TILE_THEME = "tan-with-border";
const QString DEFAULT_QUIZ_LETTER_ORDER = Defs::QUIZ_LETTERS_ALPHA;
const QRgb    DEFAULT_QUIZ_BACKGROUND_COLOR = qRgb (240, 255, 127);

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

    settings.beginGroup (SETTINGS_GEOMETRY);
    instance->mainWindowX = settings.readNumEntry (SETTINGS_GEOMETRY_X, 50);
    instance->mainWindowY = settings.readNumEntry (SETTINGS_GEOMETRY_Y, 50);
    instance->mainWindowWidth = settings.readNumEntry
        (SETTINGS_GEOMETRY_WIDTH, 640);
    instance->mainWindowHeight = settings.readNumEntry
        (SETTINGS_GEOMETRY_HEIGHT, 480);
    settings.endGroup();

    instance->useAutoImport = settings.readBoolEntry (SETTINGS_IMPORT, true);
    instance->autoImportFile = settings.readEntry (SETTINGS_IMPORT_FILE,
                                                   Auxil::getWordsDir() +
                                                   DEFAULT_AUTO_IMPORT_FILE);

    instance->useTileTheme = settings.readBoolEntry (SETTINGS_USE_TILE_THEME,
                                                     true);
    instance->tileTheme = settings.readEntry (SETTINGS_TILE_THEME,
                                              DEFAULT_TILE_THEME);

    instance->quizLetterOrder = settings.readEntry
        (SETTINGS_QUIZ_LETTER_ORDER, DEFAULT_QUIZ_LETTER_ORDER);

    instance->quizBackgroundColor.setRgb (settings.readEntry
        (SETTINGS_QUIZ_BACKGROUND_COLOR,
         QString::number (DEFAULT_QUIZ_BACKGROUND_COLOR)).toUInt());

    instance->quizShowNumResponses = settings.readBoolEntry
        (SETTINGS_QUIZ_SHOW_NUM_RESPONSES, true);
    instance->quizAutoCheck = settings.readBoolEntry
        (SETTINGS_QUIZ_AUTO_CHECK, true);

    instance->mainFont = settings.readEntry (SETTINGS_FONT_MAIN);
    instance->wordListFont = settings.readEntry (SETTINGS_FONT_WORD_LISTS);
    instance->quizLabelFont = settings.readEntry (SETTINGS_FONT_QUIZ_LABEL);
    instance->wordInputFont = settings.readEntry (SETTINGS_FONT_WORD_INPUT);
    instance->definitionFont = settings.readEntry (SETTINGS_FONT_DEFINITIONS);


    instance->wordListSortByLength = settings.readBoolEntry
        (SETTINGS_SORT_BY_LENGTH, false);
    instance->wordListShowHooks = settings.readBoolEntry (SETTINGS_SHOW_HOOKS,
                                                          true);
    instance->wordListShowHookParents = settings.readBoolEntry
        (SETTINGS_SHOW_HOOK_PARENTS, true);
    instance->wordListShowDefinitions = settings.readBoolEntry
        (SETTINGS_SHOW_DEFINITIONS, true);
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
    settings.beginGroup (SETTINGS_GEOMETRY);
    settings.writeEntry (SETTINGS_GEOMETRY_X, instance->mainWindowX);
    settings.writeEntry (SETTINGS_GEOMETRY_Y, instance->mainWindowY);
    settings.writeEntry (SETTINGS_GEOMETRY_WIDTH, instance->mainWindowWidth);
    settings.writeEntry (SETTINGS_GEOMETRY_HEIGHT,
                         instance->mainWindowHeight);
    settings.endGroup();

    settings.writeEntry (SETTINGS_IMPORT, instance->useAutoImport);
    settings.writeEntry (SETTINGS_IMPORT_FILE, instance->autoImportFile);
    settings.writeEntry (SETTINGS_USE_TILE_THEME, instance->useTileTheme);
    settings.writeEntry (SETTINGS_TILE_THEME, instance->tileTheme);
    settings.writeEntry (SETTINGS_QUIZ_LETTER_ORDER,
                         instance->quizLetterOrder);
    settings.writeEntry (SETTINGS_QUIZ_BACKGROUND_COLOR,
                         QString::number
                         (instance->quizBackgroundColor.rgb()));
    settings.writeEntry (SETTINGS_QUIZ_SHOW_NUM_RESPONSES,
                         instance->quizShowNumResponses);
    settings.writeEntry (SETTINGS_QUIZ_AUTO_CHECK, instance->quizAutoCheck);
    settings.writeEntry (SETTINGS_FONT_MAIN, instance->mainFont);
    settings.writeEntry (SETTINGS_FONT_WORD_LISTS, instance->wordListFont);
    settings.writeEntry (SETTINGS_FONT_QUIZ_LABEL, instance->quizLabelFont);
    settings.writeEntry (SETTINGS_FONT_WORD_INPUT, instance->wordInputFont);
    settings.writeEntry (SETTINGS_FONT_DEFINITIONS, instance->definitionFont);
    settings.writeEntry (SETTINGS_SORT_BY_LENGTH,
                         instance->wordListSortByLength);
    settings.writeEntry (SETTINGS_SHOW_HOOKS, instance->wordListShowHooks);
    settings.writeEntry (SETTINGS_SHOW_HOOK_PARENTS,
                         instance->wordListShowHookParents);
    settings.writeEntry (SETTINGS_SHOW_DEFINITIONS,
                         instance->wordListShowDefinitions);
    settings.endGroup();
}
