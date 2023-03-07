/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpHal_OscillatorBenchmark.c
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


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

//#define GP_LOCAL_LOG

#include <math.h>
#include "gpLog.h"
#include "gpHal_OscillatorBenchmark.h"
#include "gpAssert.h"


#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// How many measurements to use for calculating the average benchmark value
#define GP_HAL_ES_NR_OF_BENCHMARK_MEASUREMENTS_HISTORY      8

#ifndef GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER
#define GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER         8
#endif // GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER

#define GPHAL_ES_BENCHMARK_MAX_SUM                  0xFFFFFFFF
#define GPHAL_ES_BENCHMARK_MAX_MSE                  0xFFFFFFFF

#define GPHAL_ES_BENCHMARK_COUNTER_INVALID          0xFFFFFFFF /* FIMXE: rename */
#define GPHAL_ES_BENCHMARK_COUNTER_CONTINUE         0

// Number of retries before we will abort the oscillator benchmark measurements
#define GPHAL_ES_OSCILLATOR_BENCHMARK_MAX_PHASE1_RETRIES_32_KHZ_XTAL       200

/* There are 3 phases that can be done */
#ifndef GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
#define GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE               3
#endif

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define ARRAY_LENGTH(x) (sizeof(x)/sizeof(*x))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef enum _gpHal_OscillatorBenchmark_Phase {
    gpHal_OscillatorBenchmark_Result_Phase_Stabilize = 0,
#if 2 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
    gpHal_OscillatorBenchmark_Result_Phase_Average = 1,
#if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
    gpHal_OscillatorBenchmark_Result_Phase_MSE = 2,
#endif // 3
#endif // 2
} gpHal_OscillatorBenchmark_Phase_t;

typedef struct _gpHal_OscillatorBenchmark_RunAvg8
{
    UInt32 circ_buff[8];
    UInt8 idx;
    UInt8 count;
} gpHal_OscillatorBenchmark_RunAvg8_t;

typedef struct _gpHal_OscillatorBenchmark_StableCount
{
    UInt32 value;
    UInt8 length;
    UInt8 count;
} gpHal_OscillatorBenchmark_StableCount_t;

typedef struct _gpHal_OscillatorBenchmark_Average
{
    UInt32 sum;
} gpHal_OscillatorBenchmark_Average_t;

typedef struct _gpHal_OscillatorBenchmark_MSE
{
    UInt32 avg;
    UInt32 sum;
} gpHal_OscillatorBenchmark_MSE_t;

typedef struct _gpHal_OscillatorBenchmark_3Phase
{
    UInt16 count;
    gpHal_OscillatorBenchmark_Phase_t phase;
    union {
        gpHal_OscillatorBenchmark_StableCount_t stabilize;
        gpHal_OscillatorBenchmark_Average_t average;
        gpHal_OscillatorBenchmark_MSE_t mse;
    } state;
} gpHal_OscillatorBenchmark_3Phase_t;



/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static gpHal_OscillatorBenchmark_3Phase_t gpHalEs_state_32kHz;
static gpHal_OscillatorBenchmark_RunAvg8_t gpHalEs_state_RC;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#if 2 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
static UInt32 gpHal_OscillatorBenchmark_Phase2_GetAvg(gpHal_OscillatorBenchmark_Average_t *state)
{
    return state->sum >> GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER;
}
#endif

static Bool gpHal_isValidBenchMark(UInt32 benchmark)
{
    if (GPHAL_ES_BENCHMARK_COUNTER_INVALID == benchmark)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static Bool gpHal_OscillatorBenchmark_HasSufficientStableMeasurements(gpHal_OscillatorBenchmark_StableCount_t *state)
{
    return state->length <= (state->count+1);
}

static gpHal_OscillatorBenchmark_Status_t gpHal_OscillatorBenchmark_Phase1_Add(gpHal_OscillatorBenchmark_StableCount_t *state, UInt32 benchmark, UInt16 count)
{
    // If crystal not stable or measurement is 0 ignore this measurement
    if (gpHal_isValidBenchMark(benchmark))
    {
        // Check for consecutive similar values with similar defined as +/- 1
        if ( (state->value-1) <= benchmark && benchmark <= (state->value+1) )
        {
            ++state->count;
        }
        else
        {
            state->value = benchmark;
            state->count = 0;
        }

        if ( gpHal_OscillatorBenchmark_HasSufficientStableMeasurements(state) )
        {
            return gpHal_OscillatorBenchmark_Result_Stable;
        }
    }

    if(GPHAL_ES_OSCILLATOR_BENCHMARK_MAX_PHASE1_RETRIES_32_KHZ_XTAL == count)
    {
        return gpHal_OscillatorBenchmark_Result_Unstable;
    }

    return gpHal_OscillatorBenchmark_Result_NeedMoreSamples;
}

#if 2 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
static gpHal_OscillatorBenchmark_Status_t gpHal_OscillatorBenchmark_Phase2_Add(gpHal_OscillatorBenchmark_Average_t *state, UInt32 benchmark, UInt16 count)
{
    if (!gpHal_isValidBenchMark(benchmark))
    {
        return gpHal_OscillatorBenchmark_Result_Broken;
    }

    state->sum += benchmark;

    if ( (1<<GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER) == count )
    {
        return gpHal_OscillatorBenchmark_Result_Unstable;
    }
    return gpHal_OscillatorBenchmark_Result_NeedMoreSamples;
}
#endif // GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE


#if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
static gpHal_OscillatorBenchmark_Status_t gpHal_OscillatorBenchmark_Phase3_Add(gpHal_OscillatorBenchmark_MSE_t *state, UInt32 benchmark, UInt16 count)
{
    /* For Mean Square Error calculation */
    UInt32 error = benchmark < state->avg ? state->avg - benchmark : benchmark - state->avg;
    UInt32 old_sum = state->sum;
    state->sum += error * error;
    /* catch overflow */
    state->sum = state->sum < old_sum ? GPHAL_ES_BENCHMARK_MAX_SUM : state->sum;

    if ( (1<<GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER) == count)
    {
        return gpHal_OscillatorBenchmark_Result_Unstable;
    }

    return gpHal_OscillatorBenchmark_Result_NeedMoreSamples;
}
#endif // GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE

static UInt32 gpHal_OscillatorBenchmark_RunAvg8_GetAvg(gpHal_OscillatorBenchmark_RunAvg8_t *state)
{
    UInt32 sum = 0;
    UInt8 idx;

    for (idx=0; idx < state->count; ++idx)
    {
        sum += state->circ_buff[idx];
    }

    return (sum/state->count);
}

#if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
static UInt32 gpHal_OscillatorBenchmark_MSE_GetMSE(gpHal_OscillatorBenchmark_MSE_t *state)
{
    UInt32 mse;
    if (state->sum < GPHAL_ES_BENCHMARK_MAX_SUM)
    {
        mse = state->sum >> GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER;
    }
    else /* if overflow */
    {
        mse = GPHAL_ES_BENCHMARK_MAX_MSE;
    }
    return mse;
}
#endif

#if 2 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
static void gpHal_OscillatorBenchmark_3Phase_Init_Phase2(gpHal_OscillatorBenchmark_3Phase_t *state)
{
    state->phase = gpHal_OscillatorBenchmark_Result_Phase_Average;
    state->count = 0;
    state->state.average.sum = 0;
}
#endif

#if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
static void gpHal_OscillatorBenchmark_3Phase_Init_Phase3(gpHal_OscillatorBenchmark_3Phase_t *state)
{
    UInt32 avg = gpHal_OscillatorBenchmark_Phase2_GetAvg(&state->state.average);
    state->state.mse.avg = avg;
    state->phase = gpHal_OscillatorBenchmark_Result_Phase_MSE;
    state->state.mse.sum = 0;
    state->count = 0;
}
#endif

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/


void gpHal_OscillatorBenchmark_RunAvg8_Init(void)
{
    gpHal_OscillatorBenchmark_RunAvg8_t *state = &gpHalEs_state_RC;
    MEMSET(state,0,sizeof(gpHal_OscillatorBenchmark_RunAvg8_t));
}

UInt32 gpHal_OscillatorBenchmark_RunAvg8_Add(UInt32 benchmark)
{
    gpHal_OscillatorBenchmark_RunAvg8_t *state = &gpHalEs_state_RC;

    state->circ_buff[state->idx] = benchmark;
    // circular increment of index
    state->idx = (state->idx + 1) % ARRAY_LENGTH(state->circ_buff);

    ++state->count;
    state->count = state->count < ARRAY_LENGTH(state->circ_buff) ? state->count : ARRAY_LENGTH(state->circ_buff);

    return gpHal_OscillatorBenchmark_RunAvg8_GetAvg(state);
}

void gpHal_OscillatorBenchmark_3Phase_Init(UInt8 stable_length)
{
    gpHal_OscillatorBenchmark_3Phase_t *state = &gpHalEs_state_32kHz;

    MEMSET(state,0,sizeof(gpHal_OscillatorBenchmark_3Phase_t));
    state->state.stabilize.length = stable_length;
}

gpHal_OscillatorBenchmark_Status_t gpHal_OscillatorBenchmark_3Phase_Add(UInt32 benchmark)
{
    gpHal_OscillatorBenchmark_3Phase_t *state = &gpHalEs_state_32kHz;

    state->count++;

    gpHal_OscillatorBenchmark_Status_t status = gpHal_OscillatorBenchmark_Result_Broken;

    switch(state->phase)
    {
        case gpHal_OscillatorBenchmark_Result_Phase_Stabilize:
            status = gpHal_OscillatorBenchmark_Phase1_Add(&state->state.stabilize, benchmark, state->count);
            if (gpHal_OscillatorBenchmark_Result_Unstable == status)
            {
                #if 2 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
                    gpHal_OscillatorBenchmark_3Phase_Init_Phase2(state);
                    status = gpHal_OscillatorBenchmark_Result_NeedMoreSamples;
                #else
                    GP_LOG_SYSTEM_PRINTF("32khz crystal unstable after " XSTRINGIFY(GPHAL_ES_OSCILLATOR_BENCHMARK_MAX_PHASE1_RETRIES_32_KHZ_XTAL) " measurements!",0);
                #endif
            }
            break;
#if 2 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
        case gpHal_OscillatorBenchmark_Result_Phase_Average:
            status = gpHal_OscillatorBenchmark_Phase2_Add(&state->state.average, benchmark, state->count);
            if (gpHal_OscillatorBenchmark_Result_Unstable == status)
            {
                #if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
                    gpHal_OscillatorBenchmark_3Phase_Init_Phase3(state);
                    status = gpHal_OscillatorBenchmark_Result_NeedMoreSamples;
                #endif
            }
            break;
#endif // 2
#if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
        case gpHal_OscillatorBenchmark_Result_Phase_MSE:
            status = gpHal_OscillatorBenchmark_Phase3_Add(&state->state.mse, benchmark, state->count);
            break;
#endif // 3
        default:
            GP_ASSERT_SYSTEM(false);
    }
    return status;
}

UInt32 gpHal_OscillatorBenchmark_3Phase_GetMSE(void)
{
    gpHal_OscillatorBenchmark_3Phase_t *state = &gpHalEs_state_32kHz;

    if (state->phase == gpHal_OscillatorBenchmark_Result_Phase_Stabilize &&
        gpHal_OscillatorBenchmark_HasSufficientStableMeasurements(&state->state.stabilize))
    {
        return 0;
    }
    else
#if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
    if (state->phase == gpHal_OscillatorBenchmark_Result_Phase_MSE &&
        state->count == (1<<GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER))
    {
        return gpHal_OscillatorBenchmark_MSE_GetMSE(&state->state.mse);
    }
    else
#endif
    {
        return GPHAL_ES_BENCHMARK_MAX_MSE;
    }
}

UInt32 gpHal_OscillatorBenchmark_3Phase_GetAvg(void)
{
    gpHal_OscillatorBenchmark_3Phase_t *state = &gpHalEs_state_32kHz;

#if 2 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
    if (gpHal_OscillatorBenchmark_Result_Phase_Average == state->phase &&
        state->count == (1<<GPHAL_ES_OSCILLATOR_BENCHMARK_32_KHZ_XTAL_PHASE_2_AVG_POWER) )
    {
        return gpHal_OscillatorBenchmark_Phase2_GetAvg(&state->state.average);
    }
    else
#endif
#if 3 <= GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK_MAX_PHASE
    if (gpHal_OscillatorBenchmark_Result_Phase_MSE == state->phase)
    {
        return state->state.mse.avg;
    }
    else
#endif
    {
        return GPHAL_ES_BENCHMARK_MAX_SUM;
    }
}

UInt32 gpHal_OscillatorBenchmark_MSE_GetStableValue(void)
{
    gpHal_OscillatorBenchmark_StableCount_t *state = &gpHalEs_state_32kHz.state.stabilize;
    return state->value;
}
