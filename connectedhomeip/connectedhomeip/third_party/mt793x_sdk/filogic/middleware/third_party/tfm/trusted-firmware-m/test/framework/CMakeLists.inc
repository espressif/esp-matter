#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "test framework" module.
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
set(TEST_FRAMEWORK_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

set (TEST_FRAMEWORK_C_SRC "${TEST_FRAMEWORK_DIR}/test_framework_helpers.c"
		"${TEST_FRAMEWORK_DIR}/test_framework_integ_test_helper.c"
		"${TEST_FRAMEWORK_DIR}/test_framework.c"
	)

set (TEST_FRAMEWORK_C_SRC_NS "${TEST_FRAMEWORK_DIR}/non_secure_suites.c"
	)

set (TEST_FRAMEWORK_C_SRC_S "${TEST_FRAMEWORK_DIR}/secure_suites.c"
	)

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${TEST_FRAMEWORK_C_SRC})
list(APPEND ALL_SRC_C_S ${TEST_FRAMEWORK_C_SRC_S})
list(APPEND ALL_SRC_C_NS ${TEST_FRAMEWORK_C_SRC_NS})

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
