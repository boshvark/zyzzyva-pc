//---------------------------------------------------------------------------
// ZApplication.h
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

#ifndef ZYZZYVA_APPLICATION_H
#define ZYZZYVA_APPLICATION_H

#include <QApplication>
#include <QStringList>

// Admitting defeat for now: ZApplication causes a crash on certain Linux
// machines and I haven't quite figured out why. So I'm disabling the magic
// file-loading on Linux that ZApplication provides.
#if defined Z_LINUX
typedef QApplication ZApplication;
#else

class ZApplication : public QApplication
{
    Q_OBJECT
    public:
    ZApplication(int& argc, char** argv)
        : QApplication(argc, argv) { }
    ZApplication(int& argc, char** argv, bool GUIenabled)
        : QApplication(argc, argv, GUIenabled) { }
    ZApplication(int& argc, char** argv, Type type)
        : QApplication(argc, argv, type) { }

#if defined Z_LINUX
    // Only available on X11
    ZApplication(Display* display, Qt::HANDLE visual = 0,
                 Qt::HANDLE colormap = 0)
        : QApplication(display, visual, colormap) { }
    ZApplication(Display* display, int& argc, char** argv,
                 Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0)
        : QApplication(display, argc, argv, visual, colormap) { }
#endif

    virtual ~ZApplication() { }

    virtual bool event(QEvent* e);

    void clearFileOpenRequests() { fileOpenRequests.clear(); }
    QStringList getFileOpenRequests() const { return fileOpenRequests; }

    signals:
    void fileOpenRequested(const QString& file);

    private:
    QStringList fileOpenRequests;
};

#endif

#endif // ZYZZYVA_APPLICATION_H
