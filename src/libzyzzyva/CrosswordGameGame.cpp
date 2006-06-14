//---------------------------------------------------------------------------
// CrosswordGameGame.cpp
//
// A class to represent a crossword game.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

#include "CrosswordGameGame.h"
#include "Defs.h"

#include <QtDebug>

using namespace Defs;

//---------------------------------------------------------------------------
//  CrosswordGameGame
//
//! Constructor.
//---------------------------------------------------------------------------
CrosswordGameGame::CrosswordGameGame()
{
    init();

    connect (&board, SIGNAL (changed()), SIGNAL (changed()));
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the contents of the board.
//---------------------------------------------------------------------------
void
CrosswordGameGame::clear()
{
    init();
}

//---------------------------------------------------------------------------
//  init
//
//! Initialize the object.
//---------------------------------------------------------------------------
void
CrosswordGameGame::init()
{
    playerToMove = 0;
    aPlayerRack = QString();
    bPlayerRack = QString();
    aPlayerScore = 0;
    bPlayerScore = 0;
    letterBag = LetterBag();
    board.clear();
    moveHistory.clear();
}

//---------------------------------------------------------------------------
//  makeMove
//
//! Make a move in the game.
//
//! @param move the move to be played
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
CrosswordGameGame::makeMove (const CrosswordGameMove& move)
{
    if (!board.makeMove (move))
        return false;

    switch (move.getPlayerNum()) {
        case 1: playerToMove = 2; break;
        case 2: playerToMove = 1; break;
        default:
        if (playerToMove == 1)
            playerToMove = 2;
        else if (playerToMove == 2)
            playerToMove = 1;
        break;
    }

    return true;
}

//---------------------------------------------------------------------------
//  getPlayerScore
//
//! Get the score for a player.
//
//! @param playerNum the player number
//! @return the player score, or -1 if invalid player number
//---------------------------------------------------------------------------
int
CrosswordGameGame::getPlayerScore (int playerNum)
{
    switch (playerNum) {
        case 1: return aPlayerScore;
        case 2: return bPlayerScore;
        default: return -1;
    }
}

//---------------------------------------------------------------------------
//  getPlayerRack
//
//! Get the rack for a player.
//
//! @param playerNum the player number
//! @return the player rack, or -1 if invalid player number
//---------------------------------------------------------------------------
QString
CrosswordGameGame::getPlayerRack (int playerNum)
{
    switch (playerNum) {
        case 1: return aPlayerRack;
        case 2: return bPlayerRack;
        default: return QString();
    }
}
