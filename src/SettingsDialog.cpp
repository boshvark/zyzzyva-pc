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
#include "Defs.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvgroupbox.h>

const QString SETTINGS_IMPORT_FILE = "/import";
const QString DIALOG_CAPTION = "Preferences";

using namespace Defs;

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
    QVBoxLayout* mainVlay = new QVBoxLayout (this, MARGIN, SPACING,
                                             "mainVlay");
    QLabel* label = new QLabel ("Preferences", this, "label");
    mainVlay->addWidget (label);

    QVGroupBox* autoImportGbox = new QVGroupBox (this, "autoImportGbox");
    autoImportGbox->setTitle ("Auto Import");
    mainVlay->addWidget (autoImportGbox);

    QWidget* autoImportWidget = new QWidget (autoImportGbox,
                                             "autoImportWidget");
    QVBoxLayout* autoImportVlay = new QVBoxLayout (autoImportWidget, MARGIN,
                                                   SPACING, "autoImportVlay");

    QHBoxLayout* autoImportHlay = new QHBoxLayout (SPACING, "autoImportHlay");
    autoImportVlay->addLayout (autoImportHlay);

    QLabel* autoImportLabel = new QLabel ("Import on Startup:",
                                          autoImportWidget,
                                          "autoImportLabel");
    autoImportHlay->addWidget (autoImportLabel);

    autoImportLine = new QLineEdit (autoImportWidget, "autoImportFile");
    autoImportHlay->addWidget (autoImportLine);

    QPushButton* browseButton = new QPushButton ("Browse...",
                                                 autoImportWidget,
                                                 "browseButton");
    connect (browseButton, SIGNAL (clicked()), SLOT (browseButtonClicked()));
    autoImportHlay->addWidget (browseButton);

    mainVlay->addStretch (1);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QPushButton* okButton = new QPushButton ("OK", this, "okButton");
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel", this,
                                                 "cancelButton");
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setCaption (DIALOG_CAPTION);
    setGeometry (0, 0, 320, 240);
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

void
SettingsDialog::browseButtonClicked()
{
}
