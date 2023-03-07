#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "test" module.
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
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED ENABLE_AUDIT_LOGGING_SERVICE_TESTS)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_AUDIT_LOGGING_SERVICE_TESTS is undefined.")
endif()

if (NOT DEFINED ENABLE_PLATFORM_SERVICE_TESTS)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_PLATFORM_SERVICE_TESTS is undefined.")
endif()

embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/include ABSOLUTE)

# Include the test framework
include(${CMAKE_CURRENT_LIST_DIR}/framework/CMakeLists.inc)

# Include the test suites
include(${CMAKE_CURRENT_LIST_DIR}/suites/core/CMakeLists.inc)
include(${CMAKE_CURRENT_LIST_DIR}/suites/sst/CMakeLists.inc)
include(${CMAKE_CURRENT_LIST_DIR}/suites/its/CMakeLists.inc)
include(${CMAKE_CURRENT_LIST_DIR}/suites/crypto/CMakeLists.inc)
include(${CMAKE_CURRENT_LIST_DIR}/suites/attestation/CMakeLists.inc)
include(${CMAKE_CURRENT_LIST_DIR}/suites/qcbor/CMakeLists.inc)
include(${CMAKE_CURRENT_LIST_DIR}/suites/t_cose/CMakeLists.inc)
include(${CMAKE_CURRENT_LIST_DIR}/suites/ipc/CMakeLists.inc)
if (ENABLE_AUDIT_LOGGING_SERVICE_TESTS)
	include(${CMAKE_CURRENT_LIST_DIR}/suites/audit/CMakeLists.inc)
endif()
if (ENABLE_PLATFORM_SERVICE_TESTS)
	include(${CMAKE_CURRENT_LIST_DIR}/suites/platform/CMakeLists.inc)
endif()
if (TFM_MULTI_CORE_TEST)
	include(${CMAKE_CURRENT_LIST_DIR}/suites/multi_core/CMakeLists.inc)
endif()

if (MTK_CI_TEST)
	include(${CMAKE_CURRENT_LIST_DIR}/suites/MTK_CI/CMakeLists.inc)
endif()

# Include the test partitions
include(${CMAKE_CURRENT_LIST_DIR}/test_services/CMakeLists.inc)
