# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

# Fetch toolchain files before the first call to project
include(FetchContent)
FetchContent_Declare(iotsdk-toolchains
    GIT_REPOSITORY  https://git.gitlab.arm.com/iot/open-iot-sdk/toolchain.git
    GIT_TAG         11b6667b7e3b0b70e52b140f910e2b4f9f4bb039
    SOURCE_DIR      ${CMAKE_BINARY_DIR}/toolchains
)
FetchContent_MakeAvailable(iotsdk-toolchains)

project(iotsdk-storage C CXX ASM)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXECUTABLE_SUFFIX .elf)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    add_subdirectory(extern)
    option(IOTSDK_STORAGE_BUILD_EXAMPLES "Build storage example applications")
    if(IOTSDK_STORAGE_BUILD_EXAMPLES)
        add_subdirectory(${PROJECT_SOURCE_DIR}/common/utils) # for serial_printf()
    endif()
    set(HTRUN_ARGUMENTS "" CACHE STRING "Arguments passed to htrun when running integration tests")
    set(HTRUN_IMAGE_EXT ".elf" CACHE STRING "Image format for htrun when running integration tests")

    # Enable features of project_options.
    # Note: We need to enable project_options after adding fetched components
    # and before adding local components, to ensure options are only applied to
    # local components.
    FetchContent_Declare(project_options
        GIT_REPOSITORY  https://github.com/cpp-best-practices/project_options.git
        GIT_TAG         v0.21.0
    )
    FetchContent_MakeAvailable(project_options)
    include(${project_options_SOURCE_DIR}/Index.cmake)
    include(${project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

    set(DOXYGEN_EXCLUDE_PATTERNS
        */_deps/*
        */extern/*
        */toolchains/*
        */examples/*
        */tests/*
    )
    # cppcheck will be run by pre-commit checks
    set(ENABLE_CPPCHECK_DEVELOPER_DEFAULT OFF)
    set(ENABLE_CLANG_TIDY_DEVELOPER_DEFAULT OFF)
    set(GCC_WARNINGS -Wall -Wextra -Wpedantic)
    set(CLANG_WARNINGS ${GCC_WARNINGS})

    # Enable project_options only work not cross-compiling.
    if(NOT CMAKE_CROSSCOMPILING)
        if(BUILD_TESTING)
            # see https://google.github.io/googletest/gmock_cook_book.html#NiceStrictNaggy
            option(GMOCK_WARNINGS "Enable gMock warnings for uninteresting calls" OFF)
        endif()

        set(ENABLE_COVERAGE_DEVELOPER_DEFAULT ON)
        set(ENABLE_INCLUDE_WHAT_YOU_USE_DEVELOPER_DEFAULT ON)
        set(ENABLE_CLANG_TIDY_DEVELOPER_DEFAULT ON)
        set(ENABLE_SANITIZER_ADDRESS_DEVELOPER_DEFAULT ON)
        set(ENABLE_SANITIZER_LEAK_DEVELOPER_DEFAULT OFF)
        set(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR_DEVELOPER_DEFAULT ON)
    endif()
    dynamic_project_options(
        CLANG_WARNINGS
        ${CLANG_WARNINGS}
        GCC_WARNINGS
        ${GCC_WARNINGS}
        )
    if (OPT_ENABLE_COVERAGE AND NOT CMAKE_CROSSCOMPILING)
        include(CodeCoverageReport)
    endif()

    # If include-what-you-use is being used, provide a map file to teach
    # include-what-you-use about private headers, so that it doesn't recommend
    # them in place of public headers.
    if (OPT_ENABLE_INCLUDE_WHAT_YOU_USE)
        list(APPEND CMAKE_CXX_INCLUDE_WHAT_YOU_USE
            -Xiwyu;--mapping_file=${PROJECT_SOURCE_DIR}/cmake/iwyu.map
        )
    endif()
endif()

add_subdirectory(blockdevice)
add_subdirectory(kvstore)
