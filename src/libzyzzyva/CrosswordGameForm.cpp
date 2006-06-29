//---------------------------------------------------------------------------
// CrosswordGameForm.cpp
//
// A form for playing a crossword game.
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

#include "CrosswordGameForm.h"
#include "CrosswordGameBoardWidget.h"
#include "CrosswordGameGame.h"
#include "CrosswordGameMove.h"
#include "CrosswordGameRackWidget.h"
#include "IscConnectionThread.h"
#include "IscConverter.h"
#include "Auxil.h"
#include "Defs.h"
#include <QApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSignalMapper>
#include <QTextCursor>
#include <QVBoxLayout>

#include <QtDebug>

using namespace Defs;

const int BOARD_COLUMN_WIDTH = 30;
const int BOARD_ROW_HEIGHT = 30;
const int PLAYER_FONT_PIXEL_SIZE = 15;
const int TIMER_FONT_PIXEL_SIZE = 15;
const int SCORE_FONT_PIXEL_SIZE = 20;

//---------------------------------------------------------------------------
//  CrosswordGameForm
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CrosswordGameForm::CrosswordGameForm (QWidget* parent, Qt::WFlags f)
    : ActionForm (CrosswordGameFormType, parent, f),
      aSeconds (0), bSeconds (0), aOvertime (false), bOvertime (false),
      playingGame (false), game (new CrosswordGameGame()), iscThread (0)
{
    QFont playerFont = qApp->font();
    playerFont.setPixelSize (PLAYER_FONT_PIXEL_SIZE);

    QFont timerFont = qApp->font();
    timerFont.setPixelSize (TIMER_FONT_PIXEL_SIZE);

    QFont scoreFont = qApp->font();
    scoreFont.setPixelSize (SCORE_FONT_PIXEL_SIZE);
    scoreFont.setWeight (QFont::Bold);

    QHBoxLayout* mainHlay = new QHBoxLayout (this);
    Q_CHECK_PTR (mainHlay);
    mainHlay->setMargin (MARGIN);
    mainHlay->setSpacing (SPACING);

    QVBoxLayout* boardVlay = new QVBoxLayout;
    Q_CHECK_PTR (boardVlay);
    boardVlay->setMargin (MARGIN);
    boardVlay->setSpacing (SPACING);
    mainHlay->addLayout (boardVlay);

    QHBoxLayout* playerHlay = new QHBoxLayout;
    Q_CHECK_PTR (playerHlay);
    boardVlay->addLayout (playerHlay);

    QVBoxLayout* aPlayerVlay = new QVBoxLayout;
    Q_CHECK_PTR (aPlayerVlay);
    playerHlay->addLayout (aPlayerVlay);

    QHBoxLayout* aPlayerNameHlay = new QHBoxLayout;
    Q_CHECK_PTR (aPlayerNameHlay);
    aPlayerVlay->addLayout (aPlayerNameHlay);

    aPlayerLabel = new QLabel;
    Q_CHECK_PTR (aPlayerLabel);
    aPlayerLabel->setAlignment (Qt::AlignRight | Qt::AlignVCenter);
    aPlayerLabel->setFont (playerFont);
    aPlayerNameHlay->addWidget (aPlayerLabel);

    aRatingLabel = new QLabel;
    Q_CHECK_PTR (aRatingLabel);
    aRatingLabel->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);
    aRatingLabel->setFont (playerFont);
    aPlayerNameHlay->addWidget (aRatingLabel);

    QHBoxLayout* aPlayerStatsHlay = new QHBoxLayout;
    Q_CHECK_PTR (aPlayerStatsHlay);
    aPlayerVlay->addLayout (aPlayerStatsHlay);

    aScoreLabel = new QLabel;
    Q_CHECK_PTR (aScoreLabel);
    aScoreLabel->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);
    aScoreLabel->setFont (scoreFont);
    aPlayerStatsHlay->addWidget (aScoreLabel);

    aTimerLabel = new QLabel;
    Q_CHECK_PTR (aTimerLabel);
    aTimerLabel->setAlignment (Qt::AlignRight | Qt::AlignVCenter);
    aTimerLabel->setFont (timerFont);
    aPlayerStatsHlay->addWidget (aTimerLabel);

    QHBoxLayout* aPlayerRackHlay = new QHBoxLayout;
    Q_CHECK_PTR (aPlayerRackHlay);
    aPlayerVlay->addLayout (aPlayerRackHlay);

    aRackWidget = new CrosswordGameRackWidget (1, this);
    Q_CHECK_PTR (aRackWidget);
    aPlayerRackHlay->addWidget (aRackWidget);

    playerHlay->addStretch (1);

    QVBoxLayout* bPlayerVlay = new QVBoxLayout;
    Q_CHECK_PTR (bPlayerVlay);
    playerHlay->addLayout (bPlayerVlay);

    QHBoxLayout* bPlayerNameHlay = new QHBoxLayout;
    Q_CHECK_PTR (bPlayerNameHlay);
    bPlayerVlay->addLayout (bPlayerNameHlay);

    bPlayerLabel = new QLabel;
    Q_CHECK_PTR (bPlayerLabel);
    bPlayerLabel->setAlignment (Qt::AlignRight | Qt::AlignVCenter);
    bPlayerLabel->setFont (playerFont);
    bPlayerNameHlay->addWidget (bPlayerLabel);

    bRatingLabel = new QLabel;
    Q_CHECK_PTR (bRatingLabel);
    bRatingLabel->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);
    bRatingLabel->setFont (playerFont);
    bPlayerNameHlay->addWidget (bRatingLabel);

    QHBoxLayout* bPlayerStatsHlay = new QHBoxLayout;
    Q_CHECK_PTR (bPlayerStatsHlay);
    bPlayerVlay->addLayout (bPlayerStatsHlay);

    bTimerLabel = new QLabel;
    Q_CHECK_PTR (bTimerLabel);
    bTimerLabel->setAlignment (Qt::AlignLeft | Qt::AlignVCenter);
    bTimerLabel->setFont (timerFont);
    bPlayerStatsHlay->addWidget (bTimerLabel);

    bScoreLabel = new QLabel;
    Q_CHECK_PTR (bScoreLabel);
    bScoreLabel->setAlignment (Qt::AlignRight | Qt::AlignVCenter);
    bScoreLabel->setFont (scoreFont);
    bPlayerStatsHlay->addWidget (bScoreLabel);

    QHBoxLayout* bPlayerRackHlay = new QHBoxLayout;
    Q_CHECK_PTR (bPlayerRackHlay);
    bPlayerVlay->addLayout (bPlayerRackHlay);

    bRackWidget = new CrosswordGameRackWidget (2, this);
    Q_CHECK_PTR (bRackWidget);
    bPlayerRackHlay->addWidget (bRackWidget);

    QHBoxLayout* boardHlay = new QHBoxLayout;
    Q_CHECK_PTR (boardHlay);
    boardVlay->addLayout (boardHlay);

    boardWidget = new CrosswordGameBoardWidget (game, this);
    Q_CHECK_PTR (boardWidget);
    boardHlay->addWidget (boardWidget);

    QHBoxLayout* buttonHlay = new QHBoxLayout;
    Q_CHECK_PTR (buttonHlay);
    boardVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    connectButton = new QPushButton ("&Connect", this);
    Q_CHECK_PTR (connectButton);
    connect (connectButton, SIGNAL (clicked()), SLOT (connectClicked()));
    buttonHlay->addWidget (connectButton);

    disconnectButton = new QPushButton ("&Disconnect", this);
    Q_CHECK_PTR (disconnectButton);
    connect (disconnectButton, SIGNAL (clicked()), SLOT (disconnectClicked()));
    disconnectButton->setEnabled (false);
    buttonHlay->addWidget (disconnectButton);

    buttonHlay->addStretch (1);

    QVBoxLayout* messageVlay = new QVBoxLayout;
    Q_CHECK_PTR (messageVlay);
    messageVlay->setMargin (0);
    messageVlay->setSpacing (SPACING);
    mainHlay->addLayout (messageVlay);

    unseenLabel = new QLabel;
    Q_CHECK_PTR (unseenLabel);
    unseenLabel->setWordWrap (true);
    messageVlay->addWidget (unseenLabel);

    messageArea = new QTextEdit (this);
    Q_CHECK_PTR (messageArea);
    messageArea->setReadOnly (true);
    messageVlay->addWidget (messageArea);

    inputLine = new QLineEdit (this);
    Q_CHECK_PTR (inputLine);
    connect (inputLine, SIGNAL (returnPressed()),
             SLOT (inputReturnPressed()));
    messageVlay->addWidget (inputLine);

    // Set up connections for clock timers
    QSignalMapper* clockSignalMapper = new QSignalMapper (this);
    Q_CHECK_PTR (clockSignalMapper);
    connect (clockSignalMapper, SIGNAL (mapped (int)),
             SLOT (clockTimeout (int)));
    connect (&aTimer, SIGNAL (timeout()), clockSignalMapper, SLOT (map()));
    clockSignalMapper->setMapping (&aTimer, 1);
    connect (&bTimer, SIGNAL (timeout()), clockSignalMapper, SLOT (map()));
    clockSignalMapper->setMapping (&bTimer, 2);

    connect (game, SIGNAL (changed()), SLOT (gameChanged()));
}

//---------------------------------------------------------------------------
//  ~CrosswordGameForm
//
//! Destructor.
//---------------------------------------------------------------------------
CrosswordGameForm::~CrosswordGameForm()
{
}

//---------------------------------------------------------------------------
//  getStatusString
//
//! Returns the current status string.
//
//! @return the current status string
//---------------------------------------------------------------------------
QString
CrosswordGameForm::getStatusString() const
{
    return statusString;
}

//---------------------------------------------------------------------------
//  connectClicked
//
//! Called when the Connect button is clicked.
//---------------------------------------------------------------------------
void
CrosswordGameForm::connectClicked()
{
    if (!iscThread)
        iscThread = new IscConnectionThread (this);

    if (iscThread->isRunning())
        return;

    QFile file (Auxil::getUserConfigDir() + "/isc-creds");
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
        return;

    connect (iscThread, SIGNAL (messageReceived (const QString&)),
             SLOT (threadMessageReceived (const QString&)));
    connect (iscThread, SIGNAL (statusChanged (const QString&)),
             SLOT (threadStatusChanged (const QString&)));
    connect (iscThread, SIGNAL (socketError (QAbstractSocket::SocketError)),
             SLOT (threadSocketError (QAbstractSocket::SocketError)));

    QByteArray credBytes = file.readLine();
    QString credStr (credBytes);
    if (!iscThread->connectToServer (credStr.simplified()))
        return;

    connectButton->setEnabled (false);
    disconnectButton->setEnabled (true);
}

//---------------------------------------------------------------------------
//  disconnectClicked
//
//! Called when the Disconnect button is clicked.
//---------------------------------------------------------------------------
void
CrosswordGameForm::disconnectClicked()
{
    if (!iscThread)
        return;

    iscThread->disconnectFromServer();
    iscThread->quit();
    delete iscThread;
    iscThread = 0;

    disconnectButton->setEnabled (false);
    connectButton->setEnabled (true);
}

//---------------------------------------------------------------------------
//  inputReturnPressed
//
//! Called when return is pressed in the input line.
//---------------------------------------------------------------------------
void
CrosswordGameForm::inputReturnPressed()
{
    QString text = inputLine->text();
    if (text.isEmpty())
        return;

    text = canonizeMessage (text);
    messageAppendHtml (text, QColor (0x00, 0x00, 0xff));
    iscThread->sendMessage (text);

    inputLine->clear();
}

//---------------------------------------------------------------------------
//  threadStatusChanged
//
//! Called when a status message is received from the connection thread.
//
//! @param status the status message
//---------------------------------------------------------------------------
void
CrosswordGameForm::threadStatusChanged (const QString& status)
{
    statusString = status;
    emit statusChanged (status);
}

//---------------------------------------------------------------------------
//  threadMessageReceived
//
//! Called when a message is received from the connection thread.
//
//! @param message the message
//---------------------------------------------------------------------------
void
CrosswordGameForm::threadMessageReceived (const QString& message)
{
    qDebug() << "*** threadMessageReceived: " << message;

    QString command = message.section (" ", 0, 0);
    QString args = message.section (" ", 1);

    // A hush fills the room as olaugh walks in! :)
    // WHO BEST 1877 olaugh a 0 0

    // trey is playing drbing now!
    // GAMES BEST 1926 1922 trey 1930 drbing TWL98 18 0 n

    // auto-logout after 60 minutes
    // CLOSE Autologout due to idleness for over 60 minutes.
    // but then:
    // QSocketNotifier: Invalid socket specified
    // QSocketNotifier: Internal error
    // ./zyzzyva.sh: line 3: 14243 Segmentation fault
    // $HOME/dev/zyzzyva-trunk/bin/zyzzyva

    // OBSERVE LOGIN with racks hidden?
    // 0 20 0 11111
    // Woland 1805 usranii null
    // MOVE H8 nairu 12 20 00 aeiiiis 0 MOVE D8 liaise 12 20 00 uglosii 0 MOVE
    // J8 yogi 12 20 00 uroilis 0 MOVE C11 liri 16 20 00 uftroes 0 MOVE K10
    // ofter 29 20 00 udwonse 0 MOVE L4 unsowed 85 20 00 aueevho 0 MOVE 15F
    // heave 37 20 00 lheavou 0 MOVE 15L hula 46 20 00 vnereor 0 MOVE G7 nave
    // 28 20 00 gdenorr 0
    //  STOP
    //  lamancha 1394 lcy?gae null
    //  MOVE 8C elEgancy 64 12 25 xjkueae 0 MOVE C7 kex 44 11 57 naeuelj 0
    //  MOVE F6 jugal 31 11 22 nepeaao 0 CHANGE neopnae 11 12 2 MOVE 14J prone
    //  26 10 44 oatqnes 0 MOVE B12 qat 43 10 24 posewns 0 MOVE 8L wows 30 08
    //  45 etmspan 0 MOVE K4 meat 24 06 21 dnsmitp 0
    //   STOP
    //

    // Receiving a match request:
    // MATCH 0 moobles 0 60 0 0 0 1 -1

    // Accepting a match request:
    // (um, the client pickes tiles for both players and who goes first???)
    // ACCEPT moobles 0 60 0 0 0 1 2 egimnor eeeghns

    // Take care of messages the GUI doesn't need to know about
    if ((command == "TELL") || (command == "WHISPER")) {
        QString sender = args.section (" ", 0, 0);
        QString channel = args.section (" ", 1, 1); // ?
        QString text = args.section (" ", 2);

        if (command == "TELL") {
            messageAppendHtml (sender + " tells you: " + text,
                            QColor (0x8b, 0x00, 0x8b));
        }
        else if (command == "WHISPER") {
            messageAppendHtml (sender + " whispers: " + text,
                            QColor (0x64, 0x95, 0xed));
        }
    }

    else if (command == "MATCH") {
        //args = args.simplified();

        //QStringList split = args.split (" ");
        //QString somethingA = split[0];
        //QString requester = split[1];
        //QString somethingB = split[2];
        //int minutes = split[3].toInt();
        //QString somethingC = split[4];
        //QString somethingD = split[5];
        //QString somethingE = split[6];
        //QString somethingF = split[7];
        //QString somethingG = split[8];
        //    messageAppendHtml (sender + " whispers: " + text,
        //                    QColor (0x64, 0x95, 0xed));

        //if (requester == "moobles") {
        //    QString response = "ACCEPT " + requester + " " + somethingB +
        //                            " " + QString::number (minutes) + " " +
        //                            somethingC + " " + somethingD + " " +
        //                            somethingE + " " + somethingF +
        //                            " 2 aeinrst fjqvxyz";
        //    qDebug() << "Sending response: " << response;
        //    iscThread->sendMessage (response);
        //}

        messageAppendHtml (message, QColor (0x00, 0x00, 0x00));
    }

    else if (command == "OBSERVE") {
        processObserve (args);
    }

    else if (command == "ASITIS") {
        messageAppendHtml (args, QColor (0x00, 0x00, 0x00));
        // CHANNEL 0 PropRod 4 aap asked: match
        // shows up as:
        // #0 PropRod(H) -> app asked: match
    }

    else if (command == "ASITIS") {
        messageAppendHtml (args, QColor (0x00, 0x00, 0x00));
    }

    else if (command == "SETALL") {
        // do nothing yet
    }

    else if (command == "SOUGHT") {
        // do nothing yet
    }

    else if (command == "SEEK") {
        // do nothing yet
    }

    else if (command == "UNSEEK") {
        // do nothing yet
    }

    else {
        messageAppendHtml (message, QColor (0x00, 0x00, 0x00));
    }
}

//---------------------------------------------------------------------------
//  messageAppendHtml
//
//! Append HTML to the end of the message area.
//
//! @param text the text to append
//---------------------------------------------------------------------------
void
CrosswordGameForm::messageAppendHtml (const QString& text,
                                      const QColor& color)
{
    int red = color.red();
    int green = color.green();
    int blue = color.blue();
    int minPad = 2;
    int base = 16;
    QChar pad = '0';
    QString colorStr = QString ("#%1%2%3").arg (red, minPad, base, pad).
                                           arg (green, minPad, base, pad).
                                           arg (blue, minPad, base, pad);

    QString html = "<font color=\"" + colorStr + "\">" +
                   encodeHtmlEntities (text) + "</font><br>";

    // Move to the end, append HTML, and move to the end again
    QTextCursor cursor = messageArea->textCursor();
    cursor.movePosition (QTextCursor::End);
    messageArea->setTextCursor (cursor);

    messageArea->insertHtml (html);

    cursor.movePosition (QTextCursor::End);
    messageArea->setTextCursor (cursor);
}

//---------------------------------------------------------------------------
//  encodeHtmlEntities
//
//! Encode HTML entities in a string.
//
//! @param text the string to encode
//! @return the encoded string
//---------------------------------------------------------------------------
QString
CrosswordGameForm::encodeHtmlEntities (const QString& text)
{
    QString encoded = text;
    encoded.replace ("&", "&amp;");
    encoded.replace ("<", "&lt;");
    encoded.replace (">", "&gt;");
    encoded.replace ("\n", "<br>");
    return encoded;
}

//---------------------------------------------------------------------------
//  canonizeMessage
//
//! Transform a message into canonical form.  Translate any shorthand commands
//! into their full equivalents.
//
//! @param message the message to canonize
//! @return the canonized message
//---------------------------------------------------------------------------
QString
CrosswordGameForm::canonizeMessage (const QString& message)
{
    QString str = message.trimmed();
    QString command = str.section (" ", 0, 0).toUpper();
    QString args = str.section (" ", 1);

    if (command == "SET") {
        QString subcommand = args.section (" ", 0, 0).toUpper();
        command += " " + subcommand;
        args = args.section (" ", 1);
    }

    // FIXME: this will also falsely match incorrect commands like ALVO
    if (command.startsWith ("AL"))
        command = "ALLOBSERVERS";
    else if (command.startsWith ("F"))
        command = "FINGER";
    else if (command.startsWith ("O"))
        command = "OBSERVE";

    return command + " " + args;
}

//---------------------------------------------------------------------------
//  setClock
//
//! Set a certain number of seconds on one player's clock.
//
//! @param playerNum the player whose clock to set
//! @param seconds the number of seconds
//---------------------------------------------------------------------------
void
CrosswordGameForm::setClock (int playerNum, int seconds)
{
    QLabel* label = 0;
    if (playerNum == 1) {
        aSeconds = seconds;
        label = aTimerLabel;
    }
    else if (playerNum == 2) {
        bSeconds = seconds;
        label = bTimerLabel;
    }

    if (label) {
        bool overtime = false;
        int minutes = seconds / 60;
        seconds %= 60;

        if (seconds < 0) {
            QPalette palette (label->palette());
            palette.setColor (QPalette::WindowText, QColor (0xff, 0x00, 0x00));
            label->setPalette (palette);
            minutes = -minutes;
            seconds = -seconds;
            overtime = true;
        }
        else {
            QPalette palette (label->palette());
            palette.setColor (QPalette::WindowText, QColor (0x00, 0x00, 0x00));
            label->setPalette (palette);
        }

        label->setText (QString ("%1%2:%3").
                        arg (overtime ? QString ("-") : QString()).
                        arg (QString::number (minutes)).
                        arg (QString::number (seconds), 2, '0'));
    }
}

//---------------------------------------------------------------------------
//  decrementClock
//
//! Subtract one second from one player's clock
//
//! @param playerNum the player whose clock to decrement
//---------------------------------------------------------------------------
void
CrosswordGameForm::decrementClock (int playerNum)
{
    if (playerNum == 1)
        setClock (1, --aSeconds);
    else if (playerNum == 2)
        setClock (2, --bSeconds);
}

//---------------------------------------------------------------------------
//  startClock
//
//! Start the timer that decrements one player's clock.
//
//! @param playerNum the player whose clock to start
//---------------------------------------------------------------------------
void
CrosswordGameForm::startClock (int playerNum)
{
    if (playerNum == 1)
        aTimer.start (1000);
    else if (playerNum == 2)
        bTimer.start (1000);
}

//---------------------------------------------------------------------------
//  stopClock
//
//! Stop the timer that decrements one player's clock.
//
//! @param playerNum the player whose clock to stop
//---------------------------------------------------------------------------
void
CrosswordGameForm::stopClock (int playerNum)
{
    if (playerNum == 1)
        aTimer.stop();
    else if (playerNum == 2)
        bTimer.stop();
}

//---------------------------------------------------------------------------
//  processObserve
//
//! Process an OBSERVE message from the ISC server.
//
//! @param playerNum the player whose clock to stop
//---------------------------------------------------------------------------
void
CrosswordGameForm::processObserve (const QString& string)
{
    qDebug() << "*** processObserve: " << string;
    QString action = string.section (" ", 0, 0);
    QString args = string.section (" ", 1);

    // OBSERVE LOGIN when first move was made overtime:
    // OBSERVE LOGIN
    // 101 null
    // 0 3 0 01001
    // meebles 0 aademru -100
    // MOVE H8 dream 22 00 55 aenopqu 0
    //  STOP
    // moobles 0 ccdeiru null
    //
    //  STOP

    // going over time:
    // OBSERVE ADJUST OVERTIME SonOfAulay
    //
    // final pass of the game:
    // OBSERVE PAS 05 43 ---

    // final challenge (losing)
    // OBSERVE CHALLENGE Yes Yes Yes
    // OBSERVE PAS 17 24 ---
    // OBSERVE RESIGN nunavut 4

    // successful challenge - need to remove tiles from board, revert to
    // previous rack
    // OBSERVE CHALLENGE Yes No
    // OBSERVE PAS 04 20 L6_pry_28

    if (action == "MOVE") {
        args = args.simplified();

        CrosswordGameMove move (action + " " + args);

        int playerToMove = game->getPlayerToMove();
        move.setPlayerNum (playerToMove);

        fixIscMove (move);

        //CrosswordGameMove move (action + " " + args);

        makeMove (move);
    }

    else if (action == "CHANGE") {
        args = args.simplified();

        CrosswordGameMove move (action + " " + args);

        int numExchanged = move.getNumExchanged();

        int playerToMove = game->getPlayerToMove();
        move.setPlayerNum (playerToMove);

        // Yuck kludge - fix seconds left if overtime
        bool overtime = (playerToMove == 1 ? aOvertime : bOvertime);
        if (overtime) {
            move.setSecondsLeft (IscConverter::timeIscToReal
                                 (0, move.getSecondsLeft(), true));
        }

        QString playerName = playerToMove == 1 ? aPlayerLabel->text()
            : bPlayerLabel->text();

        messageAppendHtml (playerName + ": CHANGE " +
                           QString::number (numExchanged),
                           QColor (0x00, 0x00, 0xff));

        game->makeMove (move);
        stopClock (playerToMove);
        setClock (playerToMove, move.getSecondsLeft());
        startClock (playerToMove == 1 ? 2 : 1);

        // OBSERVE CHANGE erxievz 03 27 7

        // OBSERVE CHANGE czledre 02 5 5

    }

    else if (action == "LOGIN") {
        qDebug() << "*** A";
        game->clear();
        qDebug() << "*** B";
        stopClock (1);
        qDebug() << "*** C";
        stopClock (2);
        qDebug() << "*** D";

        args = args.trimmed();
        qDebug() << "*** E";
        qDebug() << "args: " << args;
        // What does the first line mean?

        QStringList lines = args.split ("\n");
        qDebug() << "*** F";
        qDebug() << "*** lines.size(): " << lines.size();

        // FIXME
        QString firstLine = lines[0];

        qDebug() << "*** G";
        qDebug() << "*** lines[1]:" << lines[1];
        QStringList vars = lines[1].split (" ");
        qDebug() << "*** G2";

        QString lexicon;
        char lexnum = vars[0][0].toAscii();
        switch (lexnum) {
            case '0': lexicon = "TWL98"; break;
            case '1': lexicon = "SOWPODS"; break;
            case '2': lexicon = "ODS"; break;
            case '3': lexicon = "LOC2000"; break;

                      // FIXME
            case '4': lexicon = "PARO"; break;
                      // FIXME
            case '5': lexicon = "MULTI"; break;

            case '6': lexicon = "SWL"; break;
            default:  lexicon = "Unknown"; break;
        }

        int minutes = vars[1].toInt();
        QString increment = vars[2];
        QString moreVars = vars[3];
        bool rated = (moreVars[0] == '1');

        int seconds = IscConverter::timeIscToReal (minutes, 0);
        setClock (1, seconds);
        setClock (2, seconds);

        // SINGLE   c
        // DOUBLE   b
        // VOID     v
        // 5-POINTS f

        char challnum = moreVars[1].toAscii();
        QString challenge;
        switch (challnum) {
            case '0': challenge = "SINGLE"; break;
            case '1': challenge = "DOUBLE"; break;
            case '2': challenge = "5-POINTS"; break;
            case '3': challenge = "VOID"; break;
            default:  challenge = "Unknown"; break;
        }

        bool noescape = (moreVars[2] == '1');

        // FIXME
        QChar something = moreVars[3];
        QChar somethingElse = moreVars[4];

        qDebug() << "*** H";
        QStringList aPlayerSplit = lines[2].split (" ");
        QString aPlayer = aPlayerSplit[0];
        QString aRating = aPlayerSplit[1];

        aPlayerLabel->setText (aPlayer);
        aRatingLabel->setText ("(" + aRating + ")");

        QString aInitialRack = aPlayerSplit[2];
        aOvertime = (aPlayerSplit[3] == "-100");


        QList<CrosswordGameMove> aPlayerMoves;

        CrosswordGameMove aDrawMove ("DRAW " + aInitialRack);
        aDrawMove.setSecondsLeft (seconds);
        aDrawMove.setPlayerNum (1);
        aPlayerMoves.append (aDrawMove);

        QString aMoveLine = lines[3];

        // FIXME: don't forget exchanges so they can be woven back in
        // order
        qDebug() << "*** I";
        QStringList tokens = aMoveLine.split (" ");
        QRegExp moveRe ("MOVE(\\s+\\S+){7}|CHANGE(\\s+\\S+){4}|"
                        "PAS(\\s+\\S+){3}");
        int pos = 0;
        while ((pos = moveRe.indexIn (aMoveLine, pos)) >= 0) {
            QString match = aMoveLine.mid (pos, moveRe.matchedLength());
            CrosswordGameMove move (match);
            move.setPlayerNum (1);

            // Yuck kludge - fix seconds left if overtime
            if (aOvertime && (move.getSecondsLeft() < 60)) {
                move.setSecondsLeft (IscConverter::timeIscToReal
                                     (0, move.getSecondsLeft(), true));
            }

            if (move.getType() == CrosswordGameMove::TakeBack) {
                CrosswordGameMove challengedMove = move;
                challengedMove.setType (CrosswordGameMove::Move);
                challengedMove.setScore (-move.getScore());
                // this is a hack - that's not really the new rack
                challengedMove.setNewRack
                    (aPlayerMoves.last().getNewRack());
                aPlayerMoves.append (challengedMove);
            }

            aPlayerMoves.append (move);
            pos += moveRe.matchedLength();
        }

        qDebug() << "*** J";
        QStringList bPlayerSplit = lines[5].split (" ");
        QString bPlayer = bPlayerSplit[0];
        QString bRating = bPlayerSplit[1];

        bPlayerLabel->setText (bPlayer);
        bRatingLabel->setText ("(" + bRating + ")");

        QString bInitialRack = bPlayerSplit[2];
        bOvertime = (bPlayerSplit[3] == "-100");

        QList<CrosswordGameMove> bPlayerMoves;

        CrosswordGameMove bDrawMove ("DRAW " + bInitialRack);
        bDrawMove.setSecondsLeft (seconds);
        bDrawMove.setPlayerNum (2);
        bPlayerMoves.append (bDrawMove);

        QString bMoveLine = lines[6];

        // FIXME: don't forget exchanges so they can be woven back in
        // order
        qDebug() << "*** K";
        tokens = bMoveLine.split (" ");
        pos = 0;
        while ((pos = moveRe.indexIn (bMoveLine, pos)) >= 0) {
            QString match = bMoveLine.mid (pos, moveRe.matchedLength());
            CrosswordGameMove move (match);
            move.setPlayerNum (2);

            // Yuck kludge - fix seconds left if overtime
            if (bOvertime && (move.getSecondsLeft() < 60)) {
                move.setSecondsLeft (IscConverter::timeIscToReal
                                     (0, move.getSecondsLeft(), true));
            }

            if (move.getType() == CrosswordGameMove::TakeBack) {
                CrosswordGameMove challengedMove = move;
                challengedMove.setType (CrosswordGameMove::Move);
                challengedMove.setScore (-move.getScore());
                challengedMove.setNewRack
                    (bPlayerMoves.last().getNewRack());
                bPlayerMoves.append (challengedMove);
            }

            bPlayerMoves.append (move);
            pos += moveRe.matchedLength();
        }

        QListIterator<CrosswordGameMove> aMoveIt (aPlayerMoves);
        QListIterator<CrosswordGameMove> bMoveIt (bPlayerMoves);

        CrosswordGameMove aMove;
        if (aMoveIt.hasNext())
            aMove = aMoveIt.next();
        CrosswordGameMove bMove;
        if (bMoveIt.hasNext())
            bMove = bMoveIt.next();

        int playerToMove = 1;
        while (aMove.isValid() || bMove.isValid()) {

            // FIXME: all this duplicated code belongs in its own method -
            // it's duplicated above when observing a MOVE, too
            if ((playerToMove == 1) && aMove.isValid()) {
                game->makeMove (aMove);
                stopClock (playerToMove);
                setClock (playerToMove, aMove.getSecondsLeft());
                if (aMoveIt.hasNext())
                    aMove = aMoveIt.next();
                else
                    aMove = CrosswordGameMove();

                if (aMove.getType() == CrosswordGameMove::TakeBack) {
                    game->makeMove (aMove);
                    stopClock (playerToMove);
                    setClock (playerToMove, aMove.getSecondsLeft());
                    if (aMoveIt.hasNext())
                        aMove = aMoveIt.next();
                    else
                        aMove = CrosswordGameMove();
                }
            }

            else if ((playerToMove == 2) && bMove.isValid()) {
                game->makeMove (bMove);
                stopClock (playerToMove);
                setClock (playerToMove, bMove.getSecondsLeft());
                if (bMoveIt.hasNext())
                    bMove = bMoveIt.next();
                else
                    bMove = CrosswordGameMove();

                if (bMove.getType() == CrosswordGameMove::TakeBack) {
                    game->makeMove (bMove);
                    stopClock (playerToMove);
                    setClock (playerToMove, bMove.getSecondsLeft());
                    if (bMoveIt.hasNext())
                        bMove = bMoveIt.next();
                    else
                        bMove = CrosswordGameMove();
                }
            }

            playerToMove = (playerToMove == 1 ? 2 : 1);
        }

        startClock (playerToMove);

        QString text = "You are now observing: " + aPlayer + " vs " +
            bPlayer + " " + lexicon + " " + QString::number (minutes) +
            " " + increment + " " +
            (rated ? QString ("rated") : QString ("unrated")) + " " +
                                         "noescape=" + (noescape ? QString ("ON") : QString ("OFF")) +
                                                                                    " challenge=" + challenge;

        // FIXME: also say which player is on move

        messageAppendHtml (text, QColor (0x00, 0x00, 0x00));
    }

    else if (action == "CHALLENGE") {
        args = args.simplified();


        // If player A plays a valid word and player B challenges it, it
        // looks like this:
        // OBSERVE CHALLENGE Yes

        // If player A plays a phony and player B challenges it off, it
        // looks like this.  Player A has 41:21 remaining, and the
        // challenged play was cvaVcier at 8A for 92 points.
        // OBSERVE CHALLENGE No
        // OBSERVE PAS   41 21 8A_cvaVcier_92


        QStringList judgments = args.split (" ");
        QString judgment;
        bool acceptable = true;
        foreach (judgment, judgments) {
            if (judgment == "Yes")
                continue;
            acceptable = false;
            break;
        }

        QString acceptableStr = acceptable ? QString ("valid")
            : QString ("invalid");

        messageAppendHtml ("Challenge: move is " + acceptableStr + ".",
                           QColor (0x00, 0x00, 0xff));

    }

    else if (action == "PAS") {
        args = args.simplified();

        int minutes = args.section (" ", 0, 0).toInt();
        int seconds = args.section (" ", 1, 1).toInt();
        QString challengedPlay = args.section (" ", 2, 2);
        int playerToMove = game->getPlayerToMove();

        if ((challengedPlay != "---") && !challengedPlay.isEmpty()) {
            game->challengeLastMove();
        }
        else {
            bool overtime = (playerToMove == 1 ? aOvertime : bOvertime);
            int realSeconds = IscConverter::timeIscToReal
                (minutes, seconds, overtime);

            CrosswordGameMove move;
            move.setType (CrosswordGameMove::Pass);
            move.setPlayerNum (playerToMove);
            move.setSecondsLeft (realSeconds);
            move.setNewRack (game->getPlayerRack (playerToMove));

            game->makeMove (move);
            stopClock (playerToMove);
            setClock (playerToMove, move.getSecondsLeft());
            startClock (playerToMove == 1 ? 2 : 1);
        }

        QString playerName = playerToMove == 1 ? aPlayerLabel->text()
            : bPlayerLabel->text();

        messageAppendHtml (playerName + ": PASS",
                           QColor (0x00, 0x00, 0xff));
    }

    else if (action == "RESIGN") {
        stopClock (1);
        stopClock (2);

        QString aPlayerName = aPlayerLabel->text();
        QString bPlayerName = bPlayerLabel->text();

        // Apply rack bonuses or penalties
        QString aRack = game->getPlayerRack (1);
        int aRackValue = game->getRackValue (aRack);
        QString bRack = game->getPlayerRack (2);
        int bRackValue = game->getRackValue (bRack);

        if ((aRackValue > 0) && (bRackValue > 0)) {
            CrosswordGameMove aRackMove;
            aRackMove.setPlayerNum (1);
            aRackMove.setType (CrosswordGameMove::RackBonus);
            aRackMove.setScore (-aRackValue);
            aRackMove.setNewRack (aRack);
            game->makeMove (aRackMove);
            messageAppendHtml (aPlayerName + " loses " +
                               QString::number (aRackValue) + " points "
                               "for the final rack: " + aRack,
                               QColor (0x00, 0x00, 0x00));

            CrosswordGameMove bRackMove;
            bRackMove.setPlayerNum (2);
            bRackMove.setType (CrosswordGameMove::RackBonus);
            bRackMove.setScore (-bRackValue);
            bRackMove.setNewRack (bRack);
            game->makeMove (bRackMove);
            messageAppendHtml (bPlayerName + " loses " +
                               QString::number (bRackValue) + " points "
                               "for the final rack: " + bRack,
                               QColor (0x00, 0x00, 0x00));
        }

        else if (aRackValue > 0) {
            int bonus = 2 * aRackValue;
            CrosswordGameMove bRackMove;
            bRackMove.setPlayerNum (2);
            bRackMove.setType (CrosswordGameMove::RackBonus);
            bRackMove.setScore (bonus);
            bRackMove.setNewRack (bRack);
            game->makeMove (bRackMove);
            messageAppendHtml (bPlayerName + " gets " +
                               QString::number (bonus) + " points for " +
                               aPlayerName + "'s final rack: " + aRack,
                               QColor (0x00, 0x00, 0x00));
        }

        else if (bRackValue > 0) {
            int bonus = 2 * bRackValue;
            CrosswordGameMove aRackMove;
            aRackMove.setPlayerNum (1);
            aRackMove.setType (CrosswordGameMove::RackBonus);
            aRackMove.setScore (bonus);
            aRackMove.setNewRack (aRack);
            game->makeMove (aRackMove);
            messageAppendHtml (bPlayerName + " gets " +
                               QString::number (bonus) + " points for " +
                               bPlayerName + "'s final rack: " + bRack,
                               QColor (0x00, 0x00, 0x00));
        }

        // Apply time penalties

        if (aOvertime) {
            int penalty = 10;
            CrosswordGameMove overtimeMove;
            overtimeMove.setPlayerNum (1);
            overtimeMove.setType (CrosswordGameMove::TimePenalty);
            overtimeMove.setScore (-penalty);
            overtimeMove.setNewRack (aRack);
            game->makeMove (overtimeMove);
            messageAppendHtml (aPlayerName + " loses " +
                               QString::number (penalty) + " points for "
                               "going over time.",
                               QColor (0x00, 0x00, 0x00));
        }

        if (bOvertime) {
            int penalty = 10;
            CrosswordGameMove overtimeMove;
            overtimeMove.setPlayerNum (2);
            overtimeMove.setType (CrosswordGameMove::TimePenalty);
            overtimeMove.setScore (-penalty);
            overtimeMove.setNewRack (bRack);
            game->makeMove (overtimeMove);
            messageAppendHtml (bPlayerName + " loses " +
                               QString::number (penalty) + " points for "
                               "going over time.",
                               QColor (0x00, 0x00, 0x00));
        }

        // End the game

        int aScore = game->getPlayerScore (1);
        int bScore = game->getPlayerScore (2);

        QString aScoreStr = aPlayerName + " " + QString::number (aScore);
        QString bScoreStr = bPlayerName + " " + QString::number (bScore);


        QString finalScoreStr = "Final score: ";
        if (aScore > bScore) {
            finalScoreStr += aScoreStr + ", " + bScoreStr;
            messageAppendHtml (finalScoreStr, QColor (0x00, 0x00, 0x00));
            messageAppendHtml (aPlayerName + " wins the game.",
                               QColor (0x00, 0x00, 0x00));
        }
        else if (bScore > aScore) {
            finalScoreStr += bScoreStr + ", " + aScoreStr;
            messageAppendHtml (finalScoreStr, QColor (0x00, 0x00, 0x00));
            messageAppendHtml (bPlayerName + " wins the game.",
                               QColor (0x00, 0x00, 0x00));
        }
        else {
            finalScoreStr += aScoreStr + ", " + bScoreStr;
            messageAppendHtml (finalScoreStr, QColor (0x00, 0x00, 0x00));
            messageAppendHtml ("The game ends in a tie.",
                               QColor (0x00, 0x00, 0x00));
        }

        gameChanged();
    }

    else if (action == "ADJUST") {
        QString arg = args.section (" ", 0, 0);
        QString playerName = args.section (" ", 1, 1);

        if (arg == "OVERTIME") {
            if (playerName == aPlayerLabel->text())
                aOvertime = true;
            else if (playerName == bPlayerLabel->text())
                bOvertime = true;

            messageAppendHtml (playerName + "'s game time has expired. " +
                               playerName + " will get 1 more minute "
                               "with a 10-point penalty.  When this "
                               "minute expires he will forfeit on time.",
                               QColor (0x00, 0x00, 0x00));
        }
    }

    else {
        messageAppendHtml (string, QColor (0x00, 0x00, 0x00));
    }
}

//---------------------------------------------------------------------------
//  makeMove
//
//! Make a move.  If actually playing a game, and if it is our turn to move,
//! send the move to the ISC server.
//
//! @param move the move
//---------------------------------------------------------------------------
void
CrosswordGameForm::makeMove (CrosswordGameMove& move)
{

    // FIXME: Just for display purposes - fix this later
    //QString placement = args.section (" ", 0, 0);
    //QString play = args.section (" ", 1, 1);
    //QString score = args.section (" ", 2, 2);
    //int minutes = args.section (" ", 3, 3).toInt();
    //int seconds = args.section (" ", 4, 4).toInt();
    //QString newRack = args.section (" ", 5, 5);
    //if (newRack == "---")
    //newRack = QString();

    int playerToMove = move.getPlayerNum();
    QString playerName = playerToMove == 1 ? aPlayerLabel->text()
        : bPlayerLabel->text();

    QString placement = move.getPlacement();
    QString word = move.getWord();
    QString score = QString::number (move.getScore());
    messageAppendHtml (playerName + ": MOVE " + placement + " " +
                       word + " " + score, QColor (0x00, 0x00, 0xff));





    game->makeMove (move);
    stopClock (playerToMove);

    setClock (playerToMove, move.getSecondsLeft());
    startClock (playerToMove == 1 ? 2 : 1);
}

//---------------------------------------------------------------------------
//  fixIscMove
//
//! Fix a move from ISC.
//
//! @param move the move
//---------------------------------------------------------------------------
void
CrosswordGameForm::fixIscMove (CrosswordGameMove& move) const
{
    // Yuck kludge - fix seconds left if overtime
    // FIXME: Should be a parameter?
    int playerToMove = game->getPlayerToMove();
    bool overtime = (playerToMove == 1 ? aOvertime : bOvertime);
    if (overtime) {
        move.setSecondsLeft (IscConverter::timeIscToReal
                             (0, move.getSecondsLeft(), true));
    }
}

//---------------------------------------------------------------------------
//  gameChanged
//
//! Called when the game changes.  Redisplay the board and all associated game
//! information.
//---------------------------------------------------------------------------
void
CrosswordGameForm::gameChanged()
{
    boardWidget->gameChanged();
    aScoreLabel->setText (QString::number (game->getPlayerScore (1)));
    bScoreLabel->setText (QString::number (game->getPlayerScore (2)));
    aRackWidget->setRack (game->getPlayerRack (1));
    bRackWidget->setRack (game->getPlayerRack (2));

    QFont font = aPlayerLabel->font();
    font.setWeight (QFont::Normal);
    QFont boldFont = font;
    boldFont.setWeight (QFont::Bold);
    int playerToMove = game->getPlayerToMove();
    if (playerToMove == 1) {
        aPlayerLabel->setFont (boldFont);
        aRatingLabel->setFont (boldFont);
        aTimerLabel->setFont (boldFont);
        bPlayerLabel->setFont (font);
        bRatingLabel->setFont (font);
        bTimerLabel->setFont (font);
    }
    else if (playerToMove == 2) {
        aPlayerLabel->setFont (font);
        aRatingLabel->setFont (font);
        aTimerLabel->setFont (font);
        bPlayerLabel->setFont (boldFont);
        bRatingLabel->setFont (boldFont);
        bTimerLabel->setFont (boldFont);
    }

    QString bagStr = game->getTilesInBag();
    int numInBag = bagStr.length();
    bagStr.replace (QRegExp ("(.)"), "\\1 ");
    QString unseenStr = game->getUnseenTiles (playerToMove);
    //int numUnseen = unseenStr.length();
    unseenStr.replace (QRegExp ("(.)"), "\\1 ");
    unseenLabel->setText ("Tiles in Bag (" + QString::number (numInBag) +
                          "): " + bagStr); // + "\n" +
                          //"Unseen (" + QString::number (numUnseen) +
                          //"): " + unseenStr);
}

//---------------------------------------------------------------------------
//  clockTimeout
//
//! Called when a clock timer emits a timeout signal.  Decrements the
//! appropriate player's clock.
//
//! @param playerNum the player number
//---------------------------------------------------------------------------
void
CrosswordGameForm::clockTimeout (int playerNum)
{
    decrementClock (playerNum);
}

//---------------------------------------------------------------------------
//  threadSocketError
//
//! Called when the socket encounters an error.
//
//! @param error the error
//---------------------------------------------------------------------------
void
CrosswordGameForm::threadSocketError (QAbstractSocket::SocketError error)
{
    QString errorMsg;
    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
        errorMsg = "Connection refused.";
        break;

        case QAbstractSocket::RemoteHostClosedError:
        errorMsg = "Connection closed by remote host.";
        break;

        case QAbstractSocket::HostNotFoundError:
        errorMsg = "Host address not found.";
        break;

        case QAbstractSocket::SocketAccessError:
        errorMsg = "Operation failed because of insufficient privileges.";
        break;

        case QAbstractSocket::SocketResourceError:
        errorMsg = "Too many sockets open.";
        break;

        case QAbstractSocket::SocketTimeoutError:
        errorMsg = "Socket operation timed out.";
        break;

        case QAbstractSocket::DatagramTooLargeError:
        errorMsg = "Datagram too large.";
        break;

        case QAbstractSocket::NetworkError:
        errorMsg = "Network error (cable unplugged?).";
        break;

        case QAbstractSocket::AddressInUseError:
        errorMsg = "Address already in use.";
        break;

        case QAbstractSocket::SocketAddressNotAvailableError:
        errorMsg = "Address does not belong to host.";
        break;

        case QAbstractSocket::UnsupportedSocketOperationError:
        errorMsg = "Unsupported socket operation.";
        break;

        case QAbstractSocket::UnknownSocketError:
        errorMsg = "Unknown socket error.";
        break;
    }

    QMessageBox::warning (this, "Network Error",
                          "A network error occurred:\n" + errorMsg);

    disconnectClicked();
}
