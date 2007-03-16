//---------------------------------------------------------------------------
// CrosswordGameRackWidget.cpp
//
// A widget for displaying and manipulating a crossword game board.
//
// Copyright 2006, 2007 Michael W Thelen <mthelen@gmail.com>.
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

#include "CrosswordGameRackWidget.h"
#include "CrosswordGameGame.h"
#include "Auxil.h"
#include "Defs.h"
#include <QPainter>

using namespace Defs;

// FIXME: all this is duplicated between here and RackWidget
const int NUM_TILES = 7;
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

const QColor BACKGROUND_COLOR = QColor(0xdc, 0xdc, 0xdc);
const QColor TILE_COLOR = QColor(0xf0, 0xe6, 0x8c);
const QColor DEFAULT_LETTER_COLOR = QColor(0x00, 0x00, 0x00);
const QColor PLAYER1_LETTER_COLOR = QColor(0x00, 0x00, 0xb0);
const QColor PLAYER2_LETTER_COLOR = QColor(0xb0, 0x00, 0x00);

//---------------------------------------------------------------------------
//  CrosswordGameRackWidget
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CrosswordGameRackWidget::CrosswordGameRackWidget(int player, QWidget* parent,
                                                 Qt::WFlags f)
    : QFrame(parent, f), playerNum(player)
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
CrosswordGameRackWidget::minimumSizeHint() const
{
    QSize size = getRackSize();
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
CrosswordGameRackWidget::sizeHint() const
{
    return minimumSizeHint();
}

//---------------------------------------------------------------------------
//  setRack
//
//! Set the rack to a set of letters.
//
//! @param rack the rack of letters
//---------------------------------------------------------------------------
void
CrosswordGameRackWidget::setRack(const QString& rack)
{
    letters = Auxil::getAlphagram(rack).toUpper();
    pixmap = makePixmap();
    update(contentsRect());
}

//---------------------------------------------------------------------------
//  makePixmap
//
//! Create a pixmap representation of the rack.
//
//! @return the pixmap
//---------------------------------------------------------------------------
QPixmap
CrosswordGameRackWidget::makePixmap() const
{
    QPixmap pixmap (getRackSize());
    QPainter painter (&pixmap);

    // FIXME: most of this is duplicated between BoardWidget and here
    QColor backgroundColor = BACKGROUND_COLOR;
    QPalette backgroundPalette;
    backgroundPalette.setColor(QPalette::Light,
                               backgroundColor.light(SQUARE_SHADE_VALUE));
    backgroundPalette.setColor(QPalette::Mid, backgroundColor);
    backgroundPalette.setColor(QPalette::Dark,
                               backgroundColor.dark(SQUARE_SHADE_VALUE));

    for (int i = 0; i < NUM_TILES; ++i) {
        QRect rect (i * COLUMN_WIDTH, 0, COLUMN_WIDTH, ROW_HEIGHT);
        painter.setPen(backgroundColor);
        painter.setBrush(backgroundColor);
        painter.drawRect(rect);

        qDrawShadePanel(&painter, rect, backgroundPalette, false,
                        SQUARE_SHADE_PANEL_WIDTH);


        if (i >= letters.length())
            continue;

        QRect tileRect(i * COLUMN_WIDTH + TILE_MARGIN, TILE_MARGIN,
                       COLUMN_WIDTH - 2 * TILE_MARGIN -
                       SQUARE_SHADE_PANEL_WIDTH,
                       ROW_HEIGHT - 2 * TILE_MARGIN -
                       SQUARE_SHADE_PANEL_WIDTH);

        QColor color = TILE_COLOR;
        QPalette palette;
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

        switch (playerNum) {
            case 1:  color = PLAYER1_LETTER_COLOR; break;
            case 2:  color = PLAYER2_LETTER_COLOR; break;
            default: color = DEFAULT_LETTER_COLOR; break;
        }
        painter.setPen(QPen(color));

        QChar letter = letters[i];
        if (letter == '?') {
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

        else {
            painter.drawText(rect, Qt::AlignCenter, letter);
        }
    }

    return pixmap;
}

//---------------------------------------------------------------------------
//  getRackSize
//
//! Get the size of the board area.
//
//! @return the board size
//---------------------------------------------------------------------------
QSize
CrosswordGameRackWidget::getRackSize() const
{
    return QSize(NUM_TILES * COLUMN_WIDTH, ROW_HEIGHT);
}

//---------------------------------------------------------------------------
//  paintEvent
//
//! Called whenever all or part of the widget needs to be repainted.
//
//! @param event the paint event
//---------------------------------------------------------------------------
void
CrosswordGameRackWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter (this);
    QRect rect (contentsRect());
    painter.drawPixmap(contentsRect().topLeft(), pixmap);
    painter.end();
    QFrame::paintEvent(event);
}
