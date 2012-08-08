//---------------------------------------------------------------------------
// CardboxRescheduleDialog.h
//
// A dialog for rescheduling words within the cardbox system.
//
// Copyright 2006-2012 Boshvark Software, LLC.
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
#include "CardboxRescheduleType.h"
#include <QDialog>
#include <QComboBox>
#include <QGroupBox>
#include <QListWidget>
#include <QRadioButton>
#include <QSpinBox>

class CardboxRescheduleDaysSpinBox;
class LexiconSelectWidget;
class SearchSpecForm;
class ZPushButton;

class CardboxRescheduleDialog : public QDialog
{
    Q_OBJECT
    public:
    CardboxRescheduleDialog(QWidget* parent = 0, Qt::WFlags f = 0);
    ~CardboxRescheduleDialog() { }

    public:
    QString getLexicon() const;
    QString getQuizType() const;
    CardboxRescheduleType getRescheduleType() const;
    int getBacklogSize() const;
    int getNumDays() const;
    bool getRescheduleAll() const;
    SearchSpec getSearchSpec() const;

    public slots:
    void shiftDaysButtonToggled(bool checked);
    void shiftDaysValueChanged(int value);
    void shiftQuestionsButtonToggled(bool checked);
    void useSearchButtonToggled(bool checked);

    private:
    LexiconSelectWidget* lexiconWidget;
    QComboBox* quizTypeCombo;
    QRadioButton* shiftDaysButton;
    CardboxRescheduleDaysSpinBox* daysSbox;
    QRadioButton* shiftQuestionsButton;
    QSpinBox* backlogSbox;
    QRadioButton* rescheduleQuestionsButton;
    QRadioButton* selectAllButton;
    QRadioButton* selectSearchButton;
    QGroupBox* searchSpecGbox;
    SearchSpecForm* searchSpecForm;
    ZPushButton* okButton;
};

#endif // ZYZZYVA_CARDBOX_RESCHEDULE_DIALOG_H

