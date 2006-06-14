//---------------------------------------------------------------------------
// CrosswordGameGame.h
//
// A class to represent a crossword game.
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

#ifndef ZYZZYVA_CROSSWORD_GAME_GAME_H
#define ZYZZYVA_CROSSWORD_GAME_GAME_H

#include "CrosswordGameBoard.h"
#include "CrosswordGameMove.h"
#include "LetterBag.h"
#include <QChar>
#include <QList>
#include <QObject>

class CrosswordGameGame : public QObject
{
    Q_OBJECT

    public:
    CrosswordGameGame();
    void clear();
    bool makeMove (const CrosswordGameMove& move);

    int getPlayerToMove() const { return playerToMove; }
    int getPlayerScore (int playerNum);
    QString getPlayerRack (int playerNum);

    CrosswordGameBoard::SquareType getSquareType (int row, int col) const {
        return board.getSquareType (row, col);
    }
    CrosswordGameBoard::Tile getTile (int row, int col) const {
        return board.getTile (row, col);
    }
    int getNumRows() const { return board.getNumRows(); }
    int getNumColumns() const { return board.getNumColumns(); }

    signals:
    void changed();

    private:
    void init();

    private:
    int playerToMove;
    int aPlayerScore;
    int bPlayerScore;
    QString aPlayerRack;
    QString bPlayerRack;
    LetterBag letterBag;
    CrosswordGameBoard board;
    QList<CrosswordGameMove> moveHistory;
};

#endif // ZYZZYVA_CROSSWORD_GAME_GAME_H