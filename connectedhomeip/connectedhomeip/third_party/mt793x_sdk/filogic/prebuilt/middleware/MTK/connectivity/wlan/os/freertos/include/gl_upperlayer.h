/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
/*
** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/os/linux/include/gl_cfg80211.h#1
*/

/*! \file   gl_cfg80211.h
*    \brief  This file is for Portable Driver linux cfg80211 support.
*/


#ifndef _GL_CFG80211_H
#define _GL_CFG80211_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#if (!CFG_SUPPORT_NO_SUPPLICANT_OPS) || (!CFG_SUPPORT_NO_SUPPLICANT_OPS_P2P)
#include "drivers/driver.h"
#include "common/defs.h"
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#if !CFG_SUPPORT_NO_SUPPLICANT_OPS
#include "driver_inband.h"
#endif

#include "lwip/pbuf.h"
#include "lwip/netif.h"
/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* code without supplicant */
#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */
#define ETH_FCS_LEN	4		/* Octets in the FCS		 */
#define IEEE80211_MAX_SSID_LEN		32

extern const uint8_t bcast_addr[ETH_ALEN];


/* needed by mgmt/rlm_domain.c */
#define regulatory_hint(_wiphy, _alpha2) \
	KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__)
/*
 * needed by
 * mgmt/rlm_domain.c
 * nic/nic_cmd_event.c
 */
#define priv_to_wiphy(_priv) \
	((void *) KAL_NEED_IMPLEMENT(__FILE__, __func__, __LINE__, _priv))

/**
 * enum nl80211_band - Frequency band
 * @NL80211_BAND_2GHZ: 2.4 GHz ISM band
 * @NL80211_BAND_5GHZ: around 5 GHz band (4.9 - 5.7 GHz)
 * @NL80211_BAND_60GHZ: around 60 GHz band (58.32 - 64.80 GHz)
 **/
enum nl80211_band {
	NL80211_BAND_2GHZ,
	NL80211_BAND_5GHZ,
	NL80211_BAND_60GHZ,
	NUM_NL80211_BANDS,
};

/* fix redefined */
#if CFG_SUPPORT_NO_SUPPLICANT_OPS
enum nl80211_auth_type {
	NL80211_AUTHTYPE_OPEN_SYSTEM,
	NL80211_AUTHTYPE_SHARED_KEY,
	NL80211_AUTHTYPE_FT,
	NL80211_AUTHTYPE_NETWORK_EAP,
	NL80211_AUTHTYPE_SAE,
	/* keep last */
	__NL80211_AUTHTYPE_NUM,
	NL80211_AUTHTYPE_MAX = __NL80211_AUTHTYPE_NUM - 1,
	NL80211_AUTHTYPE_AUTOMATIC
};
#endif
enum ieee80211_band {
	IEEE80211_BAND_2GHZ = NL80211_BAND_2GHZ,
	IEEE80211_BAND_5GHZ = NL80211_BAND_5GHZ,
	IEEE80211_BAND_60GHZ = NL80211_BAND_60GHZ,
	/* keep last */
	IEEE80211_NUM_BANDS
};

struct ieee80211_channel {
	enum ieee80211_band band;
	uint16_t center_freq;
	uint16_t hw_value;
	uint32_t flags;
	int max_antenna_gain;
	int max_power;
	/* int max_reg_power; */
	/* bool beacon_found; */
	uint32_t orig_flags;
	/* int orig_mag, orig_mpwr; */
	/* enum nl80211_dfs_state dfs_state; */
	/* unsigned long dfs_state_entered; */
	/* unsigned int dfs_cac_ms; */
};

/* needed by mgmt/rlm_domain.c */
struct ieee80211_supported_band {
	struct ieee80211_channel *channels;
	uint32_t n_channels;
};

/* needed by
 * mgmt/rlm_domain.c
 * nic/nic_cmd_event.c
 */
struct wiphy {
	struct ieee80211_supported_band *bands[NUM_NL80211_BANDS];
};

/* needed by nic/nic_cmd_event.c */
struct wireless_dev {
	struct wiphy *wiphy;
};

struct cfg80211_connect_params {
	struct ieee80211_channel *channel;
	/* struct ieee80211_channel *channel_hint; */
	char bssid[ETH_ALEN];
	/* const uint8_t *bssid_hint; */
	/* const uint8_t ssid[IEEE80211_MAX_SSID_LEN]; */
	char ssid[IEEE80211_MAX_SSID_LEN];
	uint32_t ssid_len;
	enum nl80211_auth_type auth_type;
	/* const uint8_t *ie; */
	/* uint32_t ie_len; */
	bool privacy;
	/* enum nl80211_mfp mfp; */
	/* struct cfg80211_crypto_settings crypto; */
	/* const uint8_t *key; */
	/* uint8_t key_len, key_idx; */
	/* uint32_t flags; */
	/* int bg_scan_period; */
	/* struct ieee80211_ht_cap ht_capa; */
	/* struct ieee80211_ht_cap ht_capa_mask; */
	/* struct ieee80211_vht_cap vht_capa; */
	/* struct ieee80211_vht_cap vht_capa_mask; */
};
/* code without supplicant */

#ifdef CONFIG_NL80211_TESTMODE
#define NL80211_DRIVER_TESTMODE_VERSION 2
#endif

#define WPAS_MAX_SCAN_SSIDS 16

extern const uint8_t bcast_addr[ETH_ALEN];
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

#ifdef CONFIG_NL80211_TESTMODE
#if CFG_SUPPORT_NFC_BEAM_PLUS

typedef struct _NL80211_DRIVER_SET_NFC_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	uint32_t NFC_Enable;

} NL80211_DRIVER_SET_NFC_PARAMS, *P_NL80211_DRIVER_SET_NFC_PARAMS;

#endif


typedef struct _NL80211_DRIVER_GET_STA_STATISTICS_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	uint32_t u4Version;
	uint32_t u4Flag;
	uint8_t aucMacAddr[MAC_ADDR_LEN];
} NL80211_DRIVER_GET_STA_STATISTICS_PARAMS, *P_NL80211_DRIVER_GET_STA_STATISTICS_PARAMS;

typedef enum _ENUM_TESTMODE_STA_STATISTICS_ATTR {
	NL80211_TESTMODE_STA_STATISTICS_INVALID = 0,
	NL80211_TESTMODE_STA_STATISTICS_VERSION,
	NL80211_TESTMODE_STA_STATISTICS_MAC,
	NL80211_TESTMODE_STA_STATISTICS_LINK_SCORE,
	NL80211_TESTMODE_STA_STATISTICS_FLAG,

	NL80211_TESTMODE_STA_STATISTICS_PER,
	NL80211_TESTMODE_STA_STATISTICS_RSSI,
	NL80211_TESTMODE_STA_STATISTICS_PHY_MODE,
	NL80211_TESTMODE_STA_STATISTICS_TX_RATE,

	NL80211_TESTMODE_STA_STATISTICS_TOTAL_CNT,
	NL80211_TESTMODE_STA_STATISTICS_THRESHOLD_CNT,
	NL80211_TESTMODE_STA_STATISTICS_AVG_PROCESS_TIME,

	NL80211_TESTMODE_STA_STATISTICS_FAIL_CNT,
	NL80211_TESTMODE_STA_STATISTICS_TIMEOUT_CNT,
	NL80211_TESTMODE_STA_STATISTICS_AVG_AIR_TIME,

	NL80211_TESTMODE_STA_STATISTICS_TC_EMPTY_CNT_ARRAY,
	NL80211_TESTMODE_STA_STATISTICS_TC_QUE_LEN_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_TC_AVG_QUE_LEN_ARRAY,
	NL80211_TESTMODE_STA_STATISTICS_TC_CUR_QUE_LEN_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_RESERVED_ARRAY,

	NL80211_TESTMODE_STA_STATISTICS_NUM
} ENUM_TESTMODE_STA_STATISTICS_ATTR;
#endif
/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
/* cfg80211 hooks */
/* FreeRTOS redine interface with upperlayer state machine */
int mtk_freertos_wpa_get_ssid(uint8_t *ssid, uint32_t *ssid_len);

int mtk_freertos_wpa_get_bssid(uint8_t *bssid);

int mtk_freertos_connect(uint16_t center_freq, uint8_t *bssid,
				uint8_t *ssid, uint32_t ssid_len);

int mtk_freertos_scan(uint16_t *ch_list, int num_ch);

/*For mini supplicant*/
#if (!CFG_SUPPORT_NO_SUPPLICANT_OPS) || (!CFG_SUPPORT_NO_SUPPLICANT_OPS_P2P)
extern SemaphoreHandle_t g_wait_drv_ready;
#endif

#if (!CFG_SUPPORT_NO_SUPPLICANT_OPS)
int mtk_freertos_wpa_get_channel_list_full(
				uint8_t ucSpecificBand,
				uint8_t ucMaxChannelNum,
				uint8_t ucNoDfs,
				uint8_t *pucNumOfChannel,
				uint8_t *paucChannelList);

int mtk_freertos_wpa_set_favor_ssid(void *priv,
	uint8_t *ssid, uint8_t len);

int mtk_freertos_wpa_scan(void *priv,
				struct wpa_driver_scan_params *req);

struct wpa_scan_results *
mtk_freertos_wpa_scan_results(void *priv, int *num);

int mtk_freertos_wpa_associate(void *priv,
				struct wpa_driver_associate_params *params);

int mtk_freertos_wpa_set_power(uint8_t listen_int_present, uint8_t listen_int,
				uint8_t ps_mode_present, uint8_t ps_mode);

int
mtk_freertos_wpa_disconnect(void *priv, const uint8_t *addr, int reason_code);

int
mtk_freertos_mgmt_tx(void *priv, int freq, const uint8_t *buf,
		     uint32_t len, uint64_t *cookie, int no_cck, int offchan);

int
mtk_freertos_wpa_set_key(const char *ifname, void *priv,
					enum wpa_alg alg, const uint8_t *addr,
					int key_idx, int set_tx,
					const uint8_t *seq, uint32_t seq_len,
					const uint8_t *key, uint32_t key_len);

int
mtk_freertos_wpa_set_rekey_data(void *priv,
					const uint8_t *kek, size_t kek_len,
					const uint8_t *kck, size_t kck_len,
					const uint8_t *replay_ctr);

int mtk_freertos_update_ft_ies(uint16_t mdid, const uint8_t *ies,
			       size_t ies_len);

int mtk_freertos_abort_scan(void);

int mtk_freertos_wpa_external_auth(struct external_auth *params);

int mtk_freertos_wpa_send_mlme(void *priv, const u8 *buf, size_t len);

int mtk_freertos_wpa_add_pmkid(void *priv, const u8 *bssid, const u8 *pmkid);

int mtk_freertos_wpa_remove_pmkid(void *priv, const u8 *bssid, const u8 *pmkid);

int mtk_freertos_wpa_flush_pmkid(void *priv);

#if !CFG_SUPPORT_NO_SUPPLICANT_OPS_P2P /*For mini supplicant*/

int mtk_p2p_freertos_wpa_get_channel_list(
					uint8_t ucSpecificBand,
					uint8_t ucMaxChannelNum,
					uint8_t *pucNumOfChannel,
					uint8_t *paucChannelList);

int mtk_p2p_freertos_wpa_start_ap(void *priv,
			struct wpa_driver_ap_params *settings);

int mtk_p2p_freertos_wpa_do_acs(void *priv,
					 struct drv_acs_params *params);

int mtk_p2p_freertos_wpa_stop_ap(VOID);

int mtk_p2p_freertos_wpa_del_station(void *priv,
							const uint8_t *mac);

int mtk_p2p_freertos_wpa_set_ap(void *priv,
					 struct wpa_driver_ap_params *params);

int mtk_p2p_freertos_wpa_change_beacon(void *priv,
					 struct wpa_driver_ap_params *info);

int mtk_p2p_freertos_wpa_mgmt_tx(void *priv,
				int freq, unsigned int wait_time,
				const void *data, uint32_t len, uint64_t *cookie,
				int no_cck, int no_ack, int offchan);

void mtk_p2p_freertos_wpa_add_iface(void);

void mtk_p2p_freertos_wpa_change_iface(void);

void mtk_p2p_freertos_wpa_del_iface(void);

#endif
#endif
/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

const uint8_t *mtk_cfg80211_find_ie_match_mask(uint8_t eid,
				const uint8_t *ies, int len,
				const uint8_t *match,
				int match_len, int match_offset,
				const uint8_t *match_mask);

err_t mtk_freertos_wlan_send(struct netif *netif, struct pbuf *p);
void *mtk_freertos_wpa_global_init(void *ctx);

void mtk_freertos_wpa_global_deinit(void *priv);

#if CFG_SUPPORT_SUPPLICANT_MBO
u_int8_t wextSrchDesiredSupOpClassIE(IN uint8_t *pucIEStart,
			IN int32_t i4TotalIeLen, OUT uint8_t **ppucDesiredIE);

u_int8_t wextSrchDesiredMboIE(IN uint8_t *pucIEStart,
			IN int32_t i4TotalIeLen, OUT uint8_t **ppucDesiredIE);
#endif /* CFG_SUPPORT_SUPPLICANT_MBO */
#endif /* _GL_CFG80211_H */
