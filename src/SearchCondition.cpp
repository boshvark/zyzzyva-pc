//---------------------------------------------------------------------------
// SearchCondition.cpp
//
// A class to represent a word search condition.
//
// Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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
const QString XML_TYPE_ATTR = "type";
const QString XML_STRING_ATTR = "string";
const QString XML_NUMBER_ATTR = "number";
const QString XML_PERCENT_ATTR = "percent";

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
        case TakesPrefix:
        case DoesNotTakePrefix:
        case TakesSuffix:
        case DoesNotTakeSuffix:
        case MustInclude:
        case MustExclude:
        case MustBelong:
        str += stringValue;
        break;

        case InWordList:
        case NotInWordList:
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
        str += QString::number (intValue) + "% " + stringValue;
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

    topElement.setAttribute (XML_TYPE_ATTR, Auxil::searchTypeToString (type));

    switch (type) {
        case PatternMatch:
        case AnagramMatch:
        case SubanagramMatch:
        case TakesPrefix:
        case DoesNotTakePrefix:
        case TakesSuffix:
        case DoesNotTakeSuffix:
        case MustInclude:
        case MustExclude:
        case MustBelong:
        case InWordList:
        case NotInWordList:
        topElement.setAttribute (XML_STRING_ATTR, stringValue);
        break;

        case ExactLength:
        case MinLength:
        case MaxLength:
        case ExactAnagrams:
        case MinAnagrams:
        case MaxAnagrams:
        topElement.setAttribute (XML_NUMBER_ATTR, intValue);
        break;

        case MinProbability:
        case MaxProbability:
        // XXX: Multiply by the correct factor here!
        topElement.setAttribute (XML_NUMBER_ATTR, (intValue * 1));
        break;

        case MustConsist:
        topElement.setAttribute (XML_PERCENT_ATTR, intValue);
        topElement.setAttribute (XML_STRING_ATTR, stringValue);
        break;

        default: break;
    }

    return topElement;
}

//---------------------------------------------------------------------------
//  fromDomElement
//
//! Reset the object based on the contents of a DOM element representing a
//! search condition.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
SearchCondition::fromDomElement (const QDomElement& element)
{
    if ((element.tagName() != XML_TOP_ELEMENT) ||
        (!element.hasAttribute (XML_TYPE_ATTR)))
    {
        return false;
    }

    SearchCondition tmpCondition;
    tmpCondition.type = Auxil::stringToSearchType (element.attribute
                                                   (XML_TYPE_ATTR));
    if (tmpCondition.type == UnknownSearchType)
        return false;

    bool ok = false;

    switch (tmpCondition.type) {
        case PatternMatch:
        case AnagramMatch:
        case SubanagramMatch:
        case TakesPrefix:
        case DoesNotTakePrefix:
        case TakesSuffix:
        case DoesNotTakeSuffix:
        case MustInclude:
        case MustExclude:
        case MustBelong:
        case InWordList:
        case NotInWordList:
        if (!element.hasAttribute (XML_STRING_ATTR))
            return false;
        tmpCondition.stringValue = element.attribute (XML_STRING_ATTR);
        break;

        case ExactLength:
        case MinLength:
        case MaxLength:
        case ExactAnagrams:
        case MinAnagrams:
        case MaxAnagrams:
        if (!element.hasAttribute (XML_NUMBER_ATTR))
            return false;
        tmpCondition.intValue =
            element.attribute (XML_NUMBER_ATTR).toInt (&ok);
        if (!ok)
            return false;
        break;

        case MinProbability:
        case MaxProbability:
        if (!element.hasAttribute (XML_NUMBER_ATTR))
            return false;
        tmpCondition.intValue =
            element.attribute (XML_NUMBER_ATTR).toInt (&ok);
        if (!ok)
            return false;
        // XXX: Divide by the correct factor here!
        tmpCondition.intValue /= 1;
        break;

        case MustConsist:
        if (!element.hasAttribute (XML_PERCENT_ATTR) ||
            !element.hasAttribute (XML_STRING_ATTR))
        {
            return false;
        }
        tmpCondition.intValue =
            element.attribute (XML_PERCENT_ATTR).toInt (&ok);
        if (!ok)
            return false;
        tmpCondition.stringValue = element.attribute (XML_STRING_ATTR);
        break;

        default: break;
    }

    *this = tmpCondition;
    return true;
}
