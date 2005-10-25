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

const QString XML_TOP_ELEMENT = "zyzzyva-search";
const QString XML_CONDITIONS_ELEMENT = "conditions";
const QString XML_CONJUNCTION_ELEMENT = "and";
const QString XML_DISJUNCTION_ELEMENT = "or";

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
    QListIterator<SearchCondition> it (conditions);
    while (it.hasNext()) {
        if (!str.isEmpty())
            str += (conjunction ? QString (" AND ") : QString (" OR "));
        str += it.next().asString();
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
SearchSpec::asDomElement() const
{
    QDomDocument doc;
    QDomElement topElement = doc.createElement (XML_TOP_ELEMENT);

    if (conditions.empty())
        return topElement;

    QDomElement conditionsElement = doc.createElement
        (XML_CONDITIONS_ELEMENT);
    topElement.appendChild (conditionsElement);

    QString conjunctionType = (conjunction ? XML_CONJUNCTION_ELEMENT
                                           : XML_DISJUNCTION_ELEMENT);
    QDomElement conjunctionElement = doc.createElement (conjunctionType);
    conditionsElement.appendChild (conjunctionElement);

    QListIterator<SearchCondition> it (conditions);
    while (it.hasNext())
        conjunctionElement.appendChild (it.next().asDomElement());

    return topElement;
}

//---------------------------------------------------------------------------
//  fromDomElement
//
//! Reset the object based on the contents of a DOM element representing a
//! search spec.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
SearchSpec::fromDomElement (const QDomElement& element)
{
    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    QDomElement elem = element.firstChild().toElement();
    if (elem.isNull() || (elem.tagName() != XML_CONDITIONS_ELEMENT))
        return false;

    elem = elem.firstChild().toElement();
    if (elem.isNull() || ((elem.tagName() != XML_CONJUNCTION_ELEMENT) &&
                          (elem.tagName() != XML_DISJUNCTION_ELEMENT)))
    {
        return false;
    }

    SearchSpec tmpSpec;
    tmpSpec.conjunction = (elem.tagName() == XML_CONJUNCTION_ELEMENT);

    for (elem = elem.firstChild().toElement(); !elem.isNull();
         elem = elem.nextSibling().toElement())
    {
        SearchCondition condition;
        if (!condition.fromDomElement (elem))
            return false;
        tmpSpec.conditions << condition;
    }

    if (tmpSpec.conditions.empty())
        return false;

    *this = tmpSpec;
    return true;
}

//---------------------------------------------------------------------------
//  optimize
//
//! Optimize the search specification for fast searches without making any
//! semantic changes.  Detect conflicts is certain specifications (e.g.
//! minimum length greater than maximum length), and detect constraints
//! implicit in certain specifications (e.g. Type I Sevens must have a length
//! of exactly 7 letters).
//---------------------------------------------------------------------------
void
SearchSpec::optimize()
{
    QList<SearchCondition> newConditions;

    const int MAX_ANAGRAMS = 65535;
    QString mustInclude;
    QString mustExclude;
    int minLength = 0;
    int maxLength = MAX_WORD_LEN;
    int minAnagrams = 0;
    int maxAnagrams = MAX_ANAGRAMS;

    QListIterator<SearchCondition> it (conditions);
    while (it.hasNext()) {

        const SearchCondition& condition = it.next();
        int intValue = condition.intValue;
        QString stringValue = condition.stringValue;
        switch (condition.type) {

            case SearchCondition::ExactLength:
            if ((intValue < minLength) || (intValue > maxLength)) {
                conditions.clear();
                return;
            }
            minLength = maxLength = intValue;
            break;

            case SearchCondition::MinLength:
            if (intValue > minLength)
                minLength = intValue;
            if ((minLength > MAX_WORD_LEN) || (minLength > maxLength)) {
                conditions.clear();
                return;
            }
            break;

            case SearchCondition::MaxLength:
            if (intValue < maxLength)
                maxLength = intValue;
            if ((maxLength <= 0) || (maxLength < minLength)) {
                conditions.clear();
                return;
            }
            break;

            case SearchCondition::MustInclude:
            if (!mustExclude.isEmpty()) {
                for (int i = 0; i < int (stringValue.length()); ++i) {
                    if (mustExclude.contains (stringValue.at (i))) {
                        conditions.clear();
                        return;
                    }
                }
            }
            mustInclude += stringValue;
            newConditions.append (condition);
            break;

            case SearchCondition::MustExclude:
            if (!mustInclude.isEmpty()) {
                for (int i = 0; i < int (stringValue.length()); ++i) {
                    if (mustInclude.contains (stringValue.at (i))) {
                        conditions.clear();
                        return;
                    }
                }
            }
            mustExclude += stringValue;
            newConditions.append (condition);
            break;

            case SearchCondition::MustBelong: {
                SearchSet ss = Auxil::stringToSearchSet (stringValue);
                if (ss == UnknownSearchSet)
                    break;
                SearchCondition condition;
                switch (ss) {
                    case SetTypeOneSevens:
                    case SetTypeTwoSevens:
                    case SetTypeThreeSevens:
                    condition.type = SearchCondition::ExactLength;
                    condition.intValue = 7;
                    conditions << condition;
                    break;

                    case SetTypeOneEights:
                    case SetEightsFromSevenLetterStems:
                    condition.type = SearchCondition::ExactLength;
                    condition.intValue = 8;
                    conditions << condition;
                    break;

                    default: break;
                }

                newConditions.append (condition);
            }
            break;

            case SearchCondition::ExactAnagrams:
            if ((intValue < minAnagrams) || (intValue > maxAnagrams)) {
                conditions.clear();
                return;
            }
            minAnagrams = maxAnagrams = intValue;
            break;

            case SearchCondition::MinAnagrams:
            if (intValue > minAnagrams)
                minAnagrams = intValue;
            if (minAnagrams > maxAnagrams) {
                conditions.clear();
                return;
            }
            break;

            case SearchCondition::MaxAnagrams:
            if (intValue < maxAnagrams)
                maxAnagrams = intValue;
            if ((maxAnagrams <= 0) || (maxAnagrams < minAnagrams)) {
                conditions.clear();
                return;
            }
            break;

            default:
            newConditions.append (condition);
            break;
        }
    }


    SearchCondition condition;

    // Add minimum number of anagram conditions
    if ((minAnagrams > 0) && (minAnagrams == maxAnagrams)) {
        condition.type = SearchCondition::ExactAnagrams;
        condition.intValue = minAnagrams;
        newConditions.push_front (condition);
    }
    else {
        if (minAnagrams > 0) {
            condition.type = SearchCondition::MinAnagrams;
            condition.intValue = minAnagrams;
            newConditions.push_front (condition);
        }
        if (maxAnagrams < MAX_ANAGRAMS) {
            condition.type = SearchCondition::MaxAnagrams;
            condition.intValue = maxAnagrams;
            newConditions.push_front (condition);
        }
    }

    // Add minimum number of length newConditions
    if ((minLength > 0) && (minLength == maxLength)) {
        condition.type = SearchCondition::ExactLength;
        condition.intValue = minLength;
        newConditions.push_front (condition);
    }
    else {
        if (minLength > 0) {
            condition.type = SearchCondition::MinLength;
            condition.intValue = minLength;
            newConditions.push_front (condition);
        }
        if (maxLength < MAX_WORD_LEN) {
            condition.type = SearchCondition::MaxLength;
            condition.intValue = maxLength;
            newConditions.push_front (condition);
        }
    }

    conditions = newConditions;
}

