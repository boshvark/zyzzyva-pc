PROJECT (ZYZZYVA)
SET (EXECUTABLE_OUTPUT_PATH ${ZYZZYVA_SOURCE_DIR})

INCLUDE (FindQt4)
IF (NOT QT4_FOUND)
   MESSAGE (FATAL_ERROR "Unable to find Qt4")
ENDIF (NOT QT4_FOUND)

SET (QT_USE_QTASSISTANT TRUE)
SET (QT_USE_QT3SUPPORT TRUE)
SET (QT_USE_QTXML TRUE)
INCLUDE (${QT_USE_FILE})

# Source files
SET (SRCS
    src/AboutDialog.cpp
    src/AnalyzeQuizDialog.cpp
    src/Auxil.cpp
    src/DefineForm.cpp
    src/DefinitionBox.cpp
    src/DefinitionDialog.cpp
    src/HelpDialog.cpp
    src/ImageItem.cpp
    src/JudgeForm.cpp
    src/main.cpp
    src/MainSettings.cpp
    src/MainWindow.cpp
    src/NewQuizDialog.cpp
    src/QuizEngine.cpp
    src/QuizForm.cpp
    src/QuizProgress.cpp
    src/QuizSpec.cpp
    src/QuizTimerSpec.cpp
    src/SearchForm.cpp
    src/SearchCondition.cpp
    src/SearchConditionForm.cpp
    src/SearchSpec.cpp
    src/SearchSpecForm.cpp
    src/SettingsDialog.cpp
    src/WordEngine.cpp
    src/WordEntryDialog.cpp
    src/WordGraph.cpp
    src/WordListDialog.cpp
    src/WordPopupMenu.cpp
    src/WordTableModel.cpp
    src/WordValidator.cpp
    src/WordVariationDialog.cpp
    )

# Header files that must be run through moc
SET (MOCS
    src/AboutDialog.h
    src/ActionForm.h
    src/AnalyzeQuizDialog.h
    src/DefineForm.h
    src/DefinitionBox.h
    src/DefinitionDialog.h
    src/DefinitionLabel.h
    src/HelpDialog.h
    src/JudgeForm.h
    src/MainWindow.h
    src/NewQuizDialog.h
    src/QuizForm.h
    src/SearchForm.h
    src/SearchConditionForm.h
    src/SearchSpecForm.h
    src/SettingsDialog.h
    src/WordEntryDialog.h
    src/WordLineEdit.h
    src/WordListDialog.h
    src/WordPopupMenu.h
    src/WordTableModel.h
    src/WordTableView.h
    src/WordTextEdit.h
    src/WordValidator.h
    src/WordVariationDialog.h
    )

# Get Zyzzyva version from VERSION
FILE (READ ${ZYZZYVA_SOURCE_DIR}/VERSION ZYZZYVA_VERSION)
STRING (REGEX REPLACE "\n.*" "" ZYZZYVA_VERSION ${ZYZZYVA_VERSION})

# Put version number in about.html
CONFIGURE_FILE (${ZYZZYVA_SOURCE_DIR}/data/help/about.html.in
                ${ZYZZYVA_SOURCE_DIR}/data/help/about.html)

IF (UNIX)
    SET (CMAKE_CXX_FLAGS "-Wall -ansi -pedantic -Wno-long-long")
ELSE (UNIX)
    IF (BORLAND)
        SET (CMAKE_CXX_FLAGS "-w")
    ENDIF (BORLAND)
ENDIF (UNIX)

QT4_WRAP_CPP (SRCS ${MOCS})
ADD_EXECUTABLE (zyzzyva WIN32 ${SRCS})
TARGET_LINK_LIBRARIES (zyzzyva ${QT_LIBRARIES})
