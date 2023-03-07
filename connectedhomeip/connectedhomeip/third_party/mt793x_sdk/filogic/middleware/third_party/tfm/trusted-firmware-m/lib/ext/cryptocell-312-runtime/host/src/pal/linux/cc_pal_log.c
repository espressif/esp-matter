/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <syslog.h>
#include <stdarg.h>
#include "cc_pal_types.h"
#include "cc_pal_log.h"

#ifdef DEBUG
#define SYSLOG_OPTIONS (LOG_CONS | LOG_NDELAY | LOG_PID | LOG_PERROR)
#else
#define SYSLOG_OPTIONS (LOG_CONS | LOG_NDELAY | LOG_PID)
#endif

int CC_PAL_logLevel = CC_PAL_MAX_LOG_LEVEL;
uint32_t CC_PAL_logMask = 0xFFFFFFFF;

void CC_PalLogInit(void)
{
    static int initOnce = 0;

    if (!initOnce)
        openlog("CC.Proc.", SYSLOG_OPTIONS, LOG_USER);
    initOnce = 1;
}

void CC_PalLogLevelSet(int setLevel)
{
    CC_PAL_logLevel = setLevel;
}

void CC_PalLogMaskSet(uint32_t setMask)
{
    CC_PAL_logMask = setMask;
}

void CC_PalLog(int level, const char * format, ...)
{
    va_list args;
    va_start( args, format );

    vsyslog(level + LOG_ERR, format, args);
    va_end(args);
}


