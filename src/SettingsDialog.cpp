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
// readSettings
//
//! Read settings.
//---------------------------------------------------------------------------
void
SettingsDialog::readSettings (const QSettings& settings)
{
}

//---------------------------------------------------------------------------
// writeSettings
//
//! Write settings.
//---------------------------------------------------------------------------
void
SettingsDialog::writeSettings (const QSettings& settings)
{
}
