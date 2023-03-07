/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpBle_defs.h
 *   This file contains the internal defines, typedefs,... of the BLE layer.
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GP_BLE_DEFS_H_
#define _GP_BLE_DEFS_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "gpBle_RomCode_defs.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHci_Includes.h"
#include "gpHal.h"
#include "gpPd.h"
#include "gpBle.h"
#include "gpBle_Connections.h"

// #ifdef GP_DIVERSITY_BLE_EXT_ADVERTISING_FEATURE_PRESENT
#include "gpBle_ExtAdv.h"
// #endif //GP_DIVERSITY_BLE_EXT_ADVERTISING_FEATURE_PRESENT


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define BLE_1MBIT_FACTOR                        3
#define BLE_2MBIT_FACTOR                        2
#define BLE_500KBIT_FACTOR                      4
#define BLE_125KBIT_FACTOR                      6

// bit rate = 1 Mbit/s ==> 8 us per byte, 2 Mbit/s ==> 4 us per byte
// Inter Frame Space: time interval between 2 consecutive packets on the same channel
#define T_IFS                           (150 /* us */)

// Security related defines
#define BLE_SEC_MIC_LENGTH          4

// fixme: distinguish between debug and release
#define BLE_RANGE_CHECK(value, min, max)    ((value >= min) && (value <= max))

#define BLE_ACCESS_ADDRESS_SIZE                         4
#define BLE_CRC_SIZE                                    3

// | LLID | NESN | SN | MD | Length
#define BLE_PACKET_HEADER_SIZE                          2
// CTE info is considered part of header, but easier to treat separately
#define BLE_CTE_INFO_SIZE                               1

#define BLE_ACCESS_ADDRESS_ADVERTISING_CHANNEL_PACKET   0x8E89BED6UL

#define BLE_LL_TIME_UNIT_US                 625

// Scan interval defines
#define BLE_SCAN_INTERVAL_RANGE_MIN                 0x0004
#define BLE_SCAN_INTERVAL_RANGE_MAX                 0x4000
#define BLE_SCAN_INTERVAL_INVALID                   0xFFFF
#define BLE_SCAN_WINDOW_RANGE_MIN                   0x0004
#define BLE_SCAN_WINDOW_RANGE_MAX                   0x4000
#define BLE_SCAN_WINDOW_INVALID                     0xFFFF

// Initiator interval defines
#define BLE_INITIATOR_CONN_INTERVAL_RANGE_MIN       0x0006
#define BLE_INITIATOR_CONN_INTERVAL_RANGE_MAX       0x0C80
#define BLE_CONN_INTERVAL_INVALID                   0xFFFF
#define BLE_INITIATOR_CONN_LATENCY_RANGE_MIN        0x0000
#define BLE_INITIATOR_CONN_LATENCY_RANGE_MAX        0x01F3
#define BLE_INITIATOR_CONN_LATENCY_INVALID          0xFFFF

#define BLE_INITIATOR_SUPERVISION_TO_RANGE_MIN      0x000A
#define BLE_INITIATOR_SUPERVISION_TO_RANGE_MAX      0x0C80
#define BLE_INITIATOR_SUPERVISION_TO_INVALID        0xFFFF
#define BLE_CE_LENGTH_MAX                           0xFFFF

#define BLE_INITIATOR_HOP_FIELD_MIN                 0x0005
#define BLE_INITIATOR_HOP_FIELD_MAX                 0x0010

// Advertising PDU header defines
#define BLE_ADV_PDU_HEADER_TYPE_IDX             0
#define BLE_ADV_PDU_HEADER_RFU_IDX              4
#define BLE_ADV_PDU_HEADER_CHSEL_IDX            5
#define BLE_ADV_PDU_HEADER_TXADD_IDX            6
#define BLE_ADV_PDU_HEADER_RXADD_IDX            7
#define BLE_ADV_PDU_HEADER_LENGTH_IDX           8

#define BLE_ADV_PDU_HEADER_TYPE_BM              0x000F
#define BLE_ADV_PDU_HEADER_CHSEL_BM             0x0020
#define BLE_ADV_PDU_HEADER_TXADD_BM             0x0040
#define BLE_ADV_PDU_HEADER_RXADD_BM             0x0080
#define BLE_ADV_PDU_HEADER_LENGTH_BM            0x3F00

#define BLE_EXTADV_PDU_HEADER_LENGTH_BM         0xFF00

// Data channel PDU header defines
#define BLE_DATA_PDU_HEADER_LLID_IDX            0
#define BLE_DATA_PDU_HEADER_NESN_IDX            2
#define BLE_DATA_PDU_HEADER_SN_IDX              3
#define BLE_DATA_PDU_HEADER_MD_IDX              4
#define BLE_DATA_PDU_HEADER_CP_IDX              5
#define BLE_DATA_PDU_HEADER_LENGTH_IDX          8

#define BLE_DATA_PDU_HEADER_LLID_BM             0x0003
#define BLE_DATA_PDU_HEADER_NESN_BM             0x0004
#define BLE_DATA_PDU_HEADER_SN_BM               0x0008
#define BLE_DATA_PDU_HEADER_MD_BM               0x0010
#define BLE_DATA_PDU_HEADER_CP_BM               0x0020
#ifdef GP_DIVERSITY_DIRECTIONFINDING_SUPPORTED
// AoA uses an extra bit in the first header byte (strictly speaking, this is depending on the spec version and not on the feature)
#define BLE_DATA_PDU_HEADER_RFU_BM              0x00C0
#else
#define BLE_DATA_PDU_HEADER_RFU_BM              0x00E0
#endif
#define BLE_DATA_PDU_HEADER_LENGTH_BM           0xFF00

#define BLE_DATA_PDU_FIRST_HEADER_BYTE_BITS_MASKED_TO_ZERO (BLE_DATA_PDU_HEADER_NESN_BM | BLE_DATA_PDU_HEADER_SN_BM | BLE_DATA_PDU_HEADER_MD_BM)
#define BLE_DATA_PDU_FIRST_HEADER_BYTE_AUTH_MASK    (~BLE_DATA_PDU_FIRST_HEADER_BYTE_BITS_MASKED_TO_ZERO)

// Connection handle/PB/BC bytes defines
#define BLE_CONN_HANDLE_HANDLE_IDX              0
#define BLE_CONN_HANDLE_PB_IDX                  12
#define BLE_CONN_HANDLE_BC_IDX                  14
#define BLE_CONN_HANDLE_ISOPB_IDX               12
#define BLE_CONN_HANDLE_ISOTS_IDX               14

#define BLE_CONN_HANDLE_HANDLE_BM               0x0FFF
#define BLE_CONN_HANDLE_PB_BM                   0x3000
#define BLE_CONN_HANDLE_BC_BM                   0xC000
#define BLE_CONN_HANDLE_ISOPB_BM                0x3000
#define BLE_CONN_HANDLE_ISOTS_BM                0x4000

#define BLE_CTEINFO_CTETIME_IDX     0
#define BLE_CTEINFO_RFU_IDX         5
#define BLE_CTEINFO_CTETYPE_IDX     6

#define BLE_CTEINFO_CTETIME_BM      0x1F
#define BLE_CTEINFO_CTETYPE_BM      0xC0

#define BLE_TIME_SLOT_DURATION_US               625U

#define BLE_ADV_NUMBER_OF_CHANNELS              3
#define BLE_DATA_NUMBER_OF_CHANNELS             37
#define BLE_NUMBER_OF_CHANNELS                  (BLE_ADV_NUMBER_OF_CHANNELS + BLE_DATA_NUMBER_OF_CHANNELS)
#define BLE_PRIM_EXTADV_NUMBER_OF_CHANNELS      (BLE_ADV_NUMBER_OF_CHANNELS+2)

#define BLE_ADV_CHANNEL_37                      37
#define BLE_ADV_CHANNEL_38                      38
#define BLE_ADV_CHANNEL_39                      39
#define BLE_ADV_CHANNEL_INVALID                 0xFF

#define BLE_ADV_PDU_PAYLOAD_LENGTH_MIN          6
#define BLE_ADV_PDU_PAYLOAD_LENGTH_MAX          37      // 6 bytes address + 31 bytes payload (max)


#define BLE_NR_OF_VALID_ADV_PDU_TYPES           7
#define BLE_NR_OF_VALID_EXT_ADV_PDU_TYPES       9

#define BLE_CHANNEL_MAP_SIZE  5

#define GPBLE_COMMAND_MAX_LEN 100

#define BLE_CONN_PDU_DEFAULT_WIN_SIZE       3

#define BLE_PER_TEST_MODE_IDLE 0
#define BLE_PER_TEST_MODE_MTOS 1
#define BLE_PER_TEST_MODE_STOM 2

// The largest offset we can use for a PDU sent on the advertising channel
#define GP_BLE_ADV_CHANNEL_PDU_MAX_OFFSET   (GP_HAL_PBM_MAX_SIZE - 1)

#define GP_BLE_BT_ADDRESS_SIZE              6
#define GP_BLE_CONNECT_IND_LL_DATA_SIZE     22

#define GP_BLE_CHANNEL_MAP_MSB_MASK         0x1F
#define BLE_RES_PR_DEV_ADDR_RANDOM_TYPE_MASK   0xC0
#define BLE_RES_PR_DEV_ADDR_RANDOM_STATIC      0xC0 // unused
#define BLE_RES_PR_DEV_ADDR_RANDOM_NONRES_PR   0x00 // unused
#define BLE_RES_PR_DEV_ADDR_RANDOM_RES_PR      0x40
#define BLE_RES_PR_IS_RPA_ADDR(advPeerAddressType, address)   ((gpHci_AdvPeerAddressType_Random == (advPeerAddressType)) && (((address)->addr[5] & BLE_RES_PR_DEV_ADDR_RANDOM_TYPE_MASK) == BLE_RES_PR_DEV_ADDR_RANDOM_RES_PR))

#define GP_BLE_EC_MAX_VALUE                 0xFFFF


// 1) Bluetooth spec, Version 5.2, Vol 6, Part B, Section 4.2.2 (Sleep clock accuracy).
// All instantaneous timings shall not deviate more than 16 Ã‚Âµs from the average timing.
// 2) BT spec LL $4.2.1 shows a max active-clock jitter of 2 microsec on the active clock
//
// If the sleep clock is not activated than 2 is enough. For the master jitter we cannot know this but we could
// assume a jitter of 2 if the master is a gateway-type device that never sleeps.
// For our "slave" clock we can know this but we don't take it into account here although this could reduce the
// RX-window. On the other hand if the chip is always active it's questionable that this will have a
// signficant impact on power consumption.
#define GP_BLE_ACTIVE_CLOCK_JITTER_MAX_US       2
#define GP_BLE_SLEEP_CLOCK_JITTER_MAX_US       16

// If you accurately know the jitter if your own clock this can be optimized foreach sleep clock
#define GP_BLE_T_JITTER_SLEEP_OWN GP_BLE_SLEEP_CLOCK_JITTER_MAX_US
// No way to know what the peer's jitter is. We can only assume the MAX defined by the spec.
#define GP_BLE_T_JITTER_SLEEP_PEER GP_BLE_SLEEP_CLOCK_JITTER_MAX_US

// See [ BLE_TimingsAndJitter.pptx | https://qorvo.sharepoint.com/:P:/s/LPSSoftwareDev/ETWrh7ug_s5Ph1fi5OLrEkwBVrJdFTGywHXlAKtg-kitug?e=pY94fJ ]
// for a detailed explanation of this formula.
// Window widening for jitter alone: 144us (72us * 2) --> significant!
#define GP_BLE_T_JITTER_ACTIVE_TOTAL  (4*GP_BLE_ACTIVE_CLOCK_JITTER_MAX_US)
#define GP_BLE_T_JITTER_SLEEP_TOTAL   (2*(GP_BLE_T_JITTER_SLEEP_OWN + GP_BLE_T_JITTER_SLEEP_PEER))
// Hardcoding to 16 us because that has always be enough for slave connections in the past
#define GP_BLE_T_JITTER_TOTAL         16

#define BLE_SEC_KEY_DIV_PART_LENGTH (GP_HCI_ENCRYPTION_KEY_LENGTH >> 1)

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

#define BLE_ADV_PDU_HEADER_TYPE_SET(header, type)       BLE_BM_SET(header, BLE_ADV_PDU_HEADER_TYPE_BM, BLE_ADV_PDU_HEADER_TYPE_IDX,type)
#define BLE_ADV_PDU_HEADER_TYPE_GET(header)             BLE_BM_GET(header, BLE_ADV_PDU_HEADER_TYPE_BM, BLE_ADV_PDU_HEADER_TYPE_IDX)
#define BLE_ADV_PDU_HEADER_CHSEL_SET(header, type)      BLE_BM_SET(header, BLE_ADV_PDU_HEADER_CHSEL_BM, BLE_ADV_PDU_HEADER_CHSEL_IDX,type)
#define BLE_ADV_PDU_HEADER_CHSEL_GET(header)            BLE_BM_GET(header, BLE_ADV_PDU_HEADER_CHSEL_BM, BLE_ADV_PDU_HEADER_CHSEL_IDX)
#define BLE_ADV_PDU_HEADER_TXADD_SET(header, type)      BLE_BM_SET(header, BLE_ADV_PDU_HEADER_TXADD_BM, BLE_ADV_PDU_HEADER_TXADD_IDX,type)
#define BLE_ADV_PDU_HEADER_TXADD_GET(header)            BLE_BM_GET(header, BLE_ADV_PDU_HEADER_TXADD_BM, BLE_ADV_PDU_HEADER_TXADD_IDX)
#define BLE_ADV_PDU_HEADER_RXADD_SET(header, type)      BLE_BM_SET(header, BLE_ADV_PDU_HEADER_RXADD_BM, BLE_ADV_PDU_HEADER_RXADD_IDX,type)
#define BLE_ADV_PDU_HEADER_RXADD_GET(header)            BLE_BM_GET(header, BLE_ADV_PDU_HEADER_RXADD_BM, BLE_ADV_PDU_HEADER_RXADD_IDX)
#define BLE_ADV_PDU_HEADER_LENGTH_SET(header, type)     BLE_BM_SET(header, BLE_ADV_PDU_HEADER_LENGTH_BM, BLE_ADV_PDU_HEADER_LENGTH_IDX,type)
#define BLE_ADV_PDU_HEADER_LENGTH_GET(header)           BLE_BM_GET(header, BLE_ADV_PDU_HEADER_LENGTH_BM, BLE_ADV_PDU_HEADER_LENGTH_IDX)
#define BLE_EXTADV_PDU_HEADER_LENGTH_GET(header)        BLE_BM_GET(header, BLE_EXTADV_PDU_HEADER_LENGTH_BM, BLE_ADV_PDU_HEADER_LENGTH_IDX)

#define BLE_DATA_PDU_HEADER_LLID_SET(header, val)       BLE_BM_SET(header, BLE_DATA_PDU_HEADER_LLID_BM, BLE_DATA_PDU_HEADER_LLID_IDX, val)
#define BLE_DATA_PDU_HEADER_LLID_GET(header)            BLE_BM_GET(header, BLE_DATA_PDU_HEADER_LLID_BM, BLE_DATA_PDU_HEADER_LLID_IDX)
#define BLE_DATA_PDU_HEADER_NESN_SET(header, val)       BLE_BM_SET(header, BLE_DATA_PDU_HEADER_NESN_BM, BLE_DATA_PDU_HEADER_NESN_IDX, val)
#define BLE_DATA_PDU_HEADER_NESN_GET(header)            BLE_BM_GET(header, BLE_DATA_PDU_HEADER_NESN_BM, BLE_DATA_PDU_HEADER_NESN_IDX)
#define BLE_DATA_PDU_HEADER_SN_SET(header, val)         BLE_BM_SET(header, BLE_DATA_PDU_HEADER_SN_BM, BLE_DATA_PDU_HEADER_SN_IDX, val)
#define BLE_DATA_PDU_HEADER_SN_GET(header)              BLE_BM_GET(header, BLE_DATA_PDU_HEADER_SN_BM, BLE_DATA_PDU_HEADER_SN_IDX)
#define BLE_DATA_PDU_HEADER_MD_SET(header, val)         BLE_BM_SET(header, BLE_DATA_PDU_HEADER_MD_BM, BLE_DATA_PDU_HEADER_MD_IDX, val)
#define BLE_DATA_PDU_HEADER_MD_GET(header)              BLE_BM_GET(header, BLE_DATA_PDU_HEADER_MD_BM, BLE_DATA_PDU_HEADER_MD_IDX)
#define BLE_DATA_PDU_HEADER_CP_SET(header, val)         BLE_BM_SET(header, BLE_DATA_PDU_HEADER_CP_BM, BLE_DATA_PDU_HEADER_CP_IDX, val)
#define BLE_DATA_PDU_HEADER_CP_GET(header)              BLE_BM_GET(header, BLE_DATA_PDU_HEADER_CP_BM, BLE_DATA_PDU_HEADER_CP_IDX)

#define BLE_DATA_PDU_HEADER_LENGTH_SET(header, val)     BLE_BM_SET(header, BLE_DATA_PDU_HEADER_LENGTH_BM, BLE_DATA_PDU_HEADER_LENGTH_IDX, val)
#define BLE_DATA_PDU_HEADER_LENGTH_GET(header)          BLE_BM_GET(header, BLE_DATA_PDU_HEADER_LENGTH_BM, BLE_DATA_PDU_HEADER_LENGTH_IDX)

// Macro's to set/get fields in a 16bit connection handle / pb /bc type
#define BLE_CONN_HANDLE_HANDLE_SET(handle, val)         BLE_BM_SET(handle, BLE_CONN_HANDLE_HANDLE_BM, BLE_CONN_HANDLE_HANDLE_IDX, val)
#define BLE_CONN_HANDLE_HANDLE_GET(handle)              BLE_BM_GET(handle, BLE_CONN_HANDLE_HANDLE_BM, BLE_CONN_HANDLE_HANDLE_IDX)
#define BLE_CONN_HANDLE_PB_SET(handle, val)             BLE_BM_SET(handle, BLE_CONN_HANDLE_PB_BM, BLE_CONN_HANDLE_PB_IDX, val)
#define BLE_CONN_HANDLE_PB_GET(handle)                  BLE_BM_GET(handle, BLE_CONN_HANDLE_PB_BM, BLE_CONN_HANDLE_PB_IDX)
#define BLE_CONN_HANDLE_BC_SET(handle, val)             BLE_BM_SET(handle, BLE_CONN_HANDLE_BC_BM, BLE_CONN_HANDLE_BC_IDX, val)
#define BLE_CONN_HANDLE_BC_GET(handle)                  BLE_BM_GET(handle, BLE_CONN_HANDLE_BC_BM, BLE_CONN_HANDLE_BC_IDX)
#define BLE_CONN_HANDLE_ISOPB_GET(handle)               BLE_BM_GET(handle, BLE_CONN_HANDLE_ISOPB_BM, BLE_CONN_HANDLE_ISOPB_IDX)
#define BLE_CONN_HANDLE_ISOTS_GET(handle)               BLE_BM_GET(handle, BLE_CONN_HANDLE_ISOTS_BM, BLE_CONN_HANDLE_ISOTS_IDX)

// Macro's to set/get members in cteInfo field
#define BLE_CTEINFO_TIME_SET(cteInfo, val)             BLE_BM_SET(cteInfo, BLE_CTEINFO_CTETIME_BM, BLE_CTEINFO_CTETIME_IDX, val)
#define BLE_CTEINFO_TIME_GET(cteInfo)                  BLE_BM_GET(cteInfo, BLE_CTEINFO_CTETIME_BM, BLE_CTEINFO_CTETIME_IDX)
#define BLE_CTEINFO_TYPE_SET(cteInfo, val)             BLE_BM_SET(cteInfo, BLE_CTEINFO_CTETYPE_BM, BLE_CTEINFO_CTETYPE_IDX, val)
#define BLE_CTEINFO_TYPE_GET(cteInfo)                  BLE_BM_GET(cteInfo, BLE_CTEINFO_CTETYPE_BM, BLE_CTEINFO_CTETYPE_IDX)

#define BLE_ADVERTISING_TYPE_VALID(type)            (type < gpHci_AdvertisingType_Invalid)
#define BLE_OWN_ADDRESS_TYPE_VALID(type)            (type < gpHci_OwnAddressType_Invalid)
#define BLE_ADV_PEER_ADDRESS_TYPE_VALID(type)       (type < gpHci_AdvPeerAddressType_Invalid)
#define BLE_INIT_PEER_ADDRESS_TYPE_VALID(type)      (type < gpHci_InitPeerAddressType_Invalid)

#define BLE_WHITELIST_ADDRESS_TYPE_VALID(type)      ((type == gpHci_WhitelistAddressType_PublicDevice) || (type == gpHci_WhitelistAddressType_RandomDevice))

#define BLE_ADV_CHANNEL_MAP_INVALID                 0x00
#define BLE_ADV_CHANNEL_MAP_CH37                    BM(0)
#define BLE_ADV_CHANNEL_MAP_CH38                    BM(1)
#define BLE_ADV_CHANNEL_MAP_CH39                    BM(2)
#define BLE_ADV_CHANNEL_MAP_ALL                     (BLE_ADV_CHANNEL_MAP_CH37 | BLE_ADV_CHANNEL_MAP_CH38 | BLE_ADV_CHANNEL_MAP_CH39)

#define BLE_OWN_ADDRESS_TYPE_SUPPORTED(type)        (type == gpHci_OwnAddressType_PublicDevice || type == gpHci_OwnAddressType_RandomDevice)

#define BLE_ADV_CHANNEL_MAP_VALID(map)              (map != BLE_ADV_CHANNEL_MAP_INVALID && map <= BLE_ADV_CHANNEL_MAP_ALL)

#define BLE_ADV_FILTER_POLICY_VALID(policy)         (policy < gpHci_AdvFilterPolicy_Invalid)

#define BLE_IS_ACCESS_ADDRESS_ADVERTISER(address)   (address == BLE_ACCESS_ADDRESS_ADVERTISING_CHANNEL_PACKET)

// Advertising PDU type macros
#define BLE_IS_ADV_PDU_TYPE_VALID(type)             (type < Ble_AdvertisingPduType_Invalid)
#define BLE_IS_ADV_PDU_TYPE_CONNECTABLE(type)       (type == Ble_AdvertisingPduType_ConnectableUndirected || type == Ble_AdvertisingPduType_ConnectableDirected)
#define BLE_IS_ADV_PDU_TYPE_SCANNABLE(type)         (type == Ble_AdvertisingPduType_ConnectableUndirected || type == Ble_AdvertisingPduType_ScannableUndirected)
#define BLE_IS_ADV_PDU_TYPE_DIRECTED(type)          (type == Ble_AdvertisingPduType_ConnectableDirected)
#define BLE_IS_ADV_PDU_TYPE_UNDIRECTED(type)        (type == Ble_AdvertisingPduType_ConnectableUndirected || type == Ble_AdvertisingPduType_NonConnectableUndirected || type == Ble_AdvertisingPduType_ScannableUndirected)
#define BLE_IS_SCAN_PDU_TYPE_RESP(type)             (type == Ble_AdvertisingPduType_ScanResponse)
#define BLE_IS_ADVERTISING_RECORD_PDU_TYPE(type)    (type == Ble_AdvertisingPduType_ConnectableUndirected || type == Ble_AdvertisingPduType_NonConnectableUndirected || type == Ble_AdvertisingPduType_ScannableUndirected || type == Ble_AdvertisingPduType_ScanResponse || type == Ble_AdvertisingPduType_ConnectableDirected)

#define BLE_TIME_UNIT_TO_MS(unit)                   ((unit)*BLE_LL_TIME_UNIT_US/1000)

#define BLE_SET_RESPONSE_EVENT_COMMAND_COMPLETE(code)       (code = gpHci_EventCode_CommandComplete)
#define BLE_SET_RESPONSE_EVENT_COMMAND_STATUS(code)         (code = gpHci_EventCode_CommandStatus)

#define BLE_LLCP_OPCODE_VALID(opcode)                       (opcode < gpBleLlcp_OpcodeInvalid)

#define BLE_CONTROLLER_TO_HOST_FLOW_CONTROL_VALID(fc)       (fc < gpHci_FlowControlToHostInvalid)

#define BLE_SET_RESPONSE_EVENT_INVALID(code)        (code = gpHci_EventCode_Invalid)
#define BLE_SET_RESPONSE_EVENT_NOTHING(code)        (code = gpHci_EventCode_Nothing)

#define BLE_IS_RESPONSE_EVENT_VALID(code)           (code != gpHci_EventCode_Invalid)
#define BLE_IS_RESPONSE_EVENT_NEEDED(code)          (BLE_IS_RESPONSE_EVENT_VALID(code) && code != gpHci_EventCode_Nothing)

#define BLE_TIME_UNIT_625_TO_US(unit)               ((unit) * BLE_TIME_SLOT_DURATION_US)
#define BLE_TIME_UNIT_1250_TO_US(unit)              (BLE_TIME_UNIT_625_TO_US(unit) << 1)
#define BLE_TIME_UNIT_10000_TO_US(unit)             (BLE_TIME_UNIT_1250_TO_US(unit) << 3)


#define BLE_US_TO_625_TIME_UNIT(us)                 ((us) / BLE_TIME_SLOT_DURATION_US)
#define BLE_US_TO_1250_TIME_UNIT(us)                 ((us) / (BLE_TIME_SLOT_DURATION_US << 1))

#define BLE_GET_TIME_DIFF(earliestTs,latestTs)      (latestTs>=earliestTs ? (latestTs) - (earliestTs) : 0xFFFFFFFF - ((earliestTs) - (latestTs) - 1))
#define BLE_GET_EC_DIFF(earliestEc,latestEc)      (latestEc>=earliestEc ? (latestEc) - (earliestEc) : 0xFFFF - ((earliestEc) - (latestEc) - 1))
/* first < second  ==> result < 0
 * first == second ==> result == 0
 * first > second  ==> result > 0 */
#define BLE_EC_CMP(commonPreceding, first, second)   gpBle_ec_cmp(commonPreceding, first, second)

#define BLE_GET_TX_ADD_FROM_OWN_ADDRESS_TYPE(type)      (type & 0x01)

#define BLE_CONN_HANDLE_INVALID                     0xFF

#define BLE_COMBINE_HOP_SCA(hop, sca)               ((sca) << 5 | (hop))
#define BLE_GET_HOP_FROM_LAST_LL_DATA_BYTE(byte)    ((byte) & 0x1F)
#define BLE_GET_SCA_FROM_LAST_LL_DATA_BYTE(byte)    ((byte) >> 5)

#define BLE_LL_DATA_WIN_SIZE_UPPER                  8

// Priorities used in Ble
#define Ble_Priority_VeryLow        0x33
#define Ble_Priority_Low            0x66
#define Ble_Priority_Medium         0x99
#define Ble_Priority_High           0xCC
#define Ble_Priority_VeryHigh       0xFF
typedef UInt8 Ble_Priority_t;

#define BLE_CONN_PRIORITY           Ble_Priority_Medium

#define GP_BLE_SUBEVENT_PRIORITY_RX                 0
#define GP_BLE_SUBEVENT_PRIORITY_DEFAULT            Ble_Priority_VeryLow
#define GP_BLE_SUBEVENT_PRIORITY_EXTADV             GP_BLE_SUBEVENT_PRIORITY_DEFAULT
#define GP_BLE_SUBEVENT_PRIORITY_PERIODIC           Ble_Priority_Medium
#define GP_BLE_SUBEVENT_PRIORITY_SYNC_CREATION      Ble_Priority_High

#define BLE_IS_INT_CONN_HANDLE_VALID(connId)    ((connId != BLE_CONN_HANDLE_INVALID) && ((connId < BLE_LLCP_MAX_NR_OF_CONNECTIONS) || (connId & GPHAL_BLE_VIRTUAL_CONN_MASK)))

#define BLE_IS_CHANNEL_VALID(channel)                   (channel < BLE_NUMBER_OF_CHANNELS)

#define BLE_IS_MODULATION_INDEX_VALID(index)            (index < gpHci_ModulationIndex_Invalid)

// Activity id
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_INVALID        0xFF
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_ADVERTISING    0x80
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_SCANNING       0x81
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_INITIATING     0x82

#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_PERIODIC_ADVERTISING_START     0x90
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_PERIODIC_ADVERTISING_END       0x95

#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_CIG_START     0x98
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_CIG_END       0x99

#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_CIS_START     0x9A
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_CIS_END       0x9F

#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_SYNC_START     0xA0
#define GPBLEACTIVITYMANAGER_ACTIVITY_ID_SYNC_END       0xAF

#define Ble_ReadAddressFromPd(p_PdLoh, pAddr)   gpPd_ReadWithUpdate(p_PdLoh, sizeof(BtDeviceAddress_t), (UInt8*)pAddr.addr)

#define BLE_ADV_ADDR_TYPE_TO_HAL_ADDR_BIT(advAddrBit)                   ((Bool)gpHci_AdvPeerAddressType_Random==advAddrBit?true:false)
#define BLE_HAL_ADDR_BIT_TO_ADVPEER_ADDR_TYPE(halAddrBit)               ((gpHci_AdvPeerAddressType_t)((halAddrBit)?gpHci_AdvPeerAddressType_Random:gpHci_AdvPeerAddressType_Public))

#define BLE_ADDRESS_TYPE_DEVICE_TO_IDENTITY(addressType, isResolved)    ((addressType) | ((isResolved) ? 0x02 : 0x00))
#define BLE_ADDRESS_TYPE_IDENTITY_TO_DEVICE(addressType)                ((addressType) & 0x01)

#define BLE_OWN_ADDR_TYPE_TO_ADVPEER_ADDR_TYPE(ownAddrType)                     ((gpHci_AdvPeerAddressType_t)((gpHci_OwnAddressType_Invalid == (ownAddrType)) ? gpHci_AdvPeerAddressType_Invalid : BLE_ADDRESS_TYPE_IDENTITY_TO_DEVICE((ownAddrType))))
#define BLE_ADVPEER_ADDR_TYPE_TO_INITPEER_ADDR_TYPE(advAddrType, isResolved)    ((gpHci_InitPeerAddressType_t)((gpHci_AdvPeerAddressType_Invalid == (advAddrType)) ? gpHci_InitPeerAddressType_Invalid : BLE_ADDRESS_TYPE_DEVICE_TO_IDENTITY((advAddrType), (isResolved))))
#define BLE_ADVPEER_ADDR_TYPE_TO_DIRECT_ADDR_TYPE(advAddrType, isResolved)      ((gpHci_DirectAddressType_t)((gpHci_AdvPeerAddressType_Invalid == (advAddrType)) ? gpHci_DirectAddressType_Invalid : BLE_ADDRESS_TYPE_DEVICE_TO_IDENTITY((advAddrType), (isResolved))))
#define BLE_INITPEER_ADDRESS_TYPE_TO_WL_ADRESS_TYPE(type)                       ((gpHci_WhitelistAddressType_t)((gpHci_InitPeerAddressType_NoAddress == (type)) ? gpHci_WhitelistAddressType_Anonymous : BLE_ADDRESS_TYPE_IDENTITY_TO_DEVICE((type))))
#define BLE_ADVERTISER_ADDRESS_TYPE_TO_ADVPEER_ADDR_TYPE(type)                  ((gpHci_AdvPeerAddressType_t)BLE_ADDRESS_TYPE_IDENTITY_TO_DEVICE((type)))

#define BLE_OWN_ADDR_TYPE_IS_RPA(ownAddrType)                           (((ownAddrType) == gpHci_OwnAddressType_RPAPublicIfUnavailable) || ((ownAddrType) == gpHci_OwnAddressType_RPARandomIfUnavailable))
#define BLE_OWN_ADDR_TYPE_IS_RANDOM(ownAddrType)                        (((ownAddrType) == gpHci_OwnAddressType_RandomDevice) || ((ownAddrType) == gpHci_OwnAddressType_RPARandomIfUnavailable))

#define BLE_ADVA_MODE_NONCONN_NONSCAN   0x00
#define BLE_ADVA_MODE_CONNECTABLE       0x01
#define BLE_ADVA_MODE_SCANNABLE         0x02
#define BLE_EXTADV_INVALID_ADVMODE      (BLE_ADVA_MODE_CONNECTABLE | BLE_ADVA_MODE_SCANNABLE)

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#define BLE_DIVERSITY_BLE_INITIATOR_ENABLED_IN_FLASH()  (Ble_DiversityBleInitiatorEnabledInFlash())
#define BLE_DIVERSITY_BLE_SCANNER_ENABLED_IN_FLASH()    (Ble_DiversityBleScannerEnabledInFlash())
#define BLE_DIVERSITY_BLE_EXTADV_ENABLED_IN_FLASH()     (Ble_DiversityBleExtAdvEnabledInFlash())
#else
#ifdef GP_DIVERSITY_BLE_INITIATOR
#define BLE_DIVERSITY_BLE_INITIATOR_ENABLED_IN_FLASH()  (true)
#else
#define BLE_DIVERSITY_BLE_INITIATOR_ENABLED_IN_FLASH()  (false)
#endif
#ifdef GP_DIVERSITY_BLE_SCANNER
#define BLE_DIVERSITY_BLE_SCANNER_ENABLED_IN_FLASH()    (true)
#else
#define BLE_DIVERSITY_BLE_SCANNER_ENABLED_IN_FLASH()    (false)
#endif
#define BLE_DIVERSITY_BLE_EXTADV_ENABLED_IN_FLASH()     (false)
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

#define GP_LOG_PRINTF_ADDRESS(string, address)  GP_LOG_PRINTF(string": %02x:%02x:%02x:%02x:%02x:%02x", 0,    \
                                                    (address).addr[0], (address).addr[1], (address).addr[2], \
                                                    (address).addr[3], (address).addr[4], (address).addr[5])

#define GP_LOG_SYSTEM_PRINTF_ADDRESS(string, address)   GP_LOG_SYSTEM_PRINTF(string": %02x:%02x:%02x:%02x:%02x:%02x", 0, \
                                                            (address).addr[0], (address).addr[1], (address).addr[2],     \
                                                            (address).addr[3], (address).addr[4], (address).addr[5])

#define BLE_CONN_PARAM_REQ_NR_OF_OFFSETS            6

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define BLE_MASTER_SCA_0        0
#define BLE_MASTER_SCA_1        1
#define BLE_MASTER_SCA_2        2
#define BLE_MASTER_SCA_3        3
#define BLE_MASTER_SCA_4        4
#define BLE_MASTER_SCA_5        5
#define BLE_MASTER_SCA_6        6
#define BLE_MASTER_SCA_7        7
#define BLE_MASTER_SCA_INVALID  0xFF

typedef UInt8 Ble_MasterSca_t;

/** @enum Ble_AdvertisingPduType_t
    @brief  Table 2.3: Advertising channel PDU HeaderÃ¯Â¿Â½s PDU Type field encoding
*/
//@{
#define Ble_AdvertisingPduType_ConnectableUndirected          0
#define Ble_AdvertisingPduType_ConnectableDirected            1
#define Ble_AdvertisingPduType_NonConnectableUndirected       2
#define Ble_AdvertisingPduType_ScanRequest                    3
#define Ble_AdvertisingPduType_ScanResponse                   4
#define Ble_AdvertisingPduType_ConnectRequest                 5
#define Ble_AdvertisingPduType_ScannableUndirected            6
#define Ble_AdvertisingPduType_AuxCombined                    7
#define Ble_AdvertisingPduType_AuxConnectRsp                  8
#define Ble_AdvertisingPduType_Invalid                        9

/** @typedef Ble_AdvertisingPduType_t
    @brief  Advertising PDU types
*/
typedef UInt8                             Ble_AdvertisingPduType_t;

// LLID types
#define Ble_LLID_Reserved                       0x00
#define Ble_LLID_DataContinuedOrEmpty           0x01
#define Ble_LLID_DataStart                      0x02
#define Ble_LLID_Control                        0x03
typedef UInt8 Ble_LLID_t;

typedef struct {
    Ble_AdvertisingPduType_t pduType;
#if defined(GP_DIVERSITY_JUMPTABLES)
    Bool chSel;
#endif //GP_DIVERSITY_BLE_CHANNEL_SELECTION_ALGORITHM_NR2
    Bool txAdd;
    Bool rxAdd;
    UInt8 length;
#if defined(GP_DIVERSITY_JUMPTABLES)
    Int16 headerLength;
    UInt8 extendedHeaderFlags;
    UInt8 advMode;
#endif //def GP_DIVERSITY_EXTENDED_SCANNING
} Ble_AdvChannelPduHeader_t;

typedef struct {
    Ble_LLID_t llid;
    UInt8 length;
#ifdef GP_DIVERSITY_DIRECTIONFINDING_SUPPORTED
    Bool cp;
    UInt8 cteInfo;
#endif //GP_DIVERSITY_DIRECTIONFINDING_SUPPORTED
} Ble_DataChannelHeader_t;

typedef struct {
    gpBle_AccessAddress_t accessAddress;
    UInt32 crcInit;
    UInt8 winSize;
    UInt16 winOffset;
    UInt16 interval;
    UInt16 latency;
    UInt16 timeout;
    UInt8 channelMap[BLE_CHANNEL_MAP_SIZE];
    UInt8 hopIncrement;
    Ble_MasterSca_t sleepClockAccuracy;
} Ble_LLData_t;

typedef struct {
    Ble_AdvChannelPduHeader_t pduHeader;
    BtDeviceAddress_t initAddress;
    BtDeviceAddress_t advAddress;
    Ble_LLData_t llData;
} Ble_ConnReqPdu_t;

typedef struct {
    UInt16 intervalMin;
    UInt16 intervalMax;
    UInt16 latency;
    UInt16 timeout;
    UInt8 preferredPeriodicity;
    UInt16 refConnEventCount;
    UInt16 offsets[BLE_CONN_PARAM_REQ_NR_OF_OFFSETS];
} Ble_LlcpConnParamReqRspPdu_t;

typedef struct {
    Ble_ConnReqPdu_t connReqPdu;
    UInt8 antenna;
    UInt32 timestamp;
    Bool winOffsetCalculated;
    gpHci_PhyWithCoding_t phy;
    Bool legacy;
#if defined(GP_DIVERSITY_JUMPTABLES)
    Bool remoteChSelBitSet;
    Bool useChanSelAlgo2;
#endif //GP_DIVERSITY_BLE_CHANNEL_SELECTION_ALGORITHM_NR2
    UInt16 connIntervalMin;
    UInt16 connIntervalMax;
    UInt16 minCELength;
    UInt16 maxCELength;
} Ble_ConnEstablishParams_t;

#define Ble_IntConnId_Invalid 0xFF
typedef UInt8 Ble_IntConnId_t;

typedef void (*Ble_EmptyQueueCallback_t)(Ble_IntConnId_t connId);

typedef void    (*Ble_ResourceCBFunc_t)(Ble_IntConnId_t connId);

typedef UInt8 Ble_PerAdvEntryId_t;

// Identification of an activity (advertising, (initiator) scanning, connection)
typedef UInt8 Ble_ActivityId_t;


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpBle_CodeJumpTableFlash_Defs_defs.h"
#include "gpBle_CodeJumpTableRom_Defs_defs.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

void gpBle_SendVsdMetaEvent(gpHci_VsdMetaEventParams_t* pParams);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void Ble_ParseAdvPdHeader(gpPd_Loh_t* pPdLoh, Ble_AdvChannelPduHeader_t* pHeader);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
void Ble_ParseDataChannelPduHeader(gpPd_Loh_t* pPdLoh, Ble_DataChannelHeader_t* pHeader);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

Bool Ble_IsSupervisionToValid(UInt16 supervisionTo, UInt16 latency, UInt16 connInterval);

// functions to convert between accuracy (ppm) and sca unit (spec value)
UInt16 Ble_ScaFieldToMaxPpm(Ble_MasterSca_t sca);
Ble_MasterSca_t Ble_PpmToScaField(UInt16 ppm);

void Ble_EventSpecificHandling(gpBle_EventBuffer_t* pEventBuf, gpHci_CommandOpCode_t opCode, gpHci_Result_t result);

// common service functions


void Ble_ReadConnReqFromPd(gpPd_Loh_t* pPdLoh, Ble_ConnReqPdu_t* pConnReq);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */


// LLCP

void Ble_LlcpDataInd(Ble_IntConnId_t connHandle, gpPd_Loh_t pdLoh);
void gpBleLlcp_cbAcknowledgeSeen(UInt8 connId);
void gpBleLlcpFramework_cbDataConfirm(UInt8 connId, gpPd_Handle_t pdHandle);
void Ble_LlcpResourceAvailableInd(UInt8 connId);

// Common
Bool Ble_IsAdvPduHeaderValid(Ble_AdvChannelPduHeader_t* pAdvHeader);


gpHci_Result_t Ble_ConstructPacketAdvertising(gpPd_Loh_t* pdLoh, UInt16 pduHeader, UInt8 payloadLength, UInt8* pPduPayload);
void Ble_WritePayloadAdvertising(gpPd_Loh_t* pdLoh, Ble_AdvertisingPduType_t pduType);
void Ble_WritePacketHeader(gpPd_Loh_t* pdLoh);

void Ble_DumpAddress(BtDeviceAddress_t* pAddr);

// time compare
Bool gpBle_IsTimestampEarlier(UInt32 tsFirst, UInt32 tsSecond);
Bool gpBle_IsTimestampEarlierOrEqual(UInt32 tsFirst, UInt32 tsSecond);
UInt32 gpBle_GetTimeDifference(UInt32 tsFirst, UInt32 tsSecond);
UInt32 gpBle_GetGcd(UInt32 a, UInt32 b);

// channel map
UInt8 Ble_CountChannels(gpHci_ChannelMap_t* pChannelMap);

// llcp

// Resource Manager (RM) related functions
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
gpHci_Result_t Ble_RMGetResource(gpPd_Loh_t* pdLoh);
void Ble_RMFreeResource(Ble_IntConnId_t connId, gpPd_Handle_t handle);
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

void Ble_SetTimeFor15Dot4(UInt32 timeFor15Dot4);
Bool Ble_DataTxQueueVsdIsDefaultCBEnabled(void);
void Ble_EnableCBByDefault(Bool enable);




Int8 gpBle_ec_cmp(UInt16 commonPreceding, UInt16 first, UInt16 second);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
#if defined(GP_DIVERSITY_JUMPTABLES)
Bool Ble_DiversityBleInitiatorEnabledInFlash(void);
Bool Ble_DiversityBleScannerEnabledInFlash(void);
Bool Ble_DiversityBleExtAdvEnabledInFlash(void);
#endif // defined(GP_DIVERSITY_JUMPTABLES)
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

void gpBle_AppendWithUpdate(UInt8* pBuffer, UInt8* pValue, UInt8* pIndex, UInt8 length);

#endif //!defined(GP_DIVERSITY_ROM_CODE)

gpHci_InitPeerAddressType_t Ble_GetHCiAdvAType(Ble_AdvChannelPduHeader_t *pAdvHeader, BtDeviceAddress_t* pAdvA, gpHal_BleAdvIndInfo_t* advIndInfo);

#endif //_GP_BLE_DEFS_H_
