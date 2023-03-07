/**
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef __PICUS_H_
#define __PICUS_H_

#ifdef MTK_BT_MEM_SHRINK
#define MTK_BT_PICUS_CLI_LITE
#endif

#ifdef CHIP_MT7933
#define PICUS_CHIPID  (0x7933)
#else
#define PICUS_CHIPID  (0x6631)
#endif

#define NONECOLOR       "\033[m"
#define GRAY            "\033[1;30m"
#define RED             "\033[0;31m"
#define LIGHT_RED       "\033[1;31m"
#define GREEN           "\033[0;32m"
#define LIGHT_GREEN     "\033[1;32m"
#define BROWN           "\033[0;33m"
#define YELLOW          "\033[1;33m"
#define BLUE            "\033[0;34m"
#define LIGHT_BLUE      "\033[1;34m"
#define PURPLE          "\033[0;35m"
#define LIGHT_PURPLE    "\033[1;35m"
#define CYAN            "\033[0;36m"
#define LIGHT_CYAN      "\033[1;36m"
#define LIGHT_WHITE     "\033[1;37m"

#define LINK_NUM_RSSI           6

// picus send hci cmds
#define HCI_GRP_VENDOR_SPEC (0x3F << 10) // 0xFC00
#define HCI_VND_PER_STATISTIC (0x0111 | HCI_GRP_VENDOR_SPEC)
#define HCI_VND_READ_RSSI (0x0061 | HCI_GRP_VENDOR_SPEC)

// PER information define
#define PER_PREFIX_LEN          16
#define PER_LEGACY_GROUP_LEN    26
#define PER_GROUP_LEN           27

//#define PICUS_WITHOUT_BUFFER
typedef enum _PICUS_DEBUG_LEVEL
{
    SHOW = 0,   // debug off, priority highest
    ERROR,      // only show eror
    WARN,
    TRACE,
    DEEPTRACE,
    HCITRACE,
} PICUS_DEBUG_LEVEL;

enum
{
    FWLOG_LVL_OFF = 0,
    FWLOG_LVL_LOW_POWER,
    FWLOG_LVL_SQC,
    FWLOG_LVL_FULL,  //All log
};
enum
{
    FWLOG_VIA_EVENT = 0,
    FWLOG_VIA_EMI,
    FWLOG_VIA_ACL,
    FWLOG_VIA_UART,
};

enum
{
    FWLOG_UART_BAUDRATE_115200 = 1,
    FWLOG_UART_BAUDRATE_921600,
    FWLOG_UART_BAUDRATE_3000000,
};

enum
{
    PKT_FWLOG = 0,
    PKT_COREDUMP,
};

enum
{
    LOG_TO_FS = 0,
    LOG_TO_UART,
};

enum
{
    LOG_START = 0,
    LOG_DUMPING,
    LOG_END,
};

enum
{
    DUMP_PLAIN = 0x0B,
    DUMP_CR,
    DUMP_MEM,
};

#ifdef PICUS_WITHOUT_BUFFER
typedef struct {
    unsigned char b_valid;
    unsigned int len;   //length of packet
    unsigned char *packet; //header + hci packet, dynamically alloacted and freed
}FWLOG_PACKET_T;
#else
#define FWLOG_BUF_SIZE 1024 * 8
typedef struct {
    unsigned int write_idx;
    unsigned int size;
    unsigned char *buffer;
}FWLOG_PACKET_T;
#endif

int picus_cmd_handler(int argc, char *argv[]);
void picus_coredump_out(unsigned char dump_type, unsigned char *dump_info, unsigned char *data, unsigned int len);

#endif

//---------------------------------------------------------------------------
