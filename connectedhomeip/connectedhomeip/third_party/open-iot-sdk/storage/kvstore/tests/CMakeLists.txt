# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

add_executable(tdbstore-tests test_tdbstore.cpp)

target_include_directories(tdbstore-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/common/include
)

target_link_libraries(tdbstore-tests
    PRIVATE
        iotsdk-tdbstore
        iotsdk-blockdevice
        iotsdk-blockdevice-mock
        gtest_main
        gmock_main
        project_options
)

if(GMOCK_WARNINGS)
    target_compile_definitions(tdbstore-tests PRIVATE GMOCK_WARNINGS)
endif(GMOCK_WARNINGS)

gtest_discover_tests(tdbstore-tests)
