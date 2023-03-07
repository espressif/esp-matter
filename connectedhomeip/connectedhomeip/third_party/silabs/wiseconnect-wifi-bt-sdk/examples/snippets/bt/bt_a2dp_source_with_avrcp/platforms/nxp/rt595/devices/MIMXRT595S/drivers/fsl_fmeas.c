/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_fmeas.h"

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.fmeas"
#endif

#if defined(FSL_FEATURE_FMEAS_ASYNC_SYSCON_FREQMECTRL) && (FSL_FEATURE_FMEAS_ASYNC_SYSCON_FREQMECTRL)

/*! @brief Target clock counter value  */
#define TARGET_CLOCK_COUNT(base)                                                                    \
    ((uint32_t)(((((FMEAS_SYSCON_Type *)base)->FREQMECTRL & FMEAS_SYSCON_FREQMECTRL_CAPVAL_MASK) >> \
                 FMEAS_SYSCON_FREQMECTRL_CAPVAL_SHIFT) +                                            \
                1))

/*! @brief Reference clock counter value */
#define REFERENCE_CLOCK_COUNT ((1 << FMEAS_INDEX) - 1)

#elif defined(FSL_FEATURE_SOC_FREQME_COUNT) && (FSL_FEATURE_SOC_FREQME_COUNT)
/*! @brief Target clock counter value.
 * According to user manual, 2 has to be subtracted from RESULT field. */
#define TARGET_CLOCK_COUNT(base) \
    ((uint32_t)((((FREQME_Type *)base)->FREQMECTRL_R & FREQME_FREQMECTRL_R_RESULT_MASK) - 2U))

/*! @brief Reference clock counter value. */
#define REFERENCE_CLOCK_COUNT ((uint32_t)(1U << 20))

#else
/*! @brief Target clock counter value.
 * According to user manual, 2 has to be subtracted from captured value (CAPVAL). */
#define TARGET_CLOCK_COUNT(base) \
    ((uint32_t)(                 \
        ((((SYSCON_Type *)base)->FREQMECTRL & SYSCON_FREQMECTRL_CAPVAL_MASK) >> SYSCON_FREQMECTRL_CAPVAL_SHIFT) - 2))

/*! @brief Reference clock counter value. */
#define REFERENCE_CLOCK_COUNT ((uint32_t)((SYSCON_FREQMECTRL_CAPVAL_MASK >> SYSCON_FREQMECTRL_CAPVAL_SHIFT) + 1))

#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * brief    Returns the computed value for a frequency measurement cycle
 *
 * param    base         : SYSCON peripheral base address.
 * param    refClockRate : Reference clock rate used during the frequency measurement cycle.
 *
 * return   Frequency in Hz.
 */
uint32_t FMEAS_GetFrequency(FMEAS_SYSCON_Type *base, uint32_t refClockRate)
{
    uint32_t targetClockCount = TARGET_CLOCK_COUNT(base);
    uint64_t clkrate          = 0;

    if (((int32_t)targetClockCount) > 0)
    {
        clkrate = (((uint64_t)targetClockCount) * (uint64_t)refClockRate) / REFERENCE_CLOCK_COUNT;
    }

#if defined(SYSCON_CLOCK_CTRL_FRO1MHZ_FREQM_ENA_MASK) && defined(SYSCON_CLOCK_CTRL_XTAL32MHZ_FREQM_ENA_MASK)
    /* Assume measurement complete - gate high freq clock FRO1M and XTAL32M to FMEAS */
    SYSCON->CLOCK_CTRL &= ~(SYSCON_CLOCK_CTRL_FRO1MHZ_FREQM_ENA_MASK | SYSCON_CLOCK_CTRL_XTAL32MHZ_FREQM_ENA_MASK);
#endif

    return (uint32_t)clkrate;
}

#if defined(FSL_FEATURE_FMEAS_GET_COUNT_SCALE) && (FSL_FEATURE_FMEAS_GET_COUNT_SCALE)
void FMEAS_GetCountWithScale(FMEAS_SYSCON_Type *base,
                             uint8_t scale,
                             uint32_t *refClockCount,
                             uint32_t *targetClockCount)
{
    *targetClockCount = TARGET_CLOCK_COUNT(base);
    *refClockCount    = ((1 << scale) - 1);

    /* Assume measurement complete - gate high freq clock FRO1M and XTAL32M to FMEAS */
    SYSCON->CLOCK_CTRL &= ~(SYSCON_CLOCK_CTRL_FRO1MHZ_FREQM_ENA_MASK | SYSCON_CLOCK_CTRL_XTAL32MHZ_FREQM_ENA_MASK);
}
#endif /*FSL_FEATURE_FMEAS_GET_COUNT_SCALE*/
