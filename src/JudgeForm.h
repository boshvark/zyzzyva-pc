//---------------------------------------------------------------------------
// JudgeForm.h
//
// A form for looking up words.
//
// Copyright 2004, 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef ZYZZYVA_JUDGE_FORM_H
#define ZYZZYVA_JUDGE_FORM_H

#include "ActionForm.h"

class DefinitionBox;
class WordEngine;
class WordTextEdit;
class ZPushButton;

class JudgeForm : public ActionForm
{
    Q_OBJECT
    public:
    JudgeForm (WordEngine* e, QWidget* parent = 0, Qt::WFlags f = 0);
    QString getStatusString() const;

    public slots:
    void textChanged();
    void clear();
    void judgeWord();

    private:
    WordEngine*    engine;
    WordTextEdit*  wordArea;
    ZPushButton*   clearButton;
    ZPushButton*   judgeButton;
    DefinitionBox* resultBox;
    QString        statusString;
};

#endif // ZYZZYVA_JUDGE_FORM_H
