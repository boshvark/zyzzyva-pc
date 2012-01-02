//---------------------------------------------------------------------------
// ZPushButton.cpp
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

#include "ZPushButton.h"
#include <QKeyEvent>

//---------------------------------------------------------------------------
//  init
//
//! Initialize the object.
//---------------------------------------------------------------------------
void
ZPushButton::init()
{
    setFocusPolicy(Qt::TabFocus);
}

//---------------------------------------------------------------------------
//  keyPressEvent
//
//! Called when this widget receives a key press event.  If the key is Enter
//! or Return, perform a button click.  Otherwise, let the parent class
//! implementation handle the event.
//
//! @param e the key press event
//---------------------------------------------------------------------------
void
ZPushButton::keyPressEvent(QKeyEvent* e)
{
    if (e && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter)))
        animateClick();
    else
        QPushButton::keyPressEvent(e);
}
