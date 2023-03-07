/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mps3_uart.h"

#include "hal/serial_api.h"

mdh_serial_t *get_example_serial()
{
    mps3_uart_t *uart;

    mps3_uart_init(&uart, &UART0_CMSDK_DEV_NS);

    return &(uart->serial);
}
