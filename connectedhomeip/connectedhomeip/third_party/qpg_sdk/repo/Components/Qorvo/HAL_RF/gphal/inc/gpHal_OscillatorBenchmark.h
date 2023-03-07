/*
 * Copyright (c) 2017, 2019, Qorvo Inc
 *
 *   Interface for the crystal calibration algorithm.
 *   Declarations of the public functions and enumerations of gpHal_OscillatorBenchmark.
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */


#ifndef _GPHAL_OSCILLATORBENCHMARK_H_
#define _GPHAL_OSCILLATORBENCHMARK_H_

/// @file "gpHal_OscillatorBenchmark.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "compiler.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/


typedef enum _gpHal_OscillatorBenchmark_Status {
    gpHal_OscillatorBenchmark_Result_NeedMoreSamples = 0,
    gpHal_OscillatorBenchmark_Result_Stable = 1,
    gpHal_OscillatorBenchmark_Result_Unstable = 2,
    gpHal_OscillatorBenchmark_Result_Broken = 3,
} gpHal_OscillatorBenchmark_Status_t;

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void gpHal_OscillatorBenchmark_RunAvg8_Init(void);
UInt32 gpHal_OscillatorBenchmark_RunAvg8_Add(UInt32 benchmark);

void gpHal_OscillatorBenchmark_3Phase_Init(UInt8 stable_length);
gpHal_OscillatorBenchmark_Status_t gpHal_OscillatorBenchmark_3Phase_Add(UInt32 benchmark);
UInt32 gpHal_OscillatorBenchmark_MSE_GetStableValue(void);
UInt32 gpHal_OscillatorBenchmark_3Phase_GetAvg(void);
UInt32 gpHal_OscillatorBenchmark_3Phase_GetMSE(void);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPHAL_OSCILLATORBENCHMARK_H_

