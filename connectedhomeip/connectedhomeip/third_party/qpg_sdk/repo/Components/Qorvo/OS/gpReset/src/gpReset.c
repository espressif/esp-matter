/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 * Copyright (c) 2017-2022, Qorvo Inc
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_RESET

#include "gpReset.h"
#include "hal.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define RESET_REASON_MAX        5

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/
#define RESET_HAL_TO_GPRESET_REASON(mapping, halReason)              ((halReason < RESET_REASON_MAX)? mapping[halReason] : gpReset_ResetReason_UnSpecified)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static gpReset_ResetReason_t gpReset_Reason;

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpReset_Init(void)
{
    // local mapping from hal reset reasons to gpReset reset reasons
    const gpReset_ResetReason_t resetReasonMapping[RESET_REASON_MAX] = {
        gpReset_ResetReason_UnSpecified, gpReset_ResetReason_HW_Por, gpReset_ResetReason_SW_Por,
        gpReset_ResetReason_HW_BrownOutDetected, gpReset_ResetReason_HW_Watchdog};


    // get hal reset reason and convert to gpReset_ResetReason
    gpReset_Reason = RESET_HAL_TO_GPRESET_REASON(resetReasonMapping, HAL_GET_RESET_REASON());
}

void gpReset_ResetBySwPor(void)
{
    HAL_RESET_UC();
}

void gpReset_ResetByWatchdog(void)
{
    HAL_WDT_FORCE_TRIGGER();
}

gpReset_ResetReason_t gpReset_GetResetReason(void)
{
    return gpReset_Reason;
}
