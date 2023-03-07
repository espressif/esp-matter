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
set (LIBSPRT_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(TFM_ROOT_DIR "${LIBSPRT_DIR}/../../.." ABSOLUTE)

#Include common stuff to control cmake.
include("Common/BuildSys")

#Start an embedded project.
embedded_project_start(CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
project(libtfmsprt LANGUAGES ASM C)
embedded_project_fixup()

#Get the definition of what files we need to build
include(CMakeLists.inc)

#Build secure partition runtime library as a static library
add_library(libtfmsprt STATIC ${ALL_SRC_ASM_S} ${ALL_SRC_C_S})
embedded_set_target_compile_defines(TARGET libtfmsprt LANGUAGE C DEFINES __thumb2__)
if (DEFINED CMSE_FLAGS)
    embedded_set_target_compile_flags(TARGET libtfmsprt LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()
#Set common compiler and linker flags
config_setting_shared_compiler_flags(libtfmsprt)
config_setting_shared_linker_flags(libtfmsprt)

embedded_project_end(libtfmsprt)
