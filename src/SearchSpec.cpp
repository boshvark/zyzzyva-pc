//---------------------------------------------------------------------------
// SearchSpec.cpp
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

#include "SearchSpec.h"
#include "Auxil.h"
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
SearchSpec::asString() const
{
    QString str;
    if (pattern.length()) {
        switch (type) {
            case Pattern: str += "Pattern: "; break;
            case Anagram: str += "Anagram: "; break;
            case Subanagram: str += "Subanagram: "; break;
            default: break;
        }
        str += pattern;
    }

    if (includeLetters.length()) {
        if (str.length())
            str += ", ";
        str += "Include: " + includeLetters;
    }

    if (excludeLetters.length()) {
        if (str.length())
            str += ", ";
        str += "Exclude: " + excludeLetters;
    }

    if (consistPercent > 0) {
        if (str.length())
            str += ", ";
        str += "Consist: " + QString::number (consistPercent) + "% " +
            consistLetters;
    }

    if (minLength > 1) {
        if (str.length())
            str += ", ";
        str += "Min Length: " + QString::number (minLength);
    }

    if (maxLength < MAX_WORD_LEN) {
        if (str.length())
            str += ", ";
        str += "Max Length: " + QString::number (maxLength);
    }

    if (!setMemberships.empty()) {
        if (str.length())
            str += ", ";
        str += "Set Membership: ";
        std::set<SearchSet>::const_iterator it;
        for (it = setMemberships.begin(); it != setMemberships.end(); ++it) {
            if (it != setMemberships.begin())
                str += ", ";
            str += Auxil::searchSetToString (*it);
        }
    }
    return str;
}
