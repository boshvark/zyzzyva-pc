//---------------------------------------------------------------------------
// SettingsDialog.h
//
// The settings dialog for the word study application.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
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

  public slots:
    void browseButtonClicked();

  private:
    QLineEdit* autoImportLine;
};

#endif // SETTINGS_DIALOG_H
