//---------------------------------------------------------------------------
// WordValidator.cpp
//
// A validator for ensuring words are well-formed.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "WordValidator.h"

//-----------------------------------------------------------------------------
// validate
//
//! Validate an input string.  Possibly modify the string to make it conform,
//! by upper-casing and removing all whitespace.
//
//! @param input the string to validate
//! @param pos the cursor position
//-----------------------------------------------------------------------------
QValidator::State
WordValidator::validate (QString& input, int& pos) const
{
    input = input.upper();
    replaceRegExp(QRegExp("[^A-Z]+"), "", input, pos);
}

//-----------------------------------------------------------------------------
// replaceRegExp
//
//! Replace all sub strings matching the regular expression to the left and
//! right of the cursor.  Also adjusts the new cursor position accordingly,
//! so that the deletion/replacement of text does not make the cursor appear
//! to jump to a different spot.
//
//! @param re the regex to be replaced
//! @param str the string to replace each match
//! @param input the string to do the replacement within
//! @param pos the cursor position
//-----------------------------------------------------------------------------
void
WordValidator::replaceRegExp(const QRegExp& re, const QString& str,
                             QString& input, int& pos) const
{
    QString left = input.left(pos).replace(re, str);
    input = left + input.right(input.length() - pos).replace(re, str);
    if (input.isNull()) { input = ""; }
    pos = left.length();
}
