#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

#Some project global settings
set (QCBOR_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${QCBOR_DIR}/../../.." ABSOLUTE)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_qcbor LANGUAGES C)
embedded_project_fixup()

#Append all our source files to global lists.
list(APPEND ALL_SRC_C_ENCODE
	"${QCBOR_DIR}/src/qcbor_encode.c"
	)

#Append all our source files to global lists.
list(APPEND ALL_SRC_C_DECODE
	"${QCBOR_DIR}/src/qcbor_decode.c"
	)

#Append all our source files to global lists.
list(APPEND ALL_SRC_C_QCBOR_UTILITY
	"${QCBOR_DIR}/src/ieee754.c"
	"${QCBOR_DIR}/src/UsefulBuf.c"
	)

#Append all our source files to global lists.
list(APPEND ALL_SRC_C_TEST
	"${QCBOR_DIR}/test/float_tests.c"
	"${QCBOR_DIR}/test/half_to_double_from_rfc7049.c"
	"${QCBOR_DIR}/test/qcbor_decode_tests.c"
	"${QCBOR_DIR}/test/qcbor_encode_tests.c"
	"${QCBOR_DIR}/test/run_tests.c"
	"${QCBOR_DIR}/test/UsefulBuf_Tests.c"
	)

#Setting include directories
embedded_include_directories(PATH ${QCBOR_DIR}/inc ABSOLUTE)

#Specify what we build (for the QCBOR, build as an object library)
add_library(tfm_qcbor_encode OBJECT ${ALL_SRC_C_ENCODE})
add_library(tfm_qcbor_decode OBJECT ${ALL_SRC_C_DECODE})
add_library(tfm_qcbor_utility OBJECT ${ALL_SRC_C_QCBOR_UTILITY})
add_library(tfm_qcbor_test   OBJECT ${ALL_SRC_C_TEST})

if (DEFINED CMSE_FLAGS)
	embedded_set_target_compile_flags(TARGET tfm_qcbor_encode LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
	embedded_set_target_compile_flags(TARGET tfm_qcbor_decode LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
	embedded_set_target_compile_flags(TARGET tfm_qcbor_utility LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
	embedded_set_target_compile_flags(TARGET tfm_qcbor_test   LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()

#Set common compiler flags
config_setting_shared_compiler_flags(tfm_qcbor_encode)
config_setting_shared_compiler_flags(tfm_qcbor_decode)
config_setting_shared_compiler_flags(tfm_qcbor_utility)
config_setting_shared_compiler_flags(tfm_qcbor_test)

embedded_project_end(tfm_qcbor_encode)
embedded_project_end(tfm_qcbor_decode)
embedded_project_end(tfm_qcbor_utility)
embedded_project_end(tfm_qcbor_test)
