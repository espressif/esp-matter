# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

if(IOTSDK_MDH_ARM)
    FetchContent_Declare(
        mcu-driver-reference-platforms-for-arm
        GIT_REPOSITORY https://git.gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-reference-platforms-for-arm.git
        GIT_TAG        e3f72d831d91fbe53f6f14371b5ca6a9d903073a
        GIT_PROGRESS   ON
    )

    FetchContent_MakeAvailable(mcu-driver-reference-platforms-for-arm)
endif()
