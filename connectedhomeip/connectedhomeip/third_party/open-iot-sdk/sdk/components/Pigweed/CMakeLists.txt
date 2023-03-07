# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(pigweed LANGUAGES C CXX ASM)

# The open IoT SDK has the same limitations as Zephyr regarding warning flags.
# Enable a similar profile for Open IoT SDK.
set(ZEPHYR_PIGWEED_MODULE_DIR ON CACHE STRING "Zephyr compilation mode of pigweed" FORCE)

# Disable warning overrides from Pigweed that don't play well with INTERFACE
# libraries.
set(pw_build_WARNINGS pw_build.empty CACHE STRING "" FORCE)

# To allow multiple logging backend implementations defined in the same build
# tree, the mechanism that checks that the log facade is implemented by a
# backend that bypasses pigweed's empty target.
set(pw_log_BACKEND pw_build.empty CACHE STRING "Backend for pw_log" FORCE)

if(IOTSDK_PIGWEED)
    include(Pigweed-fetch.cmake)
endif()

# include iot sdk backend implementations
add_subdirectory(pw_log_mcu_driver_hal)
