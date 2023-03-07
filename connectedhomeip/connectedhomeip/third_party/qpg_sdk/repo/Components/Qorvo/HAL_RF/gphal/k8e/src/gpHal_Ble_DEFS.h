/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpHal_DEFS.h
 *
 *  This file contains internal definitions of the GPHAL.
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

#ifndef _HAL_GP_BLE_DEFS_H_
#define _HAL_GP_BLE_DEFS_H_


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#ifdef GP_COMP_GPHAL_BLE
#include "gpHal_kx_BLE.h"
#endif //GP_COMP_GPHAL_BLE

#include "gpHal_Ble.h"

/*****************************************************************************
*                    Macro Definitions
*****************************************************************************/
#define GP_HAL_BLE_MAX_VALID_ANTENNA_ID       15

#define GPHAL_BLE_MAX_NR_OF_PHYS 1
#define GPHAL_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS         GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS
#define GPHAL_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS   GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS

#define GPHAL_BLE_CONN_ID_INVALID     0xFF

// Event Info memory
extern UInt8 gpHal_Ble_EventInfoMemory[];
#ifndef GP_COMP_CHIPEMU
#define GP_HAL_BLE_EVENT_INFO_START         ((UIntPtr)&gpHal_Ble_EventInfoMemory[0])
#endif
#define GPHAL_BLE_TEST_INFO_SIZE                0x31

#define GPHAL_BLE_NR_OF_SERVICE_EVENTS    5
// Event Info memory
#ifdef GP_COMP_CHIPEMU
extern UInt32 gpChipEmu_GetGpMicroStructBleEventInfoStart(UInt32 gp_mm_ram_linear_start);
#define GP_HAL_BLE_EVENT_INFO_START         gpChipEmu_GetGpMicroStructBleEventInfoStart(GP_MM_RAM_LINEAR_START)
#else
#define GP_HAL_BLE_EVENT_INFO_START         ((UIntPtr)&gpHal_Ble_EventInfoMemory[0])
#endif // GP_COMP_CHIPEMU


// Size of different info structures
#define GPHAL_BLE_SERVICE_EVENT_INFO_SIZE       0x38
#define GPHAL_BLE_CONN_EVENT_INFO_SIZE          0x90
#define GPHAL_BLE_CHAN_MAP_INFO_SIZE            0x30

// We need one channel map for all master links, one tmp channel map and 2 channel maps (one actual, one temp) for each slave connection and one per each periodic advertising / sync.
#define GPHAL_BLE_MAX_NR_OF_SUPPORTED_CHANNEL_MAPS    (1 + 1 + 2*GPHAL_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS + GP_BLE_MAX_NUMBER_PERASC + GP_BLE_MAX_NUMBER_SYNCC)


#define GP_HAL_BLE_EXT_INIT_VIRT_INFO_SIZE                   (0x06)
#define GP_HAL_BLE_EXT_INIT_INFO_SIZE                        (0x0E)


#define GP_HAL_BLE_EXT_SCAN_INFO_SIZE                        (0x02)




#ifdef GP_BSP_PHY_MAX_NR_OF_ANTENNAS
// keep the dflags checker happy - this board property is used in Components/Qorvo/BleController
#endif

// RT (rom and flash) version private to k8e
#define GP_HAL_BLE_FIRST_RT_VERSION_SUPPORTING_WORST_SCA_WIDENING   1
#define GP_HAL_BLE_FIRST_RT_VERSION_SUPPORTING_SEC_PHY_MASK         1

/*****************************************************************************
*                   Functional Macro Definitions
*****************************************************************************/

ALWAYS_INLINE UInt16 gpHal_Ble_Ram_Address_To_Start_Offset(UInt32 addr)
{
    UInt32 x = addr - GP_MM_RAM_LINEAR_START;
    return (UInt16)x;
}

#define GPHAL_BLE_RAM_ADDRESS_TO_START_OFFSET(addr)         gpHal_Ble_Ram_Address_To_Start_Offset(addr)

#define GP_HAL_BLE_SERVICE_TO_EVENT_INFO_ADDRESS(service)   (GP_HAL_BLE_EVENT_INFO_START + service*GPHAL_BLE_SERVICE_EVENT_INFO_SIZE)
#define GP_HAL_BLE_CONNECTION_TO_EVENT_INFO_ADDRESS(connId) (GP_HAL_BLE_SERVICE_TO_EVENT_INFO_ADDRESS(GPHAL_BLE_NR_OF_SERVICE_EVENTS) + connId * GPHAL_BLE_CONN_EVENT_INFO_SIZE)
#define GP_HAL_BLE_CHAN_MAP_TO_EVENT_INFO_ADDRESS(chanMap)  (GP_HAL_BLE_CONNECTION_TO_EVENT_INFO_ADDRESS(GPHAL_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS) + chanMap * GPHAL_BLE_CHAN_MAP_INFO_SIZE)

#define GP_HAL_BLE_SERVICE_TO_OFFSET_FROM_START(service)    /*TO_GPM_ADDR(GP_HAL_BLE_SERVICE_TO_EVENT_INFO_ADDRESS(service))*/ GPHAL_BLE_RAM_ADDRESS_TO_START_OFFSET(GP_HAL_BLE_SERVICE_TO_EVENT_INFO_ADDRESS(service))
#define GP_HAL_BLE_CONNECTION_TO_OFFSET_FROM_START(connId)  GPHAL_BLE_RAM_ADDRESS_TO_START_OFFSET(GP_HAL_BLE_CONNECTION_TO_EVENT_INFO_ADDRESS(connId))
#define GP_HAL_BLE_CHAN_MAP_TO_OFFSET_FROM_START(chanMap)   GPHAL_BLE_RAM_ADDRESS_TO_START_OFFSET(GP_HAL_BLE_CHAN_MAP_TO_EVENT_INFO_ADDRESS(chanMap))

/*****************************************************************************
*                    Type Definitions
*****************************************************************************/

typedef struct {
    UInt8 connId;
    gpHal_AbsoluteEventId_t eventNr;
    Bool running;
    Bool virtual;
} gpHal_BleConnectionContext_t;

typedef struct {
    gpHal_AbsoluteEventId_t eventNr;
} gpHal_BleServiceContext_t;

#define gpHal_BleServiceEventIdAdvertising   0x00
#define gpHal_BleServiceEventIdScanning      0x01
#define gpHal_BleServiceEventIdInitiating    0x02
#define gpHal_BleServiceEventIdSubEv         0x03
#define gpHal_BleServiceEventIdBgsc          0x04
typedef UInt8 gpHal_BleServiceEventId_t;

/*****************************************************************************
*                    Public Function Definitions
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
gpHal_Result_t gpHal_BleStartService(gpHal_BleServiceEventId_t serviceEvent, UInt32 timeStamp, UInt8 type);
gpHal_Result_t gpHal_BleStopService(gpHal_BleServiceEventId_t serviceEvent);

// PD - PBM conversion
void gpHal_BleConfPbmToPd(UInt8 pbmHandle, gpPd_Loh_t* pPdLoh);
void gpHal_BleIndPbmToPd(UInt8 pbmEntry, gpPd_Loh_t* pPdLoh);

void gpHal_BlePopulateDefaultPbmOptions(UInt8 pbmHandle, Bool TxIfTooLate);
void gpHal_BleSetPbmTxPower(UInt8 pbmHandle, Int8 txPower);
UInt8 gpHal_BlePdToPbm(gpPd_Loh_t pdLoh, Bool TxIfTooLate);

// Connection context mapping
gpHal_BleConnectionContext_t* gpHal_BleAddConnIdMapping(UInt8 connId);
gpHal_BleConnectionContext_t* gpHal_BleGetConnMappingFromId(UInt8 connId);
gpHal_BleConnectionContext_t* gpHal_BleGetConnMappingFromEvent(UInt8 eventNr);
void gpHal_BleRemoveConnIdMapping(gpHal_BleConnectionContext_t* pMapping);


#ifdef GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK
void gpHal_BleWl_cbUpdateIndex(UInt8 addressType, const BtDeviceAddress_t* pAddress, UInt8 idx);
UInt8 gpHal_BleWl_FindEntry(UInt8 addressType, BtDeviceAddress_t* pAddress);
#endif //GP_DIVERSITY_GPHAL_WHITELIST_UPDATE_CALLBACK

UInt8 gpHal_Ble_GetAdvAntenna(void);
#ifdef __cplusplus
}
#endif
#endif //_HAL_GP_BLE_DEFS_H_
