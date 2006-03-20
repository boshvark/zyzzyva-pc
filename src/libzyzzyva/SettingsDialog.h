//---------------------------------------------------------------------------
// SettingsDialog.h
//
// The settings dialog for the word study application.
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

#ifndef ZYZZYVA_SETTINGS_DIALOG_H
#define ZYZZYVA_SETTINGS_DIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QSettings>
#include <QStackedWidget>

class SettingsDialog : public QDialog
{
    Q_OBJECT
    public:
    SettingsDialog (QWidget* parent = 0, Qt::WFlags f = 0);
    ~SettingsDialog();

    void readSettings();
    void writeSettings();

    public slots:
    void navTextChanged (const QString& text);
    void autoImportBrowseButtonClicked();
    void autoImportCboxToggled (bool on);
    void autoImportLexiconActivated (const QString& text);
    void themeCboxToggled (bool on);
    void autoCheckCboxToggled (bool on);
    void judgeLogBrowseButtonClicked();
    void judgeSaveLogCboxToggled (bool on);
    void chooseFontButtonClicked (int button);
    void chooseQuizBackgroundColorButtonClicked();

    private:
    void fillThemeCombo();

    private:
    QListWidget*    navList;
    QStackedWidget* navStack;

    QWidget*     generalPrefWidget;
    QWidget*     quizPrefWidget;
    QWidget*     judgePrefWidget;
    QWidget*     fontPrefWidget;
    QWidget*     wordListPrefWidget;

    QCheckBox*   autoImportCbox;
    QComboBox*   autoImportLexiconCombo;
    QWidget*     autoImportCustomWidget;
    QLineEdit*   autoImportCustomLine;
    QCheckBox*   themeCbox;
    QLabel*      themeLabel;
    QComboBox*   themeCombo;
    QComboBox*   letterOrderCombo;
    QLineEdit*   quizBackgroundColorLine;
    QCheckBox*   quizUseFlashcardModeCbox;
    QCheckBox*   quizShowNumResponsesCbox;
    QCheckBox*   quizAutoCheckCbox;
    QCheckBox*   quizAutoAdvanceCbox;
    QCheckBox*   quizAutoEndAfterIncorrectCbox;
    QCheckBox*   quizCycleAnswersCbox;
    QCheckBox*   judgeSaveLogCbox;
    QWidget*     judgeLogDirWidget;
    QLineEdit*   judgeLogDirLine;
    QLineEdit*   fontMainLine;
    QLineEdit*   fontWordListLine;
    QLineEdit*   fontQuizLabelLine;
    QLineEdit*   fontDefinitionLine;
    QLineEdit*   fontWordInputLine;
    QCheckBox*   lengthSortCbox;
    QCheckBox*   anagramGroupCbox;
    QCheckBox*   showHookParentsCbox;
    QCheckBox*   showHooksCbox;
    QCheckBox*   showDefinitionCbox;

    QColor       quizBackgroundColor;

};

#endif // ZYZZYVA_SETTINGS_DIALOG_H
