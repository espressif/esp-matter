/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpRxArbiter.c
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

#define GP_COMPONENT_ID GP_COMPONENT_ID_RXARBITER

// #define GP_LOCAL_LOG

#include "hal.h"
#include "gpLog.h"
#include "gpRxArbiter.h"
#include "gpHal.h"
#include "gpHal_ES.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct{
    UInt8 rxOnCounter;
}gpRxArbiter_StackDesc_t;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

gpRxArbiter_StackDesc_t gpRxArbiter_StackDesc;
#define GP_RXARBITER_DIVERSITY_NROFCHANNELS_PER_STACKID 1

UInt8 gpRxArbiter_CurrentRxChannel[GP_RXARBITER_DIVERSITY_NROFCHANNELS_PER_STACKID];

gpRxArbiter_RadioState_t gpRxArbiter_CurrentRadioState;

#define RX_ARBITER_DUTY_CYCLE_ENABLED(stackId)  false


#define RX_ARBITER_RECURRENCE_INFINITE 0xFFFF

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

#if defined(GP_DIVERSITY_JUMPTABLES)
// These functions need to have an implementation in this file as well, as these are defined in the ROM tables
// if not defined, it will result in an
gpRxArbiter_Result_t gpRxArbiter_SetStackPriority( UInt8 priority , gpRxArbiter_StackId_t stackId )
{
    return gpRxArbiter_ResultSuccess;
}

// register for notifications from gpRxArbiter
gpRxArbiter_Result_t gpRxArbiter_RegisterSetFaModeCallback(gpRxArbiter_StackId_t stackId, gpRxArbiter_cbSetFaMode_t cb)
{
    return gpRxArbiter_ResultSuccess;
}
gpRxArbiter_Result_t gpRxArbiter_RegisterChannelUpdateCallback(gpRxArbiter_StackId_t stackId, gpRxArbiter_cbChannelUpdate_t cb)
{
    return gpRxArbiter_ResultSuccess;
}
gpRxArbiter_Result_t gpRxArbiter_DeRegisterChannelUpdateCallback(gpRxArbiter_StackId_t stackId)
{
    return gpRxArbiter_ResultSuccess;
}
#endif
/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


static void RxArbiter_SetRxOn(Bool rxOn)
{
    UInt8 index;
    gpRxArbiter_CurrentRadioState = rxOn ? gpRxArbiter_RadioStateOn : gpRxArbiter_RadioStateOff;

    /* assumptions */
    COMPILE_TIME_ASSERT(gpHal_SourceIdentifier_0 == 0);
    COMPILE_TIME_ASSERT(gpHal_SourceIdentifier_1 == 1);
    COMPILE_TIME_ASSERT(gpHal_SourceIdentifier_2 == 2);

    for(index = 0; index < GP_RXARBITER_DIVERSITY_NROFCHANNELS_PER_STACKID; index++)
    {
        if(gpRxArbiter_CurrentRxChannel[index] != GP_RX_ARBITER_INVALID_CHANNEL)
        {
            GP_LOG_PRINTF("Radio On:%u rxCh%d:%u",0, rxOn, index, gpRxArbiter_CurrentRxChannel[index]);
            gpHal_SetRxOnWhenIdle(index, rxOn, gpRxArbiter_CurrentRxChannel[index]);
        }
    }
}


static void RxArbiter_UpdateRadio(gpRxArbiter_StackId_t requestedStackId)
{
    // we have found the highest priority stack, now set channel and RxOn,
    // set rx on/rxof/duty cycle

#ifdef GP_LOCAL_LOG
    // Logging variable
    gpRxArbiter_RadioState_t currentRadioState = gpRxArbiter_CurrentRadioState;
#endif // GP_LOCAL_LOG

    switch(gpRxArbiter_CurrentRadioState)
    {
        case gpRxArbiter_RadioStateOff:
        {
            if(gpRxArbiter_StackDesc.rxOnCounter)
            {
                RxArbiter_SetRxOn(true);
            }
            break;
        }
        case gpRxArbiter_RadioStateOn:
        {
            if(gpRxArbiter_StackDesc.rxOnCounter == 0)
            {
                RxArbiter_SetRxOn(false);
            }
            else
            {
                //Channel update through RxOn
                RxArbiter_SetRxOn(false);
                RxArbiter_SetRxOn(true);
            }
            break;
        }
        default:
        {
            break;
        }
    }
    GP_LOG_PRINTF("UpdateRadio RadioState %d -> %d", 0, currentRadioState, gpRxArbiter_CurrentRadioState);
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpRxArbiter_Init(void)
{
    MEMSET((void*)&(gpRxArbiter_CurrentRxChannel[0]), 0xFF , GP_RXARBITER_DIVERSITY_NROFCHANNELS_PER_STACKID * sizeof(gpRxArbiter_CurrentRxChannel));
    gpRxArbiter_CurrentRadioState = gpRxArbiter_RadioStateOff;

}

void gpRxArbiter_DeInit(void)
{

    gpRxArbiter_ResetStack(gpHal_SourceIdentifier_0);
    MEMSET((void*)&(gpRxArbiter_CurrentRxChannel[0]), 0xFF , GP_RXARBITER_DIVERSITY_NROFCHANNELS_PER_STACKID * sizeof(gpRxArbiter_CurrentRxChannel));

}

gpRxArbiter_Result_t gpRxArbiter_ResetStack(gpRxArbiter_StackId_t stackId)
{
    UInt8 i;

    GP_LOG_PRINTF("Reset Stack StId %i",0,stackId);

    if(gpRxArbiter_CurrentRadioState == gpRxArbiter_RadioStateOn)
    {
        gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_0, false, gpRxArbiter_CurrentRxChannel[0]);
    }
    gpRxArbiter_CurrentRadioState = gpRxArbiter_RadioStateOff;
    gpRxArbiter_StackDesc.rxOnCounter = 0;

    for(i=0; i<GP_RXARBITER_DIVERSITY_NROFCHANNELS_PER_STACKID; i++)
    {
        if(gpRxArbiter_CurrentRxChannel[i] == GP_RX_ARBITER_INVALID_CHANNEL)
        {
            gpRxArbiter_CurrentRxChannel[i] = gpHal_GetRxChannel(i);
        }
    }
    return gpRxArbiter_ResultSuccess;
}

gpRxArbiter_Result_t gpRxArbiter_SetStackChannel(UInt8 channel , gpRxArbiter_StackId_t stackId)
{
    GP_LOG_PRINTF("Set Ch %i, StId %i",0,channel,stackId);

    if(!GP_RXARBITER_CHECK_CHANNEL_VALID(channel))
    {
        return gpRxArbiter_ResultInvalidArgument;
    }

    if(channel != gpRxArbiter_CurrentRxChannel[0])
    {
        gpRxArbiter_CurrentRxChannel[0] = channel;
        RxArbiter_UpdateRadio(stackId);
    }


    return gpRxArbiter_ResultSuccess;
}


UInt8 gpRxArbiter_GetStackSlotChannel(UInt8 slotnumber , gpRxArbiter_StackId_t stackId)
{
    if(slotnumber > gpHal_SourceIdentifier_2)
        return GP_RX_ARBITER_INVALID_CHANNEL;

    return gpRxArbiter_CurrentRxChannel[slotnumber];
}

Bool gpRxArbiter_IsAnActiveChannel(gpRxArbiter_StackId_t stackId, UInt8 channel)
{
    UInt8 i;

    for(i=0; i<GP_RXARBITER_DIVERSITY_NROFCHANNELS_PER_STACKID; i++)
    {
        if(channel == gpRxArbiter_GetStackSlotChannel(i, stackId))
        {
            return true;
        }
    }

    return false;
}

UInt8 gpRxArbiter_GetStackChannel(gpRxArbiter_StackId_t stackId)
{
    return gpRxArbiter_CurrentRxChannel[0];
}

gpRxArbiter_RadioState_t gpRxArbiter_GetCurrentRxOnState(void)
{
    return gpRxArbiter_CurrentRadioState;
}

UInt8 gpRxArbiter_GetCurrentRxChannel(void)
{
    return gpRxArbiter_CurrentRxChannel[0];
}

gpRxArbiter_Result_t gpRxArbiter_SetStackRxOn(Bool enable , gpRxArbiter_StackId_t stackId)
{
    GP_LOG_PRINTF("SetStackRxOn en:%u RxOn counter:%u: stackId: %u",0,enable, gpRxArbiter_StackDesc.rxOnCounter, stackId);

    if(!RX_ARBITER_DUTY_CYCLE_ENABLED(stackId))
    {
        if(enable)
        {
            gpRxArbiter_StackDesc.rxOnCounter++;
            if (gpRxArbiter_StackDesc.rxOnCounter == 1) // going from 0 -> 1
            {
                RxArbiter_UpdateRadio(stackId);
            }
        }
        else
        {
            if(gpRxArbiter_StackDesc.rxOnCounter)
            {
                gpRxArbiter_StackDesc.rxOnCounter--;
                if (gpRxArbiter_StackDesc.rxOnCounter == 0)
                {
                    RxArbiter_UpdateRadio(stackId);
                }
            }
            else
            {
                GP_LOG_PRINTF("RxOn invalid param negative 1",0);
                return gpRxArbiter_ResultInvalidArgument;
            }
        }
        return gpRxArbiter_ResultSuccess;
    }
    else
    {
        GP_LOG_PRINTF("RxOn invalid param Duty cycle enabled 2",0);
        return gpRxArbiter_ResultInvalidArgument;
    }
}

Bool gpRxArbiter_GetStackRxOn(gpRxArbiter_StackId_t stackId)
{
    return gpRxArbiter_StackDesc.rxOnCounter;
}


Bool gpRxArbiter_GetDutyCycleEnabled(gpRxArbiter_StackId_t stackId)
{
    return RX_ARBITER_DUTY_CYCLE_ENABLED(stackId);
}

