//---------------------------------------------------------------------------
// MainSettings.h
//
// The main settings for the word study application.
//
// Copyright 2005-2012 Boshvark Software, LLC.
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

#include "LexiconStyle.h"
#include <QColor>
#include <QList>
#include <QPoint>
#include <QSize>
#include <QString>

class MainSettings
{
    public:
    static const QString GENERAL_PREFS_GROUP;
    static const QString SEARCH_PREFS_GROUP;
    static const QString QUIZ_PREFS_GROUP;
    static const QString PROBABILITY_PREFS_GROUP;
    static const QString CARDBOX_PREFS_GROUP;
    static const QString JUDGE_PREFS_GROUP;
    static const QString FONT_PREFS_GROUP;
    static const QString WORD_LIST_PREFS_GROUP;

    public:
    static void readSettings();
    static void writeSettings();
    static void restoreDefaults(const QString& group);

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
    static bool getSearchSelectInput() { return instance->searchSelectInput; }
    static void setSearchSelectInput(bool b) {
        instance->searchSelectInput = b; }
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
    static bool getQuizRequireLexiconSymbols() {
        return instance->quizRequireLexiconSymbols; }
    static void setQuizRequireLexiconSymbols(bool b) {
        instance->quizRequireLexiconSymbols = b; }
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
    static bool getQuizMarkMissedAfterTimerExpires() {
        return instance->quizMarkMissedAfterTimerExpires; }
    static void setQuizMarkMissedAfterTimerExpires(bool b) {
        instance->quizMarkMissedAfterTimerExpires = b; }
    static bool getQuizCycleAnswers() {
        return instance->quizCycleAnswers; }
    static void setQuizCycleAnswers(bool b) {
        instance->quizCycleAnswers = b; }
    static bool getQuizTimeoutDisableInput() {
        return instance->quizTimeoutDisableInput; }
    static void setQuizTimeoutDisableInput(bool b) {
        instance->quizTimeoutDisableInput = b; }
    static int getQuizTimeoutDisableInputMillisecs() {
        return instance->quizTimeoutDisableInputMillisecs; }
    static void setQuizTimeoutDisableInputMillisecs(int i) {
        instance->quizTimeoutDisableInputMillisecs = i; }
    static bool getQuizRecordStats() {
        return instance->quizRecordStats; }
    static void setQuizRecordStats(bool b) {
        instance->quizRecordStats = b; }
    static int getProbabilityNumBlanks() {
        return instance->probabilityNumBlanks; }
    static void setProbabilityNumBlanks(int i) {
        instance->probabilityNumBlanks = i; }
    static QList<int> getCardboxScheduleList() {
        return instance->cardboxScheduleList; }
    static void setCardboxScheduleList(const QList<int>& slist) {
        instance->cardboxScheduleList = slist; }
    static QList<int> getCardboxWindowList() {
        return instance->cardboxWindowList; }
    static void setCardboxWindowList(const QList<int>& slist) {
        instance->cardboxWindowList = slist; }
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
    static bool getWordListSortByReverseLength() {
        return instance->wordListSortByReverseLength; }
    static void setWordListSortByReverseLength(bool b) {
        instance->wordListSortByReverseLength = b; }
    static bool getWordListSortByProbabilityOrder() {
        return instance->wordListSortByProbabilityOrder; }
    static void setWordListSortByProbabilityOrder(bool b) {
        instance->wordListSortByProbabilityOrder = b; }
    static bool getWordListSortByPlayabilityOrder() {
        return instance->wordListSortByPlayabilityOrder; }
    static void setWordListSortByPlayabilityOrder(bool b) {
        instance->wordListSortByPlayabilityOrder = b; }
    static bool getWordListGroupByAnagrams() {
        return instance->wordListGroupByAnagrams; }
    static void setWordListGroupByAnagrams(bool b) {
        instance->wordListGroupByAnagrams = b; }
    static bool getWordListShowProbabilityOrder() {
        return instance->wordListShowProbabilityOrder; }
    static void setWordListShowProbabilityOrder(bool b) {
        instance->wordListShowProbabilityOrder = b; }
    static bool getWordListShowPlayabilityOrder() {
        return instance->wordListShowPlayabilityOrder; }
    static void setWordListShowPlayabilityOrder(bool b) {
        instance->wordListShowPlayabilityOrder = b; }
    static bool getWordListShowHooks() {
        return instance->wordListShowHooks; }
    static void setWordListShowHooks(bool b) {
        instance->wordListShowHooks = b; }
    static bool getWordListShowHookParents() {
        return instance->wordListShowHookParents; }
    static void setWordListShowHookParents(bool b) {
        instance->wordListShowHookParents = b; }
    static bool getWordListUseHookParentHyphens() {
        return instance->wordListUseHookParentHyphens; }
    static void setWordListUseHookParentHyphens(bool b) {
        instance->wordListUseHookParentHyphens = b; }
    static bool getWordListShowDefinitions() {
        return instance->wordListShowDefinitions; }
    static void setWordListShowDefinitions(bool b) {
        instance->wordListShowDefinitions = b; }
    static bool getWordListLowerCaseWildcards() {
        return instance->wordListLowerCaseWildcards; }
    static void setWordListLowerCaseWildcards(bool b) {
        instance->wordListLowerCaseWildcards = b; }
    static bool getWordListUseLexiconStyles() {
        return instance->wordListUseLexiconStyles; }
    static void setWordListUseLexiconStyles(bool b) {
        instance->wordListUseLexiconStyles = b; }
    static QList<LexiconStyle> getWordListLexiconStyles() {
        return instance->wordListLexiconStyles; }
    static void setWordListLexiconStyles(const QList<LexiconStyle>& slist) {
        instance->wordListLexiconStyles = slist; }
    static QString getLetterDistribution() {
        return instance->letterDistribution; }
    static void setLetterDistribution(const QString& str) {
        instance->letterDistribution = str; }
    static bool getJudgeSaveLog() { return instance->judgeSaveLog; }
    static void setJudgeSaveLog(bool b) { instance->judgeSaveLog = b; }

    private:
    MainSettings() : useAutoImport(false), useTileTheme(false),
                     wordListSortByLength(false),
                     wordListSortByReverseLength(false),
                     wordListSortByProbabilityOrder(false),
                     wordListSortByPlayabilityOrder(false),
                     wordListShowHooks(false),
                     wordListShowHookParents(false),
                     wordListUseHookParentHyphens(false),
                     wordListShowDefinitions(false),
                     wordListUseLexiconStyles(false), judgeSaveLog(true) { }
    ~MainSettings() { }

    // private and undefined
    MainSettings(const MainSettings&);
    MainSettings& operator=(const MainSettings&);

    void setCardboxScheduleList(const QString& str);
    void setCardboxWindowList(const QString& str);
    void setWordListLexiconStyles(const QString& str);

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
    bool searchSelectInput;
    QString quizLetterOrder;
    QColor quizBackgroundColor;
    bool quizUseFlashcardMode;
    bool quizShowNumResponses;
    bool quizShowQuestionStats;
    bool quizRequireLexiconSymbols;
    bool quizAutoCheck;
    bool quizAutoAdvance;
    bool quizAutoEndAfterIncorrect;
    bool quizMarkMissedAfterIncorrect;
    bool quizMarkMissedAfterTimerExpires;
    bool quizCycleAnswers;
    bool quizTimeoutDisableInput;
    int quizTimeoutDisableInputMillisecs;
    bool quizRecordStats;
    int probabilityNumBlanks;
    QList<int> cardboxScheduleList;
    QList<int> cardboxWindowList;
    QString mainFont;
    QString wordListFont;
    QString quizLabelFont;
    QString wordInputFont;
    QString definitionFont;
    bool wordListSortByLength;
    bool wordListSortByReverseLength;
    bool wordListSortByProbabilityOrder;
    bool wordListSortByPlayabilityOrder;
    bool wordListGroupByAnagrams;
    bool wordListShowProbabilityOrder;
    bool wordListShowPlayabilityOrder;
    bool wordListShowHooks;
    bool wordListShowHookParents;
    bool wordListUseHookParentHyphens;
    bool wordListShowDefinitions;
    bool wordListLowerCaseWildcards;
    bool wordListUseLexiconStyles;
    QList<LexiconStyle> wordListLexiconStyles;
    QString letterDistribution;
    bool judgeSaveLog;
};

#endif // ZYZZYVA_MAIN_SETTINGS_H
