/*
 * Copyright (c) 2017-2018, 2020-2021, Qorvo Inc
 *
 * alarm_qorvo.c
 *   This file contains the implementation of the qorvo alarm api for openthread.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#define GP_COMPONENT_ID GP_COMPONENT_ID_QVOT

// #define GP_LOCAL_LOG
#define LOG_PREFIX "[Q] Alarm---------: "

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpAssert.h"
#include "gpLog.h"
#include "gpSched.h"

#include "alarm_qorvo.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define QORVO_ALARM_MILLI_WRAP          ((uint32_t)(0xFFFFFFFF / 1000UL))
#define QORVO_ALARM_KEEP_ALIVE_PERIOD   ((uint32_t)QORVO_ALARM_MILLI_WRAP - 10000)  // in milliseconds

#define us_to_ms(x)                     ((x) / 1000UL)
#define ms_to_us(x)                     ((x) * 1000UL)
#define ms_to_s(x)                      ((x) / 1000UL)
#define us_to_s(x)                      ((x) / 1000000UL)
#define ms_remaining(x)                 ((x) % 1000UL)
#define us_remaining(x)                 ((x) % 1000000UL)

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static uint16_t qorvoAlarmWrapCounter;
static uint32_t qorvoAlarmPrev;

typedef struct TimeSAndUS
{
    uint32_t sec;
    uint32_t usec;
} TimeSAndUs_t;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void qorvoAlarmResetWrapCounter(void);
static void qorvoAlarmKeepAlive(void);
static void qorvoAlarmUpdateWrapAround(uint32_t now);
static void qorvoAlarmScheduleKeepAlive(void);
static TimeSAndUs_t qorvoGetSecondsAndMicrosecondsFromMilliseconds(uint32_t milliseconds);
static TimeSAndUs_t qorvoGetSecondsAndMicrosecondsFromMicroseconds(uint32_t microseconds);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

TimeSAndUs_t qorvoGetSecondsAndMicrosecondsFromMilliseconds(uint32_t milliseconds)
{
    TimeSAndUs_t time = {ms_to_s(milliseconds), ms_to_us(ms_remaining(milliseconds))};
    return time;
}

TimeSAndUs_t qorvoGetSecondsAndMicrosecondsFromMicroseconds(uint32_t microseconds)
{
    TimeSAndUs_t time = {us_to_s(microseconds), us_remaining(microseconds)};
    return time;
}

void qorvoAlarmScheduleKeepAlive(void)
{
    TimeSAndUs_t time = qorvoGetSecondsAndMicrosecondsFromMilliseconds(QORVO_ALARM_KEEP_ALIVE_PERIOD);
    gpSched_UnscheduleEventArg((gpSched_EventCallback_t)qorvoAlarmKeepAlive, NULL);
    gpSched_ScheduleEventInSecAndUs(time.sec, time.usec, (gpSched_EventCallback_t)qorvoAlarmKeepAlive, NULL);
}

void qorvoAlarmResetWrapCounter(void)
{
    qorvoAlarmWrapCounter = 0;
    GP_LOG_PRINTF(LOG_PREFIX "QorvoAlarm: internal timer reset. cnt=%u", 0, qorvoAlarmWrapCounter);

    qorvoAlarmPrev = gpSched_GetCurrentTime();
    qorvoAlarmScheduleKeepAlive();
}

void qorvoAlarmKeepAlive(void)
{
    GP_LOG_PRINTF(LOG_PREFIX "QorvoAlarm: Keep Alive triggered", 0);
    qorvoAlarmGetTimeMs();
}

void qorvoAlarmUpdateWrapAround(uint32_t now)
{
    if(now < qorvoAlarmPrev)
    {
        qorvoAlarmWrapCounter += (qorvoAlarmWrapCounter < 1000 ? 1 : -1000);
        GP_LOG_PRINTF(LOG_PREFIX "QorvoAlarm: internal timer wrap. cnt=%u", 0, qorvoAlarmWrapCounter);
        GP_ASSERT_DEV_INT(qorvoAlarmWrapCounter <= 1000);
    }
    qorvoAlarmPrev = now;

    qorvoAlarmScheduleKeepAlive();
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void qorvoAlarmInit(void)
{
    qorvoAlarmResetWrapCounter();
}

uint32_t qorvoAlarmGetTimeMs(void)
{
    uint32_t now = gpSched_GetCurrentTime();

    qorvoAlarmUpdateWrapAround(now);
    return (uint32_t)((uint32_t)us_to_ms(now) + qorvoAlarmWrapCounter * QORVO_ALARM_MILLI_WRAP);
}

uint32_t qorvoAlarmGetTimeUs(void)
{
    return gpSched_GetCurrentTime();
}

void qorvoAlarmMilliStart(uint32_t rel_time, qorvoAlarmCallback_t callback, void* arg)
{
    TimeSAndUs_t time = qorvoGetSecondsAndMicrosecondsFromMilliseconds(rel_time);
    GP_LOG_PRINTF(LOG_PREFIX "Start milli timer for %lu ms -> %lu s, %lu us", 0, (unsigned long)rel_time, (unsigned long)time.sec, (unsigned long)time.usec);
    gpSched_ScheduleEventInSecAndUs(time.sec, time.usec, (gpSched_EventCallback_t)callback, arg);
}

void qorvoAlarmMicroStart(uint32_t rel_time, qorvoAlarmCallback_t callback, void* arg)
{
    TimeSAndUs_t time = qorvoGetSecondsAndMicrosecondsFromMicroseconds(rel_time);
    GP_LOG_PRINTF(LOG_PREFIX "Start micro timer for %lu us -> %lu s, %lu us", 0, (unsigned long)rel_time, (unsigned long)time.sec, (unsigned long)time.usec);
    gpSched_ScheduleEventInSecAndUs(time.sec, time.usec, (gpSched_EventCallback_t)callback, arg);
}

bool qorvoAlarmStop(qorvoAlarmCallback_t callback, void* arg)
{
    return (bool)gpSched_UnscheduleEventArg((gpSched_EventCallback_t)callback, arg);
}

/*****************************************************************************
 *                    Public functions for backwards compatibility
 *****************************************************************************/
void qorvoAlarmScheduleEventArg(uint32_t rel_time, qorvoAlarmCallback_t callback, void* arg)
{
    qorvoAlarmMilliStart(rel_time, callback, arg);
}

bool qorvoAlarmUnScheduleEventArg(qorvoAlarmCallback_t callback, void* arg)
{
    return qorvoAlarmStop(callback, arg);
}

