//---------------------------------------------------------------------------
// QuizSpec.cpp
//
// A class to represent a quiz specification.
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

#include "QuizSpec.h"
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

// XXX: The Search and Progress elements are redundant with strings defined in
// SearchSpec.cpp and QuizProgress.cpp.  Define them in only one place!
const QString XML_TOP_ELEMENT = "zyzzyva-quiz";
const QString XML_TOP_TYPE_ATTR = "type";
const QString XML_TOP_METHOD_ATTR = "method";
const QString XML_TOP_QUESTION_ORDER_ATTR = "question-order";
const QString XML_TOP_PROB_NUM_BLANKS_ATTR = "probability-num-blanks";
const QString XML_TOP_LEXICON_ATTR = "lexicon";
const QString XML_QUESTION_SOURCE_ELEMENT = "question-source";
const QString XML_QUESTION_SOURCE_TYPE_ATTR = "type";
const QString XML_QUESTION_SOURCE_SINGLE_QUESTION_ATTR = "single-question";
const QString XML_SEARCH_ELEMENT = "zyzzyva-search";
const QString XML_RANDOMIZER_ELEMENT = "randomizer";
const QString XML_RANDOMIZER_SEED_ATTR = "seed";
const QString XML_RANDOMIZER_SEED2_ATTR = "seed2";
const QString XML_RANDOMIZER_ALGORITHM_ATTR = "algorithm";
const QString XML_RESPONSE_ELEMENT = "response";
const QString XML_RESPONSE_MIN_LENGTH_ATTR = "min-length";
const QString XML_RESPONSE_MAX_LENGTH_ATTR = "max-length";
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
    return Auxil::quizTypeToString(type) + ": " + searchSpec.asString();
}

//---------------------------------------------------------------------------
//  asXml
//
//! Return an XML string representing the search spec.
//
//! @return the XML string representation
//---------------------------------------------------------------------------
QString
QuizSpec::asXml() const
{
    QDomImplementation implementation;
    QDomDocument document(implementation.createDocumentType(
                          "zyzzyva-quiz", QString(),
                          "http://boshvark.com/dtd/zyzzyva-quiz.dtd"));

    document.appendChild(asDomElement());

    //// XXX: There should be a programmatic way to write the <?xml?> header
    //// based on the QDomImplementation, shouldn't there?
    return QString("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n") +
        document.toString();
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
    QDomElement topElement = doc.createElement(XML_TOP_ELEMENT);
    topElement.setAttribute(XML_TOP_TYPE_ATTR,
                            Auxil::quizTypeToString(type));
    topElement.setAttribute(XML_TOP_METHOD_ATTR,
                            Auxil::quizMethodToString(method));
    topElement.setAttribute(XML_TOP_QUESTION_ORDER_ATTR,
                            Auxil::quizQuestionOrderToString(questionOrder));
    if (questionOrder == ProbabilityOrder)
        topElement.setAttribute(XML_TOP_PROB_NUM_BLANKS_ATTR, probNumBlanks);

    topElement.setAttribute(XML_TOP_LEXICON_ATTR, lexicon);

    QDomElement sourceElement = doc.createElement(XML_QUESTION_SOURCE_ELEMENT);
    sourceElement.setAttribute(XML_QUESTION_SOURCE_TYPE_ATTR,
                               Auxil::quizSourceTypeToString(sourceType));
    topElement.appendChild(sourceElement);

    if (sourceType == SearchSource)
        sourceElement.appendChild(searchSpec.asDomElement());

    if (questionOrder == RandomOrder) {
        QDomElement randomElement = doc.createElement(XML_RANDOMIZER_ELEMENT);
        randomElement.setAttribute(XML_RANDOMIZER_SEED_ATTR, randomSeed);
        randomElement.setAttribute(XML_RANDOMIZER_SEED2_ATTR, randomSeed2);
        randomElement.setAttribute(XML_RANDOMIZER_ALGORITHM_ATTR,
                                   randomAlgorithm);
        topElement.appendChild(randomElement);
    }

    if (responseMinLength || responseMaxLength) {
        QDomElement responseElement = doc.createElement(XML_RESPONSE_ELEMENT);
        if (responseMinLength) {
            responseElement.setAttribute(XML_RESPONSE_MIN_LENGTH_ATTR,
                                         responseMinLength);
        }
        if (responseMaxLength) {
            responseElement.setAttribute(XML_RESPONSE_MAX_LENGTH_ATTR,
                                         responseMaxLength);
        }
        topElement.appendChild(responseElement);
    }

    if (timerSpec.getType() != NoTimer)
        topElement.appendChild(timerSpec.asDomElement());

    if (method != CardboxQuizMethod)
        topElement.appendChild(progress.asDomElement());

    return topElement;
}

//---------------------------------------------------------------------------
//  fromDomElement
//
//! Reset the object based on the contents of a DOM element representing a
//! quiz spec.
//
//! @param element the DOM element
//! @param errStr return an error string
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
QuizSpec::fromDomElement(const QDomElement& element, QString*)
{
    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    QuizSpec tmpSpec;
    tmpSpec.setQuestionOrder(QuizSpec::RandomOrder);

    if (element.hasAttribute(XML_TOP_TYPE_ATTR)) {
        QuizSpec::QuizType type = Auxil::stringToQuizType(
            element.attribute(XML_TOP_TYPE_ATTR));
        if (type == QuizSpec::UnknownQuizType)
            return false;
        tmpSpec.setType(type);
    }

    if (element.hasAttribute(XML_TOP_METHOD_ATTR)) {
        QuizSpec::QuizMethod method = Auxil::stringToQuizMethod(
            element.attribute(XML_TOP_METHOD_ATTR));
        if (method == QuizSpec::UnknownQuizMethod)
            return false;
        tmpSpec.setMethod(method);
    }

    if (element.hasAttribute(XML_TOP_QUESTION_ORDER_ATTR)) {
        QuizSpec::QuestionOrder order = Auxil::stringToQuizQuestionOrder(
            element.attribute(XML_TOP_QUESTION_ORDER_ATTR));
        if (order == QuizSpec::UnknownOrder)
            return false;
        tmpSpec.setQuestionOrder(order);

        if (order == ProbabilityOrder) {
            // Default to 2 blanks for backward compatibility
            int numBlanks = Defs::MAX_BLANKS;
            if (element.hasAttribute(XML_TOP_PROB_NUM_BLANKS_ATTR)) {
                bool ok = false;
                numBlanks = element.attribute(
                    XML_TOP_PROB_NUM_BLANKS_ATTR).toInt(&ok);
                if (!ok)
                    return false;
            }
            tmpSpec.setProbabilityNumBlanks(numBlanks);
        }
    }

    if (element.hasAttribute(XML_TOP_LEXICON_ATTR)) {
        tmpSpec.setLexicon(Auxil::getUpdatedLexiconName(
            element.attribute(XML_TOP_LEXICON_ATTR)));
    }

    QDomElement elem = element.firstChild().toElement();
    if (elem.isNull())
        return false;

    for (; !elem.isNull(); elem = elem.nextSibling().toElement()) {
        QString tag = elem.tagName();

        // XXX: QuizQuestionSource needs to be a class of its own
        if (tag == XML_QUESTION_SOURCE_ELEMENT) {

            if (!elem.hasAttribute(XML_QUESTION_SOURCE_TYPE_ATTR))
                return false;

            QuizSpec::QuizSourceType source = Auxil::stringToQuizSourceType(
                elem.attribute(XML_QUESTION_SOURCE_TYPE_ATTR));
            if (source == QuizSpec::UnknownSource)
                return false;
            tmpSpec.setQuizSourceType(source);

            // Only for backward compatibility - "single-question" is no
            // longer produced as an attribute
            if (elem.attribute(XML_QUESTION_SOURCE_SINGLE_QUESTION_ATTR) ==
                "true")
            {
                tmpSpec.setType(QuizSpec::QuizWordListRecall);
            }

            if (source == QuizSpec::SearchSource) {
                QDomElement searchElem = elem.firstChild().toElement();
                if (searchElem.tagName() != XML_SEARCH_ELEMENT)
                    return false;

                SearchSpec tmpSearchSpec;
                if (!tmpSearchSpec.fromDomElement(searchElem))
                    return false;
                tmpSpec.setSearchSpec(tmpSearchSpec);
            }
        }

        else if (tag == XML_RESPONSE_ELEMENT) {
            if (elem.hasAttribute(XML_RESPONSE_MIN_LENGTH_ATTR)) {
                bool ok = false;
                int value = elem.attribute(
                    XML_RESPONSE_MIN_LENGTH_ATTR).toInt(&ok);
                if (!ok)
                    return false;
                tmpSpec.setResponseMinLength(value);
            }

            if (elem.hasAttribute(XML_RESPONSE_MAX_LENGTH_ATTR)) {
                bool ok = false;
                int value = elem.attribute(
                    XML_RESPONSE_MAX_LENGTH_ATTR).toInt(&ok);
                if (!ok)
                    return false;
                tmpSpec.setResponseMaxLength(value);
            }
        }

        else if (tag == XML_TIMER_ELEMENT) {
            QuizTimerSpec tmpTimerSpec;
            if (!tmpTimerSpec.fromDomElement(elem))
                return false;
            tmpSpec.setTimerSpec(tmpTimerSpec);
        }

        else if (tag == XML_RANDOMIZER_ELEMENT) {
            if (tmpSpec.getQuestionOrder() != QuizSpec::RandomOrder)
                return false;

            if (!elem.hasAttribute(XML_RANDOMIZER_SEED_ATTR) ||
                !elem.hasAttribute(XML_RANDOMIZER_ALGORITHM_ATTR))
            {
                return false;
            }
            bool ok = false;
            unsigned int tmpSeed = elem.attribute(
                XML_RANDOMIZER_SEED_ATTR).toUInt(&ok);
            if (!ok)
                return false;
            tmpSpec.setRandomSeed(tmpSeed);

            tmpSeed = elem.attribute(XML_RANDOMIZER_SEED2_ATTR).toUInt(&ok);
            if (ok)
                tmpSpec.setRandomSeed2(tmpSeed);

            int tmpAlgorithm = elem.attribute(
                XML_RANDOMIZER_ALGORITHM_ATTR).toInt(&ok);
            if (!ok)
                return false;
            tmpSpec.setRandomAlgorithm(tmpAlgorithm);
        }

        else if (tag == XML_PROGRESS_ELEMENT) {
            QuizProgress tmpProgress;
            if (!tmpProgress.fromDomElement(elem))
                return false;
            tmpSpec.setProgress(tmpProgress);
        }

        else
            return false;
    }

    *this = tmpSpec;
    return true;
}

//---------------------------------------------------------------------------
//  fromXmlFile
//
//! Reset the object based on the contents of a file containing an XML
//! string representing a quiz spec.
//
//! @param file the XML file
//! @param errStr return an error string
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
QuizSpec::fromXmlFile(QFile& file, QString* errStr)
{
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    QDomDocument document;
    bool success = document.setContent(&file, false, &errorMsg, &errorLine,
                                       &errorColumn);

    if (!success) {
        if (errStr) {
            *errStr = "Error in quiz file, line " +
                      QString::number(errorLine) + ", column " +
                      QString::number(errorColumn) + ": " +
                      errorMsg;
        }
        return false;
    }

    if (!fromDomElement(document.documentElement(), errStr))
        return false;

    filename = file.fileName();
    return true;
}
