# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

if(IOTSDK_THREADX)
    FetchContent_Declare(
        threadx
        GIT_REPOSITORY  https://github.com/azure-rtos/threadx.git
        GIT_TAG         v6.1.12_rel
        GIT_SHALLOW     ON
        GIT_PROGRESS    ON
    )
    FetchContent_MakeAvailable(threadx)
    FetchContent_GetProperties(threadx)
endif()

if(IOTSDK_THREADX_CDI_PORT)
    FetchContent_Declare(
        threadx-cdi-port
        GIT_REPOSITORY  https://git.gitlab.arm.com/iot/open-iot-sdk/threadx-cdi-port.git
        GIT_TAG         v0.1.0
        GIT_SHALLOW     ON
        GIT_PROGRESS    ON
    )
    FetchContent_MakeAvailable(threadx-cdi-port)
endif()
