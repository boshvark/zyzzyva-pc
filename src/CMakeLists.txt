PROJECT (ZYZZYVA)

# Find Qt and add its include directories
FIND_PACKAGE (Qt)
IF (NOT QT_FOUND)
   MESSAGE (FATAL_ERROR "Unable to find Qt library")
ENDIF (NOT QT_FOUND)
INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})

SET (SRCS main.cpp MainWindow.cpp JudgeForm.cpp QuizForm.cpp
     SearchForm.cpp WordValidator.cpp WordEngine.cpp QuizEngine.cpp
     WordGraph.cpp SettingsDialog.cpp)
SET (MOCS MainWindow.h JudgeForm.h QuizForm.h SearchForm.h
     WordValidator.h SettingsDialog.h)
QT_WRAP_CPP(zyzzyva SRCS MOCS)

ADD_EXECUTABLE (zyzzyva SRCS)
TARGET_LINK_LIBRARIES (zyzzyva ${QT_QT_LIBRARY})
