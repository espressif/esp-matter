
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

#include <stdio.h>
#include <string.h>

#include <lwip/inet.h>
#include <lwip/netifapi.h>
#include <aos/yloop.h>
#include "bl_main.h"
#include "bl_defs.h"
#include "bl_cmds.h"
#include "bl_rx.h"
#include "bl_utils.h"
#include "ieee80211.h"
#include <bl60x_fw_api.h>
#include <supplicant_api.h>
#include <bl_wpa.h>

#ifdef BL602_MATTER_SUPPORT
#include <lwip/dhcp6.h>
#endif

#include <bl_os_private.h>
#define USER_UNUSED(a) ((void)(a))

static wifi_event_sm_connect_ind_cb_t cb_sm_connect_ind;
static void* cb_sm_connect_ind_env;
static wifi_event_sm_disconnect_ind_cb_t cb_sm_disconnect_ind;
static void* cb_sm_disconnect_ind_env;
static wifi_event_beacon_ind_cb_t cb_beacon_ind;
static void* cb_beacon_ind_env;
static wifi_event_probe_resp_ind_cb_t cb_probe_resp_ind;
static void* cb_probe_resp_ind_env;
static wifi_event_pkt_cb_t cb_pkt;
static wifi_event_pkt_cb_adv_t cb_pkt_adv;
static void* cb_pkt_env;
static wifi_event_rssi_cb_t cb_rssi;
static void* cb_rssi_env;
static wifi_event_cb_t cb_event;
static void* cb_event_env;
extern struct bl_hw wifi_hw;

static const struct reason_code sm_reason_list[] = {
    {WLAN_FW_SUCCESSFUL, "sm connect ind ok"},
    {WLAN_FW_TX_AUTH_FRAME_ALLOCATE_FAIILURE, "tx auth frame alloc failure"},
    {WLAN_FW_AUTHENTICATION_FAIILURE, "Authentication failure"},
    {WLAN_FW_AUTH_ALGO_FAIILURE, "Auth response but auth algo failure"},
    {WLAN_FW_TX_ASSOC_FRAME_ALLOCATE_FAIILURE, "tx assoc frame alloc failure"},
    {WLAN_FW_ASSOCIATE_FAIILURE, "Association failure"},
    {WLAN_FW_DEAUTH_BY_AP_WHEN_NOT_CONNECTION, "deauth by AP when connecting"},
    {WLAN_FW_DEAUTH_BY_AP_WHEN_CONNECTION, "deauth by AP when connected"},
    {WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE, "Passwd error, 4-way handshake timeout"},
    {WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_TRANSMIT_FAILURE, "Passwd error, tx deauth frame transmit failure"},
    {WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_ALLOCATE_FAIILURE, "Passwd error, tx deauth frame allocate failure"},
    {WLAN_FW_AUTH_OR_ASSOC_RESPONSE_TIMEOUT_FAILURE, "auth or associate frame response timeout failure"},
    {WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL, "SSID error, scan no bssid and channel"},
    {WLAN_FW_CREATE_CHANNEL_CTX_FAILURE_WHEN_JOIN_NETWORK, "create channel context failure when join network"},
    {WLAN_FW_JOIN_NETWORK_FAILURE, "join network failure"},
    {WLAN_FW_ADD_STA_FAILURE, "add sta failure"},
    {WLAN_FW_BEACON_LOSS, "ap beacon loss"},
    {WLAN_FW_JOIN_NETWORK_SECURITY_NOMATCH, "network security no match"},
    {WLAN_FW_JOIN_NETWORK_WEPLEN_ERROR, "wep network psk len error"},
    {WLAN_FW_DISCONNECT_BY_USER_WITH_DEAUTH, "user disconnect and send deauth"},
    {WLAN_FW_DISCONNECT_BY_USER_NO_DEAUTH, "user disconnect but no send deauth"},
    {WLAN_FW_DISCONNECT_BY_FW_PS_TX_NULLFRAME_FAILURE, "fw disconnect(tx nullframe failures)"},
    {WLAN_FW_TRAFFIC_LOSS, "fw disconnect(traffic loss)"},
    {WLAN_FW_CONNECT_ABORT_BY_USER_WITH_DEAUTH, "user connect abort and send deauth"},
    {WLAN_FW_CONNECT_ABORT_BY_USER_NO_DEAUTH, "user connect abort without sending deauth"},
    {WLAN_FW_CONNECT_ABORT_WHEN_JOINING_NETWORK, "user connect abort when joining network"}, 
    {WLAN_FW_CONNECT_ABORT_WHEN_SCANNING, "user connect abort when scanning"},
};

static const struct reason_code apm_reason_list[] = {
    {WLAN_FW_APM_SUCCESSFUL, "apm connect ind ok"},
    {WLAN_FW_APM_DELETESTA_BY_USER, "User delete STA"},
    {WLAN_FW_APM_DEATUH_BY_STA, "STA send deauth to AP"},
    {WLAN_FW_APM_DISASSOCIATE_BY_STA, "STA send disassociate to AP"},
    {WLAN_FW_APM_DELETECONNECTION_TIMEOUT, "timeout and delete connection"},
    {WLAN_FW_APM_DELETESTA_FOR_NEW_CONNECTION, "Delete STA for new connection"},
};
int bl_rx_sm_connect_ind_cb_register(void *env, wifi_event_sm_connect_ind_cb_t cb)
{
    cb_sm_connect_ind = cb;
    cb_sm_connect_ind_env = env;

    return 0;
}

//FIXME race condition protect?
int bl_rx_sm_connect_ind_cb_unregister(void *env, wifi_event_sm_connect_ind_cb_t cb)
{
    cb_sm_connect_ind = NULL;
    cb_sm_connect_ind_env = NULL;

    return 0;
}

int bl_rx_sm_disconnect_ind_cb_register(void *env, wifi_event_sm_disconnect_ind_cb_t cb)
{
    cb_sm_disconnect_ind = cb;
    cb_sm_disconnect_ind_env = env;

    return 0;
}

//FIXME race condition protect?
int bl_rx_sm_disconnect_ind_cb_unregister(void *env, wifi_event_sm_disconnect_ind_cb_t cb)
{
    cb_sm_disconnect_ind = NULL;
    cb_sm_disconnect_ind_env = NULL;

    return 0;
}

int bl_rx_beacon_ind_cb_register(void *env, wifi_event_beacon_ind_cb_t cb)
{
    cb_beacon_ind = cb;
    cb_beacon_ind_env = env;

    return 0;
}

//FIXME race condition protect?
int bl_rx_beacon_ind_cb_unregister(void *env, wifi_event_beacon_ind_cb_t cb)
{
    cb_beacon_ind = NULL;
    cb_beacon_ind_env = NULL;

    return 0;
}

int bl_rx_probe_resp_ind_cb_register(void *env, wifi_event_probe_resp_ind_cb_t cb)
{
    cb_probe_resp_ind = cb;
    cb_probe_resp_ind_env = env;

    return 0;
}

//FIXME race condition protect?
int bl_rx_probe_resp_ind_cb_unregister(void *env, wifi_event_probe_resp_ind_cb_t cb)
{
    cb_probe_resp_ind = NULL;
    cb_probe_resp_ind_env = NULL;

    return 0;
}


int bl_rx_pkt_cb_register(void *env, wifi_event_pkt_cb_t cb)
{
    cb_pkt = cb;
    cb_pkt_env = env;

    return 0;
}

//FIXME race condition protect?
int bl_rx_pkt_cb_unregister(void *env)
{
    cb_pkt = NULL;
    cb_pkt_env = NULL;

    return 0;
}

int bl_rx_pkt_adv_cb_register(void *env, wifi_event_pkt_cb_adv_t cb)
{
    cb_pkt_adv = cb;
    cb_pkt_env = env;

    return 0;
}

int bl_rx_pkt_adv_cb_unregister(void *env)
{
    cb_pkt_adv = NULL;
    cb_pkt_env = NULL;

    return 0;
}

//FIXME race condition protect?
int bl_rx_rssi_cb_register(void *env, wifi_event_rssi_cb_t cb)
{
    cb_rssi = cb;
    cb_rssi_env = env;

    return 0;
}

int bl_rx_rssi_cb_unregister(void *env, wifi_event_rssi_cb_t cb)
{
    cb_rssi = NULL;
    cb_rssi_env = NULL;

    return 0;
}

//FIXME race condition protect?
int bl_rx_event_register(void *env, wifi_event_cb_t cb)
{
    cb_event = cb;
    cb_event_env = env;

    return 0;
}

int bl_rx_event_unregister(void *env)
{
    cb_event = NULL;
    cb_event_env = NULL;

    return 0;
}

static void notify_event_channel_switch(int channel)
{
    uint8_t buffer[sizeof(struct wifi_event) + sizeof(struct wifi_event_data_ind_channel_switch)];
    struct wifi_event *event;
    struct wifi_event_data_ind_channel_switch *ind;

    event = (struct wifi_event*)buffer;
    ind = (struct wifi_event_data_ind_channel_switch*)event->data;
    memset(event, 0, sizeof(struct wifi_event));
    memset(ind, 0, sizeof(struct wifi_event_data_ind_channel_switch));

    event->id = WIFI_EVENT_ID_IND_CHANNEL_SWITCH;
    ind->channel = channel;

    //FIXME race condition protect
    if (cb_event) {
        cb_event(cb_event_env, event);
    }
}

static void notify_event_scan_done(int join_scan)
{
    uint8_t buffer[sizeof(struct wifi_event) + sizeof(struct wifi_event_data_ind_scan_done)];
    struct wifi_event *event;
    struct wifi_event_data_ind_scan_done *ind;

    event = (struct wifi_event*)buffer;
    ind = (struct wifi_event_data_ind_scan_done*)event->data;
    memset(event, 0, sizeof(struct wifi_event));
    memset(ind, 0, sizeof(struct wifi_event_data_ind_scan_done));

    event->id = join_scan ? WIFI_EVENT_ID_IND_SCAN_DONE_ONJOIN : WIFI_EVENT_ID_IND_SCAN_DONE;
    ind->nothing = __LINE__;

    //FIXME race condition protect
    if (cb_event) {
        cb_event(cb_event_env, event);
    }
}

static int bl_rx_chan_switch_ind(struct bl_hw *bl_hw,
     struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    struct mm_channel_switch_ind *ind = (struct mm_channel_switch_ind *)msg->param;

    RWNX_DBG(RWNX_FN_ENTRY_STR);
    notify_event_channel_switch(ind->chan_index);
    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return 0;
}

static int bl_comman_ind(struct bl_hw *bl_hw,
     struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    RWNX_DBG(RWNX_FN_ENTRY_STR);
    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return 0;
}

static int bl_rx_rssi_status_ind(struct bl_hw *bl_hw,
      struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    struct mm_rssi_status_ind *ind = (struct mm_rssi_status_ind *)msg->param;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    //bl_os_printf("------> rssi_status %u:%u:%d\r\n", ind->vif_index, ind->rssi_status, ind->rssi);
    if (cb_rssi) {
        cb_rssi(cb_rssi_env, ind->rssi);
    }

    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return 0;
}

const static msg_cb_fct mm_hdlrs[MSG_I(MM_MAX)] = {
    [MSG_I(MM_CHANNEL_SWITCH_IND)]     = bl_rx_chan_switch_ind,
    [MSG_I(MM_CHANNEL_PRE_SWITCH_IND)] = bl_comman_ind,
    [MSG_I(MM_REMAIN_ON_CHANNEL_EXP_IND)] = bl_comman_ind,
    [MSG_I(MM_PS_CHANGE_IND)]          = bl_comman_ind,
    [MSG_I(MM_TRAFFIC_REQ_IND)]        = bl_comman_ind,
    [MSG_I(MM_CHANNEL_SURVEY_IND)]     = bl_comman_ind,
    [MSG_I(MM_RSSI_STATUS_IND)]        = bl_rx_rssi_status_ind,
};

static int bl_rx_scanu_start_cfm(struct bl_hw *bl_hw,
      struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    RWNX_DBG(RWNX_FN_ENTRY_STR);

#if 0
    if (bl_hw->preq_ie.buf) {
        dma_unmap_single(bl_hw->dev, bl_hw->preq_ie.dma_addr,
                         bl_hw->preq_ie.bufsz, DMA_TO_DEVICE);
        kfree(bl_hw->preq_ie.buf);
        bl_hw->preq_ie.buf = NULL;
    }

    if (bl_hw->scan_request)
        cfg80211_scan_done(bl_hw->scan_request, false);

    bl_hw->scan_request = NULL;
#endif
    notify_event_scan_done(0);
    RWNX_DBG(RWNX_FN_LEAVE_STR);

    return 0;
}

static int bl_rx_scanu_join_cfm(struct bl_hw *bl_hw, struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    notify_event_scan_done(1);

    return 0;
}

static int find_ie_ssid(uint8_t *buffer, int len, uint8_t *result, int *ssid_len)
{
    int i;

    i = 0;
    while (i < len) {
#define IE_ID_SSID 0x00
        if (IE_ID_SSID == buffer[0]) {//FIXME no magic ID for SSID
            if (buffer[1] > 32) {
                return -1;
            }
            *ssid_len = buffer[1];
            memcpy(result, &(buffer[2]), buffer[1]);//NO check for len valid
            result[buffer[1]] = '\0';
            return 0;
        } else {
            /*move to next ie*/
            i += buffer[1] + 2;//FIXME 2 is for id and len
            buffer = buffer + buffer[1] + 2;
        }
    }

    return -1;
}

static int find_ie_ds(uint8_t *buffer, int len, uint8_t *result)
{
    int i;

    i = 0;
    while (i < len) {
#define IE_ID_DS_CHANNEL 0x03
        if (IE_ID_DS_CHANNEL == buffer[0]) {//FIXME no magic DS for SSID
            if (buffer[1] > 32) {
                return -1;
            }
            *result = buffer[2];
            return 0;
        } else {
            /*move to next ie*/
            i += buffer[1] + 2;//FIXME 2 is for id and len
            buffer = buffer + buffer[1] + 2;
        }
    }

    return -1;
}

extern uint32_t mac_vsie_find(uint32_t addr, uint16_t buflen, uint8_t const *oui, uint8_t ouilen);
extern uint32_t mac_ie_find(uint32_t addr, uint16_t buflen, uint8_t ie_id);
static uint8_t co_read8p(uint32_t addr)
{
    return (*(uint8_t *)addr);
}
#define MAC_INFOELT_LEN_OFT               (1)
#define MAC_INFOELT_INFO_OFT              (2)

#define MAX_RSN_WPA_IE_LEN                (32)
static void _rx_handle_beacon(struct scanu_result_ind *ind, struct ieee80211_mgmt *mgmt)
{
    struct wifi_event_beacon_ind ind_new;
    uint32_t var_part_addr, var_part_len;
    uint32_t elmt_addr;
    uint32_t rsn_wpa_ie_len;
    wifi_wpa_ie_t wpa_ie, rsn_ie;

    memset(&ind_new, 0, sizeof(ind_new));

    var_part_addr = (uint32_t)(mgmt->u.beacon.variable);
    var_part_len = ind->length - (uint32_t)&(((struct ieee80211_mgmt*)NULL)->u.beacon.variable);

    find_ie_ssid(mgmt->u.beacon.variable, ind->length, ind_new.ssid, &ind_new.ssid_len);
    find_ie_ds(mgmt->u.beacon.variable, ind->length, &ind_new.channel);

    elmt_addr = mac_vsie_find(var_part_addr, var_part_len, (const uint8_t *) "\x00\x50\xF2\x04", 4);
    if (elmt_addr != 0)
    {
        /*wps is suoported*/
        ind_new.wps = 1;
    } else {
        /*wps isn't supported*/
        ind_new.wps = 0;
    }

    /* TODO: Only consider 2.4G and bgn mode 
     * (NO 5G and a/ac/ax) / (NO g-only and n-only difference)
     */
    #define MAC_ELTID_HT_CAPA                45
    #define MAC_ELTID_EXT_RATES              50
    if (mac_ie_find(var_part_addr, var_part_len, MAC_ELTID_HT_CAPA))
    {
        ind_new.mode = (WIFI_MODE_802_11B | WIFI_MODE_802_11G | WIFI_MODE_802_11N_2_4);
    }
    else if (mac_ie_find(var_part_addr, var_part_len, MAC_ELTID_EXT_RATES))
    {
        ind_new.mode = (WIFI_MODE_802_11B | WIFI_MODE_802_11G);
    }
    else
    {
        ind_new.mode = (WIFI_MODE_802_11B);
    }

    if (WLAN_CAPABILITY_PRIVACY & (le16_to_cpu(mgmt->u.beacon.capab_info))) {
        wifi_wpa_ie_t *parsed_wpa_ie[2] = {};
        int parsed_wpa_ie_len = 0;
        int i;
        bool tkip = false, ccmp = false;
        bool group_tkip = false, group_ccmp = false;

        #define MAC_ELTID_RSN_IEEE               48
        elmt_addr = mac_ie_find(var_part_addr, var_part_len, MAC_ELTID_RSN_IEEE);
        if (elmt_addr != 0) {
            rsn_wpa_ie_len = co_read8p(elmt_addr + MAC_INFOELT_LEN_OFT) + MAC_INFOELT_INFO_OFT;
            memset(&rsn_ie, 0, sizeof(rsn_ie));
            wpa_parse_wpa_ie_wrapper((uint8_t *)elmt_addr, rsn_wpa_ie_len, &rsn_ie);
            parsed_wpa_ie[parsed_wpa_ie_len++] = &rsn_ie;
        }

        elmt_addr = mac_vsie_find(var_part_addr, var_part_len, (uint8_t const *)"\x00\x50\xF2\x01", 4);
        if (elmt_addr != 0) {
            rsn_wpa_ie_len = co_read8p(elmt_addr + MAC_INFOELT_LEN_OFT) + MAC_INFOELT_INFO_OFT;
            wpa_parse_wpa_ie_wrapper((uint8_t *)elmt_addr, rsn_wpa_ie_len, &wpa_ie);
            parsed_wpa_ie[parsed_wpa_ie_len++] = &wpa_ie;
        }

        for (i = 0; i < parsed_wpa_ie_len; ++i) {
            wifi_wpa_ie_t *ie = parsed_wpa_ie[i];
            int pc = ie->pairwise_cipher;
            int gc = ie->group_cipher;
            int ciphers[2] = { pc, gc };
            int j;

            if (ie->proto == WPA_PROTO_WPA) {
                ind_new.auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK;
            } else if (ie->proto == WPA_PROTO_RSN) {
                if (ie->key_mgmt & (WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_PSK_SHA256)) {
                    ind_new.auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK;
                    if (ie->key_mgmt & WPA_KEY_MGMT_SAE) {
                        ind_new.auth = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE;
                    }
                } else if (ie->key_mgmt & WPA_KEY_MGMT_SAE) {
                    ind_new.auth = WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE;
                }
            }

            for (j = 0; j < sizeof(ciphers) / sizeof(ciphers[0]); ++j) {
                int cipher = ciphers[j];
                if (cipher == WIFI_CIPHER_TYPE_TKIP) {
                    tkip = true;
                    if (cipher == gc)
                        group_tkip = true;
                }
                if (cipher == WIFI_CIPHER_TYPE_CCMP) {
                    ccmp = true;
                    if (cipher == gc)
                        group_ccmp = true;
                }
                if (cipher == WIFI_CIPHER_TYPE_TKIP_CCMP) {
                    tkip = ccmp = true;
                    if (cipher == gc)
                        group_tkip = group_ccmp = true;
                }
            }
        }
        if (parsed_wpa_ie_len == sizeof(parsed_wpa_ie) / sizeof(parsed_wpa_ie[0])) {
            ind_new.auth = WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK;
        } else if (parsed_wpa_ie_len == 0) {
            ind_new.auth = WIFI_EVENT_BEACON_IND_AUTH_WEP;
            ind_new.cipher = WIFI_EVENT_BEACON_IND_CIPHER_WEP;
        }

        if (ccmp) {
            ind_new.cipher = WIFI_EVENT_BEACON_IND_CIPHER_AES;
        }
        if (tkip) {
            ind_new.cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP;
        }
        if (tkip && ccmp) {
            ind_new.cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES;
        }
        if (group_ccmp) {
            ind_new.group_cipher = WIFI_EVENT_BEACON_IND_CIPHER_AES;
        }
        if (group_tkip) {
            ind_new.group_cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP;
        }
        if (group_tkip && group_ccmp) {
            ind_new.group_cipher = WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES;
        }
    } else {
        /*This is an open BSS*/
        ind_new.auth = WIFI_EVENT_BEACON_IND_AUTH_OPEN;
    }

    ind_new.rssi = ind->rssi;
    ind_new.ppm_abs = ind->ppm_abs;
    ind_new.ppm_rel = ind->ppm_rel;
    ind_new.bssid[0]  = mgmt->bssid[0];
    ind_new.bssid[1]  = mgmt->bssid[1];
    ind_new.bssid[2]  = mgmt->bssid[2];
    ind_new.bssid[3]  = mgmt->bssid[3];
    ind_new.bssid[4]  = mgmt->bssid[4];
    ind_new.bssid[5]  = mgmt->bssid[5];
    cb_beacon_ind(cb_beacon_ind_env, &ind_new);
}

static void _rx_handle_probersp(struct scanu_result_ind *ind, struct ieee80211_mgmt *mgmt)
{
    _rx_handle_beacon(ind, mgmt);
}

static int bl_rx_scanu_result_ind(struct bl_hw *bl_hw, struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    struct scanu_result_ind *ind = (struct scanu_result_ind *)msg->param;
    struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)ind->payload;

    if (ieee80211_is_beacon(mgmt->frame_control)) {
        _rx_handle_beacon(ind, mgmt);
    } else if (ieee80211_is_probe_resp(mgmt->frame_control)) {
        _rx_handle_probersp(ind, mgmt);
    } else {
        bl_os_printf("Bug Scan IND?\r\n");
    }

    return 0;
}

const static msg_cb_fct scan_hdlrs[MSG_I(SCANU_MAX)] = {
    [MSG_I(SCANU_START_CFM)]           = bl_rx_scanu_start_cfm,
    [MSG_I(SCANU_JOIN_CFM)]            = bl_rx_scanu_join_cfm,
    [MSG_I(SCANU_RESULT_IND)]          = bl_rx_scanu_result_ind,
};

const static msg_cb_fct me_hdlrs[MSG_I(ME_MAX)] = {
    [MSG_I(ME_TKIP_MIC_FAILURE_IND)] = bl_comman_ind,
    [MSG_I(ME_TX_CREDITS_UPDATE_IND)] = bl_comman_ind,
};

static const char* _get_status_code_str(const struct reason_code list[], uint16_t len, uint16_t status_code)
{
    int i;

    for (i = 0; i < len; i++) {
        if (list[i].reason_code == status_code) {
            break;
        }
    }
    if (len == i) {
        return "Unknown Code";
    }

    return list[i].action;
}

const char* wifi_mgmr_get_sm_status_code_str(uint16_t status_code)
{
    return _get_status_code_str(sm_reason_list, sizeof(sm_reason_list)/sizeof(sm_reason_list[0]), status_code);
}

const char* wifi_mgmr_get_apm_status_code_str(uint16_t status_code)
{
    return _get_status_code_str(apm_reason_list, sizeof(apm_reason_list)/sizeof(apm_reason_list[0]), status_code);
}

static int bl_rx_sm_connect_ind(struct bl_hw *bl_hw,
                                         struct bl_cmd *cmd,
                                         struct ipc_e2a_msg *msg)
{
    struct sm_connect_ind *ind = (struct sm_connect_ind *)msg->param;
    struct bl_sta *sta;
    struct wifi_event_sm_connect_ind ind_new;
    struct bl_vif *bl_vif = NULL;

    USER_UNUSED(index);
    RWNX_DBG(RWNX_FN_ENTRY_STR);

    bl_os_printf("[RX] Connection Status\r\n");
    bl_os_printf("[RX]   status_code %u\r\n", ind->status_code);
    bl_os_printf("[RX]   reason_code %u\r\n", ind->reason_code);
    bl_os_printf("[RX]   connect result: %s\r\n", wifi_mgmr_get_sm_status_code_str(ind->status_code));
    bl_os_printf("[RX]   MAC %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            ind->bssid.array[0],
            ind->bssid.array[1],
            ind->bssid.array[2],
            ind->bssid.array[3],
            ind->bssid.array[4],
            ind->bssid.array[5]
    );
    bl_os_printf("[RX]   vif_idx %u\r\n", ind->vif_idx);
    bl_os_printf("[RX]   ap_idx %u\r\n", ind->ap_idx);
    bl_os_printf("[RX]   ch_idx %u\r\n", ind->ch_idx);
    bl_os_printf("[RX]   qos %u\r\n", ind->qos);
    bl_os_printf("[RX]   acm %u\r\n", ind->acm);
    bl_os_printf("[RX]   assoc_req_ie_len %u\r\n", ind->assoc_req_ie_len);
    bl_os_printf("[RX]   assoc_rsp_ie_len %u\r\n", ind->assoc_rsp_ie_len);
    bl_os_printf("[RX]   aid %u\r\n", ind->aid);
    bl_os_printf("[RX]   band %u\r\n", ind->band);
    bl_os_printf("[RX]   center_freq %u\r\n", ind->center_freq);
    bl_os_printf("[RX]   width %u\r\n", ind->width);
    bl_os_printf("[RX]   center_freq1 %u\r\n", (unsigned int)ind->center_freq1);
    bl_os_printf("[RX]   center_freq2 %u\r\n", (unsigned int)ind->center_freq2);
    bl_os_printf("[RX]   tlv_ptr first %p\r\n", ind->connect_diagnose.first);

    if (0 == ind->status_code) {
        bl_hw->sta_idx = ind->ap_idx;
        bl_hw->is_up = 1;
        sta = &(bl_hw->sta_table[bl_hw->sta_idx]);
        sta->qos = ind->qos;
    } else {
        bl_hw->is_up = 0;
    }
    RWNX_DBG(RWNX_FN_LEAVE_STR);

    memset(&ind_new, 0, sizeof(ind_new));
    ind_new.status_code = ind->status_code;
    ind_new.reason_code = ind->reason_code;
    ind_new.bssid[0] = ind->bssid.array[0];
    ind_new.bssid[1] = ind->bssid.array[1];
    ind_new.bssid[2] = ind->bssid.array[2];
    ind_new.bssid[3] = ind->bssid.array[3];
    ind_new.bssid[4] = ind->bssid.array[4];
    ind_new.bssid[5] = ind->bssid.array[5];
    ind_new.vif_idx = ind->vif_idx;
    ind_new.ap_idx = ind->ap_idx;
    ind_new.ch_idx = ind->ch_idx;
    ind_new.qos = ind->qos;
    ind_new.aid = ind->aid;
    ind_new.band = ind->band;
    ind_new.center_freq = ind->center_freq;
    ind_new.width = ind->width;
    ind_new.center_freq1 = ind->center_freq1;
    ind_new.center_freq2 = ind->center_freq2;
    ind_new.connect_diagnose = ind->connect_diagnose;

    if (cb_sm_connect_ind) {
        cb_sm_connect_ind(cb_sm_connect_ind_env, &ind_new);
    }

    if (ind->status_code) {
        /*empty here*/
        //XXX no netif_set_link_down here?
    } else {
        bl_vif = &(bl_hw->vif_table[ind->vif_idx]);
        if (bl_vif && bl_vif->dev) {
            netifapi_netif_set_link_up(bl_vif->dev);
            netifapi_netif_set_default(bl_vif->dev);
#ifdef BL602_MATTER_SUPPORT
            netif_create_ip6_linklocal_address(bl_vif->dev, 1);
            bl_vif->dev->ip6_autoconfig_enabled = 1;
#endif
        } else {
            bl_os_printf("[RX]  -------- CRITICAL when check netif. ptr is %p:%p\r\n",
                    bl_vif,
                    bl_vif ? bl_vif->dev : NULL
            );
        }
    }
    return 0;
}

static int bl_rx_sm_disconnect_ind(struct bl_hw *bl_hw,
                                            struct bl_cmd *cmd,
                                            struct ipc_e2a_msg *msg)
{
    struct sm_disconnect_ind *ind  = (struct sm_disconnect_ind*)msg->param;
    struct wifi_event_sm_disconnect_ind ind_new;
    struct bl_vif *bl_vif = NULL;
    ip4_addr_t addr_any;

    RWNX_DBG(RWNX_FN_ENTRY_STR);

    addr_any.addr = inet_addr("0.0.0.0");
    bl_os_printf("[RX]   sm_disconnect_ind\r\n"
              "       status_code %u\r\n"
              "       802.11 reason_code %u\r\n", ind->status_code, ind->reason_code);
    bl_os_printf("[RX]   disconnect reason: %s\r\n", wifi_mgmr_get_sm_status_code_str(ind->status_code));
    bl_os_printf("[RX]   vif_idx %u\r\n", ind->vif_idx);
    bl_os_printf("[RX]   ft_over_ds %u\r\n", ind->ft_over_ds);
    bl_os_printf("[RX]   tlv_ptr first %p\r\n", ind->connect_diagnose.first);

    if (cb_sm_disconnect_ind) {
        memset(&ind_new, 0, sizeof(ind_new));
        ind_new.status_code = ind->status_code;
        ind_new.reason_code = ind->reason_code;
        ind_new.vif_idx = ind->vif_idx;
        ind_new.ft_over_ds = ind->ft_over_ds;
        ind_new.connect_diagnose = ind->connect_diagnose;
        cb_sm_disconnect_ind(cb_sm_disconnect_ind_env, &ind_new);
    }

    //TODO check vif_idx valid?
    bl_vif = &(bl_hw->vif_table[ind->vif_idx]);
    if (bl_vif && bl_vif->dev) {
        netifapi_netif_set_link_down(bl_vif->dev);
        netifapi_netif_set_addr(bl_vif->dev, &addr_any, &addr_any, &addr_any);
    }
    RWNX_DBG(RWNX_FN_LEAVE_STR);
    return 0;
}

const static msg_cb_fct sm_hdlrs[MSG_I(SM_MAX)] = {
    [MSG_I(SM_CONNECT_IND)]    = bl_rx_sm_connect_ind,
    [MSG_I(SM_DISCONNECT_IND)] = bl_rx_sm_disconnect_ind,
};

static int bl_rx_apm_sta_add_ind(struct bl_hw *bl_hw, struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    struct apm_sta_add_ind *ind = (struct apm_sta_add_ind*)msg->param;
    struct bl_sta *sta;

    bl_os_printf("[WF] APM_STA_ADD_IND\r\n");
    bl_os_printf("[WF]    flags %08X\r\n", (unsigned int)ind->flags);
    bl_os_printf("[WF]    MAC %02X:%02X:%02X:%02X:%02X:%02X\r\n",
            ind->sta_addr.array[0] & 0xFF,
            ind->sta_addr.array[1] & 0xFF,
            ind->sta_addr.array[2] & 0xFF,
            ind->sta_addr.array[3] & 0xFF,
            ind->sta_addr.array[4] & 0xFF,
            ind->sta_addr.array[5] & 0xFF
    );
    bl_os_log_info("[WF]    tsflo: 0x%lx\r\n", ind->tsflo);
    bl_os_log_info("[WF]    tsfhi: 0x%lx\r\n", ind->tsfhi);
    bl_os_log_info("[WF]    rssi: %d\r\n", ind->rssi);
    bl_os_log_info("[WF]    data rate: 0x%x\r\n", ind->data_rate);

    bl_os_printf("[WF]    vif_idx %u\r\n", ind->vif_idx);
    bl_os_printf("[WF]    sta_idx %u\r\n", ind->sta_idx);
    if (ind->sta_idx < sizeof(bl_hw->sta_table)/sizeof(bl_hw->sta_table[0])) {
        sta = &(bl_hw->sta_table[ind->sta_idx]);
        if (sta->is_used) {
            bl_os_log_info("-------------------------Warning: sta_idx already used: %d\r\n", ind->sta_idx);
        }
        memcpy(sta->sta_addr.array, ind->sta_addr.array, 6);
        sta->sta_idx = ind->sta_idx;
        sta->vif_idx = ind->vif_idx;
        sta->is_used = 1;
        sta->rssi = ind->rssi;
        sta->tsflo = ind->tsflo;
        sta->tsfhi = ind->tsfhi;
        sta->data_rate = ind->data_rate;
    } else {
        bl_os_printf("[WF]    ------ Potential illegal sta_idx\r\n");
    }
    aos_post_event(EV_WIFI, CODE_WIFI_ON_AP_STA_ADD, ind->sta_idx);

    return 0;
}

static int bl_rx_apm_sta_del_ind(struct bl_hw *bl_hw, struct bl_cmd *cmd, struct ipc_e2a_msg *msg)
{
    struct apm_sta_del_ind *ind = (struct apm_sta_del_ind*)msg->param;
    struct bl_sta *sta;

    bl_os_printf("[WF] APM_STA_DEL_IND\r\n");
    bl_os_printf("[WF]    sta_idx %u\r\n", ind->sta_idx);
    bl_os_printf("[WF]    statuts_code %u\r\n", ind->status_code);
    bl_os_printf("[WF]    reason_code %u\r\n", ind->reason_code);
    bl_os_printf("[RX]    disconnect reason: %s\r\n", wifi_mgmr_get_apm_status_code_str(ind->status_code));

    if (ind->sta_idx < sizeof(bl_hw->sta_table)/sizeof(bl_hw->sta_table[0])) {
        sta = &(bl_hw->sta_table[ind->sta_idx]);
        if (0 == sta->is_used) {
            bl_os_log_info("[WF]    -------------------------Warning: sta_idx already empty: %d\r\n", ind->sta_idx);
        }
        sta->is_used = 0;
    } else {
        bl_os_printf("[WF]    --------- Potential illegal sta_idx\r\n");
    }
    aos_post_event(EV_WIFI, CODE_WIFI_ON_AP_STA_DEL, ind->sta_idx);

    return 0;
}

const static msg_cb_fct apm_hdlrs[MSG_I(APM_MAX)] = {
    [MSG_I(APM_STA_ADD_IND)] = bl_rx_apm_sta_add_ind,
    [MSG_I(APM_STA_DEL_IND)] = bl_rx_apm_sta_del_ind,
};

const static msg_cb_fct cfg_hdlrs[MSG_I(CFG_MAX)] = {
};
const static msg_cb_fct *msg_hdlrs[] = {
    [TASK_MM]    = mm_hdlrs,
    [TASK_SCANU] = scan_hdlrs,
    [TASK_ME]    = me_hdlrs,
    [TASK_SM]    = sm_hdlrs,
    [TASK_APM]   = apm_hdlrs,
    [TASK_CFG]  =  cfg_hdlrs,
};

void bl_rx_handle_msg(struct bl_hw *bl_hw, struct ipc_e2a_msg *msg)
{
    //FIXME potential Array index overflow
    bl_hw->cmd_mgr.msgind(&bl_hw->cmd_mgr, msg, msg_hdlrs[MSG_T(msg->id)][MSG_I(msg->id)]);
}

void bl_rx_e2a_handler(void *arg)
{
    struct ipc_e2a_msg *msg = (struct ipc_e2a_msg*)arg;

    //FIXME potential Array index overflow
    wifi_hw.cmd_mgr.msgind(&wifi_hw.cmd_mgr, msg, msg_hdlrs[MSG_T(msg->id)][MSG_I(msg->id)]);
}

void bl_rx_pkt_cb(uint8_t *pkt, int len, void *pkt_wrap, bl_rx_info_t *info)
{
    if (cb_pkt) {
        cb_pkt(cb_pkt_env, pkt, len, info);
    }
    if (cb_pkt_adv) {
        cb_pkt_adv(cb_pkt_env, pkt_wrap, info);
    }
}
