# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
include(FetchContent)

if (BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    FetchContent_Declare(
        mcu-driver-hal
        GIT_REPOSITORY https://git.gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-hal.git
        GIT_TAG        27d60312913cf2efd98d02c30dacb979c7a89aee
        GIT_PROGRESS   ON
    )
    FetchContent_MakeAvailable(mcu-driver-hal)
    if(IS_DIRECTORY "${mcu-driver-hal_SOURCE_DIR}")
        set_property(DIRECTORY ${mcu-driver-hal_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()

    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_SHALLOW    ON
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(fff
        GIT_REPOSITORY https://github.com/meekrosoft/fff.git
        GIT_SHALLOW    ON
        GIT_TAG        v1.1
    )
    FetchContent_GetProperties(fff)
    FetchContent_MakeAvailable(fff)
    configure_file(${fff_SOURCE_DIR}/fff.h fff.h COPYONLY)
    add_library(fff INTERFACE)
    target_include_directories(fff SYSTEM INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
endif ()

if(IOTSDK_MDH_ARM)
    FetchContent_Declare(
        mcu-driver-reference-platforms-for-arm
        GIT_REPOSITORY https://git.gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-reference-platforms-for-arm.git
        GIT_TAG        72138f045f98075cb0f3c81498f551d79edb18ea
        GIT_PROGRESS   ON
    )

    FetchContent_MakeAvailable(mcu-driver-reference-platforms-for-arm)
endif()
