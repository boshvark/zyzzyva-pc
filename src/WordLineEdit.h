//---------------------------------------------------------------------------
// WordLineEdit.h
//
// A class derived from QLineEdit, used to input words.  This class currently
// does nothing special.  It only exists so objects of this class can be
// distinguished from other QLineEdit objects when applying font settings.
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

#ifndef WORD_LINE_EDIT_H
#define WORD_LINE_EDIT_H

#include <qlineedit.h>

class WordLineEdit : public QLineEdit
{
    Q_OBJECT
    public:
    WordLineEdit (QWidget* parent, const char* name = 0)
        : QLineEdit (parent, name) { }
    WordLineEdit (const QString& contents, QWidget* parent, const char* name =
                  0)
        : QLineEdit (contents, parent, name) { }
    WordLineEdit (const QString& contents, const QString& inputMask, QWidget*
                  parent, const char* name = 0)
        : QLineEdit (contents, inputMask, parent, name) { }

    virtual ~WordLineEdit() { }
};

#endif // WORD_LINE_EDIT_H
