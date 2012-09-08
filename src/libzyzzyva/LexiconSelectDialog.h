//---------------------------------------------------------------------------
// LexiconSelectDialog.h
//
// A dialog for selecting the lexicons to be loaded and used.
//
// Copyright 2008-2012 Boshvark Software, LLC.
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

#ifndef ZYZZYVA_LEXICON_SELECT_DIALOG_H
#define ZYZZYVA_LEXICON_SELECT_DIALOG_H

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtCore/QMap>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

class LexiconSelectDialog : public QDialog
{
    Q_OBJECT
    public:
    LexiconSelectDialog(QWidget* parent = 0, Qt::WFlags f = 0);
    ~LexiconSelectDialog();

    QStringList getImportLexicons() const;
    QString getDefaultLexicon() const;
    QString getCustomLexiconFile() const;
    void setImportLexicons(const QStringList& lexicons);
    void setDefaultLexicon(const QString& lexicon);
    void setCustomLexiconFile(const QString& filename);

    public slots:
    void customStateChanged(int state);
    void customBrowseButtonClicked();
    void defaultLexiconChanged();

    private:
    QMap<QString, QRadioButton*> lexiconRadioButtons;
    QMap<QString, QCheckBox*> lexiconCheckBoxes;
    QLineEdit* customLexiconLine;
    QPushButton* customLexiconButton;
};

#endif // ZYZZYVA_LEXICON_SELECT_DIALOG_H

