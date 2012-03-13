//---------------------------------------------------------------------------
// CrosswordGameMove.cpp
//
// A class to represent a crossword game move.
//
// Copyright 2006-2012 Boshvark Software, LLC.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "CrosswordGameMove.h"
#include "IscConverter.h"
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

//---------------------------------------------------------------------------
//  CrosswordGameMove
//
//! Constructor.
//
//! @param str a string representation of the move
//---------------------------------------------------------------------------
CrosswordGameMove::CrosswordGameMove(const QString& str)
{
    init();

    QStringList split = str.split(" ");
    QString tMoveType = split[0];

    if (tMoveType == "MOVE") {
        if (split.size() < 7)
            return;

        type = Move;

        QString placement = IscConverter::convertCoordinates(split[1]);
        word = split[2];
        score = split[3].toInt();
        int minutes = split[4].toInt();
        int seconds = split[5].toInt();
        secondsLeft = IscConverter::timeIscToReal(minutes, seconds);
        newRack = Auxil::getAlphagram(split[6]);
        if (newRack == "---")
            newRack = QString();
        //QString tChangeNumber = split[7];

        setPlacement(placement);
    }

    else if (tMoveType == "PAS") {
        if (split.size() < 4)
            return;

        int minutes = split[1].toInt();
        int seconds = split[2].toInt();
        secondsLeft = IscConverter::timeIscToReal(minutes, seconds);
        QString challenged = split[3];
        if (challenged == "---") {
            type = Pass;
        }
        else {
            type = TakeBack;
            penaltyType = LoseTurn;
            QStringList tokens = challenged.split("_");
            QString placement = IscConverter::convertCoordinates(tokens[0]);
            word = tokens[1];
            score = -tokens[2].toInt();
            setPlacement(placement);
        }
    }

    else if (tMoveType == "CHANGE") {
        if (split.size() < 5)
            return;

        type = Exchange;
        newRack = split[1];
        int minutes = split[2].toInt();
        int seconds = split[3].toInt();
        secondsLeft = IscConverter::timeIscToReal(minutes, seconds);
        numExchanged = split[4].toInt();
    }

    else if (tMoveType == "DRAW") {
        type = DrawTiles;
        newRack = Auxil::getAlphagram(split[1]);
    }
}

//---------------------------------------------------------------------------
//  getPlacement
//
//! Return a string representing the placement of the play.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
QString
CrosswordGameMove::getPlacement() const
{
    QString placement;
    if (type != Move)
        return placement;

    QString rowStr = QString::number(row + 1);
    QString colStr = QChar('A' + column);

    if (orientation == Horizontal) {
        placement = rowStr + colStr;
    }

    else if (orientation == Vertical) {
        placement = colStr + rowStr;
    }

    return placement;
}

//---------------------------------------------------------------------------
//  isValid
//
//! Determine whether the move can possibly be valid.
//
//! @return true if valid, false otherwise
//---------------------------------------------------------------------------
bool
CrosswordGameMove::isValid() const
{
    if (type == Invalid)
        return false;

    if (playerNum <= 0)
        return false;

    if (type == Move) {
        if (orientation == NoOrientation)
            return false;
        if ((row < 0) || (row > 15))
            return false;
        if ((column < 0) || (column > 15))
            return false;
        if (word.isEmpty())
            return false;
    }

    else if (type == Exchange) {
        if ((numExchanged <= 0) || (numExchanged > 7))
            return false;
    }

    else if (type == Challenge) {
        // You can have free challenges, but they don't count as a move
        if (penaltyType == NoPenalty)
            return false;

        if (penaltyPoints <= 0)
            return false;
    }

    return true;
}

//---------------------------------------------------------------------------
//  init
//
//! Initialize the object.
//---------------------------------------------------------------------------
void
CrosswordGameMove::init()
{
    type = Invalid;
    orientation = NoOrientation;
    row = -1;
    column = -1;
    score = 0;
    secondsLeft = 0;
    numExchanged = 0;
    penaltyType = NoPenalty;
    penaltyPoints = 0;
    playerNum = 0;
}

//---------------------------------------------------------------------------
//  setPlacement
//
//! Set the orientation, row and column values from a placement string.
//
//! @param placement the placement string
//---------------------------------------------------------------------------
void
CrosswordGameMove::setPlacement(const QString& placement)
{
    QRegExp re ("\\d+|\\w");
    int pos = re.indexIn(placement, 0);
    QString aMatch = placement.mid(pos, re.matchedLength());
    pos = re.indexIn(placement, pos + re.matchedLength());
    QString bMatch = placement.mid(pos, re.matchedLength());

    orientation = aMatch[0].isNumber() ? Horizontal : Vertical;

    if (orientation == Horizontal) {
        row = aMatch.toInt() - 1;
        column = bMatch[0].toUpper().toAscii() - 'A';
    }
    else {
        row = bMatch.toInt() - 1;
        column = aMatch[0].toUpper().toAscii() - 'A';
    }
}
