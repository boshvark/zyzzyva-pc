//---------------------------------------------------------------------------
// JudgeForm.h
//
// A form for looking up words.
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

#ifndef JUDGE_FORM_H
#define JUDGE_FORM_H

#include "ActionForm.h"
#include <qtextedit.h>
#include <qpushbutton.h>

class DefinitionBox;
class WordEngine;

class JudgeForm : public ActionForm
{
    Q_OBJECT
    public:
    JudgeForm (WordEngine* e, QWidget* parent = 0, const char* name = 0,
               WFlags f = 0);

    public slots:
    void textChanged();
    void clear();
    void judgeWord();

    private:
    WordEngine*  engine;
    QTextEdit*   wordArea;
    QPushButton* clearButton;
    QPushButton* judgeButton;
    DefinitionBox* resultBox;
};

#endif // JUDGE_FORM_H
