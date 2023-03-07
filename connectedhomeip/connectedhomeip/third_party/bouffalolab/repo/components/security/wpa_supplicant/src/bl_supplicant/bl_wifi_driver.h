// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef BL_WIFI_DRIVER_H
#define BL_WIFI_DRIVER_H

#include <lwip/pbuf.h>
#include <supplicant_api.h>

#if CONFIG_NEWLIB_NANO_FORMAT
#define TASK_STACK_SIZE_ADD 0
#else
#define TASK_STACK_SIZE_ADD 512
#endif

#define WPA2_TASK_STACK_SIZE  (6144 + TASK_STACK_SIZE_ADD)
#define WPS_TASK_STACK_SIZE  (12288 + TASK_STACK_SIZE_ADD)

typedef int bl_err_t;

/* Definitions for error constants. */
#define BL_OK          0       /*!< bl_err_t value indicating success (no error) */
#define BL_FAIL        -1      /*!< Generic bl_err_t code indicating failure */

/* wifi_appie_t is in rom code and can't be changed anymore, use wifi_appie_ram_t for new app IEs */
typedef enum {
    WIFI_APPIE_RM_ENABLED_CAPS = WIFI_APPIE_MAX,
    WIFI_APPIE_RAM_MAX,
} wifi_appie_ram_t;

typedef enum {
    WPA2_ENT_EAP_STATE_NOT_START,
    WPA2_ENT_EAP_STATE_IN_PROGRESS,
    WPA2_ENT_EAP_STATE_SUCCESS,
    WPA2_ENT_EAP_STATE_FAIL,
} wpa2_ent_eap_state_t;

struct wifi_appie {
    uint16_t   ie_len;
    uint8_t    ie_data[];
};

#if 0
typedef struct {
    int proto;
    int pairwise_cipher;
    int group_cipher;
    int key_mgmt;
    int capabilities;
    size_t num_pmkid;
    const u8 *pmkid;
    int mgmt_group_cipher;
} wifi_wpa_ie_t;

struct wpa_funcs {
    bool (*wpa_sta_init)(void);
    bool (*wpa_sta_deinit)(void);
    void (*wpa_sta_connect)(uint8_t *bssid);
    void (*wpa_sta_disconnected_cb)(uint8_t reason_code);
    int (*wpa_sta_rx_eapol)(u8 *src_addr, u8 *buf, u32 len);
    bool (*wpa_sta_in_4way_handshake)(void);
    void *(*wpa_ap_init)(void);
    bool (*wpa_ap_deinit)(void *data);
    bool (*wpa_ap_join)(void **sm, u8 *bssid, u8 *wpa_ie, u8 wpa_ie_len);
    bool (*wpa_ap_remove)(void *sm);
    uint8_t *(*wpa_ap_get_wpa_ie)(uint8_t *len);
    bool (*wpa_ap_rx_eapol)(void *hapd_data, void *sm, u8 *data, size_t data_len);
    void (*wpa_ap_get_peer_spp_msg)(void *sm, bool *spp_cap, bool *spp_req);
    char *(*wpa_config_parse_string)(const char *value, size_t *len);
    int (*wpa_parse_wpa_ie)(const u8 *wpa_ie, size_t wpa_ie_len, wifi_wpa_ie_t *data);
    int (*wpa_config_bss)(u8 *bssid);
    int (*wpa_michael_mic_failure)(u16 is_unicast);
    uint8_t *(*wpa3_build_sae_msg)(uint8_t *bssid, uint32_t type, size_t *len);
    int (*wpa3_parse_sae_msg)(uint8_t *buf, size_t len, uint32_t type, uint16_t status);
    int (*wpa_sta_rx_mgmt)(u8 type, u8 *frame, size_t len, u8 *sender, u32 rssi, u8 channel, u64 current_tsf);
};

struct wpa2_funcs {
    int  (*wpa2_sm_rx_eapol)(u8 *src_addr, u8 *buf, u32 len, u8 *bssid);
    int  (*wpa2_start)(void);
    u8   (*wpa2_get_state)(void);
    int  (*wpa2_init)(void);
    void (*wpa2_deinit)(void);
};
#endif

typedef int (*wifi_wpa2_fn_t)(void *);
typedef struct {
    wifi_wpa2_fn_t   fn;
    void    *param;
} wifi_wpa2_param_t;

#define IS_WPS_REGISTRAR(type) (((type)>WPS_TYPE_MAX)?(((type)<WPS_TYPE_MAX)?true:false):false)
#define IS_WPS_ENROLLEE(type)  (((type)>WPS_TYPE_DISABLE)?(((type)<WPS_TYPE_MAX)?true:false):false)

#define WIFI_TXCB_EAPOL_ID  3
typedef void(*wifi_tx_cb_t)(void *);
typedef int (*wifi_ipc_fn_t)(void *);
typedef struct {
    wifi_ipc_fn_t fn;
    void *arg;
    uint32_t arg_size;
} wifi_ipc_config_t;

#define WPA_IGTK_LEN 16
typedef struct {
    uint8_t keyid[2];
    uint8_t pn[6];
    uint8_t igtk[WPA_IGTK_LEN];
} wifi_wpa_igtk_t;


typedef void (*bl_custom_tx_callback_t)(void *cb_arg, bool tx_ok);

struct bl_custom_tx_cfm {
    bl_custom_tx_callback_t cb;
    void *cb_arg;
};

err_t bl_wifi_eth_tx(struct pbuf *p, bool is_sta, struct bl_custom_tx_cfm *custom_cfm);
#endif /* BL_WIFI_DRIVER_H */
