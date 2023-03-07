# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(iot-sdk-emac-memory-manager-test
    test_iotsdk_memory_manager.cpp
    mocks/src/iotsdk_alloc.c
    ../iotsdk_memory_manager.c
)

target_include_directories(iot-sdk-emac-memory-manager-test
    PRIVATE
        mocks/inc
        ..
)

target_link_libraries(iot-sdk-emac-memory-manager-test
    PRIVATE
        mcu-driver-hal
        fff
        project_options
        project_warnings
)

iotsdk_add_test(iot-sdk-emac-memory-manager-test)
