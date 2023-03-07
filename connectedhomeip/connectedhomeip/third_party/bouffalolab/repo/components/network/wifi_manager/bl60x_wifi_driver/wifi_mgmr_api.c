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
#include <string.h>
#include <stdio.h>

#include "wifi_mgmr_api.h"
#include "bl_defs.h"

#define MAX_SSID_LEN_CHECK 32
#define MAX_PSK_LEN_CHECK 64

static int wifi_mgmr_api_common(WIFI_MGMR_EVENT_T ev, void *data1, void *data2, uint8_t *data)
{
    wifi_mgmr_msg_t msg;

    msg.ev = ev;
    msg.data1 = data1;
    msg.data2 = data2;
    msg.data = data;

    wifi_mgmr_event_notify(&msg, 0);

    return 0;
}

int wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_T ev, void *data1, void *data2)
{
    return wifi_mgmr_api_common(ev, data1, data2, NULL);
}

int wifi_mgmr_api_connect(char *ssid, char *passphr, const ap_connect_adv_t *ext_param)
{
    wifi_mgmr_profile_msg_t *profile = NULL;

    profile = (wifi_mgmr_profile_msg_t *)bl_os_zalloc(sizeof(wifi_mgmr_profile_msg_t));
    if (!profile) {
        goto failed;
    }

    profile->ssid_len = strlen(ssid);//ssid should never be NULL
    memcpy(profile->ssid, ssid, profile->ssid_len);
    profile->ssid_tail[0] = '\0';

    profile->passphr_len = passphr ? strlen(passphr) : 0;//passphr can be NULL
    if (profile->passphr_len > sizeof(profile->passphr)) {
        goto failed;
    } else if (profile->passphr_len > 0) {
        memcpy(profile->passphr, passphr, profile->passphr_len);
    }
    profile->passphr_tail[0] = '\0';

    profile->psk_len = ext_param->psk ? strlen(ext_param->psk) : 0; //psk can be NULL
    if (0 != profile->psk_len && sizeof(profile->psk) != profile->psk_len) {
        goto failed;
    } else if (sizeof(profile->psk) == profile->psk_len) {
        memcpy(profile->psk, ext_param->psk, profile->psk_len);
    } else if (0 == profile->psk_len && profile->passphr_len) {
        // Put PSK calculation here, Otherwise it will influence FW Response performance
        if (wifi_mgmr_psk_cal(profile->passphr, profile->ssid, profile->ssid_len, profile->psk)){
            return -1;
        }
        profile->psk_len = sizeof(profile->psk);
    }
    profile->psk_tail[0] = '\0';

    if (ext_param->ap_info.bssid) {
        memcpy(profile->bssid, ext_param->ap_info.bssid, sizeof(profile->bssid));
    }

    if (ext_param->ap_info.freq > 0) {
        //define the freq
        profile->band = ext_param->ap_info.band;
        profile->freq = ext_param->ap_info.freq;
        bl_os_printf("wifi mgmr band:%d freq: %d\r\n", profile->band, profile->freq);
    }

    if (ext_param->ap_info.type == AP_INFO_TYPE_PRESIST) {
        profile->ap_info_ttl = -1;
    } else if (ext_param->ap_info.time_to_live >= 0){
        profile->ap_info_ttl = ext_param->ap_info.time_to_live;
    } else {
        profile->ap_info_ttl = -1;
        bl_os_printf("invalid ap info type or time_to_live value!\r\n");
    }

    profile->dhcp_use = ext_param->ap_info.use_dhcp;
    profile->flags = ext_param->flags;

    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_APP_CONNECT,
        (void*)0x1,
        (void*)0x2,
        (void*)profile
    );

failed:
    if (profile) {
        bl_os_printf("%s malloc profile failed!\r\n", __FUNCTION__);
    } else {
        bl_os_printf("%s send profile failed!\r\n", __FUNCTION__);
        bl_os_free(profile);
    }
    return -1;
}

int wifi_mgmr_api_cfg_req(uint32_t ops, uint32_t task, uint32_t element, uint32_t type, uint32_t length, uint32_t *buf)
{
    if (length > MAX_LENGTH_LIMIT) {
        return -1;
    }

    wifi_mgmr_cfg_element_msg_t *cfg_req = NULL;
    cfg_req = (wifi_mgmr_cfg_element_msg_t *)bl_os_zalloc(sizeof(wifi_mgmr_cfg_element_msg_t) + length);
    if (!cfg_req) {
        bl_os_printf("%s malloc cfg_req failed!\r\n", __FUNCTION__);
        return -1;
    }

    cfg_req->ops = ops;
    cfg_req->task = task;
    cfg_req->element = element;
    cfg_req->type = type;
    cfg_req->length = length;
    if (length) {
        memcpy(cfg_req->buf, buf, length);
    }

    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_FW_CFG_REQ,
        (void*)0x1,
        (void*)0x2,
        (void*)cfg_req
    );
}

int wifi_mgmr_api_ip_got(void)
{
    wifi_mgmr_pending_task_set(WIFI_MGMR_PENDING_TASK_IP_GOT_BIT);
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_MGMR_WAKEUP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_ip_update(void)
{
    wifi_mgmr_pending_task_set(WIFI_MGMR_PENDING_TASK_IP_UPDATE_BIT);
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_MGMR_WAKEUP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_reconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_RECONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_disable_autoreconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_DISABLE_AUTORECONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_enable_autoreconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_GLB_ENABLE_AUTORECONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_disconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_DISCONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_rate_config(uint16_t config)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_RC_CONFIG, (void*)(intptr_t)config, (void*)0x2);
}

int wifi_mgmr_api_conf_max_sta(uint8_t max_sta_supported)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_CONF_MAX_STA, (void*)(intptr_t)max_sta_supported, (void*)0x2);
}

int wifi_mgmr_api_ifaceup(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_PHY_UP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_sniffer_enable(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_SNIFFER, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_scan_item_beacon(uint8_t channel, int8_t rssi, uint8_t auth, uint8_t mac[], uint8_t ssid[], int len, int8_t ppm_abs, int8_t ppm_rel, uint8_t cipher, uint8_t wps, uint32_t mode, uint8_t group_cipher)
{
    wifi_mgmr_scan_item_t scan;

    memset(&scan, 0, sizeof(scan));
    memcpy(scan.ssid, ssid, len);
    scan.ssid_tail[0] = '\0';
    scan.ssid_len = len;
    memcpy(scan.bssid, mac, sizeof(scan.bssid));
    scan.channel = channel;
    scan.rssi = rssi;
    scan.auth = auth;
    scan.cipher = cipher;
    scan.ppm_abs = ppm_abs;
    scan.ppm_rel = ppm_rel;
    scan.wps = wps;
    scan.mode = mode;
    scan.group_cipher = group_cipher;

    return wifi_mgmr_scan_beacon_save(&scan);
}

int wifi_mgmr_api_fw_disconnect(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_FW_DISCONNECT, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_fw_tsen_reload(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_RELOAD_TSEN, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_fw_scan(wifi_mgmr_scan_params_t *ch_req)
{
    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_FW_SCAN,
        (void*)0x1,
        (void*)0x2,
        (void*)ch_req
    );
}

int wifi_mgmr_api_fw_powersaving(int mode)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_FW_POWERSAVING, (void*)mode, (void*)0x2);
}

int wifi_mgmr_api_ap_start(char *ssid, char *passwd, int channel, uint8_t hidden_ssid, int8_t max_sta_supported, uint8_t use_dhcp_server)
{
    wifi_mgmr_ap_msg_t *ap = NULL;
    int ssid_len = ssid ? strlen(ssid) : 0;
    int psk_len = passwd ? strlen(passwd) : 0;

    if (!ssid || ssid_len > MAX_SSID_LEN_CHECK || (passwd && (psk_len < 8 || psk_len >= MAX_PSK_LEN_CHECK))) {
        bl_os_printf("%s error start ap with wrong paramters!\r\n", __FUNCTION__);
        return -1;
    }

    ap = (wifi_mgmr_ap_msg_t *)bl_os_zalloc(sizeof(wifi_mgmr_ap_msg_t));
    if (!ap) {
        bl_os_printf("%s malloc ap failed!\r\n", __FUNCTION__);
        return -1;
    }

    ap->ssid_len = ssid_len;
    memcpy(ap->ssid, ssid, ap->ssid_len);
    if (psk_len) {
        // Put PSK calculation here, Otherwise it will influence FW Response performance
        if (wifi_mgmr_psk_cal(passwd, ssid, ssid_len, ap->psk)) {
            return -1;
        }
        ap->psk_len = sizeof(ap->psk);
    }
    ap->channel = channel;
    ap->hidden_ssid = hidden_ssid ? 1 : 0;
    ap->use_dhcp_server = use_dhcp_server ? 1 : 0;
    ap->max_sta_supported = max_sta_supported;

    return wifi_mgmr_api_common(
        WIFI_MGMR_EVENT_APP_AP_START,
        (void*)0x1,
        (void*)0x2,
        (void*)ap
    );
}

int wifi_mgmr_api_ap_stop(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_AP_STOP, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_idle(void)
{
    return wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_APP_IDLE, (void*)0x1, (void*)0x2);
}

int wifi_mgmr_api_denoise_enable(void)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_APP_DENOISE,
        (void*)0x1,//use non-zero for enable denoise
        (void*)0x2
    );
}

int wifi_mgmr_api_denoise_disable(void)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_APP_DENOISE,
        (void*)0x0,//use non-zero for enable denoise
        (void*)0x2
    );
}

int wifi_mgmr_api_channel_set(int channel, int use_40Mhz)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_FW_CHANNEL_SET,
        (void*)channel,
        (void*)use_40Mhz
    );
}

/*TODO callback for RAW data send*/
int wifi_mgmr_api_raw_send(uint8_t *pkt, int len)
{
    return wifi_mgmr_api_common_msg(
        WIFI_MGMR_EVENT_FW_DATA_RAW_SEND,
        (void*)pkt,
        (void*)len
    );
}

int wifi_mgmr_api_set_country_code(char *country_code)
{
    wifi_mgmr_set_country_code_internal(country_code);

    return 0;
}

