/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_types.h"

int32_t g_pmCntr;

void CC_PalPowerSaveModeInit(void)
{
    g_pmCntr = 0;
    return;
}

int32_t CC_PalPowerSaveModeStatus(void)
{
    return g_pmCntr;
}

CCError_t CC_PalPowerSaveModeSelect(CCBool isPowerSaveMode)
{
    CCError_t rc = CC_OK;
    switch (isPowerSaveMode){
    case CC_FALSE:
         __atomic_fetch_add(&g_pmCntr, 1, __ATOMIC_SEQ_CST);
        break;
    case CC_TRUE:
        __atomic_fetch_sub(&g_pmCntr, 1, __ATOMIC_SEQ_CST);
        break;
    default:
        return -1;
    }

    if(g_pmCntr == 0){
        /* once the counter is zero,
         * an external callback shall be called to notify the PMU that ARM Cerberus might be powered down. */
    }

    if(g_pmCntr < 0 ){
        /* illegal state - exit with error */
        return 1;
    }

    return rc;
}
