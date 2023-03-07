/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_types.h"
#include "FreeRTOS.h"
#include "task.h"

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
        taskENTER_CRITICAL();
         g_pmCntr++;
        taskEXIT_CRITICAL();
        break;
    case CC_TRUE:
        taskENTER_CRITICAL();
        g_pmCntr--;
        taskEXIT_CRITICAL();
        break;
    default:
        return CC_FAIL;
    }

    if(g_pmCntr == 0){
        /* once the counter is zero,
         * an external callback shall be called to notify the PMU that ARM Cerberus might be powered down. */
    }

    if(g_pmCntr < 0 ){
        /* illegal state - exit with error */
        return CC_FAIL;
    }

    return rc;
}
