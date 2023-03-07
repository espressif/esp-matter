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
#ifndef _GPBLEADVERTISER_DEFS_H_
#define _GPBLEADVERTISER_DEFS_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleAdvertiser_RomCode_defs.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpBleActivityManager.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// Advertising interval defines
#define BLE_ADVERTISING_INTERVAL_DEFAULT_INVALID    0x0000
#define BLE_ADVERTISING_INTERVAL_DEFAULT_SPEC       0x0800 /* 1.28 s */
#define BLE_ADVERTISING_INTERVAL_RANGE_NORMAL_MIN   0x0020
#define BLE_ADVERTISING_INTERVAL_RANGE_NORMAL_MAX   0x4000
#define BLE_ADVERTISING_INTERVAL_RANGE_SPECIAL_MIN  0x00A0

#define BLE_ADVERTISING_CHANNEL_MAP_DEFAULT_SPEC    0x07    /* all channels enabled */

#define BLE_HIGH_DUTY_CYCLE_INTERVAL_MIN_UNIT  1
#define BLE_HIGH_DUTY_CYCLE_INTERVAL_MAX_UNIT  6

// times between advertising on different channels
#define BLE_INTRA_EVENT_TX_SPACING_HIGH_DUTY    1000
#ifdef GP_DIVERSITY_BLE_INTRA_EVENT_TX_SPACING_DEFAULT
#define BLE_INTRA_EVENT_TX_SPACING_DEFAULT      GP_DIVERSITY_BLE_INTRA_EVENT_TX_SPACING_DEFAULT
#else
#define BLE_INTRA_EVENT_TX_SPACING_DEFAULT      6000
#endif

#define BLE_ADV_HIGH_DUTY_DELAY_MAX             0
#ifdef GP_DIVERSITY_BLE_ADV_MAX_DELAY_IN_UNIT625
#define BLE_ADV_MAX_DELAY_IN_UNIT625            GP_DIVERSITY_BLE_ADV_MAX_DELAY_IN_UNIT625
#else
#define BLE_ADV_MAX_DELAY_IN_UNIT625            15
#endif

#define BLE_NORMAL_ADV_PRIORITY                 Ble_Priority_High
#define BLE_HIGH_DUTY_ADV_PRIORITY              Ble_Priority_Low

#define BLE_HIGH_DUTY_CYCLE_DIRECTED_ADV_TO_MS  1280



/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

typedef struct {
    gpHci_LeSetAdvertisingParametersCommand_t advParams;
    gpBleActivityManager_Timing_t advTimeParams;
    UInt8 advDataLength;
    UInt8 advData[GP_HCI_ADVERTISING_DATA_PAYLOAD_SIZE_MAX];
    Int8 advChannelTxPower;
    UInt8 scanRespDataLength;
    UInt8 scanRespData[GP_HCI_SCAN_RESPONSE_DATA_PAYLOAD_SIZE_MAX];
    Ble_IntConnId_t slaveConnId;
    Bool advertisingEnabled;
    BtDeviceAddress_t ownAddress;
#if defined(GP_DIVERSITY_JUMPTABLES)
    gpHal_BleRpaInfo_t rpaInfo;
#endif //defined(GP_COMP_BLERESPRADDR) || defined(GP_DIVERSITY_JUMPTABLES)
} Ble_AdvertisingAttributes_t;

#define BLE_ADV_WHITELIST_USED(pol)     (pol != gpHci_AdvFilterPolicy_All && pol < gpHci_AdvFilterPolicy_Invalid)

#define BLE_WRITE_ADDRESS(pdloh, address)  gpPd_PrependWithUpdate(pdloh, sizeof(BtDeviceAddress_t), address)

typedef struct gpBleAdvertiser_GlobalVars_s{
    Ble_AdvertisingAttributes_t*      Ble_AdvertisingAttributes_ptr;
} gpBleAdvertiser_GlobalVars_t;

extern       gpBleAdvertiser_GlobalVars_t gpBleAdvertiser_GlobalVars;

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// in case we are in ROM, we need to retrieve the pointer via a data jump table entry
#define GP_BLEADVERTISER_GET_GLOBALS()             ((gpBleAdvertiser_GlobalVars_t*)      JumpTables_DataTable.gpBleAdvertiser_GlobalVars_ptr)
//#define GP_BLEADVERTISER_GET_GLOBALS_CONST()       ((gpBleAdvertiser_ConstGlobalVars_t*) JumpTables_DataTable.gpBleAdvertiser_ConstGlobalVars_ptr)
#else // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
// in case we're not in ROM, we can directly fetch the address of the global vars.
//extern gpBleAdvertiser_GlobalVars_t gpBleAdvertiser_globals;
#define GP_BLEADVERTISER_GET_GLOBALS()             (&gpBleAdvertiser_GlobalVars)
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define Ble_AdvFilterPolicyMask_ScanReq     0x01
#define Ble_AdvFilterPolicyMask_ConnReq     0x02



/*****************************************************************************
 *                    Function Prototypes
 *****************************************************************************/

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpBleAdvertiser_CodeJumpTableFlash_Defs_defs.h"
#include "gpBleAdvertiser_CodeJumpTableRom_Defs_defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
void Ble_AdvChannelMapToChannels(UInt8 channelMapping, UInt8* pChannels, UInt8 numberOfChannels );
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */



/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
// checker/action functions
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
gpHci_Result_t Ble_SetAdvertisingParametersChecker(gpHci_LeSetAdvertisingParametersCommand_t* params);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
gpHci_Result_t Ble_SetAdvertisingOrScanDataChecker(UInt8 length, UInt8 maxLength);
gpHci_Result_t Ble_SetAdvertisingOrScanDataAction(UInt8 length, UInt8* pData, Bool advData);
gpHci_Result_t Ble_SetAdvertiseChecker(UInt8 advertisingEnable);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

// Various
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
gpHci_Result_t Ble_SetAdvertiseEnable(void);
gpHci_Result_t Ble_SetAdvertiseDisable(Ble_ConnEstablishParams_t* pConnEstablishParams, Bool sendCommandComplete, Bool sendConnectionComplete);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
void Ble_HighDutyCycleDirectedAdvTimeout(void);
UInt8 Ble_GetWhitelistMaskFromAdvParams(gpHci_AdvFilterPolicy_t filterPolicy);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
Ble_AdvertisingPduType_t Ble_ConvertAdvTypeToPduType(gpHci_AdvertisingType_t pduType);
void Ble_FreePdsIfValid(gpPd_Loh_t* pPdLohAdv, gpPd_Loh_t* pPdLohScan);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */


/*****************************************************************************
 *                    Tmp extern Function Prototypes
 *****************************************************************************/
#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //def _GPBLEADVERTISER_DEFS_H_
