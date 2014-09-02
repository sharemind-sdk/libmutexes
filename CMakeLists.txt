#
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
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
SET(SHAREMIND_COMPILERSUPPORT_INSTALL_INCLUDE_DIRS
    "${CMAKE_INSTALL_PREFIX}/include")
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
