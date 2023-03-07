# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# IoT-SDK

cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project(open-iot-sdk LANGUAGES C CXX ASM)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

option(IOTSDK_CMSIS_RTOS_API "Fetch CMSIS RTOS API")
option(IOTSDK_CMSIS_RTX "Fetch CMSIS RTX")
option(IOTSDK_FREERTOS "Fetch FreeRTOS")
option(IOTSDK_THREADX "Fetch ThreadX")
option(IOTSDK_THREADX_CDI_PORT "Fetch CMSIS-RTOSv2 adaptation layer for ThreadX")
option(IOTSDK_TFM "Fetch Trusted Firmware-M")
option(IOTSDK_ML "Fetch ML Embedded Evaluation Kit")
option(IOTSDK_MDH_ARM "Fetch MCU-Driver-HAL for Arm platforms")
option(IOTSDK_LITTLEFS "Fetch littlefs")
option(IOTSDK_TINYUSB "Fetch TinyUSB")
option(IOTSDK_NIMBLE "Fetch NimBLE")
option(IOTSDK_LWIP "Fetch lwIP")
option(IOTSDK_PIGWEED "Fetch Pigweed")
option(IOTSDK_MBEDTLS "Fetch Mbed TLS")
option(IOTSDK_AWS_CLIENT "Fetch AWS client")
option(IOTSDK_AWS_CLIENT_EXAMPLE "Build AWS client example")
option(IOTSDK_AZURE_SDK "Fetch Azure SDK")

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    include(TestTools)
endif()

if(IOTSDK_TFM)
    set(TFM_PLATFORM "" CACHE STRING "Value of -DTFM_PLATFORM= passed to TF-M")
    option(TFM_LIB_MODEL "Use secure library mode instead of IPC mode of TF-M")
    option(TFM_PARTITION_FIRMWARE_UPDATE "Enable PSA Firmware Update API in TF-M")
    set(MCUBOOT_IMAGE_VERSION_NS "0.0.0" CACHE STRING "Version number of non-secure image (in the x.x.x format)")
    option(CONFIG_TFM_ENABLE_FPU "Make FPU operational when SPE and/or NSPE require FPU usage.")
    set(TFM_CMAKE_ARGS "" CACHE STRING "Additional CMake arguments passed to TF-M (semicolon-separated)")
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/components/TF-M)
    set(TFM_PATCH_COMMAND "" CACHE STRING "Command to patch TF-M")
endif()

if(IOTSDK_ML)
    set(ML_PATCH_COMMAND "" CACHE STRING "Command to patch ML Embedded Evaluation Kit")
    set(ML_TARGETS "all" CACHE STRING "Targets to build from the ML Embedded Evaluation Kit")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Enable features of project_options.
    # Note: We need to enable project_options after adding fetched components
    # and before adding local components, to ensure options are only applied to
    # local components.
    FetchContent_Declare(
        project_options
        GIT_REPOSITORY  https://github.com/cpp-best-practices/project_options.git
        GIT_TAG         v0.21.0
    )
    FetchContent_MakeAvailable(project_options)
    include(${project_options_SOURCE_DIR}/Index.cmake)
    include(${project_options_SOURCE_DIR}/src/DynamicProjectOptions.cmake)

    # cppcheck will be run by pre-commit checks
    set(ENABLE_CPPCHECK_DEVELOPER_DEFAULT OFF)

    include(warning_flags)
    set(GCC_WARNINGS ${IOTSDK_WARNING_FLAGS} ${IOTSDK_WARNING_AS_ERRORS_FLAGS})
    set(CLANG_WARNINGS ${GCC_WARNINGS})

    dynamic_project_options(
        CLANG_WARNINGS
        ${CLANG_WARNINGS}
        GCC_WARNINGS
        ${GCC_WARNINGS}
    )
    if (OPT_ENABLE_COVERAGE AND NOT CMAKE_CROSSCOMPILING)
        include(CodeCoverageReport)
    endif()
endif()

add_subdirectory(utils)

if(CMAKE_CROSSCOMPILING)
    add_subdirectory(components EXCLUDE_FROM_ALL)
else()
    add_subdirectory(components)
endif()
