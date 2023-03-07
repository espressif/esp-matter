# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-sdk_POPULATED)
    if(NOT TARGET cmsis-rtos-api)
        message(FATAL_ERROR "The NTP client requires CMSIS RTOS API component")
    endif()

    if(NOT TARGET cmsis-sockets-api)
        message(FATAL_ERROR "The NTP client requires CMSIS SOCKET API component")
    endif()

    if(NOT TARGET iotsdk-ip-network-api)
        message(FATAL_ERROR "The NTP client requires IoT SDK IP Network API component")
    endif()

    if(NOT TARGET azure-c-shared-utility)
        message(FATAL_ERROR "The NTP client requires Azure C shared utility")
    endif()

    # NTP client library.
    add_library(ntp-client
        ${CMAKE_CURRENT_SOURCE_DIR}/src/iot_ntp_client.c
    )

    target_include_directories(ntp-client
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(ntp-client
            iotsdk-ip-network-api
            cmsis-sockets-api
            cmsis-rtos-api
            azure-c-shared-utility
    )
endif()
