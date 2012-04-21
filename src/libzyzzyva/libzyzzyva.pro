#---------------------------------------------------------------------------
# zyzzyva.pro
#
# Build configuration file for Zyzzyva using qmake.
#
# Copyright 2005-2012 Boshvark Software, LLC.
#
# This file is part of Zyzzyva.
#
# Zyzzyva is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Zyzzyva is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#---------------------------------------------------------------------------

TEMPLATE = lib
TARGET = zyzzyva
CONFIG += qt thread warn_on assistant
QT += sql xml network

ROOT = ../..
DESTDIR = $$ROOT/bin

include($$ROOT/zyzzyva.pri)

unix {
    VERSION = $$system(cat $$ROOT/VERSION)
}
win32 {
    VERSION = $$system(type ..\..\VERSION)
}

# Source files
SOURCES = \
    AboutDialog.cpp \
    AnalyzeQuizDialog.cpp \
    Auxil.cpp \
    CardboxAddDialog.cpp \
    CardboxForm.cpp \
    CardboxRemoveDialog.cpp \
    CardboxRescheduleDialog.cpp \
    CreateDatabaseThread.cpp \
    DatabaseRebuildDialog.cpp \
    DefineForm.cpp \
    DefinitionBox.cpp \
    DefinitionDialog.cpp \
    IntroForm.cpp \
    IscConnectionThread.cpp \
    IscConverter.cpp \
    JudgeDialog.cpp \
    JudgeSelectDialog.cpp \
    LetterBag.cpp \
    LexiconSelectDialog.cpp \
    LexiconSelectWidget.cpp \
    LexiconStyleDialog.cpp \
    LexiconStyleWidget.cpp \
    MainSettings.cpp \
    MainWindow.cpp \
    NewQuizDialog.cpp \
    QuizCanvas.cpp \
    QuizDatabase.cpp \
    QuizEngine.cpp \
    QuizForm.cpp \
    QuizProgress.cpp \
    QuizQuestion.cpp \
    QuizSpec.cpp \
    QuizTimerSpec.cpp \
    Rand.cpp \
    SearchForm.cpp \
    SearchCondition.cpp \
    SearchConditionForm.cpp \
    SearchSpec.cpp \
    SearchSpecForm.cpp \
    SettingsDialog.cpp \
    WordEngine.cpp \
    WordEntryDialog.cpp \
    WordGraph.cpp \
    WordListDialog.cpp \
    WordListSaveDialog.cpp \
    WordTableModel.cpp \
    WordTableView.cpp \
    WordValidator.cpp \
    WordVariationDialog.cpp \
    ZApplication.cpp \
    ZPushButton.cpp

# Header files that must be run through moc
HEADERS = \
    AboutDialog.h \
    ActionForm.h \
    AnalyzeQuizDialog.h \
    CardboxAddDialog.h \
    CardboxForm.h \
    CardboxRemoveDialog.h \
    CardboxRescheduleDialog.h \
    CreateDatabaseThread.h \
    DatabaseRebuildDialog.h \
    DefineForm.h \
    DefinitionBox.h \
    DefinitionDialog.h \
    DefinitionLabel.h \
    DefinitionTextEdit.h \
    IntroForm.h \
    IscConnectionThread.h \
    JudgeDialog.h \
    JudgeSelectDialog.h \
    LexiconSelectDialog.h \
    LexiconSelectWidget.h \
    LexiconStyleDialog.h \
    LexiconStyleWidget.h \
    MainWindow.h \
    NewQuizDialog.h \
    QuizCanvas.h \
    QuizForm.h \
    QuizProgress.h \
    QuizQuestionLabel.h \
    QuizQuestion.h \
    SearchForm.h \
    SearchConditionForm.h \
    SearchSpecForm.h \
    SettingsDialog.h \
    WordEngine.h \
    WordEntryDialog.h \
    WordLineEdit.h \
    WordListDialog.h \
    WordListSaveDialog.h \
    WordTableModel.h \
    WordTableView.h \
    WordTextEdit.h \
    WordValidator.h \
    WordVariationDialog.h \
    ZApplication.h \
    ZPushButton.h
