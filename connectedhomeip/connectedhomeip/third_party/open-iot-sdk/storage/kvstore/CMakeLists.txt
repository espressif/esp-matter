# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Tiny Database Storage (TDBStore)
add_library(iotsdk-tdbstore
    EXCLUDE_FROM_ALL
        source/TDBStore.cpp
        ${PROJECT_SOURCE_DIR}/common/source/crc.c
)

target_include_directories(iotsdk-tdbstore
    PUBLIC
        include
        ${PROJECT_SOURCE_DIR}/blockdevice/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/common/include
)
target_link_libraries(iotsdk-tdbstore PRIVATE iotsdk-blockdevice)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
        target_link_libraries(iotsdk-tdbstore
            PRIVATE
                project_options
                project_warnings
        )
        add_subdirectory(tests)
    endif()
    if(IOTSDK_STORAGE_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()
