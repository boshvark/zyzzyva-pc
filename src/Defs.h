//---------------------------------------------------------------------------
// Defs.h
//
// Definitions.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef DEFS_H
#define DEFS_H

#include <qstring.h>

namespace Defs {
    const QString IMPORT_CHOOSER_TITLE = "Choose a Word List";
    const QString EMPTY_DEFINITION = "(no definition)";
    const int DEFINITION_WRAP_LENGTH = 80;
    const int MAX_WORD_LEN = 15;
    const int SPACING = 4;
    const int MARGIN = 4;
    const int QUIZ_TILE_SPACING = 5;
    const int QUIZ_TILE_MARGIN = 10;
}

#endif // DEFS_H
