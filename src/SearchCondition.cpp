//---------------------------------------------------------------------------
// SearchCondition.cpp
//
// A class to represent a word search condition.
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

#include "SearchCondition.h"
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

const QString XML_TOP_ELEMENT = "condition";
const QString TYPE_ATTR = "type";
const QString STRING_ATTR = "string";
const QString NUMBER_ATTR = "number";
const QString PERCENT_ATTR = "percent";

//---------------------------------------------------------------------------
//  asString
//
//! Return a string representing the search spec.
//
//! @return the string representation
//---------------------------------------------------------------------------
QString
SearchCondition::asString() const
{
    if (type == UnknownSearchType)
        return QString::null;

    QString str = Auxil::searchTypeToString (type) + ": ";

    switch (type) {
        case PatternMatch:
        case AnagramMatch:
        case SubanagramMatch:
        case MustInclude:
        case MustExclude:
        case MustBelong:
        str += stringValue;
        break;

        case InWordList:
        str += "...";
        break;

        case ExactLength:
        case MinLength:
        case MaxLength:
        case ExactAnagrams:
        case MinAnagrams:
        case MaxAnagrams:
        str += QString::number (intValue);
        break;

        case MinProbability:
        case MaxProbability:
        // XXX: Multiply by the correct factor here!
        str += QString::number (intValue * 1);
        break;

        case MustConsist:
        str += QString::number (intValue) + "%% " + stringValue;
        break;

        default: break;
    }

    return str;
}

//---------------------------------------------------------------------------
//  asDomElement
//
//! Return a DOM element representing the search spec.
//
//! @return the DOM element
//---------------------------------------------------------------------------
QDomElement
SearchCondition::asDomElement() const
{
    QDomDocument doc;
    QDomElement topElement = doc.createElement (XML_TOP_ELEMENT);

    if (type == UnknownSearchType)
        return topElement;

    topElement.setAttribute (TYPE_ATTR, Auxil::searchTypeToString (type));

    switch (type) {
        case PatternMatch:
        case AnagramMatch:
        case SubanagramMatch:
        case MustInclude:
        case MustExclude:
        case MustBelong:
        case InWordList:
        topElement.setAttribute (STRING_ATTR, stringValue);
        break;

        case ExactLength:
        case MinLength:
        case MaxLength:
        case ExactAnagrams:
        case MinAnagrams:
        case MaxAnagrams:
        topElement.setAttribute (NUMBER_ATTR, intValue);
        break;

        case MinProbability:
        case MaxProbability:
        // XXX: Multiply by the correct factor here!
        topElement.setAttribute (NUMBER_ATTR, (intValue * 1));
        break;

        case MustConsist:
        topElement.setAttribute (PERCENT_ATTR, intValue);
        topElement.setAttribute (STRING_ATTR, stringValue);
        break;

        default: break;
    }

    return topElement;
}
