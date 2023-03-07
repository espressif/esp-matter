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

#include <bl_supplicant/bl_wifi_driver.h>
#include "utils/includes.h"
#include "utils/common.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/wpa_i.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "rsn_supp/wpa_ie.h"
#include "ap/wpa_auth.h"
#include "ap/wpa_auth_i.h"
#include "ap/ap_config.h"
#include "ap/hostapd.h"
#include "bl_wpas_glue.h"
#include "bl_hostap.h"

#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/aes_wrap.h"

#include "bl_wpa3_i.h"

void wpa_install_key(uint8_t vif_idx, uint8_t sta_idx, enum wpa_alg alg, int key_idx, int set_tx,
                     u8 *seq, size_t seq_len, u8 *key, size_t key_len, bool pairwise)
{
    bl_wifi_set_sta_key_internal(vif_idx, sta_idx, alg, key_idx, set_tx, seq, seq_len, key, key_len, pairwise);
}

int wpa_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx,
                u8 *key, size_t key_len, int key_entry_valid)
{
    // this function is not used
    return 0;
}

/**
 * eapol_sm_notify_eap_success - Notification of external EAP success trigger
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @success: %TRUE = set success, %FALSE = clear success
 *
 * Notify the EAPOL state machine that external event has forced EAP state to
 * success (success = %TRUE). This can be cleared by setting success = %FALSE.
 *
 * This function is called to update EAP state when WPA-PSK key handshake has
 * been completed successfully since WPA-PSK does not use EAP state machine.
 */

/* fix buf for tx for now */
#define WPA_TX_MSG_BUFF_MAXLEN 200

void wpa_sendto_wrapper(bool is_sta, void *buffer, u16 len, struct bl_custom_tx_cfm *tx_cfm)
{
    struct pbuf *p;

    p = pbuf_alloc(PBUF_RAW_TX, len, PBUF_RAM);
    if (!p) {
        return;
    }

    if (ERR_OK != pbuf_take(p, buffer, len)) {
        pbuf_free(p);
        return;
    }
    bl_wifi_eth_tx(p, is_sta, tx_cfm);
    pbuf_free(p);
}

void wpa_deauthenticate(uint8_t sta_idx, u8 reason_code)
{
    wpa_clear_4way_handshake_timer();
    bl_wifi_auth_done_internal(sta_idx, reason_code);
}

#if 0
void wpa_set_profile(u32 wpa_proto, u8 auth_mode)
{
    struct wpa_sm *sm = &gWpaSm;

    sm->proto = wpa_proto;
    if (auth_mode == WPA2_AUTH_ENT) {
        sm->key_mgmt = WPA_KEY_MGMT_IEEE8021X; /* for wpa2 enterprise */
    } else if (auth_mode == WPA2_AUTH_ENT_SHA256) {
        sm->key_mgmt = WPA_KEY_MGMT_IEEE8021X_SHA256; /* for wpa2 enterprise sha256 */
    } else if (auth_mode == WPA2_AUTH_PSK_SHA256) {
        sm->key_mgmt = WPA_KEY_MGMT_PSK_SHA256;
    } else if (auth_mode == WPA3_AUTH_PSK) {
        sm->key_mgmt = WPA_KEY_MGMT_SAE; /* for WPA3 PSK */
    } else if (auth_mode == WAPI_AUTH_PSK) {
        sm->key_mgmt = WPA_KEY_MGMT_WAPI_PSK; /* for WAPI PSK */
    } else {
        sm->key_mgmt = WPA_KEY_MGMT_PSK;  /* fixed to PSK for now */
    }
}
#endif

void  wpa_config_profile(wifi_connect_parm_t *parm)
{
    u16 key_mgmt = parm->key_mgmt;
    if (parm->proto == SEC_PROTO_WPA) {
        wpa_set_profile(WPA_PROTO_WPA, key_mgmt);
    } else if (parm->proto == SEC_PROTO_WPA2 || parm->proto == SEC_PROTO_WPA3) {
        wpa_set_profile(WPA_PROTO_RSN, key_mgmt);
    } else if (parm->proto == SEC_PROTO_WAPI) {
        wpa_set_profile(WPA_PROTO_WAPI, key_mgmt);
    } else {
        WPA_ASSERT(0);
    }
}

int wpa_config_bss(wifi_connect_parm_t *parm)
{
    int ret = 0;

    ret = wpa_set_bss(parm->vif_idx, parm->sta_idx, (char *)parm->mac, (char *)parm->bssid, parm->pairwise_cipher, parm->group_cipher, parm->pmf_required, parm->mgmt_group_cipher);
    return ret;
}

void wpa_config_assoc_ie(uint8_t vif_idx, u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len)
{
    bl_wifi_set_appie_internal(vif_idx, WIFI_APPIE_WPA_RSN, assoc_buf, assoc_wpa_ie_len, 1);
    /* esp_set_rm_enabled_ie(); */
}

void wpa_neg_complete(uint8_t sta_idx)
{
    wpa_clear_4way_handshake_timer();
    bl_wifi_auth_done_internal(sta_idx, 0);
}

bool wpa_attach(void)
{
    bool ret = true;
    ret = wpa_sm_init();
    return ret;
}

bool wpa_ap_rx_eapol(void *hapd_data, void *sm_data, u8 *data, size_t data_len)
{
    struct hostapd_data *hapd = (struct hostapd_data *)hapd_data;
    struct wpa_state_machine *sm = (struct wpa_state_machine *)sm_data;

    if (!hapd || !sm) {
        return false;
    }

    wpa_receive(hapd->wpa_auth, sm, data, data_len);

    return true;
}

bool wpa_deattach(void)
{
    /* esp_wifi_sta_wpa2_ent_disable(); */
    wpa_sm_deinit();
    return true;
}

#ifdef DEBUG_PRINT
#define ARR_02X_6 "%02X-%02X-%02X-%02X-%02X-%02X"
#define ARR_ASC_6(arr) arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]
static void dump_connect_parm(const wifi_connect_parm_t *parm)
{
    wpa_dbg(0, MSG_DEBUG, "---- BEGIN OF CONNECT PARM DUMP ----\r\n");
    wpa_dbg(0, MSG_DEBUG, "vif_idx %u, sta_idx %u\r\n", parm->vif_idx, parm->sta_idx);
    wpa_dbg(0, MSG_DEBUG, "mac " ARR_02X_6 "\r\n", ARR_ASC_6(parm->mac));
    wpa_dbg(0, MSG_DEBUG, "bssid " ARR_02X_6 "\r\n", ARR_ASC_6(parm->bssid));
    char buf[33] = { 0 };
    memcpy(buf, parm->ssid.ssid, parm->ssid.len);
    wpa_dbg(0, MSG_DEBUG, "ssid %s\r\n", buf);
    wpa_dbg(0, MSG_DEBUG, "proto %u\r\n", parm->proto);
    wpa_dbg(0, MSG_DEBUG, "pairwise_cipher %u\r\n", parm->pairwise_cipher);
    wpa_dbg(0, MSG_DEBUG, "group_cipher %u\r\n", parm->group_cipher);
    wpa_dbg(0, MSG_DEBUG, "passphrase %s\r\n", parm->passphrase);
    wpa_dbg(0, MSG_DEBUG, "pmf_required %d\r\n", parm->pmf_required);
    wpa_dbg(0, MSG_DEBUG, "mgmt_group_cipher %u\r\n", parm->mgmt_group_cipher);
    wpa_dbg(0, MSG_DEBUG, "---- END OF CONNECT PARM DUMP ----\r\n");
}
#endif

void wpa_sta_config(wifi_connect_parm_t *parm)
{
#ifdef DEBUG_PRINT
    dump_connect_parm(parm);
#endif
    int ret = 0;
    wpa_config_profile(parm);
    ret = wpa_config_bss(parm);
    WPA_ASSERT(ret == 0);
}

void wpa_sta_connect(wifi_connect_parm_t *parm)
{
#ifdef DEBUG_PRINT
    dump_connect_parm(parm);
#endif
    wpa_sm_set_state(WPA_AUTHENTICATING);

    wpa_set_parm_at_connect(parm);
    wpa_set_4way_handshake_timer();
}

int wpa_parse_wpa_ie_wrapper(const uint8_t *wpa_ie, size_t wpa_ie_len, wifi_wpa_ie_t *data)
{
    struct wpa_ie_data ie;
    int ret = 0;

    ret = wpa_parse_wpa_ie(wpa_ie, wpa_ie_len, &ie);
    data->proto = ie.proto;
    data->pairwise_cipher = cipher_type_map_supp_to_public(ie.pairwise_cipher);
    data->group_cipher = cipher_type_map_supp_to_public(ie.group_cipher);
    data->key_mgmt = ie.key_mgmt;
    data->capabilities = ie.capabilities;
    data->pmkid = ie.pmkid;
    data->mgmt_group_cipher = cipher_type_map_supp_to_public(ie.mgmt_group_cipher);

    return ret;
}

static void wpa_sta_disconnected_cb(uint8_t reason_code)
{
    switch (reason_code) {
#if 0
        case WIFI_REASON_UNSPECIFIED:
        case WIFI_REASON_AUTH_EXPIRE:
        case WIFI_REASON_NOT_AUTHED:
        case WIFI_REASON_NOT_ASSOCED:
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_INVALID_PMKID:
        case WIFI_REASON_AUTH_FAIL:
        case WIFI_REASON_ASSOC_FAIL:
        case WIFI_REASON_CONNECTION_FAIL:
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            bl_wpa3_free_sae_data();
            wpa_sta_clear_curr_pmksa();
            break;
#endif
        default:
            wpa_sm_set_state(WPA_DISCONNECTED);
            wpa_clear_4way_handshake_timer();
            bl_wpa3_free_sae_data();
#ifdef CONFIG_PMKSA_CACHE
            wpa_sta_clear_curr_pmksa();
#endif
            break;
    }
}

static const struct wpa_funcs wpa_cb = {
    .wpa_sta_init              = wpa_attach,
    .wpa_sta_deinit            = wpa_deattach,
    .wpa_sta_rx_eapol          = wpa_sm_rx_eapol,
    .wpa_sta_config            = wpa_sta_config,
    .wpa_sta_connect           = wpa_sta_connect,
    .wpa_sta_disconnected_cb   = wpa_sta_disconnected_cb,

    .wpa_ap_join             = wpa_ap_join,
    .wpa_ap_sta_associated   = wpa_ap_sta_associated,
    .wpa_ap_remove           = wpa_ap_remove,
    .wpa_ap_rx_eapol         = wpa_ap_rx_eapol,
    .wpa_ap_init             = hostap_init,
    .wpa_ap_deinit           = hostap_deinit,

    .wpa_parse_wpa_ie        = wpa_parse_wpa_ie_wrapper,
    .wpa_reg_diag_tlv_cb     = wpa_reg_diag_tlv_cb,
#ifdef CONFIG_MIC_FAILURE_COUNTERMEASURE
    .wpa_michael_mic_failure = wpa_michael_mic_failure,
#endif

#ifdef CONFIG_WPA3_SAE
    .wpa3_build_sae_msg = wpa3_build_sae_msg,
    .wpa3_parse_sae_msg = wpa3_parse_sae_msg,
    .wpa3_clear_sae     = bl_wpa3_free_sae_data,
#else
    .wpa3_build_sae_msg = NULL,
    .wpa3_parse_sae_msg = NULL,
#endif
};

int bl_supplicant_init(void)
{
    int ret = 0;

    bl_wifi_register_wpa_cb_internal(&wpa_cb);

#if CONFIG_WPA_WAPI_PSK
    ret =  esp_wifi_internal_wapi_init();
#endif

    return ret;
}

int bl_supplicant_deinit(void)
{
    return bl_wifi_unregister_wpa_cb_internal();
}
