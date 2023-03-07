/*
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpZproComps.h
 *
 * Initialization function of the ZProComps components.
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
#ifndef _GPBLECOMPS_H
#define _GPBLECOMPS_H

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef void (*Ble_ResetFunc_t)(Bool firstReset);

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

void gpBleComps_StackInit(void);
void gpBle_RegisterResetFunction(Ble_ResetFunc_t resetFunc);
void gpBle_ResetServices(Bool firstReset);
void gpBleComps_ResetLinkLayer(void);

#ifdef __cplusplus
}
#endif

#endif // _GPBLECOMPS_H

