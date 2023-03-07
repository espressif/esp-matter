/*
 * Copyright (c) 2017, 2019, Qorvo Inc
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

#include "hal.h"
#include "gpSched.h"
#include "gpSched_defs.h"
#include "gpJumpTables.h"
#include "gpJumpTables_DataTable.h"
#include "gpLog.h"
#ifdef GP_COMP_COM
#include "gpCom.h"
#endif // GP_COMP_COM


/*****************************************************************************
 *                    Precompiler checks
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define SCHED_MAIN_BODY(pS)

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Function Prototypes
 *****************************************************************************/
extern gpSched_Event_t* Sched_FindEventArg(gpUtils_LinkList_t* plst, gpSched_EventCallback_t callback, void* arg, Bool unscheduleWhenFound );
extern UInt32 Sched_GetRemainingTime(gpSched_Event_t* pevt, UInt32* sec);

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
STATIC_FUNC void Sched_RescheduleEvent (UInt32 rel_time, gpSched_Event_t * pevt);
STATIC_FUNC void Sched_RescheduleEventAbs (UInt32 time, gpSched_Event_t * pevt);
STATIC_FUNC void Sched_ReleaseEventBody(gpSched_Event_t* pevt );
#ifdef GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
STATIC_FUNC void Sched_ScheduleEvent( UInt32 time, gpSched_EventCallback_t callback, void* arg,UInt32 RemainderInSeconds ,Bool scheduleAbsTime);
STATIC_FUNC void Sched_ScheduleEventInSeconds(UInt32 delayInSec, UInt32 delayInUs, Bool reScheduling, UInt32 delayerror, gpSched_EventCallback_t callback, void* arg);
#else
STATIC_FUNC void Sched_ScheduleEvent( UInt32 time, gpSched_EventCallback_t callback, void* arg);
#endif

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#if defined(GP_DIVERSITY_GPHAL_K8E)
#define ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS  2
#endif  // GP_DIVERSITY_GPHAL_K8E


/*****************************************************************************
 *                    Static Function Patches
 *****************************************************************************/

void Sched_ExecEvent(gpSched_Event_t * pevt)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
#ifdef GP_SCHED_DIVERSITY_USE_ARGS
    if (pevt->arg) pevt->func.callbackarg(pevt->arg);
    else           pevt->func.callback();
#else
    pevt->func.callback();
#endif // GP_SCHED_DIVERSITY_USE_ARGS
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

void Sched_ReleaseEvent(gpSched_Event_t * pevt)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    HAL_DISABLE_GLOBAL_INT();
    Sched_ReleaseEventBody(pevt);
    HAL_ENABLE_GLOBAL_INT();
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

void Sched_RescheduleEvent (UInt32 rel_time, gpSched_Event_t * pevt)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    gpUtils_LinkList_t* pList = sched_globals->gpSched_EventList_p;
    gpSched_Event_t* pevt_nxt = NULL;
    UInt32 now;

    GP_ASSERT_DEV_INT(pevt); //pevt pointer should be != NULL

    HAL_TIMER_GET_CURRENT_TIME_1US(now);
    if (rel_time == GP_SCHED_EVENT_TIME_NOW)
    {
        UInt32 first_time;

        if (pList->plnk_first)
        {
            pevt_nxt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(pList->plnk_first);
            first_time = pevt_nxt->time;
        }
        else
        {
            first_time = now;
        }
        pevt->time = GP_SCHED_TIME_COMPARE_LOWER_EQUAL (first_time, now)?
                first_time - 1L
            :
                now - 1L;
    }
    else
    {
        pevt->time = now + rel_time;
    }

    // You can't schedule negative times.
    // This is aLready checked: GP_ASSERT_SYSTEM((rel_time <= GP_SCHED_EVENT_TIME_MAX) || (rel_time == GP_SCHED_EVENT_TIME_NOW));
    // But you also can't schedule further then GP_SCHED_EVENT_TIME_MAX from the first event in the list (else the order in the list will be wrong)
    // This is especially true if the first event in the list is in the past (before 'now').

    if(pList->plnk_first)
    {
        pevt_nxt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(pList->plnk_first);
        GP_ASSERT_SYSTEM(rel_time < (pevt_nxt->time - now + GP_SCHED_EVENT_TIME_MAX));
    }

    Sched_RescheduleEventAbs (pevt->time, pevt);
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

STATIC_FUNC void Sched_RescheduleEventAbs (UInt32 time, gpSched_Event_t * pevt)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    gpUtils_LinkList_t* pList = sched_globals->gpSched_EventList_p;
    gpSched_Event_t* pevt_nxt = NULL;
    gpUtils_Link_t* plnk = NULL;

    pevt->time  = time;

    // Scroll pointer
    // Insert event
    // Duration : 8us per loop
    {
        // Search for first item, that should be executed later as the current one
        // or that will be executed as last
        for (plnk = pList->plnk_first;
             plnk;
             plnk = plnk->plnk_nxt)
        {
            pevt_nxt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(plnk);
            if (GP_SCHED_TIME_COMPARE_BIGGER( pevt_nxt->time, pevt->time ))
            {
                break;
            }
        }
    }

    if (plnk)
    {
        gpUtils_LLInsertBefore( pevt, pevt_nxt, pList);
    }
    else
    {
        gpUtils_LLAdd( pevt, pList );
    }
    gpSched_Trigger();
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

#ifdef GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
void Sched_ScheduleEvent( UInt32 time, gpSched_EventCallback_t callback, void* arg,UInt32 RemainderInSeconds ,Bool scheduleAbsTime)
#else
void Sched_ScheduleEvent(UInt32 time, gpSched_EventCallback_t callback, void* arg)
#endif
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_Event_t* pevt;
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventFree_p);
    pevt = (gpSched_Event_t*)gpUtils_LLNew(sched_globals->gpSched_EventFree_p);
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventFree_p);

    if (!pevt)
    {

        /* Maximum number of events scheduled has been reached (maximum number of scheduled events can be changed with GP_SCHED_EVENT_LIST_SIZE) */
        GP_ASSERT_SYSTEM(false);
    }
    else
    {
#ifdef GP_SCHED_DIVERSITY_USE_ARGS
        {
            pevt->arg = arg;
        }
#endif //GP_SCHED_DIVERSITY_USE_ARGS
        pevt->func.callbackarg = callback;
#ifdef GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
        pevt->RemainderInSeconds = RemainderInSeconds;
        if (scheduleAbsTime)
        {
            Sched_RescheduleEventAbs(time, pevt);
        }
        else
#endif //GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
        {
            Sched_RescheduleEvent( time,  pevt);
        }

    }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

void Sched_ReleaseEventBody(gpSched_Event_t* pevt )
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    // Free event (Unlink has been done by GetEvent)
    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventFree_p);
    gpUtils_LLFree(pevt, sched_globals->gpSched_EventFree_p);
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventFree_p);
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}



STATIC_FUNC gpSched_Event_t* Sched_GetEvent(void)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_Event_t* pevt = NULL;
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    pevt = (gpSched_Event_t*)gpUtils_LLGetFirstElem(sched_globals->gpSched_EventList_p);
    if(pevt)
    {
        UInt32 time_now;
        //Check the eventList for overdue events
        HAL_TIMER_GET_CURRENT_TIME_1US(time_now);

        if (GP_SCHED_TIME_COMPARE_BIGGER_EQUAL (time_now, pevt->time))
        {
            gpUtils_LLUnlink(pevt, sched_globals->gpSched_EventList_p);
        }
        else
        {
            pevt = NULL;
        }
    }
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    return pevt;
#else
    return NULL;
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

/*****************************************************************************
 *                    Public Function Patches
 *****************************************************************************/

void gpSched_Init(void)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();

    // Inititialize event lists
    gpSched_InitExtramData(); //Initialize free list and event data buffer
    gpUtils_LLClear(sched_globals->gpSched_EventList_p);


    Sched_Integration_Init();
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
} //gpSched_init

void gpSched_DeInit(void)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    gpUtils_LLDeInit(sched_globals->gpSched_EventList_p);
    gpUtils_LLDeInitFree(sched_globals->gpSched_EventFree_p);

    Sched_Integration_DeInit();
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

void gpSched_Clear( void )
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    while(sched_globals->gpSched_EventList_p->plnk_last)
    {
        gpSched_Event_t* pevt;

        pevt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(sched_globals->gpSched_EventList_p->plnk_last);
        //GP_LOG_SYSTEM_PRINTF("event %x",2,(UInt16)pevt->func.callback);
        gpUtils_LLUnlink(pevt, sched_globals->gpSched_EventList_p);
        Sched_ReleaseEventBody(pevt);
    }


    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

Bool gpSched_EventQueueEmpty( void )
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    Bool Empty;

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    Empty = (!sched_globals->gpSched_EventList_p->plnk_first);
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    return Empty;
#else
    return true;
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

void gpSched_ScheduleEventArg(UInt32 rel_time, gpSched_EventCallback_t callback, void* arg)
{
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();

#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    GP_ASSERT_SYSTEM((rel_time <= GP_SCHED_EVENT_TIME_MAX) || (rel_time == GP_SCHED_EVENT_TIME_NOW));
#ifdef GP_DIVERSITY_FREERTOS
    if(gpSched_ScheduleEventDeferred(rel_time, callback, arg))
    {
        return;
    }
#endif
    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);

#ifdef GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
    Sched_ScheduleEvent(rel_time , callback , arg, 0, false);
#else
    Sched_ScheduleEvent(rel_time , callback , arg);
#endif
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

Bool gpSched_UnscheduleEventArg(gpSched_EventCallback_t callback, void* arg)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    gpSched_Event_t* pevt;

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    pevt = Sched_FindEventArg(sched_globals->gpSched_EventList_p, callback, arg, true);
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    return pevt?true:false;
#else
    return false;
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

Bool gpSched_ExistsEventArg(gpSched_EventCallback_t callback, void* arg)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    gpSched_Event_t* pevt;
    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    pevt = Sched_FindEventArg(sched_globals->gpSched_EventList_p,callback, arg, false);
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    return pevt?true:false;
#else
    return false;
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

UInt32 gpSched_GetRemainingTimeArgInSecAndUs(gpSched_EventCallback_t callback, void* arg, UInt32* sec)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_Event_t*        pevt=NULL;
    UInt32                  remainingTime = 0xffffffff;
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    pevt = Sched_FindEventArg(sched_globals->gpSched_EventList_p, callback, arg, false);
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);

    remainingTime = Sched_GetRemainingTime(pevt, sec);

    return remainingTime;
#else
    return 0;
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

gpSched_Event_t* Sched_FindEventArg(gpUtils_LinkList_t* plst , gpSched_EventCallback_t callback, void* arg, Bool unscheduleWhenFound)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpUtils_Link_t* plnk;
    for (plnk = plst->plnk_first;plnk;plnk = plnk->plnk_nxt)
    {
        gpSched_Event_t* pevt;
        pevt = (gpSched_Event_t*)GP_UTILS_LL_GET_ELEM(plnk);
        //using (arg == NULL) as wildcard

#ifdef GP_SCHED_DIVERSITY_USE_ARGS
        if ((pevt->func.callbackarg == callback) && ((pevt->arg == arg) || (arg == NULL)))
#else
        if (pevt->func.callbackarg == callback)
#endif
        {
            if (unscheduleWhenFound)
            {
                gpUtils_LLUnlink (pevt, plst);
                Sched_ReleaseEventBody(pevt);
            }
            return pevt;
        }
    }

#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    return NULL;
}

#ifdef GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
STATIC_FUNC void Sched_ScheduleEventInSeconds(UInt32 delayInSec, UInt32 delayInUs, Bool reScheduling, UInt32 prevDeadline, gpSched_EventCallback_t callback, void* arg)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_globals_t* sched_globals = GP_SCHED_GET_GLOBALS();
    UInt32 RemainderInUs  = delayInUs % 1000000;
    UInt32 RemainderInSec = delayInSec + (delayInUs / 1000000);
    UInt32 NextIntervalInSec;
    const Bool scheduleAbsTime = reScheduling;

    if(RemainderInSec > GP_SCHED_DISTANTEVENTS_INTERVAL)
    {
        NextIntervalInSec = GP_SCHED_DISTANTEVENTS_INTERVAL;
        RemainderInSec   -= GP_SCHED_DISTANTEVENTS_INTERVAL;
    }
    else
    {
        NextIntervalInSec = RemainderInSec;
        RemainderInSec    = 0;
    }

    RemainderInUs += NextIntervalInSec * 1000000;

    if (reScheduling) {
        RemainderInUs += prevDeadline;
    }

    gpUtils_LLLockAcquire((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
    Sched_ScheduleEvent(
        RemainderInUs,
        callback,
        arg,
        RemainderInSec,
        scheduleAbsTime
    );
    gpUtils_LLLockRelease((gpUtils_Links_t *)sched_globals->gpSched_EventList_p);
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}
#endif // GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI

/* Dummy implementation in case GP_SCHED_DIVERSITY_SLEEP is not enabled for the patch */
UInt32 Sched_CanGoToSleep(void){ return 0; }

/*
* @brief Goes to sleep if able. Flushes com, stops timers, ...
*/
void gpSched_GoToSleep( void )
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

void gpSched_SetGotoSleepEnable( Bool enable )
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    NOT_USED(enable);
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}


/*****************************************************************************
 *                    Main function
 *****************************************************************************/

void gpSched_Main_Body(void)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
    gpSched_Event_t* pevt;
#if defined(GP_COMP_CHIPEMU) &&  defined(GP_DIVERSITY_GPHAL_K8E) && defined(GP_DIVERSITY_GPHAL_GPMICRO)
    Bool gpSched_executed_event = false;
#endif

    // Check stack before event (after coming from sleep)
    GP_UTILS_CHECK_STACK_PATTERN();
    GP_UTILS_CHECK_STACK_POINTER();

    GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());

    HAL_WDT_RESET();

        // Handle radio interrupts
#if defined(GP_COMP_CHIPEMU) &&  defined(GP_DIVERSITY_GPHAL_K8E) && defined(GP_DIVERSITY_GPHAL_GPMICRO)
        gpSched_executed_event =
#endif
#if !defined(HAL_LINUX_DIVERSITY_INTERRUPT_WAKES_IOTHREAD)
        HAL_RADIO_INT_EXEC_IF_OCCURED();
#endif
        GP_UTILS_CPUMON_PROCDONE(RADIO_INT);

#if defined(GP_COMP_COM) && !defined(TBC_GPCOM)
        if (SCHED_APP_DIVERSITY_COM())
        {
            // Handle non-interrupt driven actions from gpCom
            gpCom_HandleTx();
            if (!SCHED_APP_DIVERSITY_COM_NO_RX())
            {
                gpCom_HandleRx();
            }
            GP_UTILS_CPUMON_PROCDONE(GPCOMTXRX);
        }
#endif //GP_COMP_COM

    // Handle events
    pevt = Sched_GetEvent();
    SCHED_MAIN_BODY( pevt );
    if (pevt)             // get event
    {
        GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());
            //GP_LOG_SYSTEM_PRINTF("sched: %08lx",0, pevt->func.callback);
#ifdef GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
            if (pevt->RemainderInSeconds) {
                Sched_ScheduleEventInSeconds(pevt->RemainderInSeconds, 0, true, pevt->time, pevt->func.callbackarg, pevt->arg);
            } else
#endif
            {
                Sched_ExecEvent(pevt);
#if defined(GP_COMP_CHIPEMU) &&  defined(GP_DIVERSITY_GPHAL_K8E) && defined(GP_DIVERSITY_GPHAL_GPMICRO)
                gpSched_executed_event = true;
#endif
            }

        // check stack after event
        GP_UTILS_CHECK_STACK_PATTERN();
        GP_UTILS_CHECK_STACK_POINTER();

        if (!HAL_GLOBAL_INT_ENABLED())
        {
            GP_LOG_PRINTF("AtomF %p",2,pevt->func.callback);
            GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());
        }
        Sched_ReleaseEvent(pevt);                // release event
        // check stack after event release
        GP_UTILS_CHECK_STACK_PATTERN();
        GP_UTILS_CHECK_STACK_POINTER();
        GP_UTILS_CHECK_STACK_USAGE();
    }
    GP_UTILS_CPUMON_PROCDONE(SCHEDEVENT);

#ifdef GP_COMP_CHIPEMU
    gpChipEmu_RunEmu();
#if  defined(GP_DIVERSITY_GPHAL_K8E) && defined(GP_DIVERSITY_GPHAL_GPMICRO)
    /* if gpSched_executed_event == true, then we might still have more work for NRT which has priority! */
    if (!gpSched_executed_event)
    {
        gpSched_gpMicroHasMoreWork = gpChipEmu_RunGpmicro();
    }
#endif
#endif // GP_COMP_CHIPEMU

    GP_ASSERT_DEV_EXT(HAL_GLOBAL_INT_ENABLED());
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
}

#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
void Sched_GetEventIdlePeriod(void) {}
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_SCHED_INTEGRATION_CALLS)
