//---------------------------------------------------------------------------
// CrosswordGameBoard.h
//
// A class to represent a crossword game board.
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

#ifndef ZYZZYVA_CROSSWORD_GAME_BOARD_H
#define ZYZZYVA_CROSSWORD_GAME_BOARD_H

#include <QChar>
#include <QList>

class CrosswordGameBoard
{
    class Tile {
        public:
        QChar letter;
        bool isBlank;
    };

    public:
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
    SquareType getSquareType (int row, int col) const;
    int getNumRows() const;
    int getNumColumns() const;

    private:
    void initSquareTypes();

    private:
    QList< QList<SquareType> > squareTypes;
    QList< QList<Tile> > tiles;
};

#endif // ZYZZYVA_CROSSWORD_GAME_BOARD_H
