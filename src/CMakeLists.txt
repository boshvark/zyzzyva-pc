PROJECT (WORDSTUDY)

# Find Qt and add its include directories
FIND_PACKAGE (Qt)
IF (NOT QT_FOUND)
   MESSAGE (FATAL_ERROR "Unable to find Qt library")
ENDIF (NOT QT_FOUND)
INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})

SET (SRCS main.cpp MainWindow.cpp JudgeForm.cpp SearchForm.cpp
     WordValidator.cpp WordEngine.cpp WordGraph.cpp)
SET (MOCS MainWindow.h JudgeForm.h SearchForm.h WordValidator.h)
QT_WRAP_CPP(wordstudy SRCS MOCS)

ADD_EXECUTABLE (wordstudy SRCS)
TARGET_LINK_LIBRARIES (wordstudy ${QT_QT_LIBRARY})
