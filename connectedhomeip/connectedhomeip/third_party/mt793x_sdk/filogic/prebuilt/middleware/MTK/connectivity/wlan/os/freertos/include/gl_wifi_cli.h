/*******************************************************************************
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
 ******************************************************************************/
/*
 * Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/os/linux/include
       /gl_wext_priv.h#3
 */

/*! \file   gl_wext_priv.h
 *    \brief  This file includes private ioctl support.
 */


#ifndef _GL_WEXT_PRIV_H
#define _GL_WEXT_PRIV_H
/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */
#include "gl_os.h"

#include "cli.h"
#if (CONFIG_WIFI_TEST_TOOL == 1)
#include "wifi_event_gen4m.h"
#endif
#include "wifi_api_ex.h"
/*******************************************************************************
 *			E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */
#ifdef MTK_SLT_ENABLE
extern int32_t g_i4SltConnsysWifiOn;
#endif

extern char *RATE_V2_HW_TX_MODE_STR[];

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */
#if 1
/* Define magic key of test mode (Don't change it for future compatibity) */
#define PRIV_CMD_TEST_MAGIC_KEY                         2011
#define PRIV_CMD_TEST_MAGIC_KEY_ICAP                         2013

#if (CONFIG_WIFI_TEST_TOOL == 1)
#define OID_CUSTOM_MCR_RW                               0xFFA0C801
#define OID_CUSTOM_TEST_MODE                            0xFFA0C901
#define OID_CUSTOM_ABORT_TEST_MODE                      0xFFA0C906
#define OID_CUSTOM_MTK_WIFI_TEST                        0xFFA0C911
#define PRIV_CMD_OID                    15
#endif

#if defined(MTK_MINICLI_ENABLE)
#define WIFI_IP_ADDRESS_LENGTH 4

#define WIFI_IP_BUFFER_LENGTH (16)

/**@brief The common buffer in a profile.
*/
#define WIFI_PROFILE_BUFFER_NETWORK "network"
#endif

#if CFG_SUPPORT_ROAMING_CUSTOMIZED
#define CMD_ROAM_PREFIX_LEN 9
#define CMD_ROAM_MAIN_LEN (CHIP_CONFIG_RESP_SIZE - CMD_PREFIX_LEN)

#define CMD_ROAM_BY_RSSI            "RoamingEnable"
#define CMD_ROAM_RSSI_THRESHOLD     "RoamingRCPIValue"
#define CMD_ROAM_BY_BCN_MISS        "RoamingByBcn"
#define CMD_ROAM_BY_BCN_MISS_THR    "ScreenOnBeaconTimeoutCount"
#define CMD_ROAM_BLOCK_TIME         "RoamingBlockTimeSec"
#define CMD_ROAM_LOCK_TIME          "RoamingRetryTimeSec"
#define CMD_ROAM_MAXLOCK_TIME       "RoamingRetryLimit"
#define CMD_ROAM_BTO_TIME           "RoamingBeaconTimeSec"
#define CMD_ROAM_STABLE_TIME        "RoamingStableTimeSec"

#define CMD_ROAM_SPLIT_SCAN         "scan set SplitScan"

#define ROAM_TYPE_BY_RSSI (0)
#define ROAM_TYPE_BY_BCNMISS (1)
/* BIT2~BIT7 reserved. */
#endif

#else
/* New wireless extensions API - SET/GET convention (even ioctl numbers are
 * root only)
 */
#define IOCTL_SET_INT                   (SIOCIWFIRSTPRIV + 0)
#define IOCTL_GET_INT                   (SIOCIWFIRSTPRIV + 1)

#define IOCTL_SET_ADDRESS               (SIOCIWFIRSTPRIV + 2)
#define IOCTL_GET_ADDRESS               (SIOCIWFIRSTPRIV + 3)
#define IOCTL_SET_STR                   (SIOCIWFIRSTPRIV + 4)
#define IOCTL_GET_STR                   (SIOCIWFIRSTPRIV + 5)
#define IOCTL_SET_KEY                   (SIOCIWFIRSTPRIV + 6)
#define IOCTL_GET_KEY                   (SIOCIWFIRSTPRIV + 7)
#define IOCTL_SET_STRUCT                (SIOCIWFIRSTPRIV + 8)
#define IOCTL_GET_STRUCT                (SIOCIWFIRSTPRIV + 9)
#define IOCTL_SET_STRUCT_FOR_EM         (SIOCIWFIRSTPRIV + 11)
#define IOCTL_SET_INTS                  (SIOCIWFIRSTPRIV + 12)
#define IOCTL_GET_INTS                  (SIOCIWFIRSTPRIV + 13)
#define IOCTL_SET_DRIVER                (SIOCIWFIRSTPRIV + 14)
#define IOCTL_GET_DRIVER                (SIOCIWFIRSTPRIV + 15)

#if CFG_SUPPORT_QA_TOOL
#define IOCTL_QA_TOOL_DAEMON			(SIOCIWFIRSTPRIV + 16)
#define IOCTL_IWPRIV_ATE                (SIOCIWFIRSTPRIV + 17)
#endif

#define IOC_AP_GET_STA_LIST     (SIOCIWFIRSTPRIV+19)
#define IOC_AP_SET_MAC_FLTR     (SIOCIWFIRSTPRIV+21)
#define IOC_AP_SET_CFG          (SIOCIWFIRSTPRIV+23)
#define IOC_AP_STA_DISASSOC     (SIOCIWFIRSTPRIV+25)

#define PRIV_CMD_REG_DOMAIN             0
#define PRIV_CMD_BEACON_PERIOD          1
#define PRIV_CMD_ADHOC_MODE             2

#if CFG_TCP_IP_CHKSUM_OFFLOAD
#define PRIV_CMD_CSUM_OFFLOAD       3
#endif /* CFG_TCP_IP_CHKSUM_OFFLOAD */

#define PRIV_CMD_ROAMING                4
#define PRIV_CMD_VOIP_DELAY             5
#define PRIV_CMD_POWER_MODE             6

#define PRIV_CMD_WMM_PS                 7
#define PRIV_CMD_BT_COEXIST             8
#define PRIV_GPIO2_MODE                 9

#define PRIV_CUSTOM_SET_PTA			10
#define PRIV_CUSTOM_CONTINUOUS_POLL     11
#define PRIV_CUSTOM_SINGLE_ANTENNA		12
#define PRIV_CUSTOM_BWCS_CMD			13
#define PRIV_CUSTOM_DISABLE_BEACON_DETECTION	14	/* later */
#define PRIV_CMD_OID                    15
#define PRIV_SEC_MSG_OID                16

#define PRIV_CMD_TEST_MODE              17
#define PRIV_CMD_TEST_CMD               18
#define PRIV_CMD_ACCESS_MCR             19
#define PRIV_CMD_SW_CTRL                20

#if 1				/* ANTI_PRIVCY */
#define PRIV_SEC_CHECK_OID              21
#endif

#define PRIV_CMD_WSC_PROBE_REQ          22

#define PRIV_CMD_P2P_VERSION                   23

#define PRIV_CMD_GET_CH_LIST            24

#define PRIV_CMD_SET_TX_POWER_NO_USED           25

#define PRIV_CMD_BAND_CONFIG            26

#define PRIV_CMD_DUMP_MEM               27

#define PRIV_CMD_P2P_MODE               28

#if CFG_SUPPORT_QA_TOOL
#define PRIV_QACMD_SET					29
#endif

#define PRIV_CMD_MET_PROFILING          33

#if CFG_WOW_SUPPORT
#define PRIV_CMD_SET_WOW_ENABLE			34
#define PRIV_CMD_SET_WOW_PAR			35
#endif

#ifdef UT_TEST_MODE
#define PRIV_CMD_UT		36
#endif /* UT_TEST_MODE */

#define PRIV_CMD_SET_SER                37

/* dynamic tx power control */
#define PRIV_CMD_SET_PWR_CTRL		40

/* wifi type: 11g, 11n, ... */
#define  PRIV_CMD_GET_WIFI_TYPE		41

/* 802.3 Objects (Ethernet) */
#define OID_802_3_CURRENT_ADDRESS           0x01010102

/* IEEE 802.11 OIDs */
#define OID_802_11_SUPPORTED_RATES              0x0D01020E
#define OID_802_11_CONFIGURATION                0x0D010211

/* PnP and PM OIDs, NDIS default OIDS */
#define OID_PNP_SET_POWER                               0xFD010101

#define OID_CUSTOM_OID_INTERFACE_VERSION                0xFFA0C000

/* MT5921 specific OIDs */
#define OID_CUSTOM_BT_COEXIST_CTRL                      0xFFA0C580
#define OID_CUSTOM_POWER_MANAGEMENT_PROFILE             0xFFA0C581
#define OID_CUSTOM_PATTERN_CONFIG                       0xFFA0C582
#define OID_CUSTOM_BG_SSID_SEARCH_CONFIG                0xFFA0C583
#define OID_CUSTOM_VOIP_SETUP                           0xFFA0C584
#define OID_CUSTOM_ADD_TS                               0xFFA0C585
#define OID_CUSTOM_DEL_TS                               0xFFA0C586
#define OID_CUSTOM_SLT                               0xFFA0C587
#define OID_CUSTOM_ROAMING_EN                           0xFFA0C588
#define OID_CUSTOM_WMM_PS_TEST                          0xFFA0C589
#define OID_CUSTOM_COUNTRY_STRING                       0xFFA0C58A
#define OID_CUSTOM_MULTI_DOMAIN_CAPABILITY              0xFFA0C58B
#define OID_CUSTOM_GPIO2_MODE                           0xFFA0C58C
#define OID_CUSTOM_CONTINUOUS_POLL                      0xFFA0C58D
#define OID_CUSTOM_DISABLE_BEACON_DETECTION             0xFFA0C58E

/* CR1460, WPS privacy bit check disable */
#define OID_CUSTOM_DISABLE_PRIVACY_CHECK                0xFFA0C600

/* Precedent OIDs */
#define OID_CUSTOM_MCR_RW                               0xFFA0C801
#define OID_CUSTOM_EEPROM_RW                            0xFFA0C803
#define OID_CUSTOM_SW_CTRL                              0xFFA0C805
#define OID_CUSTOM_MEM_DUMP                             0xFFA0C807

/* RF Test specific OIDs */
#define OID_CUSTOM_TEST_MODE                            0xFFA0C901
#define OID_CUSTOM_TEST_RX_STATUS                       0xFFA0C903
#define OID_CUSTOM_TEST_TX_STATUS                       0xFFA0C905
#define OID_CUSTOM_ABORT_TEST_MODE                      0xFFA0C906
#define OID_CUSTOM_MTK_WIFI_TEST                        0xFFA0C911
#define OID_CUSTOM_TEST_ICAP_MODE                       0xFFA0C913

/* BWCS */
#define OID_CUSTOM_BWCS_CMD                             0xFFA0C931
#define OID_CUSTOM_SINGLE_ANTENNA                       0xFFA0C932
#define OID_CUSTOM_SET_PTA                              0xFFA0C933

/* NVRAM */
#define OID_CUSTOM_MTK_NVRAM_RW                         0xFFA0C941
#define OID_CUSTOM_CFG_SRC_TYPE                         0xFFA0C942
#define OID_CUSTOM_EEPROM_TYPE                          0xFFA0C943

#if CFG_SUPPORT_WAPI
#define OID_802_11_WAPI_MODE                            0xFFA0CA00
#define OID_802_11_WAPI_ASSOC_INFO                      0xFFA0CA01
#define OID_802_11_SET_WAPI_KEY                         0xFFA0CA02
#endif

#if CFG_SUPPORT_WPS2
#define OID_802_11_WSC_ASSOC_INFO                       0xFFA0CB00
#endif

#if CFG_SUPPORT_LOWLATENCY_MODE
#define OID_CUSTOM_LOWLATENCY_MODE			0xFFA0CC00
#endif /* CFG_SUPPORT_LOWLATENCY_MODE */

#define OID_IPC_WIFI_LOG_UI                             0xFFA0CC01
#define OID_IPC_WIFI_LOG_LEVEL                          0xFFA0CC02

#if CFG_SUPPORT_ANT_SWAP
#define OID_CUSTOM_QUERY_ANT_SWAP_CAPABILITY		0xFFA0CD00
#endif

#if CFG_SUPPORT_NCHO
#define CMD_NCHO_COMP_TIMEOUT			1500	/* ms */
#define CMD_NCHO_AF_DATA_LENGTH			1040
#endif

#ifdef UT_TEST_MODE
#define OID_UT                                          0xFFA0CD00
#endif /* UT_TEST_MODE */

#endif
#define PRIV_CLI_CMD_TIMEOUT			3000	/* ms */
/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */
#if CFG_SUPPORT_ADVANCE_CONTROL
enum {
	CMD_ADVCTL_NOISE_ID = 1,
	CMD_ADVCTL_POP_ID,
	CMD_ADVCTL_ED_ID,
	CMD_ADVCTL_PD_ID,
	CMD_ADVCTL_MAX_RFGAIN_ID,
	CMD_ADVCTL_ADM_CTRL_ID,
	CMD_ADVCTL_BCN_TH_ID = 9,
	CMD_ADVCTL_DEWEIGHTING_TH_ID,
	CMD_ADVCTL_DEWEIGHTING_NOISE_ID,
	CMD_ADVCTL_DEWEIGHTING_WEIGHT_ID,
	CMD_ADVCTL_ACT_INTV_ID,
	CMD_ADVCTL_1RPD,
	CMD_ADVCTL_MMPS,
	CMD_ADVCTL_RXC_ID = 17,
	CMD_ADVCTL_SNR_ID = 18,
	CMD_ADVCTL_BCNTIMOUT_NUM_ID = 19,
	CMD_ADVCTL_EVERY_TBTT_ID = 20,
	CMD_ADVCTL_MAX
};
#endif /* CFG_SUPPORT_ADVANCE_CONTROL */

struct wifi_cfg {
	uint8_t opmode;
	uint8_t country_code[4];

	uint8_t sta_ssid[32];
	uint8_t sta_ssid_len;
	uint8_t sta_wpa_psk[64];
	uint8_t sta_wpa_psk_len;
	uint8_t sta_default_key_id;
	uint8_t sta_bandwidth;
	uint8_t sta_wireless_mode;
	uint8_t sta_listen_interval;
	uint8_t sta_power_save_mode;
	uint8_t sta_channel;

	uint8_t ap_ssid[32];
	uint8_t ap_ssid_len;
	uint8_t ap_channel;
	uint8_t ap_bw;
	uint8_t ap_hide_ssid;
	uint8_t ap_auth_mode;
	uint8_t ap_encryp_type;
	uint8_t ap_wpa_psk[64];
	uint8_t ap_wpa_psk_len;
	uint8_t ap_default_key_id;
	uint8_t ap_wireless_mode;
	uint8_t ap_dtim_interval;
};

/*******************************************************************************
 *			P U B L I C   D A T A
 *******************************************************************************
 */

extern uint32_t gu4DbgLevl;
#ifdef MTK_WF_DRV_CLI_ENABLE
extern const cmd_t wifi_init_cli[];
#ifdef MTK_WF_IWPRIV_CLI_ENABLE
#undef WF_DRV_CLI_ENTRY
#define WF_DRV_CLI_ENTRY \
	{ "iwpriv", "WiFi iw command", iwpriv_cli, NULL }, \
	{ "wifi", "WiFi Init CLI", NULL, (cmd_t *)wifi_init_cli},
#else
#define WF_DRV_CLI_ENTRY \
	{ "wifi", "WiFi Init CLI", NULL, (cmd_t *)wifi_init_cli},
#endif
#endif

extern struct netif *wlan_sta_netif;
extern struct netif *wlan_ap_netif;
extern wifi_scan_list_item_t g_ap_list[CFG_MAX_NUM_BSS_LIST];

/*******************************************************************************
 *			P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *			M A C R O S
 *******************************************************************************
 */

/*******************************************************************************
 *			F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */
uint8_t iwpriv_cli(uint8_t len, char *param[]);

int32_t priv_driver_cmds(struct GLUE_INFO *prGlueInfo,
			 char *pcCommand, int32_t i4TotalLen);

#if (CONFIG_WIFI_TEST_TOOL == 1)
int
priv_set_struct(struct iwreq *pwrq, IN char *pcExtra);

int
priv_get_struct(struct iwreq *pwrq, IN char *pcExtra);
#endif

uint8_t _wsys_on(uint8_t len, char *param[]);

uint8_t _wsys_off(uint8_t len, char *param[]);

uint8_t _wifi_on(uint8_t len, char *param[]);

uint8_t _wifi_off(uint8_t len, char *param[]);

uint8_t _wifi_info(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Operation Mode.
* @param [OUT]mode
*        1: WIFI_MODE_STA_ONLY,
*        2: WIFI_MODE_AP_ONLY,
*        3: WIFI_MODE_REPEATER
*        4: WIFI_MODE_MONITOR
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get opmode
*  cli_output:    1: STA  mode;  2: AP mode;
*                          3: Repeater mode 4: Monitor mode
*
*  cli_example:   wifi config get opmode
*  cli_note:      Get the current operation mode.
*
*/
uint8_t wifi_config_get_opmode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Operation Mode.
* @param [IN]mode
*       1: WIFI_MODE_STA_ONLY,
*       2: WIFI_MODE_AP_ONLY,
*       3: WIFI_MODE_REPEATER
*       4: WIFI_MODE_MONITOR
* @return  >=0 means success, <0 means fail
* @note Set WiFi Operation Mode will RESET all the configuration set by
* previous WIFI-CONFIG APIs
*
* Example of cli usage
*  cli_format:    wifi config set opmode <mode>
*  cli_param:     mode:  1: STA mode;  2: AP mode;
*                                 3: Repeater mode 4: Monitor mode
*
*  cli_example:   wifi config set opmode 1
*  cli_note:      Set the device's current operation mode as STA mode.
*
*/
uint8_t wifi_config_set_opmode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get MAC address for STA/AP wireless port
* @param [IN]port  0: STA/APCLI, 1: AP
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get mac <port>
*  cli_param:     port: 0: STA/APCLI, 1: AP
*
*  cli_example:   wifi config get mac 0
*  cli_note:      Get the device's STA/APCLI port's mac address,
* @note                    That there is no corresponding set mac cli because
* the MAC address cannot be modified dynamically.
*
*/
uint8_t wifi_config_get_mac_address_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
* @param [IN]port  0: STA/APCLI, 1: AP
* @param [IN]bw   0: HT20, 1: HT20/40 coexistence
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set bw <port> <bandwidth>
*  cli_param:     port: 0: STA/APCLI, 1: AP
*                       bandwidth: Bandwidth supported, 0: 20MHz,
*                                                          1: 20/40MHz
*
*  cli_example:   wifi config set bw 0 0
*  cli_note:      Set the device STA/APCLI port's bandwidth as 20MHz.
* The bandwidth needs to be limited and match the wireless mode settings.
*
*/
uint8_t wifi_config_set_bandwidth_ex(uint8_t len, char *param[]);

/**
* @brief Example of get bandwidth for STA/AP wireless port.
* @param [IN]port  0: STA/APCLI, 1: AP
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get bw <port>
*  cli_param:     port: 0: STA/APCLI, 1: AP
*  cli_output:    The current bandwidth
*
*  cli_example:   wifi config get bw 0
*  cli_note:      Get the device STA/APCLI port's current bandwidth
*
*/
uint8_t wifi_config_get_bandwidth_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
*
* wifi config set bw_extended <port>  <0:HT20/1:HT40> <below_above_ch>
* @parameter
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format: wifi config set bw_extended <port> <bandwidth> <below_above_ch>
*  cli_param:  port: 0: STA/APCLI, 1: AP
*                       bandwidth: Bandwidth supported, 0: 20MHz, 1: 20/40MHz
*                       below_above_ch:
*
*  cli_example:
*  cli_note:
*
*/
uint8_t wifi_config_set_bandwidth_extended_ex(uint8_t len, char *param[]);

/**
* @brief Example of get bandwidth for STA/AP wireless port.
*  wifi config get bw_extended <port>
* @parameter
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get bw_extended <port>
*  cli_param:     port: 0: STA/APCLI, 1: AP
*  cli_output:    The current externed bandwidth
*
*  cli_example:   wifi config get bw_extended 0
*  cli_note:      Get the device STA/APCLI port's current externed bandwidth
*
*/
uint8_t wifi_config_get_bandwidth_extended_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure channel for STA/AP wireless port. STA will keep
* idle and stay in channel specified
* @param [IN]port  0: STA/APCLI, 1: AP
* @param [IN]ch  1~14 are supported for 2.4G only product, channel 36~165 for
* 5G only
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set ch <port> <ch>
*  cli_param:     port: 0: STA/APCLI, 1: AP
*                       ch: 2.4G: channel 1~14, 5G: channel 36~165. please
*                       refer to #wifi_config_set_country_region
*
*  cli_example:   wifi config set ch 0 1
*  cli_note:  Set the device STA/APCLI port's current channel as 1. The
*             channel needs to be limited and match the country region/country
*             code/wireless mode setting.
*
*/
uint8_t wifi_config_set_channel_ex(uint8_t len, char *param[]);

/**
* @brief Example of get the current channel for STA/AP wireless port.
* @param [IN]port  0: STA/APCLI, 1: AP
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get ch <port>
*  cli_param:     port: 0: STA/APCLI, 1: AP
*  cli_output:    The current channel, please refer to
*                 #wifi_config_set_country_region
*
*  cli_example:   wifi config get ch 0
*  cli_note:      Get the device STA/APCLI port's current channel.
*
*/
uint8_t wifi_config_get_channel_ex(uint8_t len, char *param[]);

/**
* @brief Example of Configure channel for STA wireless port. STA will scan
*        the input channels
* wifi config set ch <port> <amount_of_channel> <ch A> <ch B>...
* @parameter
*     [IN] channel  1~14 are supported for 2.4G only product
* @return >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_multi_channel_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_chbw_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_ieee80211w_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_proto_ex(uint8_t len, char *param[]);

/**
* @brief Example of configure rx filter for packets wanted to be received
* @param [IN]flag defined in  #wifi_rx_filter_t
* @return >=0 means success, <0 means fail
* @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
*
* Example of cli usage
*  cli_format:    wifi config set rxfilter <flag>
*  cli_param:     flag: Configure packet format which to be receivedm, flag
*                 defined in #wifi_rx_filter_t of wifi api reference
*
*  cli_example:   wifi config set rxfilter 0x1de00a
*  cli_note:      Set the filter to drop specific packets.\n bit 1 Drops the
*                 FCS error frames.\n bit 3 Drops the version field of Frame
*                 Control field.\n  More @sa Please refer #wifi_rx_filter_t.
*
*/
uint8_t wifi_config_set_rx_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of get rx filter for packets format wanted to be received
* @param [OUT]flag defined in  #wifi_rx_filter_t
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get rxfilter
*  cli_output:    Flag defined in  #wifi_rx_filter_t of wifi api reference
*
*  cli_example:   wifi config get rxfilter
*  cli_note:      Get current rx filter settings.
*
*/
uint8_t wifi_config_get_rx_filter_ex(uint8_t len, char *param[]);

/**
* @brief Example of set the authentication mode and encryption mode for the
*        specified STA/AP port
* @param [IN]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [IN] encryption method index:
*           1 AES
*           2 TKIP
*           3 TKIPAES
*
* @return =0 means success, >0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set sec <port> <auth_mode> <encrypt_type>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                       auth_mode:
*                       encrypt_type:
*                       Detail authentication mode and encryption mode info
*                       please refer to section table  Combinations of Auth
*                       Mode and Encrypt Type. @sa Refer to #wifi_auth_mode_t,
*                       #wifi_encrypt_type_t
*
*  cli_example 1: wifi config set sec 1 0 1
*  cli_note:      Set the device AP port's security as Open mode.
*  cli_example 2: wifi config set sec 0 7 6
*  cli_note:      Set the device STA/APCLI port's security setting as WPA2-PSK
*         AES mode. The device must connect the AP with same security setting.
*
*/
uint8_t wifi_config_set_security_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of set multiple authentication modes and
*        encryption mode for the specified STA/AP port
* @param [IN]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [IN] encryption method index:
*           1 AES
*           2 TKIP
*           3 TKIPAES
*
* @return =0 means success, >0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set sec <port> <auth_mode> <encrypt_type>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                       auth_mode:
*                       encrypt_type:
*                       Detail authentication mode and encryption mode info
*                       please refer to section table  Combinations of Auth
*                       Mode and Encrypt Type. @sa Refer to #wifi_auth_mode_t,
*                       #wifi_encrypt_type_t
*
*  cli_example 1: wifi config set sec 1 0 1
*  cli_note:      Set the device AP port's security as Open mode.
*  cli_example 2: wifi config set sec 0 7 6
*  cli_note:      Set the device STA/APCLI port's security setting as WPA2-PSK
*         AES mode. The device must connect the AP with same security setting.
*
*/
uint8_t wifi_config_set_multi_security_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of get the authentication mode for the specified STA/AP port
* @param [OUT]authmode method index:
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param [OUT] encryption method index:
*           1 AES
*           2 TKIP
*           3 TKIPAES
*
* @return =0 means success, >0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get sec <port>
*  cli_param:     port: 0: STA/APCLI  1: AP
*  cli_output:    Get the authentication mode and encryption mode. Detail
*                 authentication mode and encryption mode info please refer
*                 to section table  Combinations of Auth Mode and Encrypt Type
*                 @sa Refer to #wifi_auth_mode_t ,  #wifi_encrypt_type_t
*
*  cli_example:   wifi config get sec 0
*  cli_note:      Get the device STA/APCLI's current security setting.
*
*/
uint8_t wifi_config_get_security_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of set the psk for the specified STA/AP port
* @param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
*
* @return =0 means success, >0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set psk <port> <password>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                       password: 8 ~ 63 bytes ASCII or 64 bytes Hex
*
*  cli_example:   wifi config set psk 0 12345678
*  cli_note:      Set the device STA/APCLI port's password as "12345678", the
*                 device will use this password to connect target AP. If the
*                 password has 64 character, it will be treated as PMK which
*                 is the 64 HEX value.
*
*/
uint8_t wifi_config_set_psk_ex(uint8_t len, char *param[]);

/**
* @deprecated This function is deprecated!It can be replaced by
* #wifi_config_set_psk_ex. Example of Set PMK for the specified STA/AP port
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [IN]PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*
* Example of cli usage
*  cli_format:    wifi config set pmk <port> <PMK>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                       PMK: Hex:  00, 05, 03,...(size 32)
*
*  cli_note:      Set PMK for the specified STA/AP port. It is replaced by psk
*                 API/CLI.
*
*/
uint8_t wifi_config_set_pmk_ex(uint8_t len, char *param[]);

/**
* @brief Example of get the psk for the specified STA/AP port
@param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
*
* @return =0 means success, >0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get psk <port>
*  cli_param:     port: 0: STA/APCLI/AP Client, 1: AP
*  cli_output:    Get the current password.
*
*  cli_example:   wifi config get psk 0
*  cli_note:      Get the device STA/APCLI port's current password.
*
*/
uint8_t wifi_config_get_psk_ex(uint8_t len, char *param[]);


/**
* @deprecated This function is deprecated!It can be replaced by
* #wifi_config_get_psk_ex. Example of Get PMK for the specified STA/AP port
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [OUT]PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*
* Example of cli usage
*  cli_format:    wifi config get pmk <port>
*  cli_param:     port: 0: STA/APCLI/AP Client, 1: AP
*  cli_output     pmk Hex:  00, 05, 03 ,...(size 32)
*
*  cli_example:   wifi config get pmk 0
*  cli_note:      Get the PMK information of the specified STA/AP port.
*                 It is replaced by psk API/CLI.
*
*/
uint8_t wifi_config_get_pmk_ex(uint8_t len, char *param[]);

/**
* @brief Example of get WiFi WEP Keys
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [OUT]wifi_wep_key_t
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get wep <port> <key_id>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                       key_id: 0~3
*  cli_output:    Get the key string of specified key index
*
*  cli_example:   wifi config get wep 0 0
*  cli_note:      Get the key string of the key index 0 for the device's
*                 STA/APCLI port.
*
*/
uint8_t wifi_config_get_wep_key_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi WEP Keys
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [IN]key_id 0~3
* @param [IN]key_string 0~26 (ASCII length: 5 or 13; Hex length:10 or 26)
* @return >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set wep <port> <key_id> <key_string_id>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                       key_id: 0~3
*                       key_string_id: The key string of specified id
*
*  cli_example 1: wifi config set wep 0 0 1234567890
*  cli_note:      Set the device STA/APCLI port's WEP key index 0's value
*                 as "0x12 0x34 0x56 0x78 0x90".
*  cli_example 2: wifi config set wep 0 0 12345
*  cli_note:      Set the device STA/APCLI port's WEP key index 's value
*                 as "0x31 0x32 0x33 0x34 0x35.
*                 The wep key string length should be 5 or 10 or 13 or 26.
*                 Key string with 5 or 13 length be treated as ASCLL, Key
*                 string with 10 or 26 length be treated as Hex.
*
*/
uint8_t wifi_config_set_wep_key_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi SSID.
* @param [IN]port  0: STA/APCLI/AP Client,  1: AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_len Length of SSID
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get ssid <port>
*  cli_param:     port: 0: STA/APCLI
*  cli_output:    The ssid for the specific port
*
*  cli_example 1: wifi config get ssid 0
*  cli_note:      Get the device's STA or APCLI port's  current SSID
*  cli_example 2: wifi config get ssid 1
*  cli_note:      Get current SSID for AP port
*
*/
uint8_t wifi_config_get_ssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi SSID.
* @param [IN]port  0: STA/APCLI / AP Client, 1: AP
* @param [IN]ssid SSID
* @param [IN]ssid_len Length of SSID
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set ssid <port> <ssid>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                       ssid: The ssid of target AP
*
*  cli_example:   wifi config set ssid 0 APRouter
*  cli_note:      Set the device STA/APCLI port's ssid named as "APRouter".
*                 The device will connect the AP which has the same SSID,
*                 when the device is doing wifi connection process.
*
*/
uint8_t wifi_config_set_ssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi BSSID.
* @param [OUT]bssid BSSID
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get bssid
*  output:        Target AP's Wi-Fi BSSID
*
*  cli_example:   wifi config get bssid
*  cli_note:      Get the current Wi-Fi BSSID which the device used to connect
*                 to AP, or the AP's BSSID which the device is connecting.
*
*/
uint8_t wifi_config_get_bssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi SSID.
* @param [IN]bssid BSSID
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set bssid <BSSID>
*  cli_param:     BSSID: target AP's Wi-Fi BSSID
*
*  cli_example:   wifi config set bssid 00:0c:45:56:78:90
*  cli_note:      Set the target AP's BSSID when device is STA mode, the
*                 device will connect the AP which has the same BSSID, when
*                 the device doing wifi connection process.
*
*/
uint8_t wifi_config_set_bssid_ex(uint8_t len, char *param[]);

/**
* @brief Example of Reload configuration
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set reload
*  cli_output:    None.  Reload configuration
*
*  cli_example:   wifi config set reload
*  cli_note:      Reload the wifi configuration, and make it take effect,
*                 such as: ssid, psk, sec, pmk, wep and so on.
*
*/
uint8_t wifi_config_set_reload_ex(uint8_t len, char *param[]);

/**
 * @brief Set Bss Preference when scanning
 *
 * @return >=0 means success, <0 means fail
 *
 * @params [Mode]off=0, prefer_2G=1, prefer_5G=2
 */
uint8_t wifi_config_set_bss_preference_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Radio ON/OFF
* @param [IN]onoff  0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off
*       at the same time
*
* Example of cli usage
*  cli_format:    wifi config set radio <on_off>
*  cli_param:     on_off:  0: OFF;  1: ON
*
*  cli_example:   wifi config set radio 1
*  cli_note:      WIFI interface radio will be turn on.
* @note           In dual mode, both WIFI interface radio will be
*                 turn on/off at the same time.
*
*/
uint8_t wifi_config_set_radio_on_ex(uint8_t len, char *param[]);

/**
* @brief Example of get WiFi Radio ON/OFF
* @param [OUT]onoff 0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off
*       at the same time
*
* Example of cli usage
*  cli_format:    wifi config get radio
*  output         0: OFF;  1: ON
*
*  cli_example:   wifi config get radio
*  cli_note:      Get the current status of WIFI interface radio status.
*
*/
uint8_t wifi_config_get_radio_on_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Wireless Mode
* @param [OUT] wirelessmode, @sa Refer to #wifi_phy_mode_t
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get wirelessmode <port>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*  output         The current wirelessmode ,@sa Refer to #wifi_phy_mode_t
*
*  cli_example:   wifi config get wirelessmode 0
*  cli_note:      Get the device STA/APCLI port's wireless mode.
*
*/
uint8_t wifi_config_get_wireless_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Wireless Mode
* @param [IN]wirelessmode, @sa Refer to #wifi_phy_mode_t
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set wirelessmode <port> <mode>
*  cli_param:     port: 0: STA/APCLI;  1: AP
*                 mode: 0~11  @sa Refer to #wifi_phy_mode_t and table
*                       WiFi Physical Mode Enumeration
*
*  cli_example:   wifi config set wirelessmode 0 0
*  cli_note:      Set the current wirless mode as 11BG mixed mode.
*
*/
uint8_t wifi_config_set_wireless_mode_ex(uint8_t len, char *param[]);

/**
* @deprecated This function is deprecated!It can be replaced by
*         #wifi_config_get_country_code_ex. Example of Get WiFi Country Region
* @param [IN]band  0: 2.4G, 1: 5G
* @param [OUT]region  @sa #wifi_config_set_country_region
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get country <band>
*  cli_param:     band: 0: 2.4G;  1: 5G
*  output:        The country region info. @sa Refer to
*                 #wifi_config_get_country_region
*
*  cli_example:   wifi config get country 0
*  cli_note:      Get 2.4G Band country region value.
*  cli_example:   wifi config get country 1
*  cli_note:      Get 5G Band country region value.
*
*/
uint8_t wifi_config_get_country_region_ex(uint8_t len, char *param[]);

/**
* @deprecated This function is deprecated!It can be replaced by
* #wifi_config_set_country_code_ex. Example of Set WiFi Country Region
* @param [IN]band  0: 2.4G, 1: 5G
* @param [IN]region band=0, region range 0-7,31-33; band=1,
*        region range 0-22, 30-37,  @sa #wifi_config_set_country_region
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set country <band> <region>
*  cli_param:     band: 0: 2.4G;  1: 5G
*                       region: the region value ,Refer to @sa
*                       #wifi_config_set_country_region
*
*  cli_example:   wifi config set country 0 1
*  cli_note:      Set 2.4G band country Region as 1.
*                 Details of country region and channel range of
*                 2.4G band or 5G band, Refer to @sa
*                 #wifi_config_set_country_region.
*
*/
uint8_t wifi_config_set_country_region_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set Country Code
* @param [IN]country_code 2 chars country code
* @return =0 means success, >0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set country_code <country_code>
*  cli_param:     country_code: The country code that the driver uses.
*                 For more information please refer to wifi_country_code_t
*                 in Wi-Fi API reference guide.
*
*  cli_example:   wifi config set country_code CN
*  cli_note:      Only support country code: CN, US, UK for reference design
*
*/
uint8_t wifi_config_set_country_code_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get Country Code
* @param  [OUT]country_code country code and list
* @return =0 means success, >0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get country_code
*  cli_output:    The code region info.
*
*  cli_example:   wifi config get country_code
*  cli_note:      Get the country code information.
*
*/
uint8_t wifi_config_get_country_code_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi DTIM Interval
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get dtim
*  cli_output:    DTIM interval(range is 1~255)
*
*  cli_example:   wifi config get dtim
*  cli_note:      Get device's current wifi DTIM interval value.
*
*/
uint8_t wifi_config_get_dtim_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi DTIM Interval
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set dtim <dtim_interval>
*  cli_param:     dtim_interval: 1~255
*
*  cli_example:   wifi config set dtim 1
*  cli_note:      set device's wifi DTIM interval value as 1, only
*                 used when device is AP mode.
*
*/
uint8_t wifi_config_set_dtim_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi beacon Interval
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set bcn_int <beacon interval>
*  cli_param:     dtim_interval: 1~255
*
*  cli_example:   wifi config set bcn_int 1
*  cli_note:      set device's wifi beacon interval value as 1, only
*                 used when device is AP mode.
*
*/
uint8_t wifi_config_set_bcn_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Listen Interval
* @param  [OUT]interval: 1~255
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config get listen
*  cli_output:    listen interval(range is 1~255).
*
*  cli_example:   wifi config get listen
*  cli_note:      Get device's current wifi listen interval value.
*
*/
uint8_t wifi_config_get_listen_interval_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Listen Interval
* @param [IN]interval 1 ~ 255
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi config set listen <listen_interval>
*  cli_param:     listen_interval: 1~255.
*
*  cli_example:   wifi config set listen 2
*  cli_note:      Set device's wifi listen interval value as 2, only used when
*                 device is STA mode.
*
*/
uint8_t wifi_config_set_listen_interval_ex(uint8_t len, char *param[]);



/* ---------------------- Connect Ex ----------------------------*/




/**
* @brief Example of connect / link up to specified AP
* @param [IN] link  0: link down, 1: link up
* @return >=0 means success, <0 means fail
* @note Be sure to configure security setting before connect to AP
*
* Example of cli usage
*  cli_format:    wifi connect set connection <linkup/down>
*  cli_param:     link: 0: link down (wifi connection disconnect ap).
*                       1: link up (wifi config reload setting).
*
*  cli_example:   wifi connect set connection 0
*  cli_note:      Disconnect to the AP , only used in STA mode.
*
*/
uint8_t wifi_connect_set_connection_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get the current STA port link up / link down
*        status of the connection
* @param [out] link  0: link down, 1: link up
* @return >=0 means success, <0 means fail
* @note WIFI_STATUS_LINK_DISCONNECTED indicates STA may in
*       IDLE/ SCAN/ CONNECTING state
*
* Example of cli usage
*  cli_format:    wifi connect get linkstatus
*  cli_output:    0: disconnected 1: connected.
*
*  cli_example:   wifi connect get linkstatus
*  cli_note:      Return the link status , only for STA mode.
*
*/
uint8_t wifi_connect_get_link_status_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Associated Station List
* @param [OUT]station_list
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi connect get stalist
*  cli_output:    Station list information
*
*  cli_example:   wifi connect get stalist
*  cli_note:      Return the number of associated stations and the stations
*                 information. Only for AP mode.
*
*/
uint8_t wifi_connect_get_station_list_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get the max number of the supported stations in ap mode
*        or Repeater Mode.
* @param [OUT] number The max number of supported stations will be returned
*
* @return  >=0 means success, <0 means fail
*
* Example of cli usage
*  cli_format:    wifi connect get max_num_sta
*  cli_output:    Max station link supported
*
*  cli_example:   wifi connect get max_num_sta
*  cli_note:      Get the maximum number of supported stations in AP mode
*                 or Repeater mode.
*
*/
uint8_t wifi_connect_get_max_station_number_ex(uint8_t len, char *param[]);

/**
* @brief Example of deauth some WiFi connection
* <br><b>
* wifi connect deauth <MAC>
* </b></br>
* @param [IN]addr STA MAC Address
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_deauth_station_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get rssi of the connected AP
* @param [OUT]rssi
* @return >=0 means success, the status will be:
*
* Example of cli usage
*  cli_format:    wifi connect get rssi
*  cli_output:    Get rssi of the connected AP
*
*  cli_example:   wifi connect get rssi
*  cli_note:      Get rssi of the connected AP. Only used for STA mode and the
*                 station has connected to the AP.
*
*/
uint8_t wifi_connect_get_rssi_ex(uint8_t len, char *param[]);

uint8_t wifi_connect_disconnect_ap_ex(uint8_t len, char *param[]);

uint8_t wifi_connect_disconnect_sta_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_ps_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_ps_mode_ex(uint8_t len, char *param[]);

int32_t wifi_config_init(struct wifi_cfg *wifi_config);

uint8_t wifi_init_ex(uint8_t len, char *param[]);

uint8_t wifi_set_csi_ex(uint8_t len, char *param[]);

uint8_t wifi_deinit_ex(uint8_t len, char *param[]);

uint8_t wifi_neighbor_rep_request_ex(uint8_t len, char *param[]);

uint8_t wifi_wnm_bss_query_ex(uint8_t len, char *param[]);

uint8_t wifi_connect_set_scan_ex(uint8_t len, char *param[]);

uint8_t wifi_connect_set_sched_scan_ex(uint8_t len, char *param[]);

/**
* @brief Example of Register WiFi Event Notifier
* wifi connect set eventcb <enable> <event ID>
* @param [IN]evt
* @param evt Event ID, More Event ID @sa #wifi_event_t
* @param [IN]enable 0: register, 1: unregister
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_connect_set_event_callback_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_connected_ap_info_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_statistic_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_single_sku_table_ex(uint8_t len, char *param[]);

uint8_t wifi_config_clear_bcn_lost_cnt_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_bcn_lost_cnt_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_pretbtt_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_pretbtt_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_ps_log_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_ps_log_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_sys_temperature_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_tx_rate_ex(uint8_t len, char *param[]);

#if IP_NAPT
uint8_t wifi_config_set_napt_entry_timer_ex(uint8_t len, char *param[]);
#endif

#if (CFG_SUPPORT_ARP_OFFLOAD_CMD == 1)
uint8_t wifi_config_set_arp_offload_ex(uint8_t len, char *param[]);
#endif

void wifi_conf_get_ip_from_str(uint8_t *ip_dst, const char *ip_src);

uint8_t get_mode_from_nvdm(char *mode);

uint8_t get_ip_from_nvdm(char* type, uint8_t *ip_dst);

uint8_t get_ip_from_nvdm_ap(char* type, uint8_t *ip_dst);

int32_t wifi_config_get_ip_addr(uint8_t port, uint8_t *ip_dst, uint8_t type);

int32_t wifi_config_set_ip_addr(uint8_t port, uint8_t* address, uint8_t type);

uint8_t wifi_config_set_ip_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_ip_addr_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_retry_limit_ex(uint8_t len, char *param[]);

/**
* @brief Example of WiFi TWT ON/OFF
* wifi config set twt <params> <...> ..., len = 2 or 9
* @param [IN] Wake_Dutation
*		 For example, Nominal Minimum TWT Wake Duration = 126
*		 126 means (126 * 256) us = 32.256 ms (322us)
* @param [IN] Wake_Interval
*        For example, TWT Wake Interval Mantissa = 64
*        64 * 2^10 = 64 * 1024 us (65536 us)
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_twt_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_bc_drop_ex(uint8_t len, char *param[]);

/**
* @brief Example of configure rx filter for packets wanted to be received
* wifi config set rxfilter <flag>
* @parameter
*	 [IN]flag defined in  wifi_rx_filter_t
* @return =0 means success, >0 means fail
* @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
*/
uint8_t wifi_config_set_rx_filter_ex(uint8_t len, char *param[]);
uint8_t wifi_config_set_mc_address_ex(uint8_t len, char *param[]);

/**
* @brief Example of get rx filter for packets format wanted to be received
* wifi config get rxfilter
* @parameter
*	 [OUT]flag defined in  wifi_rx_filter_t
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_rx_filter_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_rx_handler_mode_ex(uint8_t len, char *param[]);

/**
* @brief Example of Set WiFi Raw Packet Receiver
* wifi config set rxraw <enable>
* @param [IN]enable 0: unregister, 1: register
*
* @return  =0 means success, <0 means fail
*/
uint8_t wifi_config_set_rx_raw_pkt_ex(uint8_t len, char *param[]);

uint8_t wifi_config_set_wow_ex(uint8_t len, char *param[]);
uint8_t wifi_config_get_wow_ex(uint8_t len, char *param[]);
uint8_t wifi_config_get_wow_reason_ex(uint8_t len, char *param[]);
uint8_t wifi_config_set_wow_udp_ex(uint8_t len, char *param[]);
uint8_t wifi_config_set_wow_udp_del_ex(uint8_t len, char *param[]);
uint8_t wifi_config_get_wow_udp_ex(uint8_t len, char *param[]);
uint8_t wifi_config_set_wow_tcp_ex(uint8_t len, char *param[]);
uint8_t wifi_config_set_wow_tcp_del_ex(uint8_t len, char *param[]);
uint8_t wifi_config_get_wow_tcp_ex(uint8_t len, char *param[]);

#if CFG_SUPPORT_ROAMING_CUSTOMIZED
uint8_t wifi_config_set_roam_delta_ex(uint8_t len, char *param[]);
uint8_t wifi_config_get_roam_delta_ex(uint8_t len, char *param[]);
uint8_t wifi_config_set_roam_scan_channel_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_scan_channel_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_type_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_is_connect_ft_ap_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_statistic_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_clear_roam_statistic_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_roam_by_rssi_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_by_rssi_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_rssithreshold_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_roam_rssithreshold_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_roam_by_bcnmiss_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_by_bcnmiss_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_roam_by_bcnmissthreshold_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_by_bcnmissthreshold_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_roam_block_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_block_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_roam_lock_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_lock_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_roam_maxlock_count_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_roam_maxlock_count_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_bto_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_bto_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_stable_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_split_scan_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_stable_time_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_set_autoroam_ex(uint8_t len,
	char *param[]);
uint8_t wifi_config_get_autoroam_ex(uint8_t len,
	char *param[]);
#endif

uint8_t wifi_config_set_ser_ex(uint8_t len, char *param[]);

#if CFG_SUPPORT_NON_PREF_CHAN
uint8_t wifi_config_set_non_pref_chan_ex(uint8_t len, char *param[]);
#endif

#if CFG_SUPPORT_11KV_SWITCH
uint8_t wifi_config_set_disable_11k_ex(uint8_t len, char *param[]);
uint8_t wifi_config_set_disable_11v_ex(uint8_t len, char *param[]);
uint8_t wifi_config_get_disable_11k_ex(uint8_t len, char *param[]);
uint8_t wifi_config_get_disable_11v_ex(uint8_t len, char *param[]);
#endif

#ifdef MTK_WIFI_PROFILE_ENABLE
uint8_t wifi_profile_set_opmode_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_opmode_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_ssid_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_ssid_ex(uint8_t length, char *param[]);

uint8_t wifi_profile_set_wireless_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_wireless_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_security_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_security_mode_ex(uint8_t length, char *param[]);

uint8_t wifi_profile_set_psk_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_psk_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_wep_key_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_wep_key_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_mac_address_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_mac_address_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_channel_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_channel_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_bandwidth_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_bandwidth_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_dtim_interval_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_dtim_interval_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_listen_interval_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_listen_interval_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_power_save_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_get_power_save_mode_ex(uint8_t len, char *param[]);

#if IP_NAPT
uint8_t wifi_profile_set_napt_tcp_entry_num_ex(uint8_t len, char *param[]);

uint8_t wifi_profile_set_napt_udp_entry_num_ex(uint8_t len, char *param[]);
#endif

#endif

#if CFG_SUPPORT_ANT_DIV
uint8_t wifi_config_set_antdiv_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_antdiv_mode_ex(uint8_t len, char *param[]);

uint8_t wifi_config_get_antdiv_cur_idx_ex(uint8_t len, char *param[]);
#endif

#if 0
int
priv_set_int(IN struct net_device *prNetDev,
	     IN struct iw_request_info *prIwReqInfo,
	     IN union iwreq_data *prIwReqData, IN char *pcExtra);

int
priv_get_int(IN struct net_device *prNetDev,
	     IN struct iw_request_info *prIwReqInfo,
	     IN union iwreq_data *prIwReqData, IN OUT char *pcExtra);

int
priv_set_ints(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo,
	      IN union iwreq_data *prIwReqData, IN char *pcExtra);

int
priv_get_ints(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo,
	      IN union iwreq_data *prIwReqData, IN OUT char *pcExtra);

int
priv_set_struct(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwReqInfo,
		IN union iwreq_data *prIwReqData, IN char *pcExtra);

int
priv_get_struct(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwReqInfo,
		IN union iwreq_data *prIwReqData, IN OUT char *pcExtra);

#if CFG_SUPPORT_NCHO
uint8_t CmdString2HexParse(IN uint8_t *InStr,
			   OUT uint8_t **OutStr, OUT uint8_t *OutLen);
#endif

int
priv_set_driver(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwReqInfo,
		IN union iwreq_data *prIwReqData, IN OUT char *pcExtra);

int
priv_set_ap(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwReqInfo,
		IN union iwreq_data *prIwReqData, IN OUT char *pcExtra);

int priv_support_ioctl(IN struct net_device *prDev,
		       IN OUT struct ifreq *prReq, IN int i4Cmd);

int priv_support_driver_cmd(IN struct net_device *prDev,
			    IN OUT struct ifreq *prReq, IN int i4Cmd);
int32_t priv_driver_cmds(IN struct net_device *prNetDev,
			 IN int8_t *pcCommand, IN int32_t i4TotalLen);

int priv_driver_set_cfg(IN struct net_device *prNetDev,
			IN char *pcCommand, IN int i4TotalLen);

#if CFG_SUPPORT_QA_TOOL
int
priv_ate_set(IN struct net_device *prNetDev,
	     IN struct iw_request_info *prIwReqInfo,
	     IN union iwreq_data *prIwReqData, IN char *pcExtra);
#endif
#endif
/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */
#if CFG_SUPPORT_CSI
int priv_driver_set_csi(IN struct GLUE_INFO *prGlueInfo,
	IN char *pcCommand, IN int i4TotalLen);
#endif /* #if CFG_SUPPORT_CSI */

int mtk_event_handler_sample(wifi_event_t event_id, unsigned char *payload,
	unsigned int len);
#endif /* _GL_WEXT_PRIV_H */
