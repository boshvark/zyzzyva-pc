//---------------------------------------------------------------------------
// CrosswordGameMove.h
//
// A class to represent a crossword game move.
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

#ifndef ZYZZYVA_CROSSWORD_GAME_MOVE_H
#define ZYZZYVA_CROSSWORD_GAME_MOVE_H

#include "Auxil.h"
#include <QString>

class CrosswordGameMove
{
    public:
    enum Type {
        Invalid,
        DrawTiles,
        Move,
        Exchange,
        Challenge,
        TakeBack,
        Pass,
        Resign,
        TimePenalty,
        RackBonus,
        GameOver
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
    CrosswordGameMove(const QString& str);

    void setType(Type t) { type = t; }
    void setOrientation(Orientation o) { orientation = o; }
    void setRow(int r) { row = r; }
    void setColumn(int c) { column = c; }
    void setWord(const QString& w) { word = w; }
    void setScore(int s) { score = s; }
    void setSecondsLeft(int s) { secondsLeft = s; }
    void setNumExchanged(int n) { numExchanged = n; }
    void setExchangedLetters(const QString& s) {
        exchangedLetters = s;
        numExchanged = s.length();
    }
    void setPenaltyType(PenaltyType p) { penaltyType = p; }
    void setPenaltyPoints(int n) { penaltyPoints = n; }
    void setPlayerNum(int n) { playerNum = n; }
    void setNewRack(const QString& r) { newRack = Auxil::getAlphagram(r); }

    Type getType() const { return type; }
    Orientation getOrientation() const { return orientation; }
    int getRow() const { return row; }
    int getColumn() const { return column; }
    QString getPlacement() const;
    QString getWord() const { return word; }
    int getScore() const { return score; }
    int getSecondsLeft() const { return secondsLeft; }
    int getNumExchanged() const { return numExchanged; }
    QString getExchangedLetters() const { return exchangedLetters; }
    PenaltyType getPenaltyType() const { return penaltyType; }
    int getPenaltyPoints() const { return penaltyPoints; }
    int getPlayerNum() const { return playerNum; }
    QString getNewRack() const { return newRack; }
    bool isValid() const;

    private:
    void init();
    void setPlacement(const QString& placement);

    private:
    Type type;
    Orientation orientation;
    int row;
    int column;
    QString word;
    int score;
    int secondsLeft;
    int numExchanged;
    QString exchangedLetters;
    PenaltyType penaltyType;
    int penaltyPoints;
    int playerNum;
    QString newRack;
};

#endif // ZYZZYVA_CROSSWORD_GAME_MOVE_H
