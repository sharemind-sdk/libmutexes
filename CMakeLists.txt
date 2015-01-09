#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(SharemindLibMutexes CXX)
SET(SHAREMIND_LIBMUTEXES_VERSION "0.1.0")

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(TBB REQUIRED)
SET(TBB_LIBRARIES "")
FOREACH(dir IN LISTS TBB_LIBRARY_DIRS)
    LIST(APPEND TBB_LIBRARIES "-L${dir}")
ENDFOREACH()
LIST(APPEND TBB_LIBRARIES "${TBB_LIBRARY}")

SET(SHAREMIND_LIBMUTEXES_INSTALL_INCLUDE_DIRS "")
LIST(APPEND SHAREMIND_LIBMUTEXES_INSTALL_INCLUDE_DIRS ${TBB_INCLUDE_DIRS})
SET(SHAREMIND_LIBMUTEXES_INSTALL_LINK_LIBRARIES "")
LIST(APPEND SHAREMIND_LIBMUTEXES_INSTALL_LINK_LIBRARIES ${TBB_LIBRARIES})
SET(SHAREMIND_LIBMUTEXES_INSTALL_DEFINITIONS "")

# Add files to IDE:
FILE(GLOB_RECURSE SharemindLibMutexes_headers
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
ADD_CUSTOM_TARGET("include_SharemindLibMutexes_headers_in_IDE"
                  SOURCES ${SharemindLibMutexes_headers})

# Install headers:
INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        DESTINATION "include/sharemind"
        FILES_MATCHING PATTERN "*.h")

# Install cmake files:
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibMutexesConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibMutexesConfig.cmake" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibMutexesConfig.cmake"
        DESTINATION "lib/SharemindLibMutexes")
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibMutexesConfigVersion.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibMutexesConfigVersion.cmake" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibMutexesConfigVersion.cmake"
        DESTINATION "lib/SharemindLibMutexes")
