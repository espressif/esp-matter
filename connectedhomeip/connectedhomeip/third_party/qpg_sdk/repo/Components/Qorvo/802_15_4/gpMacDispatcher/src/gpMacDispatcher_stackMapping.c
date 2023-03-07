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
#define GP_COMPONENT_ID GP_COMPONENT_ID_MACDISPATCHER

//Show all calls for debugging
//#define GP_LOCAL_LOG

#include "gpMacCore.h"
#include "gpMacDispatcher.h"
#include "gpMacDispatcher_def.h"
#include "gpLog.h"
#include "gpAssert.h"

#ifndef GP_DIVERSITY_NR_OF_STACKS
#warning You did not provide a number of stacks. Taking 1 as default

#define GP_DIVERSITY_NR_OF_STACKS    1
#endif //GP_DIVERSITY_NR_OF_STACKS
/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
gpMacDispatcher_StackMapping_t gpMacDispatcher_StackMapping[GP_DIVERSITY_NR_OF_STACKS];

void MacDispatcher_InitStackMapping(void)
{
    UIntLoop stackMapIndex;
    for(stackMapIndex = 0; stackMapIndex < GP_DIVERSITY_NR_OF_STACKS; stackMapIndex++)
    {
        // code shared with unregisterNetworkStack
        gpMacDispatcher_StackMapping[stackMapIndex].numericId = GP_MAC_DISPATCHER_INVALID_STACK_ID;
        MEMSET(gpMacDispatcher_StackMapping[stackMapIndex].stringId.str, -1, sizeof(gpMacDispatcher_StringIdentifier_t));
    }
}

void gpMacDispatcher_RegisterCallbacks(gpMacDispatcher_StackId_t stackId, gpMacDispatcher_Callbacks_t* pCallbacks)
{
    UIntLoop stackMapIndex;

    GP_ASSERT_DEV_EXT(pCallbacks);
    for(stackMapIndex = 0; stackMapIndex < GP_DIVERSITY_NR_OF_STACKS; stackMapIndex++)
    {
        // find entry previously used by the same stackId.
        if(gpMacDispatcher_StackMapping[stackMapIndex].numericId == stackId)
        {
            MEMCPY(&gpMacDispatcher_StackMapping[stackMapIndex].callbacks,pCallbacks,sizeof(gpMacDispatcher_Callbacks_t));
            return;
        }
    }
    for(stackMapIndex = 0; stackMapIndex < GP_DIVERSITY_NR_OF_STACKS; stackMapIndex++)
    {
        // find empty entry.
        if(gpMacDispatcher_StackMapping[stackMapIndex].numericId == GP_MAC_DISPATCHER_INVALID_STACK_ID)
        {
            gpMacDispatcher_StackMapping[stackMapIndex].numericId = stackId;
            MEMCPY(&gpMacDispatcher_StackMapping[stackMapIndex].callbacks,pCallbacks,sizeof(gpMacDispatcher_Callbacks_t));
            return;
        }
    }
    GP_ASSERT_DEV_INT(false);
    return;
}

void gpMacDispatcher_GetCallbacks(gpMacDispatcher_StackId_t stackId, gpMacDispatcher_Callbacks_t* pCallbacks)
{
    gpMacDispatcher_Callbacks_t* pLookupCallbacks;

    if(pCallbacks == NULL)
    {
        GP_ASSERT_DEV_EXT(false);
        return;
    }

    pLookupCallbacks = MacDispatcher_GetCallbacks(stackId);
    if(pLookupCallbacks == NULL)
    {
        GP_ASSERT_DEV_INT(false);
        return;
    }

    MEMCPY(pCallbacks, pLookupCallbacks, sizeof(gpMacDispatcher_Callbacks_t));
}

void MacDispatcher_UnRegisterCallbacks(UInt8 stackId)
{
    UIntLoop i;

    for(i = 0; i < GP_DIVERSITY_NR_OF_STACKS; i++)
    {
        if(gpMacDispatcher_StackMapping[i].numericId == stackId)
        {
            gpMacDispatcher_StackMapping[i].numericId = GP_MAC_DISPATCHER_INVALID_STACK_ID;
            MEMSET(gpMacDispatcher_StackMapping[i].stringId.str, -1, sizeof(gpMacDispatcher_StringIdentifier_t));
            return;
        }
    }
    GP_ASSERT_DEV_INT(false);
}

#if GP_DIVERSITY_NR_OF_STACKS > 1
gpMacDispatcher_Callbacks_t* MacDispatcher_GetCallbacks(UInt8 stackId)
{
    UIntLoop stackMapIndex;
    for(stackMapIndex = 0; stackMapIndex < GP_DIVERSITY_NR_OF_STACKS; stackMapIndex++)
    {
        // find entry previously used by the same stackId.
        if(gpMacDispatcher_StackMapping[stackMapIndex].numericId == stackId)
        {
            return &(gpMacDispatcher_StackMapping[stackMapIndex].callbacks);
        }
    }
    // no entry found.
    return NULL;
}
#endif //GP_DIVERSITY_NR_OF_STACKS > 1
