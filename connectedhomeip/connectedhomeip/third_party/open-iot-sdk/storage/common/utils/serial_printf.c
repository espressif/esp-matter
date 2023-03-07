/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdarg.h>

#include "serial_printf.h"
#include "hal/serial_api.h"

static mdh_serial_t *my_serial = NULL;

extern mdh_serial_t *get_example_serial();

static void serial_setup()
{
    my_serial = get_example_serial();
    mdh_serial_set_baud(my_serial, 115200);
}

void serial_printf(const char *fmt, ...)
{
    static int inited = 0;
    if (!inited) {
        serial_setup();
        inited = 1;
    }

    static char str[IOTSDK_SERIAL_PRINTF_MAX_SIZE];
    va_list args;
    va_start(args, fmt);
    if (vsnprintf(str, sizeof(str), fmt, args) < 0) {
        // string formatting error
        va_end(args);
        return;
    }

    char *ptr = str;
    while (*ptr != '\0') {
        mdh_serial_put_data(my_serial, *ptr++);
    }

    va_end(args);
}
