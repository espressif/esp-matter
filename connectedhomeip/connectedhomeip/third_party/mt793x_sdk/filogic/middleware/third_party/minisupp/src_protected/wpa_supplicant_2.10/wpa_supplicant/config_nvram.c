/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "includes.h"

#include "common.h"
#include "config_wpa_supplicant.h"
#include "base64.h"
#include "common/wpa_common.h"
#include "wpa_supplicant_i.h"
#include "nvdm.h"
#ifdef CONFIG_DRIVER_INBAND
#include "misc.h"
//#include "wifi_api.h" // MUST before common.h for BIT redifined
//#include "wifi_scan.h"
#include "wifi_inband.h"
#include "driver_inband.h"
#include "wifi_default_config.h"
#include "wifi_init.h"
#ifdef CONFIG_WPS
#include "wifi_wps.h"
#endif
#endif

#ifdef CONFIG_DRIVER_GEN4M
#include "wifi_api.h"
#include "nvdm.h"
#include "ap/ap_config.h"
#endif
#include "wpa_debug.h"


extern struct wpa_global *global_entry;
extern wifi_config_t g_wifi_config;
//extern wifi_config_ext_t g_wifi_config_ext;
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

typedef enum _NDIS_802_11_AUTHENTICATION_MODE {
	Ndis802_11AuthModeOpen,
	Ndis802_11AuthModeShared,
	Ndis802_11AuthModeAutoSwitch,
	Ndis802_11AuthModeWPA,
	Ndis802_11AuthModeWPAPSK,
	Ndis802_11AuthModeWPANone,
	Ndis802_11AuthModeWPA2,
	Ndis802_11AuthModeWPA2PSK,
	Ndis802_11AuthModeWPA1WPA2,
	Ndis802_11AuthModeWPA1PSKWPA2PSK,
	Ndis802_11AuthModeWPA3,
	Ndis802_11AuthModeWPA3PSK,
	Ndis802_11AuthModeWPA2WPA3,
	Ndis802_11AuthModeWPA2PSKWPA3PSK,
	Ndis802_11AuthModeMax
	// Not a real mode, defined as upper bound
} NDIS_802_11_AUTHENTICATION_MODE, *PNDIS_802_11_AUTHENTICATION_MODE;

typedef enum _NDIS_802_11_WEP_STATUS {
	Ndis802_11WEPEnabled,
	Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
	Ndis802_11WEPDisabled,
	Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
	Ndis802_11WEPKeyAbsent,
	Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
	Ndis802_11WEPNotSupported,
	Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
	Ndis802_11TKIPEnable,
	Ndis802_11Encryption2Enabled = Ndis802_11TKIPEnable,
	Ndis802_11Encryption2KeyAbsent,
	Ndis802_11AESEnable,
	Ndis802_11Encryption3Enabled = Ndis802_11AESEnable,
	Ndis802_11Encryption3KeyAbsent,
	Ndis802_11TKIPAESMix,
	Ndis802_11Encryption4Enabled = Ndis802_11TKIPAESMix,    /* TKIP or AES mix */
	Ndis802_11Encryption4KeyAbsent,
	Ndis802_11GroupWEP40Enabled,
	Ndis802_11GroupWEP104Enabled,
#ifdef WAPI_SUPPORT
	Ndis802_11EncryptionSMS4Enabled,    /* WPI SMS4 support */
#endif /* WAPI_SUPPORT */
} NDIS_802_11_WEP_STATUS, *PNDIS_802_11_WEP_STATUS, NDIS_802_11_ENCRYPTION_STATUS, *PNDIS_802_11_ENCRYPTION_STATUS;

#ifdef CONFIG_WPS
static uint8_t  STA_Wsc_Pri_Dev_Type[8] = {0x00, 0x01, 0x00, 0x50, 0xf2, 0x04, 0x00, 0x01};
static uint8_t  AP_Wsc_Pri_Dev_Type[8] = {0x00, 0x06, 0x00, 0x50, 0xf2, 0x04, 0x00, 0x01};
#endif

extern wifi_config_ext_t g_wifi_config_ext;
extern void AtoH(char *src, char *dest, int destlen);

static void wpa_convert_user_wep_key(wifi_wep_key_t *wep_keys, uint8_t key_index, uint8_t key_len, uint8_t *wep_key)
{
	wep_keys->wep_tx_key_index = key_index;

	if (5 == key_len || 13 == key_len) {
		wep_keys->wep_key_length[key_index] = key_len;
		os_memcpy(wep_keys->wep_key[key_index], wep_key, key_len);
	} else if (10 == key_len || 26 == key_len) {
		wep_keys->wep_key_length[key_index] = key_len / 2;
		AtoH((char *)wep_key, (char *)&wep_keys->wep_key[key_index], (int)wep_keys->wep_key_length[key_index]);
	} else {
		wpa_printf(MSG_WARNING, "length of password is not suitable for WEP: %d", key_len);
	}
}

void wpa_config_set_auth(struct wpa_ssid *ssid, uint8_t auth_value)
{
	wpa_printf(MSG_DEBUG, "%s: auth_value = %d", __FUNCTION__, auth_value);
	ssid->auth_mode = auth_value;
	ssid->proto = 0;
	if ((auth_value == Ndis802_11AuthModeWPA) ||
			(auth_value == Ndis802_11AuthModeWPAPSK)) {
		ssid->proto = WPA_PROTO_WPA;
	}

	if ((auth_value == Ndis802_11AuthModeWPA2) ||
		(auth_value == Ndis802_11AuthModeWPA2PSK) ||
		(auth_value == Ndis802_11AuthModeWPA3PSK) ||
		(auth_value == Ndis802_11AuthModeWPA2PSKWPA3PSK)) {
		ssid->proto = WPA_PROTO_RSN;
	}

	if ((auth_value == Ndis802_11AuthModeWPA1WPA2) ||
	    (auth_value == Ndis802_11AuthModeWPA1PSKWPA2PSK)) {
		ssid->proto = (WPA_PROTO_RSN | WPA_PROTO_WPA);
	}

	ssid->ieee80211w = 0;
	if ((auth_value == Ndis802_11AuthModeWPAPSK) ||
	    (auth_value == Ndis802_11AuthModeWPA2PSK) ||
	    (auth_value == Ndis802_11AuthModeWPA1PSKWPA2PSK)) {
		ssid->key_mgmt = WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_FT_PSK;
	} else if (auth_value == Ndis802_11AuthModeWPA2PSKWPA3PSK) {
		ssid ->key_mgmt = WPA_KEY_MGMT_PSK | WPA_KEY_MGMT_FT_PSK |
							WPA_KEY_MGMT_SAE | WPA_KEY_MGMT_FT_SAE;
		ssid->ieee80211w = 1;
	} else if (auth_value == Ndis802_11AuthModeWPA3PSK) {
		ssid->key_mgmt = WPA_KEY_MGMT_SAE | WPA_KEY_MGMT_FT_SAE;
		ssid->ieee80211w = 2;
	} else {
		ssid->key_mgmt = WPA_KEY_MGMT_NONE;
	}

	ssid->auth_alg = WPA_AUTH_ALG_OPEN;
	if (auth_value == Ndis802_11AuthModeShared) {
		ssid->auth_alg = WPA_AUTH_ALG_SHARED;
	} else if (auth_value == Ndis802_11AuthModeAutoSwitch) {
		ssid->auth_alg |= WPA_AUTH_ALG_SHARED;
	}
}

static int wpa_get_user_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length)
{
	if (WIFI_PORT_AP == port) {
		os_memcpy(ssid, g_wifi_config.ap_config.ssid, WIFI_MAX_LENGTH_OF_SSID);
		*ssid_length = g_wifi_config.ap_config.ssid_length;
	} else if (WIFI_PORT_STA == port) {
		os_memcpy(ssid, g_wifi_config.sta_config.ssid, WIFI_MAX_LENGTH_OF_SSID);
		*ssid_length = g_wifi_config.sta_config.ssid_length;
	} else {
		return -1;
	}
	return 0;
}

static int wpa_get_user_wep_key(uint8_t port, wifi_wep_key_t *wep_keys)
{
	if (WIFI_PORT_AP == port) {
		uint8_t ap_key_index = (g_wifi_config_ext.ap_wep_key_index_present ? g_wifi_config_ext.ap_wep_key_index : 0);
		uint8_t ap_key_len = min(g_wifi_config.ap_config.password_length, WIFI_MAX_WEP_KEY_LENGTH);
		wpa_convert_user_wep_key(wep_keys, ap_key_index, ap_key_len, g_wifi_config.ap_config.password);
	} else if (WIFI_PORT_STA == port) {
		uint8_t sta_key_index = (g_wifi_config_ext.sta_wep_key_index_present ? g_wifi_config_ext.sta_wep_key_index : 0);
		uint8_t sta_key_len = min(g_wifi_config.sta_config.password_length, WIFI_MAX_WEP_KEY_LENGTH);
		wpa_convert_user_wep_key(wep_keys, sta_key_index, sta_key_len, g_wifi_config.sta_config.password);
	} else {
		return -1;
	}
	return 0;
}

static void wpa_store_wep_key(struct wpa_ssid *ssid, const wifi_wep_key_t *wep_key)
{
	ssid->wep_tx_keyidx = wep_key->wep_tx_key_index;
	for (int index = 0; index < NUM_WEP_KEYS; index++) {
		if ((5 == wep_key->wep_key_length[index])
				|| (13 == wep_key->wep_key_length[index])) {
			os_memcpy(ssid->wep_key[index], wep_key->wep_key[index], wep_key->wep_key_length[index]);
			ssid->wep_key_len[index] = (size_t)wep_key->wep_key_length[index];
		} else {
			/* no valid wep key for this index */
			ssid->wep_key_len[index] = 0;
		}
	}
}

void wpa_config_set_encr(struct wpa_ssid *ssid, uint8_t encr_value)
{
	wpa_printf(MSG_DEBUG, "%s: encr_value = %d", __FUNCTION__, encr_value);
	ssid->encr_type = encr_value;
	ssid->pairwise_cipher = WPA_CIPHER_NONE;
	ssid->group_cipher = WPA_CIPHER_NONE;
	if (encr_value == Ndis802_11TKIPEnable) {
		ssid->pairwise_cipher = WPA_CIPHER_TKIP;
		ssid->group_cipher = WPA_CIPHER_TKIP;
	}

	if (encr_value == Ndis802_11AESEnable) {
		ssid->pairwise_cipher = WPA_CIPHER_CCMP;
		ssid->group_cipher = WPA_CIPHER_CCMP;
	}

	if (encr_value == Ndis802_11TKIPAESMix) {
		ssid->pairwise_cipher = (WPA_CIPHER_CCMP | WPA_CIPHER_TKIP);
		ssid->group_cipher = (WPA_CIPHER_CCMP | WPA_CIPHER_TKIP);
	}

	if (ssid->mode == WPAS_MODE_INFRA) {
		if (ssid->auth_mode >= Ndis802_11AuthModeWPA) {
			ssid->group_cipher = (WPA_CIPHER_CCMP | WPA_CIPHER_TKIP);
		}
	}

	if (encr_value == Ndis802_11WEPEnabled) {
		ssid->pairwise_cipher = (WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104);
		ssid->group_cipher = (WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104);
	}
}

void wpa_config_set_wpapsk(struct wpa_ssid *ssid, u8 *passphrase, u8 passphrase_length)
{
#if 1
	os_memcpy(ssid->passphrase, passphrase, passphrase_length);
	ssid->passphrase[passphrase_length] = '\0';
	ssid->passphrase_len = passphrase_length;

	if (ssid->passphrase_len == WIFI_LENGTH_PASSPHRASE) {
		hexstr2bin(ssid->passphrase, ssid->psk, PMK_LEN);
	} else {
		wpa_config_update_psk(ssid);
	}

	ssid->psk_set = 1;
#else
	ssid->passphrase_len = passphrase_length;
	os_memcpy(ssid->passphrase, passphrase, passphrase_length);
	if (hexstr2bin(ssid->passphrase, ssid->psk, PMK_LEN) ||
			ssid->passphrase[PMK_LEN * 2] != '\0') {
		wpa_printf(MSG_INFO, "%s: 8 ~ 63 passphrase('%s').",
			   __FUNCTION__, ssid->passphrase);
		if (ssid->mode == WPAS_MODE_AP) {
			wpa_config_update_psk(ssid);
		} else if (ssid->mode == WPAS_MODE_INFRA) {
#ifdef MTK_NVDM_ENABLE
			uint8_t pmk_info[WIFI_MAX_LENGTH_OF_SSID + WIFI_LENGTH_PASSPHRASE + PMK_LEN] = {0};
			uint8_t pmk_info_zero[WIFI_MAX_LENGTH_OF_SSID + WIFI_LENGTH_PASSPHRASE + PMK_LEN] = {0};
			uint32_t pmk_info_len = sizeof(pmk_info);
			nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "PMK_INFO",
					    (uint8_t *)pmk_info, &pmk_info_len);
			if ((0 == os_memcmp(pmk_info, ssid->ssid, ssid->ssid_len)) &&
					(0 == os_memcmp(pmk_info + WIFI_MAX_LENGTH_OF_SSID, ssid->passphrase, ssid->passphrase_len)) &&
					(0 == os_memcmp(pmk_info + ssid->ssid_len, pmk_info_zero, WIFI_MAX_LENGTH_OF_SSID - ssid->ssid_len)) &&
					(0 == os_memcmp(pmk_info + WIFI_MAX_LENGTH_OF_SSID + ssid->passphrase_len, pmk_info_zero, WIFI_LENGTH_PASSPHRASE - ssid->passphrase_len))) {
				os_memcpy(ssid->psk, pmk_info + WIFI_MAX_LENGTH_OF_SSID + WIFI_LENGTH_PASSPHRASE, PMK_LEN);
				ssid->psk_set = 1;
				os_memcpy(global_entry->current_connection.ssid, ssid->ssid, ssid->ssid_len);
				global_entry->current_connection.ssid_len = ssid->ssid_len;
				os_memcpy(global_entry->current_connection.passphrase, ssid->passphrase, ssid->passphrase_len);
				global_entry->current_connection.passphrase_len = ssid->passphrase_len;
				os_memcpy(global_entry->current_connection.psk, ssid->psk, PMK_LEN);
			}
#endif //MTK_NVDM_ENABLE
		}
	} else
		wpa_printf(MSG_INFO, "%s: 64 psk ('%s').",
			   __FUNCTION__, ssid->passphrase);

	ssid->psk_set = 1;
#endif
}

static int wpa_get_user_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length)
{
	if (WIFI_PORT_AP == port) {
		os_memcpy(passphrase, g_wifi_config.ap_config.password, WIFI_LENGTH_PASSPHRASE);
		*passphrase_length = g_wifi_config.ap_config.password_length;
	} else if (WIFI_PORT_STA == port) {
		os_memcpy(passphrase, g_wifi_config.sta_config.password, WIFI_LENGTH_PASSPHRASE);
		*passphrase_length = g_wifi_config.sta_config.password_length;
	} else {
		return -1;
	}
	return 0;
}

static void save_wep_key_length(uint8_t *length, char *wep_key_len, uint8_t key_id)
{
	uint8_t id = 0;
	uint8_t index = 0;

	do {
		if ('\0' == wep_key_len[index]) {
			return;
		}
		if (key_id == id) {
			*length = (uint8_t)atoi(&wep_key_len[index]);
			return;
		}
		if (',' == wep_key_len[index++]) {
			id++;
		}
	} while (id < 4);
}

static void save_shared_key(uint8_t *wep_key, uint8_t *raw_wep_key, uint8_t length, uint8_t key_id)
{
	uint8_t id = 0;
	uint8_t index = 0;

	do {
		if ('\0' == raw_wep_key[index]) {
			return;
		}
		if (key_id == id) {
			memcpy(wep_key, &raw_wep_key[index], length);
			wep_key[length] = '\0';
			return;
		}
		if (',' == raw_wep_key[index++]) {
			id++;
		}
	} while (id < 4);
}

static int fetch_nvdm_val(const char *group_name, const char *item_name)
{
#ifdef MTK_NVDM_ENABLE
	uint8_t buff[PROFILE_BUF_LEN];
	uint32_t len;
	len = sizeof(buff);
	if (nvdm_read_data_item(group_name, item_name, buff, &len) ==
	    NVDM_STATUS_OK) {
		return atoi((char *)buff);
	} else {
		return -1;
	}
#else
	return -1;
#endif
}

#ifdef MTK_NVDM_ENABLE
static struct wpa_ssid *wpa_config_nvdm_load_profile(int id)
{
	char profile_name[16] = {};
	uint8_t buff[PROFILE_BUF_LEN];
	uint32_t len = sizeof(buff);
	int retval = 0;
	wifi_auth_mode_t auth_mode = WIFI_AUTH_MODE_OPEN;
	wifi_encrypt_type_t encrypt_type = WIFI_ENCRYPT_TYPE_WEP_DISABLED;
	wifi_wep_key_t wep_key = {{{0}}};
	uint8_t passphrase[WIFI_LENGTH_PASSPHRASE] = {0};
	uint8_t passphrase_length = 0;
	uint8_t password[WIFI_LENGTH_PASSPHRASE] = {0};
	uint8_t password_length = 0;
	uint8_t sta_ssid_len = 0;
	uint8_t sta_wep_key_index = 0;
	uint8_t sta_wep_key_index_present = 0;
	uint8_t sta_key_index = 0;
	uint8_t sta_key_len = 0;
	struct wpa_ssid *ssid = NULL;
	uint8_t channel = 0;
	uint32_t freq = 0;

	retval = snprintf(profile_name, sizeof(profile_name) - 1, "STA%d", id);
	if (retval <= 0)
		return NULL;

	len = sizeof(buff);
	if (nvdm_read_data_item(profile_name, "SsidLen", buff, &len) != NVDM_STATUS_OK)
		return NULL;
	sta_ssid_len = (uint8_t)atoi((char *)buff);
	if (sta_ssid_len > WPA_MAX_SSID_LEN || sta_ssid_len == 0)
		return NULL;

	ssid = os_zalloc(sizeof(*ssid));
	if (ssid == NULL)
		return NULL;
	ssid->ssid = (u8 *)os_zalloc(WPA_MAX_SSID_LEN + 1); /* 1-byte for '\0' */
	if (ssid->ssid == NULL) {
		os_free(ssid);
		wpa_printf(MSG_ERROR, "os_zalloc fail");
		return NULL;
	}

	len = sizeof(buff);
	if (nvdm_read_data_item(profile_name, "Ssid", buff, &len) != NVDM_STATUS_OK)
		goto fail;
	memcpy(ssid->ssid, buff, sta_ssid_len);
	ssid->ssid_len = sta_ssid_len;

	len = sizeof(buff);
	if (nvdm_read_data_item(profile_name, "EncrypType", buff, &len) != NVDM_STATUS_OK)
		goto fail;
	if (0 == (uint8_t)atoi((char *)buff)) { /*0 = WIFI_ENCRYPT_TYPE_WEP_ENABLED*/
		len = sizeof(buff);
		if (nvdm_read_data_item(profile_name, "DefaultKeyId", buff, &len) != NVDM_STATUS_OK)
			goto fail;
		sta_wep_key_index = (uint8_t)atoi((char *)buff);

		len = sizeof(buff);
		if (nvdm_read_data_item(profile_name, "SharedKeyLen", buff, &len) != NVDM_STATUS_OK)
			goto fail;
		save_wep_key_length(&password_length, (char *)buff, sta_wep_key_index);

		len = sizeof(buff);
		if (nvdm_read_data_item(profile_name, "SharedKey", buff, &len) != NVDM_STATUS_OK)
			goto fail;
		save_shared_key(password, buff, password_length, sta_wep_key_index);
	} else if (1 != (uint8_t)atoi((char *)buff)) {  /*1 = WIFI_ENCRYPT_TYPE_WEP_DISABLED*/
		sta_wep_key_index = 255;
		len = sizeof(buff);
		if (nvdm_read_data_item(profile_name, "WpaPskLen", buff, &len) != NVDM_STATUS_OK)
			goto fail;
		password_length = (uint8_t)atoi((char *)buff);
		if (password_length >= sizeof(password))
			goto fail;
		len = sizeof(buff);
		if (nvdm_read_data_item(profile_name, "WpaPsk", buff, &len) != NVDM_STATUS_OK)
			goto fail;
		memcpy(password, buff, password_length);
	}

	if (sta_wep_key_index == 255)
		sta_wep_key_index_present = 0;
	else
		sta_wep_key_index_present = 1;

	sta_key_index = sta_wep_key_index_present ? sta_wep_key_index : 0;
	sta_key_len = min(password_length, WIFI_MAX_WEP_KEY_LENGTH);
	wpa_convert_user_wep_key(&wep_key, sta_key_index, sta_key_len, password);
	wpa_store_wep_key(ssid, &wep_key);

	memcpy(passphrase, password, WIFI_LENGTH_PASSPHRASE);
	passphrase_length = password_length;

	if (passphrase_length <= PMK_LEN * 2) {
		ssid->passphrase = os_zalloc(PROFILE_BUF_LEN);
		if (ssid->passphrase == NULL) {
			wpa_printf(MSG_DEBUG, "passphrase os_zalloc faile.");
			goto fail;
		}
		wpa_config_set_wpapsk(ssid, (u8 *)passphrase, passphrase_length);
	}

	if (password_length != 0) {
		if ((retval = fetch_nvdm_val(profile_name, "AuthMode")) == -1) {
			wpa_printf(MSG_DEBUG, "use default auth mode");
			if (sta_wep_key_index_present == 0)
				auth_mode = WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
			else
				auth_mode = WIFI_AUTH_MODE_OPEN;
		} else {
			auth_mode = (wifi_auth_mode_t)retval;
		}
		if ((retval = fetch_nvdm_val(profile_name, "EncrypType")) == -1) {
			wpa_printf(MSG_DEBUG, "use default encrypt type");
			if (sta_wep_key_index_present == 0)
				encrypt_type = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;
			else
				encrypt_type = WIFI_ENCRYPT_TYPE_WEP_ENABLED;
		} else {
			encrypt_type = (wifi_encrypt_type_t)retval;
		}
	}

	wpa_config_set_auth(ssid, auth_mode);
	wpa_config_set_encr(ssid, encrypt_type);

	if (g_wifi_config_ext.sta_power_save_mode_present == 1)
		ssid->ps_mode = g_wifi_config_ext.sta_power_save_mode;
	else
		ssid->ps_mode = 0;
	ssid->bssid_set = 0;
	ssid->scan_ssid = 1;
	ssid->disabled = 0;
	ssid->mode = WPAS_MODE_INFRA;
	ssid->id = id - 1; /* 0-indexed */

	len = sizeof(buff);
	if (nvdm_read_data_item(profile_name, "Channel", buff, &len) != NVDM_STATUS_OK)
		goto fail;
	channel = (uint8_t)atoi((char *)buff);
	if ((ssid->scan_freq == NULL) && channel != 0)
		ssid->scan_freq = os_malloc(sizeof(int) * 2);
	if (ssid->scan_freq) {
		if (channel >= 1 && channel <= 13)
			freq = 2412 + (channel - 1) * 5;
		else if (channel == 14)
			freq = 2484;
		else if (channel >= 36 && channel <= 165)
			freq = 5000 + 5 * channel;
		else {
			wpa_printf(MSG_ERROR, "error channel %u", channel);
			os_free(ssid->scan_freq);
			ssid->scan_freq = NULL;
			goto fail;
		}

		ssid->scan_freq[0] = freq;
		ssid->scan_freq[1] = 0;
		wpa_printf(MSG_INFO, "scan_freq %u %lu ", ssid->scan_freq[0], freq);
	}
	return ssid;
fail:
	os_free(ssid->ssid);
	os_free(ssid);
	return NULL;
}
#endif

int wpa_get_setting_from_profile(uint8_t port, struct wpa_ssid *ssid)
{
	int ret_val = 0;
	wifi_auth_mode_t auth_mode = WIFI_AUTH_MODE_OPEN;
	wifi_encrypt_type_t encrypt_type = WIFI_ENCRYPT_TYPE_WEP_DISABLED;
	wifi_wep_key_t wep_key = {{{0}}};
	uint8_t passphrase[64] = {0};
	uint8_t passphrase_length = 0;
	uint8_t channel = 6;
	uint32_t freq = 0;

	ssid->ssid = (u8 *)os_zalloc(WPA_MAX_SSID_LEN + 1);/* 1-byte for '\0' */
	if (NULL == ssid->ssid) {
		wpa_printf(MSG_ERROR, "os_zalloc fail");
		return -1;
	}
	ret_val = wpa_get_user_ssid(port,
				    (uint8_t *)(ssid->ssid),
				    (uint8_t *)(&ssid->ssid_len));
	if (ret_val < 0 || ssid->ssid_len == 0) {
		wpa_printf(MSG_ERROR, "wpa_get_user_ssid fail");
		return -1;
	}

	if ((WIFI_PORT_STA == port) && (g_wifi_config.sta_config.bssid_present == 1)) {
		ssid->bssid_set = g_wifi_config.sta_config.bssid_present;
		os_memcpy(ssid->bssid, g_wifi_config.sta_config.bssid, ETH_ALEN);
	}

	ret_val = wpa_get_user_wep_key(port, &wep_key);
	if (ret_val < 0) {
		wpa_printf(MSG_ERROR, "wpa_get_user_wep_key fail");
		return -1;
	}
	wpa_store_wep_key(ssid, &wep_key);

	ret_val = wpa_get_user_wpa_psk_key(port, passphrase, &passphrase_length);
	if (ret_val < 0) {
		wpa_printf(MSG_ERROR, "wpa_get_user_wpa_psk_key fail");
		return -1;
	}

	if (passphrase_length <= (PMK_LEN * 2)) {
		ssid->passphrase = os_zalloc(PROFILE_BUF_LEN);
		if (ssid->passphrase == NULL) {
			wpa_printf(MSG_DEBUG, "passphrase os_zalloc faile.");
			return -1;
		}
		wpa_config_set_wpapsk(ssid, (u8 *)passphrase, passphrase_length);
	}

	if (WIFI_PORT_AP == port) {
		auth_mode = g_wifi_config.ap_config.auth_mode;
		encrypt_type = g_wifi_config.ap_config.encrypt_type;
	} else {
		if (g_wifi_config.sta_config.password_length != 0) {
			if ((ret_val = fetch_nvdm_val("STA", "AuthMode")) == -1) {
				wpa_printf(MSG_DEBUG, "use default auth mode");
				if (g_wifi_config_ext.sta_wep_key_index_present == 0)
					auth_mode = WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
				else
					auth_mode = WIFI_AUTH_MODE_OPEN;
			} else {
				auth_mode = (wifi_auth_mode_t)ret_val;
			}
			if ((ret_val = fetch_nvdm_val("STA", "EncrypType")) == -1) {
				wpa_printf(MSG_DEBUG, "use default encrypt type");
				if (g_wifi_config_ext.sta_wep_key_index_present == 0)
					encrypt_type = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;
				else
					encrypt_type = WIFI_ENCRYPT_TYPE_WEP_ENABLED;
			} else {
				encrypt_type = (wifi_encrypt_type_t)ret_val;
			}
		}
	}

	wpa_config_set_auth(ssid, auth_mode);
	wpa_config_set_encr(ssid, encrypt_type);

	if ((WIFI_PORT_STA == port) && (g_wifi_config_ext.sta_power_save_mode_present == 1)) {
		ssid->ps_mode = g_wifi_config_ext.sta_power_save_mode;
	} else {
		ssid->ps_mode = 0;
	}

	if ((port == WIFI_PORT_AP) && (g_wifi_config_ext.ap_dtim_interval_present == 1)) {
		ssid->dtim_period = g_wifi_config_ext.ap_dtim_interval;
	} else {
		ssid->dtim_period = 1;
	}

	channel = g_wifi_config.sta_config.channel;
	if ((ssid->scan_freq == NULL) && channel != 0)
		ssid->scan_freq = os_malloc(sizeof(int) * 2);
	if (ssid->scan_freq) {
		if (channel >= 1 && channel <= 13)
			freq = 2412 + (channel - 1) * 5;
		else if (channel == 14)
			freq = 2484;
		else if (channel >= 36 && channel <= 165)
			freq = 5000 + 5 * channel;
		else {
			wpa_printf(MSG_ERROR, "error channel %u", channel);
			os_free(ssid->scan_freq);
			ssid->scan_freq = NULL;
			return 0;
		}

		if (port == WIFI_PORT_STA) {
			ssid->scan_freq[0] = freq;
			ssid->scan_freq[1] = 0;
			wpa_printf(MSG_INFO, "scan_freq %u %lu ", ssid->scan_freq[0], freq);
		} else {
			wpa_printf(MSG_ERROR, "Not support.");
			os_free(ssid->scan_freq);
			ssid->scan_freq = NULL;
			return 0;
		}
	}

	return 0;
}

/* struct wpa_config *wpa_config_read(const char *name) */
struct wpa_config *wpa_config_read(const char *name, struct wpa_config *cfgp)
{
	struct wpa_config *config;
	struct wpa_ssid *ssid, *head = NULL, *tail = NULL;
	unsigned char op_mode = global_entry->op_mode;

	wpa_printf(MSG_DEBUG, "iverson wpa_config_read  opmode %d \n", op_mode);

	config = wpa_config_alloc_empty(NULL, NULL);
	if (config == NULL) {
		return NULL;
	}

	ssid = os_zalloc(sizeof(*ssid));
	if (ssid == NULL) {
		return NULL;
	}

	/**
	 *  Conf example:
	 *
	 *  network={
	 *   ssid="APE-HAP-11ac"
	 *   key_mgmt=WPA-PSK
	 *   proto=RSN
	 *   pairwise=CCMP
	 *   group=CCMP
	 *   psk="shuliang"
	 *   mode=0
	 *   disabled=0
	 *  }
	 *
	 *  network={
	 *   ssid="APE-HAP-11ac"
	 *   key_mgmt=NONE
	 *   disabled=0
	 *  }
	 *
	 */

	head = tail = ssid;
	ssid->id = 0;

	/*
	 * mode - IEEE 802.11 operation mode (Infrastucture/IBSS)
	 *
	 * 0 = infrastructure (Managed) mode, i.e., associate with an AP.
	 *
	 * 1 = IBSS (ad-hoc, peer-to-peer)
	 *
	 * 2 = AP (access point)
	 *
	 * 3 = P2P Group Owner (can be set in the configuration file)
	 *
	 * 4 = P2P Group Formation (used internally; not in configuration
	 * files)
	*/
	//    wpa_printf(MSG_DEBUG, "%s: op_mode = %d", __FUNCTION__, op_mode);
	if (op_mode == 3) {
		if (os_strstr(name, "ap"))
#ifdef CONFIG_AP
			ssid->mode = WPAS_MODE_AP;
#else
			ssid->mode = WPAS_MODE_P2P_GO;
#endif
		else
			ssid->mode = WPAS_MODE_INFRA;
	} else if (op_mode == 2) {
		ssid->mode = WPAS_MODE_AP;
	} else {
		ssid->mode = WPAS_MODE_INFRA;
	}

#if 1
	if ((ssid->mode == WPAS_MODE_AP) || (ssid->mode == WPAS_MODE_P2P_GO)) {
		wpa_get_setting_from_profile(WIFI_PORT_AP, ssid);
	} else {
		wpa_get_setting_from_profile(WIFI_PORT_STA, ssid);

#ifdef MTK_NVDM_ENABLE
		for (int id = 2;; ++id) {
			tail->next = wpa_config_nvdm_load_profile(id);
			if (tail->next == NULL) {
				wpa_printf(MSG_DEBUG, "next profile %d is empty \n", id);
				break;
			}
			tail = tail->next;
		}
#endif
	}
#endif

	/*
	    Scan this SSID with Probe Requests to improve connection time.
	*/
	ssid->scan_ssid = 1;
	/* Enable 11n by default */
	ssid->ht = 1;
	ssid->disabled = 0;
	config->ssid = head;
	tail->next = NULL;
	tail = head;
	while (tail) {
		tail->pnext = NULL;
		if (wpa_config_add_prio_network(config, tail) < 0) {
			wpa_config_free(config);
			return NULL;
		}
		tail = tail->next;
	}

	/* Need to implement/modify */
	//wpa_config_debug_dump_networks(config);
	return config;
}

int wpa_config_write(const char *name, struct wpa_config *config)
{
	struct wpa_ssid *ssid;
	struct wpa_config_blob *blob;

	wpa_printf(MSG_DEBUG, "Writing configuration file '%s'", name);

	/* TODO: write global config parameters */


	for (ssid = config->ssid; ssid; ssid = ssid->next) {
		/* TODO: write networks */
	}

	for (blob = config->blobs; blob; blob = blob->next) {
		/* TODO: write blobs */
	}

	return 0;
}

nvdm_status_t hostapd_config_read(const char *data_item_name,
				  char *buf, int *errors, uint32_t size)
{
	nvdm_status_t status;

	os_memset(buf, 0x0, size);
	status = nvdm_read_data_item("hapd", data_item_name,
				     (uint8_t *)buf, &size);

	if (status != NVDM_STATUS_OK) {
		wpa_printf(MSG_ERROR,
			   "invalid/unknown %s %d",
			   data_item_name, status);
		(*errors)++;
	} else {
		wpa_printf(MSG_DEBUG,
			   "%s %s=%s",
			   __func__, data_item_name, buf);
	}

	return status;
}

int hostapd_config_fill(struct hostapd_config *conf,
			struct hostapd_bss_config *bss, struct wpa_ssid *ssid)
{
	char buffer[SSID_MAX_LEN];
	nvdm_status_t status;
	uint32_t size = 0;
	int errors = 0;
	uint8_t channel = g_wifi_config.ap_config.channel == 0 ? 6 : g_wifi_config.ap_config.channel;

	size = sizeof(conf->bss[0]->iface);
	status = nvdm_read_data_item("hapd", "interface",
				     (uint8_t *)conf->bss[0]->iface, &size);

	if (status != NVDM_STATUS_OK) {
		wpa_printf(MSG_ERROR,
			   "invalid/unknown interface '%s'",
			   conf->bss[0]->iface);
		errors++;
	}
	wpa_printf(MSG_DEBUG, "interface='%s'", conf->bss[0]->iface);

	size = sizeof(buffer);
	if (hostapd_config_read("driver", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		int j;
		/* clear to get error below if setting is invalid */
		conf->driver = NULL;
		for (j = 0; wpa_drivers[j]; j++) {
			if (os_strcmp(buffer, wpa_drivers[j]->name) == 0) {
				conf->driver = wpa_drivers[j];
				break;
			}
		}
		if (conf->driver == NULL) {
			wpa_printf(MSG_ERROR,
				   "invalid/unknown driver '%s'",
				   buffer);
			errors++;
		}
	}

	if (hostapd_config_read("logger_syslog", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->logger_syslog = atoi(buffer);
	}

	if (hostapd_config_read("logger_syslog_level", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->logger_syslog_level = atoi(buffer);
	}

	if (hostapd_config_read("logger_stdout", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->logger_stdout = atoi(buffer);
	}

	if (hostapd_config_read("logger_stdout_level", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->logger_stdout_level = atoi(buffer);
	}

	if (bss->ssid.ssid_len == 0) {
		if (hostapd_config_read("ssid", buffer, &errors, size)
				== NVDM_STATUS_OK) {
			bss->ssid.ssid_len = os_strlen(buffer);
			if (bss->ssid.ssid_len > SSID_MAX_LEN ||
					bss->ssid.ssid_len < 1) {
				wpa_printf(MSG_ERROR, "invalid SSID '%s'",
					   buffer);
				errors++;
			} else {
				os_memcpy(bss->ssid.ssid, buffer, bss->ssid.ssid_len);
				bss->ssid.ssid_set = 1;
			}
		}
	}

	if (hostapd_config_read("macaddr_acl", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->macaddr_acl = atoi(buffer);
		if (bss->macaddr_acl != ACCEPT_UNLESS_DENIED &&
				bss->macaddr_acl != DENY_UNLESS_ACCEPTED &&
				bss->macaddr_acl != USE_EXTERNAL_RADIUS_AUTH) {
			wpa_printf(MSG_ERROR, "unknown macaddr_acl %d",
				   bss->macaddr_acl);
			errors++;
		}
	}

	if (conf->hw_mode == NUM_HOSTAPD_MODES) {
		if (channel >= 1 && channel <= 14)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211G;
		else if (channel >= 36 && channel <= 165)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211A;
	}

	if (conf->channel == 0) {
		if (hostapd_config_read("channel", buffer, &errors, size)
				== NVDM_STATUS_OK) {
			if (os_strcmp(buffer, "acs_survey") == 0) {
#ifndef CONFIG_ACS
				wpa_printf(MSG_ERROR, "tries to enable ACS but CONFIG_ACS disabled");
				errors++;
#else /* CONFIG_ACS */
				conf->acs = 1;
				conf->channel = 0;
#endif /* CONFIG_ACS */
			} else {
				conf->channel = channel;
				conf->acs = conf->channel == 0;
			}
		}
	}

	if (!ssid->beacon_int) {
		if (hostapd_config_read("beacon_int", buffer, &errors, size)
				== NVDM_STATUS_OK) {
			int val = atoi(buffer);
			/* MIB defines range as 1..65535, but very small values
			 * cause problems with the current implementation.
			 * Since it is unlikely that this small numbers are
			 * useful in real life scenarios, do not allow beacon
			 * period to be set below 15 TU. */
			if (val < 15 || val > 65535) {
				wpa_printf(MSG_ERROR, "invalid beacon_int %d (expected 15..65535)",
					   val);
				errors++;
			}
			conf->beacon_int = val;
		}
	}

	if (!ssid->dtim_period) {
		if (hostapd_config_read("dtim_period", buffer, &errors, size)
				== NVDM_STATUS_OK) {
			bss->dtim_period = atoi(buffer);
			if (bss->dtim_period < 1 || bss->dtim_period > 255) {
				wpa_printf(MSG_ERROR, "invalid dtim_period %d",
					   bss->dtim_period);
				errors++;
			}
		}
	}

	if (hostapd_config_read("max_num_sta", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->max_num_sta = atoi(buffer);
		if (bss->max_num_sta < 0 ||
				bss->max_num_sta > MAX_STA_COUNT) {
			wpa_printf(MSG_ERROR, "Invalid max_num_sta=%d; allowed range 0..%d",
				   bss->max_num_sta, MAX_STA_COUNT);
			errors++;
		}
	}

	if (hostapd_config_read("rts_threshold", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		conf->rts_threshold = atoi(buffer);
		if (conf->rts_threshold < -1 || conf->rts_threshold > 65535) {
			wpa_printf(MSG_ERROR,
				   "invalid rts_threshold %d",
				   conf->rts_threshold);
			errors++;
		}
	}

	if (hostapd_config_read("fragm_threshold", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		conf->fragm_threshold = atoi(buffer);
		if (conf->fragm_threshold == -1) {
			/* allow a value of -1 */
		} else if (conf->fragm_threshold < 256 ||
				conf->fragm_threshold > 2346) {
			wpa_printf(MSG_ERROR,
				   "invalid fragm_threshold %d",
				   conf->fragm_threshold);
			errors++;
		}
	}

	if (hostapd_config_read("auth_algs", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->auth_algs = atoi(buffer);
		if (bss->auth_algs == 0) {
			wpa_printf(MSG_ERROR, "no authentication algorithms allowed");
			errors++;
		}
	}

	if (hostapd_config_read("ignore_broadcast_ssid", buffer, &errors, size)
			== NVDM_STATUS_OK) {
		bss->ignore_broadcast_ssid = atoi(buffer);
	}

	return errors;
}

struct wpa_config *wpa_config_alloc_new_conf(struct wpa_ssid *old_ssid)
{
	//rtos_porting

	struct wpa_config *config;
	struct wpa_ssid *ssid, *head = NULL, *tail = NULL;

	config = wpa_config_alloc_empty(NULL, NULL);
	if (config == NULL) {
		return NULL;
	}
	ssid = os_zalloc(sizeof(*ssid));
	if (ssid == NULL) {
		return NULL;
	}

	os_memcpy(ssid, old_ssid, sizeof(*ssid));
	/* Reset all freed at next reload allocated memory
	 * which will be freed afterword in wpa_config_free
	 * TODO: check other "will be freed struct" and set it as NULL
	 * void wpa_config_free_ssid(struct wpa_ssid *ssid)
	 * os_free(ssid->ssid);
	 * str_clear_free(ssid->passphrase);
	 * os_free(ssid->ext_psk);
	 * #ifdef IEEE8021X_EAPOL
	 * eap_peer_config_free(&ssid->eap);
	 * #endif
	 * os_free(ssid->id_str);
	 * os_free(ssid->scan_freq);
	 * os_free(ssid->freq_list);
	 * os_free(ssid->bgscan);
	 * os_free(ssid->p2p_client_list);
	 * #ifdef CONFIG_HT_OVERRIDES
	 * os_free(ssid->ht_mcs);
	 * #endif
	 * #ifdef CONFIG_MESH
	 * os_free(ssid->mesh_basic_rates);
	 * #endif
	 * dl_list_init(&hapd->ctrl_dst);
	 * while ((psk = dl_list_first(&ssid->psk_list, struct psk_list_entry,
	 * 	list))) {
	 * 	dl_list_del(&psk->list);
	 * 	bin_clear_free(psk, sizeof(*psk));
	 * 	}
	 * 	bin_clear_free(ssid, sizeof(*ssid));
	 */
	ssid->ext_psk = NULL;
	ssid->id_str = NULL;
	ssid->scan_freq = NULL;
	ssid->freq_list = NULL;
	ssid->bgscan = NULL;
	ssid->p2p_client_list = NULL;
#ifdef CONFIG_HT_OVERRIDES
	ssid->ht_mcs = NULL;
#endif
#ifdef CONFIG_MESH
	ssid->mesh_basic_rates = NULL;
#endif
	ssid->psk_set = 0;
	dl_list_init(&ssid->psk_list);

	ssid->ssid = os_zalloc(WIFI_MAX_LENGTH_OF_SSID + 1);
	if (ssid->ssid) {
		os_memset(ssid->ssid, 0, WIFI_MAX_LENGTH_OF_SSID + 1);
		os_memcpy(ssid->ssid, old_ssid->ssid, old_ssid->ssid_len);
	}
	ssid->passphrase = os_zalloc(WIFI_LENGTH_PASSPHRASE + 1);
	if (ssid->passphrase) {
		os_memset(ssid->passphrase, 0, WIFI_LENGTH_PASSPHRASE + 1);
		os_memcpy(ssid->passphrase, old_ssid->passphrase, old_ssid->passphrase_len);
		ssid->passphrase[old_ssid->passphrase_len] = '\0';
	}
	head = tail = ssid;
	ssid->id = 0;
	ssid->disabled = 1;
	ssid->next = NULL;
	ssid->pnext = NULL;
	ssid->pt = NULL;
	if (wpa_config_add_prio_network(config, ssid)) {
		wpa_config_free_ssid(ssid);
		wpa_config_free(config);
		return NULL;
	}
	config->ssid = head;
	return config;
}

