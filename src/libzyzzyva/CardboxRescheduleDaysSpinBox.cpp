//---------------------------------------------------------------------------
// CardboxRescheduleDaysSpinBox.cpp
//
// A widget for rescheduling words by a number of days within the cardbox
// system.
//
// Copyright 2012 Boshvark Software, LLC.
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

#include "CardboxRescheduleDaysSpinBox.h"

const int MAX_SHIFT_DAYS = 5000;
const QString NONE_TEXT = "(select)";

//---------------------------------------------------------------------------
//  CardboxRescheduleDaysSpinBox
//
//! Constructor.
//
//! @param parent the parent widget
//---------------------------------------------------------------------------
CardboxRescheduleDaysSpinBox::CardboxRescheduleDaysSpinBox(QWidget* parent)
    : QSpinBox(parent)
{
    setMinimum(- MAX_SHIFT_DAYS);
    setMaximum(MAX_SHIFT_DAYS);
    setValue(0);
}

//---------------------------------------------------------------------------
//  textFromValue
//
//! Return a string corresponding to a value.
//
//! @param parent the parent widget
//---------------------------------------------------------------------------
QString
CardboxRescheduleDaysSpinBox::textFromValue(int value) const
{
    if (value == 0)
        return NONE_TEXT;

    QString modifier = (value > 0) ? "later" : "earlier";
    value = abs(value);

    QString text = QString("%1 day%2 %3").arg(value).arg(
        (value == 1) ? QString() : "s").arg(modifier);
    return text;
}
