/*
 * Copyright (c) 2010-2013, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpSched_simple.c
 *   This file contains the data buffers used by the scheduler
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

#include "gpUtils.h"
#include "gpSched.h"
#include "gpSched_defs.h"

#include "gpLog.h"
#include "gpAssert.h"


/*****************************************************************************
 *                    Precompiler checks
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//Size definitions
#ifndef GP_SCHED_EVENT_LIST_SIZE
#define GP_SCHED_EVENT_LIST_SIZE            10
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

GP_UTILS_LL_MEMORY_DECLARATION(gpSched_Event_t, GP_SCHED_EVENT_LIST_SIZE);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

GP_UTILS_LL_MEMORY_ALOCATION(gpSched_Event_t, gpSched_EventArray) GP_EXTRAM_SECTION_ATTR;

gpSched_globals_t gpSched_globals;

#if(!defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2))
gpUtils_LinkFree_t gpSched_eventLinkFree;
gpUtils_LinkList_t gpSched_eventLinkList;
#endif

#if GP_SCHED_NR_OF_IDLE_CALLBACKS > 0
static gpSched_OnIdleCallback_t sched_OnIdleCallbacks[GP_SCHED_NR_OF_IDLE_CALLBACKS];        /**< Array with functions to call when the scheduler is idle */
#endif /* GP_SCHED_NR_OF_IDLE_CALLBACKS > 0 */

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#if GP_SCHED_NR_OF_IDLE_CALLBACKS > 0
void gpSched_RegisterOnIdleCallback(gpSched_OnIdleCallback_t callback)
{
    UInt32 i;

    GP_ASSERT_DEV_EXT(callback != NULL);

    for(i=0; i<GP_SCHED_NR_OF_IDLE_CALLBACKS; ++i)
    {
        if (sched_OnIdleCallbacks[i] == NULL)
        {
            sched_OnIdleCallbacks[i] = callback;
            break;
        }
    }
}

void gpSched_PostProcessIdle(void)
{
    UInt32 i;

    // Call all registered external post process idle callbacks
    /* Note:
     * The scheduler is not checked between scheduling of the external CBs
     * to fairly divide idle time between all registered CBs
     */
    for(i=0; i<GP_SCHED_NR_OF_IDLE_CALLBACKS; ++i)
    {
        if (sched_OnIdleCallbacks[i])
        {
            sched_OnIdleCallbacks[i]();
        }
    }
}
#endif /* GP_SCHED_NR_OF_IDLE_CALLBACKS > 0 */


void gpSched_InitExtramData(void)
{
#if(!defined(GP_DIVERSITY_JUMPTABLES) || defined(GP_DIVERSITY_ROM_GPSCHED_V2))
    gpSched_globals.gpSched_EventFree_p = &gpSched_eventLinkFree;
    gpSched_globals.gpSched_EventList_p = &gpSched_eventLinkList;
#endif
    gpUtils_LLInit(gpSched_EventArray, GP_UTILS_LL_SIZE_OF (gpSched_Event_t) , GP_SCHED_EVENT_LIST_SIZE, gpSched_globals.gpSched_EventFree_p);

#if defined(GP_DIVERSITY_JUMPTABLES)
    gpSched_globals.AppDiversitySleep = SCHED_APP_DIVERSITY_SLEEP();
    gpSched_globals.AppDiversityCom = SCHED_APP_DIVERSITY_COM();
    gpSched_globals.AppDiversityComNoRx = SCHED_APP_DIVERSITY_COM_NO_RX();
#endif //defined(GP_DIVERSITY_JUMPTABLES)

#if GP_SCHED_NR_OF_IDLE_CALLBACKS > 0
    MEMSET(sched_OnIdleCallbacks, 0, sizeof(sched_OnIdleCallbacks));
#endif /* GP_SCHED_NR_OF_IDLE_CALLBACKS > 0 */

}


void gpSched_SetGotoSleepCheckCallback( gpSched_GotoSleepCheckCallback_t gotoSleepCheckCallback )
{
    NOT_USED(gotoSleepCheckCallback);
}

