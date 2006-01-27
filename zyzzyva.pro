#---------------------------------------------------------------------------
# zyzzyva.pro
#
# Build configuration file for Zyzzyva using qmake.
#
# Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
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
QT += xml

MOC_DIR = build/moc
OBJECTS_DIR = build/obj
INCLUDEPATH += build/moc
DEPENDPATH += build/moc
RESOURCES = zyzzyva.qrc

# Source files
SOURCES = \
    src/AboutDialog.cpp \
    src/AnalyzeQuizDialog.cpp \
    src/Auxil.cpp \
    src/DefineForm.cpp \
    src/DefinitionBox.cpp \
    src/DefinitionDialog.cpp \
    src/HelpDialog.cpp \
    src/JudgeForm.cpp \
    src/JudgeDialog.cpp \
    src/main.cpp \
    src/MainSettings.cpp \
    src/MainWindow.cpp \
    src/NewQuizDialog.cpp \
    src/QuizCanvas.cpp \
    src/QuizEngine.cpp \
    src/QuizForm.cpp \
    src/QuizProgress.cpp \
    src/QuizSpec.cpp \
    src/QuizTimerSpec.cpp \
    src/Rand.cpp \
    src/SearchForm.cpp \
    src/SearchCondition.cpp \
    src/SearchConditionForm.cpp \
    src/SearchSpec.cpp \
    src/SearchSpecForm.cpp \
    src/SettingsDialog.cpp \
    src/WordEngine.cpp \
    src/WordEntryDialog.cpp \
    src/WordGraph.cpp \
    src/WordListDialog.cpp \
    src/WordTableDelegate.cpp \
    src/WordTableModel.cpp \
    src/WordTableView.cpp \
    src/WordValidator.cpp \
    src/WordVariationDialog.cpp \
    src/ZPushButton.cpp

# Header files that must be run through moc
HEADERS = \
    src/AboutDialog.h \
    src/ActionForm.h \
    src/AnalyzeQuizDialog.h \
    src/DefineForm.h \
    src/DefinitionBox.h \
    src/DefinitionDialog.h \
    src/DefinitionLabel.h \
    src/HelpDialog.h \
    src/JudgeForm.h \
    src/JudgeDialog.h \
    src/MainWindow.h \
    src/NewQuizDialog.h \
    src/QuizCanvas.h \
    src/QuizForm.h \
    src/QuizQuestionLabel.h \
    src/SearchForm.h \
    src/SearchConditionForm.h \
    src/SearchSpecForm.h \
    src/SettingsDialog.h \
    src/WordEntryDialog.h \
    src/WordLineEdit.h \
    src/WordListDialog.h \
    src/WordTableDelegate.h \
    src/WordTableModel.h \
    src/WordTableView.h \
    src/WordTextEdit.h \
    src/WordValidator.h \
    src/WordVariationDialog.h \
    src/ZPushButton.h
