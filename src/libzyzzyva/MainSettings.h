//---------------------------------------------------------------------------
// MainSettings.h
//
// The main settings for the word study application.
//
// Copyright 2005, 2006, 2007, 2008 Michael W Thelen <mthelen@gmail.com>.
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

#ifndef ZYZZYVA_MAIN_SETTINGS_H
#define ZYZZYVA_MAIN_SETTINGS_H

#include <QColor>
#include <QPoint>
#include <QSize>
#include <QString>

class MainSettings
{
    public:
    static void readSettings();
    static void writeSettings();

    static QString getProgramVersion() {
        return instance->programVersion; }
    static void setProgramVersion(const QString& str) {
        instance->programVersion = str; }
    static QPoint getMainWindowPos() { return instance->mainWindowPos; }
    static void setMainWindowPos(QPoint p) { instance->mainWindowPos = p; }
    static QSize getMainWindowSize() { return instance->mainWindowSize; }
    static void setMainWindowSize(QSize s) { instance->mainWindowSize = s; }
    static bool getUseAutoImport() { return instance->useAutoImport; }
    static void setUseAutoImport(bool b) { instance->useAutoImport = b; }
    static QStringList getAutoImportLexicons() {
        return instance->autoImportLexicons; }
    static void setAutoImportLexicons(const QStringList& slist) {
        instance->autoImportLexicons = slist; }
    static QString getAutoImportFile() {
        return instance->autoImportFile; }
    static void setAutoImportFile(const QString& str) {
        instance->autoImportFile = str; }
    static QString getDefaultLexicon() {
        return instance->defaultLexicon; }
    static void setDefaultLexicon(const QString& s) {
        instance->defaultLexicon = s; }
    static bool getDisplayWelcome() { return instance->displayWelcome; }
    static void setDisplayWelcome(bool b) { instance->displayWelcome = b; }
    static QString getUserDataDir() { return instance->userDataDir; }
    static void setUserDataDir(const QString& str) {
        instance->userDataDir = str; }
    static bool getUseTileTheme() { return instance->useTileTheme; }
    static void setUseTileTheme(bool b) { instance->useTileTheme = b; }
    static QString getTileTheme() { return instance->tileTheme; }
    static void setTileTheme(const QString& str) {
        instance->tileTheme = str; }
    static QString getQuizLetterOrder() { return instance->quizLetterOrder; }
    static void setQuizLetterOrder(const QString& str) {
        instance->quizLetterOrder = str; }
    static QColor getQuizBackgroundColor() {
        return instance->quizBackgroundColor; }
    static void setQuizBackgroundColor(const QColor& color) {
        instance->quizBackgroundColor = color; }
    static bool getQuizUseFlashcardMode() {
        return instance->quizUseFlashcardMode; }
    static void setQuizUseFlashcardMode(bool b) {
        instance->quizUseFlashcardMode = b; }
    static bool getQuizShowNumResponses() {
        return instance->quizShowNumResponses; }
    static void setQuizShowNumResponses(bool b) {
        instance->quizShowNumResponses = b; }
    static bool getQuizShowQuestionStats() {
        return instance->quizShowQuestionStats; }
    static void setQuizShowQuestionStats(bool b) {
        instance->quizShowQuestionStats = b; }
    static bool getQuizAutoCheck() { return instance->quizAutoCheck; }
    static void setQuizAutoCheck(bool b) { instance->quizAutoCheck = b; }
    static bool getQuizAutoAdvance() { return instance->quizAutoAdvance; }
    static void setQuizAutoAdvance(bool b) { instance->quizAutoAdvance = b; }
    static bool getQuizAutoEndAfterIncorrect() {
        return instance->quizAutoEndAfterIncorrect; }
    static void setQuizAutoEndAfterIncorrect(bool b) {
        instance->quizAutoEndAfterIncorrect = b; }
    static bool getQuizMarkMissedAfterIncorrect() {
        return instance->quizMarkMissedAfterIncorrect; }
    static void setQuizMarkMissedAfterIncorrect(bool b) {
        instance->quizMarkMissedAfterIncorrect = b; }
    static bool getQuizCycleAnswers() {
        return instance->quizCycleAnswers; }
    static void setQuizCycleAnswers(bool b) {
        instance->quizCycleAnswers = b; }
    static bool getQuizRecordStats() {
        return instance->quizRecordStats; }
    static void setQuizRecordStats(bool b) {
        instance->quizRecordStats = b; }
    static QString getMainFont() { return instance->mainFont; }
    static void setMainFont(const QString& str) { instance->mainFont = str; }
    static QString getWordListFont() { return instance->wordListFont; }
    static void setWordListFont(const QString& str) {
        instance->wordListFont = str; }
    static QString getQuizLabelFont() { return instance->quizLabelFont; }
    static void setQuizLabelFont(const QString& str) {
        instance->quizLabelFont = str; }
    static QString getWordInputFont() { return instance->wordInputFont; }
    static void setWordInputFont(const QString& str) {
        instance->wordInputFont = str; }
    static QString getDefinitionFont() { return instance->definitionFont; }
    static void setDefinitionFont(const QString& str) {
        instance->definitionFont = str; }
    static bool getWordListSortByLength() {
        return instance->wordListSortByLength; }
    static void setWordListSortByLength(bool b) {
        instance->wordListSortByLength = b; }
    static bool getWordListSortByProbabilityOrder() {
        return instance->wordListSortByProbabilityOrder; }
    static void setWordListSortByProbabilityOrder(bool b) {
        instance->wordListSortByProbabilityOrder = b; }
    static bool getWordListGroupByAnagrams() {
        return instance->wordListGroupByAnagrams; }
    static void setWordListGroupByAnagrams(bool b) {
        instance->wordListGroupByAnagrams = b; }
    static bool getWordListShowProbabilityOrder() {
        return instance->wordListShowProbabilityOrder; }
    static void setWordListShowProbabilityOrder(bool b) {
        instance->wordListShowProbabilityOrder = b; }
    static bool getWordListShowHooks() {
        return instance->wordListShowHooks; }
    static void setWordListShowHooks(bool b) {
        instance->wordListShowHooks = b; }
    static bool getWordListShowHookParents() {
        return instance->wordListShowHookParents; }
    static void setWordListShowHookParents(bool b) {
        instance->wordListShowHookParents = b; }
    static bool getWordListShowDefinitions() {
        return instance->wordListShowDefinitions; }
    static void setWordListShowDefinitions(bool b) {
        instance->wordListShowDefinitions = b; }
    static QString getLetterDistribution() {
        return instance->letterDistribution; }
    static void setLetterDistribution(const QString& str) {
        instance->letterDistribution = str; }
    static bool getJudgeSaveLog() { return instance->judgeSaveLog; }
    static void setJudgeSaveLog(bool b) { instance->judgeSaveLog = b; }

    private:
    MainSettings() : useAutoImport(false), useTileTheme(false),
                     wordListSortByLength(false),
                     wordListSortByProbabilityOrder(false),
                     wordListShowHooks(false),
                     wordListShowHookParents(false),
                     wordListShowDefinitions(false), judgeSaveLog(true) { }
    ~MainSettings() { }

    // private and undefined
    MainSettings(const MainSettings&);
    MainSettings& operator=(const MainSettings&);

    static MainSettings* instance;

    QString programVersion;
    QPoint mainWindowPos;
    QSize mainWindowSize;
    bool useAutoImport;
    QStringList autoImportLexicons;
    QString autoImportFile;
    QString defaultLexicon;
    bool displayWelcome;
    QString userDataDir;
    bool useTileTheme;
    QString tileTheme;
    QString quizLetterOrder;
    QColor quizBackgroundColor;
    bool quizUseFlashcardMode;
    bool quizShowNumResponses;
    bool quizShowQuestionStats;
    bool quizAutoCheck;
    bool quizAutoAdvance;
    bool quizAutoEndAfterIncorrect;
    bool quizMarkMissedAfterIncorrect;
    bool quizCycleAnswers;
    bool quizRecordStats;
    QString mainFont;
    QString wordListFont;
    QString quizLabelFont;
    QString wordInputFont;
    QString definitionFont;
    bool wordListSortByLength;
    bool wordListSortByProbabilityOrder;
    bool wordListGroupByAnagrams;
    bool wordListShowProbabilityOrder;
    bool wordListShowHooks;
    bool wordListShowHookParents;
    bool wordListShowDefinitions;
    QString letterDistribution;
    bool judgeSaveLog;
};

#endif // ZYZZYVA_MAIN_SETTINGS_H
