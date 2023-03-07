# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(IOTSDK_AZURE_SDK)
    include(azure-sdk-fetch.cmake)
endif()

option(IOTSDK_AZURE_SDK_LOGGING "Enable Azure SDK logging" OFF)

add_subdirectory(c-utility)
add_subdirectory(ntp-client)
add_subdirectory(platform)
add_subdirectory(macro-utils)
add_subdirectory(parson)
add_subdirectory(uhttp)
add_subdirectory(uamqp-client)
add_subdirectory(umqtt-client)
add_subdirectory(iothub-client)
add_subdirectory(iothub-provisioning-client)

if (TARGET azure-iot-sdk-platform)
    if (NOT ${IOTSDK_AZURE_SDK_LOGGING})
        target_compile_definitions(azure-iot-sdk-platform PUBLIC -DNO_LOGGING)
    endif()
endif()
