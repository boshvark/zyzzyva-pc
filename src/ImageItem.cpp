//---------------------------------------------------------------------------
// ImageItem.h
//
// A class to hold an image and display it on a canvas.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#include "ImageItem.h"

//---------------------------------------------------------------------------
//  ImageItem
//
//! Constructor.  Converts a QImage to a QPixmap and stores it.
//
//! @param image the image to store in this object
//! @param canvas the canvas object to associate with this rectangle
//---------------------------------------------------------------------------
ImageItem::ImageItem (const QImage& image, QCanvas* canvas)
    : QCanvasRectangle (canvas)
{
    pixmap.convertFromImage (image);
}

//---------------------------------------------------------------------------
//  ImageItem
//
//! Constructor.  Takes a QPixmap and stores it.
//
//! @param pixmap the pixmap to store in this object
//! @param canvas the canvas object to associate with this rectangle
//---------------------------------------------------------------------------
ImageItem::ImageItem (const QPixmap& p, QCanvas* canvas)
    : QCanvasRectangle (canvas), pixmap (p)
{
}

//---------------------------------------------------------------------------
//  drawShape
//
//! Implementation of virtual QCanvasRectangle method.  Draws the pixmap via a
//! painter object.
//
//! @param painter the painter to do the drawing
//---------------------------------------------------------------------------
void
ImageItem::drawShape (QPainter& painter)
{
    painter.drawPixmap (int (x()), int (y()), pixmap);
}
