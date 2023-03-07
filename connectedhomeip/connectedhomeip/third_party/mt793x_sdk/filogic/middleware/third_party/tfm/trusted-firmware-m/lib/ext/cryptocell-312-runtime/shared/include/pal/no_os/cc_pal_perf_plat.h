/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CC_PAL_PERF_PLAT_H__
#define _CC_PAL_PERF_PLAT_H__

typedef unsigned int CCPalPerfData_t;

/**
 * @brief   DSM environment bug - sometimes very long write operation.
 *     to overcome this bug we added while to make sure write opeartion is completed
 *
 * @param[in]
 * *
 * @return None
 */
void CC_PalDsmWorkarround();


#endif /*_CC_PAL_PERF_PLAT_H__*/
