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

#ifndef _GPBLEADDRESSRESOLVER_DEFS_H_
#define _GPBLEADDRESSRESOLVER_DEFS_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleAddressResolver_RomCode_defs.h"
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

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Mask of applicable states (bit 0 = entry type, bit 1 = advertising, bit 2 = scanning, bit 3 = initiating, bit 4 = periodic sync)

#define BLE_ADDRESSRESOLVER_VALID_REGULAR_STATE_MASK_ALL    0x0F
#define BLE_ADDRESSRESOLVER_VALID_STATE_MASK_NONE           0x00
#define BLE_ADDRESSRESOLVER_VALID_STATE_MASK_ALL            0x3F


#define NUMBER_OF_SPECIAL_ENTRIES_SUBTRACTED_BY_ROM         2

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

#if defined(GP_DIVERSITY_JUMPTABLES)
typedef struct gpBleAddressResolver_ConstGlobalVars_s{
    UInt8                             gpBleAddressResolver_MaxNrOfWhiteListEntries;
    UInt8                             gpDiversityBleMaxNrOfSupportedConnections;
    UInt8                             gpDiversityBleMaxNrOfSupportedSlaveConnections;
} gpBleAddressResolver_ConstGlobalVars_t;

//extern       gpBleAddressResolver_GlobalVars_t      gpBleAddressResolver_GlobalVars;
extern const gpBleAddressResolver_ConstGlobalVars_t gpBleAddressResolver_ConstGlobalVars;
#endif //defined(GP_DIVERSITY_JUMPTABLES)

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// in case we are in ROM, we need to retrieve the pointer via a data jump table entry
//#define GP_BLEADDRESSRESOLVER_GET_GLOBALS()             ((gpBleAddressResolver_GlobalVars_t*)      JumpTables_DataTable.gpBleAddressResolver_GlobalVars_ptr)
#define GP_BLEADDRESSRESOLVER_GET_GLOBALS_CONST()       ((gpBleAddressResolver_ConstGlobalVars_t*) JumpTables_DataTable.gpBleAddressResolver_ConstGlobalVars_ptr)
#define BLE_MAX_NR_OF_WHITELIST_ENTRIES                 (GP_BLEADDRESSRESOLVER_GET_GLOBALS_CONST()->gpBleAddressResolver_MaxNrOfWhiteListEntries)
#else // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// in case we're not in ROM, we can directly fetch the address of the global vars.
//extern gpBleAddressResolver_GlobalVars_t gpBleAddressResolver_globals;
//#define GP_BLEADDRESSRESOLVER_GET_GLOBALS()             (&gpBleAddressResolver_GlobalVars)
//#define GP_BLEADDRESSRESOLVER_GET_GLOBALS_CONST()       (&gpBleAddressResolver_ConstGlobalVars)
                                                        /* Fix for ROM macro BLE_HOST_NR_OF_WHITELIST_ENTRIES (GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES is
                                                           set to 2 in ROM, so we must compensate this with following calculation */
#define BLE_MAX_NR_OF_WHITELIST_ENTRIES                 (GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES - (GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES - NUMBER_OF_SPECIAL_ENTRIES_SUBTRACTED_BY_ROM))
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleAddressResolver_CodeJumpTableFlash_Defs_defs.h"
#include "gpBleAddressResolver_CodeJumpTableRom_Defs_defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
gpHci_Result_t Ble_AddWhiteListEntry(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress, UInt8 stateMask, UInt8 rangeStart, UInt8 rangeStop);
gpHci_Result_t Ble_RemoveWhiteListEntry(gpHci_WhitelistAddressType_t addressType, BtDeviceAddress_t* pAddress, UInt8 rangeStart, UInt8 rangeStop);
void Ble_ClearWhitelist(Bool clearAll);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPBLEADDRESSRESOLVER_DEFS_H_

