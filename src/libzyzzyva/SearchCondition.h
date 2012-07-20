//---------------------------------------------------------------------------
// SearchCondition.h
//
// A class to represent a word search condition.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_SEARCH_CONDITION_H
#define ZYZZYVA_SEARCH_CONDITION_H

#include <QDomElement>
#include <QString>

class SearchCondition
{
    public:
    enum SearchType {
        UnknownSearchType = 0,
        PatternMatch,
        AnagramMatch,
        SubanagramMatch,
        Length,
        Prefix,
        Suffix,
        IncludeLetters,
        ConsistOf,
        BelongToGroup,
        InLexicon,
        InWordList,
        NumAnagrams,
        NumVowels,
        NumUniqueLetters,
        PointValue,
        Probability,
        ProbabilityOrder,
        LimitByProbabilityOrder,
        PlayabilityOrder,
        LimitByPlayabilityOrder,
        PartOfSpeech,
        Definition,

        // Obsolete search condition types
        OldExactLength,
        OldMinLength,
        OldMaxLength,
        OldDoesNotTakePrefix,
        OldDoesNotTakeSuffix,
        OldMustExclude,
        OldNotInWordList,
        OldExactAnagrams,
        OldMinAnagrams,
        OldMaxAnagrams
    };

    public:
    SearchCondition() : type(UnknownSearchType), minValue(0),
                        maxValue(0), intValue(0), negated(false),
                        boolValue(false), legacy(false) { }
    QString asString() const;
    QDomElement asDomElement() const;
    bool fromDomElement(const QDomElement& element);

    SearchType type;
    QString stringValue;
    int minValue;
    int maxValue;
    int intValue;
    bool negated;
    bool boolValue;
    bool legacy;
};

#endif // ZYZZYVA_SEARCH_CONDITION_H
