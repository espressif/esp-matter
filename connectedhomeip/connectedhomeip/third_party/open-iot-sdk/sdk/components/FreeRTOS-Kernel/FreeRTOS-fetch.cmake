# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

FetchContent_Declare(
    FreeRTOS-Kernel
    GIT_REPOSITORY  https://github.com/ARM-software/CMSIS-FreeRTOS.git
    GIT_TAG         b3cc689f417b2aff506a75ac15bfe4dbbecfaa50
    GIT_SHALLOW     OFF
    GIT_PROGRESS    ON
)

FetchContent_MakeAvailable(FreeRTOS-Kernel)
FetchContent_GetProperties(FreeRTOS-Kernel)
