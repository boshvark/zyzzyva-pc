//---------------------------------------------------------------------------
// QuizCanvas.cpp
//
// A drawing surface for quiz questions.
//
// Copyright 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#include <QtGui>

#include "QuizCanvas.h"
#include "MainSettings.h"
#include "Auxil.h"
#include "Defs.h"

using namespace Defs;

//---------------------------------------------------------------------------
//  QuizCanvas
//
//! Constructor.
//
//! @param parent the parent widget
//---------------------------------------------------------------------------
QuizCanvas::QuizCanvas (QWidget* parent)
    : QWidget(parent), numCanvasTiles (0), minCanvasTiles (7),
      minCanvasWidth (300), widthHint (minCanvasWidth), heightHint (100)
{
    setBackgroundRole (QPalette::Base);
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the canvas.
//---------------------------------------------------------------------------
void
QuizCanvas::clear()
{
    setText (QString::null);
}

//---------------------------------------------------------------------------
//  setText
//
//! Set the quiz question text.
//
//! @param text the question text
//---------------------------------------------------------------------------
void
QuizCanvas::setText (const QString& text)
{
    question = text;
    setNumCanvasTiles (question.length());
    displayImages.clear();

    QMap<QString,QImage>::iterator image;
    for (int i = 0; (i < numCanvasTiles) &&
                    (i < int (question.length())); ++i)
    {
        QString letter = QString (question[i]);
        if (letter == "?")
            letter = "_";
        image = tileImages.find (letter);
        if (image == tileImages.end()) {
            //qWarning ("Did not find letter '" + letter +
            //            "' in tiles map!");
        }
        else {
            displayImages.append (*image);
        }
    }
}

//---------------------------------------------------------------------------
//  clearTileTheme
//
//! Clear the current tile theme definition.
//---------------------------------------------------------------------------
void
QuizCanvas::clearTileTheme()
{
    tileImages.clear();
    maxTileWidth = 25;
    maxTileHeight = 25;
    widthHint = minCanvasWidth;
    heightHint = 100;
}

//---------------------------------------------------------------------------
//  setTileTheme
//
//! Load tile images according to a theme definition.  Find the tile images in
//! a data subdirectory named after the theme.
//
//! @param theme the name of the theme
//---------------------------------------------------------------------------
void
QuizCanvas::setTileTheme (const QString& theme)
{
    clearTileTheme();
    if (!theme.isEmpty()) {
        QString tilesDir = Auxil::getTilesDir();

        QStringList tilesList;
        for (char c = 'A'; c <= 'Z'; ++c)
            tilesList << QString (QChar (c));
        tilesList << "_";

        QStringList::iterator it;
        for (it = tilesList.begin(); it != tilesList.end(); ++it) {
            QImage image (tilesDir + "/" + theme + "/" + *it + ".png");
            tileImages.insert (*it, image);

            if (image.width() > maxTileWidth)
                maxTileWidth = image.width();
            if (image.height() > maxTileHeight)
                maxTileHeight = image.height();
        }
    }
}

//---------------------------------------------------------------------------
//  setNumCanvasTiles
//
//! Resize the canvas to be able to display a certain number of tiles.
//
//! @param num the number of tiles
//---------------------------------------------------------------------------
void
QuizCanvas::setNumCanvasTiles (int num)
{
    if (num < minCanvasTiles)
        num = minCanvasTiles;
    numCanvasTiles = num;
    widthHint = (2 * QUIZ_TILE_MARGIN) + (num * maxTileWidth) +
                ((num - 1) * QUIZ_TILE_SPACING);
    heightHint = (2 * QUIZ_TILE_MARGIN) + maxTileHeight;
    resize (widthHint, heightHint);
}

//---------------------------------------------------------------------------
//  setToMinimumSize
//
//! Set the canvas to its minimum size.
//
//! @param num the number of tiles
//---------------------------------------------------------------------------
void
QuizCanvas::setToMinimumSize()
{
    if (tileImages.empty())
        resize (minCanvasWidth, (2 * QUIZ_TILE_MARGIN) + maxTileHeight);
    else
        setNumCanvasTiles (minCanvasTiles);
}

//---------------------------------------------------------------------------
//  hasTileImages
//
//! Determine whether there are tile images set for the canvas.
//
//! @return true if there are tile images
//---------------------------------------------------------------------------
bool
QuizCanvas::hasTileImages()
{
    return !tileImages.empty();
}

//---------------------------------------------------------------------------
//  minimumSizeHint
//
//! Set the minimum size hint of the canvas.
//---------------------------------------------------------------------------
QSize
QuizCanvas::minimumSizeHint() const
{
    return QSize (widthHint, heightHint);
}

//---------------------------------------------------------------------------
//  sizeHint
//
//! Set the size hint of the canvas.
//---------------------------------------------------------------------------
QSize
QuizCanvas::sizeHint() const
{
    return QSize (widthHint, heightHint);
}

//---------------------------------------------------------------------------
//  paintEvent
//
//! Called whenever a paint event is requested.
//
//! @param event the paint event
//---------------------------------------------------------------------------
void
QuizCanvas::paintEvent (QPaintEvent*)
{
    QColor backgroundColor = MainSettings::getQuizBackgroundColor();

    QPainter painter (this);
    painter.setBrush (QBrush (backgroundColor));
    painter.drawRect (QRect (0, 0, width(), height()));

    int x = QUIZ_TILE_MARGIN + ((numCanvasTiles - question.length()) *
            (maxTileWidth + QUIZ_TILE_SPACING)) / 2;

    QListIterator<QImage> it (displayImages);

    while (it.hasNext()) {
        QImage image = it.next();
        painter.drawImage (x, QUIZ_TILE_MARGIN, image);
        x += maxTileWidth + QUIZ_TILE_SPACING;
    }
}
