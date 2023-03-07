/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/unistd.h>

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

extern int iotsdk_serial_setup();
extern int iotsdk_serial_out(const char *str, size_t len);
extern int iotsdk_serial_in(char *str, size_t len);
static int serial_up = 0;

int _write(int fd, const char *str, size_t len)
{
    if (fd != STDOUT_FILENO && fd != STDERR_FILENO) {
        return -1;
    }

    if (!serial_up) {
        if (iotsdk_serial_setup() != 0) {
            return -1;
        }
        serial_up = 1;
    }

    return iotsdk_serial_out(str, len);
}

int _read(int fd, char *str, size_t len)
{
    if (fd != STDIN_FILENO) {
        return -1;
    }

    if (!serial_up) {
        if (iotsdk_serial_setup() != 0) {
            return -1;
        }
        serial_up = 1;
    }

    return iotsdk_serial_in(str, len);
}
