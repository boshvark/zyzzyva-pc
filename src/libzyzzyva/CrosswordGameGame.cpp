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
    emit changed();
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
    if (move.getType() == CrosswordGameMove::TakeBack)
        return challengeLastMove();

    if (!board.makeMove (move))
        return false;

    bool pass = (move.getType() == CrosswordGameMove::Pass);
    CrosswordGameMove newMove = move;

    switch (newMove.getPlayerNum()) {
        case 1:
        aPlayerScore += newMove.getScore();
        if (pass)
            newMove.setNewRack (aPlayerRack);
        else
            aPlayerRack = newMove.getNewRack();
        break;

        case 2:
        bPlayerScore += newMove.getScore();
        if (pass)
            newMove.setNewRack (bPlayerRack);
        else
            bPlayerRack = newMove.getNewRack();
        break;

        default:
        if (playerToMove == 1) {
            aPlayerScore += newMove.getScore();
            if (pass)
                newMove.setNewRack (aPlayerRack);
            else
                aPlayerRack = newMove.getNewRack();
        }
        else if (playerToMove == 2) {
            bPlayerScore += newMove.getScore();
            if (pass)
                newMove.setNewRack (bPlayerRack);
            else
                bPlayerRack = newMove.getNewRack();
        }
        break;
    }

    playerToMove = (newMove.getPlayerNum() == 1) ? 2 : 1;
    moveHistory.append (newMove);
    emit changed();
    return true;
}

//---------------------------------------------------------------------------
//  getRackValue
//
//! Calculate the total point value of all the tiles on a rack.
//
//! @param rack the rack of letters
//! @return the total value
//---------------------------------------------------------------------------
int
CrosswordGameGame::getRackValue (const QString& rack) const
{
    int value = 0;
    for (int i = 0; i < rack.length(); ++i) {
        QChar letter = rack[i];
        if (letter == '?')
            letter = LetterBag::BLANK_CHAR;
        value += letterBag.getLetterValue (letter);
    }
    return value;
}

//---------------------------------------------------------------------------
//  challengeLastMove
//
//! Mark the last move as challenged, and remove it from the board.
//
//! @param playerNum the player number
//! @return the player score, or -1 if invalid player number
//---------------------------------------------------------------------------
bool
CrosswordGameGame::challengeLastMove()
{
    if (moveHistory.isEmpty())
        return false;

    QListIterator<CrosswordGameMove> it (moveHistory);
    it.toBack();

    CrosswordGameMove challengedMove = it.previous();
    int challengedPlayer = challengedMove.getPlayerNum();

    // Restore previous rack and score
    while (it.hasPrevious()) {
        CrosswordGameMove oldMove = it.previous();
        if (oldMove.getPlayerNum() != challengedPlayer)
            continue;

        QString oldRack = oldMove.getNewRack();
        if (challengedPlayer == 1) {
            aPlayerScore -= challengedMove.getScore();
            aPlayerRack = oldRack;
        }
        else {
            bPlayerScore -= challengedMove.getScore();
            bPlayerRack = oldRack;
        }

        // FIXME: this is kind of redundant when challengeLastMove is called
        // from makeMove
        CrosswordGameMove takeBackMove = challengedMove;
        takeBackMove.setType (CrosswordGameMove::TakeBack);
        takeBackMove.setScore (-challengedMove.getScore());
        takeBackMove.setNewRack (oldRack);
        moveHistory.append (takeBackMove);

        board.removeMove (challengedMove);
        break;
    }

    emit changed();
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
