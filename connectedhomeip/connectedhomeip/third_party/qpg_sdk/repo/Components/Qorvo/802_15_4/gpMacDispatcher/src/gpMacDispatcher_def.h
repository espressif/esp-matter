/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
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
#ifndef _GP_MACDISPATCHERDEF_H_
#define _GP_MACDISPATCHERDEF_H_

#include "global.h"
#include "gpMacCore.h"
#include "gpMacDispatcher.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct gpMacDispatcher_StackMapping_s {
    gpMacDispatcher_StringIdentifier_t  stringId;
    gpMacDispatcher_StackId_t           numericId;
    gpMacDispatcher_Callbacks_t         callbacks;
} gpMacDispatcher_StackMapping_t;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Internal non-static declarations
 *****************************************************************************/
extern gpMacDispatcher_StackMapping_t gpMacDispatcher_StackMapping[GP_DIVERSITY_NR_OF_STACKS];

#if GP_DIVERSITY_NR_OF_STACKS > 1
gpMacDispatcher_Callbacks_t* MacDispatcher_GetCallbacks(UInt8 stackId);
#else
#define MacDispatcher_GetCallbacks(stackId) (&gpMacDispatcher_StackMapping[0].callbacks)
#endif //GP_DIVERSITY_NR_OF_STACKS > 1

void MacDispatcher_InitStackMapping(void);
void MacDispatcher_UnRegisterCallbacks(UInt8 stackId);

#define MacDispatcher_InitAutoTxAntennaToggling(stackId) ((void)0)
#define MacDispatcher_HandleAutoTxAntennaToggling(status, stackId) ((void)0)

#endif //_GP_MACDISPATCHERDEF_H_

