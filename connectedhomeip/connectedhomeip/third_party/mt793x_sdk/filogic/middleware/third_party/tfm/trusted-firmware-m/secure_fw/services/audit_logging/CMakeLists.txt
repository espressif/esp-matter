#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

###Some project global settings
set (AUDIT_LOGGING_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${AUDIT_LOGGING_DIR}/../../.." ABSOLUTE)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_audit LANGUAGES ASM C)
embedded_project_fixup()

###Get the definition of what files we need to build
include(CMakeLists.inc)

if (NOT DEFINED TFM_LVL)
	message(FATAL_ERROR "Incomplete build configuration: TFM_LVL is undefined. ")
endif()

#Specify what we build (for the audit logging service, build as a static library)
add_library(tfm_audit STATIC ${ALL_SRC_ASM} ${ALL_SRC_C})
embedded_set_target_compile_defines(TARGET tfm_audit LANGUAGE C DEFINES __thumb2__ TFM_LVL=${TFM_LVL})

if (DEFINED CMSE_FLAGS)
	embedded_set_target_compile_flags(TARGET tfm_audit LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()

#Set common compiler and linker flags
config_setting_shared_compiler_flags(tfm_audit)
config_setting_shared_linker_flags(tfm_audit)

embedded_project_end(tfm_audit)
