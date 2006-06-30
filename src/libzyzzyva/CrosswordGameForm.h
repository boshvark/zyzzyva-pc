//---------------------------------------------------------------------------
// CrosswordGameForm.h
//
// A form for playing a crossword game.
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

#ifndef ZYZZYVA_CROSSWORD_GAME_FORM_H
#define ZYZZYVA_CROSSWORD_GAME_FORM_H

#include "ActionForm.h"
#include "CrosswordGameMove.h"
#include <QAbstractSocket>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QString>

class CrosswordGameBoardWidget;
class CrosswordGameGame;
class CrosswordGameRackWidget;
class IscConnectionThread;

class CrosswordGameForm : public ActionForm
{
    Q_OBJECT
    public:
    CrosswordGameForm (QWidget* parent = 0, Qt::WFlags f = 0);
    ~CrosswordGameForm();
    QString getStatusString() const;

    public slots:
    void connectClicked();
    void disconnectClicked();
    void inputReturnPressed();
    void threadStatusChanged (const QString& status);
    void threadMessageReceived (const QString& message);
    void threadSocketError (QAbstractSocket::SocketError error);
    void gameChanged();
    void clockTimeout (int playerNum);

    private:
    void messageAppendHtml (const QString& text, const QColor& color);
    QString encodeHtmlEntities (const QString& text);
    QString canonizeMessage (const QString& text);
    int minutesSecondsToSeconds (int minutes, int seconds,
                                 bool overtime = false);
    int secondsToMinutesSeconds (int totalSeconds, int& minutes, int& seconds,
                                 bool overtime = false);
    void setClock (int playerNum, int seconds);
    void decrementClock (int playerNum);
    void startClock (int playerNum);
    void stopClock (int playerNum);

    //void processGameAction (const QString& string);
    void processCommand (const QString& string);
    void processObserve (const QString& string);
    void processMove (const QString& string);
    void processLogin (const QString& string);
    void processResign (const QString& string);
    void makeMove (CrosswordGameMove& move);
    int scoreMove (const CrosswordGameMove& move);
    void fixMoveOvertime (CrosswordGameMove& move) const;

    bool appendMoves (QList<CrosswordGameMove>& moves, const QString& string,
                      int playerToMove) const;

    public:
    enum GameStatus {
        NoGame,
        ObservingGame,
        PlayingGame
    };

    private:
    CrosswordGameBoardWidget* boardWidget;
    QLabel* unseenLabel;
    QPushButton* connectButton;
    QPushButton* disconnectButton;
    QTextEdit* messageArea;
    QLineEdit* inputLine;
    QString statusString;

    QLabel* aPlayerLabel;
    QLabel* aRatingLabel;
    QLabel* aScoreLabel;
    QLabel* aTimerLabel;
    QLabel* bPlayerLabel;
    QLabel* bRatingLabel;
    QLabel* bScoreLabel;
    QLabel* bTimerLabel;
    CrosswordGameRackWidget* aRackWidget;
    CrosswordGameRackWidget* bRackWidget;

    // Shouldn't these really be somewhere else?
    QTimer aTimer;
    QTimer bTimer;
    int aSeconds;
    int bSeconds;
    bool aOvertime;
    bool bOvertime;

    int myPlayerNum;
    GameStatus gameStatus;
    CrosswordGameGame* game;
    IscConnectionThread* iscThread;
};

#endif // ZYZZYVA_CROSSWORD_GAME_FORM_H
