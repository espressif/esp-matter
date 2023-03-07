/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GPMAC_CORE_DEFS_DEFINES_H_
#define _GPMAC_CORE_DEFS_DEFINES_H_
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpUtils.h"
#include "gpMacCore.h"
#include "gpRxArbiter.h"
#include "gpHal.h"
#include "gpStat.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_MACCORE_SHORT_ADDR_SIZE  2
#define GP_MACCORE_EXT_ADDR_SIZE    8
#define GP_MACCORE_PANID_SIZE       2
#define GP_MACCORE_FC_SIZE          2
#define GP_MACCORE_SN_SIZE          1
#define GP_MACCORE_IE_HEADER_SIZE   2

// Indexes for fields in framecontrol
#define GP_MACCORE_FRAME_TYPE_IDX               0
#define GP_MACCORE_SECURITY_ENABLED_IDX         3
#define GP_MACCORE_FRAME_PENDING_IDX            4
#define GP_MACCORE_ACK_REQ_IDX                  5
#define GP_MACCORE_INTRA_PAN_IDX                6
#define GP_MACCORE_SEQ_NO_SUPPRESSION_IDX       8
#define GP_MACCORE_IE_PRESENT_IDX               9
#define GP_MACCORE_DEST_ADDR_MODE_IDX           10
#define GP_MACCORE_FRAME_VERSION_IDX            12
#define GP_MACCORE_SRC_ADDR_MODE_IDX            14

// Bit masks for fields in framecontrol
#define GP_MACCORE_FRAME_TYPE_BM                0x0007
#define GP_MACCORE_SECURITY_ENABLED_BM          0x0008
#define GP_MACCORE_FRAME_PENDING_BM             0x0010
#define GP_MACCORE_ACK_REQ_BM                   0x0020
#define GP_MACCORE_INTRA_PAN_BM                 0x0040
#define GP_MACCORE_SEQ_NO_SUPPRESSION_BM        0x0100
#define GP_MACCORE_IE_PRESENT_BM                0x0200
#define GP_MACCORE_DEST_ADDR_MODE_BM            0x0C00
#define GP_MACCORE_FRAME_VERSION_BM             0x3000
#define GP_MACCORE_SRC_ADDR_MODE_BM             0xC000

// Indexes for fields in superframe specification
#define MACCORE_SFS_BEACON_ORDER_IDX         0
#define MACCORE_SFS_SUPERFRAME_ORDER_IDX     4
#define MACCORE_SFS_FINAL_CAP_SLOT_IDX       8
#define MACCORE_SFS_BATTERY_LIFE_EXT_IDX     12
#define MACCORE_SFS_PAN_COORDINATOR_IDX      14
#define MACCORE_SFS_ASSOCIATION_PERMIT_IDX   15

// Bit masks for fields in superframe specification
#define MACCORE_SFS_BEACON_ORDER_BM          0x000F
#define MACCORE_SFS_SUPERFRAME_ORDER_BM      0x00F0
#define MACCORE_SFS_FINAL_CAP_SLOT_BM        0x0F00
#define MACCORE_SFS_BATTERY_LIFE_EXT_BM      0x1000
#define MACCORE_SFS_PAN_COORDINATOR_BM       0x4000
#define MACCORE_SFS_ASSOCIATION_PERMIT_BM    0x8000

//Bit masks for pending address specification in a beacon (5.2.2.1.6)
#define GP_MACCORE_BCN_SHORT_ADDRS_SPEC_BM                0x0007
#define GP_MACCORE_BCN_EXT_ADDRS_SPEC_BM                  0x0070
#define GP_MACCORE_BCN_SHORT_ADDRS_SPEC_IDX                 0
#define GP_MACCORE_BCN_EXT_ADDRS_SPEC_IDX                   4

// Indexes for fields in security control
#define GP_MACCORE_SEC_LEVEL_IDX                0
#define GP_MACCORE_KEY_ID_MODE_IDX              3

// Bit masks for fields in security control
#define GP_MACCORE_SEC_LEVEL_BM                 0x07
#define GP_MACCORE_KEY_ID_MODE_BM               0x18

// Bit masks for fields in IEs
#define GP_MACCORE_IE_LENGTH_IDX                0
#define GP_MACCORE_IE_ELEMENTID_IDX             7
#define GP_MACCORE_IE_TYPE_IDX                  15

// Bit masks for fields in IEs
#define GP_MACCORE_IE_LENGTH_BM                 0x3F
#define GP_MACCORE_IE_ELEMENTID_BM              0x7F80
#define GP_MACCORE_IE_TYPE_BM                   0x8000

#define GP_MACCORE_TRANSACTION_PERSISTENCE_TIME_TO_US(time)     ((UInt32)time*(GP_MACCORE_BASE_SUPERFRAME_DURATION*GP_MACCORE_SYMBOL_DURATION))

// mac constants
#define GP_MACCORE_UNIT_BACKOFF_PERIOD          20

#define GP_MACCORE_ASSOCIATION_REQUEST_CMD_LEN      2
#define GP_MACCORE_ASSOCIATION_RESPONSE_CMD_LEN     4
#define GP_MACCORE_DATA_REQUEST_CMD_LEN             1
#define GP_MACCORE_ORPHAN_NOTIFICATION_CMD_LEN      1
#define GP_MACCORE_BEACON_REQUEST_CMD_LEN           1
#define GP_MACCORE_COORDINATOR_REALIGNMENT_CMD_LEN  8

#define GP_MACCORE_GTS_FIELDS                   0
#define GP_MACCORE_PENDING_ADDRESS_FIELDS       0

// mac constants
#define GP_MACCORE_NUMBER_OF_RETRIES    3
#define GP_MACCORE_RESPONSE_WAIT_TIME                       32*GP_MACCORE_BASE_SUPERFRAME_DURATION //in symbols so *16 for in us
#define GP_MACCORE_WORST_CCA_RETRIES_TIME   53 // 2^3 - 1 + 2^4 - 1 + 2^5 - 1  // in backofPeriods


#define GP_MACCORE_WORST_CASE_CCA_RETRY_TIME         (UInt32)(GP_MACCORE_NUMBER_OF_RETRIES+1)*(GP_MACCORE_WORST_CCA_RETRIES_TIME * GP_MACCORE_UNIT_BACKOFF_PERIOD +(GP_MACCORE_MAX_PHY_PACKET_SIZE+GP_MACCORE_MIN_FRAME_OVERHEAD)*2 + GP_MACCORE_ACK_WAIT_DURATION) * GP_MACCORE_SYMBOL_DURATION           /* in us */

#define GP_MACCORE_MAX_FRAME_RESPONSE_TIME      1220 // symbols
#define GP_MACCORE_MAX_NUMBER_OF_RETRANSMITTED_MESSAGES_FILTER_EVENTS GP_MEMORY_ALLOCATOR_NUMBER_MAC_SEQUENCENUMBER_FILTER_EVENT

#define GP_MACCORE_INDIRECT_PACKET_SIZE_QUEUE              2

#define GP_MACCORE_NUMBER_OF_CHANNELS   16

// definitions used for the hardware scheduler, values from 0 till 0xF are available for use
#define GP_MACCORE_ES_SCHED_ISR         0xA
#define GP_MACCORE_ACL_ENTRY            0x8

#define GP_MACCORE_FRAME_COUNTER_MAXIMUM    0xFFFFFFFF
#define GP_MACCORE_IE_HEADER_TERMINATION    0x3F80

/** This function sets bits in the frame control field of the MAC header.
 *
 * @param  bf  The buffer that contains the frame control byte.
 * @param  bm  The shifted bit mask of the bits to set.
 * @param  idx The bit index of the bits to set.
 * @param  v   The value to set.
 */
#define MACCORE_BM_SET(bf,bm,idx,v)     bf &= ~bm; bf |= (((UInt16)v) << idx)
#define MACCORE_BM_GET(header, mask, idx)     ((header & mask) >> idx)

#define MACCORE_FRAMECONTROL_FRAMETYPE_SET(header, type)                MACCORE_BM_SET(header, GP_MACCORE_FRAME_TYPE_BM, GP_MACCORE_FRAME_TYPE_IDX, type)
#define MACCORE_FRAMECONTROL_SECURITY_SET(header, security)             MACCORE_BM_SET(header, GP_MACCORE_SECURITY_ENABLED_BM, GP_MACCORE_SECURITY_ENABLED_IDX, security)
#define MACCORE_FRAMECONTROL_ACKREQUEST_SET(header, ackreq)             MACCORE_BM_SET(header, GP_MACCORE_ACK_REQ_BM, GP_MACCORE_ACK_REQ_IDX, ackreq)
#define MACCORE_FRAMECONTROL_FRAMEPENDING_SET(header, fpPending)        MACCORE_BM_SET(header, GP_MACCORE_FRAME_PENDING_BM, GP_MACCORE_FRAME_PENDING_IDX, fpPending)
#define MACCORE_FRAMECONTROL_PANCOMPRESSION_SET(header, panComp)        MACCORE_BM_SET(header, GP_MACCORE_INTRA_PAN_BM, GP_MACCORE_INTRA_PAN_IDX, panComp)
#define MACCORE_FRAMECONTROL_SEQNOSUPPRESSION_SET(header, version)      MACCORE_BM_SET(header, GP_MACCORE_SEQ_NO_SUPPRESSION_BM, GP_MACCORE_SEQ_NO_SUPPRESSION_IDX, version)
#define MACCORE_FRAMECONTROL_IE_PRESENT_SET(header, iePresent)          MACCORE_BM_SET(header, GP_MACCORE_IE_PRESENT_BM, GP_MACCORE_IE_PRESENT_IDX, panComp)
#define MACCORE_FRAMECONTROL_FRAMEVERSION_SET(header, version)          MACCORE_BM_SET(header, GP_MACCORE_FRAME_VERSION_BM, GP_MACCORE_FRAME_VERSION_IDX, version)
#define MACCORE_FRAMECONTROL_DSTADDRMODE_SET(header, mode)              MACCORE_BM_SET(header, GP_MACCORE_DEST_ADDR_MODE_BM, GP_MACCORE_DEST_ADDR_MODE_IDX, mode)
#define MACCORE_FRAMECONTROL_SRCADDRMODE_SET(header, mode)              MACCORE_BM_SET(header, GP_MACCORE_SRC_ADDR_MODE_BM, GP_MACCORE_SRC_ADDR_MODE_IDX, mode)

#define MACCORE_FRAMECONTROL_FRAMETYPE_GET(header)                      MACCORE_BM_GET(header, GP_MACCORE_FRAME_TYPE_BM, GP_MACCORE_FRAME_TYPE_IDX)
#define MACCORE_FRAMECONTROL_SECURITY_GET(header)                       MACCORE_BM_GET(header, GP_MACCORE_SECURITY_ENABLED_BM, GP_MACCORE_SECURITY_ENABLED_IDX)
#define MACCORE_FRAMECONTROL_FRAMEPENDING_GET(header)                   MACCORE_BM_GET(header, GP_MACCORE_FRAME_PENDING_BM,GP_MACCORE_FRAME_PENDING_IDX)
#define MACCORE_FRAMECONTROL_PANCOMPRESSION_GET(header)                 MACCORE_BM_GET(header, GP_MACCORE_INTRA_PAN_BM,GP_MACCORE_INTRA_PAN_IDX)
#define MACCORE_FRAMECONTROL_IE_PRESENT_GET(header)                     MACCORE_BM_GET(header, GP_MACCORE_IE_PRESENT_BM,GP_MACCORE_IE_PRESENT_IDX)
#define MACCORE_FRAMECONTROL_FRAMEVERSION_GET(header)                   MACCORE_BM_GET(header, GP_MACCORE_FRAME_VERSION_BM, GP_MACCORE_FRAME_VERSION_IDX)
#define MACCORE_FRAMECONTROL_SEQNOSUPPRESSION_GET(header)               MACCORE_BM_GET(header, GP_MACCORE_SEQ_NO_SUPPRESSION_BM, GP_MACCORE_SEQ_NO_SUPPRESSION_IDX)
#define MACCORE_FRAMECONTROL_HASIEELEMENTS_GET(header)                  MACCORE_BM_GET(header, GP_MACCORE_HAS_IE_ELEMENTS_BM, GP_MACCORE_HAS_IE_ELEMENTS_IDX)
#define MACCORE_FRAMECONTROL_DSTADDRMODE_GET(header)                    MACCORE_BM_GET(header, GP_MACCORE_DEST_ADDR_MODE_BM,GP_MACCORE_DEST_ADDR_MODE_IDX)
#define MACCORE_FRAMECONTROL_SRCADDRMODE_GET(header)                    MACCORE_BM_GET(header, GP_MACCORE_SRC_ADDR_MODE_BM,GP_MACCORE_SRC_ADDR_MODE_IDX)

#define MACCORE_SECCONTROL_SECLEVEL_SET(secControl, secLevel)           MACCORE_BM_SET(secControl, GP_MACCORE_SEC_LEVEL_BM, GP_MACCORE_SEC_LEVEL_IDX, secLevel)
#define MACCORE_SECCONTROL_KEYIDMODE_SET(secControl, mode)              MACCORE_BM_SET(secControl, GP_MACCORE_KEY_ID_MODE_BM, GP_MACCORE_KEY_ID_MODE_IDX, mode)

#define MACCORE_SECCONTROL_SECLEVEL_GET(secControl)                     (secControl & 0x07)
#define MACCORE_SECCONTROL_KEYIDMODE_GET(secControl)                    MACCORE_BM_GET(secControl, GP_MACCORE_KEY_ID_MODE_BM,GP_MACCORE_KEY_ID_MODE_IDX)

#define MACCORE_SFS_BEACON_ORDER_SET(superframeSpec, bo)                MACCORE_BM_SET(superframeSpec, MACCORE_SFS_BEACON_ORDER_BM, MACCORE_SFS_BEACON_ORDER_IDX, bo)
#define MACCORE_SFS_SUPERFRAME_ORDER_SET(superframeSpec, so)            MACCORE_BM_SET(superframeSpec, MACCORE_SFS_SUPERFRAME_ORDER_BM, MACCORE_SFS_SUPERFRAME_ORDER_IDX, so)
#define MACCORE_SFS_FINAL_CAP_SLOT_SET(superframeSpec, fcs)             MACCORE_BM_SET(superframeSpec, MACCORE_SFS_FINAL_CAP_SLOT_BM, MACCORE_SFS_FINAL_CAP_SLOT_IDX, fcs)
#define MACCORE_SFS_BATTERY_LIFE_EXTENSION_SET(superframeSpec, ble)     MACCORE_BM_SET(superframeSpec, MACCORE_SFS_BATTERY_LIFE_EXT_BM, MACCORE_SFS_BATTERY_LIFE_EXT_IDX, ble)
#define MACCORE_SFS_PAN_COORDINATOR_SET(superframeSpec, pc)             MACCORE_BM_SET(superframeSpec, MACCORE_SFS_PAN_COORDINATOR_BM, MACCORE_SFS_PAN_COORDINATOR_IDX, pc)
#define MACCORE_SFS_ASSOCIATION_PERMIT_SET(superframeSpec, ap)          MACCORE_BM_SET(superframeSpec, MACCORE_SFS_ASSOCIATION_PERMIT_BM, MACCORE_SFS_ASSOCIATION_PERMIT_IDX, ap)


#define MACCORE_ADRR_PENDING_SHORT_GET(addrPendingSpec)                 MACCORE_BM_GET(addrPendingSpec, GP_MACCORE_BCN_SHORT_ADDRS_SPEC_BM, GP_MACCORE_BCN_SHORT_ADDRS_SPEC_IDX)
#define MACCORE_ADRR_PENDING_EXT_GET(addrPendingSpec)                   MACCORE_BM_GET(addrPendingSpec, GP_MACCORE_BCN_EXT_ADDRS_SPEC_BM, GP_MACCORE_BCN_EXT_ADDRS_SPEC_IDX)

/* Macros for parsing through the IE header elements: */
#define MACCORE_IE_TYPE_HEADER    0
#define MACCORE_IE_TYPE_PAYLOAD   1

#define MACCORE_IE_ID_CSL      0x1A
#define MACCORE_IE_ID_HT1      0x7E
#define MACCORE_IE_ID_HT2      0x7F

// Indexes for fields in information element headers
#define GP_MACCORE_IEHEADER_LENGTH_IDX      0
#define GP_MACCORE_IEHEADER_ELEMENTID_IDX   7
#define GP_MACCORE_IEHEADER_TYPE_IDX       15

// Bit masks for fields in information element headers
#define GP_MACCORE_IEHEADER_LENGTH_BM      0x007F
#define GP_MACCORE_IEHEADER_ELEMENTID_BM   0x7F80
#define GP_MACCORE_IEHEADER_TYPE_BM        0x8000

#define MACCORE_IEHEADER_LENGTH_GET(header)    MACCORE_BM_GET(header, GP_MACCORE_IEHEADER_LENGTH_BM,GP_MACCORE_IEHEADER_LENGTH_IDX)
#define MACCORE_IEHEADER_ELEMENTID_GET(header) MACCORE_BM_GET(header, GP_MACCORE_IEHEADER_ELEMENTID_BM,GP_MACCORE_IEHEADER_ELEMENTID_IDX)
#define MACCORE_IEHEADER_TYPE_GET(header)      MACCORE_BM_GET(header, GP_MACCORE_IEHEADER_TYPE_BM,GP_MACCORE_IEHEADER_TYPE_IDX)


// defines to access different fields within nonce
#define GP_MACCORE_NONCE_LENGTHOF_ADDRESS       sizeof(MACAddress_t)
#define GP_MACCORE_NONCE_LENGTHOF_FRAME_COUNTER sizeof(UInt32)
#define GP_MACCORE_NONCE_LENGTHOF_SEC_LEVEL     sizeof(UInt8)

#define GP_MACCORE_NONCE_STARTOF_ADDRESS        0
#define GP_MACCORE_NONCE_STARTOF_FRAME_COUNTER  GP_MACCORE_NONCE_LENGTHOF_ADDRESS
#define GP_MACCORE_NONCE_STARTOF_SEC_LEVEL      (GP_MACCORE_NONCE_LENGTHOF_ADDRESS + GP_MACCORE_NONCE_LENGTHOF_FRAME_COUNTER)


#ifndef GP_MACCORE_MEMORY_ALLOCATOR_CATCHING_TRAIN_LIST_SIZE
#define GP_MACCORE_MEMORY_ALLOCATOR_CATCHING_TRAIN_LIST_SIZE 8
#endif //GP_MACCORE_MEMORY_ALLOCATOR_CATCHING_TRAIN_LIST_SIZE

#define GP_MACCORE_INVALID_KEY_TABLE_ENTRY 0xFF
#define GP_MACCORE_INVALID_KEY_DEVICE_LIST_ENTRY 0xFF
#define GP_MACCORE_INVALID_HEADER_LENGTH    0

#define GP_MACCORE_FRAME_TYPE_FILTER_MASK_DEFAULT  (GPHAL_ENUM_FRAME_TYPE_FILTER_BCN_MASK  | \
                                                    GPHAL_ENUM_FRAME_TYPE_FILTER_RSV_MASK)

#define GP_MACCORE_FRAME_TYPE_FILTER_MASK_ONLY_CMD (GPHAL_ENUM_FRAME_TYPE_FILTER_BCN_MASK  | \
                                                    GPHAL_ENUM_FRAME_TYPE_FILTER_DATA_MASK | \
                                                    GPHAL_ENUM_FRAME_TYPE_FILTER_ACK_MASK  | \
                                                    GPHAL_ENUM_FRAME_TYPE_FILTER_RSV_MASK)

#define GP_MACCORE_FRAME_TYPE_FILTER_MASK_ONLY_BCN (GPHAL_ENUM_FRAME_TYPE_FILTER_CMD_MASK  | \
                                                    GPHAL_ENUM_FRAME_TYPE_FILTER_DATA_MASK | \
                                                    GPHAL_ENUM_FRAME_TYPE_FILTER_ACK_MASK  | \
                                                    GPHAL_ENUM_FRAME_TYPE_FILTER_RSV_MASK)

#define GP_MACCORE_ADDRESS_MODE_NOOVERRIDE 0xFF

#ifndef GP_MACCORE_MAX_NEIGHBOURS
#define GP_MACCORE_MAX_NEIGHBOURS           16
#endif  //GP_MACCORE_MAX_NEIGHBOURS

#ifndef GP_MACCORE_INDTX_ENTRIES
#define GP_MACCORE_INDTX_ENTRIES            GP_PD_NR_OF_HANDLES
#endif // GP_MACCORE_INDTX_ENTRIES

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)
#ifndef GP_MACCORE_MAX_ASSOC_RESP
#define GP_MACCORE_MAX_ASSOC_RESP            10
#endif // GP_MACCORE_MAX_ASSOC_RESP
#endif //defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT)  || defined(GP_DIVERSITY_JUMPTABLES)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct {
    gpMacCore_AddressInfo_t coordAddrInfo;
    gpMacCore_StackId_t     stackId;
    gpMacCore_Result_t      result;
    UInt16                  responseShortAddr;
    UInt32                  txTimestamp;
    Bool                    assoc;
    gpPd_Handle_t           pdHandle;
}MacCore_PollReqArgs_t;

#define MacCore_SecurityLevelPassedFalse                    0
#define MacCore_SecurityLevelPassedConditionallyPassed      1
#define MacCore_SecurityLevelPassedTrue                     2
typedef UInt8 MacCore_SecurityLevelPassed_t;

typedef struct {
    gpMacCore_ScanType_t currentScanType;
    UInt16 requestChannels;
    UInt16 scanChannels;
    UInt32 scanDurationUc;
    UInt8 resultListSize;
    UInt8 resultListSizeMax;
    UInt8 origPhyChannel;
    UInt16 unscannedChannels;
    union un{
        UInt8 origFilterMask;
    }un_t;
    UInt8* pEnergyDetectList;
    gpMacCore_StackId_t stackId;
    UInt8 scannedChannelCounter;
    UInt32 scanDurationRemaining;
    Int16 cummulatedRssi;
} gpMacCore_ScanState_t;

typedef struct {
    UInt16                  frameControl;
    gpMacCore_FrameType_t   frameType;
    UInt8                   sequenceNumber;
    gpMacCore_AddressInfo_t srcAddrInfo;
    gpMacCore_AddressInfo_t dstAddrInfo;
    UInt32                  frameCounter;
    gpMacCore_Security_t    secOptions;
    gpMacCore_StackId_t     stackId;
} MacCore_HeaderDescriptor_t;

typedef struct {
    UInt16                  assocShortAddr;
    gpMacCore_Result_t      assocStatus;
    gpMacCore_StackId_t     stackId;
    MACAddress_t            deviceAddress;
    Bool                    inFlight;
} MacCore_AssocRsp_t;

#endif //_GPMAC_CORE_DEFS_DEFINES_H_

