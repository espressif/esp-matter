/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpSched.c
 *   This file contains the implementation of the scheduler, which is the operating system.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_SCHED

#include "gpSched.h"
#include "gpSched_defs.h"
#include "hal.h"
#include "gpUtils.h"
#include "gpLog.h"
#ifdef GP_DIVERSITY_CORTEXM4
#include "gpAssert.h"
#endif

UInt32 gpSched_GetTimeToNextEvent (void)
{
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    UInt32 timeToNextEvent = 0;
    gpSched_Event_t* pevt;

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    pevt = (gpSched_Event_t*)gpUtils_LLGetFirstElem(sched_globals->gpSched_EventList_p);
    if(pevt)
    {
        UInt32 time_now;
        HAL_TIMER_GET_CURRENT_TIME_1US(time_now);

        if(GP_SCHED_TIME_COMPARE_BIGGER_EQUAL(pevt->time, time_now))
        {
            timeToNextEvent = GP_SCHED_GET_TIME_DIFF(time_now, pevt->time);
        }
        else
        {
            timeToNextEvent = 0;
        }
    }
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);

    return timeToNextEvent;
}

UInt32 gpSched_GetGoToSleepThreshold (void)
{
#ifdef GP_DIVERSITY_CORTEXM4
    // Depricated function, assertion to detect potential use
    GP_ASSERT_SYSTEM(false);
#endif
    return 0;
}

Bool gpSched_IsSleepEnabled(void)
{
#ifdef GP_DIVERSITY_CORTEXM4
    // Depricated function, assertion to detect potential use
    GP_ASSERT_SYSTEM(false);
#endif
    return 0;
}
