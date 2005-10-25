//---------------------------------------------------------------------------
// SettingsDialog.h
//
// The settings dialog for the word study application.
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

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>

class SettingsDialog : public QDialog
{
    Q_OBJECT
    public:
    SettingsDialog (QWidget* parent = 0, Qt::WFlags f = Qt::WType_TopLevel);
    ~SettingsDialog();

    void readSettings();
    void writeSettings();

    public slots:
    void browseButtonClicked();
    void autoImportCboxToggled (bool on);
    void themeCboxToggled (bool on);
    void chooseFontButtonClicked (int button);

    private:
    void fillThemeCombo();

    private:
    QCheckBox*   autoImportCbox;
    QLineEdit*   autoImportLine;
    QPushButton* browseButton;
    QCheckBox*   themeCbox;
    QLabel*      themeLabel;
    QComboBox*   themeCombo;
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
};

#endif // SETTINGS_DIALOG_H
