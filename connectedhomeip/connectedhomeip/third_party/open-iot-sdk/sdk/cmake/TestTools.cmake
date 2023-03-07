# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Add libraries useful for unit testing.

include(FetchContent)

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    FetchContent_Declare(
        mcu-driver-hal
        GIT_REPOSITORY https://git.gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-hal.git
        GIT_TAG        baa3882c7b7bccd9691f347672d96bb8294024ba
        GIT_PROGRESS   ON
    )
    FetchContent_MakeAvailable(mcu-driver-hal)
    if(IS_DIRECTORY "${mcu-driver-hal_SOURCE_DIR}")
        set_property(DIRECTORY ${mcu-driver-hal_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
    endif()

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_SHALLOW    ON
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(googletest)

    FetchContent_Declare(
        fff
        GIT_REPOSITORY https://github.com/meekrosoft/fff.git
        GIT_SHALLOW    ON
        GIT_TAG        v1.1
    )
    FetchContent_GetProperties(fff)
    FetchContent_MakeAvailable(fff)
    configure_file(${fff_SOURCE_DIR}/fff.h fff.h COPYONLY)
    add_library(fff INTERFACE)
    target_include_directories(fff SYSTEM INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

    include(GoogleTest)

    # Mark googletest libraries' include directories as SYSTEM, so that clang-tidy
    # will not warn about issues in googletest that don't comply with our
    # clang-tidy configuration.
    get_target_property(gtest_include gtest_main INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(gmock_include gmock_main INTERFACE_INCLUDE_DIRECTORIES)

    function(iotsdk_add_test target)
        target_include_directories(${target} SYSTEM
            PRIVATE
                ${gtest_include}
                ${gmock_include}
        )
        target_link_libraries(${target}
            PRIVATE
                gtest_main
                gmock_main
        )

        gtest_discover_tests(${target})
    endfunction()
endif()
