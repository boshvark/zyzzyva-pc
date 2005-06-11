PROJECT (ZYZZYVA)
SET (EXECUTABLE_OUTPUT_PATH ${ZYZZYVA_SOURCE_DIR})

# Find Qt and add its include directories
FIND_PACKAGE (Qt)
IF (NOT QT_FOUND)
   MESSAGE (FATAL_ERROR "Unable to find Qt library")
ENDIF (NOT QT_FOUND)
INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})

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
    src/MainWindow.cpp
    src/NewQuizDialog.cpp
    src/QuizEngine.cpp
    src/QuizForm.cpp
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
    src/WordListView.cpp
    src/WordListViewItem.cpp
    src/WordPopupMenu.cpp
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
    src/WordListView.h
    src/WordPopupMenu.h
    src/WordValidator.h
    src/WordVariationDialog.h
    )

# Get Zyzzyva version from VERSION
FILE (READ ${ZYZZYVA_SOURCE_DIR}/VERSION ZYZZYVA_VERSION)
STRING (REGEX REPLACE "\n.*" "" ZYZZYVA_VERSION ${ZYZZYVA_VERSION})

# Put version number in about.html
CONFIGURE_FILE (${ZYZZYVA_SOURCE_DIR}/data/help/about.html.in
                ${ZYZZYVA_SOURCE_DIR}/data/help/about.html)

QT_WRAP_CPP(zyzzyva SRCS MOCS)
ADD_EXECUTABLE (zyzzyva WIN32 SRCS)
IF (WIN32)
    TARGET_LINK_LIBRARIES (zyzzyva ${QT_QTMAIN_LIBRARY})
    ADD_DEFINITIONS (-DNEEDS_QMAIN)
ENDIF (WIN32)
TARGET_LINK_LIBRARIES (zyzzyva ${QT_QT_LIBRARY} ${QT_QASSISTANTCLIENT_LIBRARY})
