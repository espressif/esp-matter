/*
 * Copyright (c) 2020-2022, Qorvo Inc
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
 */

/**
 * @file gpSched_il.c
 * @brief GP Scheduler integration layer implementation
 *
 * This file implements the for gpSched integration layer interface APIs
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_SCHED

#include "hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "gpUtils.h"
#include "gpSched.h"
#include "gpHal_ES.h"
#include "gpSched_defs.h"

#include "gpLog.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_SCHED_TASK_NAME       ("taskGPSched")
#define GP_SCHED_TASK_PRIORITY   (configMAX_PRIORITIES - 3)
#ifndef GP_SCHED_TASK_STACK_SIZE
#define GP_SCHED_TASK_STACK_SIZE ((6 * 1024) / 4)
#endif

#define GP_SCHED_TASK_NOTIFY_EVENTQ_MASK    (0x1UL)
#define GP_SCHED_TASK_NOTIFY_TERMINATE_MASK (0x2UL)
#define GP_SCHED_TASK_NOTIFY_ALL_MASK       (GP_SCHED_TASK_NOTIFY_EVENTQ_MASK | GP_SCHED_TASK_NOTIFY_TERMINATE_MASK)

#define SCHED_EVENT_QUEUE_LENGTH 5
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


typedef struct {
    UInt32 rel_time;
    gpSched_EventCallback_t callback;
    void* arg;
} gpSched_EventQueueElement_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/** @brief gpSched FreeRTOS task handle */
static TaskHandle_t gpSched_TaskHandle;

/* The variable used to hold the scheduler event queue's data structure. */
static StaticQueue_t gpSched_EventStaticQueue;
/** @brief gpSched FreeRTOS task info */
static StaticTask_t gpSched_TaskInfo;
/** @brief gpSched FreeRTOS Stack allocation */
static StackType_t gpSched_TaskStack[GP_SCHED_TASK_STACK_SIZE];
/* The array to use as the scheduler event queue's storage area.  This must be at least
uxQueueLength * uxItemSize bytes. */
uint8_t ucSched_EventQueueStorageArea[SCHED_EVENT_QUEUE_LENGTH * sizeof(gpSched_EventQueueElement_t)];

/** @brief ID of claimed HW Absolute Event for kick of gpSched task */
static gpHal_AbsoluteEventId_t gpSched_ESTimerId;


QueueHandle_t gpSched_EventQueue;

/*****************************************************************************
 *                    External Function Prototypes
 *****************************************************************************/

extern void Application_Init(void);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#define Sched_WriteDbgInfo(line, arg1, arg2, arg3)

static void Sched_ScheduleESTimer(UInt32 nextEventTime)
{
    UInt32 currTime = 0;
    UInt32 absTime;
    UInt8 control = 0;

    if(nextEventTime)
    {
        DISABLE_GP_GLOBAL_INT();

        GP_ES_SET_EVENT_RESULT(control, gpHal_EventResultInvalid);
        GP_ES_SET_EVENT_STATE(control, gpHal_EventStateScheduled);
        HAL_TIMER_GET_CURRENT_TIME_1US(currTime);
        absTime = nextEventTime + currTime;
        gpHal_RefreshAbsoluteEvent(gpSched_ESTimerId, absTime, control);

        ENABLE_GP_GLOBAL_INT();
        Sched_WriteDbgInfo(__LINE__, currTime, nextEventTime, absTime);
    }
}

static void Sched_SetupESTimer(void)
{
    gpHal_AbsoluteEventDescriptor_t ev;

    gpSched_ESTimerId = gpHal_GetAbsoluteEvent();
    GP_ASSERT_DEV_EXT(GPHAL_ES_ABSOLUTE_EVENT_ID_INVALID != gpSched_ESTimerId);

    MEMSET(&ev, 0, sizeof(ev));
    ev.type = GP_WB_ENUM_EVENT_TYPE_DUMMY;
    /* we need ex_itl when waking up very fast */
    ev.executionOptions = GP_ES_EXECUTION_OPTIONS_EXECUTE_IF_TOO_LATE;
    ev.interruptOptions = GP_ES_INTERRUPT_OPTIONS_MASK;
    gpHal_GetTime(&ev.exTime);
    GP_ES_SET_EVENT_STATE(ev.control, gpHal_EventStateScheduled);

    gpHal_ScheduleAbsoluteEvent(&ev, gpSched_ESTimerId);
    gpHal_UnscheduleAbsoluteEvent(gpSched_ESTimerId);
    gpHal_EnableAbsoluteEventCallbackInterrupt(gpSched_ESTimerId, true);
}

static void Sched_Main(void* params)
{
    (void)params;

    Sched_SetupESTimer();

    GP_UTILS_CPUMON_INIT();

    /* scheduler task loop */
    for(;;)
    {
        UInt32 notificationVal;
        UInt32 nextEventTime = 0UL;
        Bool timerScheduled = false;
        /* wait for the notification */
        xTaskNotifyWait(0x0UL, GP_SCHED_TASK_NOTIFY_ALL_MASK, &notificationVal, portMAX_DELAY);
        if((notificationVal & GP_SCHED_TASK_NOTIFY_EVENTQ_MASK) != 0UL)
        {
            GP_UTILS_CPUMON_NEW_SCHEDULER_LOOP();
            gpSched_Main_Body();
            while((!gpSched_EventQueueEmpty() && (timerScheduled == false)) || (HAL_RADIO_INT_CHECK_IF_OCCURED()))
            {
                gpSched_Main_Body();
                if(!gpSched_EventQueueEmpty())
                {
                    nextEventTime = gpSched_GetTimeToNextEvent();
                    if((nextEventTime > 0) && (timerScheduled == false))
                    {
                        Sched_WriteDbgInfo(__LINE__, 0, nextEventTime, 0);
                        Sched_ScheduleESTimer(nextEventTime);
                        timerScheduled = true;
                    }
                }
                else
                {
                    Sched_WriteDbgInfo(__LINE__, 0, 0, 0);
                }
            }
#if GP_SCHED_NR_OF_IDLE_CALLBACKS > 0
            gpSched_PostProcessIdle();
#endif
        }
        /* terminate signal received */
        if((notificationVal & GP_SCHED_TASK_NOTIFY_TERMINATE_MASK) != 0UL)
        {
            break;
        }
    }
    /* shall never come here ideally. For safety reason, delete the task before returning */
    vTaskDelete(NULL);
}

static void gpSched_DeferredEvent(void* pvParameter1, uint32_t ulParameter2)
{
    (void)pvParameter1;
    (void)ulParameter2;

    gpSched_EventQueueElement_t queueElement = {0};

    if(xQueueReceive(gpSched_EventQueue, &(queueElement), (TickType_t)0) == pdPASS)
    {
        gpSched_ScheduleEventArg(queueElement.rel_time, queueElement.callback, queueElement.arg);
    }
}

/*****************************************************************************
 *                    FreeRTOS weak overrides
 *****************************************************************************/
void vApplicationIdleHook(void)
{
    // Kick watchdog in FreeRTOS idle loop
    HAL_WDT_RESET();
}

void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    // No actual sleep performed - implementing override to avoid FreeRTOS default to run.
    // hal_SleepFreeRTOS.c is used to activate full system sleep with FreeRTOS
    NOT_USED(xExpectedIdleTime);
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/* This function can be called either from interrupt context (handler mode)
 * or thread context (thread mode) to signal to gpSched task. This function
 * internally takes care of either sending signal to task using xTaskNotifyFromISR()
 * or xTaskNotify() function of FreeRTOS
 */
void gpSched_NotifySchedTask(void)
{
    xPSR_Type psr;
    psr.w = __get_xPSR();
    if(psr.b.ISR != 0)
    {
        xTaskNotifyFromISR(gpSched_TaskHandle, GP_SCHED_TASK_NOTIFY_EVENTQ_MASK, eSetBits, NULL);
    }
    else
    {
        xTaskNotify(gpSched_TaskHandle, GP_SCHED_TASK_NOTIFY_EVENTQ_MASK, eSetBits);
    }
}

Bool gpSched_ScheduleEventDeferred(UInt32 rel_time, gpSched_EventCallback_t callback, void* arg)
{
    xPSR_Type psr;
    psr.w = __get_xPSR();

    if(psr.b.ISR != 0)
    {
        gpSched_EventQueueElement_t queueElement = {
            .rel_time = rel_time,
            .callback = callback,
            .arg = arg,
        };
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(gpSched_EventQueue, &queueElement, &xHigherPriorityTaskWoken);
        xTimerPendFunctionCallFromISR(gpSched_DeferredEvent, NULL, 0, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        return true;
    }
    else
    {
        return false;
    }
}

#ifndef GP_SCHED_EXTERNAL_MAIN
MAIN_FUNCTION_RETURN_TYPE MAIN_FUNCTION_NAME(void)
{
    HAL_INITIALIZE_GLOBAL_INT();

    // Hardware initialization
    HAL_INIT();

    HAL_ENABLE_GLOBAL_INT();

    // Initialize gpSched + task
#if !defined(GP_BASECOMPS_DIVERSITY_NO_GPSCHED_INIT)
    gpSched_Init();
#endif
    // Initialize within gpSched task
    gpSched_ScheduleEvent(0, Application_Init);

    vTaskStartScheduler();

    return MAIN_FUNCTION_RETURN_VALUE;
}
#endif // GP_SCHED_EXTERNAL_MAIN

void Sched_Integration_Init(void)
{

    gpSched_EventQueue = xQueueCreateStatic(SCHED_EVENT_QUEUE_LENGTH, sizeof(gpSched_EventQueueElement_t),
                                          ucSched_EventQueueStorageArea, &gpSched_EventStaticQueue);

    gpSched_TaskHandle = xTaskCreateStatic(Sched_Main, GP_SCHED_TASK_NAME, GP_SCHED_TASK_STACK_SIZE, NULL,
                                           GP_SCHED_TASK_PRIORITY, gpSched_TaskStack, &gpSched_TaskInfo);

    GP_ASSERT_SYSTEM(NULL != gpSched_EventQueue);
    GP_ASSERT_SYSTEM(NULL != gpSched_TaskHandle);
}

void Sched_Integration_DeInit(void)
{
    /* dummy */
    return;
}
void gpSched_Trigger(void)
{
    gpSched_NotifySchedTask();
}
