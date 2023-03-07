# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tests)
elseif(pigweed_POPULATED)
    include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

    pw_add_module_config(pw_log_mcu_driver_hal_CONFIG)

    pw_auto_add_simple_module(pw_log_mcu_driver_hal
        IMPLEMENTS_FACADE
            pw_log
        PUBLIC_DEPS
            ${pw_log_mcu_driver_hal_CONFIG}
            mcu-driver-hal
            pw_preprocessor
    )
endif()

# Build locks supported by the backend.
add_subdirectory(lock)
