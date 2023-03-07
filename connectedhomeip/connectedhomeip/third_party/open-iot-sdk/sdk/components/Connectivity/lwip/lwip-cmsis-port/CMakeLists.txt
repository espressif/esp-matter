# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(lwip-cmsis-port)

add_subdirectory(lwip_sys)

# lwipopts library is used to supply configuration for lwIP

add_library(lwipopts INTERFACE)

# IMPORTANT: user applications must provide a "user_lwipopts.h" file which can be an empty
# file or contain lwIP options the user wants to override. This can be done by adding
# an include directory to lwipopts that contains the file.

target_include_directories(lwipopts
    INTERFACE
        include/config
)

add_library(lwip-cmsis-port EXCLUDE_FROM_ALL)

target_include_directories(lwip-cmsis-port
    PRIVATE
        include
)

target_sources(lwip-cmsis-port
    PRIVATE
        src/ip_network_api.c
        src/lwip_stack.c
        src/lwip_emac_netif.c
        src/lwip_memory_manager.c
)

target_link_libraries(lwip-cmsis-port
    PRIVATE
        mcu-driver-hal
        lwipcore
        lwip-cmsis-sys
        iotsdk-ip-network-api
    PUBLIC
        lwipopts
        lwip-cmsis-sys
)

add_library(lwip-cmsis-port-low-input-latency INTERFACE)

target_compile_definitions(lwip-cmsis-port-low-input-latency
    INTERFACE
        IOTSDK_LWIP_LOW_INPUT_LATENCY
        IOTSDK_LWIP_INPUT_TASK_THREAD_PRIORITY=osPriorityRealtime
        IOTSDK_LWIP_TCPIP_THREAD_PRIORITY=osPriorityHigh
)
