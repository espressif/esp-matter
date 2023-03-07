# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project(iotsdk-templates LANGUAGES NONE)

set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/../__codegen CACHE PATH
    "Directory of generated examples and tests. Absolute or relative to where `cmake` is invoked.")
set(EXAMPLES_OUTPUT_DIR ${OUTPUT_DIR}/examples)
set(TESTS_OUTPUT_DIR ${OUTPUT_DIR}/tests)

# Parameters for fetching toolchain files
set(TOOLCHAINS_URL https://git.gitlab.arm.com/iot/open-iot-sdk/toolchain.git)
set(TOOLCHAINS_TAG 6133b1de96ca069bcdd2e74876e62a338bf5c928)

# Parameters for fetching the SDK
set(IOTSDK_URL https://git.gitlab.arm.com/iot/open-iot-sdk/sdk.git)
execute_process(
    COMMAND
        git rev-parse HEAD
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE
        IOTSDK_TAG
    COMMAND_ERROR_IS_FATAL
        ANY
)

# Parameter for fetching project_options
set(PROJECT_OPTIONS_URL https://github.com/cpp-best-practices/project_options.git)
set(PROJECT_OPTIONS_TAG v0.21.0)

# Common linker script templates
set(LINKER_SCRIPT_GCC ${PROJECT_SOURCE_DIR}/common/gcc.ld.in)
set(LINKER_SCRIPT_ARM ${PROJECT_SOURCE_DIR}/common/armclang.sct.in)

# Common htrun arguments needed for all examples and platforms
set(HTRUN_COMMON_ARGUMENTS "--sync=0 --baud-rate=115200 --polling-timeout=120")

include(rtoses.cmake)
include(platforms.cmake)

set(README_TEMPLATE_FILE ${PROJECT_SOURCE_DIR}/common/README.md.in)

add_subdirectory(examples)
add_subdirectory(tests)
