/*
 * Copyright (c) 2011-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
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

// General includes
#include "gpHal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define PD_CHECK_HANDLE_VALID(pdHandle)         GP_HAL_CHECK_PBM_VALID(pdHandle)
#define PD_CHECK_HANDLE_IN_USE(pdHandle)        GP_HAL_IS_PBM_ALLOCATED(pdHandle)
#define PD_CHECK_HANDLE_ACCESSIBLE(pdHandle)    (PD_CHECK_HANDLE_VALID(pdHandle) && PD_CHECK_HANDLE_IN_USE(pdHandle))
#define PD_HANDLE_TO_ADDRESS(pdHandle)          GP_HAL_PBM_ENTRY2ADDR(pdHandle)

// Only allowed for ZigBee pds
#define PD_WRAP_OFFSET(offset)                  (offset &= 0x7f)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

//Packet buffers
gpPd_Descriptor_t gpPd_Descriptors[GP_PD_NR_OF_HANDLES] GP_EXTRAM_SECTION_ATTR;

#if GPHAL_NUMBER_OF_PBMS_USED <= 8
UInt8 Pd_BitMaskOfPbmsClaimedBySw = 0;
#else
UInt32 Pd_BitMaskOfPbmsClaimedBySw = 0;
#endif

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void    Pd_Init(void);
static void    Pd_DeInit(void);

//Descriptor handling
static gpPd_Handle_t        Pd_GetPd(gpPd_BufferType_t type, UInt16 size);
static void                 Pd_FreePd(gpPd_Handle_t pdHandle);
static gpPd_Result_t        Pd_CheckPdValid(gpPd_Handle_t pdHandle);
static gpPd_BufferType_t    Pd_GetPdType(gpPd_Handle_t pdHandle);

//Data handling
static UInt8         Pd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset);
static void          Pd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte);
static void          Pd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, gpPd_Length_t length, UInt8* pData);
static void          Pd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, gpPd_Length_t length, UInt8* pData);

//Properties handling
static gpPd_Rssi_t  Pd_GetRssi(gpPd_Handle_t pdHandle);
static void         Pd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi);
static gpPd_Lqi_t   Pd_GetLqi(gpPd_Handle_t pdHandle);
static void         Pd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi);
static UInt32       Pd_GetRxTimestamp(gpPd_Handle_t pdHandle);
static UInt32       Pd_GetRxTimestampChip(gpPd_Handle_t pdHandle);
static void         Pd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp);
static UInt16*      Pd_GetPhaseSamplesBuffer(gpPd_Handle_t pdHandle);
static UInt32       Pd_GetTxTimestamp(gpPd_Handle_t pdHandle);
static UInt8        Pd_GetTxChannel(gpPd_Handle_t pdHandle);
static UInt8        Pd_GetTxRetryCntr(gpPd_Handle_t pdHandle);
static UInt8        Pd_GetTxCCACntr(gpPd_Handle_t pdHandle);
static gpPd_Lqi_t   Pd_GetTxAckLqi(gpPd_Handle_t pdHandle);

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

static void Pd_Init(void)
{
    UIntLoop i;

    COMPILE_TIME_ASSERT((sizeof(Pd_BitMaskOfPbmsClaimedBySw)*8) >= GP_PD_NR_OF_HANDLES);

    for(i = 0; i<GP_PD_NR_OF_HANDLES; i++)
    {
        MEMSET(&gpPd_Descriptors[i], 0, sizeof(gpPd_Descriptor_t));
    }
    Pd_BitMaskOfPbmsClaimedBySw = 0;
#ifdef GP_COMP_UNIT_TEST
    gpPd_InitUnit();
#endif // GP_COMP_UNIT_TEST
}

static void Pd_DeInit(void)
{
    UInt8 i;

    for(i = 0; i < GP_PD_NR_OF_HANDLES; i++)
    {
        if(BIT_TST(Pd_BitMaskOfPbmsClaimedBySw, i))
        {
            Pd_FreePd(i);
        }
    }
}

// for waking up the radio we only take PBM's into account claimed by the software
// So pbm's claimed for outgoing packets. Pbms for incoming packets are related to interrupts.
static gpPd_Handle_t Pd_GetPd(gpPd_BufferType_t type, UInt16 size)
{
    UInt8 pdHandle;
#if defined(GP_COMP_UNIT_TEST)
    if( Pd_BitMaskOfPbmsClaimedBySw == 0 )
    {
        gpHal_GoToSleepWhenIdle(false);
    }
#endif // GP_DIVERSITY_GPHAL_EXTERN
    pdHandle = gpHal_GetHandle(size);
    if(!GP_HAL_CHECK_PBM_VALID(pdHandle))
    {
#if defined(GP_COMP_UNIT_TEST)
        if( Pd_BitMaskOfPbmsClaimedBySw == 0 )
        {
            gpHal_GoToSleepWhenIdle(true);
        }
#endif // GP_DIVERSITY_GPHAL_EXTERN

        return GP_PD_INVALID_HANDLE;
    }

    GP_ASSERT_DEV_EXT(pdHandle < GP_PD_NR_OF_HANDLES);
    gpPd_Descriptors[pdHandle].type = type;
    BIT_SET(Pd_BitMaskOfPbmsClaimedBySw,pdHandle);
    GP_LOG_PRINTF("Get %i %lx",0, (UInt16)pdHandle, (unsigned long)Pd_BitMaskOfPbmsClaimedBySw);


    return pdHandle;
}

static void Pd_FreePd(gpPd_Handle_t pdHandle)
{
    GP_LOG_PRINTF("Free %i",0, (UInt16)pdHandle);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    gpHal_FreeHandle(pdHandle);
    if( Pd_BitMaskOfPbmsClaimedBySw)
    {
        BIT_CLR(Pd_BitMaskOfPbmsClaimedBySw,pdHandle);
#if defined(GP_COMP_UNIT_TEST)
        if( Pd_BitMaskOfPbmsClaimedBySw == 0 )
        {
            gpHal_GoToSleepWhenIdle(true);
        }
#endif // GP_DIVERSITY_GPHAL_EXTERN
    }
}

static gpPd_Result_t Pd_CheckPdValid(gpPd_Handle_t pdHandle)
{
    gpPd_Result_t result = gpPd_ResultValidHandle;

    if(!PD_CHECK_HANDLE_VALID(pdHandle))
    {
        result = gpPd_ResultInvalidHandle;
    }
    else if(!PD_CHECK_HANDLE_IN_USE(pdHandle))
    {
        result = gpPd_ResultNotInUse;
    }
    GP_LOG_PRINTF("Check %i %i",0, (UInt16)pdHandle, (UInt16)result);

    return result;
}

static gpPd_BufferType_t Pd_GetPdType(gpPd_Handle_t pdHandle)
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    return gpPd_Descriptors[pdHandle].type;
}

//------------------------------
// Data
//------------------------------

static UInt8 Pd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("R %i %i",0, (UInt16)pdHandle, (UInt16)offset);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    if(gpPd_Descriptors[pdHandle].type == gpPd_BufferTypeZigBee)
    {
        PD_WRAP_OFFSET(offset);
        return gpHal_ReadByteInPBMCyclic(address,offset);
    }
    else
    {
        return gpHal_ReadByteInPBM(address,offset);
    }
}

static void Pd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("W %i %i %x",0, (UInt16)pdHandle, (UInt16)offset, (UInt16)byte);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    if(gpPd_Descriptors[pdHandle].type == gpPd_BufferTypeZigBee)
    {
        PD_WRAP_OFFSET(offset);
        gpHal_WriteByteInPBMCyclic(address,offset, byte);
    }
    else
    {
        gpHal_WriteByteInPBM(address,offset, byte);
    }
}

static void Pd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, gpPd_Length_t length, UInt8* pData)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("RS %i %i %i",0, (UInt16)pdHandle, (UInt16)offset, (UInt16)length);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    if(gpPd_Descriptors[pdHandle].type == gpPd_BufferTypeZigBee)
    {
        PD_WRAP_OFFSET(offset);
        gpHal_ReadDataInPBMCyclic(address,offset,pData,length);
    }
    else
    {
        gpHal_ReadDataInPBM(address,offset,pData,length);
    }
}

static void Pd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, gpPd_Length_t length, UInt8* pData)
{
    gpHal_Address_t address;

    GP_LOG_PRINTF("WS %i %i %i",0, (UInt16)pdHandle, (UInt16)offset, (UInt16)length);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));

    address = PD_HANDLE_TO_ADDRESS(pdHandle);

    if(gpPd_Descriptors[pdHandle].type == gpPd_BufferTypeZigBee)
    {
        PD_WRAP_OFFSET(offset);
        gpHal_WriteDataInPBMCyclic(address,offset,pData,length);
    }
    else
    {
        gpHal_WriteDataInPBM(address,offset,pData,length);
    }
}

//------------------------------
// Attributes
//------------------------------

static gpPd_Rssi_t Pd_GetRssi(gpPd_Handle_t pdHandle)
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return gpPd_Descriptors[pdHandle].attr.rx.rssi;
}

static void Pd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi)
{
    //Not to be overwritten
    GP_ASSERT_DEV_EXT(false);
}

static gpPd_Lqi_t Pd_GetLqi(gpPd_Handle_t pdHandle)
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return gpPd_Descriptors[pdHandle].attr.rx.lqi;
}

static void Pd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi)
{
    //Not to be overwritten
    GP_ASSERT_DEV_EXT(false);
}

static UInt32 Pd_GetRxTimestamp(gpPd_Handle_t pdHandle)
{
    UInt32 timeStamp;
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    gpHal_GetRxTimestamp(pdHandle, &timeStamp);

#ifndef GP_DIVERSITY_GPHAL_INTERN
    {
    gpPd_TimeStamp_t currentChipTime;
    UInt32 currentHostTime;

    // convert "chip time basis" to "host time basis"
    gpHal_GetTime(&currentChipTime);
    HAL_TIMER_GET_CURRENT_TIME_1US( currentHostTime );
    timeStamp = currentHostTime - (currentChipTime - timeStamp);
    }
#endif //GP_DIVERSITY_GPHAL_INTERN

    return timeStamp;
}

static UInt32 Pd_GetRxTimestampChip(gpPd_Handle_t pdHandle)
{
    UInt32 timeStamp;
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    gpHal_GetRxTimestamp(pdHandle, &timeStamp);
    return timeStamp;
}

static void Pd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static UInt32 Pd_GetTxTimestamp(gpPd_Handle_t pdHandle)
{
    UInt32 timeStamp;
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    gpHal_GetTxTimestamp(pdHandle, &timeStamp);

#ifndef GP_DIVERSITY_GPHAL_INTERN
    {
    gpPd_TimeStamp_t currentChipTime;
    UInt32 currentHostTime;

    // convert "chip time basis" to "host time basis"
    gpHal_GetTime(&currentChipTime);
    HAL_TIMER_GET_CURRENT_TIME_1US( currentHostTime );
    timeStamp = currentHostTime - (currentChipTime - timeStamp);
    }
#endif //GP_DIVERSITY_GPHAL_INTERN

    return timeStamp;
}

static void Pd_SetTxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static UInt8 Pd_GetTxChannel(gpPd_Handle_t pdHandle)
{
    return gpPd_Descriptors[pdHandle].attr.txcfm.lastChannel;
}

static UInt16* Pd_GetPhaseSamplesBuffer(gpPd_Handle_t pdHandle)
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return gpHal_GetPhaseSamples(pdHandle);
}

static gpPd_Lqi_t Pd_GetTxAckLqi(gpPd_Handle_t pdHandle)
{
    return gpHal_GetTxAckLQI(pdHandle);
}

static UInt8 Pd_GetTxCCACntr(gpPd_Handle_t pdHandle)
{
    return gpHal_GetTxCCACntr(pdHandle);
}

static UInt8 Pd_GetTxRetryCntr(gpPd_Handle_t pdHandle)
{
    return gpHal_GetTxRetryCntr(pdHandle);
}

static void Pd_SetTxRetryCntr(gpPd_Handle_t pdHandle, UInt8 txRetryCntr)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static UInt8 Pd_GetFramePendingAfterTx(gpPd_Handle_t pdHandle)
{
    return gpHal_GetFramePendingFromTxPbm(pdHandle);
}

static void Pd_SetFramePendingAfterTx(gpPd_Handle_t pdHandle, UInt8 framePending)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static Bool Pd_GetRxEnhancedAckFromTxPbm(gpPd_Handle_t pdHandle)
{
    return gpHal_GetRxEnhancedAckFromTxPbm(pdHandle);
}

static void Pd_SetRxEnhancedAckFromTxPbm(gpPd_Handle_t pdHandle, Bool enhancedAck)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static UInt16 Pd_GetFrameControlFromTxAckAfterRx(gpPd_Handle_t pdHandle)
{
    return gpHal_GetFrameControlFromTxAckAfterRx(pdHandle);
}

static void Pd_SetFrameControlFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt16 frameControl)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static UInt32 Pd_GetFrameCounterFromTxAckAfterRx(gpPd_Handle_t pdHandle)
{
    return gpHal_GetFrameCounterFromTxAckAfterRx(pdHandle);
}

static void Pd_SetFrameCounterFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt32 frameCounter)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}

static UInt8 Pd_GetKeyIdFromTxAckAfterRx(gpPd_Handle_t pdHandle)
{
    return gpHal_GetKeyIdFromTxAckAfterRx(pdHandle);
}

static void Pd_SetKeyIdFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt8 keyId)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}


static UInt8 Pd_GetRxChannel(gpPd_Handle_t pdHandle)
{
    return gpHal_GetRxedChannel(pdHandle);
}

static void Pd_SetRxChannel(gpPd_Handle_t pdHandle, UInt8 rxChannel)
{
    //Not to be overwritten in PBM
    GP_ASSERT_DEV_EXT(false);
}
//------------------------------
// Helper functions
//------------------------------

static gpPd_Handle_t Pd_CopyPd( gpPd_Handle_t pdHandle)
{
    gpPd_Handle_t pdHandleCopy = gpPd_GetPd();
    if(pdHandleCopy != GP_PD_INVALID_HANDLE )
    {
        MEMCPY(&gpPd_Descriptors[pdHandleCopy].attr,  &gpPd_Descriptors[pdHandle].attr, sizeof(gpPd_Descriptors[pdHandle].attr));
        gpHal_MakeBareCopyPBM(pdHandle,pdHandleCopy);
    }
    return pdHandleCopy;
}

//--------------------
// Request/indications
//--------------------

static UInt8 Pd_DataRequest(gpPd_Loh_t *p_PdLoh)
{
    GP_ASSERT_DEV_EXT(p_PdLoh);
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(p_PdLoh->handle));
    return p_PdLoh->handle;
}


static void Pd_cbDataConfirm(UInt8 pbmHandle, gpPd_Offset_t pbmOffset, UInt16 pbmLength, gpPd_Loh_t *p_PdLoh)
{
    GP_ASSERT_DEV_EXT(p_PdLoh);
    p_PdLoh->length = pbmLength;
    p_PdLoh->offset = pbmOffset;
    p_PdLoh->handle = pbmHandle;
#ifdef GP_COMP_GPHAL_MAC
    gpPd_Descriptors[p_PdLoh->handle].attr.txcfm.lastChannel = gpHal_GetLastUsedChannel(pbmHandle);
#else
    gpPd_Descriptors[p_PdLoh->handle].attr.txcfm.lastChannel = 0xFF; // invalid channel
#endif
}

static void Pd_DataIndication(UInt8 pbmHandle, gpPd_Offset_t pbmOffset, UInt16 pbmLength, gpPd_Loh_t *p_PdLoh, gpPd_BufferType_t type)
{
    GP_ASSERT_DEV_EXT(p_PdLoh);
    p_PdLoh->length = pbmLength;
    p_PdLoh->offset = pbmOffset;
    p_PdLoh->handle = pbmHandle;
    gpPd_Descriptors[p_PdLoh->handle].type = type;
    gpPd_Descriptors[p_PdLoh->handle].attr.rx.lqi = gpHal_GetLQI(pbmHandle);
    gpPd_Descriptors[p_PdLoh->handle].attr.rx.rssi = gpHal_GetRSSI(pbmHandle);

#ifdef GP_COMP_GPHAL_BLE
    if (gpPd_BufferTypeBle == type)
    {
        Int8 TxPathCompensation, RxPathCompensation;

        gpHal_BleGetRfPathCompensation(&TxPathCompensation, &RxPathCompensation);
        gpPd_Descriptors[p_PdLoh->handle].attr.rx.rssi -= RxPathCompensation;
    }
#endif /* GP_COMP_GPHAL_BLE */
}

static UInt8 Pd_SecRequest(gpPd_Handle_t pdHandle, UInt8 dataOffset, UInt8 dataLength, UInt8 auxOffset, UInt8 auxLength )
{
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return pdHandle;
}

static gpPd_Handle_t Pd_cbSecConfirm(UInt8 pbmHandle, UInt8 dataOffset, UInt8 dataLength)
{

    return pbmHandle;
}

static UInt8 Pd_PurgeRequest(gpPd_Handle_t pdHandle)
{
    //Pdhandle = PBMentry in this case
    //Check if Pd valid and Pd in use !
    GP_ASSERT_DEV_EXT(PD_CHECK_HANDLE_ACCESSIBLE(pdHandle));
    return pdHandle;
}

static void Pd_cbPurgeConfirm(UInt8 pbmHandle)
{
    //Do nothing
}

gpPd_Handle_t Pd_GetPdFromPBM(UInt8 pbmHandle)
{
    return pbmHandle;
}
