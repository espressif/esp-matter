/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for the alarm.
 */

#include <openthread/platform/alarm-milli.h>

#include <slist.h>
#include <stdio.h>
#include <string.h>

#include <clock_timer.h>
#include <wiced_platform.h>
#include <wiced_platform_memory.h>
#include <wiced_rtos.h>
#include <wiced_timer.h>

#ifndef ALARM_DEBUG
#define ALARM_DEBUG 0
#endif // ALARM_DEBUG

#if (ALARM_DEBUG != 0)
#define ALARM_TRACE(format, ...) printf(format, ##__VA_ARGS__)
#else
#define ALARM_TRACE(...)
#endif

#ifndef ALARM_TRIGGER_REPLACE
#define ALARM_TRIGGER_REPLACE 1
#endif // ALARM_TRIGGER_REPLACE

#ifndef XTAL_ACCURACY
#define XTAL_ACCURACY 20 // Crystal frequency accuracy: ±20ppm
#endif

/* Enabled timer information. */
typedef struct alarm_timer_info
{
    slist_node_t  node;
    otInstance *  aInstance;
    wiced_timer_t timer;
} alarm_timer_info_t;

/* Fired alarm. */
typedef struct alarm_fired
{
    slist_node_t node;
    otInstance * aInstance;
} alarm_fired_t;

/* Alarm module control block. */
typedef struct alarm_cb
{
    wiced_bool_t   initialized;
    uint32_t       event_code;
    slist_node_t   enabled_timer_list; /* Refer to alarm_timer_info_t. */
    wiced_mutex_t *p_mutex;
    slist_node_t   fired_alarm_list; /* Refer to alarm_fired_t. */
} alarm_cb_t;

static alarm_cb_t alarm_cb = {0};

static wiced_bool_t alarmEnabledTimerListCheck(alarm_timer_info_t *p_target)
{
    slist_node_t *p_node;
    int           i;

    for (i = 0; i < slist_count(&alarm_cb.enabled_timer_list); i++)
    {
        p_node = slist_get(&alarm_cb.enabled_timer_list);
        slist_add_tail(p_node, &alarm_cb.enabled_timer_list);

        if (p_node == (slist_node_t *)p_target)
        {
            return WICED_TRUE;
        }
    }

    return WICED_FALSE;
}

static alarm_timer_info_t *alarmEnabledTimerListAdd(otInstance *aInstance)
{
    slist_node_t *p_node;

    /* Allocate memory */
    p_node = (slist_node_t *)wiced_platform_memory_allocate(sizeof(alarm_timer_info_t));

    if (!p_node)
    {
        return NULL;
    }

    memset((void *)p_node, 0, sizeof(alarm_timer_info_t));

    /* Add to the tail of enabled timer list. */
    slist_add_tail(p_node, &alarm_cb.enabled_timer_list);

    /* Store information. */
    ((alarm_timer_info_t *)p_node)->aInstance = aInstance;

    return (alarm_timer_info_t *)p_node;
}

static void alarmEnabledTimerListRemove(alarm_timer_info_t *p_target)
{
    slist_node_t *p_node;
    int           i;

    for (i = 0; i < slist_count(&alarm_cb.enabled_timer_list); i++)
    {
        p_node = slist_get(&alarm_cb.enabled_timer_list);

        if (p_node == (slist_node_t *)p_target)
        {
            /* Free memory. */
            wiced_platform_memory_free((void *)p_node);

            break;
        }
        else
        {
            slist_add_tail(p_node, &alarm_cb.enabled_timer_list);
        }
    }
}

static void alarmTimerCallback(WICED_TIMER_PARAM_TYPE cb_params)
{
    otInstance *  aInstance;
    slist_node_t *p_node;

    wiced_rtos_lock_mutex(alarm_cb.p_mutex);

    /* Check if the target entry exists in the enabled timer list. */
    if (!alarmEnabledTimerListCheck((alarm_timer_info_t *)cb_params))
    {
        wiced_rtos_unlock_mutex(alarm_cb.p_mutex);
        return;
    }

    aInstance = ((alarm_timer_info_t *)cb_params)->aInstance;

    /* Remove the entry from the enable timer list. */
    alarmEnabledTimerListRemove((alarm_timer_info_t *)cb_params);

    /* Add a new entry to the fired alarm list. */
    p_node = (slist_node_t *)wiced_platform_memory_allocate(sizeof(alarm_fired_t));

    if (!p_node)
    {
        ALARM_TRACE("%s: Err: Cannot allocate fired alarm entry\n", __FUNCTION__);

        wiced_rtos_unlock_mutex(alarm_cb.p_mutex);
        return;
    }

    slist_add_tail(p_node, &alarm_cb.fired_alarm_list);

    /* Store information. */
    ((alarm_fired_t *)p_node)->aInstance = aInstance;

    /* Set an application thread event for this fired alarm. */
    wiced_platform_application_thread_event_set(alarm_cb.event_code);

    wiced_rtos_unlock_mutex(alarm_cb.p_mutex);
}

static void alarmTimeoutHandler(void)
{
    slist_node_t *p_node;
    otInstance *  aInstance;

    ALARM_TRACE("%s\n", __FUNCTION__);

    wiced_rtos_lock_mutex(alarm_cb.p_mutex);

    /* Check the fired alarm list.*/
    if (slist_count(&alarm_cb.fired_alarm_list) <= 0)
    {
        wiced_rtos_unlock_mutex(alarm_cb.p_mutex);

        return;
    }

    /* Get the first entry from the fired alarm list. */
    p_node = slist_get(&alarm_cb.fired_alarm_list);

    aInstance = ((alarm_fired_t *)p_node)->aInstance;

    wiced_platform_memory_free((void *)p_node);

    wiced_rtos_unlock_mutex(alarm_cb.p_mutex);

    /* Signal the fired alarm. */
#if OPENTHREAD_CONFIG_DIAG_ENABLE
    if (otPlatDiagModeGet())
    {
        otPlatDiagAlarmFired(aInstance);
    }
    else
#endif /* OPENTHREAD_CONFIG_DIAG_ENABLE */
    {
        otPlatAlarmMilliFired(aInstance);
    }

    /* Set an application thread event if the fired alarm list is not empty. */
    if (slist_count(&alarm_cb.fired_alarm_list) > 0)
    {
        wiced_platform_application_thread_event_set(alarm_cb.event_code);
    }
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
#if ALARM_TRIGGER_REPLACE
    int i;
#endif // ALARM_TRIGGER_REPLACE
    slist_node_t *p_node;
    uint32_t      target_timeout;
    uint32_t      time_now;

    ALARM_TRACE("%s (0x%p, %lu, %lu)\n", __FUNCTION__, aInstance, aT0, aDt);

    wiced_platform_application_thread_check();

    /* Check state. */
    if (!alarm_cb.initialized)
    {
        return;
    }

    /* Check parameter. */
    if (!aInstance)
    {
        return;
    }

    wiced_rtos_lock_mutex(alarm_cb.p_mutex);

#if ALARM_TRIGGER_REPLACE
    /* Check if the entry already exists in the enabled timer list. */
    for (i = 0; i < slist_count(&alarm_cb.enabled_timer_list); i++)
    {
        p_node = slist_get(&alarm_cb.enabled_timer_list);

        if (((alarm_timer_info_t *)p_node)->aInstance == aInstance)
        {
            /* Stop timer. */
            wiced_stop_timer(&(((alarm_timer_info_t *)p_node)->timer));

            /* Free memory. */
            wiced_platform_memory_free((void *)p_node);

            break;
        }
        else
        {
            slist_add_tail(p_node, &alarm_cb.enabled_timer_list);
        }
    }
#endif

    /* Add an entry to the enabled timer list and store information. */
    p_node = (slist_node_t *)alarmEnabledTimerListAdd(aInstance);

    if (!p_node)
    {
        ALARM_TRACE("Err: cannot add an enabled timer entry.\n");
        wiced_rtos_unlock_mutex(alarm_cb.p_mutex);

        return;
    }

    /* Calculate target timeout. */
    time_now = otPlatAlarmMilliGetNow();

    if (time_now >= aT0)
    { // Current time exceeds the target reference (start) time.
        if (aDt <= (time_now - aT0))
        { // Current time exceeds the target fired time.
            /* Trigger this timer immediately. */
            wiced_rtos_unlock_mutex(alarm_cb.p_mutex);

            alarmTimerCallback((WICED_TIMER_PARAM_TYPE)p_node);

            return;
        }
        else
        {
            target_timeout = aDt - (time_now - aT0);
        }
    }
    else
    {
        target_timeout = aDt + (aT0 - time_now);
    }

    /* Initialize the timer module. */
    wiced_init_timer(&(((alarm_timer_info_t *)p_node)->timer), alarmTimerCallback, (WICED_TIMER_PARAM_TYPE)p_node,
                     WICED_MILLI_SECONDS_TIMER);

    /* Start timer. */
    wiced_start_timer(&(((alarm_timer_info_t *)p_node)->timer), target_timeout);

    wiced_rtos_unlock_mutex(alarm_cb.p_mutex);
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    int           i;
    slist_node_t *p_node;

    ALARM_TRACE("%s (%p)\n", __FUNCTION__, aInstance);

    wiced_platform_application_thread_check();

    wiced_rtos_lock_mutex(alarm_cb.p_mutex);

    /* Find the target alarm. */
    for (i = 0; i < slist_count(&alarm_cb.enabled_timer_list); i++)
    {
        p_node = slist_get(&alarm_cb.enabled_timer_list);

        if (((alarm_timer_info_t *)p_node)->aInstance == aInstance)
        {
            /* Stop timer. */
            wiced_stop_timer(&(((alarm_timer_info_t *)p_node)->timer));

            /* Free memory. */
            wiced_platform_memory_free((void *)p_node);
        }
        else
        {
            slist_add_tail(p_node, &alarm_cb.enabled_timer_list);
        }
    }

    wiced_rtos_unlock_mutex(alarm_cb.p_mutex);
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    return (uint32_t)(clock_SystemTimeMicroseconds64() / 1000);
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return clock_SystemTimeMicroseconds32();
}

void otPlatAlramInit(void)
{
    if (alarm_cb.initialized)
    {
        return;
    }

    /* Get/Register the application thread event code. */
    if (!wiced_platform_application_thread_event_register(&alarm_cb.event_code, alarmTimeoutHandler))
    {
        ALARM_TRACE("%s: Fail to get event code.\n", __FUNCTION__);
        return;
    }

    /* Create mutex. */
    alarm_cb.p_mutex = wiced_rtos_create_mutex();

    if (alarm_cb.p_mutex == NULL)
    {
        ALARM_TRACE("%s: Fail to create mutex.\n", __FUNCTION__);
        return;
    }

    /* Initialize the mutex. */
    if (wiced_rtos_init_mutex(alarm_cb.p_mutex) != WICED_SUCCESS)
    {
        ALARM_TRACE("%s: Fail to init. mutex.\n", __FUNCTION__);
        return;
    }

    /* Initialize the enabled timer list. */
    INIT_SLIST_NODE(&alarm_cb.enabled_timer_list);

    /* Initialize the fired alarm list. */
    INIT_SLIST_NODE(&alarm_cb.fired_alarm_list);

    alarm_cb.initialized = WICED_TRUE;
}

uint16_t otPlatTimeGetXtalAccuracy(void)
{
    return XTAL_ACCURACY;
}
