//---------------------------------------------------------------------------
// CrosswordGameBoardWidget.h
//
// A widget for displaying and manipulating a crossword game board.
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

#ifndef ZYZZYVA_CROSSWORD_GAME_BOARD_WIDGET_H
#define ZYZZYVA_CROSSWORD_GAME_BOARD_WIDGET_H

#include <QFrame>
#include <QPixmap>

class CrosswordGameBoardWidget : public QFrame
{
    Q_OBJECT

    public:
    enum SquareType {
        NoBonus,
        DoubleLetter,
        TripleLetter,
        DoubleWord,
        TripleWord
    };

    public:
    CrosswordGameBoardWidget (QWidget* parent = 0, Qt::WFlags f = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    private:
    QPixmap makePixmap() const;
    QColor getBackgroundColor (int row, int col) const;
    QSize getBoardSize() const;
    void initSquareTypes();

    virtual void paintEvent (QPaintEvent* event);

    private:
    QPixmap pixmap;
    QList< QList<SquareType> > squareTypes;
};

#endif // ZYZZYVA_CROSSWORD_GAME_BOARD_WIDGET_H
