/*
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

#ifndef _GPBLEDATACOMMON_H_
#define _GPBLEDATACOMMON_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHci_Includes.h"
#include "gpBle.h"
#include "gpBle_defs.h"
#include "gpHci_types_manual.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// The max number of payload bytes we can put in a BLE PDU (independent of PHY modulation and encryption)
#define GPBLEDATACOMMON_PAYLOAD_THEORETICAL_MAX (GP_HAL_PBM_MAX_SIZE - (BLE_ACCESS_ADDRESS_SIZE + BLE_PACKET_HEADER_SIZE + BLE_CTE_INFO_SIZE + BLE_SEC_MIC_LENGTH + GP_HAL_PBM_BLE_NR_RESERVED_BYTES))

// Spec min/max values for octet and time parameters
#define GPBLEDATACOMMON_OCTETS_SPEC_MIN         27
#define GPBLEDATACOMMON_OCTETS_SPEC_DEFAULT     GPBLEDATACOMMON_OCTETS_SPEC_MIN
#define GPBLEDATACOMMON_OCTETS_SPEC_MAX         251
#define GPBLEDATACOMMON_OCTETS_SUPPORTED_MAX    min(GPBLEDATACOMMON_PAYLOAD_THEORETICAL_MAX, GPBLEDATACOMMON_OCTETS_SPEC_MAX)

#define GPBLEDATACOMMON_TIME_SPEC_MIN           gpBleDataCommon_GetPacketDurationUs(GPBLEDATACOMMON_OCTETS_SPEC_MIN, gpHci_PhyWithCoding_1Mb, true)
#define GPBLEDATACOMMON_TIME_SPEC_DEFAULT       GPBLEDATACOMMON_TIME_SPEC_MIN
#define GPBLEDATACOMMON_TIME_UNCODED_SPEC_MAX   gpBleDataCommon_GetPacketDurationUs(GPBLEDATACOMMON_OCTETS_SPEC_MAX, gpHci_PhyWithCoding_1Mb, true)
#define GPBLEDATACOMMON_TIME_CODED_SPEC_MIN     gpBleDataCommon_GetPacketDurationUs(GPBLEDATACOMMON_OCTETS_SPEC_MIN, gpHci_PhyWithCoding_Coded125kb, true)
#define GPBLEDATACOMMON_TIME_CODED_SPEC_MAX     gpBleDataCommon_GetPacketDurationUs(GPBLEDATACOMMON_OCTETS_SPEC_MAX, gpHci_PhyWithCoding_Coded125kb, true)

#define GPBLEDATACOMMON_TIME_SPEC_MAX           GPBLEDATACOMMON_TIME_CODED_SPEC_MAX


#define GPBLEDATACOMMON_TIME_SUPPORTED_MAX      gpBleDataCommon_GetPacketDurationUs(GPBLEDATACOMMON_OCTETS_SUPPORTED_MAX, gpHci_PhyWithCoding_1Mb, true)

// Intial value for the offset inside a data channel PDU. This is actually the largest offset we can use where we still have room for an (optional) MIC behind all data.
#define GPBLEDATACOMMON_PDU_FOOTER_MAX_OFFSET   (GP_HAL_PBM_MAX_SIZE - BLE_SEC_MIC_LENGTH)

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

#define GPBLEDATACOMMON_GET_SUPPORTED_PHYS_MASK()       HCI_PHYMASK_INIT((GP_HCI_PHY_MASK_1MB))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Component Function Definitions
 *****************************************************************************/

void gpBle_DataCommonInit(void);
void gpBle_DataCommonReset(Bool firstReset);
void gpBle_DataOpenConnection(Ble_IntConnId_t connId, gpHci_PhyWithCoding_t phy);
void gpBle_DataCloseConnection(Ble_IntConnId_t connId);
Bool gpBle_IsConnectionOpen(Ble_IntConnId_t connId);
UInt16 gpBle_GetMaxTxOctetsLocal(Ble_IntConnId_t connId);
UInt16 gpBle_GetMaxRxOctetsLocal(Ble_IntConnId_t connId);
UInt16 gpBle_GetMaxTxTimeLocal(Ble_IntConnId_t connId);
UInt16 gpBle_GetMaxRxTimeLocal(Ble_IntConnId_t connId);
void gpBle_SetMaxTxOctetsLocal(Ble_IntConnId_t connId, UInt16 maxTxOctetsLocal);
void gpBle_SetMaxTxTimeLocal(Ble_IntConnId_t connId, UInt16 maxTxTimeLocal);
void gpBle_SetMaxTxOctetsRemote(Ble_IntConnId_t connId, UInt16 maxTxOctetsRemote);
void gpBle_SetMaxRxOctetsRemote(Ble_IntConnId_t connId, UInt16 maxRxOctetsRemote);
void gpBle_SetMaxTxTimeRemote(Ble_IntConnId_t connId, UInt16 maxTxTimeRemote);
void gpBle_SetMaxRxTimeRemote(Ble_IntConnId_t connId, UInt16 maxRxTimeRemote);
UInt16 gpBle_GetEffectiveMaxTxOctets(Ble_IntConnId_t connId);
UInt16 gpBle_GetEffectiveMaxTxTime(Ble_IntConnId_t connId);
UInt16 gpBle_GetEffectiveMaxRxOctets(Ble_IntConnId_t connId);
UInt16 gpBle_GetEffectiveMaxRxTime(Ble_IntConnId_t connId);
gpHci_Phy_t gpBle_GetEffectivePhyTxType(Ble_IntConnId_t connId);
gpHci_PhyWithCoding_t gpBle_GetEffectivePhyTxTypeWithCoding(Ble_IntConnId_t connId);
gpHci_Phy_t gpBle_GetEffectivePhyRxType(Ble_IntConnId_t connId);
void gpBle_SetEffectivePhys(Ble_IntConnId_t connId, gpHci_PhyWithCoding_t txPhy, gpHci_PhyWithCoding_t rxPhy, gpHci_PhyOptions_t phyOptions);
UInt16 gpBle_GetHostAclDataLength(void);
UInt16 gpBle_GetHostTotalNumAclPackets(void);
gpHci_PhyMask_t gpBle_GetPreferredPhyModesTx(Ble_IntConnId_t connId);
gpHci_PhyMask_t gpBle_GetPreferredPhyModesRx(Ble_IntConnId_t connId);
void gpBle_SetPreferredPhyModesTx(Ble_IntConnId_t connId, gpHci_PhyMask_t phyModesTx);
void gpBle_SetPreferredPhyModesRx(Ble_IntConnId_t connId, gpHci_PhyMask_t phyModesRx);
void gpBle_SetDefaultPhyModesTx(gpHci_PhyMask_t phyModesTx);
void gpBle_SetDefaultPhyModesRx(gpHci_PhyMask_t phyModesRx);
Bool gpBle_DataOctetsAndTimesValidMinimum(UInt16 rxOctets, UInt16 rxTime, UInt16 txOctets, UInt16 txTime);
UInt16 gpBle_GetEffectiveMaxRxPacketDuration(Ble_IntConnId_t connId);
UInt16 gpBleDataCommon_GetPayloadDurationUs(UInt16 payloadLengthBytes, gpHci_PhyWithCoding_t phy);
UInt16 gpBleDataCommon_GetOverheadDurationUs(gpHci_PhyWithCoding_t phy);
UInt16 gpBleDataCommon_GetOverheadDurationUs_ForDF(gpHci_PhyWithCoding_t phy, Bool CteInfoPresent);
UInt16 gpBleDataCommon_GetPacketDurationUs(UInt16 payloadLengthBytes, gpHci_PhyWithCoding_t phy, Bool includeMic);
UInt16 gpBleDataCommon_GetPacketDurationUs_ForDF(UInt16 payloadLengthBytes, gpHci_PhyWithCoding_t phy, Bool includeMic, UInt8 CteDuration);
UInt16 gpBleDataCommon_GetOctetsFromDurationUs(UInt16 effectiveTime, gpHci_PhyWithCoding_t phy, UInt8 CteDuration);
gpHal_BleTxPhy_t gpBleDataCommon_HciPhyToHalTxPhy(gpHci_Phy_t hciPhy);
gpHal_BleRxPhy_t gpBleDataCommon_HciPhyToHalRxPhy(gpHci_Phy_t hciPhy);
UInt32 gpBleDataCommon_GetIntervalPortionOccupiedUs(Ble_IntConnId_t connId);
gpHci_Phy_t gpBleDataCommon_PhyWithCodingToPhy(gpHci_PhyWithCoding_t phyWithCoding);
gpHci_Phy_t gpBleDataCommon_HalTxPhyToHciPhy(gpHal_BleTxPhy_t halPhy);
gpHci_PhyWithCoding_t gpBleDataCommon_HalPhyWithCodingToHciPhyWithCoding(gpHal_BleTxPhy_t halPhy);
gpHal_BleTxPhy_t gpBleDataCommon_HciPhyWithCodingToHalPhyWithCoding(gpHci_PhyWithCoding_t hciPhy);
gpHci_Phy_t gpBleDataCommon_HalRxPhyToHciPhy(gpHal_BleRxPhy_t halPhy);
gpHci_PhyWithCoding_t gpBleDataCommon_PhyToPhyWithCoding(gpHci_Phy_t hciPhy, gpHci_PhyOptions_t phyOptions);
gpHal_BleTxPhy_t BleDataCommon_HciPhyToHalTxPhy(gpHci_Phy_t hciPhy, gpHci_PhyOptions_t phyOptions);

/*****************************************************************************
 *                    Service Function Definitions
 *****************************************************************************/



#endif //_GPBLEDATACOMMON_H_

