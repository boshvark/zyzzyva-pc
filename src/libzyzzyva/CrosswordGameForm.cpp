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
#include "CrosswordGameBoard.h"
#include "CrosswordGameBoardWidget.h"
#include "CrosswordGameMove.h"
#include "IscConnectionThread.h"
#include "Auxil.h"
#include "Defs.h"
#include <QFile>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QVBoxLayout>

#include <QtDebug>

using namespace Defs;

const int BOARD_COLUMN_WIDTH = 30;
const int BOARD_ROW_HEIGHT = 30;

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
      board (new CrosswordGameBoard()), iscThread (0)
{
    QHBoxLayout* mainHlay = new QHBoxLayout (this);
    Q_CHECK_PTR (mainHlay);
    mainHlay->setMargin (MARGIN);
    mainHlay->setSpacing (SPACING);

    QVBoxLayout* boardVlay = new QVBoxLayout;
    Q_CHECK_PTR (boardVlay);
    boardVlay->setMargin (0);
    boardVlay->setSpacing (SPACING);
    mainHlay->addLayout (boardVlay);

    boardWidget = new CrosswordGameBoardWidget (board, this);
    Q_CHECK_PTR (boardWidget);
    boardVlay->addWidget (boardWidget);

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

    messageArea = new QTextEdit (this);
    Q_CHECK_PTR (messageArea);
    messageArea->setReadOnly (true);
    messageVlay->addWidget (messageArea);

    inputLine = new QLineEdit (this);
    Q_CHECK_PTR (inputLine);
    connect (inputLine, SIGNAL (returnPressed()),
             SLOT (inputReturnPressed()));
    messageVlay->addWidget (inputLine);
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
    QString command = message.section (" ", 0, 0);
    QString args = message.section (" ", 1);

    // A hush fills the room as olaugh walks in! :)
    // WHO BEST 1877 olaugh a 0 0

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

    else if (command == "OBSERVE") {
        QString action = args.section (" ", 0, 0);
        args = args.section (" ", 1);

        // going over time:
        // OBSERVE ADJUST OVERTIME SonOfAulay
        //
        // final pass of the game:
        // OBSERVE PAS 05 43 ---

        if (action == "MOVE") {
            args = action + " " + args;
            args = args.simplified();

            CrosswordGameMove move (args);

            // FIXME: Just for display purposes - fix this later
            QString placement = args.section (" ", 0, 0);
            QString play = args.section (" ", 1, 1);
            QString score = args.section (" ", 2, 2);
            QString minutes = args.section (" ", 3, 3);
            QString seconds = args.section (" ", 4, 4);
            QString newRack = args.section (" ", 5, 5);
            messageAppendHtml ("MOVE " + placement + " " + play + " " + score,
                               QColor (0x00, 0x00, 0xff));


            displayMove (move, 1);

            // FIXME: fix player num
            //displayMove (play, translateCoordinates (placement), 1);
        }

        else if (action == "CHANGE") {
            args = args.simplified();

            messageAppendHtml (message, QColor (0x00, 0x00, 0x00));

            // OBSERVE CHANGE erxievz 03 27 7

            // OBSERVE CHANGE czledre 02 5 5

        }

        else if (action == "LOGIN") {
            args = args.trimmed();
            // What does the first line mean?

            QStringList lines = args.split ("\n");

            QString line;
            foreach (line, lines) {
                messageAppendHtml ("Line: " + line,
                                   QColor (0x00, 0x00, 0xff));
            }

            // FIXME
            QString firstLine = lines[0];

            QStringList vars = lines[1].split (" ");

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

            QString time = vars[1];
            QString increment = vars[2];
            QString moreVars = vars[3];
            bool rated = (moreVars[0] == '1');


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

            QStringList aPlayerSplit = lines[2].split (" ");
            QString aPlayer = aPlayerSplit[0];
            QString aRating = aPlayerSplit[1];
            QString aInitialRack = aPlayerSplit[2];
            // FIXME
            QString aSomething = aPlayerSplit[3];

            QString aMoveLine = lines[3];

            //QList<CrosswordGameMove> aPlayerMoves;

            // FIXME: don't forget exchanges so they can be woven back in
            // order
            QStringList tokens = aMoveLine.split (" ");
            QRegExp moveRe ("MOVE(\\s+\\S+){7}|CHANGE(\\s+\\S+){4}");
            int pos = 0;
            while ((pos = moveRe.indexIn (aMoveLine, pos)) >= 0) {
                QString match = aMoveLine.mid (pos, moveRe.matchedLength());
                CrosswordGameMove move (match);
                displayMove (move, 1);
                pos += moveRe.matchedLength();
            }


            QStringList bPlayerSplit = lines[5].split (" ");
            QString bPlayer = bPlayerSplit[0];
            QString bRating = bPlayerSplit[1];
            QString bInitialRack = bPlayerSplit[2];
            // FIXME
            QString bSomething = bPlayerSplit[3];


            QString bMoveLine = lines[6];

            // FIXME: don't forget exchanges so they can be woven back in
            // order
            tokens = bMoveLine.split (" ");
            pos = 0;
            while ((pos = moveRe.indexIn (bMoveLine, pos)) >= 0) {
                QString match = bMoveLine.mid (pos, moveRe.matchedLength());
                CrosswordGameMove move (match);
                displayMove (move, 1);
                pos += moveRe.matchedLength();
            }


            QString text = "You are now observing: " + aPlayer + " vs " +
                bPlayer + " " + lexicon + " " + time + " " + increment + " " +
                (rated ? QString ("rated") : QString ("unrated")) + " " +
                "noescape=" + (noescape ? QString ("ON") : QString ("OFF")) +
                " challenge=" + challenge;

            // FIXME: also say which player is on move

            messageAppendHtml (text, QColor (0x00, 0x00, 0x00));
        }

        else if (action == "RESIGN") {
            // FIXME
            messageAppendHtml (message, QColor (0x00, 0x00, 0x00));
        }

        else {
            messageAppendHtml (message, QColor (0x00, 0x00, 0x00));
        }
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
//  displayMove
//
//! Display a move on the board.
//
//! @param move the move
//! @param player the player number
//---------------------------------------------------------------------------
void
CrosswordGameForm::displayMove (const CrosswordGameMove& move, int player)
{
    board->makeMove (move, player);
}
