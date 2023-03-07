# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(cmsis-sockets-api)

include(IoT-Socket-fetch.cmake)

add_library(cmsis-sockets-api INTERFACE)

target_include_directories(cmsis-sockets-api
    INTERFACE
        ${cmsis-sockets-api_SOURCE_DIR}/include
        include
)

target_sources(cmsis-sockets-api
    PRIVATE
        ${cmsis-sockets-api_SOURCE_DIR}/include/iot_socket.h
)

# because we don't have sources we need to set the language manually
set_target_properties(cmsis-sockets-api PROPERTIES LINKER_LANGUAGE C)

# Include SDK implementation based on LwIP
add_subdirectory(lwip-sockets)
