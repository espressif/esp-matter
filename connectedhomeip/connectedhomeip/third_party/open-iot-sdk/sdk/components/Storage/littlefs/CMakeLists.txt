# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(littlefs LANGUAGES C)

include(littlefs-fetch.cmake)

add_library(littlefs
    ${littlefs_SOURCE_DIR}/lfs.c
    ${littlefs_SOURCE_DIR}/lfs_util.c
)

target_include_directories(littlefs
    PUBLIC
        ${littlefs_SOURCE_DIR}
)
