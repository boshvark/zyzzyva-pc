//---------------------------------------------------------------------------
// WordValidator.cpp
//
// A validator for ensuring words are well-formed.
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

#include "WordValidator.h"

//-----------------------------------------------------------------------------
//  validate
//
//! Validate an input string.  Possibly modify the string to make it conform,
//! by upper-casing and removing all whitespace.
//
//! @param input the string to validate
//! @param pos the cursor position
//-----------------------------------------------------------------------------
QValidator::State
WordValidator::validate(QString& input, int& pos) const
{
    input = input.toUpper();
    QString re = "[^A-Z";
    if (options & AllowQuestionMarks)
        re += "?";
    if (options & AllowAsterisks)
        re += "*";
    if (options & AllowCharacterClasses)
        re += "\\[\\]^";
    if (options & AllowHooks)
        re += ":";
    re += "]+";
    if (options & AllowHooks) {
        replaceRegExp(QRegExp(" "), ":", input, pos);
        replaceRegExp(QRegExp(":+"), ":", input, pos);
        replaceRegExp(QRegExp("^([^:]*:[^:]+:[^:]*):.*"), "\\1", input, pos);
    }
    replaceRegExp(QRegExp(re), "", input, pos);
    return Acceptable;
}

//-----------------------------------------------------------------------------
//  replaceRegExp
//
//! Replace all sub strings matching the regular expression to the left and
//! right of the cursor.  Also adjusts the new cursor position accordingly,
//! so that the deletion/replacement of text does not make the cursor appear
//! to jump to a different spot.
//
//! @param re the regex to be replaced
//! @param str the string to replace each match
//! @param input the string to do the replacement within
//! @param pos the cursor position
//-----------------------------------------------------------------------------
void
WordValidator::replaceRegExp(const QRegExp& re, const QString& str,
                             QString& input, int& pos) const
{
    QString left = input.left(pos).replace(re, str);
    input = left + input.right(input.length() - pos).replace(re, str);
    pos = left.length();
}
