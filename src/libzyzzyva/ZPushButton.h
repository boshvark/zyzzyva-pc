//---------------------------------------------------------------------------
// ZPushButton.h
//
// A class derived from ZPushButton.  This class allows buttons to be clicked
// by pressing Enter as well as the space bar.
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

#ifndef ZYZZYVA_PUSH_BUTTON_H
#define ZYZZYVA_PUSH_BUTTON_H

#include <QPushButton>

class ZPushButton : public QPushButton
{
    Q_OBJECT
    public:
    ZPushButton(QWidget* parent = 0)
        : QPushButton(parent) { init(); }
    ZPushButton(const QString& text, QWidget* parent = 0)
        : QPushButton(text, parent) { init(); }
    ZPushButton(const QIcon& icon, const QString& text, QWidget* parent = 0)
        : QPushButton(icon, text, parent) { init(); }

    protected:
    virtual void keyPressEvent(QKeyEvent* e);
    void init();

    virtual ~ZPushButton() { }
};

#endif // ZYZZYVA_PUSH_BUTTON_H
