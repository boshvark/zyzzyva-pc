//---------------------------------------------------------------------------
// Auxil.cpp
//
// Auxiliary functions.
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

#include "Auxil.h"
#include <qapplication.h>
#include <qfile.h>

//---------------------------------------------------------------------------
//  getAboutString
//
//! Return the About string for the application.  Read it from a file
//! initially, and store the read value in a static variable.
//
//! @return the About string
//---------------------------------------------------------------------------
QString
Auxil::getAboutString()
{
    static QString aboutString;
    if (!aboutString.isEmpty())
        return aboutString;

    QString aboutFileName = getHelpDir() + "/about.html";
    QFile aboutFile (aboutFileName);
    if (!aboutFile.open (IO_ReadOnly))
        return aboutString;

    aboutString.append (aboutFile.readAll());
    return aboutString;
}

//---------------------------------------------------------------------------
//  getHelpDir
//
//! Return the top-level directory containing the help documentation.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getHelpDir()
{
    return qApp->applicationDirPath() + "/data/help";
}

//---------------------------------------------------------------------------
//  getTilesDir
//
//! Return the top-level directory containing subdirectories with tiles
//! images.
//
//! @return the directory name
//---------------------------------------------------------------------------
QString
Auxil::getTilesDir()
{
    return qApp->applicationDirPath() + "/data/tiles";
}

//---------------------------------------------------------------------------
//  wordWrap
//
//! Wrap a string so that no line is longer than a certain length.
//
//! @param str the string to wrap
//! @param wrapLength the maximum length of a line
//! @return the wrapped string
//---------------------------------------------------------------------------
QString
Auxil::wordWrap (const QString& str, int wrapLength)
{
    int strLen = str.length();
    if (strLen <= wrapLength)
        return str;

    QChar c;
    QString wrappedStr = str;
    int lastSpace = 0;
    int lastNewline = 0;
    for (int i = 0; i < strLen; ++i) {
        c = wrappedStr.at (i);
        if (c == '\n')
            lastNewline = i;
        if (c.isSpace())
            lastSpace = i;

        if ((i - lastNewline) == wrapLength) {
            wrappedStr[lastSpace] = '\n';
            lastNewline = lastSpace;
        }
    }
    return wrappedStr;
}
