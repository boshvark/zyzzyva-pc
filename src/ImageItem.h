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

#ifndef IMAGE_ITEM_H
#define IMAGE_ITEM_H

#include <Q3Canvas>
#include <QPainter>
#include <QPixmap>

class ImageItem : public Q3CanvasRectangle
{
    public:
    ImageItem (const QImage& image, Q3Canvas* canvas);
    ImageItem (const QPixmap& p, Q3Canvas* canvas);

    virtual void drawShape (QPainter& p);

    private:
    QPixmap pixmap;
};

#endif // IMAGE_ITEM_H
