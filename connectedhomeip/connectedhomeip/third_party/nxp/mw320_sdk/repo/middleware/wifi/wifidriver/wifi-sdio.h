/** @file wifi-sdio.h
 *
 *  @brief WLAN on SDIO
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#ifndef __WIFI_SDIO_H__
#define __WIFI_SDIO_H__

/* fixme: remove this as soon as there is no dependancy */
//#define INCLUDE_FROM_MLAN

#include <wifi.h>

#define wifi_io_e(...) wmlog_e("wifi_io", ##__VA_ARGS__)
#define wifi_io_w(...) wmlog_w("wifi_io", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_IO_DEBUG
#define wifi_io_d(...) wmlog("wifi_io", ##__VA_ARGS__)
#else
#define wifi_io_d(...)
#endif /* ! CONFIG_WIFI_IO_DEBUG */

#define wifi_io_info_e(...) wmlog_e("wpkt", ##__VA_ARGS__)
#define wifi_io_info_w(...) wmlog_w("wpkt", ##__VA_ARGS__)

#ifdef CONFIG_WIFI_IO_INFO_DUMP
#define wifi_io_info_d(...) wmlog("wpkt", ##__VA_ARGS__)
#else
#define wifi_io_info_d(...)
#endif

#define WLAN_MAGIC_NUM (('W' << 0) | ('L' << 8) | ('F' << 16) | ('W' << 24))

#define SDIO_OUTBUF_LEN 2048

/* fixme: sizeof(HostCmd_DS_COMMAND) is 1132 bytes. So have kept this at
   the current size.
*/
#define WIFI_FW_CMDBUF_SIZE 1400
#define WIFI_RESP_WAIT_TIME 10

#define SDIO_INBUF_LEN (2048 * 2)

#if (SDIO_INBUF_LEN % MLAN_SDIO_BLOCK_SIZE)
#error "Please keep buffer length aligned to SDIO block size"
#endif /* Sanity check */

#if (SDIO_OUTBUF_LEN % MLAN_SDIO_BLOCK_SIZE)
#error "Please keep buffer length aligned to SDIO block size"
#endif /* Sanity check */

// extern uint8_t outbuf[SDIO_OUTBUF_LEN];

typedef struct wlanfw_hdr
{
    t_u32 magic_number;
    t_u32 len;
} wlanfw_hdr_type;

/** Card Control Registers : Function 1 Block size 0 */
#define FN1_BLOCK_SIZE_0 0x110
/** Card Control Registers : Function 1 Block size 1 */
#define FN1_BLOCK_SIZE_1 0x111

/* Duplicated in wlan.c. keep in sync till we can be included directly */
typedef struct __nvram_backup_struct
{
    t_u32 ioport;
    t_u32 curr_wr_port;
    t_u32 curr_rd_port;
    t_u32 mp_end_port;
    t_u32 bss_num;
    t_u32 sta_mac_addr1;
    t_u32 sta_mac_addr2;
    t_u32 wifi_state;
} nvram_backup_t;

extern os_thread_t wifi_core_thread;
extern bool g_txrx_flag;
#ifdef WLAN_LOW_POWER_ENABLE
extern bool low_power_mode;
#endif
extern bool cal_data_valid;
extern bool mac_addr_valid;
extern bool txpwrlimit_data_valid;
extern uint8_t trpc_country;

mlan_status sd_wifi_init(enum wlan_type type,
                         enum wlan_fw_storage_type st,
                         const uint8_t *fw_ram_start_addr,
                         const size_t size);
void sd_wifi_deinit(void);

/*
 * @internal
 *
 *
 */
int wlan_send_sdio_cmd(t_u8 *buf, t_u32 tx_blocks, t_u32 buflen);

/*
 * @internal
 *
 *
 */
int wifi_send_cmdbuffer(t_u32 tx_blocks, t_u32 len);

/*
 * @internal
 *
 *
 */
HostCmd_DS_COMMAND *wifi_get_command_buffer(void);

mlan_status wlan_xmit_pkt(t_u32 txlen, t_u8 interface);
int raw_process_pkt_hdrs(void *pbuf, t_u32 payloadlen, t_u8 interface);
uint32_t wifi_get_device_value1();

uint8_t *wifi_get_sdio_outbuf(uint32_t *outbuf_len);

void sdio_enable_interrupt();

#ifdef CONFIG_WIFI_FW_DEBUG
extern void wifi_dump_firmware_info();
extern void wifi_sdio_reg_dbg();
#endif /* CONFIG_WIFI_FW_DEBUG */

#endif /* __WIFI_SDIO_H__ */
