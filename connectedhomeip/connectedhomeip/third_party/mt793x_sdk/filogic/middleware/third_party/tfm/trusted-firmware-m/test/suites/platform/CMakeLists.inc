#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "secure platform service test" module.
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
set(PLATFORM_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED ENABLE_PLATFORM_SERVICE_TESTS)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_PLATFORM_SERVICE_TESTS is undefined. ")
elseif(ENABLE_PLATFORM_SERVICE_TESTS)
	list(APPEND PLATFORM_TEST_SRC_S
		"${PLATFORM_TEST_DIR}/secure/platform_s_interface_testsuite.c"
	)

	list(APPEND PLATFORM_TEST_SRC_NS
		"${PLATFORM_TEST_DIR}/non_secure/platform_ns_interface_testsuite.c"
	)

	list(APPEND PLATFORM_TEST_SRC
		"${PLATFORM_TEST_DIR}/platform_tests_common.c"
	)

	#Setting include directories
	embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
	embedded_include_directories(PATH ${PLATFORM_TEST_DIR} ABSOLUTE)

	#Append all our source files to global lists.
	list(APPEND ALL_SRC_C_S  ${PLATFORM_TEST_SRC_S}  ${PLATFORM_TEST_SRC})
	list(APPEND ALL_SRC_C_NS ${PLATFORM_TEST_SRC_NS} ${PLATFORM_TEST_SRC})
	unset(PLATFORM_TEST_SRC_S)
	unset(PLATFORM_TEST_SRC_NS)
endif()
