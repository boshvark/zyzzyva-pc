//---------------------------------------------------------------------------
// Rand.cpp
//
// A random number generator based on George Marsaglia's algorithms found
// on this web page:  http://www.ciphersbyritter.com/NEWS4/RANDC.HTM
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

#include "Rand.h"

#include <QString>

//---------------------------------------------------------------------------
//  rand
//
//! Return a random number between zero and a maximum value, inclusive.
//
//! @param max the maximum value to return
//! @return a random number
//---------------------------------------------------------------------------
unsigned int
Rand::rand(unsigned int max)
{
    unsigned int randnum = 0;
    switch (algorithm) {
        case SystemRand:   randnum = std::rand(); break;
        case MarsagliaMwc: randnum = mwc(); break;
        default: break;
    }

    if ((max == 0) || (max == 4294967295U))
        return randnum;

    switch (algorithm) {
        case SystemRand: return randnum % (max + 1);
        default: return (randnum / ((4294967295U / (max + 1)) + 1));
    }
}

//---------------------------------------------------------------------------
//  mwc
//
//! Return a random number using the Marsaglia multiply with carry (MWC)
//! algorithm.
//
//! @return a random number
//---------------------------------------------------------------------------
unsigned int
Rand::mwc()
{
    return ((znew() << 16) + wnew());
}

//---------------------------------------------------------------------------
//  znew
//
//! Modify the Z seed and return its modified value.
//
//! @return the modified value of the Z seed
//---------------------------------------------------------------------------
unsigned int
Rand::znew()
{
    z = 36969 * (z & 65535) + (z >> 16);
    return z;
}

//---------------------------------------------------------------------------
//  wnew
//
//! Modify the W seed and return its modified value.
//
//! @return the modified value of the W seed
//---------------------------------------------------------------------------
unsigned int
Rand::wnew()
{
    w = 18000 * (w & 65535) + (w >> 16);
    return w;
}
