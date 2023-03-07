#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "initial_attestation" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#	TFM_ROOT_DIR        - root directory of the TF-M repository.
#Outputs:
#	Will modify include directories to make the source compile.
#	ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#	Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(INITIAL_ATTESTATION_DIR ${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED ATTEST_INCLUDE_OPTIONAL_CLAIMS)
	message(FATAL_ERROR "Incomplete build configuration: ATTEST_INCLUDE_OPTIONAL_CLAIMS is undefined.")
endif()

if (NOT DEFINED ATTEST_INCLUDE_TEST_CODE)
	message(FATAL_ERROR "Incomplete build configuration: ATTEST_INCLUDE_TEST_CODE is undefined.")
endif()

if (NOT DEFINED ATTEST_INCLUDE_COSE_KEY_ID)
	message(FATAL_ERROR "Incomplete build configuration: ATTEST_INCLUDE_COSE_KEY_ID is undefined.")
endif()

list(APPEND ATTEST_C_SRC
	"${INITIAL_ATTESTATION_DIR}/tfm_attestation_secure_api.c"
	"${INITIAL_ATTESTATION_DIR}/tfm_attestation.c"
	"${INITIAL_ATTESTATION_DIR}/tfm_attestation_req_mngr.c"
	"${INITIAL_ATTESTATION_DIR}/attestation_core.c"
	"${INITIAL_ATTESTATION_DIR}/attestation_key.c"
	"${INITIAL_ATTESTATION_DIR}/attest_token.c"
	)

if (ATTEST_INCLUDE_OPTIONAL_CLAIMS)
	set_property(SOURCE ${ATTEST_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS INCLUDE_OPTIONAL_CLAIMS)
endif()

if (ATTEST_INCLUDE_TEST_CODE)
	set_property(SOURCE ${ATTEST_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS INCLUDE_TEST_CODE)
endif()

if (ATTEST_INCLUDE_COSE_KEY_ID)
	set_property(SOURCE ${ATTEST_C_SRC} APPEND PROPERTY COMPILE_DEFINITIONS INCLUDE_COSE_KEY_ID)
endif()

#Inform the user about attestation service features selected based on the cmake flags
message("The Initial Attestation service compile configuration is as follows:")
message("- ATTEST_INCLUDE_OPTIONAL_CLAIMS: ${ATTEST_INCLUDE_OPTIONAL_CLAIMS}")
message("- ATTEST_INCLUDE_TEST_CODE:       ${ATTEST_INCLUDE_TEST_CODE}")
message("- ATTEST_INCLUDE_COSE_KEY_ID:     ${ATTEST_INCLUDE_COSE_KEY_ID}")

#Setting include directories
embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/ext/common ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/platform/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/bl2/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/core/include ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/spm ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/lib/ext/qcbor/inc ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/lib/ext/t_cose/inc ABSOLUTE)
embedded_include_directories(PATH ${TFM_ROOT_DIR}/lib/ext/t_cose/src ABSOLUTE)
embedded_include_directories(PATH ${INITIAL_ATTESTATION_DIR} ABSOLUTE)

set(BUILD_CMSIS_CORE Off)
set(BUILD_RETARGET Off)
set(BUILD_NATIVE_DRIVERS Off)
set(BUILD_STARTUP Off)
set(BUILD_TARGET_CFG Off)
set(BUILD_TARGET_HARDWARE_KEYS Off)
set(BUILD_TARGET_NV_COUNTERS Off)
set(BUILD_CMSIS_DRIVERS Off)
set(BUILD_TIME Off)
set(BUILD_UART_STDOUT Off)
set(BUILD_FLASH Off)
set(BUILD_PLAT_TEST Off)
if(NOT DEFINED PLATFORM_CMAKE_FILE)
	message (FATAL_ERROR "Platform specific CMake is not defined. Please set PLATFORM_CMAKE_FILE.")
elseif(NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message (FATAL_ERROR "Platform specific CMake \"${PLATFORM_CMAKE_FILE}\" file does not exist. Please fix value of PLATFORM_CMAKE_FILE.")
else()
	include(${PLATFORM_CMAKE_FILE})
endif()

#Append all our source files to global lists.
list(APPEND ALL_SRC_C ${ATTEST_C_SRC})
unset(ATTEST_C_SRC)
