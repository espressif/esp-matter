/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/xlogging.h"
#include "cmsis_os2.h"
#include "iotsdk_alloc.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct TICK_COUNTER_INSTANCE_TAG {
    uint32_t previous_tick_count;
    tickcounter_ms_t current_ms;
} TICK_COUNTER_INSTANCE;

TICK_COUNTER_HANDLE tickcounter_create(void)
{
    TICK_COUNTER_INSTANCE *tick_counter;

    tick_counter = iotsdk_alloc_malloc(sizeof(TICK_COUNTER_INSTANCE));

    if (tick_counter == NULL) {
        LogError("Memory allocation for tick counter failed");
        return NULL;
    }

    tick_counter->current_ms = 0;
    tick_counter->previous_tick_count = osKernelGetTickCount();
    return tick_counter;
}

void tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter)
{
    if (tick_counter != NULL) {
        iotsdk_alloc_free(tick_counter);
    }
}

int tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, tickcounter_ms_t *current_ms)
{
    if (tick_counter == NULL || current_ms == NULL) {
        LogError("Invalid arguments");
        return MU_FAILURE;
    }

    // Read the current tick count and calculate the total milliseconds elapsed since the timer started.
    // As the readings are in microseconds and the counter is 32 bit, it overflows every 72
    // minutes. The current_ms line below corrects for overflow when current_tick_count <= original_tick_count but if
    // e.g. original_tick_count = 10, there are only 10 us after an overflow in which current_tick_count <=
    // previous_tick_count. We also cannot correct for multiple overflows between readings.
    uint32_t current_tick_count = osKernelGetTickCount();
    uint32_t ticks = current_tick_count - tick_counter->previous_tick_count;
    tick_counter->current_ms += (tickcounter_ms_t)((tickcounter_ms_t)ticks * 1000.0 / osKernelGetTickFreq());
    *current_ms = tick_counter->current_ms;
    tick_counter->previous_tick_count = current_tick_count;
    return 0;
}
