/*
 * Copyright (c) 2013, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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

//#define GP_LOCAL_LOG - set LOCAL_LOG in included .c files
#define GP_COMPONENT_ID GP_COMPONENT_ID_PAD

#include "gpPad.h"

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
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Source Includes
 *****************************************************************************/

#include "gpPad_ram.c"

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpPad_Init(void)
{
    Pad_Init();
}

gpPad_Handle_t gpPad_GetPad(gpPad_Attributes_t* pInitAttributes)
{
    return Pad_GetPad(pInitAttributes);
}

void gpPad_FreePad(gpPad_Handle_t padHandle)
{
    Pad_FreePad(padHandle);
}

gpPad_Result_t gpPad_CheckPadValid(gpPad_Handle_t padHandle)
{
    return Pad_CheckPadValid(padHandle);
}

void gpPad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    Pad_SetAttributes(padHandle, pAttributes);
}
void gpPad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    Pad_SetTxChannels(padHandle, channels);
}
void gpPad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower)
{
    Pad_SetTxPower(padHandle, txPower);
}
void gpPad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna)
{
    Pad_SetTxAntenna(padHandle, antenna);
}
void gpPad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE)
{
    Pad_SetTxMinBE(padHandle, minBE);
}
void gpPad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE)
{
    Pad_SetTxMaxBE(padHandle, maxBE);
}
void gpPad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs)
{
    Pad_SetTxMaxCsmaBackoffs(padHandle, maxCsmaBackoffs);
}
void gpPad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries)
{
    Pad_SetTxMaxFrameRetries(padHandle, maxFrameRetries);
}
void gpPad_SetTxCsmaMode(gpPad_Handle_t padHandle, UInt8 csma)
{
    Pad_SetTxCsmaMode(padHandle, csma);
}
void gpPad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 ccaMode)
{
    Pad_SetCcaMode(padHandle, ccaMode);
}

void gpPad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    Pad_GetAttributes(padHandle, pAttributes);
}
void  gpPad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    Pad_GetTxChannels(padHandle, channels);
}
Int8  gpPad_GetTxPower(gpPad_Handle_t padHandle)
{
    return Pad_GetTxPower(padHandle);
}
UInt8 gpPad_GetTxAntenna(gpPad_Handle_t padHandle)
{
    return Pad_GetTxAntenna(padHandle);
}
UInt8 gpPad_GetTxMinBE(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMinBE(padHandle);
}
UInt8 gpPad_GetTxMaxBE(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMaxBE(padHandle);
}
UInt8 gpPad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMaxCsmaBackoffs(padHandle);
}
UInt8 gpPad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMaxFrameRetries(padHandle);
}
UInt8 gpPad_GetTxCsmaMode(gpPad_Handle_t padHandle)
{
    return Pad_GetTxCsmaMode(padHandle);
}
UInt8 gpPad_GetCcaMode(gpPad_Handle_t padHandle)
{
    return Pad_GetCcaMode(padHandle);
}

void gpPad_SetRetransmitOnCcaFail(gpPad_Handle_t padHandle, Bool enabled)
{
    Pad_SetRetransmitOnCcaFail(padHandle, enabled);
}
Bool gpPad_GetRetransmitOnCcaFail(gpPad_Handle_t padHandle)
{
    return Pad_GetRetransmitOnCcaFail(padHandle);
}
void gpPad_SetRetransmitRandomBackoff(gpPad_Handle_t padHandle, Bool enabled)
{
    Pad_SetRetransmitRandomBackoff(padHandle, enabled);
}
Bool gpPad_GetRetransmitRandomBackoff(gpPad_Handle_t padHandle)
{
    return Pad_GetRetransmitRandomBackoff(padHandle);
}

void gpPad_SetMinBeRetransmit(gpPad_Handle_t padHandle, UInt8 minBERetransmit)
{
    Pad_SetMinBeRetransmit(padHandle, minBERetransmit);
}
UInt8 gpPad_GetMinBeRetransmit(gpPad_Handle_t padHandle)
{
    return Pad_GetMinBeRetransmit(padHandle);
}
void gpPad_SetMaxBeRetransmit(gpPad_Handle_t padHandle, UInt8 maxBERetransmit)
{
    Pad_SetMaxBeRetransmit(padHandle, maxBERetransmit);
}
UInt8 gpPad_GetMaxBeRetransmit(gpPad_Handle_t padHandle)
{
    return Pad_GetMaxBeRetransmit(padHandle);
}
