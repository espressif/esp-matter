# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(CMAKE_CROSSCOMPILING)
    add_library(iotsdk-serial-retarget OBJECT
        serial_retarget.c
        $<$<STREQUAL:${CMAKE_C_COMPILER_ID},GNU>:toolchains/gcc_retarget.c>
        $<$<STREQUAL:${CMAKE_C_COMPILER_ID},ARMClang>:toolchains/armclang_retarget.c>
        $<$<TARGET_EXISTS:mdh-arm-an552-mps3>:platforms/mps3_uart.c>
    )
    target_link_libraries(iotsdk-serial-retarget PRIVATE
        mcu-driver-hal

        # Apply project options/warnings when the SDK is at the top level
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_options>
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_warnings>
    )

    add_library(iotsdk-emac-factory
        $<$<TARGET_EXISTS:mdh-arm-an552-mps3>:platforms/mps3_emac.c>
    )
    target_include_directories(iotsdk-emac-factory PUBLIC .)
    target_link_libraries(iotsdk-emac-factory PUBLIC
        mcu-driver-hal

        # Apply project options/warnings when the SDK is at the top level
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_options>
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_warnings>
    )

    add_library(iotsdk-alloc iotsdk_alloc.c)
    target_include_directories(iotsdk-alloc PUBLIC .)
    target_link_libraries(iotsdk-alloc PUBLIC
        # Apply project options/warnings when the SDK is at the top level
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_options>
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_warnings>
    )

    add_library(iotsdk-memory-manager iotsdk_memory_manager.c)
    target_include_directories(iotsdk-memory-manager PUBLIC .)
    target_link_libraries(iotsdk-memory-manager PUBLIC
        mcu-driver-hal

        iotsdk-alloc

        # Apply project options/warnings when the SDK is at the top level
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_options>
        $<$<STREQUAL:${CMAKE_PROJECT_NAME},${PROJECT_NAME}>:project_warnings>
    )
endif()

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
endif()
