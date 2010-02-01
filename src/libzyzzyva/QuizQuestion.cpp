//---------------------------------------------------------------------------
// QuizQuestion.cpp
//
// A class to represent a quiz question.
//
// Copyright 2009, 2010 Michael W Thelen <mthelen@gmail.com>.
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

#include "QuizQuestion.h"

const QString XML_TOP_ELEMENT = "question";
const QString XML_QUESTION_ATTR = "question";
const QString XML_CORRECT_ATTR = "correct";
const QString XML_COMPLETE_ATTR = "complete";
const QString XML_RESPONSES_ELEMENT = "responses";
const QString XML_RESPONSE_ELEMENT = "response";
const QString XML_RESPONSE_WORD_ATTR = "word";

using namespace std;

//---------------------------------------------------------------------------
//  addResponse
//
//! Add a response.
//
//! @param word the response word
//---------------------------------------------------------------------------
void
QuizQuestion::addResponse(const QString& word)
{
    responses.insert(word);
}

//---------------------------------------------------------------------------
//  asDomElement
//
//! Return a DOM element representing the quiz question.
//
//! @return the DOM element
//---------------------------------------------------------------------------
QDomElement
QuizQuestion::asDomElement() const
{
    QDomDocument doc;
    QDomElement topElement = doc.createElement(XML_TOP_ELEMENT);

    topElement.setAttribute(XML_QUESTION_ATTR, question);
    topElement.setAttribute(XML_COMPLETE_ATTR,
        (complete ? QString("true") : QString("false")));
    topElement.setAttribute(XML_CORRECT_ATTR,
        (correct ? QString("true") : QString("false")));

    if (!responses.isEmpty()) {
        QDomElement responsesElement = doc.createElement(XML_RESPONSES_ELEMENT);
        topElement.appendChild(responsesElement);
        foreach (const QString& response, responses) {
            QDomElement elem = doc.createElement(XML_RESPONSE_ELEMENT);
            elem.setAttribute(XML_RESPONSE_WORD_ATTR, response);
            responsesElement.appendChild(elem);
        }
    }

    return topElement;
}

//---------------------------------------------------------------------------
//  fromDomElement
//
//! Reset the object based on the contents of a DOM element representing quiz
//! progress.
//
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
QuizQuestion::fromDomElement(const QDomElement& element)
{
    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    QuizQuestion tmpQuestion;

    if (element.hasAttribute(XML_QUESTION_ATTR)) {
        tmpQuestion.setQuestion(element.attribute(XML_QUESTION_ATTR));
    }

    if (element.hasAttribute(XML_COMPLETE_ATTR)) {
        tmpQuestion.setComplete(element.attribute(XML_COMPLETE_ATTR) == "true");
    }

    if (element.hasAttribute(XML_CORRECT_ATTR)) {
        tmpQuestion.setCorrect(element.attribute(XML_CORRECT_ATTR) == "true");
    }

    if (element.hasChildNodes()) {
        QDomElement elem = element.firstChild().toElement();
        for (; !elem.isNull(); elem = elem.nextSibling().toElement()) {

            if (elem.tagName() != XML_RESPONSES_ELEMENT)
                return false;

            if (!elem.hasChildNodes())
                break;

            QDomElement responseElem = elem.firstChild().toElement();
            for (; !responseElem.isNull();
                responseElem = responseElem.nextSibling().toElement())
            {
                if ((responseElem.tagName() != XML_RESPONSE_ELEMENT) ||
                    !responseElem.hasAttribute(XML_RESPONSE_WORD_ATTR))
                    return false;

                tmpQuestion.responses.insert(
                    responseElem.attribute(XML_RESPONSE_WORD_ATTR));
            }
        }
    }

    *this = tmpQuestion;
    return true;
}
