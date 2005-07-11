//---------------------------------------------------------------------------
// SearchCondition.h
//
// A class to represent a word search condition.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef SEARCH_CONDITION_H
#define SEARCH_CONDITION_H

#include <qdom.h>
#include <qstring.h>
#include <set>

class SearchCondition
{
    public:
    enum SearchType {
        UnknownSearchType = 0,
        PatternMatch,
        AnagramMatch,
        SubanagramMatch,
        ExactLength,
        MinLength,
        MaxLength,
        MustInclude,
        MustExclude,
        MustConsist,
        MustBelong,
        InWordList,
        ExactAnagrams,
        MinAnagrams,
        MaxAnagrams,
        MinProbability,
        MaxProbability
    };

    public:
    SearchCondition() : type (UnknownSearchType), intValue (0) { }
    QString asString() const;
    QDomElement asDomElement() const;

    SearchType type;
    QString stringValue;
    int intValue;
};

#endif // SEARCH_CONDITION_H
