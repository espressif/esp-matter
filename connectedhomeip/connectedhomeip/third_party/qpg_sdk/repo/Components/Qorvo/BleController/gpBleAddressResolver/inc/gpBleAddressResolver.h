/*
 *   Copyright (c) 2017, Qorvo Inc
 *
 *
 *   Declarations of the public functions and enumerations of gpBleAddressResolver.
 *
 *   This software is owned by Qorvo Inc
 *   and protected under applicable copyright laws.
 *   It is delivered under the terms of the license
 *   and is intended and supplied for use solely and
 *   exclusively with products manufactured by
 *   Qorvo Inc.
 *
 *
 *   THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *   CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *   IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *   LIMITED TO, IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *   QORVO INC. SHALL NOT, IN ANY
 *   CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *   INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *   FOR ANY REASON WHATSOEVER.
 *
 *   $Header$
 *   $Change$
 *   $DateTime$
 */

#ifndef _GPBLEADDRESSRESOLVER_H_
#define _GPBLEADDRESSRESOLVER_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleAddressResolver_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpHci_Includes.h"
#include "gpBle.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpBle_WlEntryType_t */
//@{
#define gpBle_WlEntryRegular                                   GP_WB_BLE_WHITELIST_ENTRY_S_ENTRY_TYPE_LSB
#define gpBle_WlEntryAdvertising                               GP_WB_BLE_WHITELIST_ENTRY_S_ADVERTISING_VALID_LSB
#define gpBle_WlEntryInitiating                                GP_WB_BLE_WHITELIST_ENTRY_S_INTIATING_VALID_LSB
#define gpBle_WlEntryScanning                                  GP_WB_BLE_WHITELIST_ENTRY_S_SCANNING_VALID_LSB
#define gpBle_WlEntryPerSync                                   GP_WB_BLE_WHITELIST_ENTRY_S_PERSYNC_VALID_LSB
typedef UInt8                             gpBle_WlEntryType_t;
//@}

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
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#include "gpBleAddressResolver_CodeJumpTableFlash_Defs.h"
#include "gpBleAddressResolver_CodeJumpTableRom_Defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
//Requests
void gpBleAddressResolver_Init(void);
void gpBleAddressResolver_Reset(Bool firstTime);

gpHci_Result_t gpBle_ClearWL(gpBle_WlEntryType_t type);
gpHci_Result_t gpBle_AddDeviceToWL(gpBle_WlEntryType_t type, gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress);
gpHci_Result_t gpBle_RemoveDeviceFromWL(gpBle_WlEntryType_t type, gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
gpHci_Result_t gpBleAddressResolver_AddSpecialWhiteListAddress(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress, UInt8 stateMask);
gpHci_Result_t gpBleAddressResolver_RemoveSpecialWhiteListAddress(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress);
UInt8 gpBleAddressResolver_GetWhiteListEntryIndex(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress);
void gpBleAddressResolver_UpdateWhiteListEntryState(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress, UInt8 state, Bool set);
void gpBleAddressResolver_UpdateWhiteListEntryStateBulk(UInt8 state, Bool set);
void gpBleAddressResolver_EnableConnectedDevicesInWhiteList(UInt8 state, Bool set);
gpHci_Result_t gpBle_LeReadWhiteListSize(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeClearWhiteList(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeAddDeviceToWhiteList(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);
gpHci_Result_t gpBle_LeRemoveDeviceFromWhiteList(gpHci_CommandParameters_t* pParams, gpBle_EventBuffer_t* pEventBuf);

//Indications

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPBLEADDRESSRESOLVER_H_

