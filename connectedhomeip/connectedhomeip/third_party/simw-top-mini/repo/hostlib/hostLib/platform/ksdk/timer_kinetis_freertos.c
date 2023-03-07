/*
 *
 * Copyright 2016-2018 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sm_timer.h>
#include <stdint.h>

#include "board.h"


#if defined(SDK_OS_FREE_RTOS) || defined(FSL_RTOS_FREE_RTOS)

#include "FreeRTOS.h"
#include "task.h"


#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize("O0")
#endif

extern volatile uint32_t gtimer_kinetis_msticks; // counter for 1ms SysTicks


/* initializes the system tick counter
 * return 0 on succes, 1 on failure */
uint32_t sm_initSleep() {
    return 0;
}

/**
 * Implement a blocking (for the calling thread) wait for a number of milliseconds.
 */

#ifndef MSEC_TO_TICK
#define MSEC_TO_TICK(msec) \
	((((uint32_t)configTICK_RATE_HZ * (uint32_t)(msec))) / 1000L)
#endif /* MSEC_TO_TICK */

void sm_sleep(uint32_t msec) {
    vTaskDelay(1 >= pdMS_TO_TICKS(msec) ? 1 : pdMS_TO_TICKS(msec));
}

void vApplicationTickHook() {
    gtimer_kinetis_msticks+=1;
}

#if defined(__GNUC__)
#pragma GCC pop_options
#endif

#endif /* SDK_OS_FREE_RTOS || FSL_RTOS_FREE_RTOS */
