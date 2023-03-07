/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
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

#ifndef _GP_MACDISPATCHER_MULTISTACK_H_
#define _GP_MACDISPATCHER_MULTISTACK_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpMacCore.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Single/Dual Stack common definitions
 *****************************************************************************/

/**
 * @file gpMacDispatcher.h
 * @brief The MacDispatcher locks the lower layer before processing any request, after the cbConfirm the lock is released.
 *
 * @defgroup INIT INIT primitives
 * This module groups the initialization routines.
 *
 * @defgroup DATA DATA primitives
 * This module groups the primitives for data transmission.
 * - gpMacDispatcher_DataRequest + cbConfirm, cbIndication
 * - gpMacDispatcher_PollRequest + cbConfirm, cbIndication
 *
 * @defgroup MANAGEMENT MANAGEMENT primitives
 * - gpMacDispatcher_ScanRequest
 * - gpMacDispatcher_AssociateRequest
 * - gpMacDispatcher_AssociateResponse
 *
 * @defgroup ATTRIBUTE ATTRIBUTE primitives
 * - gpMacDispatcher_ScanRequest
 * - gpMacDispatcher_AssociateRequest
 * - gpMacDispatcher_AssociateResponse
 */

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpMacDispatcher_CodeJumpTableFlash_Defs_MultiStack.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/** @ingroup INIT
 *
 * @brief This function initializes the MacCore layer. It should be called before calling any other request function.
 *
*/
GP_API void gpMacDispatcher_Init(void);
GP_API void gpMacDispatcher_DeInit(void);

/** @ingroup INIT
 *
 * @brief This function resets the MACDispatcher + Core layer.
 *
 *  @param setDefaultPib    If true the PIB values are reset to their default value (as specified in the IEEE802.15.4-2006 specification).
 *  @param stackId          The PIB values are reset to their default value (as specified in the IEEE802.15.4-2006 specification).
 *
 *  @note       A prerequisite; the invoker of this function should first aqcuire the Mac Dispatcher lock before a reset is allowed
 *  @return     Whether this stack has successfully issued a reset command on the MAC network layer access (true) or not (false).
*/
GP_API gpMacDispatcher_Result_t gpMacDispatcher_Reset( Bool setDefaultPib, UInt8 stackId );

/** @ingroup LOCK
 *  This function is used to request the lock for the MAC network layer access. This lock must be acquired by a stack before it may access the MAC network layer.
 *
 *  @param stackId          The identifier of the stack that requests the lock.
 *
 *  @return                 Whether this stack has successfully acquired the lock on the MAC network layer access (true) or not (false).
*/
GP_API Bool gpMacDispatcher_LockClaim( UInt8 stackId );

/** @ingroup LOCK
 *  This function is used to release the lock for the MAC network layer access.
 *
 *  @param stackId             The identifier of the stack.
*/
GP_API void gpMacDispatcher_LockRelease( UInt8 stackId );

/** @ingroup LOCK
 *  This function can be used to check whether the stack id already has the lock on the MAC network layer access.
 *
 *  @param stackId          The identifier of the stack
 *
 *  @return                 Whether this stack has the lock on the MAC network layer access (true) or not (false).
*/
GP_API Bool gpMacDispatcher_LockedByThisStack( UInt8 stackId );

/** @ingroup LOCK
 *  This function can be used to check whether any stack has the lock on the MAC network layer access.
 *
 *  @return                 Whether a stack has the lock on the MAC network layer access (true) or not (false).
*/
GP_API Bool gpMacDispatcher_Locked( void );


/** @ingroup INIT
 * @brief This function is used to register a network stack to the dispatcher.
 *
 * @return The id that is allocated by the MacCore for this stack
*/
GP_API gpMacDispatcher_StackId_t gpMacDispatcher_RegisterNetworkStack(gpMacDispatcher_StringIdentifier_t* stringIdentifier);

/** @brief Unregisters a network stack to the MAC layer.
 *
 *  @param stackId   the stack ID of the stack that needs to be removed from the MAC layer
 *  @return result   Result of the unregistering
 *                   Possible results:
 *                   - gpMacCore_ResultSuccess
 *                   - gpMacCore_ResultInvalidParameter - stackId is not registered or not in supported range
*/
GP_API gpMacCore_Result_t gpMacDispatcher_UnRegisterNetworkStack(gpMacDispatcher_StackId_t stackId);

/** @brief Returns if a stackId is in range and registered
 *
 *  @param stackId   the stack ID to check
 *  @return result   Possible results:
 *                   - true  - stack is registered and in range
 *                   - false - stackId is not registered or not in supported range
*/
GP_API Bool gpMacDispatcher_IsValidStack(gpMacDispatcher_StackId_t stackId);

/** @ingroup DATA
 * @brief This function is used to send a data packet to another device.
 *
 *  This function is used to send a data packet conform the IEEE802.15.4-2006 MAC specification.
 *
 *  @param srcAddrMode    The address modes to be used for source address.      This parameter is used as in the Frame Control Field of an IEEE802.15.4-2006 packet. We refer to the define GP_IEEEMAC_SRC_ADDR_(EXT/SHORT).
 *  @param pDstAddrInfo   All the information about the destination (address mode, address and panId).
 *  @param txOptions      Tx Options byte (see IEEE802.15.4-2006).
 *  @param pSecOptions    The security options - maybe NULL to be used (see IEEE802.15.4-2006).
 *  @param p_PdLoh        The PD Length/Offset/Handle of the data payload.
 *  @param stackId        The stack doing this data request
*/
GP_API void gpMacDispatcher_DataRequest(gpMacCore_AddressMode_t srcAddrMode, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, gpMacCore_Security_t *pSecOptions, gpMacCore_MultiChannelOptions_t multiChannelOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId);

/** @brief Schedule a packet for timed transmission.
 *
 *  To send a packet via timed transmission, perform two steps:
 *  - Call gpMacDispatcher_DataRequest() with option GP_MACCORE_TX_OPT_TIMEDTX
 *    to queue the packet;
    - Call gpMacDispatcher_ScheduleTimedTx() to schedule transmission of the queued packet.
 *
 *  This function is only available on hardware that supports a timed transmission queue.
 *
 *  @param pdHandle       Handle of TX packet to schedule for TX; must have been previously submitted via gpMacDispatcher_DataRequest().
 *  @param timingOptions  Specification of scheduled transmit time.
 *  @param stackId        The identifier of the stack doing this request
 */
GP_API gpMacCore_Result_t gpMacDispatcher_ScheduleTimedTx(gpPd_Handle_t pdHandle, gpMacCore_TxTimingOptions_t timingOptions, gpMacDispatcher_StackId_t stackId);

/** @ingroup MANAGEMENT
 * @brief This function is used to start a scan.
 *
 *  This function starts a scan conform the IEEE802.15.4-2006 spec. A scan can be issued to detect a channel with the least amount of interference, to find networks, etc.
 *
 *  @param scanType        This parameter defines which of the 3 implemented scans needs to be performed: ED, active or orphan scan.
 *  @param scanChannels    This parameter defines which channels need to be scanned. It is a bitmask where bit 0 must be '1' to select channel 0, bit 26 '1' to select channel 26, etc. Since the chip works in the 2.4GHz band, only channels 11 till 26 can be selected. If other channels are selected, they are ignored and only the selected channels between channel 11 and channel 26 are scanned.
 *  @param scanDuration    This parameter specifies how long a channel will be scanned. The scantime equals [GP_IEEEMAC_BASE_SUPERFRAME_DURATION * (2^scanduration + 1)]symbols, where 1 symbol is 16us.
 *  @param resultListSize  The length of the resultList (in bytes). If an ED scan is done, this should equal the amount of channels. If resultListSize is higher than the amount of channels that needs to be scanned there is no problem. If the value is lower, the scan is terminated when the list is full.
 *                         If an active scan is selected, resultListSize is the amount of PANDescriptors that can be saved.
 *                         If it is an orphan scan, resultListSize is 0.
 *  @param pResultList     This is a pointer to an array where the result of the scan can be saved. If an ED scan is done, the size is resultListSize. If an active scan is issued the size is resultListSize*sizeof(gpMacCore_PanDescriptor_t).
 *  @param stackId         The stack doing this scan request
*/
GP_API void gpMacDispatcher_ScanRequest(gpMacCore_ScanType_t scanType, UInt32 scanChannels, UInt8 scanDuration , UInt8 resultListSize, UInt8* pResultList, gpMacCore_StackId_t stackId);

/** @ingroup MANAGEMENT
 * @brief This function is issued by a device who wishes to join a network.
 *
 *  This function starts the associate procedure. It is used to join a network. When called, the AssociateRequest function will construct an associate request command packet, and send it to the coordinator.
 *
 *  @param logicalChannel           The channel that will be used to do the association attempt
 *  @param pCoordAddrInfo           The coordinator addressing info (address mode, panId, address)
 *  @param capabilityInformation    Specifies the operational capabilities of the associating device.
 *  @param stackId                  The stack doing this associate request
*/
GP_API void gpMacDispatcher_AssociateRequest(UInt8 logicalChannel, gpMacCore_AddressInfo_t* pCoordAddrInfo, UInt8 capabilityInformation, gpMacCore_StackId_t stackId);
/** @ingroup MANAGEMENT
 * @brief This function is used to send an associate response command packet to a device that is trying to join. The associate response command packet contains the results of the join procedure.
 *
 *  This function is used to respond to a previously received associate request command packet. Upon reception of an associate request command packet, the AssociateIndication function is called to inform the higher layer. The next higher layer then decides to allow the joining device in the network or not. Its decision is sent back to the joining device through the associate response packet.
 *
 *  @param pDeviceAddress           A pointer to the address of the device that is trying to join.
 *  @param associateShortAddress    If the joining device is allowed (successful association), this parameter contains the short address that was allocated for the device. If the association was unsuccessful, this parameter is set to 0xffff.
 *  @param status                   This parameter contains the result of the association attempt. Possible values are:
 *                                  0x0: Association successful.
 *                                  0x1: PAN at capacity.
 *                                  0x2: PAN access denied.
 *  @param stackId                  The stack doing this associate response
*/
GP_API void gpMacDispatcher_AssociateResponse(MACAddress_t *pDeviceAddress, UInt16 associateShortAddress, gpMacCore_Result_t status, gpMacCore_StackId_t stackId);

/** @ingroup DATA
 * @brief This function is used to poll a coordinator for data by sending a data request command. If the coordinator has data pending for the device that issued the poll request, it will forward the data.
 *
 *  This function starts a poll. This function is used to request packets that are pending at the coordinator (by sending a data request command to the coordinator).
 *  If the coordinator has data pending for the device that issued the poll request, it will forward the data.
 *  If a short address is allocated to the device, and it was written into the gpMacCore_AttributeShortAddress PIB attribute,
 *  the short address will be used as source address of the data request command.
 *  If the gpMacCore_AttributeShortAddress PIB attribute is set to 0xffff or 0xfffe, the device will use its extended address as source address in the data request command packet.
 *
 *  @param pCoordAddrInfo        All the details regarding the coordinator
 *  @param pSecOptions           structure with security options, if NULL no security is used.
 *  @param stackId               The stack doing this poll request
*/
GP_API void gpMacDispatcher_PollRequest( gpMacCore_AddressInfo_t* pCoordAddrInfo, gpMacCore_Security_t *pSecOptions, gpMacCore_StackId_t stackId);

/** @ingroup DATA
 * @brief This function will remove the specified transmit request form the indirect transmission queue
 */
GP_API void gpMacDispatcher_PurgeRequest(gpPd_Handle_t pdHandle, gpMacCore_StackId_t stackId);

/** @ingroup MANAGEMENT
 * @brief This function is used to send an orphan response command packet.
 */
GP_API void gpMacDispatcher_OrphanResponse(MACAddress_t* pOrphanAddress, UInt16 shortAddress, Bool associatedMember, gpMacCore_StackId_t stackId);

/** @brief This function is used to enable beacon transmission
*/
GP_API gpMacCore_Result_t gpMacDispatcher_Start(gpMacCore_PanId_t panId, UInt8 logicalChannel, Bool panCoordinator, UInt8 stackId);

// getters and setters
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetCurrentChannel(UInt8 channel, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetMultipleChannelTable(UInt8* pChannel, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetCurrentChannel(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetDefaultTransmitPowers(Int8* pDefaultTransmitPowerTable);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetTransmitPower(gpMacCore_TxPower_t transmitPower, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Int8 gpMacDispatcher_GetTransmitPower(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetCCAMode(UInt8 cCAMode, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetCCAMode(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_GetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress , gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetCoordShortAddress(UInt16 addr, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt16 gpMacDispatcher_GetCoordShortAddress(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetPanCoordinator(Bool panCoordinator, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_GetPanCoordinator(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetDsn(UInt8 dsn, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetDsn(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetMaxCsmaBackoffs(UInt8 maxCsmaBackoffs, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetMaxCsmaBackoffs(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetMinBE(UInt8 minBE, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetMinBE(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetMaxBE(UInt8 maxBE, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetMaxBE(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetCsmaMode(UInt8 csmaMode, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetCsmaMode(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetPanId(UInt16 panId, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt16 gpMacDispatcher_GetPanId(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetRxOnWhenIdle(Bool rxOnWhenIdle, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_GetRxOnWhenIdle(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetShortAddress(UInt16 shortAddress, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt16 gpMacDispatcher_GetShortAddress(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetAssociationPermit(Bool associationPermit, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_GetAssociationPermit(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetBeaconPayload(UInt8* pBeaconPayload, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_GetBeaconPayload(UInt8* pBeaconPayload, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetBeaconPayloadLength(UInt8 length, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetBeaconPayloadLength(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetPromiscuousMode(UInt8 promiscuousMode, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetPromiscuousMode(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetTransactionPersistenceTime(UInt16 time, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt16 gpMacDispatcher_GetTransactionPersistenceTime(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetExtendedAddress(MACAddress_t* pExtendedAddress, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_GetExtendedAddress(MACAddress_t* pExtendedAddress, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetNumberOfRetries(UInt8 numberOfRetries, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetNumberOfRetries(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetSecurityEnabled(Bool securityEnabled, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_GetSecurityEnabled(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetBeaconStarted(Bool BeaconStarted, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_GetBeaconStarted(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetTxAntenna(UInt8 txAntenna, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt8 gpMacDispatcher_GetTxAntenna(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetIndicateBeaconNotifications(Bool enable, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_GetIndicateBeaconNotifications(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetForwardPollIndications(Bool enable, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_GetForwardPollIndications(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetLqiThresholdForTest(UInt8 lqiThreshold, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetAddressModeOverrideForBeacons(UInt8 addressMode, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_SetMacVersion(gpMacCore_MacVersion_t macVersion , gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_MacVersion_t gpMacDispatcher_GetMacVersion(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t gpMacDispatcher_DataPending_QueueAdd(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId) ;
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t gpMacDispatcher_DataPending_QueueRemove(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId) ;
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t gpMacDispatcher_DataPending_QueueClear(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_AddNeighbour   (gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API Bool gpMacDispatcher_RemoveNeighbour(gpMacCore_AddressInfo_t *pAddrInfo, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void gpMacDispatcher_ClearNeighbours(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t gpMacDispatcher_SetDataPendingMode(gpMacCore_DataPendingMode_t dataPendingMode, gpMacCore_StackId_t stackId);

/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_SetFrameCounter(UInt32 frameCounter, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt32                                gpMacDispatcher_GetFrameCounter(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t                    gpMacDispatcher_SetKeyDescriptor(gpMacCore_KeyDescriptor_t *pKeyDescriptor, gpMacCore_Index_t index, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t                    gpMacDispatcher_GetKeyDescriptor(gpMacCore_KeyDescriptor_t *pKeyDescriptor, gpMacCore_Index_t index, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_SetKeyTableEntries(gpMacCore_KeyTablesEntries_t keyTableEntries, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_KeyTablesEntries_t          gpMacDispatcher_GetKeyTableEntries(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t                    gpMacDispatcher_SetDeviceDescriptor(gpMacCore_DeviceDescriptor_t *pDeviceDescriptor, gpMacCore_Index_t index, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t                    gpMacDispatcher_GetDeviceDescriptor(gpMacCore_DeviceDescriptor_t * pDeviceDescriptor , gpMacCore_Index_t index, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_SetDeviceTableEntries(gpMacCore_DeviceTablesEntries_t deviceTableEntries, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_DeviceTablesEntries_t       gpMacDispatcher_GetDeviceTableEntries(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t                    gpMacDispatcher_SetSecurityLevelDescriptor(gpMacCore_SecurityLevelDescriptor_t* pSecurityLevelDescriptor , gpMacCore_Index_t index, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_Result_t                    gpMacDispatcher_GetSecurityLevelDescriptor(gpMacCore_SecurityLevelDescriptor_t *pSecurityLevelDescriptor , gpMacCore_Index_t index, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API gpMacCore_SecurityLevelTableEntries_t gpMacDispatcher_GetSecurityLevelTableEntries(gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_SetSecurityLevelTableEntries(gpMacCore_SecurityLevelTableEntries_t securityLevelTableEntries, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_SetDefaultKeySource(UInt8 *pDefaultKeySource, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_GetDefaultKeySource(UInt8 *pDefaultKeySource, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_SetPanCoordExtendedAddress(MACAddress_t *pPanCoordExtendedAddress, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_GetPanCoordExtendedAddress(MACAddress_t *pPanCoordExtendedAddress, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API void                                  gpMacDispatcher_SetPanCoordShortAddress(UInt16 PanCoordShortAddress, gpMacCore_StackId_t stackId);
/** @ingroup ATTRIBUTE
 * @brief Access method for the corresponding PIB attribute
 */
GP_API UInt16                                gpMacDispatcher_GetPanCoordShortAddress(gpMacCore_StackId_t stackId);

/** @brief Set Regional Domain Settings (RDS) for the specified stack.
 *
 *  The RDS values consist of a list of blocked channels and a table of maximum power levels for non-blocked channels.
 *  Any attempt to transmit/listen on a blocked channel will fail with gpMacCore_ResultInvalidParameter.
 *  Transmit power levels above maximum will be clipped to the maximum level.
 *
 *  @param blockedChannelMask     Bitmask of blocked channels (IEEE numbers). E.g. set bit 11 to block channel 11.
 *  @param pMaxTransmitPowerTable Pointer to 16-byte table of maximum power levels for channels 11 .. 26.
 *  @param stackId                Apply RDS values to the specified stack. Each stack maintains separate RDS values.
 */
GP_API void gpMacDispatcher_SetRegionalDomainSettings(UInt32 blockedChannelMask, gpMacCore_TxPower_t* pMaxTransmitPowerTable, gpMacDispatcher_StackId_t stackId);

/** @brief Get Regional Domain Settings (RDS) for the specified stack.
 *
 *  The RDS values consist of a list of blocked channels and a table of maximum power levels for non-blocked channels.
 *
 *  @param pBlockedChannelMask    Bitmask of blocked channels (IEEE numbers). E.g. set bit 11 to block channel 11.
 *  @param pMaxTransmitPowerTable Pointer to 16-byte table of maximum power levels for channels 11 .. 26.
 *  @param stackId                Get RDS values from the specified stack. Each stack maintains separate RDS values.
 */
GP_API void gpMacDispatcher_GetRegionalDomainSettings(UInt32* pBlockedChannelMask, gpMacCore_TxPower_t* pMaxTransmitPowerTable, gpMacDispatcher_StackId_t stackId);

/** @ingroup ATTRIBUTE
 *
 *  @brief Enable or disable automatic toggling of TX antenna after transmission failure.
 *
 *  @param enable                 True to enable auto-toggling, false to disable.
 *  @param stackId                Stack identifier.
 */

/** @brief This function enables the Enhanced FramePending behavior for a specific stack.
 *
 *  @param enableEnhancedFramePending   Enables or disables the Enhanced FramePending behavior for a specific stack.
*/
void gpMacDispatcher_EnableEnhancedFramePending(Bool enableEnhancedFramePending, gpMacCore_StackId_t stackId);

#ifdef GP_MACCORE_DIVERSITY_RAW_FRAMES
/** @brief This function enables the raw mode of the stack.
 *
 * This mode allows the upper stack to create the MAC header while transmitting a packets,
 * and to stop the MacCore from processing MAC headers, and instead passing them to the upper stack.
 *
 *  @param rawModeEnabled   Enables or disables the raw mode for a specific stack.
*/
void gpMacDispatcher_SetStackInRawMode(Bool rawModeEnabled, gpMacCore_StackId_t stackId);

/** @brief This function returns if the raw mode is enabled for a specific stack.
 *
 *  @return rawModeEnabled   Indicates if raw mode is enabled or disabled for a specific stack.
*/
Bool gpMacDispatcher_GetStackInRawMode(gpMacCore_StackId_t stackId);

/** @brief This function sets the encryption key and keyIndex to be used for the Tread raw encryption.
 *
 *  @param encryptionKeyIdMode    Key ID mode. Only mode '1' is supported for Thread raw encryption.
 *  @param encryptionKeyId        Current key index.
 *  @param pCurrKey               Current key.
 *  @param stackId                Stack identifier.
*/
void gpMacDispatcher_SetRawModeEncryptionKeys(gpMacCore_KeyIdMode_t encryptionKeyIdMode, gpMacCore_KeyIndex_t encryptionKeyId, UInt8* pCurrKey, gpMacCore_StackId_t stackId);


/** @brief This function sets the fields used in the Nonce for the Thread raw encryption.
 *
 *  @param frameCounter           Initial value of the frame pointer. Will be auto-incremented by the lower layers.
 *  @param pExtendedAddress       Pointer to the local(source) extended address.
 *  @param seclevel               Security Level. Needs to be '5' for Thread raw encryption.
 *  @param stackId                Stack identifier.
*/
void gpMacDispatcher_SetRawModeNonceFields(UInt32 frameCounter, MACAddress_t* pExtendedAddress , UInt8 seclevel, gpMacCore_StackId_t stackId);

/** @brief This function configures the insertion of a VS IE header in the Enh Ack frames containing probing data.
 *
 *  @param linkMetrics      Bitmask of link metrics which should be reported.
 *  @param pExtendedAddress The extended address of the probing initiator.
 *  @param shortAddress     The short address of the probing initiator.
 *  @param stackId          The stack id.
 *  @return result          Success if the parameter are accepted, InvalidParameters otherwise.
*/
gpMacCore_Result_t gpMacDispatcher_ConfigureEnhAckProbing(UInt8 linkMetrics, MACAddress_t* pExtendedAddress , UInt16 shortAddress, gpMacCore_StackId_t stackId);

#endif //GP_MACCORE_DIVERSITY_RAW_FRAMES

#define gpMacDispatcher_SetAutoTxAntennaToggling(Bool, gpMacDispatcher_StackId_t)
/** @ingroup ATTRIBUTE
 *
 *  @brief Return the current state of automatic toggling of TX antenna.
 *
 *  @param stackId                Stack identifier.
 *  @return true when enabled, false when disabled.
 */
GP_API Bool gpMacDispatcher_GetAutoTxAntennaToggling(gpMacDispatcher_StackId_t stackId);


/** @brief Enables or disables the Retransmits on CCA failures.
 * 
 * This function enables or disables the Retransmits on CCA failures. The actual amount of retransmits 
 * is set through gpMacDispatcher_SetNumberOfRetries. As such this function only changes the 
 * default MAC 802.15.4-2015 behavior to also retransmit on a CCA fail.
 * 
 *  @param enable          Enables or disables the Retransmits.
 *  @param stackId         The identifier of the stack doing this
*/
GP_API void gpMacDispatcher_SetRetransmitOnCcaFail(Bool enable, gpMacCore_StackId_t stackId);

/** @brief returns if the feature "Retransmits on CCA fail" is enabled for a stackId.
 * 
 * This function returns if the feature "Retransmits on CCA fail" is enabled for a stackId.
 * 
 *  @param stackId         The identifier of the stack doing this
*/
GP_API Bool gpMacDispatcher_GetRetransmitOnCcaFail(gpMacCore_StackId_t stackId);

/** @brief Enables or disables the random delays between subsequent retransmits.
 * 
 * This function enables or disables the random delays between subsequent retransmits. 
 * The actual amount of retransmits is set through gpMacDispatcher_SetNumberOfRetries. As such this 
 * function only changes the default MAC 802.15.4-2015 behavior to add random delays between retransmits.
 * 
 *  @param enable          Enables or disables the random delays between subsequent retransmits.
 *  @param stackId         The identifier of the stack doing this
*/
GP_API void gpMacDispatcher_SetRetransmitRandomBackoff(Bool enable, gpMacCore_StackId_t stackId);

/** @brief returns if the feature "random delays between subsequent retransmits" is enabled for a stackId.
 * 
 * This function returns if the feature "random delays between subsequent retransmits" is enabled for a stackId.
 * 
 *  @param stackId         The identifier of the stack doing this
*/
GP_API Bool gpMacDispatcher_GetRetransmitRandomBackoff(gpMacCore_StackId_t stackId);

/** @brief This function configures the minimum backoff exponent for random delays between retransmits.
 * 
 * This function configures the minimum backoff exponent for random delays between retransmits.
 * Not to be confused with the backoff exponent for random delays between CCA failures!
 * 
 *  @param minBERetransmit The minimum backoff exponent.
 *  @param stackId         The identifier of the stack
*/
GP_API void gpMacDispatcher_SetMinBeRetransmit(UInt8 minBERetransmit, gpMacCore_StackId_t stackId);

/** @brief This function returns the minimum backoff exponent for random delays between retransmits.
 * 
 * This function returns the minimum backoff exponent for random delays between retransmits.
 * 
 *  @param stackId         The identifier of the stack
*/
GP_API UInt8 gpMacDispatcher_GetMinBeRetransmit(gpMacCore_StackId_t stackId);

/** @brief This function configures the maximum backoff exponent for random delays between retransmits.
 * 
 * This function configures the maximum backoff exponent for random delays between retransmits.
 * Not to be confused with the backoff exponent for random delays between CCA failures!
 * 
 *  @param maxBERetransmit The maximum backoff exponent.
 *  @param stackId         The identifier of the stack
*/
GP_API void gpMacDispatcher_SetMaxBeRetransmit(UInt8 maxBERetransmit, gpMacCore_StackId_t stackId);

/** @brief This function returns the maximum backoff exponent for random delays between retransmits.
 * 
 * This function returns the maximum backoff exponent for random delays between retransmits.
 * 
 *  @param stackId         The identifier of the stack
*/
GP_API UInt8 gpMacDispatcher_GetMaxBeRetransmit(gpMacCore_StackId_t stackId);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif

#endif // _GP_MACDISPATCHER_MULTISTACK_H_

