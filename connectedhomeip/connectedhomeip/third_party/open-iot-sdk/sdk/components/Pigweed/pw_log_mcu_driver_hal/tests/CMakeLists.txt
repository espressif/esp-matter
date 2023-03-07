# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_executable(pw_log_mdh_backend-test
    test_pw_log_mdh_backend.cpp
    mocks/src/serial_api.c
    mocks/src/util.c
    ../pw_log_mdh_backend.c
)

target_include_directories(pw_log_mdh_backend-test
    PRIVATE
        mocks/inc
)


target_link_libraries(pw_log_mdh_backend-test
    PRIVATE
        fff
        project_options
        project_warnings
)

iotsdk_add_test(pw_log_mdh_backend-test)
