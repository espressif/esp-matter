# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
elseif(azure-sdk_POPULATED)
    if(NOT TARGET cmsis-rtos-api)
        message(FATAL_ERROR "The Azure IoT SDK adapter requires CMSIS RTOS API component")
    endif()

    if(NOT TARGET cmsis-sockets-api)
        message(FATAL_ERROR "The Azure IoT SDK adapter requires CMSIS SOCKET API component")
    endif()

    if(NOT TARGET lwip-sockets)
        message(FATAL_ERROR "The Azure IoT SDK adapter requires LwIP sockets component")
    endif()

    if(NOT TARGET iotsdk-ip-network-api)
        message(FATAL_ERROR "The Azure IoT SDK adapter requires IoT SDK IP Network API component")
    endif()

    add_library(azure-iot-sdk-platform
        src/platform_openiot.c
        src/tickcounter_openiot.c
        src/threadapi_openiot.c
        src/socketio_openiot.c
        src/tcpsocketconnection_openiot.c
    )

    target_link_libraries(azure-iot-sdk-platform
        PUBLIC
            azure-c-shared-utility
            iotsdk-ip-network-api
            cmsis-sockets-api
            cmsis-rtos-api
            iotsdk-alloc
    )

    add_library(azure-sdk-ntp-time
        src/ntptime_openiot.c
    )

    target_link_libraries(azure-sdk-ntp-time
        PUBLIC
            ntp-client
    )

    target_compile_definitions(azure-sdk-ntp-time
        PUBLIC
            IOTSDK_AZURE_SDK_NTP_TIME=1
    )
endif()
