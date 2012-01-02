//---------------------------------------------------------------------------
// QuizTimerSpec.cpp
//
// A class to represent a quiz timer specification.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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

#include "QuizTimerSpec.h"

const QString PER_QUESTION_TYPE = "per question";
const QString PER_RESPONSE_TYPE = "per response";
const QString XML_TOP_ELEMENT = "timer";
const QString XML_TYPE_ATTR = "type";
const QString XML_DURATION_ATTR = "duration";

//---------------------------------------------------------------------------
//  asString
//
//! Return a string representing the search spec.
//
//! @return the string representation
//---------------------------------------------------------------------------
QString
QuizTimerSpec::asString() const
{
    return (type == NoTimer) ? QString()
                             : "Timer: " + QString::number(duration) +
                               " seconds " + typeToString(type);
}

//---------------------------------------------------------------------------
//  asDomElement
//
//! Return a DOM element representing the quiz timer spec.
//
//! @return the DOM element
//---------------------------------------------------------------------------
QDomElement
QuizTimerSpec::asDomElement() const
{
    QDomDocument doc;
    QDomElement topElement = doc.createElement(XML_TOP_ELEMENT);

    if (type == NoTimer)
        return topElement;

    topElement.setAttribute(XML_TYPE_ATTR, typeToString(type));
    topElement.setAttribute(XML_DURATION_ATTR, duration);

    return topElement;
}

//---------------------------------------------------------------------------
//  fromDomElement
//
//! Reset the object based on the contents of a DOM element representing a
//! quiz timer spec.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
QuizTimerSpec::fromDomElement(const QDomElement& element)
{
    QuizTimerSpec tmpSpec;

    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    if (element.hasAttribute(XML_TYPE_ATTR)) {
        tmpSpec.type = stringToType(element.attribute(XML_TYPE_ATTR));

        if (!element.hasAttribute(XML_DURATION_ATTR))
            return false;

        bool ok = false;
        tmpSpec.duration = element.attribute(XML_DURATION_ATTR).toInt(&ok);
        if (!ok)
            return false;
    }

    *this = tmpSpec;
    return true;
}

//---------------------------------------------------------------------------
//  typeToString
//
//! Convert a timer type to a string representation.
//
//! @param t the timer type
//! @return the string representation
//---------------------------------------------------------------------------
QString
QuizTimerSpec::typeToString(QuizTimerType t) const
{
    switch (t) {
        case PerQuestion: return PER_QUESTION_TYPE;
        case PerResponse: return PER_RESPONSE_TYPE;
        default: return QString();
    }
}

//---------------------------------------------------------------------------
//  stringToType
//
//! Convert a string representation to a timer type.
//
//! @param s the string representation
//! @return the timer type
//---------------------------------------------------------------------------
QuizTimerType
QuizTimerSpec::stringToType(const QString& s) const
{
    if (s == PER_QUESTION_TYPE)
        return PerQuestion;
    else if (s == PER_RESPONSE_TYPE)
        return PerResponse;
    else
        return NoTimer;
}
