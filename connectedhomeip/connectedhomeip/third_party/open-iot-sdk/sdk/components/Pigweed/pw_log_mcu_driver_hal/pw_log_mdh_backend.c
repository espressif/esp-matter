/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/serial_api.h"
#include "pw_log_mdh/config.h"
#include "pw_log_mdh/util.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Implementation notes: This first version uses the serial_putc function to
// output the trace. It is inefficient as it blocks the thread until the log
// has been transmitted over serial.
// A first improvement would be to take advantage of the async serial API or
// using a logging thread to output logs.

// Singleton used as the serial output
static mdh_serial_t *log_serial = NULL;

// Assign the output to the logger
void pw_log_mdh_init(mdh_serial_t *serial)
{
    log_serial = serial;
    _pw_log_init_lock();
}

// Output the message from PW_LOG
void _pw_log_mdh(const char *message, ...)
{
    char buffer[PW_LOG_MDH_LINE_LENGTH];

    va_list args;
    va_start(args, message);
    int result = vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    if (result < 0) {
        return;
    }

    char *ptr = buffer;

    _pw_log_lock();
    while (*ptr != '\0') {
        mdh_serial_put_data(log_serial, *ptr++);
    }
    _pw_log_unlock();
}
