//---------------------------------------------------------------------------
// QuizCanvas.cpp
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

#include "QuizCanvas.h"
#include "MainSettings.h"
#include "Auxil.h"
#include "Defs.h"
#include <QtGui>

using namespace Defs;

const QString QUIZ_TILE_MIME_TYPE = "application/x-zyzzyva-quiz-tile";
const QString LABEL_SHOW_STRING = "show";

//---------------------------------------------------------------------------
//  xLessThan
//
//! A comparison function that compares QLabels by X position.
//
//! @param a the first label to compare
//! @param b the second label to compare
//! @return true if a is to the left of b
//---------------------------------------------------------------------------
bool
xLessThan(const QLabel* a, const QLabel* b)
{
    return (a->x() < b->x());
}

//---------------------------------------------------------------------------
//  QuizCanvas
//
//! Constructor.
//
//! @param parent the parent widget
//---------------------------------------------------------------------------
QuizCanvas::QuizCanvas(QWidget* parent)
    : QWidget(parent), numCanvasTiles(0), minCanvasTiles(7),
      minCanvasWidth(300), widthHint(minCanvasWidth), heightHint(100),
      dragDropEnabled(true)
{
    setAcceptDrops(true);
    setAutoFillBackground(true);
}

//---------------------------------------------------------------------------
//  clear
//
//! Clear the canvas.
//---------------------------------------------------------------------------
void
QuizCanvas::clear()
{
    setText(QString());
}

//---------------------------------------------------------------------------
//  setText
//
//! Set the quiz question text.
//
//! @param text the question text
//---------------------------------------------------------------------------
void
QuizCanvas::setText(const QString& text)
{
    question = text;
    setNumCanvasTiles(question.length());

    // Remove existing labels
    QList<QLabel*> labels = findChildren<QLabel*>();
    QListIterator<QLabel*> it (labels);
    while (it.hasNext()) {
        QLabel* label = it.next();
        label->setObjectName(QString());
        label->close();
    }

    QMap<QString, QPixmap>::iterator pixmap;
    for (int i = 0; (i < numCanvasTiles) &&
                    (i < int(question.length())); ++i)
    {
        QString letter = QString(question[i]);
        if (letter == "?")
            letter = "_";
        pixmap = tileImages.find(letter);
        if (pixmap == tileImages.end()) {
            //qWarning("Did not find letter '" + letter +
            //         "' in tiles map!");
        }
        else {
            QLabel* label = new QLabel(this);
            label->setObjectName(LABEL_SHOW_STRING);
            label->setPixmap(*pixmap);
            label->move(i, QUIZ_TILE_MARGIN);
            label->setAttribute(Qt::WA_DeleteOnClose);
        }
    }

    squeezeTileImages();
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
QuizCanvas::setTileTheme(const QString& theme)
{
    clearTileTheme();
    if (!theme.isEmpty()) {
        QString dirStr = Auxil::getTilesDir() + "/" + theme;
        QDir dir (dirStr);
        if (!dir.exists())
            return;

        QStringList imageFiles = dir.entryList(QStringList() << "*.png");
        foreach (const QString& file, imageFiles) {
            QString letter = file.left(file.length() - 4);
            if (letter.isEmpty())
                continue;

            QImage image (dirStr + "/" + file);
            QPixmap pixmap = QPixmap::fromImage(image);
            tileImages.insert(letter, pixmap);

            if (pixmap.width() > maxTileWidth)
                maxTileWidth = pixmap.width();
            if (pixmap.height() > maxTileHeight)
                maxTileHeight = pixmap.height();
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
QuizCanvas::setNumCanvasTiles(int num)
{
    if (num < minCanvasTiles)
        num = minCanvasTiles;
    numCanvasTiles = num;
    widthHint = (2 * QUIZ_TILE_MARGIN) + (num * maxTileWidth) +
                ((num - 1) * QUIZ_TILE_SPACING);
    heightHint = (2 * QUIZ_TILE_MARGIN) + maxTileHeight;
    resize(widthHint, heightHint);
    updateGeometry();
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
    if (tileImages.empty()) {
        resize(minCanvasWidth, (2 * QUIZ_TILE_MARGIN) + maxTileHeight);
        updateGeometry();
    }
    else
        setNumCanvasTiles(minCanvasTiles);
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
    return QSize(widthHint, heightHint);
}

//---------------------------------------------------------------------------
//  sizeHint
//
//! Set the size hint of the canvas.
//---------------------------------------------------------------------------
QSize
QuizCanvas::sizeHint() const
{
    return QSize(widthHint, heightHint);
}

//---------------------------------------------------------------------------
//  dragEnterEvent
//
//! The event handler that receives drag enter events.
//
//! @param event the drag enter event
//---------------------------------------------------------------------------
void
QuizCanvas::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(QUIZ_TILE_MIME_TYPE)) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

//---------------------------------------------------------------------------
//  dropEvent
//
//! The event handler that receives drop events.
//
//! @param event the drop event
//---------------------------------------------------------------------------
void
QuizCanvas::dropEvent(QDropEvent* event)
{
    if (dragDropEnabled && event->mimeData()->hasFormat(QUIZ_TILE_MIME_TYPE)) {
        QByteArray itemData = event->mimeData()->data(QUIZ_TILE_MIME_TYPE);
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint sourcePos;
        QPoint offset;
        dataStream >> pixmap >> sourcePos >> offset;

        QLabel* label = new QLabel(this);
        label->setObjectName(LABEL_SHOW_STRING);
        label->setPixmap(pixmap);
        QPoint dropPos = event->pos() - offset;

        // Move the tile an extra half tile width in the direction of the
        // move.  This allows the tile to assume a new spot if it is dragged
        // more than halfway onto the spot.
        int extraMove = (sourcePos.x() < dropPos.x() ? maxTileWidth / 2
                                                     : -maxTileWidth / 2);
        dropPos.setX(dropPos.x() + extraMove);
        label->move(dropPos);
        label->setAttribute(Qt::WA_DeleteOnClose);

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    }
    else {
        event->ignore();
    }
}

//---------------------------------------------------------------------------
//  mousePressEvent
//
//! The event handler that receives mouse press events.
//
//! @param event the mouse press event
//---------------------------------------------------------------------------
void
QuizCanvas::mousePressEvent(QMouseEvent* event)
{
    if (!dragDropEnabled)
        return;

    QLabel* child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child)
        return;

    QPixmap pixmap = *(child->pixmap());

    QByteArray itemData;
    QDataStream dataStream (&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos())
               << QPoint(event->pos() - child->pos());

    QMimeData* mimeData = new QMimeData;
    mimeData->setData(QUIZ_TILE_MIME_TYPE, itemData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());

    QColor bgColor = palette().color(QPalette::Window);

    QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(bgColor.red(), bgColor.green(),
                                           bgColor.blue(), 127));
    painter.end();

    child->setPixmap(tempPixmap);

    if (drag->start(Qt::CopyAction | Qt::MoveAction) == Qt::MoveAction) {
        child->setObjectName(QString());
        child->close();
    }
    else {
        child->show();
        child->setPixmap(pixmap);
    }

    squeezeTileImages();
}

//---------------------------------------------------------------------------
//  squeezeTileImages
//
//! Squeeze and center tile images without reordering them.
//---------------------------------------------------------------------------
void
QuizCanvas::squeezeTileImages()
{
    QList<QLabel*> labels = findChildren<QLabel*>(LABEL_SHOW_STRING);
    qSort(labels.begin(), labels.end(), xLessThan);

    int x = QUIZ_TILE_MARGIN + ((numCanvasTiles - labels.size()) *
            (maxTileWidth + QUIZ_TILE_SPACING)) / 2;

    // Move the tiles
    QListIterator<QLabel*> it (labels);
    while (it.hasNext()) {
        QLabel* label = it.next();
        label->move(x, QUIZ_TILE_MARGIN);
        x += maxTileWidth + QUIZ_TILE_SPACING;
    }

    // Show the tiles
    it.toFront();
    while (it.hasNext())
        it.next()->show();
}
