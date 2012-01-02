//---------------------------------------------------------------------------
// QuizProgress.cpp
//
// A class to represent quiz progress.
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

#include "QuizProgress.h"

const QString XML_TOP_ELEMENT = "progress";
const QString XML_QUESTION_ATTR = "question";
const QString XML_CORRECT_ATTR = "correct";
const QString XML_QUESTION_COMPLETE_ATTR = "question-complete";
const QString XML_QUESTION_CORRECT_RESPONSES_ELEMENT =
              "question-correct-responses";
const QString XML_INCORRECT_RESPONSES_ELEMENT = "incorrect-responses";
const QString XML_MISSED_RESPONSES_ELEMENT = "missed-responses";
const QString XML_RESPONSE_ELEMENT = "response";
const QString XML_RESPONSE_WORD_ATTR = "word";
const QString XML_RESPONSE_COUNT_ATTR = "count";

using namespace std;

//---------------------------------------------------------------------------
//  isEmpty
//
//! Determine whether the progress represented in this object is empty.
//
//! @return true if empty, false otherwise
//---------------------------------------------------------------------------
bool
QuizProgress::isEmpty() const
{
    return ((question == 0) && (correct == 0) && (incorrect == 0) &&
            (missed == 0) && (questionComplete == false) &&
            questionCorrectWords.isEmpty() && incorrectWords.isEmpty() &&
            missedWords.isEmpty());
}

//---------------------------------------------------------------------------
//  addIncorrect
//
//! Add an incorrect response, incrementing the number of times the response
//! was made.
//
//! @param word the incorrect response word
//---------------------------------------------------------------------------
void
QuizProgress::addIncorrect(const QString& word)
{
    if (incorrectWords.contains(word))
        ++incorrectWords[word];
    else
        incorrectWords[word] = 1;
    ++incorrect;
}

//---------------------------------------------------------------------------
//  addIncorrect
//
//! Add an incorrect response with the number of times the response was made.
//
//! @param word the incorrect response word
//! @param count the number of times the incorrect response was given
//---------------------------------------------------------------------------
void
QuizProgress::addIncorrect(const QString& word, int count)
{
    incorrectWords[word] = count;
    incorrect += count;
}

//---------------------------------------------------------------------------
//  removeIncorrect
//
//! Remove a incorrect response.
//
//! @param word the response word
//---------------------------------------------------------------------------
void
QuizProgress::removeIncorrect(const QString& word)
{
    if (!incorrectWords.contains(word))
        return;
    --incorrectWords[word];
    --incorrect;
}

//---------------------------------------------------------------------------
//  addMissed
//
//! Add a missed response, incrementing the number of times the response was
//! missed.
//
//! @param word the missed response word
//---------------------------------------------------------------------------
void
QuizProgress::addMissed(const QString& word)
{
    if (missedWords.contains(word))
        ++missedWords[word];
    else
        missedWords[word] = 1;
    ++missed;
}

//---------------------------------------------------------------------------
//  addMissed
//
//! Add a missed response with the number of times the response was made.
//
//! @param word the missed response word
//! @param count the number of times the response was missed
//---------------------------------------------------------------------------
void
QuizProgress::addMissed(const QString& word, int count)
{
    missedWords[word] = count;
    missed += count;
}

//---------------------------------------------------------------------------
//  removeMissed
//
//! Remove a missed response.
//
//! @param word the response word
//---------------------------------------------------------------------------
void
QuizProgress::removeMissed(const QString& word)
{
    if (!missedWords.contains(word))
        return;
    --missedWords[word];
    --missed;
}

//---------------------------------------------------------------------------
//  addQuestionCorrect
//
//! Add a correct response for the current question.
//
//! @param word the correct response word
//---------------------------------------------------------------------------
void
QuizProgress::addQuestionCorrect(const QString& word)
{
    questionCorrectWords.insert(word);
}

//---------------------------------------------------------------------------
//  asDomElement
//
//! Return a DOM element representing the quiz progress.
//
//! @return the DOM element
//---------------------------------------------------------------------------
QDomElement
QuizProgress::asDomElement() const
{
    QDomDocument doc;
    QDomElement topElement = doc.createElement(XML_TOP_ELEMENT);

    topElement.setAttribute(XML_QUESTION_ATTR, question);
    topElement.setAttribute(XML_CORRECT_ATTR, correct);
    topElement.setAttribute(XML_QUESTION_COMPLETE_ATTR,
        (questionComplete ? QString ("true") : QString ("false")));

    if (!questionCorrectWords.isEmpty()) {
        QDomElement questionCorrectElement = doc.createElement(
            XML_QUESTION_CORRECT_RESPONSES_ELEMENT);
        topElement.appendChild(questionCorrectElement);

        QSetIterator<QString> it (questionCorrectWords);
        while (it.hasNext()) {
            QString word = it.next();
            QDomElement elem = doc.createElement(XML_RESPONSE_ELEMENT);
            elem.setAttribute(XML_RESPONSE_WORD_ATTR, word);
            questionCorrectElement.appendChild(elem);
        }
    }

    if (!incorrectWords.empty()) {
        QDomElement incorrectElement = doc.createElement(
            XML_INCORRECT_RESPONSES_ELEMENT);
        topElement.appendChild(incorrectElement);
        QMap<QString, int>::const_iterator it;
        for (it = incorrectWords.begin(); it != incorrectWords.end(); ++it) {
            QDomElement elem = doc.createElement(XML_RESPONSE_ELEMENT);
            elem.setAttribute(XML_RESPONSE_WORD_ATTR, it.key());
            elem.setAttribute(XML_RESPONSE_COUNT_ATTR, *it);
            incorrectElement.appendChild(elem);
        }
    }

    if (!missedWords.empty()) {
        QDomElement missedElement = doc.createElement(
            XML_MISSED_RESPONSES_ELEMENT);
        topElement.appendChild(missedElement);
        QMap<QString, int>::const_iterator it;
        for (it = missedWords.begin(); it != missedWords.end(); ++it) {
            QDomElement elem = doc.createElement(XML_RESPONSE_ELEMENT);
            elem.setAttribute(XML_RESPONSE_WORD_ATTR, it.key());
            elem.setAttribute(XML_RESPONSE_COUNT_ATTR, *it);
            missedElement.appendChild(elem);
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
QuizProgress::fromDomElement(const QDomElement& element)
{
    if (element.tagName() != XML_TOP_ELEMENT)
        return false;

    QuizProgress tmpProgress;

    if (element.hasAttribute(XML_QUESTION_ATTR)) {
        bool ok = false;
        int tmpQuestion = element.attribute(XML_QUESTION_ATTR).toInt(&ok);
        if (!ok)
            return false;
        tmpProgress.setQuestion(tmpQuestion);
    }

    if (element.hasAttribute(XML_CORRECT_ATTR)) {
        bool ok = false;
        int tmpCorrect =
            element.attribute(XML_CORRECT_ATTR).toInt(&ok);
        if (!ok)
            return false;
        tmpProgress.setCorrect(tmpCorrect);
    }

    if (element.hasAttribute(XML_QUESTION_COMPLETE_ATTR)) {
        tmpProgress.setQuestionComplete(
            element.attribute(XML_QUESTION_COMPLETE_ATTR) == "true");
    }

    QDomElement elem = element.firstChild().toElement();
    for (; !elem.isNull(); elem = elem.nextSibling().toElement()) {

        if ((elem.tagName() != XML_MISSED_RESPONSES_ELEMENT) &&
            (elem.tagName() != XML_INCORRECT_RESPONSES_ELEMENT) &&
            (elem.tagName() != XML_QUESTION_CORRECT_RESPONSES_ELEMENT))
        {
            return false;
        }

        QDomElement responseElem = elem.firstChild().toElement();
        for (; !responseElem.isNull();
             responseElem = responseElem.nextSibling().toElement())
        {
            if (!responseElem.hasAttribute(XML_RESPONSE_WORD_ATTR))
                return false;

            int count = 0;
            if ((elem.tagName() == XML_MISSED_RESPONSES_ELEMENT) ||
                (elem.tagName() == XML_INCORRECT_RESPONSES_ELEMENT))
            {
                if (!responseElem.hasAttribute(XML_RESPONSE_COUNT_ATTR))
                    return false;
                bool ok = false;
                count = responseElem.attribute(
                    XML_RESPONSE_COUNT_ATTR).toInt(&ok);
                if (!ok)
                    return false;
            }

            QString word = responseElem.attribute(XML_RESPONSE_WORD_ATTR);
            if (elem.tagName() == XML_MISSED_RESPONSES_ELEMENT)
                tmpProgress.addMissed(word, count);
            else if (elem.tagName() == XML_INCORRECT_RESPONSES_ELEMENT)
                tmpProgress.addIncorrect(word, count);
            else if (elem.tagName() == XML_QUESTION_CORRECT_RESPONSES_ELEMENT)
                tmpProgress.addQuestionCorrect(word);
        }
    }

    *this = tmpProgress;
    return true;
}
