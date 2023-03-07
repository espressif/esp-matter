#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to build the Mbed Crypto libraries
#with the same compilation setting as used by the file including this one.
cmake_minimum_required(VERSION 3.7)

#Define where mbed-crypto intermediate output files are stored.
set (MBEDCRYPTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/mbed-crypto")

#Check input variables
if(NOT DEFINED MBEDCRYPTO_DEBUG)
    message(FATAL_ERROR "Please set MBEDCRYPTO_DEBUG to 'OFF' or 'ON' before including this file.")
endif()

if(NOT DEFINED MBEDCRYPTO_SOURCE_DIR)
    message(FATAL_ERROR "Please set MBEDCRYPTO_SOURCE_DIR before including this file.")
endif()

if(NOT DEFINED MBEDCRYPTO_INSTALL_DIR)
    message(FATAL_ERROR "Please set MBEDCRYPTO_INSTALL_DIR before including this file.")
endif()

if(NOT DEFINED MBEDCRYPTO_C_FLAGS)
    message(FATAL_ERROR "Please set MBEDCRYPTO_C_FLAGS before including this file.")
endif()

if(NOT DEFINED MBEDCRYPTO_TARGET_NAME)
    message(FATAL_ERROR "Please set MBEDCRYPTO_TARGET_NAME before including this file.")
endif()

if(MBEDCRYPTO_DEBUG)
    set(MBEDCRYPTO_BUILD_TYPE "Debug")
else()
    set(MBEDCRYPTO_BUILD_TYPE "MinSizeRel")
endif()

#Based on preinclude input variables, decide if preinclude flags need to be appended
if((NOT DEFINED MBEDCRYPTO_PREINCLUDE_PREFIX) OR (NOT DEFINED MBEDCRYPTO_PREINCLUDE_HEADER))
    message(STATUS "Building mbed-crypto without pre-included headers and global symbols prefixing.")
else()
    compiler_get_preinclude_option_string(${MBEDCRYPTO_PREINCLUDE_HEADER} _PRE_INC_STRING)
    set(MBEDCRYPTO_PREINCLUDE_C_FLAGS " -DLIB_PREFIX_NAME=${MBEDCRYPTO_PREINCLUDE_PREFIX} ${_PRE_INC_STRING}")
    string(APPEND MBEDCRYPTO_C_FLAGS ${MBEDCRYPTO_PREINCLUDE_C_FLAGS})
endif()

function(get_mbedcrypto_mbedtls_version)
    file(READ "${MBEDCRYPTO_SOURCE_DIR}/include/mbedtls/version.h" MBEDCRYPTO_MBEDTLS_VER_FILE)
    string(REGEX REPLACE ".*#define[ ]+MBEDTLS_VERSION_STRING[^\"]+\"+([0-9.]+)\".*" "\\1"
        _MBEDCRYPTO_MBEDTLS_VER ${MBEDCRYPTO_MBEDTLS_VER_FILE})
    set(MBEDCRYPTO_MBEDTLS_VERSION ${_MBEDCRYPTO_MBEDTLS_VER} PARENT_SCOPE)
endfunction()

get_mbedcrypto_mbedtls_version()

string(APPEND MBEDCRYPTO_C_FLAGS ${CMAKE_C_FLAGS})

# Workaround Mbed TLS issue https://github.com/ARMmbed/mbedtls/issues/1077
if ((${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv8-M.BASE") OR
    (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv6-M") OR
    (${ARM_CPU_ARCHITECTURE} STREQUAL "ARMv6S-M"))
   string(APPEND MBEDCRYPTO_C_FLAGS " -DMULADDC_CANNOT_USE_R7")
endif()

if (TARGET ${MBEDCRYPTO_TARGET_NAME})
   message(FATAL_ERROR "A target with name ${MBEDCRYPTO_TARGET_NAME} is already\
defined. Please set MBEDCRYPTO_TARGET_NAME to a unique value.")
endif()

#Build mbed-crypto as external project.
#This ensures mbed-crypto is built with exactly defined settings.
#mbed-crypto will be used from its install location
include(ExternalProject)
# Add Mbed Crypto files to the build.
set(_static_lib_command ${CMAKE_C_CREATE_STATIC_LIBRARY})
externalproject_add(${MBEDCRYPTO_TARGET_NAME}
    SOURCE_DIR ${MBEDCRYPTO_SOURCE_DIR}
    #Set Mbed Crypto features
    CMAKE_ARGS -DENABLE_TESTING=OFF -DENABLE_PROGRAMS=OFF
    #Enforce our build system's settings.
    CMAKE_ARGS -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
    #Inherit the build setting of this project
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${MBEDCRYPTO_BUILD_TYPE}
    #C compiler settings
    CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER:string=${CMAKE_C_COMPILER}
    CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_ID:string=${CMAKE_C_COMPILER_ID}
    CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS:string=${MBEDCRYPTO_C_FLAGS}
    CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS_DEBUG:string=${CMAKE_C_FLAGS_DEBUG}
    CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS_MINSIZEREL:string=${CMAKE_C_FLAGS_MINSIZEREL}
    CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS_RELEASE:string=${CMAKE_C_FLAGS_RELEASE}
    CMAKE_CACHE_ARGS -DCMAKE_C_OUTPUT_EXTENSION:string=.o
    CMAKE_CACHE_ARGS -DCMAKE_C_COMPILER_WORKS:bool=true
    #Archiver settings
    CMAKE_CACHE_ARGS -DCMAKE_AR:string=${CMAKE_AR}
    CMAKE_CACHE_ARGS -DCMAKE_C_CREATE_STATIC_LIBRARY:internal=${_static_lib_command}
    CMAKE_CACHE_ARGS -DCMAKE_C_LINK_EXECUTABLE:string=${CMAKE_C_LINK_EXECUTABLE}
    CMAKE_CACHE_ARGS -DCMAKE_STATIC_LIBRARY_PREFIX_C:string=${CMAKE_STATIC_LIBRARY_PREFIX_C}
    CMAKE_CACHE_ARGS -DCMAKE_STATIC_LIBRARY_PREFIX_CXX:string=${CMAKE_STATIC_LIBRARY_PREFIX_CXX}
    #Install location
    CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:string=${MBEDCRYPTO_INSTALL_DIR}
    #Place for intermediate build files
    BINARY_DIR ${MBEDCRYPTO_BINARY_DIR})

#Add an install target to force installation after each mbed-crypto build. Without
#this target installation happens only when a clean mbed-crypto build is executed.
add_custom_target(${MBEDCRYPTO_TARGET_NAME}_install
    COMMAND ${CMAKE_COMMAND} --build ${MBEDCRYPTO_BINARY_DIR}  -- install
    WORKING_DIRECTORY ${MBEDCRYPTO_BINARY_DIR}
    COMMENT "Installing Mbed Crypto to ${MBEDCRYPTO_INSTALL_DIR}"
    VERBATIM)
#Make install rule depend on Mbed Crypto library build
add_dependencies(${MBEDCRYPTO_TARGET_NAME}_install ${MBEDCRYPTO_TARGET_NAME})

add_custom_command(TARGET ${MBEDCRYPTO_TARGET_NAME}_install
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${MBEDCRYPTO_INSTALL_DIR}/include/psa
                           ${MBEDCRYPTO_INSTALL_DIR}/include/mbedcrypto/psa
                   COMMAND ${CMAKE_COMMAND} -E remove_directory
                           ${MBEDCRYPTO_INSTALL_DIR}/include/psa)
