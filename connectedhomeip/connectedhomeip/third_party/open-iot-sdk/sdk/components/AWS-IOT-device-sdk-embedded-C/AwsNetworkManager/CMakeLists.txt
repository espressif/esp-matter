# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if (NOT TARGET mbedtls OR NOT TARGET pw_log OR NOT TARGET cmsis-sockets-api)
    return()
endif()

add_library(iot-sdk-aws-network-manager)

target_include_directories(iot-sdk-aws-network-manager
    PUBLIC
        include
)

target_sources(iot-sdk-aws-network-manager
    PRIVATE
        AwsNetworkManager.c
)

target_link_libraries(iot-sdk-aws-network-manager
    PUBLIC
        backoffAlgorithm
        mbedtls
        cmsis-sockets-api
        pw_log
)
