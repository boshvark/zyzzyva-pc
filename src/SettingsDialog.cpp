//---------------------------------------------------------------------------
// SettingsDialog.cpp
//
// The settings dialog for the word study application.
//
// Copyright 2004 Michael W Thelen.  Permission to copy, use, modify, sell and
// distribute this software is granted provided this copyright notice appears in
// all copies. This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//---------------------------------------------------------------------------

#include "SettingsDialog.h"
#include <qlabel.h>
#include <qlayout.h>

const QString SETTINGS_IMPORT_FILE = "/import";

//---------------------------------------------------------------------------
// SettingsDialog
//
//! Constructor.
//
//! @param parent the parent widget
//! @param name the name of this widget
//! @param modal whether the dialog is modal
//! @param f widget flags
//---------------------------------------------------------------------------
SettingsDialog::SettingsDialog (QWidget* parent, const char* name,
                                bool modal, WFlags f)
    : QDialog (parent, name, modal, f)
{
    QVBoxLayout* mainVlay = new QVBoxLayout (this, 0, 0, "mainVlay");
    QLabel* label = new QLabel ("Hello world!", this, "label");
    mainVlay->addWidget (label);

    autoImportLine = new QLineEdit (this, "autoImportFile");
    mainVlay->addWidget (autoImportLine);
}

//---------------------------------------------------------------------------
// ~SettingsDialog
//
//! Destructor.  Save application settings.
//---------------------------------------------------------------------------
SettingsDialog::~SettingsDialog()
{
}

//---------------------------------------------------------------------------
// refresh
//
//! Refresh the interface state relative to the current settings.
//---------------------------------------------------------------------------
void
SettingsDialog::refresh()
{
}

//---------------------------------------------------------------------------
// readSettings
//
//! Read settings.
//---------------------------------------------------------------------------
void
SettingsDialog::readSettings (const QSettings& settings)
{
    bool ok = false;
    QString autoImportFile = settings.readEntry (SETTINGS_IMPORT_FILE,
                                                 QString::null, &ok);
    if (ok)
        autoImportLine->setText (autoImportFile);
}

//---------------------------------------------------------------------------
// writeSettings
//
//! Write settings.
//---------------------------------------------------------------------------
void
SettingsDialog::writeSettings (QSettings& settings)
{
    QString autoImportFile = autoImportLine->text();
    if (!autoImportFile.isEmpty())
        settings.writeEntry (SETTINGS_IMPORT_FILE, autoImportFile);
}
