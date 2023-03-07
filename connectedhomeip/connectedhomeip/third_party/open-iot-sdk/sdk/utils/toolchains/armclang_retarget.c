/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rt_sys.h>
#include <stdint.h>
#include <string.h>

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#if defined(__MICROLIB)
__asm(" .global __use_full_stdio\n");
#endif

static const char stdio_name[] = ":tt";

extern int iotsdk_serial_setup();
extern int iotsdk_serial_out(const char *str, size_t len);
extern int iotsdk_serial_in(char *str, size_t len);

FILEHANDLE _sys_open(const char *name, int openmode)
{
    if (name == NULL) {
        return -1;
    }

    // By default, the Arm Compiler uses the special file path ":tt" for stdin,
    // stdout and stderr and distinguishes between them using openmode. The full
    // libc allows us to redefine the three with different paths, but MicroLib
    // does not support this. For compatibility we rely on openmode. For details,
    // see https://github.com/ARM-software/abi-aa/blob/2022Q1/semihosting/semihosting.rst#sys-open-0x01
    if (strcmp(name, stdio_name) == 0) {
        if (iotsdk_serial_setup() != 0) {
            return -1;
        }
        if (openmode & OPEN_W) {
            return STDOUT_FILENO;
        }
        if (openmode & OPEN_A) {
            return STDERR_FILENO;
        }
        return STDIN_FILENO;
    }

    return -1;
}

int _sys_close(FILEHANDLE fh)
{
    (void)fh;
    return -1;
}

int _sys_write(FILEHANDLE fd, const unsigned char *str, unsigned int len, int mode)
{
    // From <rt_sys.h>: `mode' exists for historical reasons and must be ignored.
    (void)mode;

    if (fd != STDOUT_FILENO && fd != STDERR_FILENO) {
        return -1;
    }

    if (str == NULL || len == 0 || len > SIZE_MAX) {
        return -1;
    }

    const int written = iotsdk_serial_out((const char *)str, (size_t)len);
    if (written < 0) {
        return -1;
    } else if ((unsigned int)written != len) {
        return -1;
    }

    return 0;
}

int _sys_read(FILEHANDLE fd, unsigned char *str, unsigned int len, int mode)
{
    // From <rt_sys.h>: `mode' exists for historical reasons and must be ignored.
    (void)mode;

    if (fd != STDIN_FILENO) {
        return -1;
    }

    if (str == NULL || len == 0 || len > SIZE_MAX) {
        return -1;
    }

    const int read = iotsdk_serial_in((char *)str, (size_t)len);
    if (read < 0) {
        return -1;
    } else if ((unsigned int)read != len) {
        return -1;
    }

    return 0;
}

int _sys_istty(FILEHANDLE fh)
{
    (void)fh;
    return 0;
}

long _sys_flen(FILEHANDLE fh)
{
    (void)fh;
    return -1;
}

int _sys_seek(FILEHANDLE fh, long offset)
{
    (void)fh;
    (void)offset;
    return -1;
}
