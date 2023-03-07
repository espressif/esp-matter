#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "core test" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR - root directory of the TF-M repo.
#
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(CORE_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED TFM_PARTITION_TEST_CORE)
	message(FATAL_ERROR "Incomplete build configuration: TFM_PARTITION_TEST_CORE is undefined. ")
elseif (TFM_PARTITION_TEST_CORE)
	list(APPEND ALL_SRC_C_NS "${CORE_TEST_DIR}/non_secure/core_test_api.c")
endif()

if (NOT DEFINED CORE_TEST_POSITIVE)
	message(FATAL_ERROR "Incomplete build configuration: CORE_TEST_POSITIVE is undefined. ")
elseif (CORE_TEST_POSITIVE)
	list(APPEND ALL_SRC_C_NS "${CORE_TEST_DIR}/non_secure/core_ns_positive_testsuite.c")
endif()

if (NOT DEFINED CORE_TEST_INTERACTIVE)
	message(FATAL_ERROR "Incomplete build configuration: CORE_TEST_INTERACTIVE is undefined. ")
elseif (CORE_TEST_INTERACTIVE)
	list(APPEND ALL_SRC_C_NS "${CORE_TEST_DIR}/non_secure/core_ns_interactive_testsuite.c")
endif()

# Disable recursion test from core test by default
if (ENABLE_TFM_CORE_RECURSION_TESTS)
	add_definitions(-DENABLE_TFM_CORE_RECURSION_TESTS)
endif()

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/test/interface/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
