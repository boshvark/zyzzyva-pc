//---------------------------------------------------------------------------
// CrosswordGameBoardWidget.cpp
//
// A widget for displaying and manipulating a crossword game board.
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

#include "CrosswordGameBoardWidget.h"
#include "CrosswordGameGame.h"
#include "Defs.h"
#include <QPainter>

using namespace Defs;

const int COLUMN_WIDTH = 30;
const int ROW_HEIGHT = 30;
const int LETTER_HEIGHT = 16;
const int VERTICAL_HEADER_WIDTH = 10;
const int HORIZONTAL_HEADER_HEIGHT = 10;
const int TILE_MARGIN = 2;
const int BLANK_SQUARE_MARGIN = 5;
const int SQUARE_SHADE_PANEL_WIDTH = 1;
const int SQUARE_SHADE_VALUE = 125;
const int TILE_SHADE_PANEL_WIDTH = 2;
const int TILE_SHADE_VALUE = 150;

const QColor NO_BONUS_COLOR = QColor(0xdc, 0xdc, 0xdc);
const QColor DOUBLE_LETTER_COLOR = QColor(0x64, 0x95, 0xed);
const QColor TRIPLE_LETTER_COLOR = QColor(0x6a, 0x5a, 0xcd);
const QColor DOUBLE_WORD_COLOR = QColor(0xdb, 0x70, 0x93);
const QColor TRIPLE_WORD_COLOR = QColor(0xb2, 0x22, 0x22);
const QColor TILE_COLOR = QColor(0xf0, 0xe6, 0x8c);
const QColor DEFAULT_LETTER_COLOR = QColor(0x00, 0x00, 0x00);
const QColor PLAYER1_LETTER_COLOR = QColor(0x00, 0x00, 0xb0);
const QColor PLAYER2_LETTER_COLOR = QColor(0xb0, 0x00, 0x00);

//---------------------------------------------------------------------------
//  CrosswordGameBoardWidget
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CrosswordGameBoardWidget::CrosswordGameBoardWidget(CrosswordGameGame* g,
    QWidget* parent, Qt::WFlags f)
    : QFrame(parent, f), game(g)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setLineWidth(2);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pixmap = makePixmap();
}

//---------------------------------------------------------------------------
//  minimumSizeHint
//
//! Set the minimum size hint of the board.
//---------------------------------------------------------------------------
QSize
CrosswordGameBoardWidget::minimumSizeHint() const
{
    QSize size = getBoardSize();
    size.setWidth(size.width() + 2 * lineWidth());
    size.setHeight(size.height() + 2 * lineWidth());
    return size;
}

//---------------------------------------------------------------------------
//  sizeHint
//
//! Set the size hint of the board.
//---------------------------------------------------------------------------
QSize
CrosswordGameBoardWidget::sizeHint() const
{
    return minimumSizeHint();
}

//---------------------------------------------------------------------------
//  gameChanged
//
//! Called when the game is changed.
//---------------------------------------------------------------------------
void
CrosswordGameBoardWidget::gameChanged()
{
    pixmap = makePixmap();
    update(contentsRect());
}

//---------------------------------------------------------------------------
//  makePixmap
//
//! Create a pixmap representation of the board.
//
//! @return the pixmap
//---------------------------------------------------------------------------
QPixmap
CrosswordGameBoardWidget::makePixmap() const
{
    QPixmap pixmap (getBoardSize());
    QPainter painter (&pixmap);

    for (int row = 0; row < game->getNumRows(); ++row) {
        for (int col = 0; col < game->getNumColumns(); ++col) {
            QColor color = getBackgroundColor(row, col);
            QPalette palette;
            palette.setColor(QPalette::Light,
                             color.light(SQUARE_SHADE_VALUE));
            palette.setColor(QPalette::Mid, color);
            palette.setColor(QPalette::Dark,
                             color.dark(SQUARE_SHADE_VALUE));

            QRect rect (col * COLUMN_WIDTH, row * ROW_HEIGHT,
                        COLUMN_WIDTH, ROW_HEIGHT);
            painter.setPen(color);
            painter.setBrush(color);
            painter.drawRect(rect);

            qDrawShadePanel(&painter, rect, palette, false,
                            SQUARE_SHADE_PANEL_WIDTH);

            CrosswordGameBoard::Tile tile = game->getTile(row, col);
            if (tile.isValid()) {
                QRect tileRect (col * COLUMN_WIDTH + TILE_MARGIN,
                                row * ROW_HEIGHT + TILE_MARGIN,
                                COLUMN_WIDTH - 2 * TILE_MARGIN -
                                    SQUARE_SHADE_PANEL_WIDTH,
                                ROW_HEIGHT - 2 * TILE_MARGIN -
                                    SQUARE_SHADE_PANEL_WIDTH);

                color = TILE_COLOR;
                palette.setColor(QPalette::Light,
                                 color.light(TILE_SHADE_VALUE));
                palette.setColor(QPalette::Mid, color);
                palette.setColor(QPalette::Dark,
                                 color.dark(TILE_SHADE_VALUE));

                painter.setPen(QColor("black"));
                painter.setBrush(color);
                painter.drawRect(tileRect);
                qDrawShadePanel(&painter, tileRect, palette, false,
                                TILE_SHADE_PANEL_WIDTH);

                QFont tileFont = font();
                tileFont.setPixelSize(LETTER_HEIGHT);
                tileFont.setWeight(QFont::Black);
                painter.setFont(tileFont);

                switch (tile.getPlayerNum()) {
                    case 1:  color = PLAYER1_LETTER_COLOR; break;
                    case 2:  color = PLAYER2_LETTER_COLOR; break;
                    default: color = DEFAULT_LETTER_COLOR; break;
                }
                painter.setPen(QPen(color));

                QChar letter = tile.getLetter();
                painter.drawText(rect, Qt::AlignCenter, letter);

                if (tile.isBlank()) {
                    QPen pen (color);
                    pen.setWidth(1);
                    painter.setPen(pen);
                    painter.setBrush(Qt::NoBrush);
                    QRect blankRect(rect.x() + BLANK_SQUARE_MARGIN,
                                    rect.y() + BLANK_SQUARE_MARGIN,
                                    rect.width() - 2 * BLANK_SQUARE_MARGIN -
                                        SQUARE_SHADE_PANEL_WIDTH - 1,
                                    rect.height() - 2 * BLANK_SQUARE_MARGIN -
                                        SQUARE_SHADE_PANEL_WIDTH - 1);
                    painter.drawRect(blankRect);
                }
            }
        }
    }

    return pixmap;
}

//---------------------------------------------------------------------------
//  getBackgroundColor
//
//! Get the background color associated with a square.
//
//! @param row the row number
//! @param col the column number
//! @return the background color
//---------------------------------------------------------------------------
QColor
CrosswordGameBoardWidget::getBackgroundColor(int row, int col) const
{
    CrosswordGameBoard::SquareType squareType = game->getSquareType(row, col);
    switch (squareType) {
        case CrosswordGameBoard::NoBonus:      return NO_BONUS_COLOR;
        case CrosswordGameBoard::DoubleLetter: return DOUBLE_LETTER_COLOR;
        case CrosswordGameBoard::TripleLetter: return TRIPLE_LETTER_COLOR;
        case CrosswordGameBoard::DoubleWord:   return DOUBLE_WORD_COLOR;
        case CrosswordGameBoard::TripleWord:   return TRIPLE_WORD_COLOR;
        default:                               return NO_BONUS_COLOR;
    }
}

//---------------------------------------------------------------------------
//  getBoardSize
//
//! Get the size of the board area.
//
//! @return the board size
//---------------------------------------------------------------------------
QSize
CrosswordGameBoardWidget::getBoardSize() const
{
    return QSize(game->getNumColumns() * COLUMN_WIDTH,
                 game->getNumRows() * ROW_HEIGHT);
}

//---------------------------------------------------------------------------
//  paintEvent
//
//! Called whenever all or part of the widget needs to be repainted.
//
//! @param event the paint event
//---------------------------------------------------------------------------
void
CrosswordGameBoardWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter (this);
    QRect rect (contentsRect());
    painter.drawPixmap(contentsRect().topLeft(), pixmap);
    painter.end();
    QFrame::paintEvent(event);
}
