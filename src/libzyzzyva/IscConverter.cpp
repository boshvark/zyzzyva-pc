//---------------------------------------------------------------------------
// IscConverter.cpp
//
// Functions for translating between ISC weirdnesses and reality.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include "IscConverter.h"
#include <QRegExp>

//---------------------------------------------------------------------------
//  timeIscToReal
//
//! Convert a number of minutes and seconds to a total number of seconds,
//! accounting for overtime weirdness.
//
//! @param minutes the number of minutes
//! @param seconds the number of seconds
//! @param overtime whether the time is overtime
//! @return the total number of seconds, negative if overtime
//---------------------------------------------------------------------------
int
IscConverter::timeIscToReal (int minutes, int seconds, bool overtime)
{
    if (overtime)
        return -minutes * 60 + seconds - 60;
    else
        return minutes * 60 + seconds;
}

//---------------------------------------------------------------------------
//  timeRealToIsc
//
//! Convert a total number of seconds to minutes and seconds, accounting for
//! overtime weirdness.
//
//! @param totalSeconds the total number of seconds
//! @param minutes return the number of minutes
//! @param seconds return the number of seconds
//! @param overtime whether the time is overtime
//---------------------------------------------------------------------------
void
IscConverter::timeRealToIsc (int totalSeconds, int& minutes, int& seconds,
                             bool overtime)
{
    minutes = totalSeconds / 60;
    seconds = totalSeconds % 60;
    if (overtime) {
        minutes = -minutes;
        seconds += 60;
    }
}

//---------------------------------------------------------------------------
//  convertCoordinates
//
//! Convert ISC coordinates to real coordinates, and vice versa.
//
//! @param coordinates the coordinates to convert
//! @return the converted coordinates
//---------------------------------------------------------------------------
QString
IscConverter::convertCoordinates (const QString& coordinates)
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
