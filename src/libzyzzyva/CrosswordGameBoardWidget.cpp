//---------------------------------------------------------------------------
// CrosswordGameBoardWidget.cpp
//
// A widget for displaying and manipulating a crossword game board.
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

#include "CrosswordGameBoardWidget.h"
#include "CrosswordGameBoard.h"
#include "Defs.h"
#include <QPainter>
#include <QVBoxLayout>

#include <QtDebug>

using namespace Defs;

const int COLUMN_WIDTH = 30;
const int ROW_HEIGHT = 30;
const int VERTICAL_HEADER_WIDTH = 10;
const int HORIZONTAL_HEADER_HEIGHT = 10;

const QColor NO_BONUS_COLOR = QColor ("gainsboro");
const QColor DOUBLE_LETTER_COLOR = QColor ("cornflowerblue");
const QColor TRIPLE_LETTER_COLOR = QColor ("slateblue");
const QColor DOUBLE_WORD_COLOR = QColor ("palevioletred");
const QColor TRIPLE_WORD_COLOR = QColor ("firebrick");

//---------------------------------------------------------------------------
//  CrosswordGameBoardWidget
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CrosswordGameBoardWidget::CrosswordGameBoardWidget (CrosswordGameBoard* b,
                                                    QWidget* parent,
                                                    Qt::WFlags f)
    : QFrame (parent, f), board (b)
{
    setFrameStyle (QFrame::StyledPanel | QFrame::Raised);
    setLineWidth (2);
    setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);

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
    size.setWidth (size.width() + 2 * lineWidth());
    size.setHeight (size.height() + 2 * lineWidth());
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

    for (int row = 0; row < board->getNumRows(); ++row) {
        for (int col = 0; col < board->getNumColumns(); ++col) {
            QColor color = getBackgroundColor (row, col);
            QPalette palette;
            palette.setColor (QPalette::Light, color.light (125));
            palette.setColor (QPalette::Mid, color);
            palette.setColor (QPalette::Dark, color);

            QRect rect (col * COLUMN_WIDTH, row * ROW_HEIGHT,
                            COLUMN_WIDTH, ROW_HEIGHT);
            painter.setPen (color);
            painter.setBrush (color);
            painter.drawRect (rect);

            qDrawShadePanel (&painter, rect, palette, false, 1);
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
CrosswordGameBoardWidget::getBackgroundColor (int row, int col) const
{
    CrosswordGameBoard::SquareType squareType =
        board->getSquareType (row, col);
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
    return QSize (board->getNumColumns() * COLUMN_WIDTH,
                  board->getNumRows() * ROW_HEIGHT);
}

//---------------------------------------------------------------------------
//  paintEvent
//
//! Called whenever all or part of the widget needs to be repainted.
//
//! @param event the paint event
//---------------------------------------------------------------------------
void
CrosswordGameBoardWidget::paintEvent (QPaintEvent* event)
{
    QPainter painter (this);
    QRect rect (contentsRect());
    painter.drawPixmap (contentsRect().topLeft(), pixmap);
    painter.end();

    QFrame::paintEvent (event);
}
