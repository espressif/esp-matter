#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include("Common/BuildSys")

if (TFM_TEST_CASE_ENABLE STREQUAL "y")
    set (TFM_CONFIG_FILE "ConfigRegression.cmake")
else ()
    set (TFM_CONFIG_FILE "ConfigDefault.cmake")
endif ()

if (TFM_BUILD_MODE STREQUAL "debug")
    set(TFM_BUILD_MODE_DEBUG ON)
elseif (NOT TFM_BUILD_MODE)
    set(TFM_BUILD_MODE_NORMAL ON)
endif ()

add_subdirectory(secure_fw)
add_subdirectory(app)
if (BL2)
#    add_subdirectory(bl2/ext/mcuboot)
endif()

#Define a top-level generic tfm project
project(tfm LANGUAGES)
