//---------------------------------------------------------------------------
// QuizProgress.cpp
//
// A class to represent quiz progress.
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

#include "QuizProgress.h"

//---------------------------------------------------------------------------
//  QuizProgress
//
//! Constructor.
//
//! @param q the current question number
//! @param c the number of correct responses
//! @param i the number of incorrect responses
//! @param m the number of missed responses
//---------------------------------------------------------------------------
QuizProgress::QuizProgress (int q, int c, int i, int m)
    : question (q), correct (c), incorrect (i), missed (m)
{
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
QuizProgress::addIncorrect (const QString& word, int count)
{
    incorrectWords[word] = count;
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
QuizProgress::addMissed (const QString& word, int count)
{
    missedWords[word] = count;
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
    QDomElement element;
    return element;
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
QuizProgress::fromDomElement (const QDomElement& element)
{
    return false;
}
