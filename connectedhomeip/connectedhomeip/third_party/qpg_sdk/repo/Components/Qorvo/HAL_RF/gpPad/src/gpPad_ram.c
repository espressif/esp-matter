/*
 * Copyright (c) 2013, 2016, GreenPeak Technologies
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
//#define GP_LOCAL_LOG
#include "gpLog.h"
#include "gpAssert.h"

#include "gpHal_MAC.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define PAD_CHECK_HANDLE_VALID(padHandle)      (padHandle < GP_PAD_NR_OF_HANDLES)
#define PAD_CHECK_HANDLE_IN_USE(padHandle)     (gpPad_Handles[padHandle])
#define PAD_CHECK_HANDLE_ACCESSIBLE(padHandle) (PAD_CHECK_HANDLE_VALID(padHandle) && PAD_CHECK_HANDLE_IN_USE(padHandle))

#define PAD_HANDLE_CLAIM(padHandle)           do { gpPad_Handles[padHandle] = true; } while(false);
#define PAD_HANDLE_FREE(padHandle)            do { gpPad_Handles[padHandle] = false; } while(false);

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
typedef gpPad_Attributes_t gpPad_Descriptor_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static gpPad_Descriptor_t gpPad_Descriptors[GP_PAD_NR_OF_HANDLES] GP_EXTRAM_SECTION_ATTR;

//Array for handle administration
static Bool gpPad_Handles[GP_PAD_NR_OF_HANDLES] = {false};


/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void Pad_Init(void);

//Descriptor handling
static gpPad_Handle_t Pad_GetPad(gpPad_Attributes_t* pInitAttributes);
static void           Pad_FreePad(gpPad_Handle_t padHandle);
static gpPad_Result_t Pad_CheckPadValid(gpPad_Handle_t padHandle);

//Set handling
static void Pad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);

static void Pad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
static void Pad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower);
static void Pad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna);
static void Pad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE);
static void Pad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE);
static void Pad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs);
static void Pad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries);
static void Pad_SetTxCsmaMode(gpPad_Handle_t padHandle, gpHal_CollisionAvoidanceMode_t csma);
static void Pad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 cca);

//Get handling
static void Pad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);

static void Pad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
static Int8 Pad_GetTxPower(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxAntenna(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMinBE(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMaxBE(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle);
static UInt8 Pad_GetTxCsmaMode(gpPad_Handle_t padHandle);
static UInt8 Pad_GetCcaMode(gpPad_Handle_t padHandle);

static void Pad_SetRetransmitOnCcaFail(gpPad_Handle_t padHandle, Bool enabled);
static Bool Pad_GetRetransmitOnCcaFail(gpPad_Handle_t padHandle);
static void Pad_SetRetransmitRandomBackoff(gpPad_Handle_t padHandle, Bool enabled);
static Bool Pad_GetRetransmitRandomBackoff(gpPad_Handle_t padHandle);

static void Pad_SetMinBeRetransmit(gpPad_Handle_t padHandle, UInt8 minBERetransmit);
static UInt8 Pad_GetMinBeRetransmit(gpPad_Handle_t padHandle);
static void Pad_SetMaxBeRetransmit(gpPad_Handle_t padHandle, UInt8 maxBERetransmit);
static UInt8 Pad_GetMaxBeRetransmit(gpPad_Handle_t padHandle);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

static void Pad_Init(void)
{
    UInt8 i;
    for(i = 0; i<GP_PAD_NR_OF_HANDLES; i++)
    {
        gpPad_Handles[i]=false;
        MEMSET(&gpPad_Descriptors[i], 0, sizeof(gpPad_Descriptor_t));
    }

    NOT_USED(Pad_SetTxChannels);
    NOT_USED(Pad_SetTxPower);
    NOT_USED(Pad_SetTxAntenna);
    NOT_USED(Pad_SetTxMinBE);
    NOT_USED(Pad_SetTxMaxBE);
    NOT_USED(Pad_SetTxMaxCsmaBackoffs);
    NOT_USED(Pad_SetTxMaxFrameRetries);
    NOT_USED(Pad_SetTxCsmaMode);
    NOT_USED(Pad_SetCcaMode);

    NOT_USED(Pad_GetAttributes);
    NOT_USED(Pad_GetTxChannels);
    NOT_USED(Pad_GetTxPower);
    NOT_USED(Pad_GetTxAntenna);
    NOT_USED(Pad_GetTxMinBE);
    NOT_USED(Pad_GetTxMaxBE);
    NOT_USED(Pad_GetTxMaxCsmaBackoffs);
    NOT_USED(Pad_GetTxMaxFrameRetries);
    NOT_USED(Pad_GetTxCsmaMode);
    NOT_USED(Pad_GetCcaMode);
    NOT_USED(Pad_SetRetransmitOnCcaFail);
    NOT_USED(Pad_GetRetransmitOnCcaFail);
    NOT_USED(Pad_SetRetransmitRandomBackoff);
    NOT_USED(Pad_GetRetransmitRandomBackoff);
    NOT_USED(Pad_SetMinBeRetransmit);
    NOT_USED(Pad_GetMinBeRetransmit);
    NOT_USED(Pad_SetMaxBeRetransmit);
    NOT_USED(Pad_GetMaxBeRetransmit);
}

static gpPad_Handle_t Pad_GetPad(gpPad_Attributes_t* pInitAttributes)
{
    gpPad_Handle_t padHandle = 0;

    for(padHandle = 0; padHandle < GP_PAD_NR_OF_HANDLES; padHandle++)
    {
        if(!PAD_CHECK_HANDLE_IN_USE(padHandle))
        {
            GP_ASSERT_DEV_EXT(pInitAttributes);
            PAD_HANDLE_CLAIM(padHandle);
            GP_LOG_PRINTF("G %i",0,(UInt16)padHandle);

            Pad_SetAttributes(padHandle, pInitAttributes);
            return padHandle;
        }
    }
    GP_LOG_PRINTF("G 0xFF",0);
    return GP_PAD_INVALID_HANDLE;
}

static void Pad_FreePad(gpPad_Handle_t padHandle)
{
    GP_LOG_PRINTF("F %i",0,(UInt16)padHandle);

    if(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle))
    {
        PAD_HANDLE_FREE(padHandle);
    }
    else
    {
        GP_ASSERT_DEV_EXT(false); //Make no fun about freeing unused handles!
    }
}

static gpPad_Result_t Pad_CheckPadValid(gpPad_Handle_t padHandle)
{
    gpPad_Result_t result = gpPad_ResultValidHandle;

    if(!PAD_CHECK_HANDLE_VALID(padHandle))
    {
        result = gpPad_ResultInvalidHandle;
    }
    else if(!PAD_CHECK_HANDLE_IN_USE(padHandle))
    {
        result = gpPad_ResultNotInUse;
    }
    GP_LOG_PRINTF("Chck %i %i",0,(UInt16)padHandle, (UInt16)result);

    return result;
}

//Attributes
void Pad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    GP_ASSERT_DEV_EXT(pAttributes);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));

    MEMCPY(&gpPad_Descriptors[padHandle],  pAttributes, sizeof(gpPad_Attributes_t));
#define pAttr  (&gpPad_Descriptors[padHandle])
    GP_LOG_PRINTF("ch:%u,%u,%u a:%u mBE:%u/%u",0, \
                                        pAttr->channels[0], pAttr->channels[1], pAttr->channels[2], \
                                        pAttr->antenna, \
                                        pAttr->maxBE, pAttr->minBE);
    GP_LOG_PRINTF("mBO:%u mFR:%u Pow:%i cs:%i/%i",0,\
                                        pAttr->maxCsmaBackoffs, pAttr->maxFrameRetries, \
                                        pAttr->txPower, \
                                        pAttr->csma, pAttr->cca);
#undef pAttr
}

void Pad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    GP_ASSERT_DEV_EXT(pAttributes);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));

    MEMCPY(pAttributes, &gpPad_Descriptors[padHandle], sizeof(gpPad_Attributes_t));
}

//Separate set's
static void Pad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    GP_ASSERT_DEV_EXT(channels);
    GP_LOG_PRINTF("[%i] ch:%i,%i,%i",0,padHandle,channels[0],channels[1],channels[2]);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    MEMCPY(gpPad_Descriptors[padHandle].channels, channels, GP_PAD_MAX_CHANNELS);
}
static void Pad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower)
{
    GP_LOG_PRINTF("[%i] txp:%i",0,padHandle,txPower);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));

    gpPad_Descriptors[padHandle].txPower = txPower;
}
static void Pad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna)
{
    GP_LOG_PRINTF("[%i] ant:%i",0,padHandle,antenna);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].antenna = antenna;
}
static void Pad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE)
{
    GP_LOG_PRINTF("[%i] minBE:%i",0,padHandle,minBE);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].minBE = minBE;
}
static void Pad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE)
{
    GP_LOG_PRINTF("[%i] maxBE:%i",0,padHandle,maxBE);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].maxBE = maxBE;
}
static void Pad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs)
{
    GP_LOG_PRINTF("[%i] maxBO:%i",0,padHandle,maxCsmaBackoffs);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].maxCsmaBackoffs = maxCsmaBackoffs;
}
static void Pad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries)
{
    GP_LOG_PRINTF("[%i] maxFR:%i",0,padHandle,maxFrameRetries);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].maxFrameRetries = maxFrameRetries;
}
static void Pad_SetTxCsmaMode(gpPad_Handle_t padHandle, UInt8 csma)
{
    GP_LOG_PRINTF("[%i] csma:%i",0,padHandle,csma);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].csma = csma;
}
static void Pad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 cca)
{
    GP_LOG_PRINTF("[%i] cca:%i",0,padHandle,cca);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].cca = cca;
}

//Separate get's
static void Pad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    GP_ASSERT_DEV_EXT(channels);
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));

    MEMCPY(channels, gpPad_Descriptors[padHandle].channels, 3);
}
static Int8 Pad_GetTxPower(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].txPower;
}
static UInt8 Pad_GetTxAntenna(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].antenna;
}
static UInt8 Pad_GetTxMinBE(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].minBE;
}
static UInt8 Pad_GetTxMaxBE(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].maxBE;
}
static UInt8 Pad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].maxCsmaBackoffs;
}
static UInt8 Pad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].maxFrameRetries;
}
static UInt8 Pad_GetTxCsmaMode(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].csma;
}
static UInt8 Pad_GetCcaMode(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].cca;
}
static void Pad_SetRetransmitOnCcaFail(gpPad_Handle_t padHandle, Bool enabled)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].retransmitOnCcaFail = enabled;
}
static Bool Pad_GetRetransmitOnCcaFail(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].retransmitOnCcaFail;
}
static void Pad_SetRetransmitRandomBackoff(gpPad_Handle_t padHandle, Bool enabled)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].retransmitRandomBackoff = enabled;
}
static Bool Pad_GetRetransmitRandomBackoff(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].retransmitRandomBackoff;
}

static void Pad_SetMinBeRetransmit(gpPad_Handle_t padHandle, UInt8 minBERetransmit)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].minBERetransmit = minBERetransmit;
}
static UInt8 Pad_GetMinBeRetransmit(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].minBERetransmit;
}
static void Pad_SetMaxBeRetransmit(gpPad_Handle_t padHandle, UInt8 maxBERetransmit)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    gpPad_Descriptors[padHandle].maxBERetransmit = maxBERetransmit;
}
static UInt8 Pad_GetMaxBeRetransmit(gpPad_Handle_t padHandle)
{
    GP_ASSERT_DEV_EXT(PAD_CHECK_HANDLE_ACCESSIBLE(padHandle));
    return gpPad_Descriptors[padHandle].maxBERetransmit;
}
