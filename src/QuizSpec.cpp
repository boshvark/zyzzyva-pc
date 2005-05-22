//---------------------------------------------------------------------------
// QuizSpec.cpp
//
// A class to represent a word search specification.
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

#include "QuizSpec.h"
#include "Defs.h"

using namespace Defs;

//---------------------------------------------------------------------------
//  asString
//
//! Return a string representing the search spec.
//
//! @return the string representation
//---------------------------------------------------------------------------
QString
QuizSpec::asString() const
{
    QString str;
    switch (type) {
        case QuizPatterns: str += "Pattern Match Quiz: "; break;
        case QuizAnagrams: str += "Anagram Quiz: "; break;
        case QuizSubanagrams: str += "Subanagram Quiz: "; break;
        case QuizHooks: str += "Hook Quiz: "; break;
        case QuizAnagramHooks: str += "Quiz Anagram Hooks: "; break;
        case QuizAnagramHookMnemonics: str += "Quiz Mnemonics: "; break;
        default: break;
    }
    str += searchSpec.asString();
    return str;
}
