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
#ifndef __WIFI_MGMR_API_H__
#define __WIFI_MGMR_API_H__
#include <stdint.h>
#include "wifi_mgmr.h"

/**
 ****************************************************************************************
 *
 * @file wifi_mgmr_api.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

typedef struct
{
    uint8_t wep40 : 1;
    uint8_t wep104 : 1;
    uint8_t tkip : 1;
    uint8_t ccmp : 1;
    uint8_t rsvd : 4;
} wifi_mgmr_api_cipher_t;

int wifi_mgmr_api_common_msg(WIFI_MGMR_EVENT_T ev, void *data1, void *data2);
int wifi_mgmr_api_connect(char *ssid, char *passphr, const ap_connect_adv_t *ext_param);
int wifi_mgmr_api_cfg_req(uint32_t ops, uint32_t task, uint32_t element, uint32_t type, uint32_t length, uint32_t *buf);
int wifi_mgmr_api_ip_got(void);
int wifi_mgmr_api_ip_update(void);
int wifi_mgmr_api_reconnect(void);
int wifi_mgmr_api_disconnect(void);
int wifi_mgmr_api_rate_config(uint16_t config);
int wifi_mgmr_api_conf_max_sta(uint8_t max_sta_supported);
int wifi_mgmr_api_ifaceup(void);
int wifi_mgmr_api_sniffer_enable(void);
int wifi_mgmr_api_ap_start(char *ssid, char *passwd, int channel, uint8_t hidden_ssid, int8_t max_sta_supported, uint8_t use_dhcp_server);
int wifi_mgmr_api_ap_stop(void);
int wifi_mgmr_api_idle(void);
int wifi_mgmr_api_channel_set(int channel, int use_40Mhz);
int wifi_mgmr_api_raw_send(uint8_t *pkt, int len);
int wifi_mgmr_api_set_country_code(char *country_code);

/*section for fw api*/
int wifi_mgmr_api_fw_disconnect(void);
int wifi_mgmr_api_fw_scan(wifi_mgmr_scan_params_t *scan_params);
#define WIFI_MGMR_API_FW_POWERSAVING_MODE_OFF           (1)
#define WIFI_MGMR_API_FW_POWERSAVING_MODE_ON            (2)
#define WIFI_MGMR_API_FW_POWERSAVING_MODE_DYNAMIC_ON    (3)
int wifi_mgmr_api_fw_powersaving(int mode);
int wifi_mgmr_api_disable_autoreconnect(void);
int wifi_mgmr_api_enable_autoreconnect(void);
int wifi_mgmr_api_denoise_enable(void);
int wifi_mgmr_api_denoise_disable(void);

/*section for global event*/
int wifi_mgmr_api_scan_item_beacon(uint8_t channel, int8_t rssi, uint8_t auth, uint8_t mac[], uint8_t ssid[], int len, int8_t ppm_abs, int8_t ppm_rel, uint8_t cipher, uint8_t wps, uint32_t mode, uint8_t group_cipher);
#endif
