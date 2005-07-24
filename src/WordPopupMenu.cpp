//---------------------------------------------------------------------------
// WordPopupMenu.cpp
//
// A popup menu to be executed when the user right-clicks on a word list.
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

#include "WordPopupMenu.h"

//---------------------------------------------------------------------------
//  WordPopupMenu
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//---------------------------------------------------------------------------
WordPopupMenu::WordPopupMenu (bool showWordOptions, QWidget* parent, const
                              char* name)
    : QPopupMenu (parent, name)
{
    if (showWordOptions) {
        QPopupMenu* viewPopup = new QPopupMenu (this, "viewPopup");
        viewPopup->insertItem ("Definition", ViewDefinition);
        viewPopup->insertItem ("Anagrams", ViewAnagrams);
        viewPopup->insertItem ("Subanagrams", ViewSubanagrams);
        viewPopup->insertItem ("Hooks", ViewHooks);
        viewPopup->insertItem ("Extensions", ViewExtensions);
        viewPopup->insertItem ("Anagram Hooks", ViewAnagramHooks);
        viewPopup->insertItem ("Blank Anagrams", ViewBlankAnagrams);
        viewPopup->insertItem ("Blank Matches", ViewBlankMatches);
        viewPopup->insertItem ("Transpositions", ViewTranspositions);
        insertItem ("View", viewPopup);
    }
    insertItem ("Save list...", ExportList);
    insertItem ("Quiz from list...", CreateQuiz);
}

//---------------------------------------------------------------------------
//  ~WordPopupMenu
//
//! Destructor.
//---------------------------------------------------------------------------
WordPopupMenu::~WordPopupMenu()
{
}
