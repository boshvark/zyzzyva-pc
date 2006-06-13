//---------------------------------------------------------------------------
// CrosswordGameMove.h
//
// A class to represent a crossword game move.
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

#ifndef ZYZZYVA_CROSSWORD_GAME_MOVE_H
#define ZYZZYVA_CROSSWORD_GAME_MOVE_H

#include <QString>

class CrosswordGameMove
{
    public:
    enum Type {
        Invalid,
        Move,
        Exchange,
        Challenge,
        Pass
    };

    enum Orientation {
        NoOrientation,
        Horizontal,
        Vertical
    };

    enum PenaltyType {
        NoPenalty,
        LoseTurn,
        LosePoints
    };

    public:
    CrosswordGameMove() { init(); }
    CrosswordGameMove (const QString& str);

    void setType (Type t) { type = t; }
    void setOrientation (Orientation o) { orientation = o; }
    void setRow (int r) { row = r; }
    void setColumn (int c) { column = c; }
    void setWord (const QString& w) { word = w; }
    void setNumExchanged (int n) { numExchanged = n; }
    void setExchangedLetters (const QString& s) {
        exchangedLetters = s;
        numExchanged = s.length();
    }
    void setPenaltyType (PenaltyType p) { penaltyType = p; }
    void setPenaltyPoints (int n) { penaltyPoints = n; }

    Type getType() const { return type; }
    Orientation getOrientation() const { return orientation; }
    int getRow() const { return row; }
    int getColumn() const { return column; }
    QString getWord() const { return word; }
    int getNumExchanged() const { return numExchanged; }
    QString getExchangedLetters() const { return exchangedLetters; }
    PenaltyType getPenaltyType() const { return penaltyType; }
    int getPenaltyPoints() const { return penaltyPoints; }
    bool isValid() const;

    private:
    void init();
    QString translateCoordinates (const QString& coordinates);

    private:
    Type type;
    Orientation orientation;
    int row;
    int column;
    QString word;
    int numExchanged;
    QString exchangedLetters;
    PenaltyType penaltyType;
    int penaltyPoints;
};

#endif // ZYZZYVA_CROSSWORD_GAME_MOVE_H
