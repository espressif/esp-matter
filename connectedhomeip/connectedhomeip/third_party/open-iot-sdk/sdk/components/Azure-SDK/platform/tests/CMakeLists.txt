# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(ntptime_openiot-test
    test_ntptime_openiot.cpp
    mocks/src/iot_ntp_client.c
    ../src/ntptime_openiot.c
)

target_include_directories(ntptime_openiot-test
    PRIVATE
        mocks/inc
)

target_link_libraries(ntptime_openiot-test
    PRIVATE
        fff
        project_options
        project_warnings
)

iotsdk_add_test(ntptime_openiot-test)

add_executable(tickcounter_openiot-test
    test_tickcounter_openiot.cpp
    mocks/src/iotsdk_alloc.c
    mocks/src/xlogging.c
    mocks/src/cmsis_os2.c
    ../src/tickcounter_openiot.c
)

target_include_directories(tickcounter_openiot-test
    PRIVATE
        mocks/inc
)

target_link_libraries(tickcounter_openiot-test
    PRIVATE
        fff
        project_options
        project_warnings
)

iotsdk_add_test(tickcounter_openiot-test)
