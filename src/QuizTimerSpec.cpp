//---------------------------------------------------------------------------
// QuizTimerSpec.cpp
//
// A class to represent a quiz timer specification.
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

#include "QuizTimerSpec.h"

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
    if (type == NoTimer)
        return QString::null;

    QString str = "Timer: " + QString::number (duration) + " seconds";
    switch (type) {
        case PerQuestion: str += " per question"; break;
        case PerResponse: str += " per response"; break;
        default: break;
    }
    return str;
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
    QDomElement element;
    return element;
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
QuizTimerSpec::fromDomElement (const QDomElement& element)
{
    qDebug ("QuizTimerSpec::fromDomElement");

    //if ((element.tagName() != XML_TOP_ELEMENT) ||
    //    (!element.hasAttribute (XML_
    //    return false;

    //QDomElement elem = element.firstChild().toElement();
    //if (elem.isNull())
    //    return false;

    //QuizTimerSpec tmpSpec;

    //for (; !elem.isNull(); elem = elem.nextSibling().toElement()) {
    //    qDebug ("Element: " + elem.tagName());
    //}

    //*this = tmpSpec;
    return true;
}
