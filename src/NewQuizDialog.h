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
#include <qcombobox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include "MatchType.h"

enum QuizTimerType {
    NoTimer,
    PerQuestion,
    PerResponse,
};

class QuizSpec;
class SearchSpec;
class SearchSpecForm;

class NewQuizDialog : public QDialog
{
  Q_OBJECT
  public:
    NewQuizDialog (QWidget* parent = 0, const char* name = 0,
                   bool modal = false, WFlags f = 0);

    ~NewQuizDialog() { }

    void setQuizSpec (const QuizSpec& spec);
    SearchSpec getSearchSpec() const;
    bool getQuizUseList() const;
    bool getQuizRandomOrder() const;
    bool getTimerEnabled() const;
    int  getTimerDuration() const;
    QuizTimerType getTimerType() const;

  public slots:
    void useListToggled (bool on);
    void timerToggled (bool on);

  private:
    SearchSpecForm* specForm;
    QCheckBox*      useListCbox;
    QCheckBox*      randomCbox;
    QPushButton*    okButton;
    QWidget*        timerWidget;
    QCheckBox*      timerCbox;
    QSpinBox*       timerSbox;
    QComboBox*      timerCombo;
};

#endif // NEW_QUIZ_DIALOG_H

