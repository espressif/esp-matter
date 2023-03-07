/*
 *
 * Copyright 2017 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/**
*
* @par Description
* This file implements implements platform independent sleep functionality
* @par History
*
*****************************************************************************/

#include <stdint.h>
#if defined(__gnu_linux__) || defined(__clang__)
#include <unistd.h>
#endif
#include <time.h>
#include "sm_timer.h"

#if defined(USE_RTOS) && USE_RTOS == 1
#include "FreeRTOS.h"
#include "task.h"
#endif

/* initializes the system tick counter
 * return 0 on succes, 1 on failure */
uint32_t sm_initSleep()
{
    return 0;
}

#if defined(USE_RTOS) && USE_RTOS == 1
#ifndef MSEC_TO_TICK
#define MSEC_TO_TICK(msec) \
	((((uint32_t)configTICK_RATE_HZ * (uint32_t)(msec))) / 1000L)
#endif /* MSEC_TO_TICK */
#endif /* USE_RTOS */

/**
 * Implement a blocking (for the calling thread) wait for a number of milliseconds.
 */
void sm_sleep(uint32_t msec)
{
#ifdef __OSX_AVAILABLE
    clock_t goal = msec + clock();
    while (goal > clock());
#elif defined(__gnu_linux__) || defined __clang__
    useconds_t microsec = msec*1000;
    usleep(microsec);
#elif defined(USE_RTOS) && USE_RTOS == 1
    vTaskDelay(1 >= pdMS_TO_TICKS(msec) ? 1 : pdMS_TO_TICKS(msec));
#else
    clock_t goal = msec + clock();
    while (goal > clock());
#endif
}

/**
 * Implement a blocking (for the calling thread) wait for a number of microseconds
 */
void sm_usleep(uint32_t microsec)
{
#ifdef __OSX_AVAILABLE
    // no usleep
#elif defined(_WIN32)
    #pragma message ( "No sm_usleep implemented" )
#elif defined(__gnu_linux__) || defined __clang__
    usleep(microsec);
#elif defined(__OpenBSD__)
	#warning "No sm_usleep implemented"
#else
	//#warning "No sm_usleep implemented"
#endif
}
