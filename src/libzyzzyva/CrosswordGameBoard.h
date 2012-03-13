//---------------------------------------------------------------------------
// CrosswordGameBoard.h
//
// A class to represent a crossword game board.
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

#ifndef ZYZZYVA_CROSSWORD_GAME_BOARD_H
#define ZYZZYVA_CROSSWORD_GAME_BOARD_H

#include "CrosswordGameMove.h"
#include <QChar>
#include <QList>
#include <QObject>

class CrosswordGameBoard
{
    public:
    class Tile {
        public:
        Tile() : blank(false), playerNum(0), timesUsed(0), valid(false) { }
        Tile(const QChar& c, bool b, int p)
            : letter(c), blank(b), playerNum(p), timesUsed(0), valid(true) { }

        void setLetter(const QChar& c) { letter = c; valid = true; }
        void setBlank(bool b) { blank = b; valid = true; }
        void setPlayerNum(int p) { playerNum = p; }
        void incrementTimesUsed() { ++timesUsed; }
        void decrementTimesUsed() { --timesUsed; }

        QChar getLetter() const { return letter; }
        bool isBlank() const { return blank; }
        int getPlayerNum() const { return playerNum; }
        int getTimesUsed() const { return timesUsed; }
        bool isValid() const { return valid; }

        private:
        QChar letter;
        bool blank;
        int playerNum;
        int timesUsed;
        bool valid;
    };

    enum SquareType {
        Invalid = 0,
        NoBonus,
        DoubleLetter,
        TripleLetter,
        DoubleWord,
        TripleWord
    };

    public:
    CrosswordGameBoard();
    void clear();
    SquareType getSquareType(int row, int col) const;
    Tile getTile(int row, int col) const;
    int getNumRows() const;
    int getNumColumns() const;
    bool makeMove(const CrosswordGameMove& move, QString* lettersPlaced = 0);
    bool removeMove(const CrosswordGameMove& move, QString* lettersRemoved = 0);

    private:
    QList< QList<SquareType> > squareTypes;
    QList< QList<Tile> > tiles;
};

#endif // ZYZZYVA_CROSSWORD_GAME_BOARD_H
