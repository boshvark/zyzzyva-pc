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
