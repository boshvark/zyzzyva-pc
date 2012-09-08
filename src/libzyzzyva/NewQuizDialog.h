//---------------------------------------------------------------------------
// NewQuizDialog.h
//
// A dialog for prompting the user for a quiz.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_NEW_QUIZ_DIALOG_H
#define ZYZZYVA_NEW_QUIZ_DIALOG_H

#include "QuizSpec.h"
#include "MatchType.h"
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QSpinBox>

class LexiconSelectWidget;
class SearchSpec;
class SearchSpecForm;
class ZPushButton;

class NewQuizDialog : public QDialog
{
    Q_OBJECT
    public:
    NewQuizDialog(QWidget* parent = 0, Qt::WFlags f = 0);
    ~NewQuizDialog() { }

    QuizSpec getQuizSpec();
    void setQuizSpec(const QuizSpec& spec);

    public slots:
    void timerToggled(bool on);
    void typeActivated(const QString& text);
    void methodActivated(const QString& text);
    void useSearchButtonToggled(bool on);
    void searchContentsChanged();
    void questionOrderActivated(const QString& text);
    void probNumBlanksValueChanged(int num);
    void loadQuiz();
    void saveQuiz();

    private:
    void disableProgress();
    void clearFilename();
    void updateForm();
    void fillQuestionOrderCombo(const QString& method);

    private:
    LexiconSelectWidget* lexiconWidget;
    QComboBox*      typeCombo;
    QComboBox*      methodCombo;
    QCheckBox*      progressCbox;
    QLabel*         progressLabel;
    QComboBox*      questionOrderCombo;
    QLabel*         probNumBlanksLabel;
    QSpinBox*       probNumBlanksSbox;
    QStackedWidget* sourceStack;
    QWidget*        buildWidget;
    QSpinBox*       responseMinSbox;
    QSpinBox*       responseMaxSbox;
    QWidget*        searchWidget;
    QRadioButton*   allCardboxButton;
    QRadioButton*   useSearchButton;
    QGroupBox*      searchSpecGbox;
    SearchSpecForm* searchSpecForm;
    ZPushButton*    saveQuizButton;
    ZPushButton*    okButton;
    QWidget*        timerWidget;
    QCheckBox*      timerCbox;
    QSpinBox*       timerSbox;
    QComboBox*      timerCombo;

    QuizSpec        quizSpec;
};

#endif // ZYZZYVA_NEW_QUIZ_DIALOG_H

