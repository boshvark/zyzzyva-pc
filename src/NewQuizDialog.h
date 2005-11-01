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

#include "QuizSpec.h"
#include "MatchType.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

class SearchSpec;
class SearchSpecForm;

class NewQuizDialog : public QDialog
{
    Q_OBJECT
    public:
    NewQuizDialog (QWidget* parent = 0, Qt::WFlags f = 0);
    ~NewQuizDialog() { }

    QuizSpec getQuizSpec();
    void setQuizSpec (const QuizSpec& spec);

    public slots:
    void timerToggled (bool on);
    void typeActivated (const QString& text);
    void loadQuiz();
    void saveQuiz();

    private:
    SearchSpecForm* specForm;
    QComboBox*      typeCombo;
    QCheckBox*      randomCbox;
    QPushButton*    saveQuizButton;
    QPushButton*    okButton;
    QWidget*        timerWidget;
    QCheckBox*      timerCbox;
    QSpinBox*       timerSbox;
    QComboBox*      timerCombo;

    QuizSpec        quizSpec;
};

#endif // NEW_QUIZ_DIALOG_H

