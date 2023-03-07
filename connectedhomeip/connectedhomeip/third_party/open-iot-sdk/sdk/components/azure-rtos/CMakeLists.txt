# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Note: We must set `THREADX_ARCH` and `THREADX_TOOLCHAIN` before fetching
# Azure projects, in order for Azure projects to select which subdirectory
# paths to use during CMake configuration.

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(THREADX_TOOLCHAIN "gnu")
elseif(CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
    set(THREADX_TOOLCHAIN "ac6")
else()
    message(FATAL_ERROR "Unsupported compiler/toolchain: ${CMAKE_C_COMPILER_ID}")
endif()

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m0)
    set(THREADX_ARCH "cortex_m0")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m3)
    set(THREADX_ARCH "cortex_m3")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m4)
    set(THREADX_ARCH "cortex_m4")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m7)
    set(THREADX_ARCH "cortex_m7")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m23)
    set(THREADX_ARCH "cortex_m23")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m33)
    set(THREADX_ARCH "cortex_m33")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m55)
    set(THREADX_ARCH "cortex_m55")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL cortex-m85)
    set(THREADX_ARCH "cortex_m85")
else()
    message("No port found for processor: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

add_subdirectory(ThreadX)
