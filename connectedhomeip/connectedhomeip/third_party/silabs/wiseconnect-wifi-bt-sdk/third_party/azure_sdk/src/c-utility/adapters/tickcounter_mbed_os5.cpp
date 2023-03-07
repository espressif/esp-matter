// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "mbed.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/tickcounter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The tick counter from mbed OS will be overflow (go back to zero) after approximately 70 minutes (4294s).
// So here extend the  tick counter 64bit.
static Ticker *cycle_ticker = NULL;
static volatile uint32_t last_ticker_us = 0;
static volatile uint64_t long_ticker_ms = 0;

#define TICKER_INTERVAL 60.0

static uint64_t system_tick_counter_read(void)
{
    uint64_t result;
    uint32_t t;

    core_util_critical_section_enter();
    t = us_ticker_read();
    if (t < last_ticker_us)
    {
        // overflowed
        long_ticker_ms += 0xFFFFFFFF / 1000;
    }
    last_ticker_us = t;

    result = long_ticker_ms + t / 1000;
    core_util_critical_section_exit();

    return result;
}

static void cycle_accumulator(void)
{
    system_tick_counter_read();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The tickcounter
typedef struct TICK_COUNTER_INSTANCE_TAG
{
    uint64_t current_ms;
} TICK_COUNTER_INSTANCE;

TICK_COUNTER_HANDLE tickcounter_create(void)
{
    core_util_critical_section_enter();
    if (cycle_ticker == NULL)
    {
        cycle_ticker = new Ticker();
        cycle_ticker->attach(cycle_accumulator, TICKER_INTERVAL);
    }
    core_util_critical_section_exit();

    TICK_COUNTER_INSTANCE *result;
    result = new TICK_COUNTER_INSTANCE;
    result->current_ms = system_tick_counter_read();
    return result;
}

void tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter)
{
    if (tick_counter != NULL)
    {
        delete tick_counter;
    }
}

int tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, tickcounter_ms_t *current_ms)
{
    int result;
    if (tick_counter == NULL || current_ms == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        tick_counter->current_ms = system_tick_counter_read();
        *current_ms = tick_counter->current_ms;

        result = 0;
    }
    return result;
}
