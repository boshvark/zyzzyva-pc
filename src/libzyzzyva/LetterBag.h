//---------------------------------------------------------------------------
// LetterBag.h
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

#ifndef ZYZZYVA_LETTER_BAG_H
#define ZYZZYVA_LETTER_BAG_H

#include "Rand.h"
#include <QChar>
#include <QMap>
#include <QList>
#include <QString>

class LetterBag
{
    public:
    LetterBag(const QString& distribution = QString());
    ~LetterBag() { }

    double getProbability(const QString& word, int numBlanks) const;
    double getNumCombinations(const QString& word, int numBlanks) const;

    int getLetterValue(const QChar& letter) const;
    void setLetterValue(const QChar& letter, int value);

    void resetContents(const QString& distribution = QString());
    void insertLetter(const QChar& letter);
    bool drawLetter(const QChar& letter);
    QString lookRandomLetters(int num);
    QString drawRandomLetters(int num);

    QString getLetters() const;
    int getNumLetters() const;

    private:
    int totalLetters;
    QMap<QChar, int> letterFrequencies;
    QMap<QChar, int> letterValues;

    QList<double> fullChooseCombos;
    QList<QList<double> > subChooseCombos;
    Rand rng;

    public:
    static const QChar BLANK_CHAR;
};

#endif // ZYZZYVA_LETTER_BAG_H

