/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include <stdio.h>
#include <stdarg.h>
#include "cc_pal_types.h"
#include "cc_pal_log.h"

int CC_PAL_logLevel = CC_PAL_MAX_LOG_LEVEL;
uint32_t CC_PAL_logMask = 0xFFFFFFFF;

void CC_PalLogInit(void)
{
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
    CC_UNUSED_PARAM(level);
    va_start( args, format );

    vprintf( format, args);
    va_end(args);
}
