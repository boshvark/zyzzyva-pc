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

class ZPushButton;

class SettingsDialog : public QDialog
{
    Q_OBJECT
    public:
    SettingsDialog (QWidget* parent = 0, Qt::WFlags f = Qt::WType_TopLevel);
    ~SettingsDialog();

    void readSettings();
    void writeSettings();

    public slots:
    void navTextChanged (const QString& text);
    void browseButtonClicked();
    void autoImportCboxToggled (bool on);
    void themeCboxToggled (bool on);
    void chooseFontButtonClicked (int button);
    void chooseQuizBackgroundColorButtonClicked();

    private:
    void fillThemeCombo();

    private:
    QListWidget*    navList;
    QStackedWidget* navStack;

    QWidget*     generalPrefWidget;
    QWidget*     quizPrefWidget;
    QWidget*     fontPrefWidget;
    QWidget*     wordListPrefWidget;

    QCheckBox*   autoImportCbox;
    QLineEdit*   autoImportLine;
    ZPushButton* browseButton;
    QCheckBox*   themeCbox;
    QLabel*      themeLabel;
    QComboBox*   themeCombo;
    QComboBox*   letterOrderCombo;
    QLineEdit*   quizBackgroundColorLine;
    QCheckBox*   quizShowNumResponsesCbox;
    QCheckBox*   quizAutoCheckCbox;
    QLineEdit*   fontMainLine;
    QLineEdit*   fontWordListLine;
    // XXX: Reinstate this once it's known how to change the font of canvas
    // text items via QApplication::setFont
    //QLineEdit*   fontQuizLabelLine;
    QLineEdit*   fontDefinitionLine;
    QLineEdit*   fontWordInputLine;
    QCheckBox*   lengthSortCbox;
    QCheckBox*   showHookParentsCbox;
    QCheckBox*   showHooksCbox;
    QCheckBox*   showDefinitionCbox;

    QColor       quizBackgroundColor;

};

#endif // ZYZZYVA_SETTINGS_DIALOG_H
