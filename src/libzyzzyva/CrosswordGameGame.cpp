//---------------------------------------------------------------------------
// CrosswordGameGame.cpp
//
// A class to represent a crossword game.
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
//! Make a move in the game.  May need to calculate score, new rack, etc.
//
//! @param move the move to be played
//! @return true if successful, false otherwise
//---------------------------------------------------------------------------
bool
CrosswordGameGame::makeMove(CrosswordGameMove& move)
{
    if (move.getType() == CrosswordGameMove::TakeBack)
        return challengeLastMove();

    QString lettersPlaced;
    if (!board.makeMove(move, &lettersPlaced))
        return false;

    bool pass = (move.getType() == CrosswordGameMove::Pass);
    CrosswordGameMove newMove = move;
    QString newRack;
    QString oldRack;

    switch (newMove.getPlayerNum()) {
        case 1:
        oldRack = aPlayerRack.toUpper();
        aPlayerScore += newMove.getScore();
        if (pass)
            newMove.setNewRack(aPlayerRack);
        else
            aPlayerRack = newMove.getNewRack();
        newRack = aPlayerRack.toUpper();
        break;

        case 2:
        oldRack = bPlayerRack.toUpper();
        bPlayerScore += newMove.getScore();
        if (pass)
            newMove.setNewRack(bPlayerRack);
        else
            bPlayerRack = newMove.getNewRack();
        newRack = bPlayerRack.toUpper();
        break;

        default:
        if (playerToMove == 1) {
            oldRack = aPlayerRack.toUpper();
            aPlayerScore += newMove.getScore();
            if (pass)
                newMove.setNewRack(aPlayerRack);
            else
                aPlayerRack = newMove.getNewRack();
            newRack = aPlayerRack.toUpper();
        }
        else if (playerToMove == 2) {
            oldRack = bPlayerRack.toUpper();
            bPlayerScore += newMove.getScore();
            if (pass)
                newMove.setNewRack(bPlayerRack);
            else
                bPlayerRack = newMove.getNewRack();
            newRack = bPlayerRack.toUpper();
        }
        break;
    }

    // Draw tiles from the bag
    if (!lettersPlaced.isEmpty()) {
        lettersPlaced = Auxil::getAlphagram(lettersPlaced);
        oldRack = Auxil::getAlphagram(oldRack);

        qDebug() << "Old rack was: " << oldRack;
        qDebug() << "Letters placed were: " << lettersPlaced;

        QString lettersLeft;
        int p = 0;
        for (int i = 0; i < oldRack.length(); ++i) {
            if ((p == lettersPlaced.length()) ||
                (oldRack[i] != lettersPlaced[p]))
                lettersLeft += oldRack[i];
            else
                ++p;
        }

        qDebug() << "Letters left were: " << lettersLeft;

        // Choose random tiles unless specified
        if (newRack.isEmpty()) {

            int numInBag = letterBag.getNumLetters();
            int numToDraw = 7 - lettersLeft.length();
            if (numToDraw > numInBag)
                numToDraw = numInBag;

            QString newLetters = letterBag.drawRandomLetters(numToDraw);
            qDebug() << "Draw random letters: " << newLetters;

            newRack = Auxil::getAlphagram(lettersLeft + newLetters);
            qDebug() << "New rack is: " << newRack;

            switch (newMove.getPlayerNum()) {
                case 1: aPlayerRack = newRack; break;
                case 2: bPlayerRack = newRack; break;
                default: break;
            }
        }

        else {
            newRack = Auxil::getAlphagram(newRack);
            int q = 0;
            for (int i = 0; i < newRack.length(); ++i) {
                if ((q == lettersLeft.length()) ||
                    (newRack[i] != lettersLeft[q]))
                {
                    QChar c = newRack[i];
                    if (c == '?')
                        c = LetterBag::BLANK_CHAR;
                    letterBag.drawLetter(c);
                }
                else
                    ++q;
            }
        }
    }

    else if (newMove.getType() == CrosswordGameMove::DrawTiles) {
        // Choose random tiles unless specified
        if (newRack.isEmpty())
            newRack = letterBag.lookRandomLetters(7);

        for (int i = 0; i < newRack.length(); ++i) {
            QChar c = newRack[i].toUpper();
            if (c == '?')
                c = LetterBag::BLANK_CHAR;
            letterBag.drawLetter(c);
        }
    }

    else if (newMove.getType() == CrosswordGameMove::Exchange) {
        // Choose random tiles unless specified
        if (newRack.isEmpty()) {
            QString exchanged = newMove.getExchangedLetters();

            // FIXME: do something here to effect the exchange


        }

        else {
            for (int i = 0; i < oldRack.length(); ++i) {
                QChar c = oldRack[i].toUpper();
                if (c == '?')
                    c = LetterBag::BLANK_CHAR;
                letterBag.insertLetter(c);
            }
            for (int i = 0; i < newRack.length(); ++i) {
                QChar c = newRack[i].toUpper();
                if (c == '?')
                    c = LetterBag::BLANK_CHAR;
                letterBag.drawLetter(c);
            }
        }
    }

    playerToMove = (newMove.getPlayerNum() == 1) ? 2 : 1;
    moveHistory.append(newMove);
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
CrosswordGameGame::getRackValue(const QString& rack) const
{
    int value = 0;
    for (int i = 0; i < rack.length(); ++i) {
        QChar letter = rack[i];
        if (letter == '?')
            letter = LetterBag::BLANK_CHAR;
        value += letterBag.getLetterValue(letter);
    }
    return value;
}

//---------------------------------------------------------------------------
//  getTilesInBag
//
//! Get a string of tiles that are in the bag.
//
//! @return a string containing all tiles in the bag
//---------------------------------------------------------------------------
QString
CrosswordGameGame::getTilesInBag() const
{
    QString string = letterBag.getLetters();
    string = string.replace(LetterBag::BLANK_CHAR, "?");
    return Auxil::getAlphagram(string);
}

//---------------------------------------------------------------------------
//  getUnseenTiles
//
//! Get a string of tiles that are unseen from a player's point of view.
//
//! @param playerNum the player
//! @return a string containing all unseen tiles
//---------------------------------------------------------------------------
QString
CrosswordGameGame::getUnseenTiles(int playerNum) const
{
    QString bag = letterBag.getLetters();
    bag.replace(LetterBag::BLANK_CHAR, QChar('?'));
    bag = Auxil::getAlphagram(bag);
    qDebug() << "Contents of bag: " << bag;

    QString oppRack = (playerNum == 1 ? bPlayerRack : aPlayerRack);
    oppRack = Auxil::getAlphagram(oppRack.toUpper());

    QString unseen;
    int a = 0;
    int b = 0;
    while ((a < bag.length()) || (b < oppRack.length())) {
        if (a == bag.length()) {
            unseen += oppRack[b];
            ++b;
        }
        else if (b == oppRack.length()) {
            unseen += bag[a];
            ++a;
        }
        else if (bag[a] <= oppRack[b]) {
            unseen += bag[a];
            ++a;
        }
        else {
            unseen += oppRack[b];
            ++b;
        }
    }

    unseen.replace(LetterBag::BLANK_CHAR, "?");
    return unseen;
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
    qDebug() << "Challenged player: " << challengedPlayer;
    qDebug() << "Challenged word: " << challengedMove.getWord();
    bool replaceTiles = true;

    // Restore previous rack and score
    while (it.hasPrevious()) {
        qDebug() << "Checking previous move";
        CrosswordGameMove oldMove = it.previous();

        // Skip over moves by the challenging player when looking for the
        // previous rac
        if (oldMove.getPlayerNum() != challengedPlayer) {
            replaceTiles = false;
            continue;
        }

        qDebug() << "Player A rack: " << aPlayerRack;
        qDebug() << "Player B rack: " << bPlayerRack;

        QString oldRack;
        QString newRack = oldMove.getNewRack();
        if (challengedPlayer == 1) {
            oldRack = aPlayerRack;
            aPlayerScore -= challengedMove.getScore();
            aPlayerRack = newRack;
        }
        else {
            oldRack = bPlayerRack;
            bPlayerScore -= challengedMove.getScore();
            bPlayerRack = newRack;
        }

        // FIXME: this is kind of redundant when challengeLastMove is called
        // from makeMove
        CrosswordGameMove takeBackMove = challengedMove;
        takeBackMove.setType(CrosswordGameMove::TakeBack);
        takeBackMove.setScore(-challengedMove.getScore());
        takeBackMove.setNewRack(newRack);
        moveHistory.append(takeBackMove);

        QString lettersRemoved;
        board.removeMove(challengedMove, &lettersRemoved);

        if (!lettersRemoved.isEmpty()) {
            lettersRemoved = Auxil::getAlphagram(lettersRemoved);
            newRack = Auxil::getAlphagram(newRack.toUpper());


            // Find removed letters in new rack
            qDebug() << "New rack was: " << newRack;
            qDebug() << "Letters removed were: " << lettersRemoved;

            QString lettersLeft;
            int p = 0;
            for (int i = 0; i < newRack.length(); ++i) {
                if ((p == lettersRemoved.length()) ||
                    (newRack[i] != lettersRemoved[p]))
                    lettersLeft += newRack[i];
                else
                    ++p;
            }

            qDebug() << "Letters left: " << lettersLeft;

            // Find letters in old rack not left on the rack
            oldRack = Auxil::getAlphagram(oldRack.toUpper());
            qDebug() << "Old rack was: " << oldRack;
            int q = 0;
            for (int i = 0; i < oldRack.length(); ++i) {
                if ((q == lettersLeft.length()) ||
                    (oldRack[i] != lettersLeft[q]))
                {
                    QChar c = oldRack[i];
                    if (c == '?')
                        c = LetterBag::BLANK_CHAR;
                    qDebug() << "Replacing letter: " << c;
                    letterBag.insertLetter(c);
                }
                else
                    ++q;
            }
        }
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
CrosswordGameGame::getPlayerScore(int playerNum)
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
CrosswordGameGame::getPlayerRack(int playerNum)
{
    switch (playerNum) {
        case 1: return aPlayerRack;
        case 2: return bPlayerRack;
        default: return QString();
    }
}
