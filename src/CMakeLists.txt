PROJECT (WORDSTUDY)

# Find Qt and add its include directories
FIND_PACKAGE (Qt)
IF (NOT QT_FOUND)
   MESSAGE (FATAL_ERROR "Unable to find Qt library")
ENDIF (NOT QT_FOUND)
INCLUDE_DIRECTORIES (${QT_INCLUDE_DIR})

SET (SRCS main.cpp MainWindow.cpp LookupForm.cpp WordEngine.cpp DAWG.cpp)
SET (MOCS MainWindow.h LookupForm.h)
QT_WRAP_CPP(wordstudy SRCS MOCS)

ADD_EXECUTABLE (wordstudy SRCS)
TARGET_LINK_LIBRARIES (wordstudy ${QT_QT_LIBRARY})
