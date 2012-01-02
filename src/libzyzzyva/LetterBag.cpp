//---------------------------------------------------------------------------
// LetterBag.cpp
//
// A class for holding instances of letters.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

const QChar LetterBag::BLANK_CHAR = '_';

//---------------------------------------------------------------------------
//  LetterBag
//
//! Constructor.  Precalculate the M choose N probabilities for all
//! combinations up to MAX_WORD_LEN.
//
//! @param distribution the letter distribution to use
//---------------------------------------------------------------------------
LetterBag::LetterBag(const QString& distribution)
    : totalLetters(0)
{
    // Set letter values
    // FIXME: this should be able to be passed in as a parameter
    letterValues[QChar(BLANK_CHAR)] = 0;
    letterValues[QChar('A')] = 1;
    letterValues[QChar('B')] = 3;
    letterValues[QChar('C')] = 3;
    letterValues[QChar('D')] = 2;
    letterValues[QChar('E')] = 1;
    letterValues[QChar('F')] = 4;
    letterValues[QChar('G')] = 2;
    letterValues[QChar('H')] = 4;
    letterValues[QChar('I')] = 1;
    letterValues[QChar('J')] = 8;
    letterValues[QChar('K')] = 5;
    letterValues[QChar('L')] = 1;
    letterValues[QChar('M')] = 3;
    letterValues[QChar('N')] = 1;
    letterValues[QChar('O')] = 1;
    letterValues[QChar('P')] = 3;
    letterValues[QChar('Q')] = 10;
    letterValues[QChar('R')] = 1;
    letterValues[QChar('S')] = 1;
    letterValues[QChar('T')] = 1;
    letterValues[QChar('U')] = 1;
    letterValues[QChar('V')] = 4;
    letterValues[QChar('W')] = 4;
    letterValues[QChar('X')] = 8;
    letterValues[QChar('Y')] = 4;
    letterValues[QChar('Z')] = 10;

    rng.srand(QDateTime::currentDateTime().toTime_t(), Auxil::getPid());
    resetContents(distribution);
}

//---------------------------------------------------------------------------
//  getProbability
//
//! Return the probability of drawing a word from a full bag of letters when
//! drawing the number of letters in the word.
//
//! @param word the word
//! @param numBlanks the number of blanks considered to be in the bag - if
//! greater than 2, then pared back to 2
//! @return the probability of drawing letters to form the word, times 1e9
//---------------------------------------------------------------------------
double
LetterBag::getProbability(const QString& word, int numBlanks) const
{
    return (1e9 * getNumCombinations(word, numBlanks)) /
        fullChooseCombos[word.length()];
}

//---------------------------------------------------------------------------
//  getNumCombinations
//
//! Return the unique ways of drawing a word from a full bag of letters when
//! drawing the number of letters in the word.
//
//! @param word the word
//! @param numBlanks the number of blanks considered to be in the bag - if
//! greater than 2, then pared back to 2
//! @return the number of ways of drawing letters to form the word
//---------------------------------------------------------------------------
double
LetterBag::getNumCombinations(const QString& word, int numBlanks) const
{
    if (numBlanks < 0)
        numBlanks = 0;
    else if (numBlanks > 2)
        numBlanks = 2;

    // Build parallel arrays of letters with their counts, and the
    // precalculated combinations based on the letter frequency
    QList<QChar> letters;
    QList<int> counts;
    QList<const QList<double>*> combos;
    for (int i = 0; i < word.length(); ++i) {
        QChar c = word.at(i);

        bool foundLetter = false;
        for (int j = 0; j < letters.size(); ++j) {
            if (letters[j] == c) {
                ++counts[j];
                foundLetter = true;
                break;
            }
        }

        if (!foundLetter) {
            letters.append(c);
            counts.append(1);
            combos.append(&subChooseCombos[ letterFrequencies[c] ]);
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

    if (numBlanks == 0)
        return totalCombos;

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

    if (numBlanks == 1)
        return totalCombos;

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

    //if (numBlanks == 2)
    //    return totalCombos;

    return totalCombos;
}

//---------------------------------------------------------------------------
//  getLetterValue
//
//! Return the point value of a letter.
//
//! @param letter the letter
//! @return the point value
//---------------------------------------------------------------------------
int
LetterBag::getLetterValue(const QChar& letter) const
{
    return letterValues.value(letter.toUpper(), 0);
}

//---------------------------------------------------------------------------
//  setLetterValue
//
//! Set the point value of a letter.
//
//! @param letter the letter
//! @param value the point value
//---------------------------------------------------------------------------
void
LetterBag::setLetterValue(const QChar& letter, int value)
{
    letterValues[letter] = value;
}

//---------------------------------------------------------------------------
//  resetContents
//
//! Reset the contents of the bag.  If no letter distribution is specified,
//! the default distribution is used.
//
//! @param distribution the letter distribution
//---------------------------------------------------------------------------
void
LetterBag::resetContents(const QString& distribution)
{
    QString dist (distribution);
    if (dist.isEmpty())
        dist = MainSettings::getLetterDistribution();
    QStringList strList = dist.split(" ");

    int maxFrequency = MAX_WORD_LEN;

    totalLetters = 0;
    letterFrequencies.clear();

    foreach (const QString& str, strList) {
        QChar letter = str.section(":", 0, 0)[0];
        int frequency = str.section(":", 1, 1).toInt();
        letterFrequencies.insert(letter, frequency);
        totalLetters += frequency;
        if (frequency > maxFrequency)
            maxFrequency = frequency;
    }

    // Precalculate M choose N combinations - use doubles because the numbers
    // get very large
    double a = 1;
    double r = 1;
    for (int i = 0; i <= maxFrequency; ++i, ++r) {
        fullChooseCombos.append(a);
        a *= (totalLetters + 1.0 - r) / r;

        QList<double> subList;
        for (int j = 0; j <= maxFrequency; ++j) {
            if ((i == j) || (j == 0))
                subList.append(1.0);
            else if (i == 0)
                subList.append(0.0);
            else {
                // XXX: For some reason this crashes on Linux when referencing
                // the first value as subChooseCombos[i-1][j-1], so value() is
                // used instead.  Weeeeird.
                subList.append(subChooseCombos.value(i-1).value(j-1) +
                               subChooseCombos.value(i-1).value(j));
            }
        }
        subChooseCombos.append(subList);
    }
}

//---------------------------------------------------------------------------
//  insertLetter
//
//! Insert a letter into the bag.
//
//! @param letter the letter
//---------------------------------------------------------------------------
void
LetterBag::insertLetter(const QChar& letter)
{
    QChar c = letter.toUpper();
    if (letterFrequencies.contains(c))
        ++letterFrequencies[c];
    else
        letterFrequencies[c] = 1;
    ++totalLetters;
}

//---------------------------------------------------------------------------
//  drawLetter
//
//! Draw a specific letter from the bag.  This even allows you to draw letters
//! that are not in the bag, or more of a letter than is in the bag.  The
//! reason for this is because racks are invented at certain points when
//! recreating an ISC game (particularly when a word has been challenged off).
//! We need to allow letters to be drawn multiple times and then replaced
//! after the challenge.
//
//! @param letter the letter
//! @return true if the letter can be drawn, false if it is not in the bag
//---------------------------------------------------------------------------
bool
LetterBag::drawLetter(const QChar& letter)
{
    QChar c = letter.toUpper();
    --letterFrequencies[c];
    --totalLetters;
    return true;
}

//---------------------------------------------------------------------------
//  lookRandomLetters
//
//! Look at random letters from the bag, but do not draw them.
//
//! @return the letters, or an empty character of the bag is empty
//---------------------------------------------------------------------------
QString
LetterBag::lookRandomLetters(int num)
{
    if (totalLetters < num)
        return QString();

    QString letters;
    unsigned int choose = num;
    unsigned int chooseFrom = totalLetters;

    QMapIterator<QChar, int> it (letterFrequencies);
    while (it.hasNext()) {
        it.next();
        int frequency = it.value();
        for (int i = 0; i < frequency; ++i, --chooseFrom) {
            unsigned int r = chooseFrom > 1 ? rng.rand(chooseFrom - 1) : 0;
            if (r >= choose)
                continue;

            QChar letter = it.key();
            letters += letter;
            --choose;
            if (!choose)
                return letters;
        }
    }

    return QString();
}

//---------------------------------------------------------------------------
//  drawRandomLetters
//
//! Draw random letters from the bag.
//
//! @return the letters, or an empty string if the bag is empty
//---------------------------------------------------------------------------
QString
LetterBag::drawRandomLetters(int num)
{
    QString letters = lookRandomLetters(num);
    for (int i = 0; i < letters.length(); ++i) {
        drawLetter(letters[i]);
    }
    return letters;
}

//---------------------------------------------------------------------------
//  getLetters
//
//! Return a string containing all the letters in the bag.
//
//! @return the string with letters alphabetized
//---------------------------------------------------------------------------
QString
LetterBag::getLetters() const
{
    QString string;
    QMapIterator <QChar, int> it (letterFrequencies);
    while (it.hasNext()) {
        it.next();
        QChar letter = it.key();
        int frequency = it.value();
        for (int i = 0; i < frequency; ++i) {
            string += letter;
        }
    }
    return string;
}

//---------------------------------------------------------------------------
//  getNumLetters
//
//! Return the number of letters in the bag.
//
//! @return the number of letters
//---------------------------------------------------------------------------
int
LetterBag::getNumLetters() const
{
    return totalLetters;
}
