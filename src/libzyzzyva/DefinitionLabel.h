//---------------------------------------------------------------------------
// DefinitionLabel.h
//
// A class derived from QLabel, used to display word definitions.  This class
// currently does nothing special.  It only exists so objects of this class
// can be distinguished from other QLabel objects when applying font settings.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_DEFINITION_LABEL_H
#define ZYZZYVA_DEFINITION_LABEL_H

#include <QLabel>

class DefinitionLabel : public QLabel
{
    Q_OBJECT
    public:
    DefinitionLabel(QWidget* parent = 0, Qt::WFlags f = 0)
        : QLabel(parent, f) { }
    DefinitionLabel(const QString& text, QWidget* parent = 0,
                    Qt::WFlags f = 0)
        : QLabel(text, parent, f) { }

    virtual ~DefinitionLabel() { }
};

#endif // ZYZZYVA_DEFINITION_LABEL_H
