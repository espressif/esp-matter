/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by\
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

//#define GP_LOCAL_LOG

#define GP_COMPONENT_ID GP_COMPONENT_ID_BLEADVERTISER

#include "gpBle_Manual.h"
#include "gpBleComps.h"
#include "gpBleAddressResolver.h"
#include "gpBle_defs.h"
#include "gpBleActivityManager.h"
#include "gpAssert.h"
#include "gpPd.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpHal.h"
#include "gpHal_Ble_Manual.h"
#include "gpBleConfig.h"
#include "gpBleAdvertiser.h"
#include "gpBleAdvertiser_defs.h"

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
#include "gpBleLlcp.h"
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Defines whether we should consider advertising as a single shot or a repeating activity. This is important for scheduling.
#ifdef GP_DIVERSITY_BLE_ADVERTISER_SINGLE_EVENT_SCHEDULING
// Special case where we should consider advertisings as a single shot activity
#define BLE_ADVERTISING_SINGLE_EVENT_TRIGGER    1
#else
// Regular advertising (repeating activity)
#define BLE_ADVERTISING_SINGLE_EVENT_TRIGGER    0
#endif //GP_DIVERSITY_BLE_ADVERTISER_SINGLE_EVENT_SCHEDULING

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

// All context for the advertiser service
static Ble_AdvertisingAttributes_t Ble_AdvertisingAttributes;

static Ble_Priority_t BleAdvertiser_EventPriority;
static Ble_Priority_t BleHdcAdvertiser_EventPriority; // High Duty Cycle

gpBleAdvertiser_GlobalVars_t gpBleAdvertiser_GlobalVars;

/*****************************************************************************
 *                    Tmp extern Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

// Packet construction
static gpHci_Result_t Ble_ConstructAdvertisingPacket(gpPd_Loh_t* pPdLoh, Ble_AdvertisingPduType_t pduType);
static void Ble_WritePduHeader(gpPd_Loh_t* pdLoh, Ble_AdvertisingPduType_t pduType);
static void Ble_WritePduPayloadAdvertising(gpPd_Loh_t* pdLoh, Ble_AdvertisingPduType_t pduType);
static void Ble_WriteAdvertisingAddress(gpPd_Loh_t* pdLoh);
#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
static void Ble_WriteInitAddress(gpPd_Loh_t* pdLoh);
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

static void Ble_PopulateAdvEventInfo(gpHal_AdvEventInfo_t* pEventInfo, gpPd_Loh_t pdLohAdv, gpPd_Loh_t pdLohScan);
static INLINE Bool Ble_GetRxAdd(Ble_AdvertisingPduType_t pduType);

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
static void Ble_RestartAdvEvent(void);
static Bool Ble_IsConnReqPduAcceptable(Ble_ConnReqPdu_t* pConnReq);
static void Ble_ProcessSlaveCreateConn(gpHal_BleSlaveCreateConnInfo_t *info, Ble_ConnEstablishParams_t *pConnEstablishParams);
static void Ble_FreeSlaveConnection(Bool sendConnectionComplete, Ble_ConnEstablishParams_t* pConnEstablishParams);
// HAL callbacks
static void Ble_cbSlaveCreateConn(gpHal_BleSlaveCreateConnInfo_t *info);
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

//introduce a static vairant of Ble_SetAdvertisingParametersChecker to allow conditional checking of the ExtendedAdvertising
// - required only internally in this file
// - changing signature of original method would introduce incompatibility with existgin ROM images
// - original method maps on this _Extended method with checkExtAdv == true
static gpHci_Result_t Ble_SetAdvertisingParametersChecker_Extended(gpHci_LeSetAdvertisingParametersCommand_t* params, Bool checkExtAdv)
{
    if(Ble_AdvertisingAttributes.advertisingEnabled)
    {
        GP_LOG_PRINTF("Advertising enabled",0);
        return gpHci_ResultCommandDisallowed;
    }

    if(!BLE_ADVERTISING_TYPE_VALID(params->advertisingType))
    {
        GP_LOG_PRINTF("Advertising type invalid: %x",0,params->advertisingType);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    if(!BLE_OWN_ADDRESS_TYPE_VALID(params->ownAddressType))
    {
        GP_LOG_PRINTF("Own address type invalid: %x",0, params->ownAddressType);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    if(!BLE_OWN_ADDRESS_TYPE_SUPPORTED(params->ownAddressType))
    {
        GP_LOG_PRINTF("Own address type unsupported: %x",0, params->ownAddressType);
        return gpHci_ResultUnsupportedFeatureOrParameterValue;
    }

    if(GP_HCI_IS_DIRECTED_ADV_TYPE(params->advertisingType))
    {
        if(!BLE_ADV_PEER_ADDRESS_TYPE_VALID(params->peerAddressType))
        {
            GP_LOG_PRINTF("Peer address type invalid: %x",0,params->peerAddressType);
            return gpHci_ResultInvalidHCICommandParameters;
        }
    }

    // Ignore interval parameters in case of high duty cycle directed advertising
    if(!GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(params->advertisingType))
    {
        UInt16 lowerBoundary = BLE_ADVERTISING_INTERVAL_RANGE_NORMAL_MIN;
        UInt16 upperBoundary = BLE_ADVERTISING_INTERVAL_RANGE_NORMAL_MAX;

#if GP_DIVERSITY_BLECONFIG_VERSION_ID <= gpBleConfig_BleVersionId_4_2
        /* SDP004-383: Special advertising interval is only needed for pre 5.0 controllers */
        if(GP_HCI_IS_ADV_TYPE_SPECIAL_ADV_INT(params->advertisingType))
        {
            lowerBoundary = BLE_ADVERTISING_INTERVAL_RANGE_SPECIAL_MIN;
        }
#endif // GP_DIVERSITY_BLECONFIG_VERSION_ID

        if(params->advertisingIntervalMin > params->advertisingIntervalMax)
        {
            GP_LOG_PRINTF("Adv min > max: %x > %x",0,params->advertisingIntervalMin, params->advertisingIntervalMax);
            return gpHci_ResultInvalidHCICommandParameters;
        }

        if(!BLE_RANGE_CHECK(params->advertisingIntervalMin,lowerBoundary,upperBoundary))
        {
            GP_LOG_PRINTF("Adv min not in range: %x <= %x <= %x",0, lowerBoundary, params->advertisingIntervalMin, upperBoundary);
            return gpHci_ResultInvalidHCICommandParameters;
        }

        if(!BLE_RANGE_CHECK(params->advertisingIntervalMax,lowerBoundary,upperBoundary))
        {
            GP_LOG_PRINTF("Adv max not in range %x <= %x <= %x",0, lowerBoundary, params->advertisingIntervalMax, upperBoundary);
            return gpHci_ResultInvalidHCICommandParameters;
        }
    }

    if(!BLE_ADV_CHANNEL_MAP_VALID(params->channelMap))
    {
        GP_LOG_PRINTF("Channel map invalid: %x",0, params->channelMap);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    if(!BLE_ADV_FILTER_POLICY_VALID(params->filterPolicy))
    {
        GP_LOG_PRINTF("Filter policy invalid: %x",0, params->filterPolicy);
        return gpHci_ResultInvalidHCICommandParameters;
    }

    return gpHci_ResultSuccess;
}

gpHci_Result_t Ble_SetAdvertisingParametersChecker(gpHci_LeSetAdvertisingParametersCommand_t* params)
{
    return Ble_SetAdvertisingParametersChecker_Extended(params, true);
}

gpHci_Result_t Ble_ConstructAdvertisingPacket(gpPd_Loh_t* pPdLoh, Ble_AdvertisingPduType_t pduType)
{
    gpHci_Result_t result;
    UInt32 accessAddress = BLE_ACCESS_ADDRESS_ADVERTISING_CHANNEL_PACKET;

    result = Ble_RMGetResource(pPdLoh);

    if(result != gpHci_ResultSuccess)
    {
        return result;
    }

    // Write pdu payload
    Ble_WritePduPayloadAdvertising(pPdLoh, pduType);
    // Write pdu header
    Ble_WritePduHeader(pPdLoh, pduType);
    // Add Access Address
    gpPd_PrependWithUpdate(pPdLoh, sizeof(accessAddress), (UInt8*)&accessAddress);
    // preamble added by hal
    return gpHci_ResultSuccess;
}

void Ble_WritePduHeader(gpPd_Loh_t* pdLoh, Ble_AdvertisingPduType_t pduType)
{
    UInt16 header = 0;
    UInt8 length = 0;

    // Calculate length
    switch(pduType)
    {
        case Ble_AdvertisingPduType_ConnectableUndirected:
        case Ble_AdvertisingPduType_NonConnectableUndirected:
        case Ble_AdvertisingPduType_ScannableUndirected:
        {
            length = GP_HAL_BT_DEVICE_ADDRESS_LENGTH + Ble_AdvertisingAttributes.advDataLength;
            break;
        }
        case Ble_AdvertisingPduType_ConnectableDirected:
        {
            length = GP_HAL_BT_DEVICE_ADDRESS_LENGTH + GP_HAL_BT_DEVICE_ADDRESS_LENGTH;
            break;
        }
        case Ble_AdvertisingPduType_ScanResponse:
        {
            length = GP_HAL_BT_DEVICE_ADDRESS_LENGTH + Ble_AdvertisingAttributes.scanRespDataLength;
            break;
        }
        default:
        {
            GP_ASSERT_DEV_INT(false);       // should not happen
            break;
        }
    }

    //PDU type
    BLE_ADV_PDU_HEADER_TYPE_SET(header, pduType);

    // ChSel

    // Tx add
    {
        BLE_ADV_PDU_HEADER_TXADD_SET(header, BLE_GET_TX_ADD_FROM_OWN_ADDRESS_TYPE(Ble_AdvertisingAttributes.advParams.ownAddressType));
    }
    // Rx add
    BLE_ADV_PDU_HEADER_RXADD_SET(header, Ble_GetRxAdd(pduType));
    // Length
    BLE_ADV_PDU_HEADER_LENGTH_SET(header, length);

    // write header in pd
    gpPd_PrependWithUpdate(pdLoh, 2, (UInt8*)&header);
}

void Ble_WritePduPayloadAdvertising(gpPd_Loh_t* pdLoh, Ble_AdvertisingPduType_t pduType)
{
    if(BLE_IS_ADV_PDU_TYPE_UNDIRECTED(pduType))
    {
        // Add AdvData
        if(Ble_AdvertisingAttributes.advDataLength > 0)
        {
            gpPd_PrependWithUpdate(pdLoh, Ble_AdvertisingAttributes.advDataLength, Ble_AdvertisingAttributes.advData);
        }
    }
#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
    else if(BLE_IS_ADV_PDU_TYPE_DIRECTED(pduType))
    {
        // Add InitA
        Ble_WriteInitAddress(pdLoh);
    }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE
    else if(BLE_IS_SCAN_PDU_TYPE_RESP(pduType))
    {
        // Add ScanRespData
        if(Ble_AdvertisingAttributes.scanRespDataLength > 0)
        {
            gpPd_PrependWithUpdate(pdLoh, Ble_AdvertisingAttributes.scanRespDataLength, Ble_AdvertisingAttributes.scanRespData);
        }
    }
    else
    {
        GP_ASSERT_DEV_INT(false);   // should not happen
    }

    // Add AdvA
     Ble_WriteAdvertisingAddress(pdLoh);
}

void Ble_WriteAdvertisingAddress(gpPd_Loh_t* pdLoh)
{
    // Add AdvA
    {
        BLE_WRITE_ADDRESS(pdLoh, Ble_AdvertisingAttributes.ownAddress.addr);
        GP_LOG_PRINTF_ADDRESS("AdvA", Ble_AdvertisingAttributes.ownAddress);
    }
}

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
void Ble_WriteInitAddress(gpPd_Loh_t* pdLoh)
{
    GP_ASSERT_DEV_INT(GP_HCI_IS_DIRECTED_ADV_TYPE(Ble_AdvertisingAttributes.advParams.advertisingType));

    // Add initiator address
    {
        BLE_WRITE_ADDRESS(pdLoh, Ble_AdvertisingAttributes.advParams.peerAddress.addr);
        GP_LOG_PRINTF_ADDRESS("InitRx", Ble_AdvertisingAttributes.advParams.peerAddress);
    }
}
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE


// introduce a static variant of Ble_SetAdvertiseEnable to allow conditional checking of the parameters
// - required only internally in this file
// - changing signature of original method will introduce incompatibility with existgin ROM images
// - original method maps on this _Extended method with checkParams == true
static gpHci_Result_t Ble_SetAdvertiseEnable_Extended(Bool checkParams)
{
    gpHci_Result_t result=gpHci_ResultSuccess;
    gpHal_Result_t advResult;
    gpBleActivityManager_IntervalMinMax_t advIntParams;
    gpHal_AdvEventInfo_t advEventInfo;
    Ble_AdvertisingPduType_t pduType;
    gpPd_Loh_t pdLoh_Adv = {0, GP_BLE_ADV_CHANNEL_PDU_MAX_OFFSET, GP_PD_INVALID_HANDLE};
    gpPd_Loh_t pdLoh_ScanResp = {0, GP_BLE_ADV_CHANNEL_PDU_MAX_OFFSET, GP_PD_INVALID_HANDLE};
    Bool highDutyDirected;

    pduType = Ble_ConvertAdvTypeToPduType(Ble_AdvertisingAttributes.advParams.advertisingType);

    // check if advertising context is valid
    if(checkParams)
    {
        result = Ble_SetAdvertisingParametersChecker(&Ble_AdvertisingAttributes.advParams);

        /* fix for EBQ test - HCI/DDI/BI-06C */
        if ((gpHci_OwnAddressType_RandomDevice == BLE_OWN_ADDR_TYPE_TO_ADVPEER_ADDR_TYPE(Ble_AdvertisingAttributes.advParams.ownAddressType)) &&
            (!gpBleConfig_HasRandomAddress()))
        {
            /* In this case it means the random address was not set. */
            GP_LOG_PRINTF("Random address not set", 0);
            return gpHci_ResultInvalidHCICommandParameters;
        }
    }

    if(result != gpHci_ResultSuccess)
    {
        GP_LOG_PRINTF("Invalid adv params",0);
        return result;
    }

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
    if(BLE_IS_ADV_PDU_TYPE_DIRECTED(pduType))
    {
        // Early check: reject if we already have a connection with this peer address
        if(Ble_IsConnectionActive(&Ble_AdvertisingAttributes.advParams.peerAddress))
        {
            GP_LOG_PRINTF("Already a connection with device:",0);
            Ble_DumpAddress(&Ble_AdvertisingAttributes.advParams.peerAddress);
            return gpHci_ResultConnectionAlreadyExists;
        }
    }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

    /* Consider moving "ownAddress" to stack for memory optimization */
    result = gpBleConfig_GetOwnAddress(&Ble_AdvertisingAttributes.ownAddress, Ble_AdvertisingAttributes.advParams.ownAddressType);

    if(result != gpHci_ResultSuccess)
    {
        GP_LOG_PRINTF("Get own address failed with result 0x%x",0, result);
        return result;
    }

    highDutyDirected = GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(Ble_AdvertisingAttributes.advParams.advertisingType);

    if(highDutyDirected)
    {
        // high duty cycle directed advertising ==> T < 3.75 msec (ignore HCI interval)
        Ble_AdvertisingAttributes.advParams.advertisingIntervalMin = BLE_HIGH_DUTY_CYCLE_INTERVAL_MAX_UNIT; /* JIRA SDP004-1445 */
        Ble_AdvertisingAttributes.advParams.advertisingIntervalMax = BLE_HIGH_DUTY_CYCLE_INTERVAL_MAX_UNIT;
    }

    advIntParams.intervalMin = Ble_AdvertisingAttributes.advParams.advertisingIntervalMin;
    advIntParams.intervalMax = Ble_AdvertisingAttributes.advParams.advertisingIntervalMax;

    // request suitable advertising interval to NRT BLE scheduler
    result = gpBleActivityManager_StartAdvertising(&advIntParams, &Ble_AdvertisingAttributes.advTimeParams, highDutyDirected, BLE_ADVERTISING_SINGLE_EVENT_TRIGGER);

    if(result != gpHci_ResultSuccess)
    {
        GP_LOG_PRINTF("Adv interval could not be allocated",0);
        return result;
    }

    GP_LOG_PRINTF("adv interval is %lu us (range: [%lu-%lu])",0,
        (unsigned long)BLE_TIME_UNIT_625_TO_US(Ble_AdvertisingAttributes.advTimeParams.interval),
        (unsigned long)BLE_TIME_UNIT_625_TO_US(advIntParams.intervalMin),
        (unsigned long)BLE_TIME_UNIT_625_TO_US(advIntParams.intervalMax));
    GP_LOG_PRINTF("adv interval is %u units (range: [%u-%u])",0,
        (Ble_AdvertisingAttributes.advTimeParams.interval),
        (advIntParams.intervalMin),(advIntParams.intervalMax));

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
    // Obtain a new_slave_connection_handle from the LLCP Service in case of connectable advertising
    if(BLE_IS_ADV_PDU_TYPE_CONNECTABLE(pduType))
    {
        result = gpBleLlcp_AllocateConnectionSlave(&Ble_AdvertisingAttributes.advParams.peerAddress, Ble_AdvertisingAttributes.advParams.peerAddressType, &Ble_AdvertisingAttributes.slaveConnId);

        if(!BLE_IS_INT_CONN_HANDLE_VALID(Ble_AdvertisingAttributes.slaveConnId))
        {
            GP_LOG_PRINTF("Could not allocate valid conn handle",0);
            goto Ble_SetAdvertiseEnable_failAllocateConnection;
        }
    }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

    /*consider  moving "rpaInfo" to stack for memory optimization*/

    result = Ble_ConstructAdvertisingPacket(&pdLoh_Adv, pduType);

    if(result != gpHci_ResultSuccess)
    {
        GP_LOG_PRINTF("Pd construction adv failed",0);
#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
        goto Ble_SetAdvertiseEnable_failConstructAdvPd;
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE
    }

    // Construct scan response pd in case of scannable advertising
    if(BLE_IS_ADV_PDU_TYPE_SCANNABLE(pduType))
    {
        // Update entries in whitelist
        gpBleAddressResolver_UpdateWhiteListEntryStateBulk(GPHAL_ENUM_WHITELIST_ENTRY_ADVERTISING_VALID_MASK, true);

        // prepare a pbm for scan response data
        result = Ble_ConstructAdvertisingPacket(&pdLoh_ScanResp, Ble_AdvertisingPduType_ScanResponse);

        if(result != gpHci_ResultSuccess)
        {
            GP_LOG_PRINTF("Pd construction scan failed",0);
            goto Ble_SetAdvertiseEnable_failStartAdvertising;
        }
    }

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
    // Add peer address to whitelist in case of directed advertising
    if(BLE_IS_ADV_PDU_TYPE_DIRECTED(pduType))
    {
        // Add special whitelist entry
        GP_LOG_PRINTF_ADDRESS("adv wl entry", Ble_AdvertisingAttributes.advParams.peerAddress);
        gpBle_AddDeviceToWL(gpBle_WlEntryAdvertising, Ble_AdvertisingAttributes.advParams.peerAddressType, &Ble_AdvertisingAttributes.advParams.peerAddress);
    }

    if(BLE_IS_ADV_PDU_TYPE_CONNECTABLE(pduType))
    {
        if(Ble_AdvertisingAttributes.advParams.filterPolicy)
        {
            // Update entries in whitelist
            gpBleAddressResolver_UpdateWhiteListEntryStateBulk(GPHAL_ENUM_WHITELIST_ENTRY_ADVERTISING_VALID_MASK, true);
        }

        // Make sure we disable entries we are already connected to
        gpBleAddressResolver_EnableConnectedDevicesInWhiteList(GPHAL_ENUM_WHITELIST_ENTRY_ADVERTISING_VALID_MASK, false);
    }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

    // All info for advertising event info structure is known here ==> fill
    Ble_PopulateAdvEventInfo(&advEventInfo, pdLoh_Adv, pdLoh_ScanResp);

    // Keep the current tx power (this power will be used to transmit the adv channel PDUs)
    Ble_AdvertisingAttributes.advChannelTxPower = gpHal_BleGetTxPower();

    // Start advertising schedule
    advResult = gpHal_BleStartAdvertising(Ble_AdvertisingAttributes.advTimeParams.firstActivityTs, &advEventInfo);

    if(advResult != gpHal_ResultSuccess)
    {
        GP_LOG_PRINTF("gphal start adv failed",0);
        result = gpHci_ResultMemoryCapacityExceeded;
        goto Ble_SetAdvertiseEnable_failStartAdvertising;
    }

    if(GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(Ble_AdvertisingAttributes.advParams.advertisingType))
    {
        // In case of high duty cycle directed advertising, start a supervision timer of 1.28 seconds.
        gpSched_ScheduleEvent(MS_TO_US(BLE_HIGH_DUTY_CYCLE_DIRECTED_ADV_TO_MS), Ble_HighDutyCycleDirectedAdvTimeout);
    }

    // From now on, advertising is enabled
    Ble_AdvertisingAttributes.advertisingEnabled = true;

    return gpHci_ResultSuccess;

    // Cleanup
Ble_SetAdvertiseEnable_failStartAdvertising:
    Ble_FreePdsIfValid(&pdLoh_Adv, &pdLoh_ScanResp);
#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
Ble_SetAdvertiseEnable_failConstructAdvPd:
    if(BLE_IS_ADV_PDU_TYPE_CONNECTABLE(pduType))
    {
        Bool sendConnComplete = GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(Ble_AdvertisingAttributes.advParams.advertisingType);
        Ble_FreeSlaveConnection(sendConnComplete, NULL);
    }
Ble_SetAdvertiseEnable_failAllocateConnection:
    gpBleActivityManager_UnregisterActivity(GPBLEACTIVITYMANAGER_ACTIVITY_ID_ADVERTISING);
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

    return result;
}

gpHci_Result_t Ble_SetAdvertiseEnable(void)
{
    return Ble_SetAdvertiseEnable_Extended(true);
}

gpHci_Result_t Ble_SetAdvertiseDisable(Ble_ConnEstablishParams_t* pConnEstablishParams, Bool sendCommandComplete, Bool sendConnectionComplete)
{
    if (Ble_AdvertisingAttributes.advertisingEnabled)
    {
        Ble_AdvertisingPduType_t pduType;
        gpPd_Loh_t pdLohAdv = {0,0, GP_PD_INVALID_HANDLE};
        gpPd_Loh_t pdLohScan = {0,0, GP_PD_INVALID_HANDLE};

        pduType = Ble_ConvertAdvTypeToPduType(Ble_AdvertisingAttributes.advParams.advertisingType);

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
        if(BLE_IS_ADV_PDU_TYPE_CONNECTABLE(pduType))
        {
            if(Ble_AdvertisingAttributes.slaveConnId == BLE_CONN_HANDLE_INVALID)
            {
                // Strange case: received a 2nd Conn req? In anby case: disable already done - ignore
                return gpHci_ResultSuccess;
            }
        }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

        gpHal_BleStopAdvertising(&pdLohAdv, &pdLohScan);

        Ble_FreePdsIfValid(&pdLohAdv, &pdLohScan);

        // Inform BLE activity manager that advertising is stopped
        gpBleActivityManager_UnregisterActivity(GPBLEACTIVITYMANAGER_ACTIVITY_ID_ADVERTISING);

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
        // In case of directed advertising, update WL
        if(BLE_IS_ADV_PDU_TYPE_DIRECTED(pduType))
        {
            gpBle_RemoveDeviceFromWL(gpBle_WlEntryAdvertising, Ble_AdvertisingAttributes.advParams.peerAddressType, &Ble_AdvertisingAttributes.advParams.peerAddress);
        }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

        if(BLE_IS_ADV_PDU_TYPE_SCANNABLE(pduType)
#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
           || BLE_IS_ADV_PDU_TYPE_CONNECTABLE(pduType)
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE
        )
        {
            // Update entries in whitelist
            gpBleAddressResolver_UpdateWhiteListEntryStateBulk(GPHAL_ENUM_WHITELIST_ENTRY_ADVERTISING_VALID_MASK, false);
        }

        if(GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(Ble_AdvertisingAttributes.advParams.advertisingType))
        {
            // Unschedule the high duty cycle directed adv timeout
            gpSched_UnscheduleEvent(Ble_HighDutyCycleDirectedAdvTimeout);
        }

        // From now on, advertising is disabled
        Ble_AdvertisingAttributes.advertisingEnabled = false;

        if(sendCommandComplete)
        {
            gpHci_CommandCompleteParams_t ccParams;
            MEMSET(&ccParams, 0, sizeof(gpHci_CommandCompleteParams_t));

            ccParams.result = gpHci_ResultSuccess;
            ccParams.numHciCmdPackets = 0x01;
            ccParams.opCode = gpHci_OpCodeLeSetAdvertiseEnable;
            // Send a command complete
            gpBle_SendCommandCompleteEvent(&ccParams);
        }

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
        if(BLE_IS_ADV_PDU_TYPE_CONNECTABLE(pduType))
        {
            // Notify LLCP
            GP_LOG_PRINTF("connect, finish slave: %x",0,Ble_AdvertisingAttributes.slaveConnId);
            Ble_FreeSlaveConnection(sendConnectionComplete, pConnEstablishParams);
        }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE
    }

    return gpHci_ResultSuccess;
}


void Ble_PopulateAdvEventInfo(gpHal_AdvEventInfo_t* pEventInfo, gpPd_Loh_t pdLohAdv, gpPd_Loh_t pdLohScan)
{
    Ble_AdvertisingPduType_t pduType;

    // fixme: should be omitted when we do per member comparison
    MEMSET(pEventInfo, 0, sizeof(gpHal_AdvEventInfo_t));

    // Convert channel map to real channels
    Ble_AdvChannelMapToChannels(Ble_AdvertisingAttributes.advParams.channelMap, pEventInfo->channelMap, BLE_ADV_NUMBER_OF_CHANNELS);

    // Get the pdu type from the advertising type
    pduType = Ble_ConvertAdvTypeToPduType(Ble_AdvertisingAttributes.advParams.advertisingType);

    pEventInfo->interval = BLE_TIME_UNIT_625_TO_US(Ble_AdvertisingAttributes.advTimeParams.interval);

    if(GP_HCI_IS_ADV_TYPE_HIGH_DUTY_CYCLE_DIRECTED(Ble_AdvertisingAttributes.advParams.advertisingType))
    {
        pEventInfo->priority = BleHdcAdvertiser_EventPriority;
        pEventInfo->advDelayMax = BLE_ADV_HIGH_DUTY_DELAY_MAX;
    }
    else
    {
        pEventInfo->priority = BleAdvertiser_EventPriority;
        pEventInfo->advDelayMax = BLE_ADV_MAX_DELAY_IN_UNIT625;
    }

    // Do not use extended priority mechanism for advertising
    pEventInfo->enableExtPriority = 0;

    pEventInfo->pdLohAdv = pdLohAdv;
    pEventInfo->pdLohScan = pdLohScan;

    pEventInfo->frameTypeAcceptMask = 0;

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
    if(BLE_IS_ADV_PDU_TYPE_CONNECTABLE(pduType))
    {
        pEventInfo->frameTypeAcceptMask |= GPHAL_ENUM_SCAN_FRAME_TYPE_ACCEPT_MASK_CONNECT_REQ;
    }
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

    if(BLE_IS_ADV_PDU_TYPE_SCANNABLE(pduType))
    {
        pEventInfo->frameTypeAcceptMask |= GPHAL_ENUM_SCAN_FRAME_TYPE_ACCEPT_MASK_SCAN_REQ;
    }

    pEventInfo->whitelistEnableMask = Ble_GetWhitelistMaskFromAdvParams(Ble_AdvertisingAttributes.advParams.filterPolicy);

}

Bool Ble_GetRxAdd(Ble_AdvertisingPduType_t pduType)
{
    Bool result = false;
    if(pduType == Ble_AdvertisingPduType_ConnectableDirected)
    {
        result = Ble_AdvertisingAttributes.advParams.peerAddressType == gpHci_AdvPeerAddressType_Random ;
    }
    return result;
}

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
void Ble_RestartAdvEvent(void)
{
    UInt32 newAdvTime;
    UInt32 currentTime;

    newAdvTime = Ble_AdvertisingAttributes.advTimeParams.firstActivityTs;
    gpHal_GetTime(&currentTime);

    //The following code may have issues when dealing with clock-wrap
    //possibly already more than a clock-wrap amount of time ago
    // Look into the info structure or request a new one from activity manager
    // as long as we  not passed the current time, advance the adv time by adv int
    while(newAdvTime < currentTime)
    {
        newAdvTime += Ble_AdvertisingAttributes.advTimeParams.interval;
    }

    // Advance it with 1 advertising interval
    newAdvTime += Ble_AdvertisingAttributes.advTimeParams.interval;

    // reschedule it
    gpHal_BleRestartAdvertising(newAdvTime);
}

Bool Ble_IsConnReqPduAcceptable(Ble_ConnReqPdu_t* pConnReq)
{
    /* We do not check the formatting of the access address here. Although the spec dictates some rules for
     * access addres formatting, we chose not to check this for two reasons:
     * 1) this requires quite some extra processing that can introduce a significant delay
     * 2) it won't break any functionality if we are working with an ill-formed access address (as long as it is unique)
     */

    BtDeviceAddress_t *pInitIdentityAddr;

    {
        pInitIdentityAddr = &pConnReq->initAddress;
    }

    // Check if we do not already have a connection with this device
    if(Ble_IsConnectionActive(pInitIdentityAddr))
    {
        GP_LOG_PRINTF("already a connection for this address",0);
        Ble_DumpAddress(&pConnReq->initAddress);
        return false;
    }

    // Check if the access address is unique
    if(Ble_IsAccessAddressUsed(pConnReq->llData.accessAddress))
    {
        GP_LOG_PRINTF("Access address not unique: %lx",0,(unsigned long)pConnReq->llData.accessAddress);
        return false;
    }

    // Perform parameter checks
    if(!Ble_IsAdvPduHeaderValid(&pConnReq->pduHeader))
    {
        return false;
    }

    if(!BLE_RANGE_CHECK(pConnReq->llData.interval,BLE_INITIATOR_CONN_INTERVAL_RANGE_MIN,BLE_INITIATOR_CONN_INTERVAL_RANGE_MAX))
    {
        GP_LOG_PRINTF("Interval field not in range: %x <= %x <= %x",0, BLE_INITIATOR_CONN_INTERVAL_RANGE_MIN, pConnReq->llData.interval, BLE_INITIATOR_CONN_INTERVAL_RANGE_MAX);
        return false;
    }

    if(!Ble_LlcpTxWinOffsetValid(pConnReq->llData.winOffset, pConnReq->llData.interval))
    {
        GP_LOG_PRINTF("Tx win offset (%u) invalid, should be < %u",0, pConnReq->llData.winOffset, pConnReq->llData.interval);
        return false;
    }

    if(!Ble_LlcpTxWinSizeValid(pConnReq->llData.winSize, pConnReq->llData.interval))
    {
        // Correct invalid winSize to max valid winSize
        pConnReq->llData.winSize = BLE_LL_DATA_WIN_SIZE_UPPER;
    }

    if(!BLE_RANGE_CHECK(pConnReq->llData.latency,BLE_INITIATOR_CONN_LATENCY_RANGE_MIN,BLE_INITIATOR_CONN_LATENCY_RANGE_MAX))
    {
        GP_LOG_PRINTF("latency not in range: %x <= %x <= %x",0,BLE_INITIATOR_CONN_LATENCY_RANGE_MIN,pConnReq->llData.latency,BLE_INITIATOR_CONN_LATENCY_RANGE_MAX);
        return false;
    }

    if(!Ble_IsSupervisionToValid(pConnReq->llData.timeout, pConnReq->llData.latency, pConnReq->llData.interval))
    {
        return false;
    }

    if(!BLE_RANGE_CHECK(pConnReq->llData.hopIncrement,BLE_INITIATOR_HOP_FIELD_MIN,BLE_INITIATOR_HOP_FIELD_MAX))
    {
        GP_LOG_PRINTF("hop inc not in range: %x <= %x <= %x",0,BLE_INITIATOR_HOP_FIELD_MIN, pConnReq->llData.hopIncrement, BLE_INITIATOR_HOP_FIELD_MAX);
        return false;
    }

    return true;
}

void Ble_ProcessSlaveCreateConn(gpHal_BleSlaveCreateConnInfo_t *info, Ble_ConnEstablishParams_t *pConnEstablishParams)
{
    Bool acceptPdu;
    Ble_ReadConnReqFromPd(&info->pdLoh, &pConnEstablishParams->connReqPdu);


    acceptPdu = Ble_IsConnReqPduAcceptable(&pConnEstablishParams->connReqPdu);

    if(acceptPdu)
    {
        GP_LOG_PRINTF("Valid conn_req received ",0);
        // fixme: add rx antenna
        pConnEstablishParams->timestamp = gpPd_GetRxTimestamp(info->pdLoh.handle);



        if (Ble_AdvertisingAttributes.advertisingEnabled)
        {
            // legacy advertising mechanism
            pConnEstablishParams->phy = gpHci_PhyWithCoding_1Mb;
            pConnEstablishParams->legacy = true; // we are using legacy adv PDUs -> legacy connection creation
            Ble_SetAdvertiseDisable(pConnEstablishParams, false, true);
        }
        else
        {
            // So we only support the legacy advertising HCI command set.
            // We received an acceptable Conn Req PDU while Ble_AdvertisingAttributes.advertisingEnabled == false
            // This is another "impossible case" - only explicitized via exhaustive logic expressions
            // We want to know IF it ever happens.
            // When it happens we can ignore it: advertising is not ongoing, so there are not resources that need cleaning up. Ignoring the PDU only affects the sender of the PDU.
            GP_ASSERT_DEV_INT(false);
        }
    }
    else
    {
        GP_LOG_PRINTF("Invalid conn_req ==> restart adv",0);
        // Advertising was stopped by RT, but PDU was invalid ==> reschedule

        // Note that advertising (incl legacy) using extended the advertising scheduling mechanisms
        // only gets stopped when the PDU is accepted
        // . . . so no need to restart the extended advertising scheduling mechanism
        if (Ble_AdvertisingAttributes.advertisingEnabled)
        {
            Ble_RestartAdvEvent();
        }
    }
}

void Ble_FreeSlaveConnection(Bool sendConnectionComplete, Ble_ConnEstablishParams_t* pConnEstablishParams)
{
    GP_ASSERT_DEV_INT(Ble_AdvertisingAttributes.slaveConnId != BLE_CONN_HANDLE_INVALID);

    gpBleLlcp_FinishConnectionCreation(Ble_AdvertisingAttributes.slaveConnId, sendConnectionComplete, pConnEstablishParams);

    if(pConnEstablishParams == NULL)
    {
        gpBleLlcp_FreeConnectionSlave(Ble_AdvertisingAttributes.slaveConnId);
    }

    Ble_AdvertisingAttributes.slaveConnId = BLE_CONN_HANDLE_INVALID;
}

// HAL callback for incoming CONNECT_REQ PDU's
void Ble_cbSlaveCreateConn(gpHal_BleSlaveCreateConnInfo_t *info)
{
    Ble_ConnEstablishParams_t slaveParams;
    MEMSET(&slaveParams,0, sizeof(slaveParams));

    {
        // Sanity checks: see if we are in the correct state to receive connect requests
        GP_ASSERT_DEV_INT(Ble_AdvertisingAttributes.advertisingEnabled);
        // Process the packet

        Ble_ProcessSlaveCreateConn(info, &slaveParams);
    }

    Ble_RMFreeResource(0, info->pdLoh.handle);
}
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

void gpBleAdvertiser_Init(gpHal_BleCallbacks_t* pCallbacks)
{
    gpBleAdvertiser_GlobalVars.Ble_AdvertisingAttributes_ptr   = &Ble_AdvertisingAttributes;

    gpHal_EnableSlaveCreateConnInterrupts(true);

    // Add HAL callbacks
#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
    pCallbacks->cbSlaveCreateConn = Ble_cbSlaveCreateConn;
#else
    pCallbacks->cbSlaveCreateConn = NULL;
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

    BleAdvertiser_EventPriority = BLE_NORMAL_ADV_PRIORITY;
    BleHdcAdvertiser_EventPriority = BLE_HIGH_DUTY_ADV_PRIORITY;

}

void gpBleAdvertiser_Reset(Bool firstReset)
{
    GP_LOG_PRINTF("BLE Advertiser reset: %x",0,firstReset);

    if(firstReset)
    {
        Ble_AdvertisingAttributes.advertisingEnabled = false;
    }

    if(Ble_AdvertisingAttributes.advertisingEnabled)
    {
        // Make sure we disable advertising when it is still running
        Ble_SetAdvertiseDisable(NULL, false, false);
//        Ble_AdvertisingAttributes.advertisingEnabled  = false;
    }

    // Set advertising parameters to default values
    Ble_AdvertisingAttributes.advParams.advertisingIntervalMin = BLE_ADVERTISING_INTERVAL_DEFAULT_SPEC;
    Ble_AdvertisingAttributes.advParams.advertisingIntervalMax = BLE_ADVERTISING_INTERVAL_DEFAULT_SPEC;
    Ble_AdvertisingAttributes.advParams.advertisingType = gpHci_AdvertisingType_ConnectableUndirected;
    Ble_AdvertisingAttributes.advParams.ownAddressType = gpHci_OwnAddressType_PublicDevice;
    Ble_AdvertisingAttributes.advParams.peerAddressType = gpHci_AdvPeerAddressType_Public;
    Ble_AdvertisingAttributes.advParams.channelMap = BLE_ADVERTISING_CHANNEL_MAP_DEFAULT_SPEC;
    Ble_AdvertisingAttributes.advParams.filterPolicy = gpHci_AdvFilterPolicy_All;
    Ble_AdvertisingAttributes.advChannelTxPower = gpHal_BleGetTxPower();
    MEMSET(&Ble_AdvertisingAttributes.advParams.peerAddress, 0, sizeof(Ble_AdvertisingAttributes.advParams.peerAddress));

    // Reset advertising and scan response data
    MEMSET(Ble_AdvertisingAttributes.advData, 0, GP_HCI_ADVERTISING_DATA_PAYLOAD_SIZE_MAX);
    Ble_AdvertisingAttributes.advDataLength = 0;
    MEMSET(Ble_AdvertisingAttributes.scanRespData, 0, GP_HCI_SCAN_RESPONSE_DATA_PAYLOAD_SIZE_MAX);
    Ble_AdvertisingAttributes.scanRespDataLength = 0;

    Ble_AdvertisingAttributes.slaveConnId = BLE_CONN_HANDLE_INVALID;

}

gpHci_Result_t gpResPrAddr_cbAdvPrandTimeout(void)
{
    gpHci_Result_t result;

    if (!Ble_AdvertisingAttributes.advertisingEnabled)
    {
        return gpHci_ResultSuccess;
    }

    // The approach of stopping/restarting advertising works, but is not optimal.
    // Cleaner solution would be to introduce a pause and resume (where it is not needed to do a full stop and restart)
    Ble_SetAdvertiseDisable(NULL, false, false);
    result = Ble_SetAdvertiseEnable();

    GP_ASSERT_DEV_INT(result == gpHci_ResultSuccess);
    return result;
}

/*****************************************************************************
 *                    Service Function Definitions
 *****************************************************************************/


