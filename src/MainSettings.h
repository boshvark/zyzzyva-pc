//---------------------------------------------------------------------------
// MainSettings.h
//
// The main settings for the word study application.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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

#ifndef MAIN_SETTINGS_H
#define MAIN_SETTINGS_H

#include <QColor>
#include <QString>

class MainSettings
{
    public:
    static void readSettings();
    static void writeSettings();

    static int getMainWindowX() { return instance->mainWindowX; }
    static void setMainWindowX (int i) { instance->mainWindowX = i; }
    static int getMainWindowY() { return instance->mainWindowY; }
    static void setMainWindowY (int i) { instance->mainWindowY = i; }
    static int getMainWindowWidth() { return instance->mainWindowWidth; }
    static void setMainWindowWidth (int i) { instance->mainWindowWidth = i; }
    static int getMainWindowHeight() { return instance->mainWindowHeight; }
    static void setMainWindowHeight (int i) {
        instance->mainWindowHeight = i; }
    static bool getUseAutoImport() { return instance->useAutoImport; }
    static void setUseAutoImport (bool b) { instance->useAutoImport = b; }
    static QString getAutoImportFile() { return instance->autoImportFile; }
    static void setAutoImportFile (const QString& str) {
        instance->autoImportFile = str; }
    static bool getUseTileTheme() { return instance->useTileTheme; }
    static void setUseTileTheme (bool b) { instance->useTileTheme = b; }
    static QString getTileTheme() { return instance->tileTheme; }
    static void setTileTheme (const QString& str) {
        instance->tileTheme = str; }
    static QString getQuizLetterOrder() { return instance->quizLetterOrder; }
    static void setQuizLetterOrder (const QString& str) {
        instance->quizLetterOrder = str; }
    static QColor getQuizBackgroundColor() {
        return instance->quizBackgroundColor; }
    static void setQuizBackgroundColor (const QColor& color) {
        instance->quizBackgroundColor = color; }
    static QString getMainFont() { return instance->mainFont; }
    static void setMainFont (const QString& str) { instance->mainFont = str; }
    static QString getWordListFont() { return instance->wordListFont; }
    static void setWordListFont (const QString& str) {
        instance->wordListFont = str; }
    static QString getWordInputFont() { return instance->wordInputFont; }
    static void setWordInputFont (const QString& str) {
        instance->wordInputFont = str; }
    static QString getDefinitionFont() { return instance->definitionFont; }
    static void setDefinitionFont (const QString& str) {
        instance->definitionFont = str; }
    static bool getWordListSortByLength() {
        return instance->wordListSortByLength; }
    static void setWordListSortByLength (bool b) {
        instance->wordListSortByLength = b; }
    static bool getWordListShowHooks() {
        return instance->wordListShowHooks; }
    static void setWordListShowHooks (bool b) {
        instance->wordListShowHooks = b; }
    static bool getWordListShowHookParents() {
        return instance->wordListShowHookParents; }
    static void setWordListShowHookParents (bool b) {
        instance->wordListShowHookParents = b; }
    static bool getWordListShowDefinitions() {
        return instance->wordListShowDefinitions; }
    static void setWordListShowDefinitions (bool b) {
        instance->wordListShowDefinitions = b; }

    private:
    MainSettings() : mainWindowX (0), mainWindowY (0), mainWindowWidth (0),
                     mainWindowHeight (0), useAutoImport (false), useTileTheme
                         (false), wordListSortByLength (false),
                     wordListShowHooks (false), wordListShowHookParents
                         (false), wordListShowDefinitions (false) { };
    ~MainSettings() { };

    // private and undefined
    MainSettings (const MainSettings&);
    MainSettings& operator= (const MainSettings&);

    static MainSettings* instance;

    int mainWindowX;
    int mainWindowY;
    int mainWindowWidth;
    int mainWindowHeight;
    bool useAutoImport;
    QString autoImportFile;
    bool useTileTheme;
    QString tileTheme;
    QString quizLetterOrder;
    QColor quizBackgroundColor;
    QString mainFont;
    QString wordListFont;
    QString quizLabelFont;
    QString wordInputFont;
    QString definitionFont;
    bool wordListSortByLength;
    bool wordListShowHooks;
    bool wordListShowHookParents;
    bool wordListShowDefinitions;
};

#endif // MAIN_SETTINGS_H
