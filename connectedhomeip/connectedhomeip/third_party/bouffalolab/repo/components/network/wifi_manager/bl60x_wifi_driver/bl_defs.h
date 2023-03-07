/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __RWNX_DEFS_H__
#define __RWNX_DEFS_H__
#include "errno.h"
#include "ipc_host.h"
#include "ipc_shared.h"
#include "bl_cmds.h"
#include "bl_mod_params.h"

#define ETH_ALEN    6       
/**
 ****************************************************************************************
 *
 * @file bl_defs.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#define ETH_P_PAE   0x888E      /* Port Access Entity (IEEE 802.1X) */

#include "ieee80211.h"
#include "cfg80211.h"
#include "nl80211.h"

//#define CONFIG_RWNX_DBG

#define RWNX_RXBUFF_MAX (4 * 1)
#define IEEE80211_MIN_AMPDU_BUF 0x8
#define IEEE80211_MAX_AMPDU_BUF 0x40
#define NX_VIRT_DEV_MAX 2
#define NX_REMOTE_STA_MAX CFG_STA_MAX
#define CONFIG_USER_MAX 1
#define cpu_to_le16(v16) (v16)
#define cpu_to_le32(v32) (v32)
#define le16_to_cpu(v16) (v16)
#define le32_to_cpu(v32) (v32)
#define RWNX_TX_LIFETIME_MS             100

#define IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT   2

#define RWNX_HWTXHDR_ALIGN_SZ           4
#define RWNX_HWTXHDR_ALIGN_MSK (RWNX_HWTXHDR_ALIGN_SZ - 1)
#define RWNX_HWTXHDR_ALIGN_PADS(x) \
                    ((RWNX_HWTXHDR_ALIGN_SZ - ((x) & RWNX_HWTXHDR_ALIGN_MSK)) \
                     & RWNX_HWTXHDR_ALIGN_MSK)

#define REG_SW_SET_PROFILING(env, value)   do{  }while(0)
#define REG_SW_CLEAR_PROFILING(env, value)   do{  }while(0)

enum {
    SW_PROF_HOSTBUF_IDX = 12,
    /****** IPC IRQs related signals ******/
    /* E2A direction */
    SW_PROF_IRQ_E2A_RXDESC = 16,    // to make sure we let 16 bits available for LMAC FW
    SW_PROF_IRQ_E2A_TXCFM,
    SW_PROF_IRQ_E2A_DBG,
    SW_PROF_IRQ_E2A_MSG,
    SW_PROF_IPC_MSGPUSH,
    SW_PROF_MSGALLOC,
    SW_PROF_MSGIND,
    SW_PROF_DBGIND,

    /* A2E direction */
    SW_PROF_IRQ_A2E_TXCFM_BACK,

    /****** Driver functions related signals ******/
    SW_PROF_WAIT_QUEUE_STOP,
    SW_PROF_WAIT_QUEUE_WAKEUP,
    SW_PROF_RWNXDATAIND,
    SW_PROF_RWNX_IPC_IRQ_HDLR,
    SW_PROF_RWNX_IPC_THR_IRQ_HDLR,
    SW_PROF_IEEE80211RX,
    SW_PROF_RWNX_PATTERN,
    SW_PROF_MAX
};

/*
 *  * Maximum length of AMPDU that the STA can receive.
 *   * Length = 2 ^ (13 + max_ampdu_length_exp) - 1 (octets)
 *    */
enum ieee80211_max_ampdu_length_exp {
    IEEE80211_HT_MAX_AMPDU_8K = 0,
    IEEE80211_HT_MAX_AMPDU_16K = 1,
    IEEE80211_HT_MAX_AMPDU_32K = 2,
    IEEE80211_HT_MAX_AMPDU_64K = 3
};


/* U-APSD queues for WMM IEs sent by STA */
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VO  (1<<0)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_VI  (1<<1)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BK  (1<<2)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_BE  (1<<3)
#define IEEE80211_WMM_IE_STA_QOSINFO_AC_MASK    0x0f
/**
* enum ieee80211_vht_mcs_support - VHT MCS support definitions
* @IEEE80211_VHT_MCS_SUPPORT_0_7: MCSes 0-7 are supported for the
* number of streams
* @IEEE80211_VHT_MCS_SUPPORT_0_8: MCSes 0-8 are supported
* @IEEE80211_VHT_MCS_SUPPORT_0_9: MCSes 0-9 are supported
* @IEEE80211_VHT_MCS_NOT_SUPPORTED: This number of streams isn't supported
*
* These definitions are used in each 2-bit subfield of the @rx_mcs_map
* and @tx_mcs_map fields of &struct ieee80211_vht_mcs_info, which are
* both split into 8 subfields by number of streams. These values indicate
* which MCSes are supported for the number of streams the value appears
* for.
*/
enum ieee80211_vht_mcs_support {
    IEEE80211_VHT_MCS_SUPPORT_0_7   = 0,
    IEEE80211_VHT_MCS_SUPPORT_0_8   = 1,
    IEEE80211_VHT_MCS_SUPPORT_0_9   = 2,
    IEEE80211_VHT_MCS_NOT_SUPPORTED = 3,
};

enum RWNX_INTERFACE_STATUS {
    RWNX_INTERFACE_STATUS_DOWN = 0,
    RWNX_INTERFACE_STATUS_UP = 1,
};

struct bl_stats {
    int cfm_balance;
    unsigned long last_rx, last_tx; /* jiffies */
};

struct bl_patternbuf {
    u32 *buf;
    u32 dma_addr;
    int bufsz;
};

struct bl_dbginfo {
    BL_Mutex_t mutex;
    struct dbg_debug_dump_tag *buf;
    u32 dma_addr;
    int bufsz;
};

struct net_device_stats {
	unsigned long	rx_packets;
	unsigned long	tx_packets;
	unsigned long	rx_bytes;
	unsigned long	tx_bytes;
	unsigned long	rx_errors;
	unsigned long	tx_errors;
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
	unsigned long	multicast;
	unsigned long	collisions;
	unsigned long	rx_length_errors;
	unsigned long	rx_over_errors;
	unsigned long	rx_crc_errors;
	unsigned long	rx_frame_errors;
	unsigned long	rx_fifo_errors;
	unsigned long	rx_missed_errors;
	unsigned long	tx_aborted_errors;
	unsigned long	tx_carrier_errors;
	unsigned long	tx_fifo_errors;
	unsigned long	tx_heartbeat_errors;
	unsigned long	tx_window_errors;
	unsigned long	rx_compressed;
	unsigned long	tx_compressed;
};

/*
 * Structure used to save information relative to the managed stations.
 */
struct bl_sta {
    struct mac_addr sta_addr;
    u8 is_used;
    u8 sta_idx;             /* Identifier of the station */
    u8 vif_idx;             /* Identifier of the VIF (fw id) the station
                               belongs to */
    u8 vlan_idx;            /* Identifier of the VLAN VIF (fw id) the station
                               belongs to (= vif_idx if no vlan in used) */
    uint8_t qos;
    int8_t rssi;
    uint8_t data_rate;
    uint32_t tsflo;
    uint32_t tsfhi;
};

/**
 * struct bl_bcn - Information of the beacon in used (AP mode)
 *
 * @head: head portion of beacon (before TIM IE)
 * @tail: tail portion of beacon (after TIM IE)
 * @ies: extra IEs (not used ?)
 * @head_len: length of head data
 * @tail_len: length of tail data
 * @ies_len: length of extra IEs data
 * @tim_len: length of TIM IE
 * @len: Total beacon len (head + tim + tail + extra)
 * @dtim: dtim period
 */
struct bl_bcn {
    u8 *head;
    u8 *tail;
    u8 *ies;
    size_t head_len;
    size_t tail_len;
    size_t ies_len;
    size_t tim_len;
    size_t len;
    u8 dtim;
};

/*
 * Structure used to save information relative to the managed interfaces.
 * This is also linked within the bl_hw vifs list.
 *
 */
struct bl_vif {
    struct list_head list;
    struct netif *dev;
    bool up;                    /* Indicate if associated netdev is up
                                   (i.e. Interface is created at fw level) */
    union
    {
        struct
        {
            struct bl_sta *ap; /* Pointer to the peer STA entry allocated for
                                    the AP */
            struct bl_sta *tdls_sta; /* Pointer to the TDLS station */
        } sta;
        struct
        {
            struct list_head sta_list; /* List of STA connected to the AP */
            u8 bcmc_index;             /* Index of the BCMC sta to use */
        } ap;
        struct
        {
            struct bl_vif *master;   /* pointer on master interface */
            struct bl_sta *sta_4a;
        } ap_vlan;
    };
};

struct bl_hw {
    int is_up;
    struct bl_cmd_mgr cmd_mgr;
    struct ipc_host_env_tag *ipc_env;           /* store the IPC environment */
    struct list_head vifs;
    struct bl_vif vif_table[NX_VIRT_DEV_MAX + NX_REMOTE_STA_MAX]; /* indexed with fw id */
    struct bl_sta sta_table[NX_REMOTE_STA_MAX + NX_VIRT_DEV_MAX];
    unsigned long drv_flags;
    struct bl_mod_params *mod_params;
    struct ieee80211_sta_ht_cap ht_cap;
    int vif_index_sta;
    int vif_index_ap;

    /*custom added id*/
    int sta_idx;
    int ap_bcmc_idx;
#ifdef CFG_BL_STATISTIC
    struct bl_stats       stats;
#endif
};

struct ethhdr {
    unsigned char   h_dest[ETH_ALEN];   /* destination eth addr */
    unsigned char   h_source[ETH_ALEN]; /* source ether addr    */
    __be16      h_proto;        /* packet type ID field */
} __attribute__((packed));

/// Definitions of the RF bands
enum
{
    /// 2.4GHz Band
    PHY_BAND_2G4,
    /// 5GHz band
    PHY_BAND_5G,
    /// Number of bands
    PHY_BAND_MAX,
};

/// Definitions of the channel bandwidths
enum
{
    /// 20MHz BW
    PHY_CHNL_BW_20,
    /// 40MHz BW
    PHY_CHNL_BW_40,
    /// 80MHz BW
    PHY_CHNL_BW_80,
    /// 160MHz BW
    PHY_CHNL_BW_160,
    /// 80+80MHz BW
    PHY_CHNL_BW_80P80,
    /// Reserved BW
    PHY_CHNL_BW_OTHER,
};

#endif
