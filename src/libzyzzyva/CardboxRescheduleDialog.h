//---------------------------------------------------------------------------
// CardboxRescheduleDialog.h
//
// A dialog for rescheduling words within the cardbox system.
//
// Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_CARDBOX_RESCHEDULE_DIALOG_H
#define ZYZZYVA_CARDBOX_RESCHEDULE_DIALOG_H

#include "SearchSpec.h"
#include <QDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>

class SearchSpecForm;

class CardboxRescheduleDialog : public QDialog
{
    Q_OBJECT
    public:
    CardboxRescheduleDialog (QWidget* parent = 0, Qt::WFlags f = 0);
    ~CardboxRescheduleDialog() { }

    public:
    QString getQuizType() const;
    bool getShiftQuestions() const;
    int getBacklogSize() const;
    bool getRescheduleAll() const;
    SearchSpec getSearchSpec() const;

    public slots:
    void shiftQuestionsButtonToggled (bool checked);
    void useSearchButtonToggled (bool checked);

    private:
    QComboBox* quizTypeCombo;
    QRadioButton* shiftQuestionsButton;
    QSpinBox* backlogSbox;
    QRadioButton* rescheduleQuestionsButton;
    QRadioButton* selectAllButton;
    QRadioButton* selectSearchButton;
    QGroupBox* searchSpecGbox;
    SearchSpecForm* searchSpecForm;
};

#endif // ZYZZYVA_CARDBOX_RESCHEDULE_DIALOG_H

