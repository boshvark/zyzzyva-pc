PROJECT (ZYZZYVA)
SET (VERSION 0.3.0)
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
    src/DefinitionDialog.cpp
    src/HelpDialog.cpp
    src/JudgeForm.cpp
    src/main.cpp
    src/MainWindow.cpp
    src/NewQuizDialog.cpp
    src/QuizEngine.cpp
    src/QuizForm.cpp
    src/SearchForm.cpp
    src/SearchSpec.cpp
    src/SearchSpecForm.cpp
    src/SettingsDialog.cpp
    src/WordEngine.cpp
    src/WordGraph.cpp
    src/WordPopupMenu.cpp
    src/WordValidator.cpp
    src/ZListView.cpp
    src/ZListViewItem.cpp
    )

# Header files that must be run through moc
SET (MOCS
    src/AboutDialog.h
    src/AnalyzeQuizDialog.h
    src/DefinitionDialog.h
    src/HelpDialog.h
    src/JudgeForm.h
    src/MainWindow.h
    src/NewQuizDialog.h
    src/QuizForm.h
    src/SearchForm.h
    src/SearchSpecForm.h
    src/SettingsDialog.h
    src/WordPopupMenu.h
    src/WordValidator.h
    src/ZListView.h
    )

# Put version number in about.html
CONFIGURE_FILE (${ZYZZYVA_SOURCE_DIR}/docs/help/about.html.in
                ${ZYZZYVA_SOURCE_DIR}/docs/help/about.html)

QT_WRAP_CPP(zyzzyva SRCS MOCS)
ADD_EXECUTABLE (zyzzyva SRCS)
TARGET_LINK_LIBRARIES (zyzzyva ${QT_QT_LIBRARY} qassistantclient)
