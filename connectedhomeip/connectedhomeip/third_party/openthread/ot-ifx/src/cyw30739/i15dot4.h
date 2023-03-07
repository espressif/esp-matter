/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file defines the related parameters used for i15dot4 MAC operation
 */

#ifndef _I15DOT4_H_
#define _I15DOT4_H_

#include <stdint.h>

#include "i15dot4_types.h"

#ifndef I15DOT4_VARAIABLE_ACK_LENGTH
#define I15DOT4_VARAIABLE_ACK_LENGTH 1
#endif // I15DOT4_VARAIABLE_ACK_LENGTH

#ifndef I15DOT4_TX_SCHEDULE_ENABLE
#define I15DOT4_TX_SCHEDULE_ENABLE 1
#endif // I15DOT4_TX_SCHEDULE_ENABLE

#ifndef I15DOT4_TX_EPA
#define I15DOT4_TX_EPA 1
#endif // I15DOT4_TX_EPA

typedef enum _CONTEXT_ID_t
{
    CONTEXT_ID_ZBPRO = 0,
    CONTEXT_ID_RF4CE = 1,
    CONTEXT_ID_6LO   = 2,
    CONTEXT_ID_NUM
} CONTEXT_ID_t;

typedef enum
{
    CONTEXT_TYPE_MAC = 0,
    CONTEXT_TYPE_APP = 1,
    CONTEXT_TYPE_NUM,
} CMD_CONTEXT_TYPE_t;

typedef enum
{
    ADDR_MATCH_DISABLE         = 0,
    ADDR_MATCH_ENABLE          = 1,
    ADDR_MATCH_ADD             = 2,
    ADDR_MATCH_CLEAR           = 3,
    ADDR_MATCH_CLEAR_ALL       = 4,
    ADDR_MATCH_SET_KEY         = 5,
    ADDR_MATCH_SET_KEY_ID_MODE = 6,
    ADDR_MATCH_SET_KEY_INDEX   = 7,
} ADDR_MATCH_ACTION_t;

typedef enum
{
    I15DOT4_ADDR_MATCH_FLAG_FRAME_PENDING = 0x01,
    I15DOT4_ADDR_MATCH_FLAG_VENDOR_IE     = 0x02,
    I15DOT4_ADDR_MATCH_FLAG_EXT_ADDR_MODE = 0x80,
} ADDR_MATCH_TYPE_t;

/* MLME/MCPS status */
typedef enum
{
    I15DOT4_STATUS_SUCCESS                 = 0x00,
    I15DOT4_STATUS_PAN_AT_CAPACITY         = 0x01,
    I15DOT4_STATUS_ACCESS_DENIED           = 0x02,
    I15DOT4_STATUS_COUNTER_ERROR           = 0xdb,
    I15DOT4_STATUS_IMPROPER_KEY_TYPE       = 0xdc,
    I15DOT4_STATUS_IMPROPER_SECURITY_LEVEL = 0xdd,
    I15DOT4_STATUS_UNSUPPORTED_LEGACY      = 0xde,
    I15DOT4_STATUS_UNSUPPORTED_SECURITY    = 0xdf,
    I15DOT4_STATUS_BEACON_LOSS             = 0xe0,
    I15DOT4_STATUS_CHANNEL_ACCESS_FAILURE  = 0xe1,
    I15DOT4_STATUS_DENIED                  = 0xe3,
    I15DOT4_STATUS_SECURITY_ERROR          = 0xe4,
    I15DOT4_STATUS_FRAME_TOO_LONG          = 0xe5,
    I15DOT4_STATUS_INVALID_GTS             = 0xe6,
    I15DOT4_STATUS_INVALID_HANDLE          = 0xe7,
    I15DOT4_STATUS_INVALID_PARAMETER       = 0xe8,
    I15DOT4_STATUS_NO_ACK                  = 0xe9,
    I15DOT4_STATUS_NO_BEACON               = 0xea,
    I15DOT4_STATUS_NO_DATA                 = 0xeb,
    I15DOT4_STATUS_NO_SHORT_ADDRESS        = 0xec,
    I15DOT4_STATUS_OUT_OF_CAP              = 0xed,
    I15DOT4_STATUS_PAN_ID_CONFLICT         = 0xee,
    I15DOT4_STATUS_REALIGNMENT             = 0xef,
    I15DOT4_STATUS_TRANSACTION_EXPIRED     = 0xf0,
    I15DOT4_STATUS_TRANSACTION_OVERFLOW    = 0xf1,
    I15DOT4_STATUS_TX_ACTIVE               = 0xf2,
    I15DOT4_STATUS_UNAVAILABLE_KEY         = 0xf3,
    I15DOT4_STATUS_UNSUPPORTED_ATTRIBUTE   = 0xf4,
    I15DOT4_STATUS_INVALID_ADDRESS         = 0xf5,
    I15DOT4_STATUS_ON_TIME_TOO_LONG        = 0xf6,
    I15DOT4_STATUS_PAST_TIME               = 0xf7,
    I15DOT4_STATUS_TRACKING_OFF            = 0xf8,
    I15DOT4_STATUS_INVALID_INDEX           = 0xf9,
    I15DOT4_STATUS_LIMIT_REACHED           = 0xfa,
    I15DOT4_STATUS_READ_ONLY               = 0xfb,
    I15DOT4_STATUS_SCAN_IN_PROGRESS        = 0xfc,
    I15DOT4_STATUS_SUPERFRAME_OVERLAP      = 0xfd,

    /* Vendor status code */
    I15DOT4_STATUS_CREATE_TASK_FAILURE        = 0x40,
    I15DOT4_STATUS_UNKNOWN_ERROR              = 0x41,
    I15DOT4_STATUS_CONDITIONALLY_PASSED       = 0x42,
    I15DOT4_STATUS_UNAVAILABLE_SECURITY_LEVEL = 0x43,
    I15DOT4_STATUS_UNAVAILABLE_DEVICE         = 0x44,
    I15DOT4_STATUS_AES_BUSY                   = 0x45, // aes hardware is busy doing other tasks.
    I15DOT4_STATUS_RAW_SUCCESS                = 0x46,
    I15DOT4_STATUS_RAW_TIMEOUT                = 0x47,
    I15DOT4_STATUS_UNKNOWN_FRAME_TX_DONE      = 0x48,
    I15DOT4_STATUS_CSMACA_TIMEOUT             = 0x49,
    I15DOT4_STATUS_UNKNOWN_TX_TIMEOUT         = 0x4a,
    I15DOT4_STATUS_AES_MIC_ERROR              = 0x4b,
    I15DOT4_STATUS_AES_OTHER_ERROR            = 0x4d,
    I15DOT4_STATUS_AUTO_ACK_PREEMPTED         = 0x4f,
} I15DOT4_STATUS_t;

/* 802.15.4 MAC/PHY constants */
enum
{
    I15DOT4_MAX_PHY_PACKET_SIZE         = 127,
    I15DOT4_TURNAROUND_TIME             = 12,
    I15DOT4_BASE_SLOT_DURATION          = 60,
    I15DOT4_GTS_DESC_PERSISTENCE_TIME   = 4,
    I15DOT4_MAX_BEACON_OVERHEAD         = 75,
    I15DOT4_MAX_LOST_BEACONS            = 4,
    I15DOT4_MAX_MPDU_UNSECURED_OVERHEAD = 25,
    I15DOT4_MAX_SIFS_FRAME_SIZE         = 18,
    I15DOT4_MIN_CAP_LENGTH              = 440,
    I15DOT4_MIN_MPDU_OVERHEAD           = 9,
    I15DOT4_NUMBER_SUPERFRAME_SLOTS     = 16,
    I15DOT4_UNIT_BACKOFF_PERIOD         = 20,
    I15DOT4_BASE_SUPERFRAME_DURATION    = I15DOT4_BASE_SLOT_DURATION * I15DOT4_NUMBER_SUPERFRAME_SLOTS,
    I15DOT4_MAX_MAC_SAFE_PAYLOAD_SIZE   = I15DOT4_MAX_PHY_PACKET_SIZE - I15DOT4_MAX_MPDU_UNSECURED_OVERHEAD,
    I15DOT4_MAX_MAC_PAYLOAD_SIZE        = I15DOT4_MAX_PHY_PACKET_SIZE - I15DOT4_MIN_MPDU_OVERHEAD,
    I15DOT4_MAX_BEACON_PAYLOAD_LENGTH   = I15DOT4_MAX_PHY_PACKET_SIZE - I15DOT4_MAX_BEACON_OVERHEAD,
    I15DOT4_FCS_LENGTH                  = 2,
#if I15DOT4_VARAIABLE_ACK_LENGTH
    I15DOT4_ACK_LENGTH = I15DOT4_MAX_PHY_PACKET_SIZE,
#else
    I15DOT4_ACK_LENGTH = 3,
#endif
};

/* Command/Event ID */
enum
{
    I15DOT4_CMDID_MLME_ASSOC_REQ             = 0,
    I15DOT4_CMDID_MLME_DISASSOC_REQ          = 1,
    I15DOT4_CMDID_MLME_GET_REQ               = 2,
    I15DOT4_CMDID_MLME_GTS_REQ               = 3,
    I15DOT4_CMDID_MLME_RESET_REQ             = 4,
    I15DOT4_CMDID_MLME_RX_ENABLE_REQ         = 5,
    I15DOT4_CMDID_MLME_SCAN_REQ              = 6,
    I15DOT4_CMDID_MLME_SET_REQ               = 7,
    I15DOT4_CMDID_MLME_START_REQ             = 8,
    I15DOT4_CMDID_MLME_SYNC_REQ              = 9,
    I15DOT4_CMDID_MLME_POLL_REQ              = 10,
    I15DOT4_CMDID_MCPS_DATA_REQ              = 11,
    I15DOT4_CMDID_MLME_ASSOC_RESP            = 12,
    I15DOT4_CMDID_MLME_ORPHAN_RESP           = 13,
    I15DOT4_CMDID_MCPS_DATA_RESP             = 14,
    I15DOT4_CMDID_MLME_ASSOC_IND             = 15,
    I15DOT4_CMDID_MLME_DISASSOC_IND          = 16,
    I15DOT4_CMDID_MLME_BEACON_NOTIFY_IND     = 17,
    I15DOT4_CMDID_MLME_COMM_STATUS_IND       = 18,
    I15DOT4_CMDID_MLME_GTS_IND               = 19,
    I15DOT4_CMDID_MLME_ORPHAN_IND            = 20,
    I15DOT4_CMDID_MLME_SYNC_LOSS_IND         = 21,
    I15DOT4_CMDID_MCPS_DATA_IND              = 22,
    I15DOT4_CMDID_MLME_ASSOC_CONF            = 23,
    I15DOT4_CMDID_MLME_DISASSOC_CONF         = 24,
    I15DOT4_CMDID_MLME_GET_CONF              = 25,
    I15DOT4_CMDID_MLME_GTS_CONF              = 26,
    I15DOT4_CMDID_MLME_RESET_CONF            = 27,
    I15DOT4_CMDID_MLME_RX_ENABLE_CONF        = 28,
    I15DOT4_CMDID_MLME_SCAN_CONF             = 29,
    I15DOT4_CMDID_MLME_SET_CONF              = 30,
    I15DOT4_CMDID_MLME_START_CONF            = 31,
    I15DOT4_CMDID_MLME_POLL_CONF             = 32,
    I15DOT4_CMDID_MCPS_DATA_CONF             = 33,
    I15DOT4_CMDID_MCPS_PURGE_REQ             = 34,
    I15DOT4_CMDID_MCPS_PURGE_CONF            = 35,
    I15DOT4_CMDID_TEST                       = 64,
    I15DOT4_CMDID_BRCM_PHY_TX_TEST_REQ       = 65,
    I15DOT4_CMDID_BRCM_PHY_TX_TEST_CONF      = 66,
    I15DOT4_CMDID_TEST_CONF                  = 69,
    I15DOT4_CMDID_BRCM_CONF                  = 70,
    I15DOT4_CMDID_BRCM_CLEAN_ALL_KEY_REQ     = 71,
    I15DOT4_CMDID_BRCM_ADD_KEY_REQ           = 72,
    I15DOT4_CMDID_BRCM_DEL_KEY_REQ           = 73,
    I15DOT4_CMDID_BRCM_ADD_KEY_ID_LOOKUP_REQ = 74,
    I15DOT4_CMDID_BRCM_DEL_KEY_ID_LOOKUP_REQ = 75,
    I15DOT4_CMDID_BRCM_ADD_KEY_DEVICE_REQ    = 76,
    I15DOT4_CMDID_BRCM_DEL_KEY_DEVICE_REQ    = 77,
    I15DOT4_CMDID_BRCM_ADD_DEVICE_REQ        = 78,
    I15DOT4_CMDID_BRCM_DEL_DEVICE_REQ        = 79,
    I15DOT4_CMDID_BRCM_POLL_IND              = 80,
    I15DOT4_CMDID_MLME_SCAN_CONF_MORE        = 81,
    I15DOT4_CMDID_BRCM_DATA_CONF_DEBUG       = 82,
    I15DOT4_CMDID_MCPS_DATA_REQ_SCHED        = 83,
    I15DOT4_CMDID_THREAD_DATA_REQ            = 84,
    I15DOT4_CMDID_THREAD_DATA_CONF           = 85,
    I15DOT4_CMDID_THREAD_DATA_IND            = 86,
    I15DOT4_CMDID_THREAD_ADDR_MATCH_REQ      = 87,
    I15DOT4_CMDID_THREAD_ADDR_MATCH_CONF     = 88,

    I15DOT4_CMDID_APP_ACTION     = 128,
    I15DOT4_CMDID_APP_ACTION_MSG = 129,
};

/* LossReason */
enum
{
    I15DOT4_LOSSREASON_PAN_ID_CONFLICT,
    I15DOT4_LOSSREASON_REALIGNMENT,
    I15DOT4_LOSSREASON_BEACON_LOST,
    I15DOT4_LOSSREASON_SUPERFRAME_OVERLAP,
};

/* CCA Mode */
typedef enum
{
    /*
     * CCA Mode 1: Energy above threshold. CCA shall report a busy medium upon detecting any energy
     *             above the ED threshold.
     */
    I15DOT4_PHY_CCA_MODE_1 = 0,

    /*
     * CCA Mode 2: Carrier sense only. CCA shall report a busy medium only upon the detection of
     *             a signal compliant with this standard with the same modulation and spreading
     *             characteristics of the PHY that is currently in use by the device.
     */
    I15DOT4_PHY_CCA_MODE_2 = 1,

    /*
     * CCA Mode 3: Carrier sense with energy above threshold. CCA shall report a busy medium using
     *             a logical combination of:
     *                 — Detection of a signal with the modulation and spreading characteristics of
     *                   this standard
     *                 — Energy above the ED threshold, where the logical operator may be AND or OR
     *
     */
    I15DOT4_PHY_CCA_MODE_3 = 2,

    /*
     * CCA Mode 4: ALOHA. CCA shall always report an idle medium.
     */
    I15DOT4_PHY_CCA_MODE_4 = 3,
} I15DOT4_PHY_CCA_MODE_t;

/* MLME/MCPS Parameter format */
typedef struct __attribute__((packed)) _I15DOT4_PARAM_HDR
{
    CONTEXT_ID_t context_id;
    uint8_t      cmd_id;
    uint8_t      data[1];
} I15DOT4_PARAM_HDR_t;

/* Index for MAC PIB Attributes */
typedef enum
{
    /* PHY PIB Attributes */
    I15DOT4_PHY_CURRENT_CHANNEL    = 0x00,
    I15DOT4_PHY_CHANNELS_SUPPORTED = 0x01,
    I15DOT4_PHY_TX_POWER           = 0x02,
    I15DOT4_PHY_CCA_MODE           = 0x03,
    I15DOT4_PHY_CURRENT_PAGE       = 0x04,
    I15DOT4_PHY_MAX_FRAME_DURATION = 0x05,
    I15DOT4_PHY_SHR_DURATION       = 0x06,
    I15DOT4_PHY_SYMBOLS_PER_OCTET  = 0x07,

    /* MAC PIB Attributes */
    I15DOT4_MAC_ACK_WAIT_DURATION            = 0x40,
    I15DOT4_MAC_ASSOCIATION_PERMIT           = 0x41,
    I15DOT4_MAC_AUTO_REQUEST                 = 0x42,
    I15DOT4_MAC_BATT_LIFE_EXT                = 0x43,
    I15DOT4_MAC_BATT_LIFE_EXT_PERIODS        = 0x44,
    I15DOT4_MAC_BEACON_PAYLOAD               = 0x45,
    I15DOT4_MAC_BEACON_PAYLOAD_LENGTH        = 0x46,
    I15DOT4_MAC_BEACON_ORDER                 = 0x47,
    I15DOT4_MAC_BEACON_TX_TIME               = 0x48,
    I15DOT4_MAC_BSN                          = 0x49,
    I15DOT4_MAC_COORD_EXTENDED_ADDRESS       = 0x4a,
    I15DOT4_MAC_COORD_SHORT_ADDRESS          = 0x4b,
    I15DOT4_MAC_DSN                          = 0x4c,
    I15DOT4_MAC_GTS_PERMIT                   = 0x4d,
    I15DOT4_MAC_MAX_CSMA_BACKOFFS            = 0x4e,
    I15DOT4_MAC_MIN_BE                       = 0x4f,
    I15DOT4_MAC_PAN_ID                       = 0x50,
    I15DOT4_MAC_PROMISCUOUS_MODE             = 0x51,
    I15DOT4_MAC_RX_ON_WHEN_IDLE              = 0x52,
    I15DOT4_MAC_MAC_SHORT_ADDRESS            = 0x53,
    I15DOT4_MAC_MAC_SUPERFRAME_ORDER         = 0x54,
    I15DOT4_MAC_TRANSACTION_PERSISTENCE_TIME = 0x55,
    I15DOT4_MAC_ASSOCIATED_PAN_COORD         = 0x56,
    I15DOT4_MAC_MAX_BE                       = 0x57,
    I15DOT4_MAC_MAX_FRAME_TOTAL_WAIT_TIME    = 0x58,
    I15DOT4_MAC_MAX_FRAME_RETRIES            = 0x59,
    I15DOT4_MAC_RESPONSE_WAIT_TIME           = 0x5a,
    I15DOT4_MAC_SYNC_SYMBOL_OFFSET           = 0x5b,
    I15DOT4_MAC_TIMESTAMP_SUPPORTED          = 0x5c,
    I15DOT4_MAC_SECURITY_ENABLED             = 0x5d,
    I15DOT4_MAC_KEY_TABLE                    = 0x71,
    I15DOT4_MAC_KEY_TABLE_ENTRIES            = 0x72,
    I15DOT4_MAC_DEVICE_TABLE                 = 0x73,
    I15DOT4_MAC_DEVICE_TABLE_ENTRIES         = 0x74,
    I15DOT4_MAC_SECURITY_LEVEL_TABLE         = 0x75,
    I15DOT4_MAC_SECURITY_LEVEL_TABLE_ENTRIES = 0x76,
    I15DOT4_MAC_FRAME_COUNTER                = 0x77,
    I15DOT4_MAC_AUTO_REQUEST_SECURITY_LEVEL  = 0x78,
    I15DOT4_MAC_AUTO_REQUEST_KEY_ID_MODE     = 0x79,
    I15DOT4_MAC_AUTO_REQUEST_KEY_SOURCE      = 0x7a,
    I15DOT4_MAC_AUTO_REQUEST_KEY_INDEX       = 0x7b,
    I15DOT4_MAC_DEFAULT_KEY_SOURCE           = 0x7c,
    I15DOT4_MAC_EXTENDED_ADDRESS             = 0x80,
#if I15DOT4_TX_SCHEDULE_ENABLE
    I15DOT4_MAC_CURRENT_TIME          = 0x81,
    I15DOT4_MAC_MAX_TX_FRAMES         = 0x82,
    I15DOT4_MAC_TX_SCHED_TIME_PREPARE = 0x83,
#endif // I15DOT4_TX_SCHEDULE_ENABLE
#if I15DOT4_TX_EPA
    I15DOT4_MAC_TX_EPA_GPIO = 0x84, // refer to I15DOT4_EPA_TX_ENABLE_t
#endif                              // I15DOT4_TX_EPA

    /* BRCM Attributes */
    I15DOT4_BRCM_BEACON_TX_COUNT       = 0x90,
    I15DOT4_BRCM_BEACON_RX_COUNT       = 0x91,
    I15DOT4_BRCM_TEST_MODE             = 0x92,
    I15DOT4_BRCM_NF_INCORRECT_PACKETS  = 0x93,
    I15DOT4_BRCM_NF_CORRECT_PACKETS    = 0x94,
    I15DOT4_BRCM_NF_RECEIVED_PACKETS   = 0x95,
    I15DOT4_BRCM_PER                   = 0x96,
    I15DOT4_BRCM_MAC_STATISTICS        = 0x97,
    I15DOT4_BRCM_INDIRECT_TX_DELAY     = 0x98,
    I15DOT4_CY_SLEEPING_NODE_ADDR_LIST = 0x99,
    I15DOT4_CY_DEFAULT_LEAVE_CMD       = 0x9a,
    I15DOT4_CY_ED_THRESHOLD            = 0x9b,
    I15DOT4_CY_CS_THRESHOLD            = 0x9c,

} I15DOT4_PIB_ATTRIBUTE_ID_t;

/* Scan type */
typedef enum
{
    I15DOT4_SCAN_TYPE_ED      = 0,
    I15DOT4_SCAN_TYPE_ACTIVE  = 1,
    I15DOT4_SCAN_TYPE_PASSIVE = 2,
    I15DOT4_SCAN_TYPE_ORPHAN  = 3,
    I15DOT4_SCAN_TYPE_NONE    = 4,
    I15DOT4_SCAN_TYPE_NUM     = 5,
} I15DOT4_SCAN_TYPE_t;

#if I15DOT4_TX_EPA

typedef struct __attribute__((packed)) _I15DOT4_EPA_TX_ENABLE
{
    uint8_t port;
    uint8_t enable;
} I15DOT4_EPA_TX_ENABLE_t;

#endif // I15DOT4_TX_EPA

typedef struct __attribute__((packed)) _I15DOT4_SECURITY_INFO
{
    I15DOT4_SECURITY_CONTROL_t ctrl;
    uint8_t                    key_source[8];
    uint8_t                    key_index;
} I15DOT4_SECURITY_INFO_t;

typedef struct __attribute__((packed)) _I15DOT4_PAN_DESCRIPTOR
{
    uint8_t                 coord_addr_mode;
    uint16_t                coord_pan_id;
    I15DOT4_ADDR_t          coord_addr;
    uint8_t                 channel_number;
    uint8_t                 channel_page;
    uint16_t                superframe_spec;
    uint8_t                 gts_permit;
    uint8_t                 link_quality;
    UINT32                  time_stamp;
    uint8_t                 security_status;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_PAN_DESCRIPTOR_t;

typedef struct __attribute__((packed)) _I15DOT4_PEND_ADDR_INFO
{
    I15DOT4_PEND_ADDR_SPEC_t spec;
    I15DOT4_ADDR_t           addr[7];
} I15DOT4_PEND_ADDR_INFO_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_HDR
{
    uint8_t context_id;
    uint8_t cmd_id;
} I15DOT4_MLME_HDR_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_ASSOC_REQ
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 channel_number;
    uint8_t                 channel_page;
    uint8_t                 coord_addr_mode;
    uint16_t                coord_pan_id;
    I15DOT4_ADDR_t          coord_addr;
    I15DOT4_CAP_INFO_t      cap_info;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_ASSOC_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_ASSOC_IND
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    UINT64                  dev_addr;
    uint8_t                 cap_info;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_ASSOC_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_ASSOC_RESP
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    I15DOT4_ADDR_t          dev_addr;
    uint16_t                assoc_short_addr;
    uint8_t                 status;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_ASSOC_RESP_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_ASSOC_CONF
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint16_t                short_addr;
    uint8_t                 status;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_ASSOC_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_DISASSOC_REQ
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 dev_addr_mode;
    uint16_t                dev_pan_id;
    I15DOT4_ADDR_t          dev_addr;
    uint8_t                 reason;
    BOOL8                   tx_indirect;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_DISASSOC_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_DISASSOC_IND
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    UINT64                  dev_addr;
    uint8_t                 reason;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_DISASSOC_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_DISASSOC_CONF
{
    uint8_t        context_id;
    uint8_t        cmd_id;
    uint8_t        status;
    uint8_t        dev_addr_mode;
    uint16_t       pan_id;
    I15DOT4_ADDR_t dev_addr;
} I15DOT4_MLME_DISASSOC_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_BEACON_NOTIFY_IND
{
    uint8_t                  context_id;
    uint8_t                  cmd_id;
    uint8_t                  bsn;
    I15DOT4_PAN_DESCRIPTOR_t desc;
    I15DOT4_PEND_ADDR_INFO_t pend_addr_info;
    uint8_t                  sdu_length;
    uint8_t                  sdu[I15DOT4_MAX_BEACON_PAYLOAD_LENGTH];
} I15DOT4_MLME_BEACON_NOTIFY_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_COMM_STATUS_IND
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint16_t                pan_id;
    uint8_t                 src_addr_mode;
    I15DOT4_ADDR_t          src_addr;
    uint8_t                 dest_addr_mode;
    I15DOT4_ADDR_t          dest_addr;
    uint8_t                 status;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_COMM_STATUS_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_GET_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t attrib_id;
} I15DOT4_MLME_GET_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_GET_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
    uint8_t attrib_id;
    uint8_t attrib_len;
    uint8_t attrib[249];
} I15DOT4_MLME_GET_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_GTS_REQ
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 gts_char;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_GTS_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_GTS_IND
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint16_t                dev_addr;
    I15DOT4_GTS_CHAR_t      gts_char;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_GTS_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_GTS_CONF
{
    uint8_t            context_id;
    uint8_t            cmd_id;
    I15DOT4_GTS_CHAR_t gts_char;
    uint8_t            status;
} I15DOT4_MLME_GTS_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_ORPHAN_IND
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    I15DOT4_EADDR_t         orphan_addr;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_ORPHAN_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_ORPHAN_RESP
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    I15DOT4_EADDR_t         orphan_addr;
    I15DOT4_SADDR_t         short_addr;
    BOOL8                   assoc_member;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_ORPHAN_RESP_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_RESET_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    BOOL8   set_default_pib;
} I15DOT4_MLME_RESET_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_RESET_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
} I15DOT4_RESET_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_RX_ENABLE_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t defer_permit;
    UINT32  rx_on_time;
    UINT32  rx_on_duration;
} I15DOT4_MLME_RX_ENABLE_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_RX_ENABLE_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
} I15DOT4_MLME_RX_ENABLE_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_SCAN_REQ
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 scan_type;
    UINT32                  scan_channel;
    uint8_t                 scan_duration;
    uint8_t                 channel_page;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_SCAN_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_SCAN_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
    uint8_t scan_type;
    uint8_t channel_page;
    UINT32  unscanned_channels;
    uint8_t result_list_size;
    union
    {
        uint8_t                  ed[32];
        I15DOT4_PAN_DESCRIPTOR_t desc[7];
    } result;
} I15DOT4_MLME_SCAN_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_SCAN_CONF_MORE
{
    uint8_t                  context_id;
    uint8_t                  cmd_id;
    uint8_t                  sn;
    uint8_t                  current_list_size;
    UINT32                   reserved1;
    UINT32                   reserved2;
    I15DOT4_PAN_DESCRIPTOR_t desc[7];
} I15DOT4_MLME_SCAN_CONF_MORE_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_SET_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t attrib_id;
    UINT32  attrib_len;
    uint8_t attrib[249];
} I15DOT4_MLME_SET_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_SET_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
    uint8_t attrib_id;
} I15DOT4_MLME_SET_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_START_REQ
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint16_t                pan_id;
    uint8_t                 channel_number;
    uint8_t                 channel_page;
    UINT32                  start_time;
    uint8_t                 beacon_order;
    uint8_t                 superframe_order;
    BOOL8                   pan_coordinator;
    BOOL8                   batt_life_ext;
    BOOL8                   coord_realignment;
    I15DOT4_SECURITY_INFO_t coord_realign_security_info;
    I15DOT4_SECURITY_INFO_t beacon_security_info;
} I15DOT4_MLME_START_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_START_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
} I15DOT4_MLME_START_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_SYNC_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t channel_number;
    uint8_t channel_page;
    uint8_t track_beacon;
} I15DOT4_MLME_SYNC_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_SYNC_LOSS_IND
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 loss_reason;
    uint16_t                pan_id;
    uint8_t                 channel_number;
    uint8_t                 channel_page;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_SYNC_LOSS_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_POLL_REQ
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 coord_addr_mode;
    uint16_t                coord_pan_id;
    I15DOT4_ADDR_t          coord_addr;
    I15DOT4_SECURITY_INFO_t security_info;
} I15DOT4_MLME_POLL_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_POLL_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
} I15DOT4_MLME_POLL_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_DATA_REQ
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 src_addr_mode;
    uint8_t                 dest_addr_mode;
    uint16_t                dest_pan_id;
    I15DOT4_ADDR_t          dest_addr;
    uint8_t                 msdu_length;
    uint8_t                 msdu_handle;
    BOOL8                   ack_tx;
    BOOL8                   gts_tx;
    BOOL8                   indirect_tx;
    I15DOT4_SECURITY_INFO_t security_info;
    uint8_t                 msdu[I15DOT4_MAX_MAC_PAYLOAD_SIZE];
} I15DOT4_MCPS_DATA_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MLME_DATA_REQ_SCHED
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 src_addr_mode;
    uint8_t                 dest_addr_mode;
    uint16_t                dest_pan_id;
    I15DOT4_ADDR_t          dest_addr;
    uint8_t                 msdu_length;
    uint8_t                 msdu_handle;
    BOOL8                   ack_tx;
    BOOL8                   gts_tx;
    BOOL8                   indirect_tx;
    I15DOT4_SECURITY_INFO_t security_info;
    UINT32                  sched_time;
    uint8_t                 skip_csma_ca;
    uint8_t                 msdu[I15DOT4_MAX_MAC_PAYLOAD_SIZE];
} I15DOT4_MCPS_DATA_REQ_SCHED_t;

typedef struct __attribute__((packed)) _I15DOT4_MCPS_DATA_IND
{
    uint8_t                 context_id;
    uint8_t                 cmd_id;
    uint8_t                 src_addr_mode;
    uint16_t                src_pan_id;
    I15DOT4_ADDR_t          src_addr;
    uint8_t                 dest_addr_mode;
    uint16_t                dest_pan_id;
    I15DOT4_ADDR_t          dest_addr;
    uint8_t                 msdu_length;
    uint8_t                 msdu_link_quality;
    uint8_t                 dsn;
    UINT32                  time_stamp;
    I15DOT4_SECURITY_INFO_t security_info;
    uint8_t                 msdu[I15DOT4_MAX_MAC_PAYLOAD_SIZE];
    uint8_t                 msdu_link_quality2;
} I15DOT4_MCPS_DATA_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_MCPS_DATA_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t msdu_handle;
    UINT32  time_stamp;
    uint8_t status;
} I15DOT4_MCPS_DATA_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_MCPS_PURGE_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t msdu_handle;
} I15DOT4_MCPS_PURGE_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_MCPS_PURGE_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t msdu_handle;
    uint8_t status;
} I15DOT4_MCPS_PURGE_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_TEST_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t param;
    UINT32  data_len;
    uint8_t data[249];
} I15DOT4_TEST_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_TEST_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t param;
    uint8_t status;
    uint8_t data_len;
    uint8_t data[249];
} I15DOT4_TEST_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_PHY_TX_TEST_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    UINT32  nf_packet;
    UINT32  ifs;
    uint8_t packet_len;
    uint8_t pattern_type;
} I15DOT4_PHY_TX_TEST_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_PHY_TEST_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
} I15DOT4_BRCM_PHY_TEST_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_PER_RESET_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t reset_param;
} I15DOT4_PER_RESET_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_KEY_ID_LOOKUP_DESC
{
    uint8_t key_id_mode;

    union
    {
        struct __attribute__((packed))
        {
            uint8_t        dev_addr_mode;
            uint16_t       dev_pan_id;
            I15DOT4_ADDR_t dev_addr;
        } mode0;

        struct __attribute__((packed))
        {
            uint8_t key_index;
            uint8_t reserved[10];
        } mode1;

        struct __attribute__((packed))
        {
            uint8_t key_index;
            uint8_t key_source[8];
            uint8_t reserved[2];
        } mode2_3;
    } u;
} I15DOT4_KEY_ID_LOOKUP_DESC_t;

typedef struct __attribute__((packed)) _I15DOT4_DEVICE_DESC
{
    uint16_t        pan_id;
    uint16_t        short_addr;
    I15DOT4_EADDR_t ext_addr;
    UINT32          frame_counter;
    BOOL8           exempt;
} I15DOT4_DEVICE_DESC_t;

typedef struct __attribute__((packed)) _I15DOT4_DEVICE_DESC_TABLE
{
    uint8_t               num;
    I15DOT4_DEVICE_DESC_t desc[14];
} I15DOT4_DEVICE_DESC_TABLE_t;

typedef struct __attribute__((packed)) _I15DOT4_SECURITY_LEVEL_DESC
{
    uint8_t frame_type;
    uint8_t cfid;
    uint8_t sec_min;
    uint8_t override_sec_min;
    uint8_t allowed_sec_levels; /* bitmap */
} I15DOT4_SECURITY_LEVEL_DESC_t;

typedef struct __attribute__((packed)) _I15DOT4_SECURITY_LEVEL_DESC_TABLE
{
    uint8_t                       num;
    I15DOT4_SECURITY_LEVEL_DESC_t desc[49];
} I15DOT4_SECURITY_LEVEL_DESC_TABLE_t;

typedef struct __attribute__((packed)) _I15DOT4_KEY_USAGE_DESC
{
    uint8_t frame_type;
    uint8_t cfid;
} I15DOT4_KEY_USAGE_DESC_t;

typedef struct __attribute__((packed)) _I15DOT4_KEY_DESC
{
    uint8_t                      key_lookup_num;
    I15DOT4_KEY_ID_LOOKUP_DESC_t key_lookup[15];
    uint8_t                      dev_desc_handle_num;
    uint8_t                      dev_desc_handle[15];
    uint8_t                      key_usage_num;
    I15DOT4_KEY_USAGE_DESC_t     key_usage[15];
    uint8_t                      key[16];
} I15DOT4_KEY_DESC_t;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t param;
    uint8_t status;
    uint8_t data_len;
    uint8_t data[249];
} I15DOT4_BRCM_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_CLEAN_ALL_KEY_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    UINT32  reserved;
} I15DOT4_BRCM_CLEAN_ALL_KEY_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_ADD_KEY_REQ
{
    uint8_t                  context_id;
    uint8_t                  cmd_id;
    uint8_t                  key[16];
    uint8_t                  key_usage_num;
    I15DOT4_KEY_USAGE_DESC_t key_usage[12];
} I15DOT4_BRCM_ADD_KEY_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_DEL_KEY_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t key[16];
} I15DOT4_BRCM_DEL_KEY_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_ADD_KEY_ID_LOOKUP_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t key[16];
    uint8_t lookup_data[9];
    uint8_t lookup_size;
} I15DOT4_BRCM_ADD_KEY_ID_LOOKUP_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_DEL_KEY_ID_LOOKUP_REQ
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t key[16];
    uint8_t lookup_data[9];
    uint8_t lookup_size;
} I15DOT4_BRCM_DEL_KEY_ID_LOOKUP_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_ADD_KEY_DEVICE_REQ
{
    uint8_t  context_id;
    uint8_t  cmd_id;
    uint8_t  key[16];
    uint16_t device_handle;
    BOOL8    unique_device;
    BOOL8    blacklisted;
} I15DOT4_BRCM_ADD_KEY_DEVICE_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_DEL_KEY_DEVICE_REQ
{
    uint8_t  context_id;
    uint8_t  cmd_id;
    uint8_t  key[16];
    uint16_t device_handle;
} I15DOT4_BRCM_DEL_KEY_DEVICE_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_ADD_DEVICE_REQ
{
    uint8_t               context_id;
    uint8_t               cmd_id;
    uint16_t              device_handle;
    I15DOT4_DEVICE_DESC_t device_desc;
} I15DOT4_BRCM_ADD_DEVICE_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_DEL_DEVICE_REQ
{
    uint8_t  context_id;
    uint8_t  cmd_id;
    uint16_t device_handle;
} I15DOT4_BRCM_DEL_DEVICE_REQ;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_POLL_IND
{
    uint8_t        context_id;
    uint8_t        cmd_id;
    uint8_t        src_addr_mode;
    I15DOT4_ADDR_t src_addr;
} I15DOT4_BRCM_POLL_IND;

typedef struct __attribute__((packed)) M154_TXDATA_DEBUG
{
    uint8_t backoff_retry_cnt[8];
    uint8_t ed_value[8][6]; /* the array length = MAX(g_i15dot4_mac_max_csma_backoffs) + 1 */
    uint8_t cs_value[8][6];
} M154_TX_DATA_DEBUG;

typedef struct __attribute__((packed)) _I15DOT4_BRCM_DATA_CONF_DEBUG
{
    uint8_t            context_id;
    uint8_t            cmd_id;
    M154_TX_DATA_DEBUG txdata_debug;
} I15DOT4_BRCM_DATA_CONF_DEBUG;

typedef struct __attribute__((packed)) _I15DOT4_SLEEPING_NODE_ADDR_LIST
{
    uint8_t  num;
    uint16_t addr[100];
} I15DOT4_SLEEPING_NODE_ADDR_LIST;

typedef struct __attribute__((packed)) _I15DOT4_DEFAULT_LEAVE_CMD
{
    uint8_t len;
    uint8_t dst_addr_offset;
    uint8_t dst_addr_len;
    uint8_t msdu[127];
} I15DOT4_DEFAULT_LEAVE_CMD;

typedef struct __attribute__((packed)) _I15DOT4_THREAD_DATA_REQ
{
    uint8_t  context_id;
    uint8_t  cmd_id;
    uint8_t  handle;
    uint8_t  key[16];
    uint32_t tx_delay;
    uint32_t tx_delay_base_time;
    uint8_t  max_csma_backoffs;
    uint8_t  max_frame_retries;
    uint8_t  csma_ca_enabled : 1;
    uint8_t  csl_present : 1;
    uint8_t  security_enable : 1;
#if I15DOT4_TX_SCHEDULE_ENABLE
    uint8_t tx_sched : 1;
    uint8_t reserved : 4;
#else  // !I15DOT4_TX_SCHEDULE_ENABLE
    uint8_t reserved : 5;
#endif // I15DOT4_TX_SCHEDULE_ENABLE
    uint8_t key_id_mode;
    uint8_t key_index;
    uint8_t sec_hdr_offset;
    uint8_t sec_hdr_len;
    uint8_t payload_offset;
    uint8_t payload_len;
    uint8_t psdu_length;
    uint8_t psdu[I15DOT4_MAX_PHY_PACKET_SIZE - I15DOT4_FCS_LENGTH];
} I15DOT4_THREAD_DATA_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_THREAD_DATA_CONF
{
    uint8_t  context_id;
    uint8_t  cmd_id;
    uint8_t  handle;
    uint8_t  status;
    uint32_t frame_counter;
#if !I15DOT4_VARAIABLE_ACK_LENGTH
    uint8_t reserved;
#endif
#if I15DOT4_TX_SCHEDULE_ENABLE
    uint64_t time_stamp;
#else  // !I15DOT4_TX_SCHEDULE_ENABLE
    uint32_t time_stamp;
#endif // I15DOT4_TX_SCHEDULE_ENABLE
#if I15DOT4_VARAIABLE_ACK_LENGTH
    uint8_t ack_frame_length;
#endif
    uint8_t ack_frame[I15DOT4_ACK_LENGTH];
} I15DOT4_THREAD_DATA_CONF_t;

typedef struct __attribute__((packed)) _I15DOT4_THREAD_DATA_IND
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t rssi;
#if I15DOT4_TX_SCHEDULE_ENABLE
    uint64_t symbol_counter;
#else  // !I15DOT4_TX_SCHEDULE_ENABLE
    uint32_t symbol_counter;
#endif // I15DOT4_TX_SCHEDULE_ENABLE
    uint32_t ack_frame_pending : 1;
    uint32_t reserved : 31;
    uint8_t  psdu_length;
    uint8_t  psdu[I15DOT4_MAX_PHY_PACKET_SIZE];
} I15DOT4_THREAD_DATA_IND_t;

typedef struct __attribute__((packed)) _I15DOT4_THREAD_ADDR_MATCH_REQ
{
    uint8_t        context_id;
    uint8_t        cmd_id;
    uint8_t        action;
    I15DOT4_ADDR_t addr;
    uint8_t        flag; // refer to ADDR_MATCH_TYPE_t
    uint8_t        buf_len;
    uint8_t        buf[16];
} I15DOT4_THREAD_ADDR_MATCH_REQ_t;

typedef struct __attribute__((packed)) _I15DOT4_THREAD_ADDR_MATCH_CONF
{
    uint8_t context_id;
    uint8_t cmd_id;
    uint8_t status;
} I15DOT4_THREAD_ADDR_MATCH_CONF_t;

#endif /* _I15DOT4_H_ */
