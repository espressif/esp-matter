/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_types.h"
#include "FreeRTOS.h"
#include "task.h"

int32_t g_apbcCntr;

void CC_PalApbcCntrInit(void)
{
    g_apbcCntr = 0;
    return;
}

int32_t CC_PalApbcCntrValue(void)
{
    return g_apbcCntr;
}

CCError_t CC_PalApbcModeSelect(CCBool isApbcInc)
{
    CCError_t rc = CC_OK;

    switch (isApbcInc){
    case CC_FALSE:
        taskENTER_CRITICAL();
        g_apbcCntr--;
        taskEXIT_CRITICAL();
        break;
    case CC_TRUE:
        taskENTER_CRITICAL();
        g_apbcCntr++;
        taskEXIT_CRITICAL();
        break;
    default:
        return CC_FAIL;
    }

    if(g_apbcCntr < 0 ){
        /* illegal state - exit with error */
        return 1;
    }

    return rc;
}
