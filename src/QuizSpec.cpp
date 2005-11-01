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
const QString XML_TOP_TYPE_ATTR = "type";
const QString XML_QUESTION_SOURCE_ELEMENT = "question-source";
const QString XML_QUESTION_SOURCE_TYPE_ATTR = "type";
const QString XML_QUESTION_SOURCE_SINGLE_QUESTION_ATTR = "single-question";
const QString XML_SEARCH_ELEMENT = "zyzzyva-search";
const QString XML_RANDOMIZER_ELEMENT = "randomizer";
const QString XML_RANDOMIZER_SEED_ATTR = "seed";
const QString XML_RANDOMIZER_ALGORITHM_ATTR = "algorithm";
const QString XML_TIMER_ELEMENT = "timer";
const QString XML_TIMER_TIMEOUT_ATTR = "timeout";
const QString XML_TIMER_PERIOD_ATTR = "period";
const QString XML_PROGRESS_ELEMENT = "progress";

const QString PATTERNS_TYPE = "Patterns";
const QString ANAGRAMS_TYPE = "Anagrams";
const QString SUBANAGRAMS_TYPE = "Subanagrams";
const QString ANAGRAM_JUMBLE_TYPE = "Anagram Jumble";
const QString SUBANAGRAM_JUMBLE_TYPE = "Subanagram Jumble";
const QString HOOKS_TYPE = "Hooks";
const QString ANAGRAM_HOOKS_TYPE = "Anagram Hooks";
const QString ANAGRAM_HOOK_MNEMONICS_TYPE = "Anagram Hook Mnemonics";

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
        case QuizAnagramJumble: str += "Anagram Jumble Quiz: "; break;
        case QuizSubanagramJumble: str += "Subanagram Jumble Quiz: "; break;
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
    QDomDocument doc;
    QDomElement topElement = doc.createElement (XML_TOP_ELEMENT);
    topElement.setAttribute (XML_TOP_TYPE_ATTR, typeToString (type));

    QDomElement sourceElement = doc.createElement
        (XML_QUESTION_SOURCE_ELEMENT);
    sourceElement.setAttribute (XML_QUESTION_SOURCE_TYPE_ATTR, "search");
    sourceElement.setAttribute (XML_QUESTION_SOURCE_SINGLE_QUESTION_ATTR,
                                (useList ? QString ("true")
                                         : QString ("false")));

    sourceElement.appendChild (searchSpec.asDomElement());
    topElement.appendChild (sourceElement);

    if (randomOrder) {
        QDomElement randomElement = doc.createElement
            (XML_RANDOMIZER_ELEMENT);
        randomElement.setAttribute (XML_RANDOMIZER_SEED_ATTR, randomSeed);
        randomElement.setAttribute (XML_RANDOMIZER_ALGORITHM_ATTR,
                                    randomAlgorithm);
        topElement.appendChild (randomElement);
    }

    if (timerSpec.getType() != NoTimer)
        topElement.appendChild (timerSpec.asDomElement());

    topElement.appendChild (progress.asDomElement());

    return topElement;
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
    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    QDomElement elem = element.firstChild().toElement();
    if (elem.isNull())
        return false;

    QuizSpec tmpSpec;
    tmpSpec.setRandomOrder (false);

    for (; !elem.isNull(); elem = elem.nextSibling().toElement()) {

        QString tag = elem.tagName();
        // XXX: QuizQuestionSource needs to be a class of its own
        if (tag == XML_QUESTION_SOURCE_ELEMENT) {

            if ((!elem.hasAttribute (XML_QUESTION_SOURCE_TYPE_ATTR)) ||
                (elem.attribute (XML_QUESTION_SOURCE_TYPE_ATTR) != "search"))
            {
                return false;
            }

            if (elem.attribute (XML_QUESTION_SOURCE_SINGLE_QUESTION_ATTR) ==
                "true")
            {
                tmpSpec.setUseList (true);
            }

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
            tmpSpec.setRandomOrder (true);
            if (!elem.hasAttribute (XML_RANDOMIZER_SEED_ATTR) ||
                !elem.hasAttribute (XML_RANDOMIZER_ALGORITHM_ATTR))
            {
                return false;
            }
            bool ok = false;
            unsigned int tmpSeed = elem.attribute
                (XML_RANDOMIZER_SEED_ATTR).toUInt (&ok);
            if (!ok)
                return false;
            tmpSpec.setRandomSeed (tmpSeed);
            int tmpAlgorithm = elem.attribute
                (XML_RANDOMIZER_ALGORITHM_ATTR).toInt (&ok);
            if (!ok)
                return false;
            tmpSpec.setRandomAlgorithm (tmpAlgorithm);
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

//---------------------------------------------------------------------------
//  typeToString
//
//! Convert a timer type to a string representation.
//
//! @param t the timer type
//! @return the string representation
//---------------------------------------------------------------------------
QString
QuizSpec::typeToString (QuizType t) const
{
    switch (t) {
        case QuizPatterns: return PATTERNS_TYPE;
        case QuizAnagrams: return ANAGRAMS_TYPE;
        case QuizSubanagrams: return SUBANAGRAMS_TYPE;
        case QuizAnagramJumble: return ANAGRAM_JUMBLE_TYPE;
        case QuizSubanagramJumble: return SUBANAGRAM_JUMBLE_TYPE;
        case QuizHooks: return HOOKS_TYPE;
        case QuizAnagramHooks: return ANAGRAM_HOOKS_TYPE;
        case QuizAnagramHookMnemonics: return ANAGRAM_HOOK_MNEMONICS_TYPE;
        default: return QString::null;
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
QuizType
QuizSpec::stringToType (const QString& s) const
{
    if (s == PATTERNS_TYPE)
        return QuizPatterns;
    else if (s == ANAGRAMS_TYPE)
        return QuizAnagrams;
    else if (s == SUBANAGRAMS_TYPE)
        return QuizSubanagrams;
    else if (s == ANAGRAM_JUMBLE_TYPE)
        return QuizAnagramJumble;
    else if (s == SUBANAGRAM_JUMBLE_TYPE)
        return QuizSubanagramJumble;
    else if (s == HOOKS_TYPE)
        return QuizHooks;
    else if (s == ANAGRAM_HOOKS_TYPE)
        return QuizAnagramHooks;
    else if (s == ANAGRAM_HOOK_MNEMONICS_TYPE)
        return QuizAnagramHookMnemonics;
    else
        return UnknownQuizType;
}
