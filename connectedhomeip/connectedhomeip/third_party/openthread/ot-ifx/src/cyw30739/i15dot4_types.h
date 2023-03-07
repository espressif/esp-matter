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

#ifndef _I15DOT4_TYPES_H_
#define _I15DOT4_TYPES_H_

#include <data_types.h>

/* Frame Type */
enum
{
    I15DOT4_FRAME_TYPE_BEACON  = 0,
    I15DOT4_FRAME_TYPE_DATA    = 1,
    I15DOT4_FRAME_TYPE_ACK     = 2,
    I15DOT4_FRAME_TYPE_MAC_CMD = 3,
};

/* Address mode */
enum
{
    I15DOT4_ADDR_MODE_NO_ADDR  = 0,
    I15DOT4_ADDR_MODE_SHORT    = 2,
    I15DOT4_ADDR_MODE_EXTENDED = 3
};

typedef uint16_t I15DOT4_SADDR_t;
typedef uint64_t I15DOT4_EADDR_t;

typedef union _I15DOT4_ADDR
{
    I15DOT4_SADDR_t short_addr;
    I15DOT4_EADDR_t ext_addr;
} I15DOT4_ADDR_t;

typedef struct _I15DOT4_ADDR_INFO
{
    uint8_t        mode;
    I15DOT4_ADDR_t addr;
} I15DOT4_ADDR_INFO_t;

/* Command Frame Identifier */
typedef enum _I15DOT4_CFID
{
    I15DOT4_CFID_ASSOCIATION_REQUEST          = 0x01,
    I15DOT4_CFID_ASSOCIATION_RESPONSE         = 0x02,
    I15DOT4_CFID_DISASSOC_NOTIFICATION        = 0x03,
    I15DOT4_CFID_DATA_REQUEST                 = 0x04,
    I15DOT4_CFID_PAN_ID_CONFLICT_NOTIFICATION = 0x05,
    I15DOT4_CFID_ORPHAN_NOTIFICATION          = 0x06,
    I15DOT4_CFID_BEACON_REQUEST               = 0x07,
    I15DOT4_CFID_COORDINATOR_REALIGNMENT      = 0x08,
    I15DOT4_CFID_GTS_REQUEST                  = 0x09,
} I15DOT4_CFID_t;

typedef union _I15DOT4_FRAME_CTRL
{
    struct __attribute__((packed))
    {
        uint16_t frame_type : 3;
        uint16_t security_enable : 1;
        uint16_t frame_pending : 1;
        uint16_t ack_req : 1;
        uint16_t pan_id_compress : 1;
        uint16_t reserved : 3;
        uint16_t dest_addr_mode : 2;
        uint16_t frame_version : 2;
        uint16_t src_addr_mode : 2;
    } field;
    uint16_t value;
} I15DOT4_FRAME_CTRL_t;

typedef union _I15DOT4_SUPERFRAME_SPEC
{
    struct __attribute__((packed)) _I15DOT4_SUPERFRAME_SPEC_STRUCT
    {
        uint16_t beacon_order : 4;
        uint16_t superframe_order : 4;
        uint16_t final_cap_slot : 4;
        uint16_t batt_life_ext : 1;
        uint16_t reserved : 1;
        uint16_t pan_coordinator : 1;
        uint16_t assoc_permit : 1;
    } field;
    uint16_t value;
} I15DOT4_SUPERFRAME_SPEC_t;

typedef union _I15DOT4_GTS_SPEC
{
    struct __attribute__((packed)) _I15DOT4_GTS_SPEC_STRUCT
    {
        uint8_t gts_desc_count : 3;
        uint8_t reserved : 4;
        uint8_t gts_permit : 1;
    } field;
    uint8_t value;
} I15DOT4_GTS_SPEC_t;

typedef union _I15DOT4_GTS_CHAR
{
    struct __attribute__((packed)) _I15DOT4_GTS_CHAR_STRUCT
    {
        uint8_t gts_length : 4;
        uint8_t gts_direct : 1;
        uint8_t char_type : 1;
        uint8_t reserved : 2;
    } field;
    uint8_t value;
} I15DOT4_GTS_CHAR_t;

enum
{
    GTS_DIRECT_RX = 0,
    GTS_DIRECT_TX = 1,
};

typedef union _I15DOT4_PEND_ADDR_SPEC
{
    struct __attribute__((packed))
    {
        uint8_t nf_short_addr : 3;
        uint8_t reserved : 1;
        uint8_t nf_ext_addr : 3;
        uint8_t reserved2 : 1;
    } field;
    uint8_t value;
} I15DOT4_PEND_ADDR_SPEC_t;

typedef union _I15DOT4_CAP_INFO
{
    struct __attribute__((packed)) _I15DOT4_CAP_INFO_STRUCT
    {
        uint8_t reserved : 1;
        uint8_t device_type : 1;
        uint8_t power_source : 1;
        uint8_t rx_on_when_idle : 1;
        uint8_t reserved2 : 2;
        uint8_t security_cap : 1;
        uint8_t alloc_addr : 1;
    } field;
    uint8_t value;
} I15DOT4_CAP_INFO_t;

typedef struct __attribute__((packed)) _I15DOT4_MAC_HEADER
{
    I15DOT4_FRAME_CTRL_t fctrl;
    uint8_t              sn;
    union
    {
        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint16_t dest_short_addr;
            uint16_t src_pan_id;
            uint16_t src_short_addr;
        } addr_mode_22;

        struct __attribute__((packed))
        {
            uint16_t src_pan_id;
            uint16_t src_short_addr;
        } addr_mode_02;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint16_t dest_short_addr;
        } addr_mode_20;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint64_t dest_ext_addr;
            uint16_t src_pan_id;
            uint64_t src_ext_addr;
        } addr_mode_33;

        struct __attribute__((packed))
        {
            uint16_t src_pan_id;
            uint64_t src_ext_addr;
        } addr_mode_03;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint64_t dest_ext_addr;
        } addr_mode_30;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint64_t dest_ext_addr;
            uint16_t src_pan_id;
            uint16_t src_short_addr;
        } addr_mode_32;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint16_t dest_short_addr;
            uint16_t src_pan_id;
            uint64_t src_ext_addr;
        } addr_mode_23;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint16_t dest_short_addr;
            uint16_t src_short_addr;
        } addr_mode_22_compress;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint64_t dest_ext_addr;
            uint16_t src_short_addr;
        } addr_mode_32_compress;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint16_t dest_short_addr;
            uint64_t src_ext_addr;
        } addr_mode_23_compress;

        struct __attribute__((packed))
        {
            uint16_t dest_pan_id;
            uint64_t dest_ext_addr;
            uint64_t src_ext_addr;
        } addr_mode_33_compress;
    } addr;
} I15DOT4_MAC_HEADER_t;

typedef union _I15DOT4_SECURITY_CONTROL
{
    struct __attribute__((packed)) _I15DOT4_SECURITY_CONTROL_STRUCT
    {
        uint8_t security_level : 3;
        uint8_t key_id_mode : 2;
        uint8_t reserved : 3;
    } field;
    uint8_t value;
} I15DOT4_SECURITY_CONTROL_t;

typedef union _I15DOT4_SECURITY_HEADER
{
    I15DOT4_SECURITY_CONTROL_t ctrl;

    struct __attribute__((packed))
    {
        I15DOT4_SECURITY_CONTROL_t ctrl;
        uint32_t                   frame_counter;
    } mode0;

    struct __attribute__((packed))
    {
        I15DOT4_SECURITY_CONTROL_t ctrl;
        uint32_t                   frame_counter;
        uint8_t                    key_index;
    } mode1;

    struct __attribute__((packed))
    {
        I15DOT4_SECURITY_CONTROL_t ctrl;
        uint32_t                   frame_counter;
        uint8_t                    key_source[4];
        uint8_t                    key_index;
    } mode2;

    struct __attribute__((packed))
    {
        I15DOT4_SECURITY_CONTROL_t ctrl;
        uint32_t                   frame_counter;
        uint8_t                    key_source[8];
        uint8_t                    key_index;
    } mode3;
} I15DOT4_SECURITY_HEADER_t;

typedef struct __attribute__((packed)) _I15DOT4_ASSOC_RESP_CMD
{
    uint8_t  cfid;
    uint16_t short_addr;
    uint8_t  status;
} I15DOT4_ASSOC_RESP_CMD_t;

typedef struct __attribute__((packed)) _I15DOT4_DISASSOC_NOTIFY_CMD
{
    uint8_t cfid;
    uint8_t reason;
} I15DOT4_DISASSOC_NOTIFY_CMD_t;

typedef struct __attribute__((packed)) _I15DOT4_REALIGN_CMD
{
    uint8_t  cfid;
    uint16_t pan_id;
    uint16_t coord_short_addr;
    uint8_t  channel_number;
    uint16_t short_addr;
    uint8_t  channel_page;
} I15DOT4_REALIGN_CMD;

typedef struct __attribute__((packed)) _I15DOT4_ACK_FRAME
{
    I15DOT4_FRAME_CTRL_t fctrl;
    uint8_t              sn;
} I15DOT4_ACK_FRAME_t;
#endif /* _I15DOT4_TYPES_H_ */
