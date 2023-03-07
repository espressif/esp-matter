# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(lwip-sockets)

# Do not expose library if dependencies aren't available.
# Testing the presence of lwip-cmsis-sys is sufficient as it depends on other
# LwIP components.
if (NOT TARGET lwip-cmsis-sys)
    return()
endif()

add_library(lwip-sockets)

target_sources(lwip-sockets
    PRIVATE
        ${cmsis-sockets-api_SOURCE_DIR}/source/lwip/iot_socket.c
)

target_link_libraries(lwip-sockets
    PRIVATE
        cmsis-sockets-api
        lwipcore
        lwip-cmsis-sys
        lwipopts
)
