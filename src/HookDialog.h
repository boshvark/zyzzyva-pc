//---------------------------------------------------------------------------
// HookDialog.h
//
// A dialog for displaying the front and back hooks of a word.
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

#ifndef HOOK_DIALOG_H
#define HOOK_DIALOG_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>

class WordEngine;
class WordListView;

class HookDialog : public QDialog
{
  Q_OBJECT
  public:
    HookDialog (WordEngine* we, const QString& word, QWidget* parent = 0,
                const char* name = 0, bool modal = false, WFlags f = 0);
    ~HookDialog();

  private:
    void setWord (const QString& word);

  private:
    WordEngine*   wordEngine;
    QLabel*       wordLabel;
    WordListView* frontList;
    WordListView* backList;
    QPushButton*  closeButton;
};

#endif // HOOK_DIALOG_H

