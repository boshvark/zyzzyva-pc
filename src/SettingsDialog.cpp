//---------------------------------------------------------------------------
// SettingsDialog.cpp
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

#include "SettingsDialog.h"
#include "Auxil.h"
#include "Defs.h"
#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qfont.h>
#include <qfontdialog.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qvgroupbox.h>

const QString SETTINGS_IMPORT = "/autoimport";
const QString SETTINGS_IMPORT_FILE = "/autoimport_file";
const QString SETTINGS_FONT = "/font";
const QString SETTINGS_SORT_BY_LENGTH = "/wordlist_sort_by_length";
const QString SETTINGS_USE_TILE_THEME = "/use_tile_theme";
const QString SETTINGS_TILE_THEME = "/tile_theme";
const QString DEFAULT_TILE_THEME = "tan-with-border";
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
    Q_CHECK_PTR (mainVlay);

    QVGroupBox* autoImportGbox = new QVGroupBox (this, "autoImportGbox");
    Q_CHECK_PTR (autoImportGbox);
    autoImportGbox->setTitle ("Auto Import");
    mainVlay->addWidget (autoImportGbox);

    QWidget* autoImportWidget = new QWidget (autoImportGbox,
                                             "autoImportWidget");
    Q_CHECK_PTR (autoImportWidget);
    QVBoxLayout* autoImportVlay = new QVBoxLayout (autoImportWidget, MARGIN,
                                                   SPACING, "autoImportVlay");
    Q_CHECK_PTR (autoImportVlay);

    autoImportCbox = new QCheckBox ("Automatically import a word list "
                                    "on startup", autoImportWidget,
                                    "autoImportCbox");
    Q_CHECK_PTR (autoImportCbox);
    connect (autoImportCbox, SIGNAL (toggled (bool)),
             SLOT (autoImportCboxToggled (bool)));
    autoImportVlay->addWidget (autoImportCbox);

    QHBoxLayout* autoImportHlay = new QHBoxLayout (SPACING, "autoImportHlay");
    Q_CHECK_PTR (autoImportHlay);
    autoImportVlay->addLayout (autoImportHlay);

    autoImportLine = new QLineEdit (autoImportWidget, "autoImportFile");
    Q_CHECK_PTR (autoImportLine);
    autoImportHlay->addWidget (autoImportLine);

    browseButton = new QPushButton ("Browse...", autoImportWidget,
                                    "browseButton");
    Q_CHECK_PTR (browseButton);
    connect (browseButton, SIGNAL (clicked()), SLOT (browseButtonClicked()));
    autoImportHlay->addWidget (browseButton);

    QVGroupBox* themeGbox = new QVGroupBox (this, "themeGbox");
    Q_CHECK_PTR (themeGbox);
    themeGbox->setTitle ("Tile Theme");
    mainVlay->addWidget (themeGbox);

    QWidget* themeWidget = new QWidget (themeGbox, "themeWidget");
    Q_CHECK_PTR (themeWidget);
    QVBoxLayout* themeVlay = new QVBoxLayout (themeWidget, MARGIN, SPACING,
                                              "themeVlay");
    Q_CHECK_PTR (themeVlay);

    themeCbox = new QCheckBox ("Use tile images in Quiz mode", themeWidget,
                               "themeCbox");
    Q_CHECK_PTR (themeCbox);
    connect (themeCbox, SIGNAL (toggled (bool)),
             SLOT (themeCboxToggled (bool)));
    themeVlay->addWidget (themeCbox);

    QHBoxLayout* themeHlay = new QHBoxLayout (SPACING, "themeHlay");
    Q_CHECK_PTR (themeHlay);
    themeVlay->addLayout (themeHlay);

    themeLabel = new QLabel ("Theme:", themeWidget, "themeLabel");
    Q_CHECK_PTR (themeLabel);
    themeLabel->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    themeHlay->addWidget (themeLabel);

    themeCombo = new QComboBox (themeWidget, "themeCombo");
    Q_CHECK_PTR (themeCombo);
    themeHlay->addWidget (themeCombo);

    QVGroupBox* fontGbox = new QVGroupBox (this, "fontGbox");
    Q_CHECK_PTR (fontGbox);
    fontGbox->setTitle ("Font");
    mainVlay->addWidget (fontGbox);

    QWidget* fontWidget = new QWidget (fontGbox, "fontWidget");
    Q_CHECK_PTR (fontWidget);

    QHBoxLayout* fontHlay = new QHBoxLayout (fontWidget, MARGIN, SPACING,
                                             "fontHlay");
    Q_CHECK_PTR (fontHlay);

    fontLine = new QLineEdit (fontWidget, "fontLine");
    Q_CHECK_PTR (fontLine);
    fontLine->setReadOnly (true);
    fontLine->setText (this->font().toString());
    fontHlay->addWidget (fontLine);

    chooseFontButton = new QPushButton ("Choose...", fontWidget,
                                        "chooseFontButton");
    Q_CHECK_PTR (chooseFontButton);
    connect (chooseFontButton, SIGNAL (clicked()),
             SLOT (chooseFontButtonClicked()));
    fontHlay->addWidget (chooseFontButton);

    mainVlay->addStretch (1);

    QVGroupBox* miscGbox = new QVGroupBox (this, "miscGbox");
    Q_CHECK_PTR (miscGbox);
    miscGbox->setTitle ("Miscellaneous");
    mainVlay->addWidget (miscGbox);

    QWidget* miscWidget = new QWidget (miscGbox, "miscWidget");
    Q_CHECK_PTR (miscWidget);

    QVBoxLayout* miscVlay = new QVBoxLayout (miscWidget, MARGIN, SPACING,
                                             "miscVlay");
    Q_CHECK_PTR (miscVlay);

    lengthSortCbox = new QCheckBox ("Sort word lists by word length",
                                    miscWidget, "lengthSortCbox");
    Q_CHECK_PTR (lengthSortCbox);
    miscVlay->addWidget (lengthSortCbox);

    QHBoxLayout* buttonHlay = new QHBoxLayout (SPACING, "buttonHlay");
    Q_CHECK_PTR (buttonHlay);
    mainVlay->addLayout (buttonHlay);

    buttonHlay->addStretch (1);

    QPushButton* okButton = new QPushButton ("OK", this, "okButton");
    Q_CHECK_PTR (okButton);
    okButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault (true);
    connect (okButton, SIGNAL (clicked()), SLOT (accept()));
    buttonHlay->addWidget (okButton);

    QPushButton* cancelButton = new QPushButton ("Cancel", this,
                                                 "cancelButton");
    Q_CHECK_PTR (cancelButton);
    cancelButton->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (cancelButton, SIGNAL (clicked()), SLOT (reject()));
    buttonHlay->addWidget (cancelButton);

    setCaption (DIALOG_CAPTION);
    fillThemeCombo();
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

    bool autoImport = settings.readBoolEntry (SETTINGS_IMPORT, false);
    autoImportCbox->setChecked (autoImport);
    autoImportCboxToggled (autoImport);

    QString autoImportFile = settings.readEntry (SETTINGS_IMPORT_FILE,
                                                 QString::null, &ok);
    if (ok)
        autoImportLine->setText (autoImportFile);

    fillThemeCombo();
    bool useTileTheme = settings.readBoolEntry (SETTINGS_USE_TILE_THEME,
                                                true);
    themeCbox->setChecked (useTileTheme);
    if (themeCbox->isEnabled()) {
        QString tileTheme = settings.readEntry (SETTINGS_TILE_THEME,
                                                DEFAULT_TILE_THEME, &ok);
        QString themeStr;
        for (int i = 0; i < themeCombo->count(); ++i) {
            themeStr = themeCombo->text (i);
            if (themeStr == tileTheme) {
                themeCombo->setCurrentItem (i);
                break;
            }
            else if (themeStr == DEFAULT_TILE_THEME)
                themeCombo->setCurrentItem (i);
        }
        themeCboxToggled (useTileTheme);
    }

    QString fontStr = settings.readEntry (SETTINGS_FONT, QString::null, &ok);
    if (ok)
        fontLine->setText (fontStr);

    bool lengthSort = settings.readBoolEntry (SETTINGS_SORT_BY_LENGTH, false);
    lengthSortCbox->setChecked (lengthSort);
}

//---------------------------------------------------------------------------
// writeSettings
//
//! Write settings.
//---------------------------------------------------------------------------
void
SettingsDialog::writeSettings (QSettings& settings)
{
    settings.writeEntry (SETTINGS_IMPORT, autoImportCbox->isChecked());
    settings.writeEntry (SETTINGS_IMPORT_FILE, autoImportLine->text());
    settings.writeEntry (SETTINGS_USE_TILE_THEME, themeCbox->isChecked());
    settings.writeEntry (SETTINGS_TILE_THEME, themeCombo->currentText());
    settings.writeEntry (SETTINGS_FONT, fontLine->text());
    settings.writeEntry (SETTINGS_SORT_BY_LENGTH,
                         lengthSortCbox->isChecked());
}

//---------------------------------------------------------------------------
// getAutoImportFile
//
//! Return the file to be imported on startup, if auto-importing is
//! enabled.
//
//! @return the auto-import filename
//---------------------------------------------------------------------------
QString
SettingsDialog::getAutoImportFile() const
{
    return (autoImportCbox->isChecked() ? autoImportLine->text()
            : QString::null);
}

//---------------------------------------------------------------------------
// getFont
//
//! Return the font setting.
//
//! @return the name of the preferred font
//---------------------------------------------------------------------------
QString
SettingsDialog::getFont() const
{
    return fontLine->text();
}

//---------------------------------------------------------------------------
// getTileTheme
//
//! Return the tile theme setting.
//
//! @return the name of the tile theme
//---------------------------------------------------------------------------
QString
SettingsDialog::getTileTheme() const
{
    return ((themeCbox->isEnabled() && themeCbox->isChecked()) ?
            themeCombo->currentText() : QString::null);
}

//---------------------------------------------------------------------------
// getSortByLength
//
//! Return the "sort by length" setting.
//
//! @return true if "sort by length" is preferred, false otherwise
//---------------------------------------------------------------------------
bool
SettingsDialog::getSortByLength() const
{
    return lengthSortCbox->isChecked();
}

//---------------------------------------------------------------------------
// browseButtonClicked
//
//! Slot called when the Browse button is clicked.  Create a file chooser
//! dialog and place the name of the chosen file in the auto-import line
//! edit.
//---------------------------------------------------------------------------
void
SettingsDialog::browseButtonClicked()
{
    QString file = QFileDialog::getOpenFileName (QDir::current().path(),
                                                 "All Files (*.*)", this,
                                                 "fileDialog",
                                                 IMPORT_CHOOSER_TITLE);
    if (!file.isNull())
        autoImportLine->setText (file);
}

//---------------------------------------------------------------------------
// autoImportCboxToggled
//
//! Slot called when the Auto Import check box is toggled.  Enable or
//! disable the auto-import file edit area.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::autoImportCboxToggled (bool on)
{
    autoImportLine->setEnabled (on);
    browseButton->setEnabled (on);
}

//---------------------------------------------------------------------------
// themeCboxToggled
//
//! Slot called when the Tile Theme check box is toggled.  Enable or disable
//! the tile theme selection area.
//
//! @param on true if the check box is on, false if it is off
//---------------------------------------------------------------------------
void
SettingsDialog::themeCboxToggled (bool on)
{
    themeLabel->setEnabled (on);
    themeCombo->setEnabled (on);
}

//---------------------------------------------------------------------------
// chooseFontButtonClicked
//
//! Slot called when the Choose Font button is clicked.  Create a font chooser
//! dialog and place the name of the chosen font in the font line edit.
//---------------------------------------------------------------------------
void
SettingsDialog::chooseFontButtonClicked()
{
    bool ok = false;
    QFont font = QFontDialog::getFont (&ok, this->font(), this);
    if (ok)
        fontLine->setText (font.toString());
}

//---------------------------------------------------------------------------
// fillThemeCombo
//
//! Fill the Tile Theme combo box with the list of available tile themes.
//---------------------------------------------------------------------------
void
SettingsDialog::fillThemeCombo()
{
    themeCombo->clear();

    QDir themeDir (Auxil::getTilesDir());
    QStringList themes = themeDir.entryList (QDir::Dirs, QDir::Name).grep
        (QRegExp ("^[^\\.]"));

    bool enableThemes = (themes.count() > 0);
    themeCbox->setEnabled (enableThemes);
    if (enableThemes) {
        themeCombo->insertStringList (themes);
    }
    else {
        themeLabel->setEnabled (false);
        themeCombo->setEnabled (false);
    }
}
