# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(iotsdk-ip-network-api)

add_library(iotsdk-ip-network-api INTERFACE)

target_include_directories(iotsdk-ip-network-api
    INTERFACE
        include
)

target_sources(iotsdk-ip-network-api
    PRIVATE
        include/iotsdk/ip_network_api.h
)

target_link_libraries(iotsdk-ip-network-api
    INTERFACE
        cmsis-core
)

# because we don't have source we need to set the language manually
set_target_properties(iotsdk-ip-network-api PROPERTIES LINKER_LANGUAGE C)
