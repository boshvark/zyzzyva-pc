//---------------------------------------------------------------------------
// Rand.h
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

#ifndef ZYZZYVA_RAND_H
#define ZYZZYVA_RAND_H

#include <cstdlib>

class Rand
{
    public:
    enum Algorithm {
        SystemRand = 0,
        MarsagliaMwc = 1
    };

    public:
    Rand(int a = MarsagliaMwc, unsigned int z0 = 362436069,
         unsigned int w0 = 521288629)
        : algorithm(Algorithm(a)), z(z0), w(w0) { }
    ~Rand() { }

    void setAlgorithm(int a) { algorithm = Algorithm(a); }
    void srand(unsigned int z0, unsigned int w0 = 0) {
        if (algorithm == SystemRand) std::srand(z0);
        z = z0; w = w0;
    }
    unsigned int rand(unsigned int max = 4294967295U);

    private:
    unsigned int mwc();
    unsigned int znew();
    unsigned int wnew();

    private:
    Algorithm algorithm;
    unsigned int z;
    unsigned int w;
};

#endif // ZYZZYVA_RAND_H
