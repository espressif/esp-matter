// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include <stdint.h>
#include <time.h>

/* Codes_SRS_TICKCOUNTER_FREERTOS_30_001: [ The tickcounter_freertos adapter shall use the following data types as defined in tickcounter.h. */
/* Codes_SRS_TICKCOUNTER_FREERTOS_30_002: [ The tickcounter_freertos adapter shall implement the API calls defined in tickcounter.h */
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct TICK_COUNTER_INSTANCE_TAG
{
    uint32_t original_tick_count;
} TICK_COUNTER_INSTANCE;

TICK_COUNTER_HANDLE tickcounter_create(void)
{
    /* Codes_SRS_TICKCOUNTER_FREERTOS_30_003: [ tickcounter_create shall allocate and initialize an internally-defined TICK_COUNTER_INSTANCE structure and return its pointer on success.] */
    TICK_COUNTER_INSTANCE* result = (TICK_COUNTER_INSTANCE*)malloc(sizeof(TICK_COUNTER_INSTANCE));
    if (result == NULL)
    {
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_004: [ If allocation of the internally-defined TICK_COUNTER_INSTANCE structure fails,  tickcounter_create shall return NULL.] */
        LogError("Failed creating tick counter");
    }
    else
    {
        // The FreeRTOS call xTaskGetTickCount has no failure path
        // Store the initial tick count in order to meet these two requirements:
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_009: [ tickcounter_get_current_ms shall set *current_ms to the number of milliseconds elapsed since the tickcounter_create call for the specified tick_counter and return 0 to indicate success in situations where the FreeRTOS call xTaskGetTickCount has not experienced overflow between the calls to tickcounter_create and tickcounter_get_current_ms. (In FreeRTOS this call has no failure case.) ] */
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_010: [ tickcounter_get_current_ms shall set *current_ms to the number of milliseconds elapsed since the tickcounter_create call for the specified tick_counter and return 0 to indicate success in situations where the FreeRTOS call xTaskGetTickCount has experienced overflow between the calls to tickcounter_create and tickcounter_get_current_ms. (In FreeRTOS this call has no failure case.) ] */
        result->original_tick_count = xTaskGetTickCount();
    }
    return result;
}

void tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter)
{
    /* Codes_SRS_TICKCOUNTER_FREERTOS_30_006: [ If the tick_counter parameter is NULL, tickcounter_destroy shall do nothing. ] */
    if (tick_counter != NULL)
    {
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_005: [ tickcounter_destroy shall delete the internally-defined TICK_COUNTER_INSTANCE structure specified by the tick_counter parameter. (This call has no failure case.) ] */
        free(tick_counter);
    }
}

int tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, tickcounter_ms_t * current_ms)
{
    int result;

    if (tick_counter == NULL || current_ms == NULL)
    {
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_007: [ If the tick_counter parameter is NULL, tickcounter_get_current_ms shall return a non-zero value to indicate error. ] */
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_008: [ If the current_ms parameter is NULL, tickcounter_get_current_ms shall return a non-zero value to indicate error. ] */
        LogError("Invalid Arguments.");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_009: [ tickcounter_get_current_ms shall set *current_ms to the number of milliseconds elapsed since the tickcounter_create call for the specified tick_counter and return 0 to indicate success (In FreeRTOS this call has no failure case.) ] */
        /* Codes_SRS_TICKCOUNTER_FREERTOS_30_010: [ If the FreeRTOS call xTaskGetTickCount experiences a single overflow between the calls to tickcounter_create and tickcounter_get_current_ms, the tickcounter_get_current_ms call shall still return the correct interval. ] */
        *current_ms = (tickcounter_ms_t)(
            // Subtraction of two uint32_t's followed by a cast to uint32_t
            // ensures that the result remains valid until the real difference exceeds 32 bits.
            ((uint32_t)(xTaskGetTickCount() - tick_counter->original_tick_count))
            // Now that overflow behavior is ensured it is safe to scale. CONFIG_FREERTOS_HZ is typically
            // equal to 1000 or less, so overflow won't happen until the 49.7 day limit
            // of this call's effective uint32_t return value.
            * 1000.0 / CONFIG_FREERTOS_HZ
            );
        result = 0;
    }

    return result;
}
