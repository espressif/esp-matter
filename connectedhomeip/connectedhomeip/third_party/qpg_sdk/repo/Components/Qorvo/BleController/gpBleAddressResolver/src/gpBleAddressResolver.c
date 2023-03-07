/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

//#define GP_LOCAL_LOG

#define GP_COMPONENT_ID GP_COMPONENT_ID_BLEADDRESSRESOLVER

#include "gpBle.h"
#include "gpBleComps.h"
#include "gpBle_defs.h"
#include "gpAssert.h"
#include "gpLog.h"
#include "gpHal.h"
#include "gpBleAddressResolver.h"
#include "gpBleAddressResolver_defs.h"
#include "gpBleConfig.h"

#if defined(GP_DIVERSITY_BLE_BROADCASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
#include "gpBleAdvertiser.h"
#endif //GP_DIVERSITY_BLE_BROADCASTER || GP_DIVERSITY_BLE_SLAVE

#if defined(GP_DIVERSITY_BLE_OBSERVER) || defined(GP_DIVERSITY_BLE_MASTER)
#include "gpBleScanner.h"
#endif //GP_DIVERSITY_BLE_OBSERVER || GP_DIVERSITY_BLE_MASTER

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
#include "gpBleInitiator.h"
#include "gpBleLlcp.h"
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

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

#if defined(GP_DIVERSITY_JUMPTABLES)
const gpBleAddressResolver_ConstGlobalVars_t gpBleAddressResolver_ConstGlobalVars =
{
    .gpBleAddressResolver_MaxNrOfWhiteListEntries    = GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES
                                                     /* Fix for ROM macro BLE_HOST_NR_OF_WHITELIST_ENTRIES (GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES is
                                                        set to 2 in ROM, so we must compensate this with following calculation */
                                                     - (GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES - NUMBER_OF_SPECIAL_ENTRIES_SUBTRACTED_BY_ROM),
    .gpDiversityBleMaxNrOfSupportedConnections       = GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS,
    .gpDiversityBleMaxNrOfSupportedSlaveConnections  = GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS,
};
#endif //defined(GP_DIVERSITY_JUMPTABLES)


/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
static void Ble_loopAndReportWLModifyEvent(UInt8 matchMask, UInt8 stateMask_mask, UInt8 stateMask_value);
/*****************************************************************************
 *                    Tmp extern Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
void Ble_loopAndReportWLModifyEvent(UInt8 matchMask, UInt8 stateMask_mask, UInt8 stateMask_value)
{
    for(UInt8 i=0; i<GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES; i++)
    {
        UInt8 stateMask = gpHal_BleGetWhitelistEntryState(i);
        if((stateMask != 0x00) && (matchMask & stateMask))
        {
            GP_LOG_PRINTF("[WL] Removed entry %u", 0, i);
            gpHal_WhiteListEntry_t entry;
            gpHal_BleGetWhitelistEntry(i, &entry);
            {
                gpHci_VsdMetaEventParams_t vsdParams;

                MEMSET(&vsdParams, 0, sizeof(vsdParams));
                vsdParams.subEventCode = gpHci_VsdSubEventWhiteListModified;
                GP_LOG_PRINTF_ADDRESS("[WL] Removed", entry.address);
                MEMCPY(&vsdParams.params.whiteListModified.address, &entry.address, sizeof(BtDeviceAddress_t));
                vsdParams.params.whiteListModified.isAdded = false;
                gpBle_SendVsdMetaEvent(&vsdParams);
            }
        }
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpBleAddressResolver_Init(void)
{

}

void gpBleAddressResolver_Reset(Bool firstReset)
{
    // Fully erase the whitelist (including the special entries)
    Ble_ClearWhitelist(true);

}


gpHci_Result_t gpBle_ClearWL(gpBle_WlEntryType_t type)
{
    UInt8 matchMask;
    UInt8 stateMask;

    switch(type)
    {
        case gpBle_WlEntryRegular:
            matchMask = BLE_ADDRESSRESOLVER_VALID_REGULAR_STATE_MASK_ALL;
            stateMask = BLE_ADDRESSRESOLVER_VALID_REGULAR_STATE_MASK_ALL;
            if(gpBleConfig_IsLeMetaVSDEventMasked(gpHci_VsdSubEventWhiteListModified))
            {
                Ble_loopAndReportWLModifyEvent( matchMask, stateMask, BLE_ADDRESSRESOLVER_VALID_STATE_MASK_NONE);
            }
            break;
        case gpBle_WlEntryAdvertising:
            matchMask = GP_WB_BLE_WHITELIST_ENTRY_S_ADVERTISING_VALID_MASK;
            stateMask = GP_WB_BLE_WHITELIST_ENTRY_S_ADVERTISING_VALID_MASK;
            break;
        case gpBle_WlEntryInitiating:
            matchMask = GP_WB_BLE_WHITELIST_ENTRY_S_INTIATING_VALID_MASK;
            stateMask = GP_WB_BLE_WHITELIST_ENTRY_S_INTIATING_VALID_MASK;
            break;
        case gpBle_WlEntryScanning:
            matchMask = GP_WB_BLE_WHITELIST_ENTRY_S_SCANNING_VALID_MASK;
            stateMask = GP_WB_BLE_WHITELIST_ENTRY_S_SCANNING_VALID_MASK;
            break;
#ifdef GP_DIVERSITY_PERIODIC_ADVERTISING_SYNC
        case gpBle_WlEntryPerSync:
            matchMask = GP_WB_BLE_WHITELIST_ENTRY_S_PERSYNC_VALID_MASK;
            stateMask = GP_WB_BLE_WHITELIST_ENTRY_S_PERSYNC_VALID_MASK;
            break;
#endif //GP_DIVERSITY_PERIODIC_ADVERTISING_SYNC
        default:
            return gpHci_ResultInvalid;
            break;
    }

    gpHal_BleUpdateWhiteListEntryStateBulk(matchMask, stateMask, BLE_ADDRESSRESOLVER_VALID_STATE_MASK_NONE);
    return gpHci_ResultSuccess;
}

gpHci_Result_t gpBle_AddDeviceToWL(gpBle_WlEntryType_t type, gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress)
{
    UInt8 stateMask;
    UInt8 stateMaskValue;
    UInt8 event = false;


    switch(type)
    {
        case gpBle_WlEntryRegular:
            stateMask = GP_WB_BLE_WHITELIST_ENTRY_S_ENTRY_TYPE_MASK;
            stateMaskValue = GP_WB_BLE_WHITELIST_ENTRY_S_ENTRY_TYPE_MASK;
            if(gpBleConfig_IsLeMetaVSDEventMasked(gpHci_VsdSubEventWhiteListModified))
            {
                event = (GP_HAL_BLE_WL_ID_INVALID == gpHal_BleFindWhiteListEntry(addressType, pAddress, 0x00, GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1) );
            }
            break;
        case gpBle_WlEntryAdvertising:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_ADVERTISING_VALID_MASK;
            stateMaskValue = GP_WB_BLE_WHITELIST_ENTRY_S_ADVERTISING_VALID_MASK;
            break;
        case gpBle_WlEntryInitiating:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_INTIATING_VALID_MASK;
            stateMaskValue = GP_WB_BLE_WHITELIST_ENTRY_S_INTIATING_VALID_MASK;
            break;
        case gpBle_WlEntryScanning:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_SCANNING_VALID_MASK;
            stateMaskValue = GP_WB_BLE_WHITELIST_ENTRY_S_SCANNING_VALID_MASK;
            break;
#ifdef GP_DIVERSITY_PERIODIC_ADVERTISING_SYNC
        case gpBle_WlEntryPerSync:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_PERSYNC_VALID_MASK;
            stateMaskValue = GP_WB_BLE_WHITELIST_ENTRY_S_PERSYNC_VALID_MASK;
            break;
#endif //GP_DIVERSITY_PERIODIC_ADVERTISING_SYNC
        default:
            return gpHci_ResultInvalid;
            break;
    }

    gpHal_Result_t status = gpHal_BleUpdateWhiteListEntryState(addressType, pAddress, stateMask, stateMaskValue, true);
    if (status == gpHal_ResultSuccess && event == true)
    {
            gpHci_VsdMetaEventParams_t vsdParams;

            MEMSET(&vsdParams, 0, sizeof(vsdParams));
            vsdParams.subEventCode = gpHci_VsdSubEventWhiteListModified;
            MEMCPY(&vsdParams.params.whiteListModified.address, pAddress, sizeof(BtDeviceAddress_t));
            GP_LOG_PRINTF_ADDRESS("[WL] Adding", (*pAddress));
            vsdParams.params.whiteListModified.isAdded = true;
            gpBle_SendVsdMetaEvent(&vsdParams);
    }
    else
    {
        GP_LOG_PRINTF_ADDRESS("[WL] adding failed", (*pAddress));
    }
    return status?gpHci_ResultMemoryCapacityExceeded:gpHci_ResultSuccess;
}

gpHci_Result_t gpBle_RemoveDeviceFromWL(gpBle_WlEntryType_t type, gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress)
{
    UInt8 stateMask;
    UInt8 event = false;
    switch(type)
    {
        case gpBle_WlEntryRegular:
            stateMask = BLE_ADDRESSRESOLVER_VALID_REGULAR_STATE_MASK_ALL;
            if(gpBleConfig_IsLeMetaVSDEventMasked(gpHci_VsdSubEventWhiteListModified))
            {
                event = (GP_HAL_BLE_WL_ID_INVALID != (gpHal_BleFindWhiteListEntry(addressType, pAddress, 0x00, GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES-1)));
            }
            break;
        case gpBle_WlEntryAdvertising:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_ADVERTISING_VALID_MASK;
            break;
        case gpBle_WlEntryInitiating:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_INTIATING_VALID_MASK;
            break;
        case gpBle_WlEntryScanning:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_SCANNING_VALID_MASK;
            break;
#ifdef GP_DIVERSITY_PERIODIC_ADVERTISING_SYNC
        case gpBle_WlEntryPerSync:
            stateMask      = GP_WB_BLE_WHITELIST_ENTRY_S_PERSYNC_VALID_MASK;
            break;
#endif //GP_DIVERSITY_PERIODIC_ADVERTISING_SYNC
        default:
            return gpHci_ResultInvalid;
            break;
    }

    gpHal_Result_t status = gpHal_BleUpdateWhiteListEntryState(addressType, pAddress, stateMask, BLE_ADDRESSRESOLVER_VALID_STATE_MASK_NONE, false);
    if (status == gpHal_ResultSuccess && event == true)
    {
            gpHci_VsdMetaEventParams_t vsdParams;

            MEMSET(&vsdParams, 0, sizeof(vsdParams));
            vsdParams.subEventCode = gpHci_VsdSubEventWhiteListModified;
            MEMCPY(&vsdParams.params.whiteListModified.address, pAddress, sizeof(BtDeviceAddress_t));
            GP_LOG_PRINTF_ADDRESS("[WL] Removing", (*pAddress));
            vsdParams.params.whiteListModified.isAdded = false;
            gpBle_SendVsdMetaEvent(&vsdParams);
    }
    return status?gpHci_ResultInvalid:gpHci_ResultSuccess;

}

/*****************************************************************************
 *                    Public Service Function Definitions
 *****************************************************************************/
