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
#include "Defs.h"
#include <QPainter>
#include <QVBoxLayout>

#include <QtDebug>

using namespace Defs;

const int NUM_ROWS = 15;
const int NUM_COLUMNS = 15;
const int COLUMN_WIDTH = 30;
const int ROW_HEIGHT = 30;
const int VERTICAL_HEADER_WIDTH = 10;
const int HORIZONTAL_HEADER_HEIGHT = 10;

const QColor NO_BONUS_COLOR = QColor (0xff, 0xff, 0xff);
const QColor DOUBLE_LETTER_COLOR = QColor (0x7f, 0xff, 0xd4);
const QColor TRIPLE_LETTER_COLOR = QColor (0x00, 0x00, 0xff);
const QColor DOUBLE_WORD_COLOR = QColor (0xfa, 0x80, 0x72);
const QColor TRIPLE_WORD_COLOR = QColor (0xff, 0x00, 0x00);

//---------------------------------------------------------------------------
//  CrosswordGameBoardWidget
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
CrosswordGameBoardWidget::CrosswordGameBoardWidget (QWidget* parent,
                                                    Qt::WFlags f)
    : QFrame (parent, f)
{
    setFrameStyle (QFrame::StyledPanel | QFrame::Raised);
    setLineWidth (2);

    setPalette (QPalette (QColor (0xff, 0xff, 0xff)));
    setAutoFillBackground (true);

    resize (NUM_ROWS * ROW_HEIGHT, NUM_COLUMNS * COLUMN_WIDTH);
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);

    initSquareTypes();
    pixmap = makePixmap();
}

//---------------------------------------------------------------------------
//  makePixmap
//
//! Create a pixmap representation of the board.
//
//! @return the pixmap
//---------------------------------------------------------------------------
QPixmap
CrosswordGameBoardWidget::makePixmap()
{
    QPixmap pixmap (getBoardSize());
    QPainter painter (&pixmap);

    for (int row = 0; row < NUM_ROWS; ++row) {
        for (int col = 0; col < NUM_COLUMNS; ++col) {
            painter.setBrush (getBackgroundColor (row, col));
            painter.drawRect (col * COLUMN_WIDTH, row * ROW_HEIGHT,
                              COLUMN_WIDTH, ROW_HEIGHT);
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
CrosswordGameBoardWidget::getBackgroundColor (int row, int col)
{
    SquareType squareType = squareTypes[row][col];
    switch (squareType) {
        case NoBonus:      return NO_BONUS_COLOR;
        case DoubleLetter: return DOUBLE_LETTER_COLOR;
        case TripleLetter: return TRIPLE_LETTER_COLOR;
        case DoubleWord:   return DOUBLE_WORD_COLOR;
        case TripleWord:   return TRIPLE_WORD_COLOR;
        default:           return NO_BONUS_COLOR;
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
CrosswordGameBoardWidget::getBoardSize()
{
    return QSize (NUM_COLUMNS * COLUMN_WIDTH, NUM_ROWS * ROW_HEIGHT);
}

//---------------------------------------------------------------------------
//  initSquareTypes
//
//! Initialize the collection of square types on the board.
//---------------------------------------------------------------------------
void
CrosswordGameBoardWidget::initSquareTypes()
{
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
