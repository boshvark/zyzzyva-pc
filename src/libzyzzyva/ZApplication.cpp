//---------------------------------------------------------------------------
// ZApplication.cpp
//
// A class derived from QApplication.
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

#include "ZApplication.h"
#include <QFileOpenEvent>

#include <QMessageBox>

#if not defined Z_LINUX

//---------------------------------------------------------------------------
//  keyPressEvent
//
//! Called when this object receives an event.  If the event is a
//! QFileOpenEvent, emit the appropriate signal.
//
//! @param e the event
//---------------------------------------------------------------------------
bool
ZApplication::event(QEvent* e)
{
    if (!e)
        return false;

    switch (e->type()) {
        case QEvent::FileOpen: {
            QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(e);
            fileOpenRequests.append(fileEvent->file());
            emit fileOpenRequested(fileEvent->file());
            return true;
            break;
        }

        default:
        return QApplication::event(e);
    }
}

#endif
