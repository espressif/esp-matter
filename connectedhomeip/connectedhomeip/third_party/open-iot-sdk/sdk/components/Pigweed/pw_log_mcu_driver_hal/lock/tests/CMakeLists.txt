# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(cmsis_rtos_lock-test
    test_cmsis_rtos_lock.cpp
    mocks/src/cmsis_os2.c
    ../cmsis_rtos_lock.c
)

target_include_directories(cmsis_rtos_lock-test
    PRIVATE
        mocks/inc
)

target_link_libraries(cmsis_rtos_lock-test
    PRIVATE
        fff
        project_options
        project_warnings
)

iotsdk_add_test(cmsis_rtos_lock-test)
