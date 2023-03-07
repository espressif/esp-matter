#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

# Tell cmake where our modules can be found
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../cmake)

###Some project global settings
set (INTERNAL_TRUSTED_STORAGE_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${INTERNAL_TRUSTED_STORAGE_DIR}/../../.." ABSOLUTE)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(tfm_internal_trusted_storage LANGUAGES ASM C)
embedded_project_fixup()

###Get the definition of what files we need to build
include(CMakeLists.inc)

if (NOT DEFINED TFM_LVL)
    message(FATAL_ERROR "Incomplete build configuration: TFM_LVL is undefined. ")
endif()

# Specify what we build (for the internal trusted storage service, build as a static library)
add_library(tfm_internal_trusted_storage STATIC ${ALL_SRC_ASM} ${ALL_SRC_C})

#Set common compiler and linker flags
if (DEFINED CMSE_FLAGS)
    embedded_set_target_compile_flags(TARGET tfm_internal_trusted_storage LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()
config_setting_shared_compiler_flags(tfm_internal_trusted_storage)
config_setting_shared_linker_flags(tfm_internal_trusted_storage)

embedded_set_target_compile_defines(TARGET tfm_internal_trusted_storage LANGUAGE C DEFINES __thumb2__ TFM_LVL=${TFM_LVL})

embedded_project_end(tfm_internal_trusted_storage)
