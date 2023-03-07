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

#include "gpJumpTables.h"
#include "gpBle.h"
#include "gpBle_defs.h"
#include "gpBleAddressResolver.h"
#include "gpBleAddressResolver_defs.h"
#include "gpLog.h"

#if defined(GP_DIVERSITY_BLE_BROADCASTER) || defined(GP_DIVERSITY_BLE_SLAVE) || defined(GP_DIVERSITY_BLE_ADVERTISER)
#include "gpBleAdvertiser.h"
#endif //GP_DIVERSITY_BLE_BROADCASTER || GP_DIVERSITY_BLE_SLAVE || GP_DIVERSITY_BLE_ADVERTISER

#if defined(GP_DIVERSITY_BLE_OBSERVER) || defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SCANNER)
#include "gpBleScanner.h"
#endif //GP_DIVERSITY_BLE_OBSERVER || GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SCANNER

#if defined(GP_DIVERSITY_BLE_MASTER) || defined(GP_DIVERSITY_BLE_SLAVE)
#include "gpBleLlcp.h"
#include "gpBleInitiator.h"
#endif //GP_DIVERSITY_BLE_MASTER || GP_DIVERSITY_BLE_SLAVE

/*****************************************************************************
 *                    NRT ROM patch fix version numbers
 *****************************************************************************/

#if   defined(GP_DIVERSITY_GPHAL_K8E)
#define ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER (1)
#endif

#define GP_LOCAL_LOG

/*****************************************************************************
 *                    ROM Function prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_BLEADDRESSRESOLVER

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
 *                    Static Function Prototypes
 *****************************************************************************/

STATIC_FUNC gpHci_Result_t Ble_ManipulateWhiteListChecker(gpHci_WhitelistAddressType_t addressType);
STATIC_FUNC gpHci_Result_t Ble_ManipulateWhiteListAllowedChecker(void);

/*****************************************************************************
 *                    Tmp extern Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

gpHci_Result_t Ble_ManipulateWhiteListChecker(gpHci_WhitelistAddressType_t addressType)
{
    GP_LOG_PRINTF("[PATCH] Ble_ManipulateWhiteListChecker",0);

    gpHci_Result_t result;

    result = Ble_ManipulateWhiteListAllowedChecker();

    if(result == gpHci_ResultSuccess)
    {
        if(!BLE_WHITELIST_ADDRESS_TYPE_VALID(addressType))
        {
            result = gpHci_ResultInvalidHCICommandParameters;
        }
    }
    return result;
}

gpHci_Result_t Ble_ManipulateWhiteListAllowedChecker(void)
{
    //GP_LOG_PRINTF("PATCH ..Ble_ManipulateWhiteListAllowedChecker",0);

#ifdef GP_DIVERSITY_BLE_ADVERTISER
    if(gpBleAdvertiser_IsEnabled() && gpBleAdvertiser_IsWhitelistUsed())
    {
        GP_LOG_PRINTF("WL manip not allowed during adv",0);
        // the advertising filter policy uses the white list and advertising is enabled
        return gpHci_ResultCommandDisallowed;
    }
#endif // GP_DIVERSITY_BLE_ADVERTISER
#ifdef GP_DIVERSITY_BLE_SCANNER
    if(gpBleScanner_IsEnabled() && gpBleScanner_IsWhitelistUsed())
    {
        GP_LOG_PRINTF("WL manip not allowed during scan",0);
        // the scanning filter policy uses the white list and scanning is enabled
        return gpHci_ResultCommandDisallowed;
    }
#endif // GP_DIVERSITY_BLE_SCANNER
#ifdef GP_DIVERSITY_BLE_INITIATOR
    if(gpBleInitiator_IsEnabled() && gpBleInitiator_IsWhitelistUsed())
    {
        GP_LOG_PRINTF("WL manip not allowed during init",0);
        // the initiator filterpolicy uses the white list and an LE_Create_Connection command is outstanding
        return gpHci_ResultCommandDisallowed;
    }
#endif // GP_DIVERSITY_BLE_INITIATOR
    return gpHci_ResultSuccess;
}

void Ble_ClearWhitelist_orgrom(Bool clearAll);
void Ble_ClearWhitelist(Bool clearAll)
{
#if(GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    {
        if(clearAll)
        {
            gpHal_BleUpdateWhiteListEntryStateBulk(BLE_ADDRESSRESOLVER_VALID_STATE_MASK_ALL,
                                                   BLE_ADDRESSRESOLVER_VALID_STATE_MASK_ALL,
                                                   BLE_ADDRESSRESOLVER_VALID_STATE_MASK_NONE);
        }
        else
        {
            gpBle_ClearWL(gpBle_WlEntryRegular);
        }
        return;
    }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    Ble_ClearWhitelist_orgrom(clearAll);
    return;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Service Function Definitions
 *****************************************************************************/
gpHci_Result_t gpBle_LeAddDeviceToWhiteList_orgrom(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeAddDeviceToWhiteList(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    gpHci_Result_t result;

    gpHci_LeAddDeviceToWhiteListCommand_t* pAddDeviceToWlParams = &pParams->LeAddDeviceToWhiteList;

    GP_LOG_PRINTF("[PATCH] Add device to WL",0);

    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    result = Ble_ManipulateWhiteListChecker(pAddDeviceToWlParams->addressType);

    if(result == gpHci_ResultSuccess)
    {
#if(GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
        if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
        {
            return gpBle_AddDeviceToWL(gpBle_WlEntryRegular, pAddDeviceToWlParams->addressType, &pAddDeviceToWlParams->address);
        }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
        return gpBle_LeAddDeviceToWhiteList_orgrom(pParams, pEventBuf);
    }

    return result;
}

gpHci_Result_t gpBle_LeRemoveDeviceFromWhiteList_orgrom(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeRemoveDeviceFromWhiteList(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf)
{
    gpHci_Result_t result;

    gpHci_LeRemoveDeviceFromWhiteListCommand_t* pRemoveDeviceFromWlParams = &pParams->LeRemoveDeviceFromWhiteList;

    GP_LOG_PRINTF("[PATCH] Remove device from WL",0);

    BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(pEventBuf->eventCode);

    result = Ble_ManipulateWhiteListChecker(pRemoveDeviceFromWlParams->addressType);

    if(result == gpHci_ResultSuccess)
    {
#if(GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
        if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
        {
            return gpBle_RemoveDeviceFromWL(gpBle_WlEntryRegular, pRemoveDeviceFromWlParams->addressType, &pRemoveDeviceFromWlParams->address);
        }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
        return gpBle_LeRemoveDeviceFromWhiteList_orgrom(pParams, pEventBuf);
    }

    return result;
}

void gpBleAddressResolver_UpdateWhiteListEntryStateBulk_orgrom(UInt8 state, Bool set);
void gpBleAddressResolver_UpdateWhiteListEntryStateBulk(UInt8 state, Bool set)
{
    GP_LOG_PRINTF("[PATCH] gpBleAddressResolver_UpdateWhiteListEntryStateBulk",0);
#if(GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    {
        gpHal_BleUpdateWhiteListEntryStateBulk(BLE_ADDRESSRESOLVER_VALID_STATE_MASK_ALL, state, set?state:0x00);
        return;
    }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    gpBleAddressResolver_UpdateWhiteListEntryStateBulk_orgrom(state, set);
}

void gpBleAddressResolver_UpdateWhiteListEntryState_orgrom(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress, UInt8 state, Bool set);
void gpBleAddressResolver_UpdateWhiteListEntryState(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress, UInt8 state, Bool set)
{
    GP_LOG_PRINTF("[PATCH] gpBleAddressResolver_UpdateWhiteListEntryState",0);
#if(GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    {
        gpHal_BleUpdateWhiteListEntryState(addressType, pAddress, state, set?state:0x00, false);
        return;
    }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    gpBleAddressResolver_UpdateWhiteListEntryState_orgrom(addressType, pAddress, state, set);
}

void gpBleAddressResolver_EnableConnectedDevicesInWhiteList_orgrom(UInt8 state, Bool set);
void gpBleAddressResolver_EnableConnectedDevicesInWhiteList(UInt8 state, Bool set)
{
    GP_LOG_PRINTF("[PATCH] gpBleAddressResolver_EnableConnectedDevicesInWhiteList",0);
#if(GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    {
        UIntLoop i;
        for(i = 0; i < BLE_LLCP_MAX_NR_OF_CONNECTIONS; i++)
        {
            if(Ble_LlcpIsConnectionCreated(i))
            {
                Bool peerAddressType;
                BtDeviceAddress_t peerAddress;

                Ble_LlcpGetPeerAddressInfo(i, &peerAddressType, &peerAddress);
                gpHal_BleUpdateWhiteListEntryState(peerAddressType, &peerAddress, state, set?state:0x00, false);
            }
        }
        return;
    }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_ADDRESSRESOLVER)
    gpBleAddressResolver_EnableConnectedDevicesInWhiteList_orgrom(state, set);
}
