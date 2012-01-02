//---------------------------------------------------------------------------
// CrosswordGameBoard.cpp
//
// A class to represent a crossword game board.
//
// Copyright 2006-2012 Boshvark Software, LLC.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "CrosswordGameBoard.h"
#include "Defs.h"

using namespace Defs;

const int NUM_ROWS = 15;
const int NUM_COLUMNS = 15;

//---------------------------------------------------------------------------
//  CrosswordGameBoard
//
//! Constructor.
//---------------------------------------------------------------------------
CrosswordGameBoard::CrosswordGameBoard()
{
    // Initialize tiles
    for (int row = 0; row < NUM_ROWS; ++row) {
        QList<Tile> rowTiles;
        for (int col = 0; col < NUM_COLUMNS; ++col) {
            rowTiles << Tile();
        }
        tiles << rowTiles;
    }

    // Initialize square types
    QList<SquareType> col1;
    col1 << TripleWord << NoBonus << NoBonus << DoubleLetter << NoBonus <<
        NoBonus << NoBonus << TripleWord << NoBonus << NoBonus <<
        NoBonus << DoubleLetter << NoBonus << NoBonus << TripleWord;

    QList<SquareType> col2;
    col2 << NoBonus << DoubleWord << NoBonus << NoBonus << NoBonus <<
        TripleLetter << NoBonus << NoBonus << NoBonus << TripleLetter <<
        NoBonus << NoBonus << NoBonus << DoubleWord << NoBonus;

    QList<SquareType> col3;
    col3 << NoBonus << NoBonus << DoubleWord << NoBonus << NoBonus <<
        NoBonus << DoubleLetter << NoBonus << DoubleLetter << NoBonus <<
        NoBonus << NoBonus << DoubleWord << NoBonus << NoBonus;

    QList<SquareType> col4;
    col4 << DoubleLetter << NoBonus << NoBonus << DoubleWord << NoBonus <<
        NoBonus << NoBonus << DoubleLetter << NoBonus << NoBonus <<
        NoBonus << DoubleWord << NoBonus << NoBonus << DoubleLetter;

    QList<SquareType> col5;
    col5 << NoBonus << NoBonus << NoBonus << NoBonus << DoubleWord <<
        NoBonus << NoBonus << NoBonus << NoBonus << NoBonus <<
        DoubleWord << NoBonus << NoBonus << NoBonus << NoBonus;

    QList<SquareType> col6;
    col6 << NoBonus << TripleLetter << NoBonus << NoBonus << NoBonus <<
        TripleLetter << NoBonus << NoBonus << NoBonus << TripleLetter <<
        NoBonus << NoBonus << NoBonus << TripleLetter << NoBonus;

    QList<SquareType> col7;
    col7 << NoBonus << NoBonus << DoubleLetter << NoBonus << NoBonus <<
        NoBonus << DoubleLetter << NoBonus << DoubleLetter << NoBonus <<
        NoBonus << NoBonus << DoubleLetter << NoBonus << NoBonus;

    QList<SquareType> col8;
    col8 << TripleWord << NoBonus << NoBonus << DoubleLetter << NoBonus <<
        NoBonus << NoBonus << DoubleWord << NoBonus << NoBonus <<
        NoBonus << DoubleLetter << NoBonus << NoBonus << TripleWord;

    squareTypes << col1 << col2 << col3 << col4 << col5 << col6 << col7 <<
        col8 << col7 << col6 << col5 << col4 << col3 << col2 << col1;
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the contents of the board.
//---------------------------------------------------------------------------
void
CrosswordGameBoard::clear()
{
    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLUMNS; ++col) {
            tiles[row][col] = Tile();
        }
    }
}

//---------------------------------------------------------------------------
//  getSquareType
//
//! Get the square type for a row and column.
//
//! @param row the row number
//! @param col the column number
//! @return the square type
//---------------------------------------------------------------------------
CrosswordGameBoard::SquareType
CrosswordGameBoard::getSquareType(int row, int col) const
{
    if ((row < 0) || (row > NUM_ROWS) || (col < 0) || (col > NUM_COLUMNS))
        return Invalid;

    return squareTypes[row][col];
}

//---------------------------------------------------------------------------
//  getTile
//
//! Get the tile for a row and column.
//
//! @param row the row number
//! @param col the column number
//! @return the square type
//---------------------------------------------------------------------------
CrosswordGameBoard::Tile
CrosswordGameBoard::getTile(int row, int col) const
{
    if ((row < 0) || (row > NUM_ROWS) || (col < 0) || (col > NUM_COLUMNS))
        return Tile();

    return tiles[row][col];
}

//---------------------------------------------------------------------------
//  getNumRows
//
//! Get the number of rows.
//
//! @return the number of rows
//---------------------------------------------------------------------------
int
CrosswordGameBoard::getNumRows() const
{
    return NUM_ROWS;
}

//---------------------------------------------------------------------------
//  getNumColumns
//
//! Get the number of columns.
//
//! @return the number of columns
//---------------------------------------------------------------------------
int
CrosswordGameBoard::getNumColumns() const
{
    return NUM_COLUMNS;
}

//---------------------------------------------------------------------------
//  makeMove
//
//! Make a move on the board.
//
//! @param move the move to be played
//! @param lettersPlaced return a list of letters placed on the board
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
CrosswordGameBoard::makeMove(const CrosswordGameMove& move,
                             QString* lettersPlaced)
{
    if (!move.isValid())
        return false;

    CrosswordGameMove::Orientation orientation = move.getOrientation();
    int row = move.getRow();
    int col = move.getColumn();
    QString word = move.getWord();

    for (int i = 0; i < word.length(); ++i) {
        if (!tiles[row][col].isValid()) {
            QChar letter = word[i];
            tiles[row][col].setLetter(letter.toUpper());
            tiles[row][col].setBlank(letter.isUpper());
            tiles[row][col].setPlayerNum(move.getPlayerNum());
            if (lettersPlaced) {
                (*lettersPlaced) += (letter.isUpper() ? QChar('?')
                                                      : letter.toUpper());
            }
        }
        tiles[row][col].incrementTimesUsed();

        if (orientation == CrosswordGameMove::Horizontal)
            ++col;
        else
            ++row;
    }

    return true;
}

//---------------------------------------------------------------------------
//  removeMove
//
//! Remove a move from the board.
//
//! @param move the move to be removed
//! @param lettersRemoved return a list of letters removed from the board
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
CrosswordGameBoard::removeMove(const CrosswordGameMove& move,
                               QString* lettersRemoved)
{
    if (!move.isValid())
        return false;

    CrosswordGameMove::Orientation orientation = move.getOrientation();
    int row = move.getRow();
    int col = move.getColumn();
    QString word = move.getWord();

    for (int i = 0; i < word.length(); ++i) {
        tiles[row][col].decrementTimesUsed();
        if (tiles[row][col].getTimesUsed() <= 0) {
            if (lettersRemoved) {
                (*lettersRemoved) += tiles[row][col].isBlank() ? QChar('?')
                                     : tiles[row][col].getLetter();
            }
            tiles[row][col] = Tile();
        }

        if (orientation == CrosswordGameMove::Horizontal)
            ++col;
        else
            ++row;
    }

    return true;
}
