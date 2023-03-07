/*
 * Copyright (c) 2008-2016, GreenPeak Technologies
 * Copyright (c) 2017-2021, Qorvo Inc
 *
 * gpSched.h
 *   This file contains the definitions of the scheduler, wich is the operating system.
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
/**
 * @file gpSched.h
 * @brief The cooperative scheduling component that handles all Qorvo stack events.
 */

#ifndef _GPSCHED_H_
#define _GPSCHED_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#ifdef GP_DIVERSITY_LINUXKERNEL
#include "gpSched_kernel.h"
#endif //GP_DIVERSITY_LINUXKERNEL
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_SCHED_DISTANTEVENTS_INTERVAL (20 * 60)
#define GP_SCHED_EVENT_TIME_NOW         ((UInt32)-1L)
#define GP_SCHED_EVENT_TIME_MAX         0x7FFFFFFFUL

/** @brief Compares times from the chip's timebase - check if t1 < t2 (in us)
*/
#define GP_SCHED_TIME_COMPARE_LOWER_US(t1, t2) (!((UInt32)((t1) - (t2)) /*&(0xFFFFFFFF)*/ < (0x80000000LU)))

/** @brief Allow sleep only if no events are pending
*/
#define GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES ((UInt32)(0xFFFFFFFF))

/** @brief Default time between events before going to sleep is considered */
#ifndef HAL_DEFAULT_GOTOSLEEP_THRES
#define HAL_DEFAULT_GOTOSLEEP_THRES GP_SCHED_NO_EVENTS_GOTOSLEEP_THRES
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef void (*gpSched_EventCallback_t)(void*);

typedef Bool (*gpSched_GotoSleepCheckCallback_t)(void);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "gpSched_CodeJumpTableFlash_Defs.h"
#include "gpSched_CodeJumpTableRom_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief Initializes the scheduler
 *
 * This function intializes the scheduler.
 *
 */
GP_API void gpSched_Init(void);
GP_API void gpSched_DeInit(void);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
/**
 * @brief Sets the minimum time required between events for going to sleep.
 *
 * This function sets a threshold in us, that is used for determining if we can go to sleep.
 * If the next scheduled event is at a time later than the current time + the defined threshold (- wakeup threshold)
 * then going to sleep is allowed
 *
 *  @param Threshold     threshold in us
 */
GP_API Bool gpSched_SetGotoSleepThreshold(UInt32 Threshold);
GP_API void gpSched_SetGotoSleepCheckCallback(gpSched_GotoSleepCheckCallback_t gotoSleepCheckCallback);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
GP_API void gpSched_SetGotoSleepEnable(Bool enable);

GP_API void gpSched_GoToSleep(void);

/**
 * @brief Starts the time base.
 *
 * Starts the time base, i.e. the timer interrupts are enabled.
 */
GP_API void gpSched_StartTimeBase(void);

/**
 * @brief Returns the current time of the time base.
 *
 * @return The current time in time base units of 1us.
 */
GP_API UInt32 gpSched_GetCurrentTime(void);

/**
 * @brief Compare 2 times (in 1us) taking overflows into account.
 *
 * @param n_time1   Time (in us) to be compared.
 * @param n_time2   Second time (in us) to compare against n_time1.
 *
 * @return return True if n_time1 is earlier then n_time2
 */
GP_API Bool gpSched_TimeCompareLower(UInt32 n_time1, UInt32 n_time2);

/**
 * @brief Clears the event queue.
 *
 * Clears the event queue.
 *
 */
GP_API void gpSched_Clear(void);

/**
 * @brief Returns the event queue status.
 *
 * Returns the next event to execute.  The return pointer is NULL when no event is pending yet.  This function
 * should be called in a polling loop in the main thread.
 *
 * @return  Event queue status:
 *              - true: Queue is empty
 *              - false: Still events in the queue
 */
GP_API Bool gpSched_EventQueueEmpty(void);

/**
 * @brief Schedules a scheduled event.
 *
 * Schedules an event.  The event is inserted into the event queue.
 *
 * @param  rel_time     Relative execution time (delay) in us.  If the delay equals 0, the function will be scheduled for immedate execution. rel_time shall not exceed GP_SCHED_EVENT_TIME_MAX.
 * @param  callback     Callback function.
 */
GP_API void gpSched_ScheduleEvent(UInt32 rel_time, void_func callback);

/**
 * @brief Schedules a scheduled event.
 *
 * Schedules an event.  The event is inserted into the event queue.
 *
 * @param  rel_time     Relative execution time (delay) in us.  If the delay equals 0, the function will be scheduled for immedate execution. rel_time shall not exceed GP_SCHED_EVENT_TIME_MAX.
 * @param  callback     Callback function.
 * @param  arg          Pointer to the argument buffer.
 *                      Limitation: Passing a NULL pointer or ((void*) 0) as @p arg will result in calling @p callback
 *                      WITHOUT an argument: i.e. callback(); instead of callback(arg);
 *                      If the callback were to use the argument, it will use random data from the stack.
 */
GP_API void gpSched_ScheduleEventArg(UInt32 rel_time, gpSched_EventCallback_t callback, void* arg);

/**
 * @brief Unschedule a scheduled event.
 *
 * Unschedules a scheduled event before its execution.  The event is identified with the callback pointer
 *
 * @param  callback Scheduled event callback.
 * @return          True if event was found. False if event was no longer in the queue.
 */
GP_API Bool gpSched_UnscheduleEvent(void_func callback);

/**
 * @brief Check if an event is still waiting for execution
 *
 * @param  callback Scheduled event callback to check.
 * @return          True if callback is still pending.
*/
GP_API Bool gpSched_ExistsEvent(void_func callback);
/**
 * @brief Get the remaining time untill event execution
 *
 * @param  callback Scheduled event callback to check.
 * @return          Time in us untill event normally executes. If event is not found 0xFFFFFFFF is returned.
*/
GP_API UInt32 gpSched_GetRemainingTime(void_func callback);

/**
 * @brief Unschedule a scheduled event.
 *
 * Unschedules a scheduled event before its execution.  The event is identified with the callback pointer and the
 * arguments content.
 *
 * @param  callback Scheduled event callback.
 * @param  arg      Pointer to the the arguments used when scheduling. Use NULL to skip matching the arg pointer.
 * @return          True if event was found. False if event was no longer in the queue.
 */
GP_API Bool gpSched_UnscheduleEventArg(gpSched_EventCallback_t callback, void* arg);
/**
 * @brief Check if an event is still waiting for execution.
 *
 * @param  callback Scheduled event callback to check.
 * @param  arg      Pointer to the the arguments used when scheduling. Use NULL to skip matching the arg pointer.
 * @return          True if callback is still pending.
*/
GP_API Bool gpSched_ExistsEventArg(gpSched_EventCallback_t callback, void* arg);
/**
 * @brief Get the remaining time untill event execution.
 *
 * @param  callback Scheduled event callback to check.
 * @param  arg      Pointer to the the arguments used when scheduling. Use NULL to skip matching the arg pointer.
 * @return          Time in us untill event normally executes. If event is not found 0xFFFFFFFF is returned.
*/
GP_API UInt32 gpSched_GetRemainingTimeArg(gpSched_EventCallback_t callback, void* arg);

/**
 * @brief Get the remaining time untill event execution.
 *  The remaining time for the event can be computed from the *sec parameter and the return parameter as: (*sec * 1000000 + returnvalue) microseconds.
 *
 * @param  callback Scheduled event callback to check.
 * @param  arg      Pointer to the the arguments used when scheduling. Use NULL to skip matching the arg pointer.
 * @param  sec      Pointer to a UInt32, where the remaining time in seconds will be stored. If event is not found 0xFFFFFFFF is filled in.
 * @return          Time in us untill event normally executes. If event is not found 0xFFFFFFFF is returned.
*/
GP_API UInt32 gpSched_GetRemainingTimeArgInSecAndUs(gpSched_EventCallback_t callback, void* arg, UInt32* sec);


GP_API void gpSched_Main_Body(void);

#ifdef GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
/**
 *
 *  This primitive is used to schedule an event with seconds resolution.
 *  The gpSched_ScheduleEvent has a limit on 30 minutes, therefore this function was built to allow scheduling beyond that limit
 *
 * @param  delayInSec           Relative execution time (delay) in seconds.  If the delay equals 0, the function will be scheduled for immediate execution.
 * @param  callback             Callback function.
 * @param  arg                  Pointer to the argument buffer.
*/
GP_API void gpSched_ScheduleEventInSeconds(UInt32 delayInSec, gpSched_EventCallback_t callback, void* arg);

/**
 *
 *  This primitive is used to schedule an event with seconds and microseconds resolution
 *  The event will be scheduled after a delay of (delayInSec * 1000000 + delayInUs) microseconds.
 *
 * @param  delayInSec           Relative execution time (delay) in seconds.
 * @param  delayInUs            Relative execution time (delay) in microseconds.  If both delay parameters equal 0, the function will be scheduled for immediate execution.
 * @param  callback             Callback function.
 * @param  arg                  Pointer to the argument buffer.
*/
void gpSched_ScheduleEventInSecAndUs(UInt32 delayInSec, UInt32 delayInUs, gpSched_EventCallback_t callback, void* arg);
#endif // GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */


GP_API UInt32 gpSched_GetTimeToNextEvent(void);
GP_API UInt32 gpSched_GetGoToSleepThreshold(void);
GP_API Bool gpSched_IsSleepEnabled(void);

/**
 * @brief Function to notify gpSched task/thread when running on an OS.
 *
 * Will notify the thread to run again in an OS-dependent manner.
 */
void gpSched_NotifySchedTask(void);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#if GP_SCHED_NR_OF_IDLE_CALLBACKS > 0
typedef void (*gpSched_OnIdleCallback_t)(void);

/**
 * @brief Register a function that should be called when nothing is scheduled.
 *        This is also referred to as post processing.
 *        The callback should be:
 *        - non-blocking
 *        - Use the scheduler for deffered work (using gpSched_ScheduleEvent)
 *
 * @param  callback   Function that will be triggered when the scheduler(OS) is idle.
*/
GP_API void gpSched_RegisterOnIdleCallback(gpSched_OnIdleCallback_t callback);

/**
 * @brief Execute all registered idle callback functions
 *
*/
GP_API void gpSched_PostProcessIdle(void);
#endif /* GP_SCHED_NR_OF_IDLE_CALLBACKS > 0*/

/**
 * @brief the MAIN_FUNCTION_NAME define will be used as entry point for the gpSched scheduler
 *
 * When defining MAIN_FUNCTION_NAME to a certain function, another main() implementation is expected
 * To start the normal gpSched operation, the MAIN_FUNCTION_NAME can be called.
 * gpSched will then loop infinitely, executing any scheduled event and servicing stack functionality
*/
#ifndef GP_SCHED_EXTERNAL_MAIN
#ifdef MAIN_FUNCTION_NAME
GP_API MAIN_FUNCTION_RETURN_TYPE MAIN_FUNCTION_NAME(void);
#else //MAIN_FUNCTION_NAME
#define MAIN_FUNCTION_NAME main
#endif //MAIN_FUNCTION_NAME
#endif // not defined GP_SCHED_EXTERNAL_MAIN

GP_API void gpSched_Main_Init(void);

#define GP_SCHED_GET_TIME_DIFF(earliestTs, latestTs) ((UInt32)(latestTs) - (UInt32)(earliestTs))


/**
 * @brief A function that checks if an schedule event should be deferred to the context of the task
 *
 * @return success True if need be deffered, False if called from task without deffer.
 */
Bool gpSched_ScheduleEventDeferred(UInt32 rel_time, gpSched_EventCallback_t callback, void* arg);

/**
 * @brief A function that notifies the gpSched task to process new events.
 */
void gpSched_Trigger(void);
#ifdef __cplusplus
}
#endif

#endif // _GPSCHED_H_
