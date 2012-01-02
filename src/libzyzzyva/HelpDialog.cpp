//---------------------------------------------------------------------------
// HelpDialog.cpp
//
// A help browser dialog.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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
#include "Auxil.h"
#include "Defs.h"
#include <QStringList>

//---------------------------------------------------------------------------
//  HelpDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param f widget flags
//---------------------------------------------------------------------------
HelpDialog::HelpDialog(const QString& path, QObject* parent)
    : QAssistantClient(path, parent)
{
    QString mainHelpFile = Auxil::getHelpDir() + "/zyzzyva.dcf";
    QStringList args;
    args << "-profile" << mainHelpFile;
    setArguments(args);
}
