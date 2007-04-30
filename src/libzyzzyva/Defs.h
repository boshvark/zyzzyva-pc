//---------------------------------------------------------------------------
// Defs.h
//
// Definitions.
//
// Copyright 2004, 2005, 2006, 2007 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_DEFS_H
#define ZYZZYVA_DEFS_H

#include <QColor>
#include <QString>

namespace Defs {
    const QString ZYZZYVA_VERSION = "1.0.4";
    const int CURRENT_DATABASE_VERSION = 2;
    const QString IMPORT_CHOOSER_TITLE = "Choose a Word List";
    const QString EMPTY_DEFINITION = "(no definition)";
    const int DEFINITION_WRAP_LENGTH = 80;
    const int MAX_WORD_LEN = 15;
    const int MAX_INPUT_LINE_LEN = 640;
    const int SPACING = 4;
    const int MARGIN = 4;
    const int DIALOG_LINE_LENGTH = 60;
    const int QUIZ_TILE_SPACING = 10;
    const int QUIZ_TILE_MARGIN = 15;
    const QString QUIZ_LETTERS_ALPHA = "Alphabetical";
    const QString QUIZ_LETTERS_RANDOM = "Random";
    const QString QUIZ_LETTERS_VOWELS_FIRST = "Vowels First";
    const QString QUIZ_LETTERS_CONSONANTS_FIRST = "Consonants First";
}

#endif // ZYZZYVA_DEFS_H
