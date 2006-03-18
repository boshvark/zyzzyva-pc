//---------------------------------------------------------------------------
// LetterBag.cpp
//
// A class for holding instances of letters.
//
// Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#include "LetterBag.h"
#include "MainSettings.h"
#include "Defs.h"

using namespace Defs;

const QChar BLANK_CHAR = '_';

//---------------------------------------------------------------------------
//  LetterBag
//
//! Constructor.  Precalculate the M choose N probabilities for all
//! combinations up to MAX_WORD_LEN.
//---------------------------------------------------------------------------
LetterBag::LetterBag()
    : totalLetters (0)
{
    QString distribution = MainSettings::getLetterDistribution();
    QStringList strList = distribution.split (" ");

    int maxFrequency = MAX_WORD_LEN;

    QString str;
    foreach (str, strList) {
        QChar letter = str.section (":", 0, 0)[0];
        int frequency = str.section (":", 1, 1).toInt();
        letterFrequencies.insert (letter, frequency);
        totalLetters += frequency;
        if (frequency > maxFrequency)
            maxFrequency = frequency;
    }

    // Precalculate M choose N combinations - use doubles because the numbers
    // get very large
    double a = 1;
    double r = 1;
    for (int i = 0; i <= maxFrequency; ++i, ++r) {
        fullChooseCombos.append (a);
        a *= (totalLetters + 1.0 - r) / r;

        QList<double> subList;
        for (int j = 0; j <= maxFrequency; ++j) {
            if ((i == j) || (j == 0))
                subList.append (1.0);
            else if (i == 0)
                subList.append (0.0);
            else
                subList.append (subChooseCombos[i-1][j-1] +
                                subChooseCombos[i-1][j]);
        }
        subChooseCombos.append (subList);
    }
}

//---------------------------------------------------------------------------
//  getProbability
//
//! Return the probability of drawing a word from a full bag of letters when
//! drawing the number of letters in the word.
//
//! @param word the word
//! @return the probability of drawing letters to form the word, times 1e9
//---------------------------------------------------------------------------
double
LetterBag::getProbability (const QString& word) const
{
    return (1e9 * getNumCombinations (word)) /
        fullChooseCombos[word.length()];
}

//---------------------------------------------------------------------------
//  getNumCombinations
//
//! Return the unique ways of drawing a word from a full bag of letters when
//! drawing the number of letters in the word.
//
//! @param word the word
//! @return the number of ways of drawing letters to form the word
//---------------------------------------------------------------------------
double
LetterBag::getNumCombinations (const QString& word) const
{
    // Build parallel arrays of letters with their counts, and the
    // precalculated combinations based on the letter frequency
    QList<QChar> letters;
    QList<int> counts;
    QList<const QList<double>*> combos;
    for (int i = 0; i < word.length(); ++i) {
        QChar c = word.at (i);

        bool foundLetter = false;
        for (int j = 0; j < letters.size(); ++j) {
            if (letters[j] == c) {
                ++counts[j];
                foundLetter = true;
                break;
            }
        }

        if (!foundLetter) {
            letters.append (c);
            counts.append (1);
            combos.append (&subChooseCombos[ letterFrequencies[c] ]);
        }
    }

    // XXX: Generalize the following code to handle arbitrary number of blanks
    double totalCombos = 0.0;
    int numLetters = letters.size();

    // Calculate the combinations with no blanks
    double thisCombo = 1.0;
    for (int i = 0; i < numLetters; ++i) {
        thisCombo *= (*combos[i])[ counts[i] ];
    }
    totalCombos += thisCombo;

    // Calculate the combinations with one blank
    for (int i = 0; i < numLetters; ++i) {
        --counts[i];
        thisCombo = subChooseCombos[ letterFrequencies[BLANK_CHAR] ][1];
        for (int j = 0; j < numLetters; ++j) {
            thisCombo *= (*combos[j])[ counts[j] ];
        }
        totalCombos += thisCombo;
        ++counts[i];
    }

    // Calculate the combinations with two blanks
    for (int i = 0; i < numLetters; ++i) {
        --counts[i];
        for (int j = i; j < numLetters; ++j) {
            if (!counts[j])
                continue;
            --counts[j];
            thisCombo = subChooseCombos[ letterFrequencies[BLANK_CHAR] ][2];

            for (int k = 0; k < numLetters; ++k) {
                thisCombo *= (*combos[k])[ counts[k] ];
            }
            totalCombos += thisCombo;
            ++counts[j];
        }
        ++counts[i];
    }

    return totalCombos;
}
