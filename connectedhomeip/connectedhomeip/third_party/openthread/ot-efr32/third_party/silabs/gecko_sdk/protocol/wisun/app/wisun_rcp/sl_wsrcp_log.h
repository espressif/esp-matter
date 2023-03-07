/***************************************************************************//**
 * Copyright 2021 Silicon Laboratories Inc. www.silabs.com
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available here[1]. This software is distributed to you in
 * Source Code format and is governed by the sections of the MSLA applicable to
 * Source Code.
 *
 * [1] www.silabs.com/about-us/legal/master-software-license-agreement
 *
 ******************************************************************************/
#ifndef SL_WSRCP_LOG_H
#define SL_WSRCP_LOG_H

#include <inttypes.h> // for PRIu32, etc...
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "rail.h"

/*
 * Use BUG() and BUG_ON() in the same ways than assert(). Consider this
 * statement as a part of the developer documentation. If the user encounter
 * this error, it means he won't be able to solve it himself and he should
 * send a bug report to the developer. For errors resulting from the user
 * environment, consider FATAL().
 *
 * Use FATAL() and FATAL_ON() you have detected something wrong in the
 * environment. You consider it make no sense to continue, but it is not your
 * fault. You should always provide an explanation as precise as possible to
 * help the user. Typically:
 *     fd = open(filename, O_RDWR);
 *     FATAL_ON(fd < 0, 1, "open: %s: %m", filename);
 *
 * Use WARN() and WARN_ON() to log unexpected events but you are able to
 * recover. They are not (yet) a bug and not (yet) fatal.
 *
 * Use INFO() to log some useful information for user. Use it thrifty. Only log
 * useful information for final user. Some people may consider these logs as a part
 * of the API.
 *
 * Use DEBUG() to add a temporary trace. DEBUG() shouldn't appears in final
 * code.
 *
 * Use TRACE() to provide debug traces in final code. TRACE() is always
 * conditional. The user have to set g_enabled_traces to make some traces
 * appear.
 *
 * BUG_ON(), FATAL_ON() and WARN_ON(), allow to keep error handling small
 * enough. However, as soon as you add a description of the error, the code will
 * be probably clearer if you use the unconditional versions of these macros.
 */

extern unsigned int g_enabled_traces;
extern bool g_enable_color_traces;

enum {
    TR_RF   = 0x01,
    TR_CHAN = 0x02,
    TR_BUS  = 0x04,
    TR_HDLC = 0x08,
    TR_HIF  = 0x10,
};
#define TRACE(COND, ...)          __TRACE(COND, "" __VA_ARGS__)
#define DEBUG(...)                __DEBUG("" __VA_ARGS__)
#define WARN(...)                 __WARN("" __VA_ARGS__)
#define WARN_ON(COND, ...)        __WARN_ON(COND, "" __VA_ARGS__)
#define FATAL(CODE, ...)          __FATAL(CODE, "" __VA_ARGS__)
#define FATAL_ON(COND, CODE, ...) __FATAL_ON(COND, CODE, "" __VA_ARGS__)
#define BUG(...)                  __BUG("" __VA_ARGS__)
#define BUG_ON(COND, ...)         __BUG_ON(COND, "" __VA_ARGS__)

enum bytes_str_options {
    DELIM_SPACE     = 0x01, // Add space between each bytes
    DELIM_COLON     = 0x02, // Add colon between each bytes
    ELLIPSIS_ABRT   = 0x04, // Assert if output is too small
    ELLIPSIS_STAR   = 0x08, // End output with * if too small
    ELLIPSIS_DOTS   = 0x10, // End output with ... if too small
    UPPER_HEX       = 0x20, // Use upper letters for hexadecimal digits
};

char *bytes_str(const void *in_start, size_t in_len, const void **in_done, char *out_start, size_t out_len, int opt);

#define __TRACE(COND, MSG, ...) \
    do {                                                             \
        if (g_enabled_traces & (COND)) {                             \
            if (MSG[0] != '\0')                                      \
                __PRINT_WITH_TIME(90, MSG, ##__VA_ARGS__);           \
            else                                                     \
                __PRINT_WITH_TIME(90, "%s:%d", __FILE__, __LINE__);  \
        }                                                            \
    } while (0)

#define __DEBUG(MSG, ...) \
    do {                                                             \
        if (MSG[0] != '\0')                                          \
            __PRINT_WITH_TIME_LINE(94, MSG, ##__VA_ARGS__);          \
        else                                                         \
            __PRINT_WITH_TIME_LINE(94, "trace");                     \
    } while (0)

#define INFO(MSG, ...) \
    do {                                                             \
        __PRINT(0, MSG, ##__VA_ARGS__);                              \
    } while (0)

#define __WARN(MSG, ...) \
    do {                                                             \
        if (MSG[0] != '\0')                                          \
            __PRINT(93, "warning: " MSG, ##__VA_ARGS__);             \
        else                                                         \
            __PRINT_WITH_LINE(93, "warning");                        \
    } while (0)

#define __WARN_ON(COND, MSG, ...) \
    ({                                                               \
        bool __ret = (COND);                                         \
        if (__ret) {                                                 \
            if (MSG[0] != '\0')                                      \
                __PRINT(93, "warning: " MSG, ##__VA_ARGS__);         \
            else                                                     \
                __PRINT_WITH_LINE(93, "warning: \"%s\"", #COND);     \
        }                                                            \
        __ret;                                                       \
    })

#define __FATAL(CODE, MSG, ...) \
    do {                                                             \
        if (MSG[0] != '\0')                                          \
            __PRINT(31, MSG, ##__VA_ARGS__);                         \
        else                                                         \
            __PRINT_WITH_LINE(31, "fatal error");                    \
        exit(CODE);                                                  \
    } while (0)

#define __FATAL_ON(COND, CODE, MSG, ...) \
    do {                                                             \
        if (COND) {                                                  \
            if (MSG[0] != '\0')                                      \
                __PRINT(31, MSG, ##__VA_ARGS__);                     \
            else                                                     \
                __PRINT_WITH_LINE(31, "fatal error: \"%s\"", #COND); \
            exit(CODE);                                              \
        }                                                            \
    } while (0)

#define __BUG(MSG, ...) \
    do {                                                             \
        if (MSG[0] != '\0')                                          \
            __PRINT_WITH_LINE(91, "bug: " MSG, ##__VA_ARGS__);       \
        else                                                         \
            __PRINT_WITH_LINE(91, "bug");                            \
        __BKPT(0);                                                   \
        for (;;);                                                    \
    } while (0)

#define __BUG_ON(COND, MSG, ...) \
    do {                                                             \
        if (COND) {                                                  \
            if (MSG[0] != '\0')                                      \
                __PRINT_WITH_LINE(91, "bug: " MSG, ##__VA_ARGS__);   \
            else                                                     \
                __PRINT_WITH_LINE(91, "bug: \"%s\"", #COND);         \
            __BKPT(0);                                               \
            for (;;);                                                \
        }                                                            \
    } while (0)

#define __PRINT(COLOR, MSG, ...) \
    do {                                                             \
        if (COLOR != 0 && g_enable_color_traces)                     \
            printf("\x1B[" #COLOR "m" MSG "\x1B[0m\n", ##__VA_ARGS__); \
        else                                                         \
            printf(MSG "\n", ##__VA_ARGS__);                \
    } while(0)

#define __PRINT_WITH_TIME(COLOR, MSG, ...) \
    do {                                                             \
        unsigned int t_us = RAIL_GetTime();                          \
        __PRINT(COLOR, "%u.%06u: " MSG, t_us / 1000000, t_us % 1000000, ##__VA_ARGS__); \
    } while (0)

#define __PRINT_WITH_LINE(COLOR, MSG, ...) \
    __PRINT(COLOR, "%s():%d: " MSG, __func__, __LINE__, ##__VA_ARGS__)

#define __PRINT_WITH_TIME_LINE(COLOR, MSG, ...) \
    __PRINT_WITH_TIME(COLOR, "%s():%d: " MSG, __func__, __LINE__, ##__VA_ARGS__)

#endif
