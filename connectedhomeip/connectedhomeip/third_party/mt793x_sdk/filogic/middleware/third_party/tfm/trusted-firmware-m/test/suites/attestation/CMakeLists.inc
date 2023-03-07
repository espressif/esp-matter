#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "secure attestation test" module.
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
set(ATTESTATION_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
	message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED ATTEST_INCLUDE_TEST_CODE)
	message(FATAL_ERROR "Incomplete build configuration: ATTEST_INCLUDE_TEST_CODE is undefined. ")
endif()

if (NOT DEFINED ENABLE_ATTESTATION_SERVICE_TESTS)
	message(FATAL_ERROR "Incomplete build configuration: ENABLE_ATTESTATION_SERVICE_TESTS is undefined. ")
elseif(ENABLE_ATTESTATION_SERVICE_TESTS)
	list(APPEND ATTEST_TEST_SRC_S
		"${ATTESTATION_TEST_DIR}/secure/attestation_s_interface_testsuite.c"
		"${ATTESTATION_TEST_DIR}/attest_token_test.c"
		"${ATTESTATION_TEST_DIR}/attest_token_decode.c"
		"${ATTESTATION_TEST_DIR}/attest_public_key.c"
		"${TFM_ROOT_DIR}/lib/ext/qcbor/util/qcbor_util.c"
	)

	list(APPEND ATTEST_TEST_SRC_NS
		"${ATTESTATION_TEST_DIR}/non_secure/attestation_ns_interface_testsuite.c"
		"${ATTESTATION_TEST_DIR}/attest_token_test.c"
		"${ATTESTATION_TEST_DIR}/attest_token_decode.c"
		"${ATTESTATION_TEST_DIR}/attest_public_key.c"
		"${TFM_ROOT_DIR}/lib/ext/qcbor/util/qcbor_util.c"
	)

	if (ATTEST_INCLUDE_TEST_CODE)
		set_property(SOURCE ${ATTEST_TEST_SRC_S}  APPEND PROPERTY COMPILE_DEFINITIONS INCLUDE_TEST_CODE)
		set_property(SOURCE ${ATTEST_TEST_SRC_NS} APPEND PROPERTY COMPILE_DEFINITIONS INCLUDE_TEST_CODE)
	endif()

	#Setting include directories
	embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/secure_fw/services/initial_attestation ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/lib/ext/qcbor/inc ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/lib/ext/qcbor/util ABSOLUTE)
	embedded_include_directories(PATH ${TFM_ROOT_DIR}/lib/ext/t_cose/inc ABSOLUTE)

	#Append all our source files to global lists.
	list(APPEND ALL_SRC_C_S  ${ATTEST_TEST_SRC_S})
	list(APPEND ALL_SRC_C_NS ${ATTEST_TEST_SRC_NS})
	unset(ATTEST_TEST_SRC_S)
	unset(ATTEST_TEST_SRC_NS)
endif()
