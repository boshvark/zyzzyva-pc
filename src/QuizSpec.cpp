//---------------------------------------------------------------------------
// QuizSpec.cpp
//
// A class to represent a quiz specification.
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

// XXX: The Search and Progress elements are redundant with strings defined in
// SearchSpec.cpp and QuizProgress.cpp.  Define them in only one place!
const QString XML_TOP_ELEMENT = "zyzzyva-quiz";
const QString XML_QUESTION_SOURCE_ELEMENT = "question-source";
const QString XML_SEARCH_ELEMENT = "zyzzyva-search";
const QString XML_RANDOMIZER_ELEMENT = "randomizer";
const QString XML_RANDOMIZER_SEED_ATTR = "seed";
const QString XML_RANDOMIZER_ALGORITHM_ATTR = "algorithm";
const QString XML_TIMER_ELEMENT = "timer";
const QString XML_TIMER_TIMEOUT_ATTR = "timeout";
const QString XML_TIMER_PERIOD_ATTR = "period";
const QString XML_PROGRESS_ELEMENT = "progress";

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

//---------------------------------------------------------------------------
//  asDomElement
//
//! Return a DOM element representing the quiz spec.
//
//! @return the DOM element
//---------------------------------------------------------------------------
QDomElement
QuizSpec::asDomElement() const
{
    QDomElement element;
    return element;
}

//---------------------------------------------------------------------------
//  fromDomElement
//
//! Reset the object based on the contents of a DOM element representing a
//! quiz spec.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
QuizSpec::fromDomElement (const QDomElement& element)
{
    qDebug ("QuizSpec::fromDomElement");

    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    QDomElement elem = element.firstChild().toElement();
    if (elem.isNull())
        return false;

    QuizSpec tmpSpec;

    for (; !elem.isNull(); elem = elem.nextSibling().toElement()) {
        qDebug ("Element: " + elem.tagName());

        QString tag = elem.tagName();
        if (tag == XML_QUESTION_SOURCE_ELEMENT) {
            QDomElement searchElem = elem.firstChild().toElement();
            if (searchElem.tagName() != XML_SEARCH_ELEMENT)
                return false;
            SearchSpec tmpSearchSpec;
            if (!tmpSearchSpec.fromDomElement (searchElem))
                return false;
            tmpSpec.setSearchSpec (tmpSearchSpec);
        }

        else if (tag == XML_TIMER_ELEMENT) {
            QuizTimerSpec tmpTimerSpec;
            if (!tmpTimerSpec.fromDomElement (elem))
                return false;
            tmpSpec.setTimerSpec (tmpTimerSpec);
        }

        else if (tag == XML_RANDOMIZER_ELEMENT) {
        }

        else if (tag == XML_PROGRESS_ELEMENT) {
            QuizProgress tmpProgress;
            if (!tmpProgress.fromDomElement (elem))
                return false;
            tmpSpec.setProgress (tmpProgress);
        }

        else
            return false;
    }

    *this = tmpSpec;
    return true;
}
