//---------------------------------------------------------------------------
// LoadAnagramsThread.h
//
// A class for loading anagram counts in the background.
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

#ifndef ZYZZYVA_LOAD_ANAGRAMS_THREAD_H
#define ZYZZYVA_LOAD_ANAGRAMS_THREAD_H

#include <QMap>
#include <QThread>
#include <QString>

class LoadAnagramsThread : public QThread
{
    Q_OBJECT
    public:
    LoadAnagramsThread (QObject* parent = 0)
        : QThread (parent) { }
    ~LoadAnagramsThread() { }

    void setFilename (const QString& f) { filename = f; }
    QMap<QString, int> getAnagramCounts() { return anagramCounts; }

    signals:
    void loaded();

    protected:
    void run();

    QString filename;
    QMap<QString, int> anagramCounts;
};

#endif // ZYZZYVA_LOAD_ANAGRAMS_THREAD_H
