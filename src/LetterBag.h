//---------------------------------------------------------------------------
// LetterBag.h
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

#ifndef ZYZZYVA_LETTER_BAG_H
#define ZYZZYVA_LETTER_BAG_H

#include <QChar>
#include <QMap>
#include <QList>
#include <QString>

class LetterBag
{
    public:
    LetterBag (const QString& distribution = QString::null);
    ~LetterBag() { }

    double getProbability (const QString& word) const;
    double getNumCombinations (const QString& word) const;

    private:
    int totalLetters;
    QMap<QChar, int> letterFrequencies;

    QList<double> fullChooseCombos;
    QList<QList<double> > subChooseCombos;
};

#endif // ZYZZYVA_LETTER_BAG_H

