//---------------------------------------------------------------------------
// SearchSet.h
//
// An enumeration of match types.
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

#ifndef ZYZZYVA_SEARCH_SET_H
#define ZYZZYVA_SEARCH_SET_H

#include <QString>

enum SearchSet {
    UnknownSearchSet = 0,
    SetHookWords,
    SetFrontHooks,
    SetBackHooks,
    SetHighFives,
    SetTypeOneSevens,
    SetTypeTwoSevens,
    SetTypeThreeSevens,
    SetTypeOneEights,
    SetTypeTwoEights,
    SetTypeThreeEights,
    SetEightsFromSevenLetterStems,

    // Obsolete search sets
    SetOldNewInOwl2,
    SetOldNewInCsw
};

#endif // ZYZZYVA_SEARCH_SET_H
