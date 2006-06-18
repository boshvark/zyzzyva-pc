//---------------------------------------------------------------------------
// CrosswordGameMove.cpp
//
// A class to represent a crossword game move.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include <QtDebug>

using namespace Defs;

//---------------------------------------------------------------------------
//  CrosswordGameMove
//
//! Constructor.
//
//! @param str a string representation of the move
//---------------------------------------------------------------------------
CrosswordGameMove::CrosswordGameMove (const QString& str)
{
    init();

    QStringList split = str.split (" ");
    QString tMoveType = split[0];

    if (tMoveType == "MOVE") {
        if (split.size() < 7)
            return;

        type = Move;

        QString placement = translateCoordinates (split[1]);
        word = split[2];
        score = split[3].toInt();
        int minutes = split[4].toInt();
        int seconds = split[5].toInt();
        secondsLeft = IscConverter::timeIscToReal (minutes, seconds);
        newRack = Auxil::getAlphagram (split[6]);
        if (newRack == "---")
            newRack = QString();
        //QString tChangeNumber = split[7];

        setPlacement (placement);
    }

    else if (tMoveType == "PAS") {
        if (split.size() < 4)
            return;

        int minutes = split[1].toInt();
        int seconds = split[2].toInt();
        secondsLeft = IscConverter::timeIscToReal (minutes, seconds);
        QString challenged = split[3];
        if (challenged == "---") {
            type = Pass;
        }
        else {
            type = TakeBack;
            penaltyType = LoseTurn;
            QStringList tokens = challenged.split ("_");
            QString placement = translateCoordinates (tokens[0]);
            word = tokens[1];
            score = -tokens[2].toInt();
            setPlacement (placement);
        }
    }

    else if (tMoveType == "CHANGE") {
        if (split.size() < 5)
            return;

        type = Exchange;
        newRack = split[1];
        int minutes = split[2].toInt();
        int seconds = split[3].toInt();
        secondsLeft = IscConverter::timeIscToReal (minutes, seconds);
        numExchanged = split[4].toInt();
    }

    else if (tMoveType == "DRAW") {
        type = DrawTiles;
        newRack = Auxil::getAlphagram (split[1]);
    }
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
CrosswordGameMove::setPlacement (const QString& placement)
{
    QRegExp re ("\\d+|\\w");
    int pos = re.indexIn (placement, 0);
    QString aMatch = placement.mid (pos, re.matchedLength());
    pos = re.indexIn (placement, pos + re.matchedLength());
    QString bMatch = placement.mid (pos, re.matchedLength());

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

//---------------------------------------------------------------------------
//  translateCoordinates
//
//! Translate ISC coordinates to real coordinates, and vice versa.
//
//! @param coordinates the coordinates to translate
//! @return the translated coordinates
//---------------------------------------------------------------------------
QString
CrosswordGameMove::translateCoordinates (const QString& coordinates) const
{
    QString real;
    QRegExp re ("\\d+|\\w");

    int pos = 0;
    while ((pos = re.indexIn (coordinates, pos)) >= 0) {
        QString match = coordinates.mid (pos, re.matchedLength());

        if (match == "1") real += "A";
        else if (match == "2") real += "B";
        else if (match == "3") real += "C";
        else if (match == "4") real += "D";
        else if (match == "5") real += "E";
        else if (match == "6") real += "F";
        else if (match == "7") real += "G";
        else if (match == "8") real += "H";
        else if (match == "9") real += "I";
        else if (match == "10") real += "J";
        else if (match == "11") real += "K";
        else if (match == "12") real += "L";
        else if (match == "13") real += "M";
        else if (match == "14") real += "N";
        else if (match == "15") real += "O";
        else if (match == "A") real += "1";
        else if (match == "B") real += "2";
        else if (match == "C") real += "3";
        else if (match == "D") real += "4";
        else if (match == "E") real += "5";
        else if (match == "F") real += "6";
        else if (match == "G") real += "7";
        else if (match == "H") real += "8";
        else if (match == "I") real += "9";
        else if (match == "J") real += "10";
        else if (match == "K") real += "11";
        else if (match == "L") real += "12";
        else if (match == "M") real += "13";
        else if (match == "N") real += "14";
        else if (match == "O") real += "15";

        pos += re.matchedLength();
    }

    return real;
}
