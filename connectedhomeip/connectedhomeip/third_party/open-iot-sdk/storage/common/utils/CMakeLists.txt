# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(storage-serial-printf
    serial_printf.c
    $<$<TARGET_EXISTS:mdh-arm-corstone-300-common>:platforms/mps3_uart.c>
)
target_include_directories(storage-serial-printf PUBLIC include)
target_link_libraries(storage-serial-printf PRIVATE mcu-driver-hal)
