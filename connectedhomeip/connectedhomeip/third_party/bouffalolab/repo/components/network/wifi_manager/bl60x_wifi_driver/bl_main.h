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
#ifndef __RWNX_MAIN_H__
#define __RWNX_MAIN_H__
#include <stdint.h>
#include <lwip/netif.h>
#include "lmac_mac.h"
#include <wifi_mgmr_ext.h>

struct wifi_apm_sta_info
{
    uint8_t  sta_idx;
    uint8_t  is_used;
    uint8_t  sta_mac[6];
    uint32_t tsfhi;
    uint32_t tsflo;
    int      rssi;
    uint8_t  data_rate;
};

int bl_main_powersaving(int mode);
int bl_main_denoise(int mode);
int bl_main_disconnect(void);
int bl_main_phy_up(void);
int bl_main_channel_set(int channel);
int bl_main_monitor_channel_set(int channel, int use_40MHZ);
int bl_main_rate_config(uint8_t sta_idx, uint16_t fixed_rate_cfg);
int bl_main_if_remove(uint8_t vif_index);
int bl_main_if_add(int is_sta, struct netif *netif, uint8_t *vif_index);
int bl_main_monitor(void);
int bl_main_monitor_disable(void);
int bl_main_connect(const uint8_t* ssid, int ssid_len, const uint8_t *psk, int psk_len, const uint8_t *pmk, int pmk_len, const uint8_t *mac, const uint8_t band, const uint16_t freq, const uint32_t flags);
int bl_main_connect_abort(uint8_t *status);
int bl_main_apm_start(char *ssid, char *password, int channel, uint8_t vif_index, uint8_t hidden_ssid, uint16_t bcn_int);
int bl_main_apm_stop(uint8_t vif_index);
int bl_main_apm_sta_cnt_get(uint8_t *sta_cnt);
int bl_main_apm_sta_info_get(struct wifi_apm_sta_info *apm_sta_info, uint8_t idx);
int bl_main_apm_sta_delete(uint8_t sta_idx);
int bl_main_apm_remove_all_sta();
int bl_main_conf_max_sta(uint8_t max_sta_supported);
int bl_main_cfg_task_req(uint32_t ops, uint32_t task, uint32_t element, uint32_t type, void *arg1, void *arg2);
int bl_main_scan(struct netif *netif, uint16_t *fixed_channels, uint16_t channel_num, struct mac_addr *bssid, struct mac_ssid *ssid, uint8_t scan_mode, uint32_t duration_scan);
int bl_main_raw_send(uint8_t *pkt , int len);
int bl_main_set_country_code(char *country_code);
int bl_main_get_channel_nums();
int bl_main_beacon_interval_set(uint16_t beacon_int);

struct wifi_event_sm_connect_ind
{
    uint16_t status_code;
    uint16_t reason_code;
    /// BSSID
    uint8_t bssid[6];
    /// Index of the VIF for which the association process is complete
    uint8_t vif_idx;
    /// Index of the STA entry allocated for the AP
    uint8_t ap_idx;
    /// Index of the LMAC channel context the connection is attached to
    uint8_t ch_idx;
    /// Flag indicating if the AP is supporting QoS
    int qos;
    uint16_t aid;
    uint8_t band;
    uint16_t center_freq;
    uint8_t width;
    uint32_t center_freq1;
    uint32_t center_freq2;
    /// Pointer to the structure used for the diagnose module
    struct sm_tlv_list connect_diagnose;
};

struct wifi_event_sm_disconnect_ind
{
    /// Status code of the disconnection procedure
    uint16_t status_code;
    /// Reason of the disconnection.
    uint16_t reason_code;
    /// Index of the VIF.
    uint8_t vif_idx;
    /// FT over DS is ongoing
    int ft_over_ds;
    /// Pointer to the structure used for the diagnose module
    struct sm_tlv_list connect_diagnose;
};

typedef struct
{
    uint8_t  noRsn      : 1;
    uint8_t  wepStatic  : 1;
    uint8_t  wepDynamic : 1;
    uint8_t  wpa        : 1;
    uint8_t  wpaNone    : 1;
    uint8_t  wpa2       : 1;
    uint8_t  cckm       : 1;
    uint8_t  wapi       : 1;
    uint8_t  rsvd       : 8;
} wifi_secmode_t;

typedef struct
{
    uint8_t   wep40      : 1;
    uint8_t   wep104     : 1;
    uint8_t   tkip       : 1;
    uint8_t   ccmp       : 1;
    uint8_t   rsvd       : 4;
} wifi_cipher_t;

#define WIFI_EVENT_BEACON_IND_AUTH_OPEN            0
#define WIFI_EVENT_BEACON_IND_AUTH_WEP             1
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK         2
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK        3
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK    4
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT         5
#define WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE        6
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE 7
#define WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN      0xff

#define WIFI_EVENT_BEACON_IND_CIPHER_NONE           0
#define WIFI_EVENT_BEACON_IND_CIPHER_WEP            1
#define WIFI_EVENT_BEACON_IND_CIPHER_AES            2
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP           3
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES       4

struct wifi_event_beacon_ind
{
    int mode;
    uint8_t bssid[6];
    uint8_t ssid[33];
    int8_t rssi;
    int8_t ppm_abs;
    int8_t ppm_rel;
    uint8_t channel;
    uint8_t auth;//0: open; 1:wep; 2:WPA/WPA2 - PSK; 3: WPA/WPA2 - Enterprise; 0xFF: unknown
    uint8_t cipher;
    wifi_cipher_t wpa_mcstCipher;
    wifi_cipher_t wpa_ucstCipher;
    wifi_cipher_t rsn_mcstCipher;
    wifi_cipher_t rsn_ucstCipher;
    wifi_secmode_t sec_mode;
    int ssid_len;
    uint8_t wps;
    uint8_t group_cipher;
};

#pragma  pack(push,1)
struct wifi_event_data_ind_channel_switch
{
    int channel;
};

struct wifi_event_data_ind_scan_done
{
    uint32_t nothing;
};

struct wifi_event
{
#define WIFI_EVENT_ID_IND_CHANNEL_SWITCH        0
#define WIFI_EVENT_ID_IND_SCAN_DONE             1
#define WIFI_EVENT_ID_IND_SCAN_DONE_ONJOIN      2
    uint32_t id;
    uint8_t data[0];
};
#pragma  pack(pop)

//TODO: add general EVENT Framework
typedef void (*wifi_event_sm_connect_ind_cb_t)(void *env, struct wifi_event_sm_connect_ind *ind);
typedef void (*wifi_event_sm_disconnect_ind_cb_t)(void *env, struct wifi_event_sm_disconnect_ind *ind);
typedef void (*wifi_event_beacon_ind_cb_t)(void *env, struct wifi_event_beacon_ind *ind);
typedef void (*wifi_event_probe_resp_ind_cb_t)(void *env, long long timestamp);
typedef void (*wifi_event_pkt_cb_t)(void *env, uint8_t *ieee80211_pkt, int len, bl_rx_info_t *info);
typedef void (*wifi_event_pkt_cb_adv_t)(void *env, void *pkt_wrap, bl_rx_info_t *info);
typedef void (*wifi_event_rssi_cb_t)(void *env, int8_t rssi);
typedef void (*wifi_event_cb_t)(void *env, struct wifi_event *event);
int bl_rx_sm_connect_ind_cb_register(void *env, wifi_event_sm_connect_ind_cb_t cb);
int bl_rx_sm_connect_ind_cb_unregister(void *env, wifi_event_sm_connect_ind_cb_t cb);
int bl_rx_sm_disconnect_ind_cb_register(void *env, wifi_event_sm_disconnect_ind_cb_t cb);
int bl_rx_sm_disconnect_ind_cb_unregister(void *env, wifi_event_sm_disconnect_ind_cb_t cb);
int bl_rx_beacon_ind_cb_register(void *env, wifi_event_beacon_ind_cb_t cb);
int bl_rx_probe_resp_ind_cb_register(void *env, wifi_event_probe_resp_ind_cb_t cb);
int bl_rx_beacon_ind_cb_unregister(void *env, wifi_event_beacon_ind_cb_t cb);
int bl_rx_pkt_cb_register(void *env, wifi_event_pkt_cb_t cb);
int bl_rx_pkt_cb_unregister(void *env);
int bl_rx_pkt_adv_cb_register(void *env, wifi_event_pkt_cb_adv_t cb);
int bl_rx_pkt_adv_cb_unregister(void *env);
int bl_rx_rssi_cb_register(void *env, wifi_event_rssi_cb_t cb);
int bl_rx_rssi_cb_unregister(void *env, wifi_event_rssi_cb_t cb);
int bl_rx_event_register(void *env, wifi_event_cb_t cb);
int bl_rx_event_unregister(void *env);
int bl_main_tx_still_free(void);
#endif
