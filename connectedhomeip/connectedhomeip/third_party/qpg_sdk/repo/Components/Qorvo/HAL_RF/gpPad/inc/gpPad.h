/*
 * Copyright (c) 2013, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
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

#ifndef _GPPAD_H_
#define _GPPAD_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_PAD_NR_OF_HANDLES
#define GP_PAD_NR_OF_HANDLES    3
#endif //GP_PD_NR_OF_HANDLES

#define GP_PAD_INVALID_HANDLE   0xFF

#define GP_PAD_MAX_CHANNELS     3

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 gpPad_Handle_t;

#define gpPad_ResultValidHandle        0x0
#define gpPad_ResultInvalidHandle      0x1
#define gpPad_ResultNotInUse           0x2
typedef UInt8 gpPad_Result_t;

typedef struct gpPad_Attributes_s {
    UInt8 channels[GP_PAD_MAX_CHANNELS];
    UInt8 antenna;
    Int8  txPower;
    UInt8 minBE;
    UInt8 maxBE;
    UInt8 maxCsmaBackoffs;
    UInt8 maxFrameRetries;
    UInt8 csma;
    UInt8 cca;
    Bool  retransmitOnCcaFail;
    Bool  retransmitRandomBackoff;
    UInt8 minBERetransmit;
    UInt8 maxBERetransmit;
} gpPad_Attributes_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpPad_CodeJumpTableFlash_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

void    gpPad_Init(void);

//Descriptor handling
gpPad_Handle_t gpPad_GetPad(gpPad_Attributes_t* pInitAttributes);
void           gpPad_FreePad(gpPad_Handle_t padHandle);
gpPad_Result_t gpPad_CheckPadValid(gpPad_Handle_t padHandle);

//TxRequest
void gpPad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);
void gpPad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);

void gpPad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
void gpPad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower);
void gpPad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna);
void gpPad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE);
void gpPad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE);
void gpPad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs);
void gpPad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries);
void gpPad_SetTxCsmaMode(gpPad_Handle_t padHandle, UInt8 csma);
void gpPad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 cca);

void  gpPad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
Int8  gpPad_GetTxPower(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxAntenna(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMinBE(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMaxBE(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxCsmaMode(gpPad_Handle_t padHandle);
UInt8 gpPad_GetCcaMode(gpPad_Handle_t padHandle);

void gpPad_SetRetransmitOnCcaFail(gpPad_Handle_t padHandle, Bool enabled);
Bool gpPad_GetRetransmitOnCcaFail(gpPad_Handle_t padHandle);
void gpPad_SetRetransmitRandomBackoff(gpPad_Handle_t padHandle, Bool enabled);
Bool gpPad_GetRetransmitRandomBackoff(gpPad_Handle_t padHandle);

void gpPad_SetMinBeRetransmit(gpPad_Handle_t padHandle, UInt8 minBERetransmit);
UInt8 gpPad_GetMinBeRetransmit(gpPad_Handle_t padHandle);
void gpPad_SetMaxBeRetransmit(gpPad_Handle_t padHandle, UInt8 maxBERetransmit);
UInt8 gpPad_GetMaxBeRetransmit(gpPad_Handle_t padHandle);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _GPPD_H_
