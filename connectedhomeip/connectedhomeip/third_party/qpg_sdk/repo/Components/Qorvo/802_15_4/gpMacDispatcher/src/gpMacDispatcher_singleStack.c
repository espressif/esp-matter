/*
 * Copyright (c) 2017, 2019, Qorvo Inc
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
#include "gpAssert.h"
#include "gpLog.h"

#include "gpMacDispatcher.h"
#include "gpMacDispatcher_def.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_MACDISPATCHER

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define MACDISPATCHER_SINGLE_STACKID    0

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

void gpMacDispatcher_Init(void)
{
}

void gpMacDispatcher_DeInit(void)
{
}

gpMacDispatcher_Result_t gpMacDispatcher_Reset(Bool setDefaultPib, UInt8 stackId)
{
    gpMacDispatcher_Result_t result = gpMacCore_Reset(setDefaultPib, stackId);
    if (setDefaultPib && (result == gpMacCore_ResultSuccess))
    {
        MacDispatcher_InitAutoTxAntennaToggling(stackId);
    }
    return result;
}

Bool gpMacDispatcher_LockClaim(UInt8 stackId)
{
    return true;
}
void gpMacDispatcher_LockRelease(UInt8 stackId)
{
}

Bool gpMacDispatcher_LockedByThisStack(UInt8 stackId)
{
    return false;
}
Bool gpMacDispatcher_Locked(void)
{
    return false;
}

gpMacDispatcher_StackId_t gpMacDispatcher_RegisterNetworkStack(gpMacDispatcher_StringIdentifier_t* stringIdentifier)
{
    gpMacCore_StackAdded(MACDISPATCHER_SINGLE_STACKID);
    MacDispatcher_InitAutoTxAntennaToggling(MACDISPATCHER_SINGLE_STACKID);
    return MACDISPATCHER_SINGLE_STACKID;
}

gpMacCore_Result_t gpMacDispatcher_UnRegisterNetworkStack(gpMacDispatcher_StackId_t stackId)
{
    gpMacCore_StackRemoved(MACDISPATCHER_SINGLE_STACKID);
    return gpMacCore_ResultSuccess;
}

Bool gpMacDispatcher_IsValidStack(gpMacDispatcher_StackId_t stackId)
{
    return gpMacCore_cbValidStack(stackId);
}

void gpMacDispatcher_DataRequest(gpMacCore_AddressMode_t srcAddrMode, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, gpMacCore_Security_t *pSecOptions, gpMacCore_MultiChannelOptions_t multiChannelOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId)
{
    gpMacCore_DataRequest(srcAddrMode, pDstAddrInfo, txOptions, pSecOptions, multiChannelOptions, pdLoh, stackId);
}
#if (defined(GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR) \
  || defined(GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR) \
  || defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR) \
  || defined(GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR))
void gpMacDispatcher_ScanRequest(gpMacCore_ScanType_t scanType, UInt32 scanChannels, UInt8 scanDuration , UInt8 resultListSize, UInt8* pResultList, gpMacCore_StackId_t stackId)
{
    gpMacCore_ScanRequest(scanType, scanChannels, scanDuration, resultListSize, pResultList, stackId);
}

#endif //GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
void gpMacDispatcher_AssociateRequest(UInt8 logicalChannel, gpMacCore_AddressInfo_t* pCoordAddrInfo, UInt8 capabilityInformation, gpMacCore_StackId_t stackId)
{
    gpMacCore_AssociateRequest(logicalChannel, pCoordAddrInfo, capabilityInformation, stackId);
}

#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR

void gpMacDispatcher_AssociateResponse(MACAddress_t *pDeviceAddress, UInt16 associateShortAddress, gpMacCore_Result_t status, gpMacCore_StackId_t stackId)
{
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
    gpMacCore_AssociateResponse(pDeviceAddress, associateShortAddress, status, stackId);
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
}

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
void gpMacDispatcher_PollRequest(gpMacCore_AddressInfo_t* pCoordAddrInfo, gpMacCore_Security_t *pSecOptions, gpMacCore_StackId_t stackId)
{
    gpMacCore_PollRequest(pCoordAddrInfo, pSecOptions, stackId);
}

#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION
void gpMacDispatcher_PurgeRequest(gpPd_Handle_t pdHandle, gpMacCore_StackId_t stackId)
{
    gpMacCore_PurgeRequest(pdHandle, stackId);
}

#endif //GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION

void gpMacDispatcher_OrphanResponse(MACAddress_t* pOrphanAddress, UInt16 shortAddress, Bool associatedMember, gpMacCore_StackId_t stackId)
{
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
    gpMacCore_OrphanResponse(pOrphanAddress, shortAddress, associatedMember, stackId);
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
}

gpMacCore_Result_t gpMacDispatcher_Start(gpMacCore_PanId_t panId, UInt8 logicalChannel, Bool panCoordinator, UInt8 stackId)
{
    return gpMacCore_Start(panId, logicalChannel, panCoordinator, stackId);
}

/* getters and setters */
void gpMacDispatcher_SetCurrentChannel(UInt8 channel, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetCurrentChannel(channel, stackId);
}


UInt8 gpMacDispatcher_GetCurrentChannel(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetCurrentChannel(stackId);
}
void gpMacDispatcher_SetDefaultTransmitPowers(Int8* pDefaultTransmitPowerTable)
{
    gpMacCore_SetDefaultTransmitPowers(pDefaultTransmitPowerTable);
}
void gpMacDispatcher_SetTransmitPower(gpMacCore_TxPower_t transmitPower, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetTransmitPower(transmitPower, stackId);
}
Int8 gpMacDispatcher_GetTransmitPower(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetTransmitPower(stackId);
}

void gpMacDispatcher_SetCCAMode(UInt8 cCAMode, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetCCAMode(cCAMode, stackId);
}

UInt8 gpMacDispatcher_GetCCAMode(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetCCAMode(stackId);
}
void gpMacDispatcher_SetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetCoordExtendedAddress(pCoordExtendedAddress, stackId);
}

void gpMacDispatcher_GetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress , gpMacCore_StackId_t stackId)
{
    gpMacCore_GetCoordExtendedAddress(pCoordExtendedAddress, stackId);
}

void gpMacDispatcher_SetCoordShortAddress(UInt16 addr, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetCoordShortAddress(addr, stackId);
}

UInt16 gpMacDispatcher_GetCoordShortAddress(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetCoordShortAddress(stackId);
}

void gpMacDispatcher_SetPanCoordinator(Bool panCoordinator, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetPanCoordinator(panCoordinator, stackId);
}

Bool gpMacDispatcher_GetPanCoordinator(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetPanCoordinator(stackId);
}

void gpMacDispatcher_SetDsn(UInt8 dsn, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetDsn(dsn, stackId);
}

UInt8 gpMacDispatcher_GetDsn(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetDsn(stackId);
}

void gpMacDispatcher_SetMaxCsmaBackoffs(UInt8 maxCsmaBackoffs, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetMaxCsmaBackoffs(maxCsmaBackoffs, stackId);
}

UInt8 gpMacDispatcher_GetMaxCsmaBackoffs(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetMaxCsmaBackoffs(stackId);
}

void gpMacDispatcher_SetMinBE(UInt8 minBE, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetMinBE(minBE, stackId);
}

UInt8 gpMacDispatcher_GetMinBE(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetMinBE(stackId);
}

void gpMacDispatcher_SetMaxBE(UInt8 maxBE, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetMaxBE(maxBE, stackId);
}

UInt8 gpMacDispatcher_GetMaxBE(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetMaxBE(stackId);
}

void gpMacDispatcher_SetCsmaMode(UInt8 csmaMode, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetCsmaMode(csmaMode, stackId);
}

UInt8 gpMacDispatcher_GetCsmaMode(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetCsmaMode(stackId);
}

void gpMacDispatcher_SetPanId(UInt16 panId, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetPanId(panId, stackId);
}

UInt16 gpMacDispatcher_GetPanId(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetPanId(stackId);
}

void gpMacDispatcher_SetRxOnWhenIdle(Bool rxOnWhenIdle, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetRxOnWhenIdle(rxOnWhenIdle, stackId);
}

Bool gpMacDispatcher_GetRxOnWhenIdle(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetRxOnWhenIdle(stackId);
}

void gpMacDispatcher_SetShortAddress(UInt16 shortAddress, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetShortAddress(shortAddress, stackId);
}

UInt16 gpMacDispatcher_GetShortAddress(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetShortAddress(stackId);
}

void gpMacDispatcher_SetAssociationPermit(Bool associationPermit, gpMacCore_StackId_t stackId)
{
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
    gpMacCore_SetAssociationPermit(associationPermit, stackId);
#endif
}

Bool gpMacDispatcher_GetAssociationPermit(gpMacCore_StackId_t stackId)
{
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
    return gpMacCore_GetAssociationPermit(stackId);
#else
    return 0;
#endif
}

void gpMacDispatcher_SetBeaconPayload(UInt8* pBeaconPayload, gpMacCore_StackId_t stackId)
{
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
    gpMacCore_SetBeaconPayload(pBeaconPayload, stackId);
#endif
}

void gpMacDispatcher_GetBeaconPayload(UInt8* pBeaconPayload, gpMacCore_StackId_t stackId)
{
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
    gpMacCore_GetBeaconPayload(pBeaconPayload, stackId);
#endif
}

void gpMacDispatcher_SetBeaconPayloadLength(UInt8 length, gpMacCore_StackId_t stackId)
{
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
    gpMacCore_SetBeaconPayloadLength(length, stackId);
#endif
}

UInt8 gpMacDispatcher_GetBeaconPayloadLength(gpMacCore_StackId_t stackId)
{
#if defined(GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT)
    return gpMacCore_GetBeaconPayloadLength(stackId);
#else
    return 0;
#endif
}

void gpMacDispatcher_SetPromiscuousMode(UInt8 promiscuousMode, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetPromiscuousMode(promiscuousMode, stackId);
}

UInt8 gpMacDispatcher_GetPromiscuousMode(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetPromiscuousMode(stackId);
}

void gpMacDispatcher_SetTransactionPersistenceTime(UInt16 time, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetTransactionPersistenceTime(time, stackId);
}

UInt16 gpMacDispatcher_GetTransactionPersistenceTime(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetTransactionPersistenceTime(stackId);
}

void gpMacDispatcher_SetExtendedAddress(MACAddress_t* pExtendedAddress, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetExtendedAddress(pExtendedAddress, stackId);
}

void gpMacDispatcher_GetExtendedAddress(MACAddress_t* pExtendedAddress, gpMacCore_StackId_t stackId)
{
    gpMacCore_GetExtendedAddress(pExtendedAddress, stackId);
}

void gpMacDispatcher_SetNumberOfRetries(UInt8 numberOfRetries, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetNumberOfRetries(numberOfRetries, stackId);
}

UInt8 gpMacDispatcher_GetNumberOfRetries(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetNumberOfRetries(stackId);
}

void gpMacDispatcher_SetSecurityEnabled(Bool securityEnabled, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetSecurityEnabled(securityEnabled, stackId);
}

Bool gpMacDispatcher_GetSecurityEnabled(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetSecurityEnabled(stackId);
}

void gpMacDispatcher_SetBeaconStarted(Bool BeaconStarted, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetBeaconStarted(BeaconStarted, stackId);
}

Bool gpMacDispatcher_GetBeaconStarted(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetBeaconStarted(stackId);
}
void gpMacDispatcher_SetIndicateBeaconNotifications(Bool enable, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetIndicateBeaconNotifications(enable, stackId);
}

Bool gpMacDispatcher_GetIndicateBeaconNotifications(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetIndicateBeaconNotifications(stackId);
}

void gpMacDispatcher_SetForwardPollIndications(Bool enable, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetForwardPollIndications(enable, stackId);
}

Bool gpMacDispatcher_GetForwardPollIndications(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetForwardPollIndications(stackId);
}
void gpMacDispatcher_SetMacVersion(gpMacCore_MacVersion_t macVersion , gpMacCore_StackId_t stackId)
{
    gpMacCore_SetMacVersion(macVersion, stackId);
}

gpMacCore_MacVersion_t gpMacDispatcher_GetMacVersion(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetMacVersion(stackId);
}
#ifdef GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION
gpMacCore_Result_t gpMacDispatcher_DataPending_QueueAdd(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId)
{
    return gpMacCore_DataPending_QueueAdd(pAddrInfo, stackId);
}

gpMacCore_Result_t gpMacDispatcher_DataPending_QueueRemove(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId)
{
    return gpMacCore_DataPending_QueueRemove(pAddrInfo, stackId);
}

gpMacCore_Result_t gpMacDispatcher_DataPending_QueueClear(gpMacCore_StackId_t stackId)
{
    return gpMacCore_DataPending_QueueClear(stackId);
}

#endif //GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION

Bool gpMacDispatcher_AddNeighbour   (gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId)
{
#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
    return gpMacCore_AddNeighbour(pAddrInfo, stackId);
#else
    return false;
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT
}

Bool gpMacDispatcher_RemoveNeighbour(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId)
{
#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
    return gpMacCore_RemoveNeighbour(pAddrInfo, stackId);
#else
    return false;
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT
}

void gpMacDispatcher_ClearNeighbours(gpMacCore_StackId_t stackId)
{
#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
    gpMacCore_ClearNeighbours(stackId);
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT
}

gpMacCore_Result_t gpMacDispatcher_SetDataPendingMode(gpMacCore_DataPendingMode_t dataPendingMode, gpMacCore_StackId_t stackId)
{
#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
    return gpMacCore_SetDataPendingMode(dataPendingMode, stackId);
#else
    return gpMacCore_ResultSuccess;
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT
}



void gpMacDispatcher_EnableEnhancedFramePending(Bool enableEnhancedAck, gpMacCore_StackId_t stackId)
{
    gpMacCore_EnableEnhancedFramePending(enableEnhancedAck, stackId);
}

#ifdef GP_MACCORE_DIVERSITY_RAW_FRAMES
void gpMacDispatcher_SetStackInRawMode(Bool rawModeEnabled, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetStackInRawMode(rawModeEnabled, stackId);
}

Bool gpMacDispatcher_GetStackInRawMode(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetStackInRawMode(stackId);
}


gpMacCore_Result_t gpMacDispatcher_ConfigureEnhAckProbing(UInt8 linkMetrics, MACAddress_t* pExtendedAddress, UInt16 shortAddress, gpMacCore_StackId_t stackId)
{
     return gpMacCore_ConfigureEnhAckProbing(linkMetrics, pExtendedAddress, shortAddress, stackId);
}
#endif //GP_MACCORE_DIVERSITY_RAW_FRAMES


void gpMacDispatcher_SetRetransmitOnCcaFail(Bool enable, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetRetransmitOnCcaFail(enable, stackId);
}

Bool gpMacDispatcher_GetRetransmitOnCcaFail(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetRetransmitOnCcaFail(stackId);
}

void gpMacDispatcher_SetRetransmitRandomBackoff(Bool enable, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetRetransmitRandomBackoff(enable, stackId);
}

Bool gpMacDispatcher_GetRetransmitRandomBackoff(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetRetransmitRandomBackoff(stackId);
}

void gpMacDispatcher_SetMinBeRetransmit(UInt8 minBERetransmit, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetMinBeRetransmit(minBERetransmit, stackId);
}

UInt8 gpMacDispatcher_GetMinBeRetransmit(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetMinBeRetransmit(stackId);
}

void gpMacDispatcher_SetMaxBeRetransmit(UInt8 maxBERetransmit, gpMacCore_StackId_t stackId)
{
    gpMacCore_SetMaxBeRetransmit(maxBERetransmit, stackId);
}

UInt8 gpMacDispatcher_GetMaxBeRetransmit(gpMacCore_StackId_t stackId)
{
    return gpMacCore_GetMaxBeRetransmit(stackId);
}
