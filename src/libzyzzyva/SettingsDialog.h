//---------------------------------------------------------------------------
// SettingsDialog.h
//
// The settings dialog for the word study application.
//
// Copyright 2004-2012 Boshvark Software, LLC.
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
#include <QList>
#include <QListWidget>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>

class LexiconStyle;
class ZPushButton;

class SettingsDialog : public QDialog
{
    Q_OBJECT
    public:
    SettingsDialog(QWidget* parent = 0, Qt::WFlags f = 0);
    ~SettingsDialog();

    void readSettings();
    void refreshSettings();
    void writeSettings();

    public slots:
    void navTextChanged(const QString& text);
    void restoreDefaultsClicked();
    void autoImportCboxToggled(bool on);
    void autoImportLexiconActivated(const QString& text);
    void chooseLexiconsClicked();
    void userDataDirBrowseButtonClicked();
    void userDataDirChanged(const QString& text);
    void themeCboxToggled(bool on);
    void autoCheckCboxToggled(bool on);
    void judgeSaveLogCboxToggled(bool on);
    void showHookParentsCboxToggled(bool on);
    void chooseFontButtonClicked(int button);
    void chooseQuizBackgroundColorButtonClicked();
    void timeoutDisableInputCboxToggled(bool on);
    void lexiconStyleCboxToggled(bool on);
    void chooseLexiconStyleClicked();

    private:
    void setImportLexicons(const QStringList& lexicons, const QString&
                           defaultLexicon);
    void getImportLexicons(QStringList& lexicons, QString& defaultLexicon)
        const;
    void setLexiconStyles(const QList<LexiconStyle>& styleStr);
    QList<LexiconStyle> getLexiconStyles() const;
    void fillThemeCombo();
    void updateJudgeLogDirLabel();

    private:
    QListWidget*    navList;
    QStackedWidget* navStack;

    QWidget*     generalPrefWidget;
    QWidget*     quizPrefWidget;
    QWidget*     probabilityPrefWidget;
    QWidget*     cardboxPrefWidget;
    QWidget*     judgePrefWidget;
    QWidget*     fontPrefWidget;
    QWidget*     wordListPrefWidget;

    QCheckBox*   autoImportCbox;
    QLabel*      autoImportLabel;
    ZPushButton* autoImportButton;
    QWidget*     autoImportCustomWidget;
    QLineEdit*   autoImportCustomLine;
    QCheckBox*   displayWelcomeCbox;
    QLineEdit*   userDataDirLine;
    QCheckBox*   userDataDirMoveCbox;
    QCheckBox*   themeCbox;
    QLabel*      themeLabel;
    QComboBox*   themeCombo;
    QComboBox*   letterOrderCombo;
    QLineEdit*   quizBackgroundColorLine;
    QCheckBox*   quizUseFlashcardModeCbox;
    QCheckBox*   quizShowNumResponsesCbox;
    QCheckBox*   quizShowQuestionStatsCbox;
    QCheckBox*   quizRequireLexiconSymbolsCbox;
    QCheckBox*   quizAutoCheckCbox;
    QCheckBox*   quizAutoAdvanceCbox;
    QCheckBox*   quizAutoEndAfterIncorrectCbox;
    QCheckBox*   quizMarkMissedAfterIncorrectCbox;
    QCheckBox*   quizMarkMissedAfterTimerCbox;
    QCheckBox*   quizCycleAnswersCbox;
    QCheckBox*   quizTimeoutDisableInputCbox;
    QSpinBox*    quizTimeoutDisableInputSbox;
    QSpinBox*    probBlanksSbox;
    QList<QSpinBox*> cardboxScheduleSboxList;
    QList<QSpinBox*> cardboxWindowSboxList;
    QCheckBox*   judgeSaveLogCbox;
    QWidget*     judgeLogDirWidget;
    QLabel*      judgeLogDirLabel;
    QLineEdit*   fontMainLine;
    QLineEdit*   fontWordListLine;
    QLineEdit*   fontQuizLabelLine;
    QLineEdit*   fontDefinitionLine;
    QLineEdit*   fontWordInputLine;
    QCheckBox*   lengthSortCbox;
    QCheckBox*   anagramGroupCbox;
    QCheckBox*   showProbabilityOrderCbox;
    QCheckBox*   showPlayabilityOrderCbox;
    QCheckBox*   showHookParentsCbox;
    QCheckBox*   useHookParentHyphensCbox;
    QCheckBox*   showHooksCbox;
    QCheckBox*   showDefinitionCbox;
    QCheckBox*   lowerCaseWildcardsCbox;
    QCheckBox*   lexiconStyleCbox;
    QLabel*      lexiconStyleLabel;
    ZPushButton* lexiconStyleButton;

    QColor       quizBackgroundColor;
    QString      origUserDataDir;

};

#endif // ZYZZYVA_SETTINGS_DIALOG_H
