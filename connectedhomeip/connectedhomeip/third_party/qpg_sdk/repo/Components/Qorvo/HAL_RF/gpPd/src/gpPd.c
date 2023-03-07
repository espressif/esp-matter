/*
 * Copyright (c) 2012-2016, GreenPeak Technologies
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/



// General includes
#include "gpLog.h"
#include "gpAssert.h"
#include "gpStat.h"

#include "gpPd.h"


#if !defined(GP_DIVERSITY_PD_USE_PBM_VARIANT)
#error Error: Choose ram or pbm variant (GP_DIVERSITY_PD_USE_RAM_VARIANT or GP_DIVERSITY_PD_USE_PBM_VARIANT) !
#endif

#define GP_COMPONENT_ID GP_COMPONENT_ID_PD
#define GP_MODULE_ID GP_MODULE_ID_PD

#ifdef GP_DIVERSITY_PD_USE_PBM_VARIANT
#include "gpPd_pbm.c"
#else //GP_DIVERSITY_PD_USE_PBM_VARIANT
#include "gpPd_ram.c"
#endif //GP_DIVERSITY_PD_USE_PBM_VARIANT
#include "gpPd_common.c"

//#define GP_LOCAL_LOG

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
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpPd_Init(void)
{
    Pd_Init();
}

void gpPd_DeInit(void)
{
    Pd_DeInit();
    gpPd_Init();
}

gpPd_Handle_t gpPd_GetPd(void)
{
    return Pd_GetPd(gpPd_BufferTypeZigBee, GP_PD_BUFFER_SIZE_ZIGBEE);
}

gpPd_Handle_t gpPd_GetCustomPd(gpPd_BufferType_t type, UInt16 size)
{
    GP_STAT_SAMPLE_TIME();
    return Pd_GetPd(type, size);
}

void gpPd_FreePd(gpPd_Handle_t pdHandle)
{
    GP_STAT_SAMPLE_TIME();
    Pd_FreePd( pdHandle);
}

gpPd_Result_t gpPd_CheckPdValid(gpPd_Handle_t pdHandle)
{
    return Pd_CheckPdValid( pdHandle);
}

gpPd_BufferType_t gpPd_GetPdType(gpPd_Handle_t pdHandle)
{
    return Pd_GetPdType(pdHandle);
}

UInt8 gpPd_ReadByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset)
{
    return Pd_ReadByte( pdHandle, offset);
}

void gpPd_WriteByte(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 byte)
{
    Pd_WriteByte( pdHandle, offset, byte);
}

void gpPd_ReadByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData)
{
    Pd_ReadByteStream( pdHandle, offset, length, pData);
}

void gpPd_WriteByteStream(gpPd_Handle_t pdHandle, gpPd_Offset_t offset, UInt8 length, UInt8* pData)
{
    Pd_WriteByteStream( pdHandle, offset, length, pData);
}

//Convenience functions
void gpPd_AppendWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    Pd_AppendWithUpdate(pPdLoh, length, pData);
}
void gpPd_PrependWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 const *pData)
{
    Pd_PrependWithUpdate(pPdLoh, length, pData);
}
void gpPd_ReadWithUpdate(gpPd_Loh_t *pPdLoh, UInt8 length, UInt8 *pData)
{
    Pd_ReadWithUpdate(pPdLoh, length, pData);
}

//Properties handling

gpPd_Rssi_t gpPd_GetRssi(gpPd_Handle_t pdHandle)
{
    return Pd_GetRssi(pdHandle);
}

void gpPd_SetRssi(gpPd_Handle_t pdHandle, gpPd_Rssi_t rssi)
{
    Pd_SetRssi(pdHandle, rssi);
}

gpPd_Lqi_t gpPd_GetLqi(gpPd_Handle_t pdHandle)
{
    return Pd_GetLqi(pdHandle);
}

void gpPd_SetLqi(gpPd_Handle_t pdHandle, gpPd_Lqi_t lqi)
{
    Pd_SetLqi(pdHandle, lqi);
}

gpPd_TimeStamp_t gpPd_GetRxTimestamp(gpPd_Handle_t pdHandle)
{
    return Pd_GetRxTimestamp( pdHandle);
}

gpPd_TimeStamp_t gpPd_GetRxTimestampChip(gpPd_Handle_t pdHandle)
{
    return Pd_GetRxTimestampChip(pdHandle);
}

UInt16* gpPd_GetPhaseSamplesBuffer(gpPd_Handle_t pdHandle)
{
    return Pd_GetPhaseSamplesBuffer(pdHandle);
}

gpPd_TimeStamp_t gpPd_GetTxTimestamp(gpPd_Handle_t pdHandle)
{
    return Pd_GetTxTimestamp( pdHandle);
}

UInt8 gpPd_GetTxChannel(gpPd_Handle_t pdHandle)
{
    return Pd_GetTxChannel( pdHandle);
}

void gpPd_SetRxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp)
{
    Pd_SetRxTimestamp(pdHandle, timestamp);
}

void gpPd_SetTxTimestamp(gpPd_Handle_t pdHandle, gpPd_TimeStamp_t timestamp)
{
    Pd_SetTxTimestamp(pdHandle, timestamp);
}

gpPd_Handle_t gpPd_CopyPd(gpPd_Handle_t pdHandle)
{
    return Pd_CopyPd( pdHandle );
}

gpPd_Lqi_t gpPd_GetTxAckLqi(gpPd_Handle_t pdHandle)
{
    return Pd_GetTxAckLqi(pdHandle);
}

UInt8 gpPd_GetTxCCACntr(gpPd_Handle_t pdHandle)
{
    return Pd_GetTxCCACntr(pdHandle);
}

UInt8 gpPd_GetTxRetryCntr(gpPd_Handle_t pdHandle)
{
    return Pd_GetTxRetryCntr(pdHandle);
}

void gpPd_SetTxRetryCntr(gpPd_Handle_t pdHandle, UInt8 txRetryCntr)
{
    Pd_SetTxRetryCntr(pdHandle, txRetryCntr);
}

UInt8 gpPd_GetFramePendingAfterTx(gpPd_Handle_t pdHandle)
{
    return Pd_GetFramePendingAfterTx(pdHandle);
}

void gpPd_SetFramePendingAfterTx(gpPd_Handle_t pdHandle, UInt8 framePending)
{
    Pd_SetFramePendingAfterTx(pdHandle, framePending);
}

Bool gpPd_GetRxEnhancedAckFromTxPbm(gpPd_Handle_t pdHandle)
{
    return Pd_GetRxEnhancedAckFromTxPbm(pdHandle);
}

void gpPd_SetRxEnhancedAckFromTxPbm(gpPd_Handle_t pdHandle, Bool enhancedAck)
{
    Pd_SetRxEnhancedAckFromTxPbm(pdHandle, enhancedAck);
}

UInt16 gpPd_GetFrameControlFromTxAckAfterRx(gpPd_Handle_t pdHandle)
{
    return Pd_GetFrameControlFromTxAckAfterRx(pdHandle);
}

void gpPd_SetFrameControlFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt16 frameControl)
{
    Pd_SetFrameControlFromTxAckAfterRx(pdHandle, frameControl);
}


UInt32 gpPd_GetFrameCounterFromTxAckAfterRx(gpPd_Handle_t pdHandle)
{
    return Pd_GetFrameCounterFromTxAckAfterRx(pdHandle);
}

void gpPd_SetFrameCounterFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt32 frameCounter)
{
    Pd_SetFrameCounterFromTxAckAfterRx(pdHandle, frameCounter);
}

UInt8 gpPd_GetKeyIdFromTxAckAfterRx(gpPd_Handle_t pdHandle)
{
    return Pd_GetKeyIdFromTxAckAfterRx(pdHandle);
}

void gpPd_SetKeyIdFromTxAckAfterRx(gpPd_Handle_t pdHandle, UInt8 keyId)
{
    Pd_SetKeyIdFromTxAckAfterRx(pdHandle, keyId);
}


UInt8 gpPd_GetRxChannel(gpPd_Handle_t pdHandle)
{
    return Pd_GetRxChannel(pdHandle);
}

void  gpPd_SetRxChannel(gpPd_Handle_t pdHandle, UInt8 rxChannel)
{
    Pd_SetRxChannel(pdHandle, rxChannel);
}

#if defined(GP_COMP_GPHAL) 

UInt8 gpPd_DataRequest(gpPd_Loh_t *p_PdLoh)
{
    return Pd_DataRequest(p_PdLoh);
}

#ifdef GP_COMP_GPHAL_PBM
void gpPd_cbDataConfirm(gpPd_Handle_t pbmHandle, UInt16 pbmOffset, UInt16 pbmLength, gpPd_Loh_t *p_PdLoh)
{
    Pd_cbDataConfirm(pbmHandle, pbmOffset, pbmLength, p_PdLoh);
}
#endif // GP_COMP_GPHAL_PBM

UInt8 gpPd_SecRequest(gpPd_Handle_t pdHandle, UInt8 dataOffset, UInt8 dataLength, UInt8 auxOffset, UInt8 auxLength )
{
    return Pd_SecRequest(pdHandle, dataOffset, dataLength, auxOffset,auxLength );
}

gpPd_Handle_t gpPd_cbSecConfirm(gpPd_Handle_t pbmHandle, UInt8 dataOffset, UInt8 dataLength)
{
    return Pd_cbSecConfirm(pbmHandle, dataOffset, dataLength);
}

void gpPd_DataIndication(gpPd_Handle_t pbmHandle, UInt16 pbmOffset, UInt16 pbmLength, gpPd_Loh_t *p_PdLoh, gpPd_BufferType_t type)
{
    Pd_DataIndication(pbmHandle, pbmOffset, pbmLength, p_PdLoh, type);
}

UInt8 gpPd_PurgeRequest(gpPd_Handle_t pdHandle)
{
    return Pd_PurgeRequest(pdHandle);
}

void gpPd_cbPurgeConfirm(UInt8 pbmHandle)
{
    Pd_cbPurgeConfirm(pbmHandle);
}

gpPd_Handle_t gpPd_GetPdFromPBM(UInt8 pbmHandle)
{
    return Pd_GetPdFromPBM(pbmHandle);
}
#endif //defined(GP_COMP_GPHAL) && !defined(GP_DIVERSITY_GPHAL_COPROC)
