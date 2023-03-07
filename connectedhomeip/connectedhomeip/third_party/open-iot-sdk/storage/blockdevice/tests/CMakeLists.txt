# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

include(GoogleTest)

add_library(iotsdk-blockdevice-mock INTERFACE)
target_include_directories(iotsdk-blockdevice-mock INTERFACE include)


add_executable(flashiap-block-device-tests test_flashiapblockdev.cpp)
target_link_libraries(flashiap-block-device-tests PRIVATE iotsdk-blockdevice gtest_main fff)
target_link_libraries(flashiap-block-device-tests PRIVATE project_options)

gtest_discover_tests(flashiap-block-device-tests)


add_executable(flashiap-block-device-experimental-tests test_flashiapblockdev.cpp)
target_link_libraries(flashiap-block-device-experimental-tests
    PRIVATE
    iotsdk-cdi-blockdevice-experimental
    mcu-driver-hal
    gtest_main
    fff
    project_options
)

gtest_discover_tests(flashiap-block-device-experimental-tests)


add_executable(buffered-block-device-tests test_bufferedblockdev.cpp)
target_include_directories(buffered-block-device-tests PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(buffered-block-device-tests
    PRIVATE
    iotsdk-blockdevice
    project_options
)

# Mark googletest libraries' include directories as SYSTEM, so that clang-tidy
# will not warn about issues in googletest that don't comply with our
# clang-tidy configuration.
get_target_property(gtest_include gtest_main INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(gmock_include gmock_main INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(buffered-block-device-tests SYSTEM
    PRIVATE
    ${gtest_include}
    ${gmock_include}
)
target_link_libraries(buffered-block-device-tests
    PRIVATE
    gtest_main
    gmock_main
)

if(GMOCK_WARNINGS)
    target_compile_definitions(buffered-block-device-tests PRIVATE GMOCK_WARNINGS)
endif(GMOCK_WARNINGS)

gtest_discover_tests(buffered-block-device-tests)
