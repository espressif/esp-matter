/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_FMEAS_H_
#define _FSL_FMEAS_H_

#include "fsl_common.h"

/*!
 * @addtogroup fmeas
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief Defines LPC Frequency Measure driver version 2.1.0. */
#define FSL_FMEAS_DRIVER_VERSION (MAKE_VERSION(2, 1, 0))
/*@}*/

#if defined(FSL_FEATURE_FMEAS_INDEX_20) && (FSL_FEATURE_FMEAS_INDEX_20)
/*! The calibration duration is 2^FMEAS_INDEX times the reference clock period */
#define FMEAS_INDEX 20
#endif /*FSL_FEATURE_FMEAS_INDEX_20*/

#if defined(FSL_FEATURE_FMEAS_ASYNC_SYSCON_FREQMECTRL) && (FSL_FEATURE_FMEAS_ASYNC_SYSCON_FREQMECTRL)
#define FMEAS_SYSCON_FREQMECTRL_CAPVAL_MASK ASYNC_SYSCON_FREQMECTRL_CAPVAL_MASK
#define FMEAS_SYSCON_FREQMECTRL_CAPVAL_SHIFT ASYNC_SYSCON_FREQMECTRL_CAPVAL_SHIFT
#define FMEAS_SYSCON_FREQMECTRL_CAPVAL ASYNC_SYSCON_FREQMECTRL_CAPVAL
#define FMEAS_SYSCON_FREQMECTRL_PROG_MASK ASYNC_SYSCON_FREQMECTRL_PROG_MASK
#define FMEAS_SYSCON_FREQMECTRL_PROG_SHIFT ASYNC_SYSCON_FREQMECTRL_PROG_SHIFT
#define FMEAS_SYSCON_FREQMECTRL_PROG ASYNC_SYSCON_FREQMECTRL_PROG
#else
#define FMEAS_SYSCON_FREQMECTRL_CAPVAL_MASK SYSCON_FREQMECTRL_CAPVAL_MASK
#define FMEAS_SYSCON_FREQMECTRL_CAPVAL_SHIFT SYSCON_FREQMECTRL_CAPVAL_SHIFT
#define FMEAS_SYSCON_FREQMECTRL_CAPVAL SYSCON_FREQMECTRL_CAPVAL
#define FMEAS_SYSCON_FREQMECTRL_PROG_MASK SYSCON_FREQMECTRL_PROG_MASK
#define FMEAS_SYSCON_FREQMECTRL_PROG_SHIFT SYSCON_FREQMECTRL_PROG_SHIFT
#define FMEAS_SYSCON_FREQMECTRL_PROG SYSCON_FREQMECTRL_PROG
#endif /*FSL_FEATURE_FMEAS_ASYNC_SYSCON_FREQMECTRL*/

/*******************************************************************************
 * Types
 *******************************************************************************/

#if defined(FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON) && (FSL_FEATURE_FMEAS_USE_ASYNC_SYSCON)
typedef ASYNC_SYSCON_Type FMEAS_SYSCON_Type;
#elif defined(FSL_FEATURE_SOC_FREQME_COUNT) && (FSL_FEATURE_SOC_FREQME_COUNT)
typedef FREQME_Type FMEAS_SYSCON_Type;
#else
typedef SYSCON_Type FMEAS_SYSCON_Type;
#endif

/*******************************************************************************
 * Definitions
 *******************************************************************************/

/*******************************************************************************
 * API
 *******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @name FMEAS Functional Operation
 * @{
 */

/*!
 * @brief    Starts a frequency measurement cycle.
 *
 * @param    base : SYSCON peripheral base address.
 */
static inline void FMEAS_StartMeasure(FMEAS_SYSCON_Type *base)
{
#if defined(FSL_FEATURE_SOC_FREQME_COUNT) && (FSL_FEATURE_SOC_FREQME_COUNT)
    base->FREQMECTRL_W = 0;
    /*! Set the reference clock count cycle to 2^20 times*/
    base->FREQMECTRL_W = FREQME_FREQMECTRL_W_MEASURE_IN_PROGRESS_MASK | FREQME_FREQMECTRL_W_REF_SCALE(20);
#else
    base->FREQMECTRL = 0;
#if defined(SYSCON_CLOCK_CTRL_FRO1MHZ_FREQM_ENA_MASK)
    /* Un-gate FRO1M to FMEAS if used as reference or target clock */
    if ((((INPUTMUX->FREQMEAS_REF & INPUTMUX_FREQMEAS_REF_CLKIN_MASK) >> INPUTMUX_FREQMEAS_REF_CLKIN_SHIFT) ==
         kCLOCK_fmeasFRO1Mhz) ||
        (((INPUTMUX->FREQMEAS_TARGET & INPUTMUX_FREQMEAS_TARGET_CLKIN_MASK) >> INPUTMUX_FREQMEAS_TARGET_CLKIN_SHIFT) ==
         kCLOCK_fmeasFRO1Mhz))
    {
        SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_FRO1MHZ_FREQM_ENA_MASK;
    }
#endif /*SYSCON_CLOCK_CTRL_FRO1MHZ_FREQM_ENA_MASK*/

#if defined(SYSCON_CLOCK_CTRL_XTAL32MHZ_FREQM_ENA_MASK)
    /* Un-gate XTAL32M to FMEAS if used as reference or target clock */
    if ((((INPUTMUX->FREQMEAS_REF & INPUTMUX_FREQMEAS_REF_CLKIN_MASK) >> INPUTMUX_FREQMEAS_REF_CLKIN_SHIFT) ==
         kCLOCK_fmeasXtal32Mhz) ||
        (((INPUTMUX->FREQMEAS_TARGET & INPUTMUX_FREQMEAS_TARGET_CLKIN_MASK) >> INPUTMUX_FREQMEAS_TARGET_CLKIN_SHIFT) ==
         kCLOCK_fmeasXtal32Mhz))
    {
        SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_XTAL32MHZ_FREQM_ENA_MASK;
    }
#endif

#if defined(FMEAS_INDEX)
    base->FREQMECTRL = FMEAS_INDEX | FMEAS_SYSCON_FREQMECTRL_PROG_MASK;
#else
    base->FREQMECTRL = (1UL << 31);
#endif
#endif
}

/*!
 * @brief    Indicates when a frequency measurement cycle is complete.
 *
 * @param    base : SYSCON peripheral base address.
 * @return   true if a measurement cycle is active, otherwise false.
 */
static inline bool FMEAS_IsMeasureComplete(FMEAS_SYSCON_Type *base)
{
#if defined(FSL_FEATURE_SOC_FREQME_COUNT) && (FSL_FEATURE_SOC_FREQME_COUNT)
    return (bool)((base->FREQMECTRL_R & FREQME_FREQMECTRL_R_MEASURE_IN_PROGRESS_MASK) == 0);
#else
    return (bool)((base->FREQMECTRL & (1UL << 31)) == 0);
#endif
}

/*!
 * @brief    Returns the computed value for a frequency measurement cycle
 *
 * @param    base         : SYSCON peripheral base address.
 * @param    refClockRate : Reference clock rate used during the frequency measurement cycle.
 *
 * @return   Frequency in Hz.
 */
uint32_t FMEAS_GetFrequency(FMEAS_SYSCON_Type *base, uint32_t refClockRate);

#if defined(FSL_FEATURE_FMEAS_GET_COUNT_SCALE) && (FSL_FEATURE_FMEAS_GET_COUNT_SCALE)
/*!
 * @brief    Get the clock count during the measurement time
 *
 * @param    base         : SYSCON peripheral base address.
 * @param    scale : measurement time is 2^scale cycle of reference clock, value is from 2 to 31.
 * @param    refClockCount : Reference clock cycle during the measurement time.
 * @param    targetClockCount : Target clock cycle during the measurement time.
 *
 */
void FMEAS_GetCountWithScale(FMEAS_SYSCON_Type *base,
                             uint8_t scale,
                             uint32_t *refClockCount,
                             uint32_t *targetClockCount);
#endif

/*@}*/

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @}*/

#endif /* _FSL_FMEAS_H_ */
