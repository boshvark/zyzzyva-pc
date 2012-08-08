//---------------------------------------------------------------------------
// SearchSpec.cpp
//
// A class to represent a word search specification.
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

#include "SearchSpec.h"
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

const int CURRENT_VERSION = 1;
const QString XML_TOP_ELEMENT = "zyzzyva-search";
const QString XML_VERSION_ATTR = "version";
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
            str += (conjunction ? QString(" AND ") : QString(" OR "));
        str += it.next().asString();
    }
    return str;
}

//---------------------------------------------------------------------------
//  asXml
//
//! Return an XML string representing the search spec.
//
//! @return the XML string representation
//---------------------------------------------------------------------------
QString
SearchSpec::asXml() const
{
    QDomImplementation implementation;
    QDomDocument document(implementation.createDocumentType(
                          "zyzzyva-search", QString(),
                          "http://boshvark.com/dtd/zyzzyva-search.dtd"));

    document.appendChild(asDomElement());

    //// XXX: There should be a programmatic way to write the <?xml?> header
    //// based on the QDomImplementation, shouldn't there?
    return QString("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n") +
        document.toString();
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
    QDomElement topElement = doc.createElement(XML_TOP_ELEMENT);
    topElement.setAttribute(XML_VERSION_ATTR, CURRENT_VERSION);

    if (conditions.empty())
        return topElement;

    QDomElement conditionsElement = doc.createElement(XML_CONDITIONS_ELEMENT);
    topElement.appendChild(conditionsElement);

    QString conjunctionType = (conjunction ? XML_CONJUNCTION_ELEMENT
                                           : XML_DISJUNCTION_ELEMENT);
    QDomElement conjunctionElement = doc.createElement(conjunctionType);
    conditionsElement.appendChild(conjunctionElement);

    QListIterator<SearchCondition> it (conditions);
    while (it.hasNext())
        conjunctionElement.appendChild(it.next().asDomElement());

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
SearchSpec::fromDomElement(const QDomElement& element)
{
    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    SearchSpec tmpSpec;

    if (element.hasAttribute(XML_VERSION_ATTR)) {
        bool ok;
        tmpSpec.version = element.attribute(XML_VERSION_ATTR).toInt(&ok);
        if (!ok)
            return false;
    }

    QDomElement elem = element.firstChild().toElement();
    if (elem.isNull() || (elem.tagName() != XML_CONDITIONS_ELEMENT))
        return false;

    elem = elem.firstChild().toElement();
    if (elem.isNull() || ((elem.tagName() != XML_CONJUNCTION_ELEMENT) &&
                          (elem.tagName() != XML_DISJUNCTION_ELEMENT)))
    {
        return false;
    }

    tmpSpec.conjunction = (elem.tagName() == XML_CONJUNCTION_ELEMENT);

    for (elem = elem.firstChild().toElement(); !elem.isNull();
         elem = elem.nextSibling().toElement())
    {
        SearchCondition condition;
        if (!condition.fromDomElement(elem))
            return false;
        tmpSpec.conditions.append(condition);
    }

    if (tmpSpec.conditions.empty())
        return false;

    tmpSpec.update();

    *this = tmpSpec;
    return true;
}

//---------------------------------------------------------------------------
//  optimize
//
//! Optimize the search specification for fast searches without making any
//! semantic changes.  Detect conflicts in certain specifications (e.g.
//! minimum length greater than maximum length), and detect constraints
//! implicit in certain specifications (e.g. Type I Sevens must have a length
//! of exactly 7 letters).
//
//! @param lexicon the lexicon for which this search is to be used
//---------------------------------------------------------------------------
void
SearchSpec::optimize(const QString& lexicon)
{
    QList<SearchCondition> newConditions;
    QList<SearchCondition> wildcardConditions;

    const int MAX_ANAGRAMS = 65535;
    QString mustInclude;
    QString mustExclude;
    int minLength = 0;
    int maxLength = MAX_WORD_LEN + 1;
    int minAnagrams = 0;
    int maxAnagrams = MAX_ANAGRAMS;
    int minNumVowels = 0;
    int maxNumVowels = MAX_WORD_LEN + 1;
    int minNumUniqueLetters = 0;
    int maxNumUniqueLetters = MAX_WORD_LEN + 1;
    int minPointValue = 0;
    int maxPointValue = 10 * MAX_WORD_LEN + 1;
    QMap<QString, bool> inLexicons;
    QMap<QString, bool> pos;
    inLexicons[lexicon] = true;

    QMutableListIterator<SearchCondition> it (conditions);
    while (it.hasNext()) {

        SearchCondition& condition = it.next();
        int minValue = condition.minValue;
        int maxValue = condition.maxValue;
        QString stringValue = condition.stringValue;
        bool negated = condition.negated;
        switch (condition.type) {

            case SearchCondition::PatternMatch:
            case SearchCondition::AnagramMatch:
            case SearchCondition::SubanagramMatch: {

                condition.stringValue = stringValue =
                    Auxil::getCanonicalSearchString(stringValue);
                if (stringValue.contains("*")) {
                    newConditions.append(condition);
                    break;
                }

                bool wildcard = (stringValue.contains("[") ||
                                 stringValue.contains("?"));

                if (!negated) {
                    int length = stringValue.length();
                    if (stringValue.contains("[")) {
                        int subtract = 0;
                        bool inCharClass = false;
                        for (int i = 0; i < length; ++i) {
                            QChar c = stringValue.at(i);
                            if (c == '[')
                                inCharClass = true;
                            else if (c == ']') {
                                inCharClass = false;
                                ++subtract;
                            }
                            else if (inCharClass)
                                ++subtract;
                        }
                        length -= subtract;
                    }

                    if ((condition.type == SearchCondition::PatternMatch) ||
                        (condition.type == SearchCondition::AnagramMatch))
                    {
                        if ((length < minLength) || (length > maxLength)) {
                            conditions.clear();
                            return;
                        }
                        minLength = maxLength = length;
                    }
                    else {
                        if (length < minLength) {
                            conditions.clear();
                            return;
                        }
                        else if (length < maxLength)
                            maxLength = length;
                    }
                }

                if (wildcard)
                    wildcardConditions.append(condition);
                else
                    newConditions.append(condition);
            }
            break;

            case SearchCondition::Length:
            if (minValue > minLength)
                minLength = minValue;
            if (maxValue < maxLength)
                maxLength = maxValue;
            if ((minLength > MAX_WORD_LEN) || (maxLength <= 0) ||
                (minLength > maxLength))
            {
                conditions.clear();
                return;
            }
            break;

            case SearchCondition::IncludeLetters:
            condition.stringValue = stringValue =
                Auxil::getCanonicalSearchString(stringValue);
            if (negated) {
                if (!mustInclude.isEmpty()) {
                    for (int i = 0; i < int(stringValue.length()); ++i) {
                        if (mustInclude.contains(stringValue.at(i))) {
                            conditions.clear();
                            return;
                        }
                    }
                }
                mustExclude += stringValue;
                newConditions.append(condition);
            }
            else {
                if (!mustExclude.isEmpty()) {
                    for (int i = 0; i < int(stringValue.length()); ++i) {
                        if (mustExclude.contains(stringValue.at(i))) {
                            conditions.clear();
                            return;
                        }
                    }
                }
                mustInclude += stringValue;
                newConditions.append(condition);
            }
            break;

            case SearchCondition::BelongToGroup: {
                SearchSet ss = Auxil::stringToSearchSet(stringValue);
                if (ss == UnknownSearchSet)
                    break;

                else if (!negated) {
                    SearchCondition addCondition;
                    switch (ss) {
                        case SetHighFives:
                        addCondition.type = SearchCondition::Length;
                        addCondition.minValue = 5;
                        addCondition.maxValue = 5;
                        break;

                        case SetTypeOneSevens:
                        case SetTypeTwoSevens:
                        case SetTypeThreeSevens:
                        addCondition.type = SearchCondition::Length;
                        addCondition.minValue = 7;
                        addCondition.maxValue = 7;
                        break;

                        case SetTypeOneEights:
                        case SetEightsFromSevenLetterStems:
                        addCondition.type = SearchCondition::Length;
                        addCondition.minValue = 8;
                        addCondition.maxValue = 8;
                        break;

                        default: break;
                    }

                    if (addCondition.type !=
                        SearchCondition::UnknownSearchType)
                    {
                        newConditions.append(addCondition);
                    }
                }
            }
            newConditions.append(condition);
            break;

            case SearchCondition::InLexicon:
            if (inLexicons.contains(stringValue) &&
                inLexicons[stringValue] == negated)
            {
                conditions.clear();
                return;
            }
            inLexicons[stringValue] = negated;
            newConditions.append(condition);
            break;

            case SearchCondition::PartOfSpeech:
            if (pos.contains(stringValue) &&
                pos[stringValue] != negated)
            {
                conditions.clear();
                return;
            }
            pos[stringValue] = negated;
            newConditions.append(condition);
            break;

            case SearchCondition::NumAnagrams:
            if (minValue > minAnagrams)
                minAnagrams = minValue;
            if (maxValue < maxAnagrams)
                maxAnagrams = maxValue;
            if ((minAnagrams > MAX_ANAGRAMS) || (maxAnagrams <= 0) ||
                (minAnagrams > maxAnagrams))
            {
                conditions.clear();
                return;
            }
            break;

            case SearchCondition::NumVowels:
            if (minValue > minNumVowels)
                minNumVowels = minValue;
            if (maxValue < maxNumVowels)
                maxNumVowels = maxValue;
            if ((minNumVowels > MAX_WORD_LEN) || (maxNumVowels < 0) ||
                (minNumVowels > maxNumVowels))
            {
                conditions.clear();
                return;
            }
            break;

            case SearchCondition::NumUniqueLetters:
            if (minValue > minNumUniqueLetters)
                minNumUniqueLetters = minValue;
            if (maxValue < maxNumUniqueLetters)
                maxNumUniqueLetters = maxValue;
            if ((minNumUniqueLetters > MAX_WORD_LEN) ||
                (maxNumUniqueLetters <= 0) ||
                (minNumUniqueLetters > maxNumUniqueLetters))
            {
                conditions.clear();
                return;
            }
            break;

            case SearchCondition::PointValue:
            if (minValue > minPointValue)
                minPointValue = minValue;
            if (maxValue < maxPointValue)
                maxPointValue = maxValue;
            if ((minPointValue > 10 * MAX_WORD_LEN) || (maxPointValue <= 0) ||
                (minPointValue > maxPointValue))
            {
                conditions.clear();
                return;
            }
            break;

            default:
            newConditions.append(condition);
            break;
        }
    }

    // Sanity checks for impossible conditions
    if ((minNumVowels > maxLength) || (minNumUniqueLetters > maxLength) ||
        (minPointValue > (10 * maxLength)) || (maxPointValue < minLength))
    {
        conditions.clear();
        return;
    }

    SearchCondition condition;

    // Add Number of Anagrams conditions
    if ((minAnagrams > 0) || (maxAnagrams < MAX_ANAGRAMS)) {
        condition.type = SearchCondition::NumAnagrams;
        condition.minValue = minAnagrams;
        condition.maxValue = maxAnagrams;
        newConditions.push_front(condition);
    }

    // Add Point Value conditions
    if ((minPointValue > 0) || (maxPointValue < (10 * MAX_WORD_LEN + 1))) {
        condition.type = SearchCondition::PointValue;
        condition.minValue = minPointValue;
        condition.maxValue = maxPointValue;
        newConditions.push_front(condition);
    }

    // Add Number of Unique Letters conditions
    if ((minNumUniqueLetters > 0) ||
        (maxNumUniqueLetters < (MAX_WORD_LEN + 1)))
    {
        condition.type = SearchCondition::NumUniqueLetters;
        condition.minValue = minNumUniqueLetters;
        condition.maxValue = maxNumUniqueLetters;
        newConditions.push_front(condition);
    }

    // Add Number of Vowels conditions
    if ((minNumVowels > 0) || (maxNumVowels < (MAX_WORD_LEN + 1))) {
        condition.type = SearchCondition::NumVowels;
        condition.minValue = minNumVowels;
        condition.maxValue = maxNumVowels;
        newConditions.push_front(condition);
    }

    // Add Length conditions
    if ((minLength > 0) || (maxLength < (MAX_WORD_LEN + 1))) {
        condition.type = SearchCondition::Length;
        condition.minValue = minLength;
        condition.maxValue = maxLength;
        newConditions.push_front(condition);
    }

    conditions = wildcardConditions + newConditions;
}


//---------------------------------------------------------------------------
//  update
//
//! Update older search specs so they are current.  This process includes
//! replacing obsolete search conditions with equivalent current ones.  For
//! example, the meaning of the Probability Order condition has changed - its
//! old meaning is now represented by the Limit By Probability Order
//! condition.
//---------------------------------------------------------------------------
void
SearchSpec::update()
{
    // 0: Change Probability Order conditions to Limit By Probability Order
    if (version == 0) {
        QMutableListIterator<SearchCondition> it (conditions);
        while (it.hasNext()) {
            SearchCondition& condition = it.next();
            if (condition.type == SearchCondition::ProbabilityOrder) {
                condition.type = SearchCondition::LimitByProbabilityOrder;
                condition.legacy = true;
            }
        }
        ++version;
    }
}
