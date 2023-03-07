/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
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
 */

/** @file gpHal_Ble.h
 *  @brief Declarations of the public functions and enumerations of gpHal_Ble.
*/

#ifndef _GPHAL_BLE_H_
#define _GPHAL_BLE_H_

// MANUAL
#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_RomCode_Ble.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpPd.h"

/* <CodeGenerator Placeholder> AdditionalIncludes */
#include "gp_global.h"
#include "gpHal.h"
#include "gpHal_Phy.h"
#include "gpHal_reg.h"
/* </CodeGenerator Placeholder> AdditionalIncludes */

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpHal_BleTxPhy_t */
//@{
#define gpHal_BleTxPhy1Mb                                      GPHAL_ENUM_PHY_MODE_TX_BLE
#define gpHal_BleTxPhy2Mb                                      GPHAL_ENUM_PHY_MODE_TX_BLE_HDR
#ifdef GP_HAL_DIVERSITY_BLE_LONG_RANGE_SUPPORTED
#define gpHal_BleTxPhyCoded125kb                               GPHAL_ENUM_PHY_MODE_TX_BLE_LR125
#define gpHal_BleTxPhyCoded500kb                               GPHAL_ENUM_PHY_MODE_TX_BLE_LR500
#endif // GP_HAL_DIVERSITY_BLE_LONG_RANGE_SUPPORTED
#define gpHal_BleTxPhyInvalid                                  GPHAL_ENUM_PHY_MODE_TX_BLE_INVALID
/** @typedef gpHal_BleTxPhy_t
 *  @brief Type of Tx PHY.
 */
typedef UInt8                             gpHal_BleTxPhy_t;
//@}

/** @enum gpHal_BleRxPhy_t */
//@{
#define gpHal_BleRxPhy1Mb                                      GPHAL_ENUM_PHY_MODE_RX_BLE
#define gpHal_BleRxPhy2Mb                                      GPHAL_ENUM_PHY_MODE_RX_BLE_HDR
#ifdef GP_HAL_DIVERSITY_BLE_LONG_RANGE_SUPPORTED
#define gpHal_BleRxPhyCoded                                    GPHAL_ENUM_PHY_MODE_RX_BLE_LR
#endif // GP_HAL_DIVERSITY_BLE_LONG_RANGE_SUPPORTED
#define gpHal_BleRxPhyInvalid                                  GPHAL_ENUM_PHY_MODE_RX_BLE_INVALID
/** @typedef gpHal_BleRxPhy_t
 *  @brief Type of Rx PHY.
 */
typedef UInt8                             gpHal_BleRxPhy_t;
//@}

/** @enum gpHal_BleAdvASCFlags_t */
//@{
#define gpHal_BleAdvASCQueue                                   BM(0)
#define gpHal_BleAdvASCDirectAdv                               BM(1)
#define gpHal_BleAdvASCScanReq                                 BM(2)
#define gpHal_BleAdvASCConnReq                                 BM(3)
typedef UInt8                             gpHal_BleAdvASCFlags_t;
//@}

/** @enum gpHal_BleScanASCFlags_t */
//@{
#define gpHal_BleScanASCInUse                                  BM(0)
#define gpHal_BleScanASCHalted                                 BM(1)
typedef UInt8                             gpHal_BleScanASCFlags_t;
//@}

/** @enum gpHal_BleSedType_t */
//@{
#define gpHal_BleSedTxEAPrim                                   GP_WB_ENUM_SUBEV_TYPE_EA_TX_PRI
#define gpHal_BleSedTxEASecConn                                GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_CONN
#define gpHal_BleSedTxEASecScan                                GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_SCAN
#define gpHal_BleSedTxEASecData                                GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_DATA
#define gpHal_BleSedTxEASecPeriodicSync                        GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_PERIODIC_SYNC
#define gpHal_BleSedTxEASecPeriodicData                        GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_PERIODIC_DATA
#define gpHal_BleSedTxLAPrim                                   GP_WB_ENUM_SUBEV_TYPE_LA_TX_PRI
#define gpHal_BleSedRxEAPrimScan                               GP_WB_ENUM_SUBEV_TYPE_EA_RX_PRI_SCAN
#define gpHal_BleSedRxEAPrimInitScan                           GP_WB_ENUM_SUBEV_TYPE_EA_RX_PRI_INIT_SCAN
#define gpHal_BleSedRxEASecConn                                GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_CONN
#define gpHal_BleSedRxEASecScan                                GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_SCAN
#define gpHal_BleSedRxEASecData                                GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_DATA
#define gpHal_BleSedRxEASecPeriodicSync                        GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_PERIODIC_SYNC
#define gpHal_BleSedRxEASecPeriodicData                        GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_PERIODIC_DATA
#define gpHal_BleSedTypeInvalid                                GP_WB_ENUM_SUBEV_TYPE_NOT_VALID
typedef UInt8                             gpHal_BleSedType_t;
//@}

/** @enum gpHal_WlEntryIndex_t */
//@{
typedef UInt8                             gpHal_WlEntryIndex_t;
//@}

/** @enum gpHal_ConnEventInfoMask_t */
//@{
#define connEventInfoMask_priority                             0x0000001
#define connEventInfoMask_enableExtPriority                    0x0000002
#define connEventInfoMask_interval                             0x0000004
#define connEventInfoMask_channelId                            0x0000008
#define connEventInfoMask_hopIncrement                         0x0000010
#define connEventInfoMask_chanMapHandle                        0x0000020
#define connEventInfoMask_accessAddress                        0x0000040
#define connEventInfoMask_crcInit                              0x0000080
#define connEventInfoMask_windowDuration                       0x0000100
#define connEventInfoMask_masterSca                            0x0000200
#define connEventInfoMask_slaveLatency                         0x0000400
#define connEventInfoMask_eventCounter                         0x0000800
#define connEventInfoMask_unmappedChannelPtr                   0x0001000
#define connEventInfoMask_pdLoh                                0x0002000
#define connEventInfoMask_txSeqNr                              0x0004000
#define connEventInfoMask_txNextSeqNr                          0x0008000
#define connEventInfoMask_rxSeqNr                              0x0010000
#define connEventInfoMask_rxNextSeqNr                          0x0020000
#define connEventInfoMask_lastConnEventCount                   0x0040000
#define connEventInfoMask_lastConnEventCountValid              0x0080000
#define connEventInfoMask_rxFlowCtrlFlag                       0x0100000
#define connEventInfoMask_txQueueWritePtr                      0x0200000
#define connEventInfoMask_txQueueReadPtr                       0x0400000
#define connEventInfoMask_txQueue                              0x0800000
#define connEventInfoMask_lastCorrelationTime                  0x1000000
#define connEventInfoMask_nrOfConsecSkippedEvents              0x2000000
/** @typedef gpHal_ConnEventInfoMask_t
 *  @brief Mask for selecting which ConnEventInfo field must be written!
 */
typedef UInt32                            gpHal_ConnEventInfoMask_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro GP_HAL_BT_DEVICE_ADDRESS_LENGTH */
#define GP_HAL_BT_DEVICE_ADDRESS_LENGTH              6
/** @macro GP_HAL_BLE_WL_ID_INVALID */
#define GP_HAL_BLE_WL_ID_INVALID                     0xFF
/** @macro GP_HAL_BLE_IRK_IDX_INVALID */
#define GP_HAL_BLE_IRK_IDX_INVALID                   0xFF
/** @macro GP_HAL_NR_OF_ADVERTISING_CHANNELS */
#define GP_HAL_NR_OF_ADVERTISING_CHANNELS            3
/** @macro GP_HAL_CHANNEL_MAP_LENGTH */
#define GP_HAL_CHANNEL_MAP_LENGTH                    5
/** @macro GP_HAL_NR_OF_DATA_CHANNELS */
#define GP_HAL_NR_OF_DATA_CHANNELS                   37
/** @macro GP_HAL_NR_OF_BLE_CHANNELS */
#define GP_HAL_NR_OF_BLE_CHANNELS                    (GP_HAL_NR_OF_ADVERTISING_CHANNELS + GP_HAL_NR_OF_DATA_CHANNELS)
/** @macro GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES */
#define GP_HAL_BLE_MAX_NR_OF_WHITELIST_ENTRIES       GP_DIVERSITY_BLE_MAX_NR_OF_WHITELIST_ENTRIES
/** @macro GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES */
#define GP_HAL_BLE_MAX_NR_OF_WL_CONTROLLER_SPECIFIC_ENTRIES     3
/** @macro GP_HAL_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES */
#define GP_HAL_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES     GP_DIVERSITY_BLE_MAX_NR_OF_RESOLVINGLIST_ENTRIES
/** @macro GP_HAL_BLE_CHANNEL_MAP_INVALID */
#define GP_HAL_BLE_CHANNEL_MAP_INVALID               0xFF
/** @macro GPHAL_BLE_PREAMBLE_MATCHING_INDEX_INVALID */
#define GPHAL_BLE_PREAMBLE_MATCHING_INDEX_INVALID     32
/** @macro GPHAL_BLE_DIRECT_TEST_MODE_SYNCWORD */
#define GPHAL_BLE_DIRECT_TEST_MODE_SYNCWORD          0x71764129
/** @macro GPHAL_BLE_BANDWIDTH_CONTROL_DISABLED */
#define GPHAL_BLE_BANDWIDTH_CONTROL_DISABLED         0xFFFFFFFF
/** @macro GPHAL_BLE_BANDWIDTH_CONTROL_ENABLED */
#define GPHAL_BLE_BANDWIDTH_CONTROL_ENABLED          0x00000000
/** @macro GPHAL_BLE_DF_MAX_NUM_MEASUREMENTS */
/** @brief 45 = 8 + (160-4-8)/4 */
#define GPHAL_BLE_DF_MAX_NUM_MEASUREMENTS            45
/** @macro GPHAL_BLE_DF_NUM_WORDS_PER_DEINTERLEAVED_SLOT */
#define GPHAL_BLE_DF_NUM_WORDS_PER_DEINTERLEAVED_SLOT     4
/** @macro GPHAL_BLE_DF_SAMPLE_RATE */
/** @brief Value in MHz */
#define GPHAL_BLE_DF_SAMPLE_RATE                     192
/** @macro GPHAL_BLE_EXT_INIT_INFO_NR_OF_PHYS */
#define GPHAL_BLE_EXT_INIT_INFO_NR_OF_PHYS           2
/** @macro GPHAL_BLE_EXT_INIT_INFO_HDR_IDX */
#define GPHAL_BLE_EXT_INIT_INFO_HDR_IDX              0
/** @macro GPHAL_BLE_EXT_INIT_INFO_LR_IDX */
#define GPHAL_BLE_EXT_INIT_INFO_LR_IDX               1
/** @macro GPHAL_BLE_PHY_MASK_1MB */
#define GPHAL_BLE_PHY_MASK_1MB                       0x01
/** @macro GPHAL_BLE_PHY_MASK_2MB */
#define GPHAL_BLE_PHY_MASK_2MB                       0x02
/** @macro GPHAL_BLE_PHY_MASK_CODED */
#define GPHAL_BLE_PHY_MASK_CODED                     0x04
/** @macro GPHAL_BLE_VIRTUAL_CONN_MASK */
#define GPHAL_BLE_VIRTUAL_CONN_MASK                  0x40
/** @macro GP_HAL_BLE_ANTENNASWITCHINGDISABLED */
#define GP_HAL_BLE_ANTENNASWITCHINGDISABLED          0x0
/** @macro GP_HAL_BLE_ANTENNASWITCHINGENABLED */
#define GP_HAL_BLE_ANTENNASWITCHINGENABLED           0x1
/* <CodeGenerator Placeholder> AdditionalMacroDefinitions */



#define GP_BLE_MAX_NUMBER_PERASC 0

#define GP_BLE_MAX_NUMBER_SYNCC 0

// By default, the scan asc have a low priority, only background scanning has a lower priority (0).
#define GP_HAL_BLE_SCAN_ASC_DEFAULT_PRIORITY        1

/* </CodeGenerator Placeholder> AdditionalMacroDefinitions */
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 gpHal_BleChannelMapHandle_t;

/** @pointer to function gpHal_BleSubEvDone_t
 *  @brief The gpBle_PreSchedActCb_t callback type definition defines the callback for a trigger type.
 *  @param index
 *  @param handle
 */
typedef void (*gpHal_BleSubEvDone_t) (UInt8 index, UInt8 handle);

/** @struct gpHal_AdvEventInfo_t */
typedef struct {
    gpPd_Loh_t                     pdLohAdv;
    gpPd_Loh_t                     pdLohScan;
    UInt32                         interval;
    UInt8                          priority;
    Bool                           enableExtPriority;
    UInt8                          advDelayMax;
/* <CodeGenerator Placeholder> imp_gpHal_AdvEventInfo_t_channelMap */
    UInt8                          channelMap[GP_HAL_NR_OF_ADVERTISING_CHANNELS];
/* </CodeGenerator Placeholder> imp_gpHal_AdvEventInfo_t_channelMap */
    UInt8                          frameTypeAcceptMask;
    UInt8                          whitelistEnableMask;
    Bool                           forwardResPrivSrc;
    Bool                           forwardResPrivDst;
} gpHal_AdvEventInfo_t;

/** @struct gpHal_phyMask_t */
typedef struct {
    /** @brief bit 0 = 1 mbit PHY, bit 1 = 2 mbit PHY, bit 2 = coded PHY */
    UInt8                          mask;
} gpHal_phyMask_t;

/** @struct gpHal_ScanEventInfo_t */
typedef struct {
    UInt8                          priority;
    Bool                           enableExtPriority;
    UInt32                         interval;
    UInt32                         scanDuration;
    gpPd_Loh_t                     pdLoh;
    UInt8                          channelMapIndex;
/* <CodeGenerator Placeholder> imp_gpHal_ScanEventInfo_t_channelMap */
    UInt8                          channelMap[GP_HAL_NR_OF_ADVERTISING_CHANNELS];
/* </CodeGenerator Placeholder> imp_gpHal_ScanEventInfo_t_channelMap */
    BtDeviceAddress_t              ownAddress;
    UInt8                          ownAddressType;
    Bool                           activeScanning;
    UInt8                          frameTypeAcceptMask;
    UInt8                          whitelistEnableMask;
    UInt8                          scanBackoffCount;
    UInt8                          upperLimitMask;
    UInt8                          successCount;
    UInt8                          failureCount;
    Bool                           acceptUnresolvedRpaDst;
    Bool                           generateRpa;
    gpHal_phyMask_t                phyMask;
} gpHal_ScanEventInfo_t;

/** @struct gpHal_ExtInitEventInfo_t */
typedef struct {
    UInt8                          virtualConnId;
    UInt16                         latency;
    UInt16                         timeout;
} gpHal_ExtInitEventInfo_t;

/** @struct gpHal_InitEventInfo_t */
typedef struct {
    UInt8                          priority;
    Bool                           enableExtPriority;
    UInt32                         interval;
    UInt32                         initWindowDuration;
    gpPd_Loh_t                     pdLohConn;
    UInt8                          channelMapIndex;
/* <CodeGenerator Placeholder> imp_gpHal_InitEventInfo_t_channelMap */
    UInt8                          channelMap[GP_HAL_NR_OF_ADVERTISING_CHANNELS];
/* </CodeGenerator Placeholder> imp_gpHal_InitEventInfo_t_channelMap */
    UInt8                          frameTypeAcceptMask;
    UInt8                          whitelistEnableMask;
    BtDeviceAddress_t              ownAddress;
    UInt8                          ownAddressType;
    UInt8                          virtualConnId;
    BtDeviceAddress_t              connDstAddress;
    Bool                           generateRpa;
    gpHal_phyMask_t                initPhyMask;
#if defined(GP_DIVERSITY_JUMPTABLES)
/* <CodeGenerator Placeholder> imp_gpHal_InitEventInfo_t_extInitEventInfo */
    gpHal_ExtInitEventInfo_t       extInitEventInfo[GPHAL_BLE_EXT_INIT_INFO_NR_OF_PHYS];
/* </CodeGenerator Placeholder> imp_gpHal_InitEventInfo_t_extInitEventInfo */
#endif //defined(GP_HAL_DIVERSITY_EXTENDED_INITIATING) || defined(GP_DIVERSITY_JUMPTABLES)
} gpHal_InitEventInfo_t;

/** @struct gpHal_ConnEventInfo_t */
typedef struct {
    UInt8                          priority;
    Bool                           enableExtPriority;
    Bool                           suspendEvent;
    UInt32                         interval;
    UInt8                          channelId;
    UInt8                          hopIncrement;
    UInt8                          channelMapHandle;
    UInt32                         accessAddress;
    UInt32                         crcInit;
    UInt32                         windowDuration;
    /** @brief fixedWDThreshold; the number of missed RX Events to use maxRx-Window; slave side  */
    UInt16                         fixedWDThreshold;
    UInt16                         masterSca;
    UInt16                         slaveLatency;
    UInt16                         eventCounter;
    UInt8                          unmappedChannelPtr;
    UInt8                          txSeqNr;
    UInt8                          txNextSeqNr;
    UInt8                          rxSeqNr;
    UInt8                          rxNextSeqNr;
    UInt16                         lastConnEventCount;
    Bool                           lastConnEventCountValid;
    Bool                           rxFlowCtrlFlag;
    UInt8                          txQueueWritePtr;
    UInt8                          txQueueReadPtr;
/* <CodeGenerator Placeholder> imp_gpHal_ConnEventInfo_t_txQueue */
    UInt8                          txQueue[8];
/* </CodeGenerator Placeholder> imp_gpHal_ConnEventInfo_t_txQueue */
    UInt32                         tsLastValidPacketReceived;
    UInt32                         tsLastPacketReceived;
    UInt16                         nrOfConsecSkippedEvents;
    UInt8                          preamble;
    Bool                           winOffsetCalculated;
#if defined(GP_DIVERSITY_JUMPTABLES)
    Bool                           useChanSelAlgo2;
#endif //defined(GP_HAL_DIVERSITY_CHANNEL_SELECTION_ALGORITHM_NR2) || defined(GP_DIVERSITY_JUMPTABLES)
    gpHal_BleTxPhy_t               phy;
} gpHal_ConnEventInfo_t;

/** @union gpHal_BleRxTxPhy_t */
typedef union {
    gpHal_BleTxPhy_t               txPhy;
    gpHal_phyMask_t                rxPhyMask;
} gpHal_BleRxTxPhy_t;

/** @struct gpHal_TestInfo_t */
typedef struct {
    Bool                           tx;
    UInt8                          channel;
    gpHal_BleRxTxPhy_t             phy;
    gpHal_AntennaSelection_t       antenna;
    UInt32                         accesscode;
    UInt16                         interval;
    gpPd_Loh_t                     pdLoh;
    UInt16                         txPacketCount;
    Bool                           forwardRxPdus;
    UInt8                          cteLengthUnit;
    UInt8                          cteType;
    UInt8                          switchingPatternLength;
    UInt8*                         pAntennaIDs;
} gpHal_TestInfo_t;

/** @struct gpHal_BleTestModeEnd_t */
typedef struct {
    gpPd_Handle_t                  pdHandle;
    UInt16                         nrOfPackets;
} gpHal_BleTestModeEnd_t;

/** @struct gpHal_BlePhyUpdateInfo_t */
typedef struct {
    gpHal_BleTxPhy_t               phyIdTx;
    gpHal_BleRxPhy_t               phyIdRx;
    UInt8                          preamble;
} gpHal_BlePhyUpdateInfo_t;

/** @struct gpHal_ConnEventMetrics_t */
typedef struct {
    UInt32                         windowDuration;
    UInt32                         tsLastValidPacketReceived;
    UInt32                         tsLastPacketReceived;
    UInt32                         nextAnchorTime;
    UInt32                         anchorTimeLastRxEvent;
    UInt16                         nrOfConsecSkippedEvents;
    UInt16                         eventCounterNext;
    UInt16                         eventCounterLastRx;
    UInt16                         nrNoRXEvents;
} gpHal_ConnEventMetrics_t;

/** @struct gpHal_UpdateConnEventInfo_t */
typedef struct {
    UInt32                         interval;
    UInt32                         windowDuration;
    UInt16                         latency;
    UInt16                         eventCount;
    UInt8                          unmappedChannelPtr;
    UInt32                         tsLastValidPacketReceived;
    UInt16                         nrNoRXEvents;
} gpHal_UpdateConnEventInfo_t;

/** @struct gpHal_ChannelMap_t */
typedef struct {
/* <CodeGenerator Placeholder> imp_gpHal_ChannelMap_t_usedChanIds */
    UInt8                          usedChanIds[GP_HAL_CHANNEL_MAP_LENGTH];
/* </CodeGenerator Placeholder> imp_gpHal_ChannelMap_t_usedChanIds */
/* <CodeGenerator Placeholder> imp_gpHal_ChannelMap_t_remapTable */
    UInt8                          remapTable[GP_HAL_NR_OF_DATA_CHANNELS];
/* </CodeGenerator Placeholder> imp_gpHal_ChannelMap_t_remapTable */
    UInt8                          hopRemapTableLength;
} gpHal_ChannelMap_t;

/** @struct gpHal_AdvASC_t */
typedef struct {
    UInt8                          priority;
    Bool                           enableExtPriority;
    Bool                           suspendEvent;
/* <CodeGenerator Placeholder> imp_gpHal_AdvASC_t_channelMap */
    UInt8                          channelMap[5];
/* </CodeGenerator Placeholder> imp_gpHal_AdvASC_t_channelMap */
    UInt8                          channelMapSize;
    UInt8                          randomFlags;
    BtDeviceAddress_t              ownAddress;
    UInt8                          ownAddressFlags;
    BtDeviceAddress_t              peerAddress;
    UInt16                         peerAddressType;
    UInt8                          pbmHandle;
    Bool                           pbmTxIfTooLate;
    UInt16                         auxPtrOffset;
    UInt8                          advertisingHandle;
    UInt16                         frameTypeAcceptMask;
    UInt16                         whiteListEnableMask;
    /** @brief IntraTxTime in units of 300usec */
    UInt16                         intraTxTime;
    gpHal_BleAdvASCFlags_t         contextFlags;
} gpHal_AdvASC_t;

/** @struct gpHal_ScanASC_t */
typedef struct {
    UInt8                          priority;
    Bool                           enableExtPriority;
    Bool                           suspendEvent;
    gpHal_BleScanASCFlags_t        flags;
} gpHal_ScanASC_t;

/** @struct gpHal_PerASC_t */
typedef struct {
    UInt8                          priority;
    Bool                           enableExtPriority;
    Bool                           suspendEvent;
    UInt32                         accessAddress;
    UInt32                         crcInit;
    gpHal_BleChannelMapHandle_t    channelMapHandle;
} gpHal_PerASC_t;



/** @struct gpHal_BleSubEvDoneEntry_t */
typedef struct {
    gpHal_BleSubEvDone_t           cbSubEvDone;
} gpHal_BleSubEvDoneEntry_t;

/** @struct gpHal_WhiteListEntry_t */
typedef struct {
    UInt8                          stateMask;
    Bool                           addressType;
    BtDeviceAddress_t              address;
} gpHal_WhiteListEntry_t;

/** @struct gpHal_BleTxOptions_t */
typedef struct {
    UInt8                          supplementalLengthUs;
} gpHal_BleTxOptions_t;

#if defined(GP_DIVERSITY_JUMPTABLES)
/** @struct gpHal_BleRpaHandle_t
 *  @brief Handle to an entry in the resolving list
 */
typedef struct {
    UInt8                          idx;
    Bool                           idx_is_valid;
} gpHal_BleRpaHandle_t;

#endif //defined(GP_HAL_DIVERSITY_BLE_RPA) || defined(GP_DIVERSITY_JUMPTABLES)
#if defined(GP_DIVERSITY_JUMPTABLES)
/** @struct gpHal_BleRpaInfo_t
 *  @brief Object that contains Resolvable Private Address information
 */
typedef struct {
    gpHal_BleRpaHandle_t           rpaHandle;
    /** @brief True if the src address is an RPA that was generated or resolved using the list entry corresponding to rpaHandle. If false the src address can still be an RPA but it was not resolved or generated by the link layer! */
    Bool                           srcAddrIsLLRPA;
    /** @brief True if the dst address is an RPA that was generated or resolved using the list entry corresponding to rpaHandle. If false the dst address can still be an RPA but it was not resolved or generated by the link layer! */
    Bool                           dstAddrIsLLRPA;
} gpHal_BleRpaInfo_t;

#endif //defined(GP_HAL_DIVERSITY_BLE_RPA) || defined(GP_DIVERSITY_JUMPTABLES)
/** @struct gpHal_BleSlaveCreateConnInfo_t
 *  @brief Object that contains additional information about the slave connection to create.
 */
typedef struct {
    gpPd_Loh_t                     pdLoh;
} gpHal_BleSlaveCreateConnInfo_t;

/** @struct gpHal_BleMasterCreateConnInfo_t
 *  @brief Object that contains additional information about the master connection to create.
 */
typedef struct {
    gpPd_Loh_t                     pdLoh;
} gpHal_BleMasterCreateConnInfo_t;

/** @struct gpHal_BleAdvIndInfo_t
 *  @brief Object that contains additional information about the adv indication.
 */
typedef struct {
    gpPd_Loh_t                     pdLoh;
} gpHal_BleAdvIndInfo_t;

/** @struct gpHal_BleConnRspIndInfo_t
 *  @brief Object that contains additional information about the connection response indication.
 */
typedef struct {
    gpPd_Loh_t                     pdLoh;
} gpHal_BleConnRspIndInfo_t;

/** @pointer to function gpHal_BleAdvIndCallback_t
 *  @brief The gpHal_BleAdvIndCallback_t callback type definition defines the callback prototype of the Ble advertising indication.
 *  @param advIndInfo
 */
typedef void (*gpHal_BleAdvIndCallback_t) (gpHal_BleAdvIndInfo_t* advIndInfo);

/** @pointer to function gpHal_BleSlaveCreateConnCallback_t
 *  @brief The gpHal_BleSlaveCreateConnCallback_t callback type definition defines the callback prototype of the Ble slave connection.
 *  @param slaveCreateConnInfo
 */
typedef void (*gpHal_BleSlaveCreateConnCallback_t) (gpHal_BleSlaveCreateConnInfo_t* slaveCreateConnInfo);

/** @pointer to function gpHal_BleDataIndCallback_t
 *  @brief The gpHal_BleDataIndCallback_t callback type definition defines the callback prototype of the Ble data indication.
 *  @param connId
 *  @param pdLoh
 */
typedef void (*gpHal_BleDataIndCallback_t) (UInt8 connId, gpPd_Loh_t pdLoh);

/** @pointer to function gpHal_BleDataConfCallback_t
 *  @brief The gpHal_BleDataConfCallback_t callback type definition defines the callback prototype of the Ble data confirm.
 *  @param connId
 *  @param pdLoh
 */
typedef void (*gpHal_BleDataConfCallback_t) (UInt8 connId, gpPd_Loh_t pdLoh);

/** @pointer to function gpHal_BleAdvEventDoneCallback_t
 *  @brief The gpHal_BleAdvEventDoneCallback_t callback type definition defines the callback prototype of the Adv event done confirm.
 *  @param
 */
typedef void (*gpHal_BleAdvEventDoneCallback_t) (void );

/** @pointer to function gpHal_BleScanEventDoneCallback_t
 *  @brief The gpHal_BleScanEventDoneCallback_t callback type definition defines the callback prototype of the Scan event done confirm.
 *  @param
 */
typedef void (*gpHal_BleScanEventDoneCallback_t) (void );

/** @pointer to function gpHal_BleInitEventDoneCallback_t
 *  @brief The gpHal_BleInitEventDoneCallback_t callback type definition defines the callback prototype of the Init event done confirm.
 *  @param
 */
typedef void (*gpHal_BleInitEventDoneCallback_t) (void );

/** @pointer to function gpHal_BleConnEventDoneCallback_t
 *  @brief The gpHal_BleConnEventDoneCallback_t callback type definition defines the callback prototype of the Connection event done confirm.
 *  @param connId
 */
typedef void (*gpHal_BleConnEventDoneCallback_t) (UInt8 connId);

/** @pointer to function gpHal_BleMasterCreateConnCallback_t
 *  @brief The gpHal_BleMasterCreateConnCallback_t callback type definition defines the callback prototype of the master create connection.
 *  @param masterCreateConnInfo
 */
typedef void (*gpHal_BleMasterCreateConnCallback_t) (gpHal_BleMasterCreateConnInfo_t* masterCreateConnInfo);

/** @pointer to function gpHal_BleLastSchedEventPassedCallback_t
 *  @brief The gpHal_BleLastSchedEventPassedCallback_t callback type definition defines the callback prototype of the instant passed primitive.
 *  @param connId
 */
typedef void (*gpHal_BleLastSchedEventPassedCallback_t) (UInt8 connId);

/** @pointer to function gpHal_BlePurgeConfCallback_t
 *  @brief The gpHal_BlePurgeConfCallback_t callback type definition defines the callback prototype of the (unsolicited) Ble purge confirm.
 *  @param connId
 *  @param handle
 */
typedef void (*gpHal_BlePurgeConfCallback_t) (UInt8 connId, UInt8 handle);

/** @pointer to function gpHal_BleConnRspIndCallback_t
 *  @brief The gpHal_BleConnRspIndCallback_t callback type definition defines the callback prototype of the connect Response Indication.
 *  @param connRspIndInfo
 */
typedef void (*gpHal_BleConnRspIndCallback_t) (gpHal_BleConnRspIndInfo_t* connRspIndInfo);

/** @pointer to function gpHal_BleTestModeDataIndCallback_t
 *  @brief The gpHal_BleTestModeDataIndCallback_t callback type definition defines the callback prototype of the Ble test mode data indication.
 *  @param pdLoh
 */
typedef void (*gpHal_BleTestModeDataIndCallback_t) (gpPd_Loh_t pdLoh);

/** @pointer to function gpHal_BleTestModeCteMeasurementIndCallback_t
 *  @brief The gpHal_BleTestModeCteMeasurementIndCallback_t callback type definition defines the callback prototype of test mode packets with CTE measurements
 */
typedef void (*gpHal_BleTestModeCteMeasurementIndCallback_t) (void);

/** @pointer to function gpHal_BleScanReqIndCallback_t
 *  @brief The gpHal_BleScanReqIndCallback_t callback type definition defines the callback prototype of the (aux) scan req indication.
 *  @param gpHal_BleAdvIndInfo_t*
 */
typedef void (*gpHal_BleScanReqIndCallback_t) (gpHal_BleAdvIndInfo_t* advIndInfo);

/** @pointer to function gpHal_BleIsoRxCallback_t
 *  @brief The gpHal_BleIsoRxCallback_t callback type definition defines the callback prototype of the BLE Iso Rx indication.
 */
typedef void (*gpHal_BleIsoRxCallback_t) (void);

/** @struct gpHal_BleCallbacks_t */
typedef struct {
    gpHal_BleAdvIndCallback_t      cbAdvInd;
    gpHal_BleSlaveCreateConnCallback_t cbSlaveCreateConn;
    gpHal_BleAdvEventDoneCallback_t cbAdvEvDone;
    gpHal_BleScanEventDoneCallback_t cbScanEvDone;
    gpHal_BleInitEventDoneCallback_t cbInitEvDone;
    gpHal_BleMasterCreateConnCallback_t cbMasterCreateConn;
    gpHal_BleConnEventDoneCallback_t cbConnEventDone;
    gpHal_BleDataIndCallback_t     cbDataInd;
    gpHal_BleDataConfCallback_t    cbDataConf;
    gpHal_BleLastSchedEventPassedCallback_t cbLastSchedEventPassed;
    gpHal_BlePurgeConfCallback_t   cbPurgeConf;
    gpHal_BleTestModeDataIndCallback_t cbTestModeDataInd;
    gpHal_BleTestModeCteMeasurementIndCallback_t cbCteMeasurementInd;
    gpHal_BleScanReqIndCallback_t  cbScanReqInd;
} gpHal_BleCallbacks_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpHal_CodeJumpTableFlash_Defs_Ble.h"
#include "gpHal_CodeJumpTableRom_Defs_Ble.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

//Requests
void gpHal_InitBle(void);

gpHal_Result_t gpHal_BleStartAdvertising(UInt32 firstAdvTs, gpHal_AdvEventInfo_t* pAdvEventInfo);


gpHal_Result_t gpHal_BleStopInitScanning(gpPd_Loh_t* pConnectReqPdu);

gpHal_Result_t gpHal_BleRestartInitScanning(UInt32 firstInitScanTs, gpPd_Loh_t ConnectReqPdu);

void gpHal_BleRegisterCallbacks(gpHal_BleCallbacks_t* pCallbacks);

void gpHal_BleSetChannelMap(gpHal_BleChannelMapHandle_t channelMapHandle, gpHal_ChannelMap_t* pChanMap);

void gpHal_BleGetChannelMap(gpHal_BleChannelMapHandle_t handle, gpHal_ChannelMap_t* pChanMap);

void gpHal_BleGetDeviceAddress(BtDeviceAddress_t* pAddress);

gpHal_Result_t gpHal_BleStartInitScanning(UInt32 firstInitScanTs, gpHal_InitEventInfo_t* pInitEventInfo);

gpHal_Result_t gpHal_BleStartConnection(UInt8 connId, UInt32 firstConnTs, gpHal_ConnEventInfo_t* pConnEventInfo);

void gpHal_BleSetDeviceAddress(BtDeviceAddress_t* pAddress);

/** @brief in flash jump table and in rom jump table
 *
 *  @param pEntry
 *  @param rangeStart
 *  @param rangeStop
 *  @return index
 */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
UInt8 gpHal_BleGetWhiteListEntryIndex(gpHal_WhiteListEntry_t* pEntry, UInt8 rangeStart, UInt8 rangeStop);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

/** @brief in flash jump table and in rom jump table
 *
 *  @param pEntry
 *  @param rangeStart
 *  @param rangeStop
 *  @return result
 */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */
gpHal_Result_t gpHal_BleRemoveDeviceFromWhiteList(gpHal_WhiteListEntry_t* pEntry, UInt8 rangeStart, UInt8 rangeStop);
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

/** @brief in flash jump table
 *
 *  @param id
 *  @param state
 *  @param set
 *  @return result
 */
gpHal_Result_t gpHal_UpdateWhiteListEntryState(UInt8 id, UInt8 state, Bool set);

/** @brief in flash jump table
 *
 *  @param id
 *  @return valid
 */
Bool gpHal_BleIsWhiteListEntryValid(UInt8 id);

/** @brief in flash jump table
 *
 *  @param pEntry
 *  @param rangeStart
 *  @param rangeStop
 *  @return result
 */
gpHal_Result_t gpHal_BleAddDeviceToWhiteList(gpHal_WhiteListEntry_t* pEntry, UInt8 rangeStart, UInt8 rangeStop);

/** @brief in flash jump table
 *
 *  @param rangeStart
 *  @param rangeStop
 */
void gpHal_BleClearWhiteList(UInt8 rangeStart, UInt8 rangeStop);

/** @brief in flash jump table
 *
 *  @param id
 *  @return state
 */
UInt8 gpHal_BleGetWhitelistEntryState(UInt8 id);

/** @brief in flash jump table
 *
 *  @param addressType
 *  @param pAddress
 *  @param start
 *  @param stop
 *  @return result
 */
UInt8 gpHal_BleFindWhiteListEntry(UInt8 addressType, BtDeviceAddress_t* pAddress, UInt8 start, UInt8 stop);

/** @brief in flash jump table
 *
 *  @param addressType
 *  @param pAddress
 *  @param stateMask_mask
 *  @param stateMask_value
 *  @param createIfNotExist
 *  @return result
 */
gpHal_Result_t gpHal_BleUpdateWhiteListEntryState(UInt8 addressType, BtDeviceAddress_t* pAddress, UInt8 stateMask_mask, UInt8 stateMask_value, Bool createIfNotExist);

/** @brief in flash jump table
 *
 *  @param matchMask
 *  @param stateMask_mask
 *  @param stateMask_value
 */
void gpHal_BleUpdateWhiteListEntryStateBulk(UInt8 matchMask, UInt8 stateMask_mask, UInt8 stateMask_value);

/** @brief in flash jump table
 *
 *  @param id
 *  @param pEntry
 *  @return result
 */
gpHal_Result_t gpHal_BleGetWhitelistEntry(UInt8 id, gpHal_WhiteListEntry_t* pEntry);

void gpHal_EnableMasterCreateConnInterrupts(Bool enable);

void gpHal_EnableSlaveCreateConnInterrupts(Bool enable);

void gpHal_EnableAdvIndInterrupts(Bool enable);

void gpHal_EnableDataIndInterrupts(Bool enable);

void gpHal_EnableDataConfInterrupts(Bool enable);

gpHal_Result_t gpHal_BleRestartAdvertising(UInt32 advTs);

gpHal_Result_t gpHal_BleStopAdvertising(gpPd_Loh_t* pdLohAdv, gpPd_Loh_t* pdLohScan);

gpHal_Result_t gpHal_BleStartScanning(UInt32 firstScanTs, gpHal_ScanEventInfo_t* pScanEventInfo);

gpHal_Result_t gpHal_BleStopScanning(gpPd_Loh_t* pdLohScan);

gpHal_Result_t gpHal_BleStartVirtualConnection(UInt8 connId, UInt32 firstConnTs, UInt32 interval);

gpHal_Result_t gpHal_BleStopVirtualConnection(UInt8 connId);

gpHal_Result_t gpHal_UpdateConnection(UInt8 connId, UInt32 firstConnTs, gpHal_UpdateConnEventInfo_t* pUpdate);

gpHal_Result_t gpHal_BleEstablishMasterConnection(UInt8 virtualConnId, UInt8 masterConnId, UInt32* nextEventTs);

gpHal_Result_t gpHal_BleUpdateMasterConnection(UInt8 connId, UInt32 firstConnTs, gpHal_ConnEventInfo_t* pConnEventInfo);

gpHal_Result_t gpHal_BleUpdateChannelMap(UInt8 connId, gpHal_BleChannelMapHandle_t newChannelMapHandle);

gpHal_Result_t gpHal_BleUpdatePhy(UInt8 connId, gpHal_BlePhyUpdateInfo_t* pInfo);

gpHal_Result_t gpHal_BleSetLastScheduledConnEventCount(UInt8 connId, UInt16 lastEventCount);

gpHal_Result_t gpHal_BleStopLastScheduledConnEventCount(UInt8 connId);

UInt16 gpHal_BleGetCurrentConnEventCount(UInt8 connId);

gpHal_Result_t gpHal_BleStopConnection(UInt8 connId);

gpHal_Result_t gpHal_BlePauseConnectionEvent(UInt8 connId);

gpHal_Result_t gpHal_BleSetFlowCtrl(UInt16 connMask);

gpHal_BleChannelMapHandle_t gpHal_BleAllocateChannelMapHandle(void);

Bool gpHal_BleIsChannelMapValid(gpHal_BleChannelMapHandle_t channelMap);

void gpHal_BleFreeChannelMapHandle(gpHal_BleChannelMapHandle_t handle);

gpHal_Result_t gpHal_BleAddPduToQueue(UInt8 connHandle, gpPd_Loh_t pdLoh, gpHal_BleTxOptions_t* pTxOptions);

Bool gpHal_BleIsTxQueueEmpty(UInt8 connId);

UInt32 gpHal_BleGetLastRxTimestamp(UInt8 connId);

gpHal_Result_t gpHal_BleGetNextExpectedEventTimestamp(UInt8 connId, UInt32* ts);

gpHal_Result_t gpHal_BleGetUnmappedChannelPtr(UInt8 connId, UInt8* unmappedChannelPtr);

UInt8 gpHal_BleGetNrOfAvailableLinkQueueEntries(UInt8 connId);

gpHal_Result_t gpHal_BleSetSlaveLatency(UInt8 connId, UInt16 slaveLatency);

gpHal_Result_t gpHal_BleStartTestMode(gpHal_TestInfo_t* pInfo);

gpHal_Result_t gpHal_BleTestMode_EnableSampleCollection(void);

gpHal_Result_t gpHal_BleTestModeGetPhaseSamplesBuffer(UInt32* pSamples);

gpHal_Result_t gpHal_BleEndTestMode(gpHal_BleTestModeEnd_t* pInfo);

Bool gpHal_BleTestModeIsActive(void);

gpHal_Result_t gpHal_BleIsAccessAddressAcceptable(UInt32 accessAddress);

gpHal_Result_t gpHal_SetConnectionPriority(UInt8 connId, UInt8 priority);

gpHal_Result_t gpHal_SetConnectionGuardTime(UInt8 connId, UInt32 maxDurationPacketPair, UInt32 extraIdleTime, UInt16 slaveLatency);

gpHal_Result_t gpHal_GetConnectionGuardTime(UInt8 connId, UInt32* pGuard);

UInt16 gpHal_BleGetMaxQueuedPacketLength(UInt8 connId);

gpHal_Result_t gpHal_BleGetConnectionMetrics(UInt8 connId, gpHal_ConnEventMetrics_t* pMetrics);


Int8 gpHal_BleGetTxPower(void);

gpHal_Result_t gpHal_BleSetTxPower(Int8 txPower);

gpHal_Result_t gpHal_SetSuspendConnectionEvent(UInt8 connId, Bool suspend);





Bool gpHal_BleHasFullBgScanSupport(void);

void gpHal_BleSetMaxRxPayloadLength(UInt8 payloadLength);


gpHal_Result_t gpHal_BleSetRfPathCompensation(Int8 newTxPathGaindBm, Int8 newRxPathGaindBm);

void gpHal_BleGetRfPathCompensation(Int8* pTxPathGaindBm, Int8* pRxPathGaindBm);

void gpHal_BleGetMinMaxPowerLevels(Int8* minTxPower, Int8* maxTxPower);































UInt8 gpHal_BleGetBleChannelIndex(UInt8 pbmHandle);

UInt16 gpHal_BleCalcWaveLengthInmm(UInt8 bleChannelIdx);










void gpHal_BleTestDisableBleMgr(void);

void gpHal_BleTestReEnableBleMgr(void);

void gpHal_BleTestSetChannel(UInt8 channel);



















Int8 gpHal_BleGetNearestSupportedTxPower(Int8 requested_txPower_dBm_at_Antenna);



/** @brief Set MultiStandardListening mode on/off for BLE
 * If multistandard listening is enabled, a simultaneous ZB rx and BLE scan is done by the radio
 * Note that this is not compatible with certain ZB rx modes
 *
 *  @param enable                    Enables (or disables) the multistandard listening mode.
 *  @return result                   Possible results are :
 *                                    - gpHal_ResultSuccess
 *                                    - gpHal_ResultInvalidParameter: (invalid combination was selected)
 */
gpHal_Result_t gpHal_BleSetMultiStandard(Bool enable);

/** @brief Get MultiStandardListening mode for BLE
 *  @return isEnabled                Bool indicating if multistandard listening mode.for BLE is enabled or not
 */
Bool gpHal_BleGetMultiStandard(void);

/** @brief Get slow antenna switching enabled for BLE
 *  @return isEnabled                Indicates if slow antenna switching for BLE is enabled
 */
UInt8 gpHal_Ble_GetAdvAntennaSwitching(void);

/** @brief Get the (initial) internal antenna Id for BLE advertising
 *  @return antenna_Id               internal antenna Id
 */
UInt8 gpHal_Ble_GetAdvAntenna(void);


Bool gpHal_BleIsChanSelAlgo2Used(UInt8 connId);

UInt8 gpHal_BleGetHopIncrement(UInt8 connId);

//Indications
void gpHal_cbBleAdvInd(gpHal_BleAdvIndInfo_t* advIndInfo);

void gpHal_cbBleScanEventDone(void);

void gpHal_cbBleLastScheduledConnEventCount(UInt8 connId);


void gpHal_cbBleMasterCreateConn(gpHal_BleMasterCreateConnInfo_t* masterCreateConnInfo);

// MANUAL The below function is listed in gphal_Ble.xml, but the signature is in <platform>/gpHal_DEFS.h
// void gpHal_cbBleDataConfirm(UInt8 connId, gpPd_Loh_t pdLoh);

void gpHal_cbBleDataInd(UInt8 connId, gpPd_Loh_t pdLoh);

void gpHal_cbBleSlaveCreateConn(gpHal_BleSlaveCreateConnInfo_t* slaveCreateConnInfo);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

void gpHal_BleAlwaysEnablePrecalibration(Bool enable);

UInt8 gpHal_BleGetRtBleMgrVersion(void);

void gpHal_cbCigEventProcessed(UInt8 eventId);

/**
 * @brief Compensates the sleep clock accuracy
 *
 * This function compensates the central and peripheral role combined average and worst case sleep clock accuracy.
 *
 * @param connId Connection identification index
 * @param combinedSca Peripheral and central role  combined SCA in ppm
 * @param centralSca Central role SCA in ppm
*/
void gpHal_CompensateSleepClockAccuracy(UInt8 connId, UInt16 combinedSca, UInt16 centralSca);

#ifdef __cplusplus
}
#endif //__cplusplus

// MANUAL
#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif //_GPHAL_BLE_H_

