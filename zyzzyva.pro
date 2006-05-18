#---------------------------------------------------------------------------
# zyzzyva.pro
#
# Build configuration file for Zyzzyva using qmake.
#
# Copyright 2005, 2006 Michael W Thelen <mike@pietdepsi.com>.
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

TEMPLATE = app
TARGET = zyzzyva
CONFIG += qt release thread warn_on assistant
QT += sql xml

ROOT = .
DESTDIR = $$ROOT/bin
MOC_DIR = build/moc
OBJECTS_DIR = build/obj
INCLUDEPATH += build/moc
DEPENDPATH += build/moc
RESOURCES = $$ROOT/zyzzyva.qrc
macx {
    RC_FILE = $$ROOT/images/zyzzyva.icns
}

unix {
    VERSION = $$system(cat $$ROOT/VERSION)
    DEFINES += ZYZZYVA_VERSION=\"$$VERSION\"
}
win32 {
    VERSION = $$system(type ..\..\VERSION)
    DEFINES += ZYZZYVA_VERSION=\"$$VERSION\"
}

# Source files
SOURCES = \
    $$ROOT/src/AboutDialog.cpp \
    $$ROOT/src/AnalyzeQuizDialog.cpp \
    $$ROOT/src/Auxil.cpp \
    $$ROOT/src/CreateDatabaseThread.cpp \
    $$ROOT/src/DefineForm.cpp \
    $$ROOT/src/DefinitionBox.cpp \
    $$ROOT/src/DefinitionDialog.cpp \
    $$ROOT/src/HelpDialog.cpp \
    $$ROOT/src/IntroForm.cpp \
    $$ROOT/src/JudgeForm.cpp \
    $$ROOT/src/JudgeDialog.cpp \
    $$ROOT/src/LetterBag.cpp \
    $$ROOT/src/MainSettings.cpp \
    $$ROOT/src/MainWindow.cpp \
    $$ROOT/src/NewQuizDialog.cpp \
    $$ROOT/src/QuizCanvas.cpp \
    $$ROOT/src/QuizEngine.cpp \
    $$ROOT/src/QuizForm.cpp \
    $$ROOT/src/QuizProgress.cpp \
    $$ROOT/src/QuizSpec.cpp \
    $$ROOT/src/QuizTimerSpec.cpp \
    $$ROOT/src/Rand.cpp \
    $$ROOT/src/SearchForm.cpp \
    $$ROOT/src/SearchCondition.cpp \
    $$ROOT/src/SearchConditionForm.cpp \
    $$ROOT/src/SearchSpec.cpp \
    $$ROOT/src/SearchSpecForm.cpp \
    $$ROOT/src/SettingsDialog.cpp \
    $$ROOT/src/WordEngine.cpp \
    $$ROOT/src/WordEntryDialog.cpp \
    $$ROOT/src/WordGraph.cpp \
    $$ROOT/src/WordListDialog.cpp \
    $$ROOT/src/WordTableDelegate.cpp \
    $$ROOT/src/WordTableModel.cpp \
    $$ROOT/src/WordTableView.cpp \
    $$ROOT/src/WordValidator.cpp \
    $$ROOT/src/WordVariationDialog.cpp \
    $$ROOT/src/ZApplication.cpp \
    $$ROOT/src/ZPushButton.cpp \
    $$ROOT/src/zyzzyva.cpp

# Header files that must be run through moc
HEADERS = \
    $$ROOT/src/AboutDialog.h \
    $$ROOT/src/ActionForm.h \
    $$ROOT/src/AnalyzeQuizDialog.h \
    $$ROOT/src/CreateDatabaseThread.h \
    $$ROOT/src/DefineForm.h \
    $$ROOT/src/DefinitionBox.h \
    $$ROOT/src/DefinitionDialog.h \
    $$ROOT/src/DefinitionLabel.h \
    $$ROOT/src/HelpDialog.h \
    $$ROOT/src/IntroForm.h \
    $$ROOT/src/JudgeForm.h \
    $$ROOT/src/JudgeDialog.h \
    $$ROOT/src/MainWindow.h \
    $$ROOT/src/NewQuizDialog.h \
    $$ROOT/src/QuizCanvas.h \
    $$ROOT/src/QuizForm.h \
    $$ROOT/src/QuizQuestionLabel.h \
    $$ROOT/src/SearchForm.h \
    $$ROOT/src/SearchConditionForm.h \
    $$ROOT/src/SearchSpecForm.h \
    $$ROOT/src/SettingsDialog.h \
    $$ROOT/src/WordEngine.h \
    $$ROOT/src/WordEntryDialog.h \
    $$ROOT/src/WordLineEdit.h \
    $$ROOT/src/WordListDialog.h \
    $$ROOT/src/WordTableDelegate.h \
    $$ROOT/src/WordTableModel.h \
    $$ROOT/src/WordTableView.h \
    $$ROOT/src/WordTextEdit.h \
    $$ROOT/src/WordValidator.h \
    $$ROOT/src/WordVariationDialog.h \
    $$ROOT/src/ZApplication.h \
    $$ROOT/src/ZPushButton.h
