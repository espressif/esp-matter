/*
 * Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/serial_api.h"

#include <stdarg.h>
#include <stdio.h>

static mdh_serial_t *serial_obj = NULL;

extern mdh_serial_t *get_example_serial();

int iotsdk_serial_setup()
{
    if (serial_obj != NULL) {
        return 0;
    }

    serial_obj = get_example_serial();
    if (serial_obj == NULL) {
        return -1;
    }

    mdh_serial_set_baud(serial_obj, 115200);

    return 0;
}

int iotsdk_serial_out(const char *str, size_t len)
{
    if (serial_obj == NULL) {
        return -1;
    }

    if (str == NULL || len == 0) {
        return -1;
    }

    size_t written = 0;
    while (written++ < len) {
        mdh_serial_put_data(serial_obj, *str++);
    }

    return len;
}

int iotsdk_serial_in(char *str, size_t len)
{
    if (serial_obj == NULL) {
        return -1;
    }

    if (str == NULL || len == 0) {
        return -1;
    }

    if (!mdh_serial_is_readable(serial_obj)) {
        return 0;
    }

    size_t read = 0;
    while (read++ < len) {
        *str++ = (char)mdh_serial_get_data(serial_obj);
    }

    return len;
}
