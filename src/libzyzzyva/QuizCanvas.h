//---------------------------------------------------------------------------
// QuizCanvas.h
//
// A drawing surface for quiz questions.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_QUIZ_CANVAS_H
#define ZYZZYVA_QUIZ_CANVAS_H

#include <QBrush>
#include <QLabel>
#include <QMap>
#include <QPen>
#include <QPixmap>
#include <QWidget>

class QuizCanvas : public QWidget
{
    Q_OBJECT
    public:
    QuizCanvas(QWidget *parent = 0);

    void clear();
    void setText(const QString& text);
    void clearTileTheme();
    void setTileTheme(const QString& theme);
    void setNumCanvasTiles(int num);
    void setToMinimumSize();
    bool hasTileImages();
    void setDragDropEnabled(bool on) { dragDropEnabled = on; }

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void mousePressEvent(QMouseEvent* event);

    private slots:
    void squeezeTileImages();

    private:
    QMap<QString, QPixmap> tileImages;
    QString question;
    int maxTileWidth, maxTileHeight;
    int numCanvasTiles, minCanvasTiles, minCanvasWidth;
    int widthHint, heightHint;
    bool dragDropEnabled;
};

#endif // ZYZZYVA_QUIZ_CANVAS_H
