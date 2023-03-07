# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(OPT_ENABLE_CPPCHECK)
    # Disable check for "member function can be static" as it raises a false
    # positive in older versions of cppcheck.
    list(APPEND CMAKE_CXX_CPPCHECK --suppress=functionStatic)
endif()

add_library(iotsdk-cdi-blockdevice-experimental EXCLUDE_FROM_ALL
    experimental/source/flashiap_block_device.c
    experimental/source/CentauriBlockDevice.cpp
    experimental/source/FlashIAPBlockDevice.cpp
)
target_include_directories(iotsdk-cdi-blockdevice-experimental PUBLIC experimental/include include)
target_link_libraries(iotsdk-cdi-blockdevice-experimental PUBLIC mcu-driver-hal)

add_library(iotsdk-blockdevice EXCLUDE_FROM_ALL
    source/FlashIAPBlockDevice.cpp
    source/BufferedBlockDevice.cpp
)
target_include_directories(iotsdk-blockdevice
    PUBLIC
        include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/common/include
)
target_link_libraries(iotsdk-blockdevice PUBLIC mcu-driver-hal)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
        target_link_libraries(iotsdk-blockdevice PRIVATE project_options project_warnings)
        target_link_libraries(iotsdk-cdi-blockdevice-experimental PRIVATE project_options project_warnings)
        add_subdirectory(tests)
    endif()
    if(IOTSDK_STORAGE_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()
