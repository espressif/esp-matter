/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_PAL_LOG_PLAT_H_
#define _CC_PAL_LOG_PLAT_H_

#include "cc_log_mask.h"
#include <stdio.h>

/************** PRINTF rules ******************/
#if defined(DEBUG)

void CC_PalLog(int level, const char * format, ...);

#else /* Disable all prints */

#define CC_PalLog(...)  do {} while (0)

#endif


#endif /*_CC_PAL_LOG_PLAT_H_*/
