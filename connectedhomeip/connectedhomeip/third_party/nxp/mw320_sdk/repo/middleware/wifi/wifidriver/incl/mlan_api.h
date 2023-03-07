/** @file mlan_api.h
 *
 *  @brief MLAN Interface
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

#ifndef __MLAN_API_H__
#define __MLAN_API_H__
#include <string.h>
#include <wmtypes.h>
#include <wlan.h>

#include "fsl_debug_console.h"

#define MLAN_WMSDK_MAX_WPA_IE_LEN 256

#ifdef CONFIG_EXT_SCAN
#define EXT_SCAN_SUPPORT
#endif

#include "mlan.h"
#include "mlan_join.h"
#include "mlan_util.h"
#include "mlan_fw.h"
#include "mlan_main.h"
#include "mlan_wmm.h"
#include "mlan_11n.h"
#include "mlan_11h.h"
#include "mlan_11ac.h"
#include "mlan_decl.h"
#include "mlan_11n_aggr.h"
#include "mlan_sdio.h"
#include "mlan_11n_rxreorder.h"
#include "mlan_meas.h"
#include "mlan_ioctl.h"
#include "mlan_uap.h"
#include <wifi-debug.h>
#include "wifi-internal.h"

/* #define CONFIG_WIFI_DEBUG */

#ifdef CONFIG_WIFI_DEBUG
/* #define DEBUG_11N_ASSOC */
/* #define DEBUG_11N_AGGR */
/* #define DEBUG_11N_REORDERING */
#define DEBUG_MLAN
/* #define DEBUG_DEVELOPMENT */
/* #define DUMP_PACKET_MAC */
#endif /* CONFIG_WIFI_DEBUG */

#ifdef ENTER
#undef ENTER
#define ENTER(...)
#endif /* ENTER */

#ifdef EXIT
#undef EXIT
#define EXIT(...)
#endif /* EXIT */

#ifdef DEBUG_MLAN

#ifdef PRINTM
#undef PRINTM
#define PRINTM(level, ...)               \
    do                                   \
    {                                    \
        wmprintf("[mlan] " __VA_ARGS__); \
        wmprintf("\n\r");                \
    } while (0)
#else
#define PRINTM(...)
#endif /* PRINTM */

#ifdef DBG_HEXDUMP
#undef DBG_HEXDUMP
#define DBG_HEXDUMP(level, x, y, z)   \
    do                                \
    {                                 \
        wmprintf("[mlan] %s\r\n", x); \
        dump_hex(y, z);               \
        wmprintf("\r\n");             \
    } while (0)
#else
#define DBG_HEXDUMP(...)
#endif /* DBG_HEXDUMP */

#ifdef HEXDUMP
#undef HEXDUMP
#define HEXDUMP(x, y, z)              \
    do                                \
    {                                 \
        wmprintf("[mlan] %s\r\n", x); \
        dump_hex(y, z);               \
        wmprintf("\r\n");             \
    } while (0)
#else
#define HEXDUMP(...)
#endif /* HEXDUMP */
#endif /* DEBUG_MLAN */

#define DOT11N_CFG_ENABLE_RIFS            0x08
#define DOT11N_CFG_ENABLE_GREENFIELD_XMIT (1 << 4)
#define DOT11N_CFG_ENABLE_SHORT_GI_20MHZ  (1 << 5)
#define DOT11N_CFG_ENABLE_SHORT_GI_40MHZ  (1 << 6)

#define CLOSEST_DTIM_TO_LISTEN_INTERVAL 65534

#define SDIO_DMA_ALIGNMENT 4

/* Following is allocated in mlan_register */
extern mlan_adapter *mlan_adap;

extern os_rw_lock_t ps_rwlock;

extern bool sta_ampdu_rx_enable;

void dump_mac_addr(const char *msg, unsigned char *addr);
void dump_htcap_info(const MrvlIETypes_HTCap_t *htcap);
void dump_ht_info(const MrvlIETypes_HTInfo_t *htinfo);

mlan_status wifi_prepare_and_send_cmd(IN mlan_private *pmpriv,
                                      IN t_u16 cmd_no,
                                      IN t_u16 cmd_action,
                                      IN t_u32 cmd_oid,
                                      IN t_void *pioctl_buf,
                                      IN t_void *pdata_buf,
                                      int bss_type,
                                      void *priv);
int wifi_uap_prepare_and_send_cmd(mlan_private *pmpriv,
                                  t_u16 cmd_no,
                                  t_u16 cmd_action,
                                  t_u32 cmd_oid,
                                  t_void *pioctl_buf,
                                  t_void *pdata_buf,
                                  int bss_type,
                                  void *priv);

bool wmsdk_is_11N_enabled(void);

/**
 * Abort the split scan if it is in progress.
 *
 * After this call returns this scan function will abort the current split
 * scan and return back to the caller. The scan list may be incomplete at
 * this moment. There are no other side effects on the scan function apart
 * from this. The next call to scan function should proceed as normal.
 */
void wlan_abort_split_scan(void);

void wlan_scan_process_results(IN mlan_private *pmpriv);

bool check_for_wpa2_entp_ie(bool *wpa2_entp_IE_exist, const void *element_data, unsigned element_len);

static inline mlan_status wifi_check_bss_entry_wpa2_entp_only(BSSDescriptor_t *pbss_entry, t_u8 element_id)
{
    if (element_id == RSN_IE)
    {
        check_for_wpa2_entp_ie(&pbss_entry->wpa2_entp_IE_exist, pbss_entry->rsn_ie_buff + 8,
                               pbss_entry->rsn_ie_buff_len - 10);
    }
    return MLAN_STATUS_SUCCESS;
}
#endif /* __MLAN_API_H__ */
