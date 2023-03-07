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
#ifndef __WIFI_MGMR_H__
#define __WIFI_MGMR_H__
#include <stdint.h>

#include "include/wifi_mgmr_ext.h"
#include "stateMachine.h"
#include "lmac_mac.h"
#include "bl_os_private.h"

#define WIFI_MGMR_SCAN_ITEMS_MAX (50)
#define WIFI_MGMR_PROFILES_MAX (2)
#define WIFI_MGMR_MQ_MSG_COUNT (10)

#define MAC_ADDR_LIST(m) (m)[0], (m)[1], (m)[2], (m)[3], (m)[4], (m)[5]
#define WIFI_MGMR_CONNECT_PMF_CAPABLE_BIT       (1 << 0)
#define WIFI_MGMR_CONNECT_PMF_REQUIRED_BIT      (1 << 1)

#define WIFI_MGMR_STA_DISCONNECT_DELAY          (1000) //ms 

/**
 ****************************************************************************************
 *
 * @file wifi_mgmr.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

enum EVENT_TYPE {
   EVENT_TYPE_FW,
   EVENT_TYPE_APP,
   EVENT_TYPE_GLB,
};

typedef enum WIFI_MGMR_EVENT {
    /*section for wifi manager event*/
    WIFI_MGMR_EVENT_APP_IDLE = 0,
    WIFI_MGMR_EVENT_APP_CONNECT,
    WIFI_MGMR_EVENT_APP_SNIFFER,
    WIFI_MGMR_EVENT_APP_CONNECTED,
    WIFI_MGMR_EVENT_APP_IP_GOT,
    WIFI_MGMR_EVENT_APP_DISCONNECT,
    WIFI_MGMR_EVENT_APP_RECONNECT,
    WIFI_MGMR_EVENT_APP_PHY_UP,
    WIFI_MGMR_EVENT_APP_AP_START,
    WIFI_MGMR_EVENT_APP_AP_STOP,
    WIFI_MGMR_EVENT_APP_CONF_MAX_STA,
    WIFI_MGMR_EVENT_APP_RC_CONFIG,
    WIFI_MGMR_EVENT_APP_DENOISE,
    WIFI_MGMR_EVENT_APP_RELOAD_TSEN,

    /*boundary between APP and FW*/
    WIFI_MGMR_EVENT_MAXAPP_MINIFW,

    /*section for sending FW command event*/
    WIFI_MGMR_EVENT_FW_DISCONNECT,
    WIFI_MGMR_EVENT_FW_POWERSAVING,
    WIFI_MGMR_EVENT_FW_CHANNEL_SET,
    WIFI_MGMR_EVENT_FW_SCAN,
    WIFI_MGMR_EVENT_FW_IND_DISCONNECT,
    WIFI_MGMR_EVENT_FW_IND_CONNECTED,
    WIFI_MGMR_EVENT_FW_DATA_RAW_SEND,
    WIFI_MGMR_EVENT_FW_CFG_REQ,

    /*boundary between APP and Global handled EVENT*/
    WIFI_MGMR_EVENT_MAXFW_MINI_GLOBAL,

    /*section for sending global handled event*/
    WIFI_MGMR_EVENT_GLB_SCAN_IND_BEACON,
    WIFI_MGMR_EVENT_GLB_SCAN_IND_PROBE_RESP,
    WIFI_MGMR_EVENT_GLB_AP_IND_STA_NEW,
    WIFI_MGMR_EVENT_GLB_AP_IND_STA_DEL,
    WIFI_MGMR_EVENT_GLB_DISABLE_AUTORECONNECT,
    WIFI_MGMR_EVENT_GLB_ENABLE_AUTORECONNECT,
    WIFI_MGMR_EVENT_GLB_IP_UPDATE,
    WIFI_MGMR_EVENT_GLB_MGMR_WAKEUP,

} WIFI_MGMR_EVENT_T;

typedef enum WIFI_MGMR_CONNECTION_STATUS {
    WIFI_MGMR_CONNECTION_STATUS_IDLE,
    WIFI_MGMR_CONNECTION_STATUS_CONNECTING,
    WIFI_MGMR_CONNECTION_STATUS_CONNECTED_IP_YES,
    WIFI_MGMR_CONNECTION_STATUS_CONNECTED_IP_NO,
    WIFI_MGMR_CONNECTION_STATUS_DISCONNECTED,
} WIFI_MGMR_CONNECTION_STATUS_T;

#pragma pack(push, 1)
typedef struct wifi_mgmr_msg {
    WIFI_MGMR_EVENT_T ev;
    void *data1; // ONLY for stack
    void *data2; // ONLY for stack
    void *data;  // ONLY for heap
} wifi_mgmr_msg_t;

#define MAX_LENGTH_LIMIT        (32)
typedef struct wifi_mgmr_cfg_element_msg {
    uint32_t ops;
    uint32_t task;
    uint32_t element;
    uint32_t type;
    uint32_t length;
    uint32_t buf[];
} wifi_mgmr_cfg_element_msg_t;

typedef struct wifi_mgmr_profile_msg {
    char ssid[32];
    char ssid_tail[1];
    uint32_t ssid_len;
    char passphr[64];
    char passphr_tail[1];
    char psk[64];
    char psk_tail[1];
    uint32_t passphr_len;
    uint32_t psk_len;

    uint8_t bssid[6];
    uint8_t band;
    uint16_t freq;
    int ap_info_ttl;

    uint8_t dhcp_use;
    uint32_t flags;
} wifi_mgmr_profile_msg_t;

typedef struct wifi_mgmr_ipgot_msg {
    uint32_t ip;
    uint32_t mask;
    uint32_t gw;
    uint32_t dns1;
    uint32_t dns2;
} wifi_mgmr_ipgot_msg_t;

typedef struct wifi_mgmr_ap_msg {
    int32_t channel;
    char ssid[32];
    char ssid_tail[1];
    uint8_t hidden_ssid;
    uint32_t ssid_len;
    char psk[64];
    char psk_tail[1];
    uint8_t use_dhcp_server;
    uint32_t psk_len;
    int8_t max_sta_supported;
} wifi_mgmr_ap_msg_t;

#pragma pack(pop)

typedef struct wifi_mgmr_profile {
    uint16_t ssid_len;
    uint16_t passphr_len;
    uint16_t psk_len;
    char ssid[33];
    //uint8_t no_autoconnect;
    char passphr[65];
    char psk[65];

    uint8_t bssid[6];
    uint8_t band;
    uint16_t freq;
    int ap_info_ttl;

    uint8_t dhcp_use;
    uint32_t flags;

    /*reserved field for wifi manager*/
    uint8_t priority;
    uint8_t isActive;
    uint8_t isUsed;
} wifi_mgmr_profile_t;


typedef struct
{
    uint8_t wep40 : 1;
    uint8_t wep104 : 1;
    uint8_t tkip : 1;
    uint8_t ccmp : 1;
    uint8_t rsvd : 4;
} wifi_mgmr_cipher_t;

typedef struct wifi_mgmr_scan_item {
    uint32_t mode;
    uint32_t timestamp_lastseen;
    uint16_t ssid_len;
    uint8_t channel;
    int8_t rssi;
    char ssid[32];
    char ssid_tail[1];//always put ssid_tail after ssid
    uint8_t bssid[6];
    int8_t ppm_abs;
    int8_t ppm_rel;
    uint8_t auth;
    uint8_t cipher;
    uint8_t is_used;
    uint8_t wps;
    uint8_t group_cipher;
} wifi_mgmr_scan_item_t;

struct wlan_netif {
    int mode;//0: sta; 1: ap
    uint8_t vif_index;
    uint8_t mac[6];
    uint8_t dhcp_started;
    struct {
        uint32_t ip;
        uint32_t mask;
        uint32_t gw;
        uint32_t dns1;
        uint32_t dns2;
    } ipv4;
    struct netif netif;
    union {
        struct {
            int8_t rssi;
        } sta;
    };
};

#define MAX_FIXED_CHANNELS_LIMIT (14)
typedef struct wifi_mgmr_scan_params {
    uint8_t bssid[6];
    struct mac_ssid ssid;
    uint8_t scan_mode;
    uint32_t duration_scan;  
    uint16_t channel_num;
    uint16_t channels[];
} wifi_mgmr_scan_params_t;

typedef struct wifi_mgmr_connect_ind_stat_info {
    uint16_t status_code;
    uint16_t reason_code;
    uint16_t chan_freq;
    /*mgmr recv ind event from fw when connect or disconnect  */
#define WIFI_MGMR_CONNECT_IND_STAT_INFO_TYPE_IND_CONNECTION (1)
#define WIFI_MGMR_CONNECT_IND_STAT_INFO_TYPE_IND_DISCONNECTION (2)
    char ssid[32];
    char passphr[65];
    uint8_t bssid[6];
    uint8_t type_ind;
    uint8_t chan_band;
    BL_Mutex_t diagnose_lock;
    BL_Mutex_t diagnose_get_lock;
    /// Pointer to the structure used for the diagnose module
    struct sm_tlv_list connect_diagnose;
} wifi_mgmr_connect_ind_stat_info_t;

typedef struct wifi_mgmr_sta_basic_info {
    uint8_t  sta_idx;
    uint8_t  is_used;;
    uint8_t  sta_mac[6];
    uint32_t tsfhi;
    uint32_t tsflo;
    int      rssi;
    uint8_t  data_rate;
} wifi_mgmr_sta_basic_info_t;

typedef struct wifi_mgmr {
    /*filed for PHY*/
    int channel;
    int inf_ap_enabled;

    struct wlan_netif wlan_sta;
    struct wlan_netif wlan_ap;
    WIFI_MGMR_CONNECTION_STATUS_T status;
    /*profiles*/
    wifi_mgmr_profile_t profiles[WIFI_MGMR_PROFILES_MAX];
    int profile_active_index;

    BL_Mutex_t scan_items_lock;
    wifi_mgmr_scan_item_t scan_items[WIFI_MGMR_SCAN_ITEMS_MAX];
    BL_MessageQueue_t mq;
    // uint8_t mq_pool[sizeof(wifi_mgmr_msg_t)*WIFI_MGMR_MQ_MSG_COUNT];
    struct stateMachine m;
    BL_Timer_t timer;
    wifi_mgmr_connect_ind_stat_info_t wifi_mgmr_stat_info;
    uint8_t ready;//TODO mgmr init process
    char country_code[3];
    uint8_t disable_autoreconnect;
    uint16_t ap_bcn_int;
    int channel_nums;

    int ap_info_ttl_curr;

    /*pending task*/
    union {
        uint32_t val;
        struct {
#define WIFI_MGMR_PENDING_TASK_SCAN_BIT      (1 << 0)
#define WIFI_MGMR_PENDING_TASK_IP_UPDATE_BIT (1 << 1)
#define WIFI_MGMR_PENDING_TASK_IP_GOT_BIT    (1 << 2)
#define WIFI_MGMR_PENDING_TASK_CONNECT_BIT   (1 << 3)
            unsigned int scan       :   1;
            unsigned int ip_update  :   1;
            unsigned int ip_got     :   1;
            unsigned int connect    :   1;
        } bits;
    } pending_task;
    /*Feature Bits*/
    uint32_t features;
#define WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID    (1 << 0)

    /*Manager config*/
    int scan_item_timeout;
#define WIFI_MGMR_CONFIG_SCAN_ITEM_TIMEOUT      (15000)

#define MAX_HOSTNAME_LEN_CHECK 32
    char hostname[MAX_HOSTNAME_LEN_CHECK];
    void *dns_server;
#ifdef DEBUG_CONNECT_ABORT
    unsigned long connect_time;
#endif
} wifi_mgmr_t;


/// Constant value corresponding to the Broadcast MAC address
extern const struct mac_addr mac_addr_bcst;
int wifi_mgmr_pending_task_set(uint32_t bits);
int wifi_mgmr_event_notify(wifi_mgmr_msg_t *msg, int use_block);
int wifi_mgmr_detailed_state_get_internal(int *state, int *state_d);
int wifi_mgmr_state_get_internal(int *state);
int wifi_mgmr_status_code_clean_internal(void);
int wifi_mgmr_status_code_get_internal(int *s_code);
int wifi_mgmr_set_country_code_internal(char *country_code);
int wifi_mgmr_ap_sta_cnt_get_internal(uint8_t *sta_cnt);
int wifi_mgmr_ap_sta_info_get_internal(wifi_mgmr_sta_basic_info_t *sta_info_internal, uint8_t idx);
int wifi_mgmr_ap_sta_delete_internal(uint8_t sta_idx);
int wifi_mgmr_scan_complete_notify();
extern wifi_mgmr_t wifiMgmr;
char *wifi_mgmr_mode_to_str(uint32_t mode);
char *wifi_mgmr_auth_to_str(uint8_t auth);
char *wifi_mgmr_cipher_to_str(uint8_t cipher);
int wifi_mgmr_api_fw_tsen_reload(void);
int wifi_mgmr_scan_beacon_save( wifi_mgmr_scan_item_t *scan );

static inline int wifi_mgmr_scan_item_is_timeout(wifi_mgmr_t *mgmr, wifi_mgmr_scan_item_t *item)
{
    return ((unsigned int)bl_os_get_time_ms() - (unsigned int)item->timestamp_lastseen) >= mgmr->scan_item_timeout ? 1 : 0;
}
#endif
