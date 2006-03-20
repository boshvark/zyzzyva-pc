//---------------------------------------------------------------------------
// LoadDefinitionsThread.h
//
// A class for loading definitions in the background.
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

#ifndef ZYZZYVA_LOAD_DEFINITIONS_THREAD_H
#define ZYZZYVA_LOAD_DEFINITIONS_THREAD_H

#include <QThread>
#include <QString>
#include <map>

class LoadDefinitionsThread : public QThread
{
    Q_OBJECT
    public:
    LoadDefinitionsThread (QObject* parent = 0)
        : QThread (parent) { }
    ~LoadDefinitionsThread() { }

    void setFilename (const QString& f) { filename = f; }
    std::map<QString, std::multimap<QString, QString> > getDefinitions() {
        return definitions;
    }

    signals:
    void loaded();

    protected:
    void run();

    private:
    void addDefinition (const QString& word, const QString& definition);

    QString filename;
    std::map<QString, std::multimap<QString, QString> > definitions;
};

#endif // ZYZZYVA_LOAD_DEFINITIONS_THREAD_H
