/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 *   gpRxArbiter.h
 *   This file contains the definitions of the public functions and enumerations of the gpRxArbiter.
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


#ifndef _GP_RX_ARBITER_H_
#define _GP_RX_ARBITER_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef GP_DIVERSITY_NR_OF_STACKS
#define GP_RX_ARBITER_NUMBER_OF_STACKS  GP_DIVERSITY_NR_OF_STACKS
#else//GP_DIVERSITY_NR_OF_STACKS
#define GP_RX_ARBITER_NUMBER_OF_STACKS  1
#endif //GP_DIVERSITY_NR_OF_STACKS

#define GP_RX_ARBITER_INVALID_CHANNEL   0xFF

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define GP_RXARBITER_CHECK_CHANNEL_VALID(channel) (((channel) >= 11) && ((channel) <= 26))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @name gpRxArbiter_FaMode_t*/
//@{
/** @ingroup GEN_FA
 *  @brief The frequency agility is disabled.
*/
#define gpRxArbiter_FaModeDisabled       0x00
/** @ingroup GEN_FA
 *  @brief The frequency agility is enabled.
*/
#define gpRxArbiter_FaModeEnabled        0x01
/** @ingroup GEN_FA
 *  @brief The frequency agility is running in power save mode.
*/
typedef UInt8 gpRxArbiter_FaMode_t ;
//@}

typedef UInt8 gpRxArbiter_StackId_t;
#define gpRxArbiter_StackIdRf4ce        0x0
#define gpRxArbiter_StackIdZigBeePro    0x1
#define gpRxArbiter_StackIdZigBeeIp     0x2
#define gpRxArbiter_StackIdInvalid      0xFF

typedef UInt8 gpRxArbiter_Result_t;
#define gpRxArbiter_ResultSuccess               0x0
#define gpRxArbiter_ResultInvalidArgument       0x1

typedef UInt8 gpRxArbiter_RadioState_t;
#define gpRxArbiter_RadioStateOff           0
#define gpRxArbiter_RadioStateOn            1
#define gpRxArbiter_RadioStateDutyCycle     2
#define gpRxArbiter_RadioStateInvalid       0xff
#define gpRxArbiter_RadioStateActive(state) (state==gpRxArbiter_RadioStateOn || state==gpRxArbiter_RadioStateDutyCycle)

typedef void (* gpRxArbiter_cbSetFaMode_t)(gpRxArbiter_StackId_t stackId, gpRxArbiter_FaMode_t mode) ;
typedef void (* gpRxArbiter_cbChannelUpdate_t)(gpRxArbiter_StackId_t stackId, UInt8 channel) ;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpRxArbiter_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

void gpRxArbiter_Init( void );
void gpRxArbiter_DeInit( void );

//Added for enabling 3 Rx channels for multichannel listen over MAC
gpRxArbiter_Result_t gpRxArbiter_SetMultipleChannelTable(UInt8* pChannel , gpRxArbiter_StackId_t stackId);
UInt8 gpRxArbiter_GetStackSlotChannel(UInt8 slotnumber , gpRxArbiter_StackId_t stackId);

gpRxArbiter_Result_t gpRxArbiter_ResetStack( gpRxArbiter_StackId_t stackId );
gpRxArbiter_Result_t gpRxArbiter_SetStackChannel( UInt8 channel , gpRxArbiter_StackId_t stackId );
UInt8                gpRxArbiter_GetStackChannel( gpRxArbiter_StackId_t stackId );
UInt8                gpRxArbiter_GetCurrentRxChannel( void );

gpRxArbiter_Result_t gpRxArbiter_SetStackRxOn( Bool enable , gpRxArbiter_StackId_t stackId );
Bool                 gpRxArbiter_GetStackRxOn( gpRxArbiter_StackId_t stackId );
gpRxArbiter_RadioState_t gpRxArbiter_GetCurrentRxOnState( void );

Bool                 gpRxArbiter_GetDutyCycleEnabled( gpRxArbiter_StackId_t stackId );



#if (GP_RX_ARBITER_NUMBER_OF_STACKS == 1) && (!(defined(GP_DIVERSITY_JUMPTABLES)))
#define gpRxArbiter_SetStackPriority( priority , stackId )      (gpRxArbiter_ResultSuccess)

#define gpRxArbiter_RegisterSetFaModeCallback( stackId, cb)     (gpRxArbiter_ResultSuccess)
#define gpRxArbiter_RegisterChannelUpdateCallback( stackId, cb) (gpRxArbiter_ResultSuccess)
#define gpRxArbiter_DeRegisterChannelUpdateCallback( stackId ) (gpRxArbiter_ResultSuccess)

#else

gpRxArbiter_Result_t gpRxArbiter_SetStackPriority( UInt8 priority , gpRxArbiter_StackId_t stackId );

// register for notifications from gpRxArbiter
gpRxArbiter_Result_t gpRxArbiter_RegisterSetFaModeCallback(gpRxArbiter_StackId_t stackId, gpRxArbiter_cbSetFaMode_t cb) ;
gpRxArbiter_Result_t gpRxArbiter_RegisterChannelUpdateCallback(gpRxArbiter_StackId_t stackId, gpRxArbiter_cbChannelUpdate_t cb) ;
gpRxArbiter_Result_t gpRxArbiter_DeRegisterChannelUpdateCallback(gpRxArbiter_StackId_t stackId);

#endif // #if GP_RX_ARBITER_NUMBER_OF_STACKS == 1

Bool gpRxArbiter_IsAnActiveChannel(gpRxArbiter_StackId_t stackId, UInt8 channel);



/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GP_RX_ARBITER_H_


