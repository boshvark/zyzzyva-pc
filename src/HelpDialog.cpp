//---------------------------------------------------------------------------
// HelpDialog.cpp
//
// A help browser dialog.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#include "HelpDialog.h"
#include "Defs.h"
#include <qfile.h>
#include <qstringlist.h>

//---------------------------------------------------------------------------
// HelpDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param f widget flags
//---------------------------------------------------------------------------
HelpDialog::HelpDialog (const QString& path, QObject* parent, const char* name)
    : QAssistantClient (path, parent, name), valid (false)
{
    QString adpFilename = "docs/help/zyzzyva.dcf";
    QFile adpFile (adpFilename);
    if (!adpFile.open (IO_WriteOnly)) {
        qWarning ("Unable to open " + adpFilename);
        return;
    }

    QString aboutFilename = "docs/help/about.html";
    QFile aboutFile (aboutFilename);
    if (!aboutFile.open (IO_WriteOnly)) {
        qWarning ("Unable to open " + aboutFilename);
        return;
    }
    QTextStream aboutOut (&aboutFile);
    aboutOut << Defs::ABOUT_STRING;

    QTextStream adpOut (&adpFile);
    adpOut << "<assistantconfig version=\"" << QT_VERSION_STR << "\">\n"
           << "  <profile>\n"
           << "    <property name=\"name\">Zyzzyva</property>\n"
           << "    <property name=\"title\">Zyzzyva</property>\n"
           //<< "    <property name=\"applicationicon\">logo.png</property>\n"
           << "    <property name=\"startpage\">./index.html</property>\n"
           << "    <property name=\"aboutmenutext\">About Zyzzyva</property>\n"
           << "    <property name=\"abouturl\">./about.html</property>\n"
           << "  </profile>\n"
           << "  <DCF ref=\"./index.html\" title=\"Zyzzyva\">\n"
           << "  </DCF>\n"
           << "</assistantconfig>\n";

    QStringList args;
    args << "-profile" << adpFilename;
    setArguments (args);
    valid = true;
}
