#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

#Some project global settings
set (INITIAL_ATTESTATION_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${INITIAL_ATTESTATION_DIR}/../../.." ABSOLUTE)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_attest LANGUAGES ASM C)
embedded_project_fixup()

#Get the definition of what files we need to build
include(CMakeLists.inc)

if (NOT DEFINED TFM_LVL)
	message(FATAL_ERROR "Incomplete build configuration: TFM_LVL is undefined.")
endif()

if (NOT TARGET tfm_qcbor_encode)
	add_subdirectory(${TFM_ROOT_DIR}/lib/ext/qcbor ${CMAKE_CURRENT_BINARY_DIR}/qcbor)
endif()

if (NOT TARGET tfm_t_cose_sign)
	add_subdirectory(${TFM_ROOT_DIR}/lib/ext/t_cose ${CMAKE_CURRENT_BINARY_DIR}/t_cose)
endif()

#Specify what we build (for the initial attestation service, build as a static library)
add_library(tfm_attest STATIC ${ALL_SRC_ASM} ${ALL_SRC_C} $<TARGET_OBJECTS:tfm_qcbor_encode> $<TARGET_OBJECTS:tfm_qcbor_utility> $<TARGET_OBJECTS:tfm_t_cose_sign> $<TARGET_OBJECTS:tfm_t_cose_utility>)
embedded_set_target_compile_defines(TARGET tfm_attest LANGUAGE C DEFINES __thumb2__ TFM_LVL=${TFM_LVL})

if (DEFINED CMSE_FLAGS)
	embedded_set_target_compile_flags(TARGET tfm_attest LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()

#Set common compiler flags
config_setting_shared_compiler_flags(tfm_attest)

embedded_project_end(tfm_attest)
