#==============================================================================
# vtkGraphCut
#
# Description: Graph cut algorithm
#
# Author: Berend Klein Haneveld <berendkleinhaneveld@gmail.com>, (C) 2013
#
# Copyright: See COPYING file that comes with this distribution
# License: new BSD licensing
#==============================================================================

# Define the project
PROJECT(vtkGraphCut)
SET(KIT vtkGraphCutTestApp)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
ENABLE_TESTING()

IF(POLICY CMP0017)
  CMAKE_POLICY(SET CMP0017 NEW)
ENDIF(POLICY CMP0017)

# On some platforms (OSX) this may not be defined, so search for it
IF(NOT DEFINED CMAKE_INSTALL_NAME_TOOL)
  FIND_PROGRAM(CMAKE_INSTALL_NAME_TOOL install_name_tool)
ENDIF(NOT DEFINED CMAKE_INSTALL_NAME_TOOL)

# Find VTK
FIND_PACKAGE(VTK)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
  ADD_SUBDIRECTORY(Testing)
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR
    "Cannot build the executable without VTK. Please set the VTK_DIR")
ENDIF(VTK_FOUND)


# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

# Find all headers in root folder
FILE(GLOB HEADERS *.h)

# Find all source files in root folder
FILE(GLOB SOURCES *.cxx)

SET_SOURCE_FILES_PROPERTIES(
  vtkEdge.cxx
  Edges.cxx
  vtkNode.cxx
  vtkNodes.cxx
  WRAP_EXCLUDE
)

SET(ALL_SOURCES
  ${SOURCES}
)

# Give user option of building shared or static libraries.  Default to
# the choice made for VTK.
OPTION(BUILD_SHARED_LIBS 
  "Build with shared libraries."
  ${VTK_BUILD_SHARED_LIBS}
)

# Set VTKLOCAL_BUILD_SHARED_LIBS to 1 or 0 for use in creating the
# configuration header.
IF(BUILD_SHARED_LIBS)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 1)
ELSE(BUILD_SHARED_LIBS)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 0)
ENDIF(BUILD_SHARED_LIBS)

# List the kits from VTK that are needed by this project.
SET(VTK_LOCAL_LIBS
  vtkCommonCore
  vtkIOCore
)

# Hide warnings about using deprecated function calls
IF(APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
ENDIF(APPLE)

# If this is a build tree, provide an option for putting this
# project's executables and libraries in with VTK's.
IF(EXISTS ${VTK_DIR}/bin)
  OPTION(USE_VTK_OUTPUT_PATHS
    "Use VTK's output directory for this project's executables and libraries."
    OFF)
  MARK_AS_ADVANCED(USE_VTK_OUTPUT_PATHS)
  IF(USE_VTK_OUTPUT_PATHS)
    SET(LIBRARY_OUTPUT_PATH ${VTK_DIR}/bin)
    SET(EXECUTABLE_OUTPUT_PATH ${VTK_DIR}/bin)
  ENDIF(USE_VTK_OUTPUT_PATHS)
ENDIF(EXISTS ${VTK_DIR}/bin)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME}
  ${HEADERS}
  ${SOURCES}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  ${VTK_LOCAL_LIBS}
)

# Give the user the option to build the test as an app
IF(APPLE)
  OPTION(BUILD_TEST_AS_APP
    "Build the test as an app bundle."
    OFF
  )
  IF(BUILD_TEST_AS_APP)
    SET(EXECUTABLE_FLAG MACOSX_BUNDLE)
  ENDIF(BUILD_TEST_AS_APP)
ENDIF(APPLE)

# Provide the user with the option to build a test app
# Default is ON
OPTION(BUILD_TEST
  "Build a test application."
  ON
)

# Give the user the option of wrapping the library in Python
OPTION(WRAP_PYTHON 
  "Wrap the library in Python."
  ${VTK_WRAP_PYTHON}
)

# Create Python wrappers if VTK is wrapped in Python.
IF(WRAP_PYTHON AND VTK_WRAP_PYTHON)
  SET(VTK_WRAP_PYTHON_FIND_LIBS ON)
  INCLUDE(${VTK_CMAKE_DIR}/vtkWrapPython.cmake)
  INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")
  VTK_WRAP_PYTHON3(${PROJECT_NAME}Python 
    VTK_LOCAL_PYTHON_SRCS
    "${SOURCES}"
  )
  ADD_LIBRARY(${PROJECT_NAME}PythonD 
    ${VTK_LOCAL_PYTHON_SRCS}
  )
  ADD_LIBRARY(${PROJECT_NAME}Python 
    MODULE
    ${PROJECT_NAME}PythonInit.cxx
  )
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}PythonD 
    ${VTK_LOCAL_LIBS}
  	${PROJECT_NAME}
  )
  FOREACH(c ${VTK_LOCAL_LIBS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}PythonD ${c}PythonD)
  ENDFOREACH(c)
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}Python 
  	${PROJECT_NAME}PythonD
  )
  
  # Python extension modules on Windows must have the extension ".pyd"
  # instead of ".dll" as of Python 2.5.  Older python versions do support
  # this suffix.
  IF(WIN32 AND NOT CYGWIN)
    SET_TARGET_PROPERTIES(${PROJECT_NAME}Python PROPERTIES SUFFIX ".pyd")
  ENDIF(WIN32 AND NOT CYGWIN)    
ENDIF(WRAP_PYTHON AND VTK_WRAP_PYTHON)
