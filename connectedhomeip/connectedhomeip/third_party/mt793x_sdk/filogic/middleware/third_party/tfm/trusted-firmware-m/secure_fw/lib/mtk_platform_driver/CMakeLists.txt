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
project(lib_mtk_drv LANGUAGES ASM C)
embedded_project_fixup()

#Get the definition of what files we need to build
include(CMakeLists.inc)

#Build secure partition runtime library as a static library
add_library(lib_mtk_drv STATIC ${LIB_MTK_DRV_C_SRC})

embedded_set_target_compile_defines(TARGET lib_mtk_drv LANGUAGE C DEFINES __thumb2__)
embedded_set_target_compile_defines(TARGET lib_mtk_drv LANGUAGE C DEFINES ${LIB_MTK_DRV_C_DEF} APPEND)

if (PSRAM_MODE_UHS STREQUAL "y")
    embedded_set_target_compile_defines(TARGET lib_mtk_drv LANGUAGE C DEFINES HAL_PSRAM_UHS_ENABLED APPEND)
else ()
    embedded_set_target_compile_defines(TARGET lib_mtk_drv LANGUAGE C DEFINES HAL_PSRAM_NON_UHS_ENABLED APPEND)
endif ()

if (DEFINED CMSE_FLAGS)
    embedded_set_target_compile_flags(TARGET lib_mtk_drv LANGUAGE C APPEND FLAGS ${CMSE_FLAGS})
endif()

#Set common compiler and linker flags
config_setting_shared_compiler_flags(lib_mtk_drv)
config_setting_shared_linker_flags(lib_mtk_drv)
embedded_set_target_compile_flags(TARGET lib_mtk_drv LANGUAGE C APPEND FLAGS -Os)

embedded_project_end(lib_mtk_drv)
