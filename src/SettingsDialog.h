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

#include <qcheckbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qsettings.h>

class SettingsDialog : public QDialog
{
  Q_OBJECT
  public:
    SettingsDialog (QWidget* parent = 0, const char* name = 0,
                    bool modal = false, WFlags f = WType_TopLevel);
    ~SettingsDialog();

    void readSettings (const QSettings& settings);
    void writeSettings (QSettings& settings);
    QString getAutoImportFile() const;
    QString getFont() const;
    bool getLengthSort() const;

  public slots:
    void browseButtonClicked();
    void autoImportCboxToggled (bool on);
    void chooseFontButtonClicked();

  private:
    QCheckBox*   autoImportCbox;
    QLineEdit*   autoImportLine;
    QPushButton* browseButton;
    QLineEdit*   fontLine;
    QPushButton* chooseFontButton;
    QCheckBox*   lengthSortCbox;
};

#endif // SETTINGS_DIALOG_H
