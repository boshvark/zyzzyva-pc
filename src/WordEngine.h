//---------------------------------------------------------------------------
// WordEngine.h
//
// A class to handle the loading and searching of words.
//
// Copyright 2004, 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef WORD_ENGINE_H
#define WORD_ENGINE_H

#include "WordGraph.h"
#include <qstring.h>
#include <qstringlist.h>
#include <map>

class WordEngine
{
  public:
    WordEngine() { }
    ~WordEngine() { }

    int importFile (const QString& filename, bool loadDefinitions = false,
                    QString* errString = 0);
    bool isAcceptable (const QString& word) const;
    QStringList search (const SearchSpec& spec, bool allCaps) const;
    QStringList alphagrams (const QStringList& list) const;
    QString alphagram (const QString& word) const;

  private:
    WordGraph graph;
    std::map<QString,QString> definitions;
};

#endif // WORD_ENGINE_H
