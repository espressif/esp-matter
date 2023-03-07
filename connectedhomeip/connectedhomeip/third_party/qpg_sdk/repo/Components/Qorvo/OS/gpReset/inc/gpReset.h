/*
 * Copyright (c) 2011-2012, 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Control over system reset
 *   Declarations of the public functions and enumerations of gpReset.
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPRESET_H_
#define _GPRESET_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpReset_ResetReason_t */
//@{
#define gpReset_ResetReason_UnSpecified                        0x00
#define gpReset_ResetReason_HW_BrownOutDetected                0x01
#define gpReset_ResetReason_HW_Watchdog                        0x02
#define gpReset_ResetReason_HW_Por                             0x03
#define gpReset_ResetReason_SW_Por                             0x04
typedef UInt8                             gpReset_ResetReason_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/** @brief Default reset method: maps on sw por */
#define gpReset_ResetSystem()              gpReset_ResetBySwPor()

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests

void gpReset_Init(void);

void gpReset_ResetBySwPor(void);

gpReset_ResetReason_t gpReset_GetResetReason(void);

void gpReset_ResetByWatchdog(void);

//Indications

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPRESET_H_

