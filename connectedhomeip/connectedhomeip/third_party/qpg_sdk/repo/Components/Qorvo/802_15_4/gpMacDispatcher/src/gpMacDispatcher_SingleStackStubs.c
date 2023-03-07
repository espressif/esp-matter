/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpMacDispatcher.c
 *   This file contains the implementation of the MAC dispatcher.
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

#include "gpMacDispatcher.h"
#include "gpMacDispatcher_def.h"


/*****************************************************************************
 *                    Application callbacks
 *****************************************************************************/

void gpMacCore_cbDataConfirm(gpMacCore_Result_t status, gpPd_Handle_t pdHandle)
{
    MacDispatcher_HandleAutoTxAntennaToggling(status, 0);
    if(gpMacDispatcher_StackMapping[0].callbacks.dataConfirmCallback)
    {
        gpMacDispatcher_StackId_t stackId = gpMacDispatcher_StackMapping[0].numericId;
        gpMacDispatcher_StackMapping[0].callbacks.dataConfirmCallback(status, pdHandle, stackId);
    }
}

void gpMacCore_cbDataIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 dsn,
                                gpMacCore_Security_t *pSecOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.dataIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.dataIndicationCallback(pSrcAddrInfo, pDstAddrInfo, dsn, pSecOptions, pdLoh, stackId) ;
    }
}

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
void gpMacCore_cbPollConfirm( gpMacCore_Result_t status, gpMacCore_AddressInfo_t* pCoordAddrInfo, gpPd_TimeStamp_t txTime  )
{
    if(gpMacDispatcher_StackMapping[0].callbacks.pollConfirmCallback)
    {
        gpMacDispatcher_StackId_t stackId = gpMacDispatcher_StackMapping[0].numericId;
        gpMacDispatcher_StackMapping[0].callbacks.pollConfirmCallback(status, pCoordAddrInfo, txTime, stackId);
    }
}
#endif

#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
void gpMacCore_cbPollIndication(gpMacCore_AddressInfo_t* pAddrInfo, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.pollIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.pollIndicationCallback(pAddrInfo, rxTime, stackId);
    }
}

void gpMacCore_cbPollNotify(gpMacCore_AddressInfo_t* pAddrInfo, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime, gpPd_Handle_t pdHandle, Bool fromNeighbour)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.pollNotifyCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.pollNotifyCallback(pAddrInfo, rxTime, pdHandle, fromNeighbour, stackId);
    }
}
#endif // GP_MACCORE_DIVERSITY_POLL_RECIPIENT

#ifdef GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION
void gpMacCore_cbPurgeConfirm( gpMacCore_Result_t status, gpPd_Handle_t pdHandle )
{
    if(gpMacDispatcher_StackMapping[0].callbacks.purgeConfirmCallback)
    {
        gpMacDispatcher_StackId_t stackId = gpMacDispatcher_StackMapping[0].numericId;
        gpMacDispatcher_StackMapping[0].callbacks.purgeConfirmCallback(status, pdHandle, stackId);
    }
}
#endif //GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION

#if (defined(GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR) \
  || defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR))
// This function call is the result of an active scan request - invoke only the stack, which is having the dispatcher lock
void gpMacCore_cbBeaconNotifyIndication( UInt8 bsn, gpMacCore_PanDescriptor_t* pPanDescriptor, gpMacCore_StackId_t stackId , UInt8 beaconPayloadLength , UInt8 *pBeaconPayload )
{
    if(gpMacDispatcher_StackMapping[0].callbacks.beaconNotifyIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.beaconNotifyIndicationCallback(bsn, pPanDescriptor, beaconPayloadLength , pBeaconPayload, stackId);
    }
}
#endif

#if (defined(GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR) \
  || defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR) \
  || defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR) \
  || defined(GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR))
void gpMacCore_cbScanConfirm(gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, UInt32 unscannedChannels, UInt8 resultListSize, UInt8* pEdScanResultList)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.scanConfirmCallback)
    {
        gpMacDispatcher_StackId_t stackId = gpMacDispatcher_StackMapping[0].numericId;
        gpMacDispatcher_StackMapping[0].callbacks.scanConfirmCallback(status, scanType, unscannedChannels, resultListSize, pEdScanResultList, stackId);
    }
}

#endif // GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
void gpMacCore_cbAssociateConfirm(UInt16 assocShortAddress, gpMacCore_AssocStatus_t status, gpPd_TimeStamp_t txTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.assocConfirmCallback)
    {
        gpMacDispatcher_StackId_t stackId = gpMacDispatcher_StackMapping[0].numericId;
        gpMacDispatcher_StackMapping[0].callbacks.assocConfirmCallback(assocShortAddress, status, txTime, stackId);
    }
}
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
void gpMacCore_cbAssociateIndication(gpMacCore_Address_t* pDeviceAddress, UInt8 capabilityInformation, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.assocIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.assocIndicationCallback(pDeviceAddress, capabilityInformation, rxTime, stackId);
    }
}

void gpMacCore_cbAssociateCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.associateCommStatusIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.associateCommStatusIndicationCallback(pSrcAddrInfo, pDstAddrInfo, status, txTime, stackId);
    }
}
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
void gpMacCore_cbOrphanCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.orphanCommStatusIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.orphanCommStatusIndicationCallback(pSrcAddrInfo, pDstAddrInfo, status, txTime, stackId);
    }
}
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT

void gpMacCore_cbOrphanIndication(MACAddress_t* pOrphanAddress, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.orphanIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.orphanIndicationCallback(pOrphanAddress, rxTime, stackId);
    }
}

void gpMacCore_cbSecurityFailureCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t time)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.securityFailureCommStatusIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.securityFailureCommStatusIndicationCallback(pSrcAddrInfo, pDstAddrInfo, status, time, stackId);
    }
}

void gpMacCore_cbSecurityFrameCounterIndication(UInt32 frameCounter, gpMacDispatcher_StackId_t stackId)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.securityFrameCounterIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.securityFrameCounterIndicationCallback(frameCounter, stackId);
    }
}

Bool gpMacCore_cbValidStack(UInt8 stackId)
{
    return (stackId != GP_MAC_DISPATCHER_INVALID_STACK_ID);
}

