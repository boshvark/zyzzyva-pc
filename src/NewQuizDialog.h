//---------------------------------------------------------------------------
// NewQuizDialog.h
//
// A dialog for prompting the user for a quiz.
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

#ifndef NEW_QUIZ_DIALOG_H
#define NEW_QUIZ_DIALOG_H

#include <qcheckbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include "MatchType.h"

class SearchSpecForm;

class NewQuizDialog : public QDialog
{
  Q_OBJECT
  public:
    NewQuizDialog (QWidget* parent = 0, const char* name = 0,
                   bool modal = false, WFlags f = 0);

    ~NewQuizDialog() { }

    MatchType getQuizType() const;
    bool      getQuizAlphagrams() const;
    bool      getQuizRandomOrder() const;
    QString   getQuizString() const;

  public slots:
    void alphagramsToggled (bool on);
    void inputChanged (const QString& text);

  private:
    SearchSpecForm* specForm;
    QCheckBox*      alphagramCbox;
    QCheckBox*      randomCbox;
    QPushButton*    okButton;
};

#endif // NEW_QUIZ_DIALOG_H

