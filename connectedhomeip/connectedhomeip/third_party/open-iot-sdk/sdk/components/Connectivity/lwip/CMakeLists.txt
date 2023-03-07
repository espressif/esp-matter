# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(lwip)

# populates lwIP dir
include(lwip-fetch.cmake)

# Set proper includes for lwipcore target
set(LWIP_DIR ${lwip_SOURCE_DIR})
target_include_directories(lwipcore
    PUBLIC
        ${lwip_SOURCE_DIR}/src/include
)

add_subdirectory(lwip-cmsis-port)
