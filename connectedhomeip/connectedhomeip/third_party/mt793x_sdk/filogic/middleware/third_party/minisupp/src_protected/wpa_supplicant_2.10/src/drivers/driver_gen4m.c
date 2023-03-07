/* Copyright Statement:
 *
 * (C) 2018-2018  MediaTek Inc. All rights reserved.
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


/****************************************************************************
 *
 * WPA-Supplicant to GEN4M glue layer.
 *
 ****************************************************************************/


#include "includes.h"
#if (!defined(__CC_ARM)) && (!defined(__ICCARM__))
#include <sys/types.h>
#endif

// TODO: should include gen4m header file to api definition.
#include "wifi_netif.h"

#include "common.h"
#include "driver.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"

#include "eloop.h"
#include "utils/list.h"
#include "../l2_packet/l2_packet.h"
#ifdef HOSTAPD
#include "ap/hostapd.h"
#endif /* HOSTAPD */
#include "../wpa_supplicant/wpa_supplicant_i.h"
#include "../wpa_supplicant/wps_supplicant.h"
#include "../wpa_supplicant/config_wpa_supplicant.h"
#include "../wpa_supplicant/scan_wpa_supplicant.h"
#include "../ap/hostapd.h"
#include "../ap/ap_config.h"
#include "../ap/sta_info.h"
#include "../rsn_supp/wpa.h"
#include "../rsn_supp/wpa_i.h"
#include "../wpa_supplicant/bss_wpa_supplicant.h"
#include "../crypto/sha1.h"
#ifdef CONFIG_AP
#include "../wpa_supplicant/ap.h"
#include "../wpa_supplicant/driver_i.h"
#endif /* CONFIG_AP */

#ifdef CONFIG_WPS
//#include "wifi_wps.h"
#endif/*CONFIG_WPS*/
//#include "misc.h"
//#include "inband_queue.h"
//#include "wifi_init.h"
//#include "wifi_channel.h"
//#include "wifi_scan.h"
//#include "driver_inband.h"
/* #include "mt7658_netif.h" */

/* GEN4M driver */
#include "gl_upperlayer.h"

/**
 * for Wi-Fi event definitions.
 */
#include "wifi_event_id.h"

/* get lwip netif */
#include "lwip/netif.h"
//extern struct netif sta_if;
#include "wifi_api_ex.h"
/*
 * Wi-Fi event dispatcher API
 */
#include "wifi_event_gen4m.h"

#include "wifi_os_api.h"
//TODO: fixme change from extern to local for build pass
uint8_t g_scan_ssid[32] ;
uint32_t g_scan_ssid_len;
unsigned char g_scanning;
unsigned char g_scan_by_app;
unsigned char g_scan_done_flag; //Pengfei+, Sta_Fast_Link feature
unsigned char g_rx_handler_mode;
//extern wifi_init_callback g_wifi_init_callback;
unsigned char g_sched_scan_enabled = 1;

extern int32_t supplicant_ready_query(uint8_t init_ready, uint8_t opmode_ready, uint32_t timeout);

extern struct wpa_supplicant *mtk_supplicant_get_interface_by_port(uint8_t port);

unsigned char g_mode_change = 0;
extern struct wpa_global *global_entry;
extern char *sta_ifname;
extern char *ap_ifname;
extern const char *ctrl_interface;
extern struct wpa_supplicant *g_wpa_s;
extern wifi_config_ext_t g_wifi_config_ext;
#ifdef CONFIG_AP
extern struct netif ap_netif;
#endif
unsigned char wps_auto_connection_ctrl = 1;

#ifdef MTK_WIFI_DIRECT_ENABLE
extern char             *p2p_ifname;
extern unsigned char    g_p2p_operation_channel;
extern uint8_t          g_p2p_role;

#include "dhcpd.h"
#include "dhcp.h"
#endif /*MTK_WIFI_DIRECT_ENABLE*/

//extern SCAN_ARRAY_ITEM g_scan_array[MAX_SCAN_ARRAY_ITEM];

#define MAX_SCAN_ARRAY_SIZE  4
#define DRV_INBAND_SHRINK_SIZE  (1)

#define MAX_CHN_NUM          39
#define MAX_2G_BAND_CHN_NUM  14
#define MAX_5G_BAND_CHN_NUM  (MAX_CHN_NUM - MAX_2G_BAND_CHN_NUM)

/* for event packet buffer */
#define MAX_EVTID_SIZE		 1
#define MAX_DATA_LEN_SIZE	 4
#define MAX_EVT_HDR_SIZE	 MAX_EVTID_SIZE + MAX_DATA_LEN_SIZE

enum ENUM_BAND {
	BAND_NULL,
	BAND_2G4,
	BAND_5G,
	BAND_NUM
};

volatile int gWiFiFlag = 0;

//#include "inband_queue.h"
/* copy from inband_queue.h*/
/* Define EVENT ID from firmware to Host (v0.09) */

// #include "nl80211_copy.h"
/* copy from nl80211_copy.h */
enum nl80211_commands {
	/* don't change the order or add anything between, this is ABI! */
	NL80211_CMD_UNSPEC,

	NL80211_CMD_GET_WIPHY,    /* can dump */
	NL80211_CMD_SET_WIPHY,
	NL80211_CMD_NEW_WIPHY,
	NL80211_CMD_DEL_WIPHY,

	NL80211_CMD_GET_INTERFACE,  /* can dump */
	NL80211_CMD_SET_INTERFACE,
	NL80211_CMD_NEW_INTERFACE,
	NL80211_CMD_DEL_INTERFACE,

	NL80211_CMD_GET_KEY,
	NL80211_CMD_SET_KEY, /* 10 */
	NL80211_CMD_NEW_KEY,
	NL80211_CMD_DEL_KEY,

	NL80211_CMD_GET_BEACON,
	NL80211_CMD_SET_BEACON,
	NL80211_CMD_NEW_BEACON,
	NL80211_CMD_DEL_BEACON,

	NL80211_CMD_GET_STATION,
	NL80211_CMD_SET_STATION,
	NL80211_CMD_NEW_STATION,
	NL80211_CMD_DEL_STATION, /* 20 */

	NL80211_CMD_GET_MPATH,
	NL80211_CMD_SET_MPATH,
	NL80211_CMD_NEW_MPATH,
	NL80211_CMD_DEL_MPATH,

	NL80211_CMD_SET_BSS,

	NL80211_CMD_SET_REG,
	NL80211_CMD_REQ_SET_REG,

	NL80211_CMD_GET_MESH_CONFIG,
	NL80211_CMD_SET_MESH_CONFIG,

	NL80211_CMD_SET_MGMT_EXTRA_IE, /* 30 */

	NL80211_CMD_GET_REG,

	NL80211_CMD_GET_SCAN,
	NL80211_CMD_TRIGGER_SCAN,
	NL80211_CMD_NEW_SCAN_RESULTS,
	NL80211_CMD_SCAN_ABORTED,

	NL80211_CMD_REG_CHANGE,

	NL80211_CMD_AUTHENTICATE,
	NL80211_CMD_ASSOCIATE,
	NL80211_CMD_DEAUTHENTICATE,
	NL80211_CMD_DISASSOCIATE,

	NL80211_CMD_MICHAEL_MIC_FAILURE,

	NL80211_CMD_REG_BEACON_HINT,

	NL80211_CMD_JOIN_IBSS,
	NL80211_CMD_LEAVE_IBSS,

	NL80211_CMD_TESTMODE,

	NL80211_CMD_CONNECT,
	NL80211_CMD_ROAM,
	NL80211_CMD_DISCONNECT,

	NL80211_CMD_SET_WIPHY_NETNS,

	NL80211_CMD_GET_SURVEY,
	NL80211_CMD_NEW_SURVEY_RESULTS,

	NL80211_CMD_SET_PMKSA,
	NL80211_CMD_DEL_PMKSA,
	NL80211_CMD_FLUSH_PMKSA,

	NL80211_CMD_REMAIN_ON_CHANNEL,
	NL80211_CMD_CANCEL_REMAIN_ON_CHANNEL,

	NL80211_CMD_SET_TX_BITRATE_MASK,

	NL80211_CMD_REGISTER_FRAME,
	NL80211_CMD_REGISTER_ACTION = NL80211_CMD_REGISTER_FRAME,
	NL80211_CMD_FRAME,
	NL80211_CMD_ACTION = NL80211_CMD_FRAME,
	NL80211_CMD_FRAME_TX_STATUS,
	NL80211_CMD_ACTION_TX_STATUS = NL80211_CMD_FRAME_TX_STATUS,

	NL80211_CMD_SET_POWER_SAVE,
	NL80211_CMD_GET_POWER_SAVE,

	NL80211_CMD_SET_CQM,
	NL80211_CMD_NOTIFY_CQM,

	NL80211_CMD_SET_CHANNEL,
	NL80211_CMD_SET_WDS_PEER,

	NL80211_CMD_FRAME_WAIT_CANCEL,

	NL80211_CMD_JOIN_MESH,
	NL80211_CMD_LEAVE_MESH,

	NL80211_CMD_UNPROT_DEAUTHENTICATE,
	NL80211_CMD_UNPROT_DISASSOCIATE,

	NL80211_CMD_NEW_PEER_CANDIDATE,

	NL80211_CMD_GET_WOWLAN,
	NL80211_CMD_SET_WOWLAN,

	NL80211_CMD_START_SCHED_SCAN,
	NL80211_CMD_STOP_SCHED_SCAN,
	NL80211_CMD_SCHED_SCAN_RESULTS,
	NL80211_CMD_SCHED_SCAN_STOPPED,

	NL80211_CMD_SET_REKEY_OFFLOAD,

	NL80211_CMD_PMKSA_CANDIDATE,

	NL80211_CMD_TDLS_OPER,
	NL80211_CMD_TDLS_MGMT,

	NL80211_CMD_UNEXPECTED_FRAME,

	NL80211_CMD_PROBE_CLIENT,

	NL80211_CMD_REGISTER_BEACONS,

	NL80211_CMD_UNEXPECTED_4ADDR_FRAME,

	NL80211_CMD_SET_NOACK_MAP,

	NL80211_CMD_EXTERNAL_AUTH,

	NL80211_CMD_ACS,
	/* add new commands above here */
	NL80211_CMD_DISCONNECT_LOCALLY,

	NL80211_CMD_FT_INDICATION,

	/* used to define NL80211_CMD_MAX below */
	__NL80211_CMD_AFTER_LAST,
	NL80211_CMD_MAX = __NL80211_CMD_AFTER_LAST - 1
};

enum nl80211_iftype {
	NL80211_IFTYPE_UNSPECIFIED,
	NL80211_IFTYPE_ADHOC,
	NL80211_IFTYPE_STATION,
	NL80211_IFTYPE_AP,
	NL80211_IFTYPE_AP_VLAN,
	NL80211_IFTYPE_WDS,
	NL80211_IFTYPE_MONITOR,
	NL80211_IFTYPE_MESH_POINT,
	NL80211_IFTYPE_P2P_CLIENT,
	NL80211_IFTYPE_P2P_GO,

	/* keep last */
	NUM_NL80211_IFTYPES,
	NL80211_IFTYPE_MAX = NUM_NL80211_IFTYPES - 1
};

enum nl80211_attrs {
	/* don't change the order or add anything between, this is ABI! */
	NL80211_ATTR_UNSPEC,

	NL80211_ATTR_WIPHY,
	NL80211_ATTR_WIPHY_NAME,

	NL80211_ATTR_IFINDEX,
	NL80211_ATTR_IFNAME,
	NL80211_ATTR_IFTYPE,

	NL80211_ATTR_MAC,

	NL80211_ATTR_KEY_DATA,
	NL80211_ATTR_KEY_IDX,
	NL80211_ATTR_KEY_CIPHER,
	NL80211_ATTR_KEY_SEQ,
	NL80211_ATTR_KEY_DEFAULT,

	NL80211_ATTR_BEACON_INTERVAL,
	NL80211_ATTR_DTIM_PERIOD,
	NL80211_ATTR_BEACON_HEAD,
	NL80211_ATTR_BEACON_TAIL,

	NL80211_ATTR_STA_AID,
	NL80211_ATTR_STA_FLAGS,
	NL80211_ATTR_STA_LISTEN_INTERVAL,
	NL80211_ATTR_STA_SUPPORTED_RATES,
	NL80211_ATTR_STA_VLAN,
	NL80211_ATTR_STA_INFO,

	NL80211_ATTR_WIPHY_BANDS,

	NL80211_ATTR_MNTR_FLAGS,

	NL80211_ATTR_MESH_ID,
	NL80211_ATTR_STA_PLINK_ACTION,
	NL80211_ATTR_MPATH_NEXT_HOP,
	NL80211_ATTR_MPATH_INFO,

	NL80211_ATTR_BSS_CTS_PROT,
	NL80211_ATTR_BSS_SHORT_PREAMBLE,
	NL80211_ATTR_BSS_SHORT_SLOT_TIME,

	NL80211_ATTR_HT_CAPABILITY,

	NL80211_ATTR_SUPPORTED_IFTYPES,

	NL80211_ATTR_REG_ALPHA2,
	NL80211_ATTR_REG_RULES,

	NL80211_ATTR_MESH_CONFIG,

	NL80211_ATTR_BSS_BASIC_RATES,

	NL80211_ATTR_WIPHY_TXQ_PARAMS,
	NL80211_ATTR_WIPHY_FREQ,
	NL80211_ATTR_WIPHY_CHANNEL_TYPE,

	NL80211_ATTR_KEY_DEFAULT_MGMT,

	NL80211_ATTR_MGMT_SUBTYPE,
	NL80211_ATTR_IE,

	NL80211_ATTR_MAX_NUM_SCAN_SSIDS,

	NL80211_ATTR_SCAN_FREQUENCIES,
	NL80211_ATTR_SCAN_SSIDS,
	NL80211_ATTR_GENERATION, /* replaces old SCAN_GENERATION */
	NL80211_ATTR_BSS,

	NL80211_ATTR_REG_INITIATOR,
	NL80211_ATTR_REG_TYPE,

	NL80211_ATTR_SUPPORTED_COMMANDS,

	NL80211_ATTR_FRAME,
	NL80211_ATTR_SSID,
	NL80211_ATTR_AUTH_TYPE,
	NL80211_ATTR_REASON_CODE,

	NL80211_ATTR_KEY_TYPE,

	NL80211_ATTR_MAX_SCAN_IE_LEN,
	NL80211_ATTR_CIPHER_SUITES,

	NL80211_ATTR_FREQ_BEFORE,
	NL80211_ATTR_FREQ_AFTER,

	NL80211_ATTR_FREQ_FIXED,


	NL80211_ATTR_WIPHY_RETRY_SHORT,
	NL80211_ATTR_WIPHY_RETRY_LONG,
	NL80211_ATTR_WIPHY_FRAG_THRESHOLD,
	NL80211_ATTR_WIPHY_RTS_THRESHOLD,

	NL80211_ATTR_TIMED_OUT,

	NL80211_ATTR_USE_MFP,

	NL80211_ATTR_STA_FLAGS2,

	NL80211_ATTR_CONTROL_PORT,

	NL80211_ATTR_TESTDATA,

	NL80211_ATTR_PRIVACY,

	NL80211_ATTR_DISCONNECTED_BY_AP,
	NL80211_ATTR_STATUS_CODE,

	NL80211_ATTR_CIPHER_SUITES_PAIRWISE,
	NL80211_ATTR_CIPHER_SUITE_GROUP,
	NL80211_ATTR_WPA_VERSIONS,
	NL80211_ATTR_AKM_SUITES,

	NL80211_ATTR_REQ_IE,
	NL80211_ATTR_RESP_IE,

	NL80211_ATTR_PREV_BSSID,

	NL80211_ATTR_KEY,
	NL80211_ATTR_KEYS,

	NL80211_ATTR_PID,

	NL80211_ATTR_4ADDR,

	NL80211_ATTR_SURVEY_INFO,

	NL80211_ATTR_PMKID,
	NL80211_ATTR_MAX_NUM_PMKIDS,

	NL80211_ATTR_DURATION,

	NL80211_ATTR_COOKIE,

	NL80211_ATTR_WIPHY_COVERAGE_CLASS,

	NL80211_ATTR_TX_RATES,

	NL80211_ATTR_FRAME_MATCH,

	NL80211_ATTR_ACK,

	NL80211_ATTR_PS_STATE,

	NL80211_ATTR_CQM,

	NL80211_ATTR_LOCAL_STATE_CHANGE,

	NL80211_ATTR_AP_ISOLATE,

	NL80211_ATTR_WIPHY_TX_POWER_SETTING,
	NL80211_ATTR_WIPHY_TX_POWER_LEVEL,

	NL80211_ATTR_TX_FRAME_TYPES,
	NL80211_ATTR_RX_FRAME_TYPES,
	NL80211_ATTR_FRAME_TYPE,

	NL80211_ATTR_CONTROL_PORT_ETHERTYPE,
	NL80211_ATTR_CONTROL_PORT_NO_ENCRYPT,

	NL80211_ATTR_SUPPORT_IBSS_RSN,

	NL80211_ATTR_WIPHY_ANTENNA_TX,
	NL80211_ATTR_WIPHY_ANTENNA_RX,

	NL80211_ATTR_MCAST_RATE,

	NL80211_ATTR_OFFCHANNEL_TX_OK,

	NL80211_ATTR_BSS_HT_OPMODE,

	NL80211_ATTR_KEY_DEFAULT_TYPES,

	NL80211_ATTR_MAX_REMAIN_ON_CHANNEL_DURATION,

	NL80211_ATTR_MESH_SETUP,

	NL80211_ATTR_WIPHY_ANTENNA_AVAIL_TX,
	NL80211_ATTR_WIPHY_ANTENNA_AVAIL_RX,

	NL80211_ATTR_SUPPORT_MESH_AUTH,
	NL80211_ATTR_STA_PLINK_STATE,

	NL80211_ATTR_WOWLAN_TRIGGERS,
	NL80211_ATTR_WOWLAN_TRIGGERS_SUPPORTED,

	NL80211_ATTR_SCHED_SCAN_INTERVAL,

	NL80211_ATTR_INTERFACE_COMBINATIONS,
	NL80211_ATTR_SOFTWARE_IFTYPES,

	NL80211_ATTR_REKEY_DATA,

	NL80211_ATTR_MAX_NUM_SCHED_SCAN_SSIDS,
	NL80211_ATTR_MAX_SCHED_SCAN_IE_LEN,

	NL80211_ATTR_SCAN_SUPP_RATES,

	NL80211_ATTR_HIDDEN_SSID,

	NL80211_ATTR_IE_PROBE_RESP,
	NL80211_ATTR_IE_ASSOC_RESP,

	NL80211_ATTR_STA_WME,
	NL80211_ATTR_SUPPORT_AP_UAPSD,

	NL80211_ATTR_ROAM_SUPPORT,

	NL80211_ATTR_SCHED_SCAN_MATCH,
	NL80211_ATTR_MAX_MATCH_SETS,

	NL80211_ATTR_PMKSA_CANDIDATE,

	NL80211_ATTR_TX_NO_CCK_RATE,

	NL80211_ATTR_TDLS_ACTION,
	NL80211_ATTR_TDLS_DIALOG_TOKEN,
	NL80211_ATTR_TDLS_OPERATION,
	NL80211_ATTR_TDLS_SUPPORT,
	NL80211_ATTR_TDLS_EXTERNAL_SETUP,

	NL80211_ATTR_DEVICE_AP_SME,

	NL80211_ATTR_DONT_WAIT_FOR_ACK,

	NL80211_ATTR_FEATURE_FLAGS,

	NL80211_ATTR_PROBE_RESP_OFFLOAD,

	NL80211_ATTR_PROBE_RESP,

	NL80211_ATTR_DFS_REGION,

	NL80211_ATTR_DISABLE_HT,
	NL80211_ATTR_HT_CAPABILITY_MASK,

	NL80211_ATTR_NOACK_MAP,

	/* add attributes here, update the policy in nl80211.c */

	__NL80211_ATTR_AFTER_LAST,
	NL80211_ATTR_MAX = __NL80211_ATTR_AFTER_LAST - 1
};
/* end of copy from nl80211_copy.h */


/* copy from netlink */
struct nlattr { // TODO: should be rename
	unsigned short  nla_len;
	unsigned short  nla_type;
};
/* end of copy from netlink */


#define DRIVER_MSG_PORT_STA_ONLY 6662
#define DRIVER_MSG_PORT_AP_ONLY  6663
#define DRIVER_MSG_PORT_APCLI    6664
#define DRIVER_MSG_PORT_AP       6665
#define DRIVER_MSG_PORT_P2P      6666

/* copy from driver_nl80211 */
struct gen4m_global {
	void *ctx;
	struct dl_list interfaces;
	int if_add_ifindex;
	void *drv_priv;
	//leo struct netlink_data *netlink;
	//leo struct nl_cb *nl_cb;
	//leo struct nl_handle *nl;
	int nl80211_id;
	int ioctl_sock; /* socket for ioctl() use */

	//leo struct nl_handle *nl_event;
};


struct wpa_driver_gen4m_data {
#if 0 //ndef DRV_INBAND_SHRINK_SIZE
	struct nl80211_global   *global;
	struct dl_list          list;
	struct dl_list          wiphy_list;
	char                    phyname[32];
#endif
	void *ctx;
	int ifindex;
	int if_removed;
	int if_disabled;
	int ignore_if_down_event;
	//leo struct rfkill_data *rfkill;
	struct wpa_driver_capa capa;
	int has_capability;

	int operstate;
	int scan_complete_events;

	//leo struct nl_cb *nl_cb;
#ifndef DRV_INBAND_SHRINK_SIZE
	u8 auth_bssid[ETH_ALEN];
#endif
	u8 bssid[ETH_ALEN];
	int associated;
	u8 ssid[WIFI_MAX_LENGTH_OF_SSID];
	size_t ssid_len;
	enum nl80211_iftype nlmode; // nl80211_copy.h
	enum nl80211_iftype ap_scan_as_station; // nl80211_copy.h
	unsigned int assoc_freq;

#ifndef DRV_INBAND_SHRINK_SIZE
	int monitor_sock;
	int monitor_ifidx;
	int monitor_refcount;
#endif // #ifndef DRV_INBAND_SHRINK_SIZE

	unsigned int disabled_11b_rates: 1;
	unsigned int pending_remain_on_chan: 1;
	unsigned int in_interface_list: 1;
	unsigned int device_ap_sme: 1;
	unsigned int poll_command_supported: 1;
	unsigned int data_tx_status: 1;
	unsigned int scan_for_auth: 1;
	unsigned int retry_auth: 1;
	unsigned int use_monitor: 1;

#ifndef DRV_INBAND_SHRINK_SIZE
	u64 remain_on_chan_cookie;
	u64 send_action_cookie;

	unsigned int last_mgmt_freq;

	//leo struct wpa_driver_scan_filter *filter_ssids;
	size_t num_filter_ssids;

	//leo struct i802_bss first_bss;

	int eapol_tx_sock;
#endif // #ifndef DRV_INBAND_SHRINK_SIZE

	struct l2_packet_data *sock_recv;

#ifdef HOSTAPD
	struct hostapd_data *hapd;

	int eapol_sock; /* socket for EAPOL frames */

	int default_if_indices[16];
	int *if_indices;
	int num_if_indices;

	int last_freq;
	int last_freq_ht;
#endif /* HOSTAPD */

#ifndef DRV_INBAND_SHRINK_SIZE
	/* From failed authentication command */
	int auth_freq;
	u8 auth_bssid_[ETH_ALEN];
	u8 auth_ssid[32];
	size_t auth_ssid_len;
	int auth_alg;
	u8 *auth_ie;
	size_t auth_ie_len;
	u8 auth_wep_key[4][16];
	size_t auth_wep_key_len[4];
	int auth_wep_tx_keyidx;
	int auth_local_state_change;
	int auth_p2p;
#endif // #ifndef DRV_INBAND_SHRINK_SIZE
	int driver_msg_sock;
	int driver_msg_port;

	u8 own_addr[ETH_ALEN];
	int connect_wmm_ap;
	u8 ignore_sta_disconnect_event;
};


struct l2_packet_data {
	char ifname[17];
	u8 own_addr[ETH_ALEN];
	void (*rx_callback)(void *ctx, const u8 *src_addr,
			    const u8 *buf, size_t len);
	void *rx_callback_ctx;
	int l2_hdr; /* whether to include layer 2 (Ethernet) header data
        * buffers */
	int fd;
};

enum nl80211_external_auth_action {
	NL80211_EXTERNAL_AUTH_START,
	NL80211_EXTERNAL_AUTH_ABORT,
};

/**
 * struct cfg80211_ssid - SSID description
 * @ssid: the SSID
 * @ssid_len: length of the ssid
 */
struct cfg80211_ssid {
	u8 ssid[IEEE80211_MAX_SSID_LEN];
	u8 ssid_len;
};


/**
 * struct cfg80211_external_auth_params - Trigger External authentication.
 *
 * Commonly used across the external auth request and event interfaces.
 *
 * @action: action type / trigger for external authentication. Only significant
 *  for the authentication request event interface (driver to user space).
 * @bssid: BSSID of the peer with which the authentication has
 *  to happen. Used by both the authentication request event and
 *  authentication response command interface.
 * @ssid: SSID of the AP.  Used by both the authentication request event and
 *  authentication response command interface.
 * @key_mgmt_suite: AKM suite of the respective authentication. Used by the
 *  authentication request event interface.
 * @status: status code, %WLAN_STATUS_SUCCESS for successful authentication,
 *  use %WLAN_STATUS_UNSPECIFIED_FAILURE if user space cannot give you
 *  the real status code for failures. Used only for the authentication
 *  response command interface (user space to driver).
 */
struct cfg80211_external_auth_params {
	enum nl80211_external_auth_action action;
	u8 bssid[ETH_ALEN];
	struct cfg80211_ssid ssid;
	unsigned int key_mgmt_suite;
	u16 status;
};


#if 1
typedef int (*inband_cmd_handle_t)(void *priv, const u8 *buf, uint8_t buf_len);

typedef struct {
	char *cmd;
	inband_cmd_handle_t fptr;
} inband_cmd_t;

static void do_process_drv_event(struct wpa_driver_gen4m_data *drv, int cmd, void *data, int len);
int static do_process_wifi_event(void *priv, unsigned char evt_id, unsigned char *body, int len);


#ifdef __NOT_PORTED__
static void wpa_driver_inband_raw_receive(
	void *ctx, const u8 *buf, size_t len);
#endif // __NOT_PORTED__


#ifdef __NOT_PORTED__
static void mtk_supplicant_radio_onoff(uint8_t radio_on);
#endif // __NOT_PORTED__


#ifdef MTK_WIFI_DIRECT_ENABLE
unsigned int            g_p2p_listen_freq;
union wpa_event_data    g_p2p_tx_status_event;
int                     g_p2p_rx_public_action_freq;

struct wpa_driver_inband_tx_status_info {
	u8 ack;
	u8 ucReserved[3];
};
#endif /*MTK_WIFI_DIRECT_ENABLE*/

#if 0 /* TODO: fixme move from gen4m to here */
global_event_callback __process_global_event[MAX_INTERFACE_NUM];

void register_process_global_event(void *priv, int (*func)(void *, unsigned char, unsigned char *, int), unsigned char op_mode)
{
	uint8_t idx = 0;
	if (op_mode == WIFI_MODE_AP_ONLY) {
		idx = WIFI_PORT_AP;
	}

	__process_global_event[idx].priv = priv;
	__process_global_event[idx].func = func;
	__process_global_event[idx].op_mode = op_mode;
}

void unregister_process_global_event(void)
{
	uint8_t i = 0;
	for (i = 0; i < MAX_INTERFACE_NUM; i++) {
		__process_global_event[i].priv = NULL;
		__process_global_event[i].func = NULL;
		__process_global_event[i].op_mode = 0;
		__process_global_event[i].ready = 0;
	}
}
#endif

int static _do_process_driver_event(void *priv, unsigned char evt_id, unsigned char *body, int len)
{
	struct wpa_driver_gen4m_data    *inband_drv = priv;
	void                            *arg        = NULL;
	int                             i           = 0;

	wpa_printf(MSG_DEBUG, "*** EVENT_ID_IOT = 0x%x ***\n", evt_id);

	typedef struct _driver_to_nl802_id_t {
		unsigned char   driver_event_id;
		int             nl80211_cmd_id;
	} driver_to_nl802_id_t;

	const static driver_to_nl802_id_t map[] = {
		{ WIFI_EVENT_ID_IOT_SCAN_LIST, NL80211_CMD_NEW_SCAN_RESULTS },
		{ WIFI_EVENT_ID_IOT_CONNECTED, NL80211_CMD_CONNECT },
		{ WIFI_EVENT_ID_IOT_DISASSOC, NL80211_CMD_DISCONNECT},
		{ WIFI_EVENT_ID_IOT_DISASSOC_LOCALLY, NL80211_CMD_DISCONNECT_LOCALLY},
		{ WIFI_EVENT_ID_IOT_ASSOCIATED, NL80211_CMD_NEW_STATION },
		{ WIFI_EVENT_ID_IOT_EXTERNAL_AUTH, NL80211_CMD_EXTERNAL_AUTH},
		{ WIFI_EVENT_ID_IOT_TX_STATUS, NL80211_CMD_FRAME_TX_STATUS},
		{ WIFI_EVENT_ID_IOT_RX_MGMT, NL80211_CMD_FRAME},
		{ WIFI_EVENT_ID_IOT_ASSOC_REJECT, NL80211_CMD_CONNECT},
		{ WIFI_EVENT_ID_IOT_ACS, NL80211_CMD_ACS},
		{ WIFI_EVENT_ID_IOT_FT_INDICATION, NL80211_CMD_FT_INDICATION},
		{ 0,                           NL80211_CMD_MAX }
	};

	while (map[i].nl80211_cmd_id <= NL80211_CMD_MAX) {
		if (map[i].driver_event_id == evt_id) {
			wpa_printf(MSG_DEBUG, "%s: map event 0x%x to cmd 0x%x\n", __FUNCTION__, evt_id,
				   map[i].nl80211_cmd_id);
			arg = body;
			do_process_drv_event(inband_drv, map[i].nl80211_cmd_id, arg, len);
			return 0;
		}
		i++;
	}

	switch (evt_id) {
	case WIFI_EVENT_ID_IOT_RELOAD_CONFIGURATION: {
		if (wpa_supplicant_reload_configuration(inband_drv->ctx) == 0) {
			wpa_supplicant_req_scan(inband_drv->ctx, 0, 100000);
			if (WIFI_MODE_STA_ONLY == global_entry->op_mode || WIFI_MODE_REPEATER == global_entry->op_mode)
				mtk_supplicant_radio_onoff(1);
		} else {
			wifi_api2supp_result_semphr_give();
			return -1;
		}

		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_OPMODE_SWITCH: {
		extern os_semaphore_t opmode_switch_mutex;
		extern os_semaphore_t opmode_switch_result_semphr;

		//__g_wpa_supplicant_api.get_sta_qos_bit = NULL; /* Do not support checking qos bit. */

		/*status = wifi_inband_opmode(true, &new_opmode);
		if(status < 0) {
		    *ret_addr = status;
		    if(NULL != opmode_switch_result_semphr)
		        wifi_os_semphr_give(opmode_switch_result_semphr);
		    return -1;
		}*/
#ifdef CONFIG_AP
		int32_t  status;
		uint8_t  new_opmode = *body;
		mtk_supplicant_stop_scan();

		status = wpa_supplicant_entry_op_mode_set(new_opmode);
		wpa_printf(MSG_DEBUG, "op mode set status: %d\n", status);

		if (new_opmode == WIFI_MODE_STA_ONLY) { //Pengfei Add: for disable auto connect feature
			mtk_supplicant_stop_scan();
			g_scanning = 0;
			g_scan_by_app = 0;
		}
#else
		wpa_printf(MSG_DEBUG, "SAP mode is not supported.\n");
#endif
		if (NULL != opmode_switch_result_semphr)
			wifi_os_semphr_give(opmode_switch_result_semphr);
		wifi_api2supp_result_semphr_give();
	}
	break;

	default:
		break;
	}
	do_process_wifi_event(priv, evt_id, body, len);

	return -1;
}

static void driver_msg_recv(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct wpa_driver_gen4m_data    *inband_drv = (struct wpa_driver_gen4m_data *) sock_ctx;
	unsigned char                   evt_id;
	unsigned char                   *body;
	int                             len_body;
	struct sockaddr_in              addr;
	unsigned char                   buf[MAX_EVT_HDR_SIZE + EVENT_DATA_MAX_LEN];
	int                             len;
	socklen_t                       fromlen = sizeof(addr);

	len = recvfrom(sock, buf, sizeof(buf), 0,
		       (struct sockaddr *)&addr, &fromlen);

	if (len < 0) {
		wpa_printf(MSG_ERROR, "packet length is %d, not available", len);
		return;
	}

	evt_id = buf[0];
	body   = buf + MAX_EVT_HDR_SIZE;
	os_memcpy(&len_body, buf + MAX_EVTID_SIZE, MAX_DATA_LEN_SIZE);
	_do_process_driver_event(inband_drv, evt_id, body, len_body);
}

int mtk_supplicant_set_bcn_interval(uint8_t interval)
{
	struct wpa_ssid *supp_ssid;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	if (new_conf_ap) {
		supp_ssid = new_conf_ap->ssid;
		supp_ssid->beacon_int = interval;
		wpa_printf(MSG_ERROR, "%s: beacon interval = %d", __FUNCTION__, supp_ssid->beacon_int);
		return 0;
	} else {
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	}
	return -1;
}

int mtk_supplicant_set_dtim_interval(uint8_t interval)
{
	struct wpa_ssid *supp_ssid;
	struct wpa_supplicant *wpa_s;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	wpa_s = wpa_supplicant_get_iface(global_entry, ap_ifname);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}

	if (wpa_s->conf) {
		supp_ssid = wpa_s->conf->ssid;
		supp_ssid->dtim_period = interval;
		if (new_conf_ap) {
			supp_ssid = new_conf_ap->ssid;
			supp_ssid->dtim_period = interval;
		}
		wpa_printf(MSG_ERROR, "%s: dtim periord = %d", __FUNCTION__, supp_ssid->dtim_period);
		return 0;
	} else {
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	}
	return -1;
}

int mtk_supplicant_get_dtim_interval(uint8_t *interval)
{
	struct wpa_ssid *supp_ssid;
	struct wpa_supplicant *wpa_s;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	wpa_s = wpa_supplicant_get_iface(global_entry, ap_ifname);
		if (wpa_s == NULL) {
			wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
			return -1;
		}

	if (wpa_s->conf) {
		supp_ssid = wpa_s->conf->ssid;
		*interval = supp_ssid->dtim_period;
		return 0;
	} else {
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	}
	return -1;
}

int mtk_supplicant_set_channel(uint8_t port, uint8_t channel)
{
	struct wpa_ssid *supp_ssid;
	struct wpa_supplicant *wpa_s;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	wpa_s = mtk_supplicant_get_interface_by_port(port);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}
	if (wpa_s->conf) {
		supp_ssid = wpa_s->conf->ssid;
		if (supp_ssid->scan_freq == NULL)
			supp_ssid->scan_freq = os_malloc(sizeof(int) * 2);
		if (supp_ssid->scan_freq) {
			uint32_t freq = 0;
			if (channel >= 1 && channel <= 13)
				freq = 2412 + (channel - 1) * 5;
			else if (channel == 14)
				freq = 2484;
			else if (channel >= 36 && channel <= 165)
				freq = 5000 + 5 * channel;
			else
				return -1;
			if (port == WIFI_PORT_STA) {
				supp_ssid->scan_freq[0] = freq;
				supp_ssid->scan_freq[1] = 0;
			} else if (port == WIFI_PORT_AP) {
				supp_ssid->frequency = freq;
				wpa_printf(MSG_ERROR, "%s: supp_ssid->frequency = %d\n", __FUNCTION__, supp_ssid->frequency);
			} else {
				wpa_printf(MSG_ERROR, "Not support.");
				return -1;
			}
			return 0;
		} else
			wpa_printf(MSG_ERROR, "%s: memory alloc fail.", __FUNCTION__);
	} else {
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	}
	return -1;

}

int mtk_supplicant_set_multi_channel(uint8_t port, uint8_t *channel, uint8_t channel_len)
{
	static struct wpa_ssid *supp_ssid = NULL;
	struct wpa_supplicant *wpa_s;
	uint32_t freq[] = {0};
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	wpa_s = mtk_supplicant_get_interface_by_port(port);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}
	if (wpa_s->conf) {
		supp_ssid = wpa_s->conf->ssid;
		if (supp_ssid != NULL)
			os_free(supp_ssid->scan_freq);
		supp_ssid->scan_freq = os_malloc(sizeof(int) * (channel_len + 1));
		for (uint8_t i = 0; i < channel_len; i ++) {
			if (supp_ssid->scan_freq) {
				if (channel[i] >= 1 && channel[i] <= 13)
					freq[i] = 2412 + (channel[i] - 1) * 5;
				else if (channel[i] == 14)
					freq[i] = 2484;
				else if (channel[i] >= 36 && channel[i] <= 165)
					freq[i] = 5000 + 5 * channel[i];
				else
					return -1;
				if (port == WIFI_PORT_STA) {
					supp_ssid->scan_freq[i] = freq[i];
				} else if (port == WIFI_PORT_AP) {
					supp_ssid->frequency = freq[i];
					wpa_printf(MSG_ERROR, "%s: supp_ssid->frequency = %d\n", __FUNCTION__, supp_ssid->frequency);
				} else {
					wpa_printf(MSG_ERROR, "Not support.");
					return -1;
				}
			} else {
				wpa_printf(MSG_ERROR, "%s: memory alloc fail.", __FUNCTION__);
				return -1;
			}
		}
		supp_ssid->scan_freq[channel_len] = 0;
		return 0;
	} else {
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	}
	return -1;
}


int mtk_supplicant_set_security(uint8_t port, uint8_t auth_mode, uint8_t encrypt_type)
{
	struct wpa_config *conf;
	struct wpa_ssid *ssid;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		ssid = conf->ssid;
		if (auth_mode == WIFI_AUTH_MODE_OPEN) {
			for (int idx = 0; idx < WIFI_NUMBER_WEP_KEYS; idx++) {
				os_memset(&ssid->wep_key[idx], 0, MAX_WEP_KEY_LEN);
				ssid->wep_key_len[idx] = 0;
			}
		}
		wpa_config_set_auth(conf->ssid, auth_mode);
		wpa_config_set_encr(conf->ssid, encrypt_type);
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}


int mtk_supplicant_set_multi_security(uint8_t port, uint32_t key_mgmt,
				      uint8_t encrypt_type)
{
	struct wpa_config *conf;
	struct wpa_ssid *ssid;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		ssid = conf->ssid;
		ssid->key_mgmt = key_mgmt;
		wpa_config_set_encr(conf->ssid, encrypt_type);
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}

int mtk_supplicant_set_ieee80211w(uint8_t port, uint8_t ieee80211w)
{
	struct wpa_config *conf;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		conf->ssid->ieee80211w = ieee80211w;
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}

int mtk_supplicant_set_proto(uint8_t port, uint8_t proto)
{
	struct wpa_config *conf;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		conf->ssid->proto = proto;
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}

int mtk_supplicant_get_security(uint8_t port, uint8_t *auth_mode, uint8_t *encrypt_type)
{
	struct wpa_ssid *ssid;
	struct wpa_supplicant *wpa_s;

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	wpa_s = mtk_supplicant_get_interface_by_port(port);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}
	if (wpa_s->conf) {
		ssid = wpa_s->conf->ssid;
		*auth_mode = ssid->auth_mode;
		*encrypt_type = ssid->encr_type;
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	return -1;
}


int mtk_supplicant_set_pmk(uint8_t port, uint8_t *pmk)
{
	struct wpa_ssid *ssid;
	struct wpa_config *conf;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		ssid = conf->ssid;
		os_memcpy(ssid->psk, pmk, 32);
		ssid->psk_set = 1;
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}


int mtk_supplicant_get_pmk(uint8_t port, uint8_t *pmk)
{
	struct wpa_ssid *ssid;
	struct wpa_supplicant *wpa_s;

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	wpa_s = mtk_supplicant_get_interface_by_port(port);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}
	if (wpa_s->conf) {
		ssid = wpa_s->conf->ssid;
		os_memcpy(pmk, ssid->psk, 32);
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	return -1;
}


/**
 * Calculate WPA PMK.
 *
 * @retval  0 if succeeded.
 * @retval -1 if the input format is invalid.
 */
#ifdef __NOT_PORTED__
static int mtk_supplicant_calculate_pmk(uint8_t *passphrase,
					uint8_t *ssid,
					uint8_t ssid_length,
					uint8_t *psk)
{
	if (hexstr2bin((const char *) passphrase, psk, WIFI_LENGTH_PMK) ||
			passphrase[ WIFI_LENGTH_PMK * 2 ] != '\0') {
		pbkdf2_sha1((const char *)passphrase,
			    (const u8 *)ssid,
			    (size_t)ssid_length,
			    4096,
			    (u8 *)psk,
			    WIFI_LENGTH_PMK);

		return 0;
	}
	return -1;
}
#endif // __NOT_PORTED__


int mtk_supplicant_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length)
{
	struct wpa_ssid *ssid;
	struct wpa_config *conf;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		ssid = conf->ssid;
		if (ssid->passphrase == NULL) {
			ssid->passphrase = os_zalloc(WIFI_LENGTH_PASSPHRASE + 1);
		}
		if (ssid->passphrase) {
			os_memset(ssid->passphrase, 0, WIFI_LENGTH_PASSPHRASE + 1);
			wpa_config_set_wpapsk(ssid, passphrase, passphrase_length);
			return 0;
		} else
			wpa_printf(MSG_ERROR, "%s: memory alloc fail.", __FUNCTION__);
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}

int mtk_supplicant_get_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length)
{
	struct wpa_ssid *ssid;
	struct wpa_supplicant *wpa_s;

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	wpa_s = mtk_supplicant_get_interface_by_port(port);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}
	if (wpa_s->conf) {
		ssid = wpa_s->conf->ssid;
		*passphrase_length = ssid->passphrase_len;
		os_memcpy(passphrase, ssid->passphrase, ssid->passphrase_len);
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	return -1;
}


int mtk_supplicant_set_ssid(uint8_t port, uint8_t *ssid, uint8_t ssid_length)
{
	struct wpa_ssid *supp_ssid;
	struct wpa_config *conf;

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		supp_ssid = conf->ssid;
		if (supp_ssid->ssid == NULL) {
			supp_ssid->ssid = os_zalloc(WIFI_MAX_LENGTH_OF_SSID + 1);
		}
		if (supp_ssid->ssid) {
			os_memset(supp_ssid->ssid, 0, WIFI_MAX_LENGTH_OF_SSID + 1);
			os_memcpy(supp_ssid->ssid, ssid, ssid_length);
			supp_ssid->ssid_len = ssid_length;
			return 0;
		} else
			wpa_printf(MSG_ERROR, "%s: memory alloc fail.", __FUNCTION__);
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}


int mtk_supplicant_get_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length)
{
	struct wpa_ssid *supp_ssid;
	struct wpa_supplicant *wpa_s;

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	wpa_s = mtk_supplicant_get_interface_by_port(port);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}
	if (wpa_s->conf) {
		supp_ssid = wpa_s->conf->ssid;
		*ssid_length = supp_ssid->ssid_len;
		os_memcpy(ssid, supp_ssid->ssid, supp_ssid->ssid_len);
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	return -1;
}


int mtk_supplicant_set_bssid(uint8_t *bssid)
{
	struct wpa_ssid *supp_ssid;
	unsigned char zero_mac[WIFI_MAC_ADDRESS_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	if (new_conf_sta) {
		supp_ssid = new_conf_sta->ssid;
		if (supp_ssid != NULL) {
			os_memcpy(supp_ssid->bssid, bssid, ETH_ALEN);
			if (os_memcmp(bssid, zero_mac, WIFI_MAC_ADDRESS_LENGTH) == 0)
				supp_ssid->bssid_set = 0;
			else
				supp_ssid->bssid_set = 1;
			return 0;
		} else
			wpa_printf(MSG_ERROR, "%s: memory alloc fail.", __FUNCTION__);
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;
}


int mtk_supplicant_set_wep_key(uint8_t port, wifi_wep_key_t *wep_keys)
{
	struct wpa_ssid *ssid;
	int idx;
	struct wpa_config *conf;
	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	conf = (port == WIFI_PORT_AP) ? new_conf_ap : new_conf_sta;
	if (conf == NULL) {
		wpa_printf(MSG_ERROR, "%s: conf get fail.", __FUNCTION__);
		return -1;
	}
	if (conf) {
		ssid = conf->ssid;
		for (idx = 0; idx < WIFI_NUMBER_WEP_KEYS; idx++) {
			if (wep_keys->wep_key_length[idx] != 0) {
				os_memset(&ssid->wep_key[idx], 0, MAX_WEP_KEY_LEN);
				if (wep_keys->wep_key_length[idx] >  MAX_WEP_KEY_LEN) {
					wpa_printf(MSG_ERROR, "%s: the length of wep key excess the max length (%d).", __FUNCTION__, MAX_WEP_KEY_LEN);
					return -1;
				}
				os_memcpy(&ssid->wep_key[idx], &wep_keys->wep_key[idx], wep_keys->wep_key_length[idx]);
				ssid->wep_key_len[idx] = wep_keys->wep_key_length[idx];
			}
		}
		ssid->wep_tx_keyidx = wep_keys->wep_tx_key_index;
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no memory to save new configuration.", __FUNCTION__);
	return -1;

}

int mtk_supplicant_get_wep_key(uint8_t port, wifi_wep_key_t *wep_keys)
{
	struct wpa_ssid *ssid;
	struct wpa_supplicant *wpa_s;
	int idx;

	wpa_printf(MSG_DEBUG, "%s\n", __FUNCTION__);

	supplicant_ready_query(TRUE, TRUE, 3000);

	wpa_s = mtk_supplicant_get_interface_by_port(port);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return -1;
	}
	if (wpa_s->conf) {
		ssid = wpa_s->conf->ssid;
		for (idx = 0; idx < WIFI_NUMBER_WEP_KEYS; idx++) {
			if (ssid->wep_key_len[idx] != 0) {
				os_memset(&wep_keys->wep_key[idx], 0, WIFI_MAX_WEP_KEY_LENGTH);
				os_memcpy(&wep_keys->wep_key[idx], &ssid->wep_key[idx], ssid->wep_key_len[idx]);
				wep_keys->wep_key_length[idx] = ssid->wep_key_len[idx];
			}
		}
		wep_keys->wep_tx_key_index = ssid->wep_tx_keyidx;
		return 0;
	} else
		wpa_printf(MSG_ERROR, "%s: no configuration.", __FUNCTION__);
	return -1;
}


void mtk_supplicant_stop_scan()
{
	supplicant_ready_query(TRUE, TRUE, 3000);

	struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
	if (wpa_s) {
		wpa_supplicant_cancel_sched_scan(wpa_s);
		wpa_supplicant_cancel_scan(wpa_s);
		wpa_s->conf->ap_scan = 0;
	}
}

//extern void wpa_supplicant_scan(void *eloop_ctx, void *timeout_ctx);
void mtk_supplicant_start_scan()
{
	struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
	const char *_cmd = "WAKE";
	size_t _cmd_len = sizeof(_cmd);

	if (wpa_s) {
		wpa_s->scan_req = MANUAL_SCAN_REQ;
		/* in eloop_run select timeout
		 * if not register it default wait 10 sec
		 * so worse case it will block 10 sec before next
		 * eloop_register_timeout
		 */
		wpa_s->conf->ap_scan = 1;
		//wpa_supplicant_scan(wpa_s, NULL);
		wpa_supplicant_req_scan(wpa_s, 0, 0);
		sendto(wake_up_eloop, _cmd, _cmd_len, 0,
		       (struct sockaddr *)&wake_up_addr, sizeof(wake_up_addr));
	}
}

int32_t mtk_supplicant_get_state(uint8_t *link_status)
{
	struct wpa_supplicant *wpa_s = NULL;
	char status = 0;
	supplicant_ready_query(TRUE, TRUE, 3000);
	for (wpa_s = global_entry->ifaces; wpa_s; wpa_s = wpa_s->next) {
		if (wpa_s->op_mode == WIFI_MODE_STA_ONLY) {
			if (wpa_s->wpa_state == WPA_COMPLETED) {
				*link_status = 1;
				return status;
			} else {
				*link_status = 0;
				return status;
			}
		}
	}
	*link_status = 0;
	return -1;
}

int32_t mtk_supplicant_get_radio(uint8_t *on_off)
{
	struct wpa_supplicant *wpa_s = NULL;
	supplicant_ready_query(TRUE, TRUE, 3000);
	if (global_entry->op_mode == WIFI_MODE_STA_ONLY) {
		for (wpa_s = global_entry->ifaces; wpa_s; wpa_s = wpa_s->next) {
			if (wpa_s->op_mode == WIFI_MODE_STA_ONLY) {
				if (wpa_s->wpa_state == WPA_INTERFACE_DISABLED) {
					*on_off = 0;
					return 0;
				} else {
					*on_off = 1;
					return 0;
				}
			}
		}
	} else if (global_entry->op_mode == WIFI_MODE_AP_ONLY) {
		wpa_s = wpa_supplicant_get_iface(global_entry, ap_ifname);
		if (wpa_s == NULL) {
			*on_off = 0;
			return 0;
		} else {
			*on_off = 1;
			return 0;
		}
	}
	return -1;
}

void mtk_supplicant_update_listen_interval(uint8_t interval)
{
	g_wifi_config_ext.sta_listen_interval = interval;
}

void mtk_supplicant_update_ps_mode(uint8_t mode)
{
	g_wifi_config_ext.sta_power_save_mode = mode;
}

void mtk_supplicant_set_passive_scan(uint8_t scan_option)
{

	struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return;
	}

	if (scan_option == 1)
		wpa_s->manual_scan_passive = 1;
	else
		wpa_s->manual_scan_passive = 0;
}

void mtk_supplicant_set_sched_scan(uint8_t mode)
{
	if (mode == 0)
		g_sched_scan_enabled = 0;
	else
		g_sched_scan_enabled = 1;
}


#ifdef CONFIG_BAND_PREFERENCE
static void mtk_supplicant_set_bss_preference(uint8_t mode)
{
	struct wpa_supplicant *wpa_s;

	wpa_s = mtk_supplicant_get_interface_by_port(WIFI_PORT_STA);
	if (wpa_s == NULL) {
		wpa_printf(MSG_ERROR, "%s: wpa_s get fail.", __FUNCTION__);
		return;
	}

	wpa_s->band_preference = mode;
}
#endif

void mtk_supplicant_radio_onoff(uint8_t radio_on)
{
	struct wpa_supplicant *wpa_s = NULL;

	supplicant_ready_query(TRUE, TRUE, 3000);

	for (wpa_s = global_entry->ifaces; wpa_s; wpa_s = wpa_s->next) {
		if (radio_on) {
			wpa_supplicant_event(wpa_s, EVENT_INTERFACE_ENABLED, NULL);
		} else {
			wpa_supplicant_cancel_sched_scan(wpa_s);
			wpa_supplicant_cancel_scan(wpa_s);
			mtk_freertos_abort_scan();
			wpa_supplicant_event(wpa_s, EVENT_INTERFACE_DISABLED, NULL);
		}
	}
}

#if MTK_WIFI_AP_ENABLE_FLAG
void mtk_supplicant_ap_onoff(uint8_t radio_on)
{
	/*
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	struct wpa_group * auth_group = NULL;
	struct hostapd_data *hapd = NULL;
	*/
	struct wpa_supplicant *wpa_s;
	struct wpa_interface *ifaces;
	char buffer[SSID_MAX_LEN];
	int errors = 0;
	uint32_t size = 0;

	if (radio_on) {
		ifaces = os_zalloc(sizeof(struct wpa_interface));

		if (ifaces == NULL) {
			wpa_printf(MSG_ERROR, "[ERROR]%s ifaces NULL", __func__);
			return;
		}
		wpa_printf(MSG_DEBUG, "add iface ra0");
		ifaces->bridge_ifname = NULL;
		ifaces->ctrl_interface = ctrl_interface;
		ifaces->driver = ctrl_interface;
		ifaces->ifname = "ra0";
		ifaces->confname = "ap_config";

		wpa_s = wpa_supplicant_add_iface(global_entry, ifaces, g_wpa_s);
		if (wpa_s == NULL)
			return;
		wpa_s->l2 = wpa_drv_get_socket(wpa_s);
		wpa_s->conf->ssid->mode = WPAS_MODE_AP;
		size = sizeof(buffer);
		if (hostapd_config_read("ssid", buffer, &errors, size)
			== 0) {
			wpa_s->conf->ssid->ssid_len = os_strlen(buffer);
			if (wpa_s->conf->ssid->ssid_len > SSID_MAX_LEN ||
				wpa_s->conf->ssid->ssid_len < 1) {
				wpa_printf(MSG_ERROR, "invalid SSID '%s'",
						buffer);
				errors++;
			} else {
				if (wpa_s->conf->ssid->ssid == NULL)
					wpa_s->conf->ssid->ssid = os_zalloc(WIFI_MAX_LENGTH_OF_SSID + 1);
				os_memcpy(wpa_s->conf->ssid->ssid, buffer, os_strlen(buffer));
				wpa_printf(MSG_ERROR, "SSID = '%s'", wpa_s->conf->ssid->ssid);
			}
		}
		wpa_s->conf->ssid->pairwise_cipher = WPA_CIPHER_NONE;
		netif_set_default(&ap_netif);
		netif_set_link_up(&ap_netif);
		wpa_supplicant_create_ap(wpa_s, wpa_s->conf->ssid);
		g_ap_created = 1;
		os_free(ifaces);
	} else {
		netif_set_link_down(&ap_netif);
		wpa_supplicant_entry_iface_remove(ap_ifname);
		g_ap_created = 0;
	}

    /*supplicant_ready_query(TRUE,TRUE,3000);
    if(wpa_s->ap_iface != NULL) {
        if(wpa_s->ap_iface->bss[0]) {
            hapd = wpa_s->ap_iface->bss[0];
            auth_group = hapd->wpa_auth->group;
            if (radio_on) {
                //currently empty
            } else {
                //AP off, FW will clear all keys include GMK, we need make sure AP is seen_none_sta state during auth after radio on.
                auth_group->first_sta_seen = FALSE;
            }
        }
    }*/
}
#endif

#define WSC_ID_DEVICE_PWD_ID        0x1012
#define WSC_ID_SELECTED_REGISTRAR       0x1041
#define WSC_ID_CONFIGURATION_METHODS    0x1008

/* WSC IE structure */
struct __wsc_ie_st {
	uint16_t    Type;
	uint16_t    Length;
	uint8_t         Data[1]; /* variable length data */
};


#ifdef __NOT_PORTED__
static void mtk_supplicant_check_wps_element_from_ap(
	void               *ie_buf,
	wifi_wps_element_t *wps_element)
{
	PEID_STRUCT        eid = (PEID_STRUCT)ie_buf;
	uint8_t            *data;
	int16_t            length = 0;
	struct __wsc_ie_st *p_wsc_ie;

	data   = (uint8_t *) eid->Octet + 4;
	length = (int16_t)(eid->Len - 4);

	while (length > 0) {
		struct __wsc_ie_st wsc_ie;
		os_memcpy(&wsc_ie, data, sizeof(struct __wsc_ie_st));

		/* Check for WSC IEs */
		p_wsc_ie = &wsc_ie;

		/* Check for device password ID, PIN = 0x0000, PBC = 0x0004 */
		if (be2cpu16(p_wsc_ie->Type) == WSC_ID_DEVICE_PWD_ID) {
			/* Found device password ID */
			os_memcpy(&(wps_element->device_password_id), data + 4, be2cpu16(p_wsc_ie->Length));
			wps_element->device_password_id = (wps_device_password_id_t)be2cpu16(wps_element->device_password_id);
			wpa_printf(MSG_EXCESSIVE, "%s: device_passwd_id = 0x%04x"
				   , __FUNCTION__, wps_element->device_password_id);
		}

		/* Check for device Selected Registrar IE, 1 means WPS triggered */
		if (be2cpu16(p_wsc_ie->Type) == WSC_ID_SELECTED_REGISTRAR) {
			/* Found ID */
			os_memcpy(&(wps_element->selected_registrar), data + 4, be2cpu16(p_wsc_ie->Length));
			//wps_element->selected_registrar = be2cpu16(wps_element->selected_registrar);
			wpa_printf(MSG_EXCESSIVE, "%s: selected_registrar = 0x%04x"
				   , __FUNCTION__, wps_element->selected_registrar);
		}

		/* Check for device configuration methods IE */
		if (be2cpu16(p_wsc_ie->Type) == WSC_ID_CONFIGURATION_METHODS) {
			/* Found ID */
			os_memcpy(&(wps_element->configuration_methods), data + 4, be2cpu16(p_wsc_ie->Length));
			wps_element->configuration_methods = (wps_config_method_t)be2cpu16(wps_element->configuration_methods);
			wpa_printf(MSG_EXCESSIVE, "%s: configuration_methods = 0x%04x"
				   , __FUNCTION__, wps_element->configuration_methods);
		}

		/* Set the offset and look for PBC information */
		/* Since Type and Length are both short type, we need to offset 4, not 2 */
		data   += (be2cpu16(p_wsc_ie->Length) + 4);
		length -= (be2cpu16(p_wsc_ie->Length) + 4);
	}

	return;
}
#endif // __NOT_PORTED__


#ifdef CONFIG_WPS
/*
    AP/APCLI Dual Interface doesn't need to support WPS function. @20151223
*/

int mtk_supplicant_connection_wps_pbc(uint8_t port, uint8_t *bssid)
{
	int ret = 0;
	struct wpa_supplicant *wpa_s = global_entry->ifaces;

	supplicant_ready_query(TRUE, TRUE, 3000);

#if defined(CONFIG_REPEATER)
	if ((wpa_s->op_mode != WIFI_MODE_STA_ONLY) &&
			(wpa_s->op_mode != WIFI_MODE_AP_ONLY)) {
		/*wps is onlybe triggered in station mode or ap mode*/
		wpa_printf(MSG_DEBUG, "op_mode not support", wpa_s->op_mode);
		return -1;
	}
#endif/*CONFIG_REPEATER*/

#ifdef CONFIG_AP
	if (wpa_s->ap_iface) {
#if defined(CONFIG_REPEATER)
		if ((port == WIFI_PORT_AP) && wpa_s->op_mode == WIFI_MODE_AP_ONLY)
#else
		if ((port == WIFI_PORT_AP) && global_entry->op_mode == WIFI_MODE_AP_ONLY)
#endif
			return wpa_supplicant_ap_wps_pbc(wpa_s, bssid, bssid);
	}
#endif /* CONFIG_AP */

	//wifi_scan_reset();
#if defined(CONFIG_REPEATER)
	if ((port == WIFI_PORT_STA) && wpa_s->op_mode == WIFI_MODE_STA_ONLY)
#else
	if ((port == WIFI_PORT_STA) && global_entry->op_mode == WIFI_MODE_STA_ONLY)
#endif
	{
		ret = wpas_wps_start_pbc(wpa_s, bssid, 0);
		g_scan_pbc_ap = 1;
		os_memcpy(&target_bssid[0], bssid, ETH_ALEN);
		return ret;
	} else {
		/*wps PBC mode input port is not match with the current interface*/
		wpa_printf(MSG_DEBUG, "Port not match Interface");
		return -1;
	}
}

int mtk_supplicant_connection_wps_pin(uint8_t port, uint8_t *bssid, uint8_t *pin_code)
{
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	int ret;
	unsigned char buf[9] = {0};
	int buflen = 9;

	supplicant_ready_query(TRUE, TRUE, 3000);

#if defined(CONFIG_REPEATER)
	if ((wpa_s->op_mode != WIFI_MODE_STA_ONLY) &&
			(wpa_s->op_mode != WIFI_MODE_AP_ONLY)) {
		/*wps is onlybe triggered in station mode or ap mode*/
		wpa_printf(MSG_DEBUG, "op_mode not support", wpa_s->op_mode);
		return -1;
	}
#endif/*CONFIG_REPEATER*/

#ifdef CONFIG_AP
	if (wpa_s->ap_iface) {
#if defined(CONFIG_REPEATER)
		if ((port == WIFI_PORT_AP) && wpa_s->op_mode == WIFI_MODE_AP_ONLY)
#else
		if ((port == WIFI_PORT_AP) && global_entry->op_mode == WIFI_MODE_AP_ONLY)
#endif
		{
			return ret;
		}
	}
#endif /* CONFIG_AP */

#if defined(CONFIG_REPEATER)
	if ((port == WIFI_PORT_STA) && wpa_s->op_mode == WIFI_MODE_STA_ONLY)
#else
	if ((port == WIFI_PORT_STA) && global_entry->op_mode == WIFI_MODE_STA_ONLY)
#endif
	{
		if (pin_code == NULL) {
			/* Return the generated PIN */
			ret = os_snprintf((char *)buf, buflen, "%08d", ret);
			if (ret < 0 || ret >= buflen) {
				if (ret >= buflen) {
					wpa_printf(MSG_ERROR, "\n\n[ERROR]: %s(line.%d) why memory overflow?? ret=%d, buflen=%d\n\n",
						   __FUNCTION__, __LINE__, ret, buflen);
				}
				return -1;
			}
			pin_code = buf;
		}

		if (pin_code) {
			wpa_printf(MSG_DEBUG, "\nWPS PIN:%s\n", pin_code);
			ret = wpas_wps_start_pin(wpa_s, bssid, (const char *)pin_code, 0,
						 DEV_PW_DEFAULT);
			os_memcpy(&target_bssid[0], bssid, ETH_ALEN);
			if (ret < 0) {
				return -1;
			}
			os_memcpy((void *)buf, pin_code, strlen((const char *)pin_code));
			return 0;
		}
	} else {
		wpa_printf(MSG_DEBUG, "Port not match Interface");
		return -1;
	}
	return 0;
}

int mtk_supplicatn_wps_get_pin(uint8_t port, uint8_t *pin_code)
{
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	int ret;
	int buflen = 9;
	const char *temp_pin;

	supplicant_ready_query(TRUE, TRUE, 3000);

#if defined(CONFIG_REPEATER)
	if ((wpa_s->op_mode != WIFI_MODE_STA_ONLY) &&
			(wpa_s->op_mode != WIFI_MODE_AP_ONLY)) {
		/*wps is onlybe triggered in station mode or ap mode*/
		wpa_printf(MSG_DEBUG, "op_mode = %d not support", wpa_s->op_mode);
		return -1;
	}
#endif/*CONFIG_REPEATER*/

#ifdef CONFIG_AP
	if (wpa_s->ap_iface) {
#if defined(CONFIG_REPEATER)
		if ((port == WIFI_PORT_AP) && wpa_s->op_mode == WIFI_MODE_AP_ONLY)
#else
		if ((port == WIFI_PORT_AP) && global_entry->op_mode == WIFI_MODE_AP_ONLY)
#endif
		{
			wpa_printf(MSG_DEBUG, "AP Interface");
			temp_pin = wpas_wps_ap_pin_get(wpa_s);
			if ((temp_pin == NULL) || strlen(temp_pin) >= buflen) {
				wpa_printf(MSG_DEBUG, "AP PIN NULL or Lenth ERROR");
				return -1;
			}
			os_memcpy(pin_code, temp_pin, buflen);
			return 0;
		}
	}
#endif

#if defined(CONFIG_REPEATER)
	if ((port == WIFI_PORT_STA) && wpa_s->op_mode == WIFI_MODE_STA_ONLY)
#else
	if ((port == WIFI_PORT_STA) && global_entry->op_mode == WIFI_MODE_STA_ONLY)
#endif
	{
		wpa_printf(MSG_DEBUG, "STA Interface");

		ret = os_snprintf((char *)pin_code, buflen, "%08d", ret);

		if (ret < 0 || ret >= buflen) {
			if (ret >= buflen) {
				wpa_printf(MSG_ERROR, "\n\n[ERROR]: %s(line.%d) why memory overflow?? ret=%d, buflen=%d\n\n",
					   __FUNCTION__, __LINE__, ret, buflen);
			}
			return -1;
		}
	} else {
		wpa_printf(MSG_DEBUG, "Port not match with Interface");
		return -1;
	}

	return 0;
}

int mtk_supplicant_config_set_wps_auto_connection(bool on_off)
{
	LOG_I(minisupp, "mtk_supplicant_config_set_wps_auto_connection on_off=%d\n", on_off);
	wps_auto_connection_ctrl = on_off;
	return 0;
}

int mtk_supplicant_config_get_wps_auto_connection(bool *on_off)
{
	LOG_I(minisupp, "mtk_supplicant_config_get_wps_auto_connection\n");
	*on_off = wps_auto_connection_ctrl;
	return 0;
}


int mtk_supplicant_ctrl_wps_auto_conneciton(char *cmd)
{
	int ret;
	bool connection = 0;
	wpa_printf(MSG_DEBUG, "wps_auto_conneciton: cmd '%s'",
		   cmd);

	if (os_strcmp(cmd, "on") == 0) {
		LOG_I(minisupp, "enable auto connection\n");
		ret = wifi_wps_config_set_auto_connection(1);
	} else if (os_strcmp(cmd, "off") == 0) {
		LOG_I(minisupp, "disable auto connection\n");
		ret = wifi_wps_config_set_auto_connection(0);
	} else if (os_strcmp(cmd, "get") == 0) {
		ret = wifi_wps_config_get_auto_connection(&connection);
		LOG_W(minisupp, "wps_auto_connection status(1:on,0:off): %d \n", connection);
	} else {
		LOG_W(minisupp, "Unknown cmd\n");
		ret = -1;
	}
	return ret;
}

int mtk_supplicant_ctrl_wps_set_credential_handler(char *cmd)
{
	int ret;
	wpa_printf(MSG_DEBUG, "wps_set_credential_handler: cmd '%s'",
		   cmd);

	if (os_strcmp(cmd, "register") == 0) {
		LOG_I(minisupp, "register credential handler\n");
		ret = wifi_wps_register_credential_handler();
	} else if (os_strcmp(cmd, "unregister") == 0) {
		LOG_I(minisupp, "unregister credential handler\n");
		ret = wifi_wps_unregister_credential_handler();
	} else {
		ret = -1;
		LOG_W(minisupp, "Unknown cmd\n");
	}

	return ret;

}


/*
    AP/APCLI Dual Interface doesn't need to support WPS function. @20151223
*/

/*
    cmd format:
    "00:11:22:33:44:55" or "any"
*/
static int mtk_supplicant_ctrl_wps_pbc(char *cmd)
{
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	u8 bssid[ETH_ALEN], *_bssid = bssid;
#ifdef CONFIG_P2P
	u8 p2p_dev_addr[ETH_ALEN];
#endif /* CONFIG_P2P */
#ifdef CONFIG_AP
#ifdef CONFIG_P2P
	u8 *_p2p_dev_addr = NULL;
#endif
#endif /* CONFIG_AP */

	if (cmd == NULL || os_strcmp(cmd, "any") == 0) {
		_bssid = NULL;
#ifdef CONFIG_P2P
	} else if (os_strncmp(cmd, "p2p_dev_addr=", 13) == 0) {
		if (hwaddr_aton(cmd + 13, p2p_dev_addr)) {
			wpa_printf(MSG_DEBUG, "CTRL_IFACE WPS_PBC: invalid "
				   "P2P Device Address '%s'",
				   cmd + 13);
			return -1;
		}
		_p2p_dev_addr = p2p_dev_addr;
#endif /* CONFIG_P2P */
	} else if (hwaddr_aton(cmd, bssid)) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE WPS_PBC: invalid BSSID '%s'",
			   cmd);
		return -1;
	}

#ifdef CONFIG_AP
	if (wpa_s->ap_iface) {
	}
#endif /* CONFIG_AP */
	return 0;

}

/*
    cmd format:
    "00:11:22:33:44:55 12345670" or "00:11:22:33:44:55"

    buf: pin
    buflen: pin len
*/
static int mtk_supplicant_ctrl_wps_pin(char *cmd, char *buf,
				       int buflen)
{
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	u8 bssid[ETH_ALEN] = {0}, *_bssid = bssid;
	char *pin = NULL, new_pin[9];

	if (cmd == NULL) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE WPS_PIN: invalid cmd '%s'",
			   cmd);
		return -1;
	}

	pin = os_strchr(cmd, ' ');
	if (pin) {
		*pin++ = '\0';

		os_memset(new_pin, 0, 9);
		ignore_spaces_dash(pin, new_pin);

		pin = new_pin;
		if ((strlen(pin) != 8)
				&& (strlen(pin) != 4)) {
			wpa_printf(MSG_ERROR, "CTRL_IFACE WPS_PIN: incorrect PIN '%s'=(%d)",
				   pin, strlen(pin));
			return -1;
		}
	}

	if (os_strcmp(cmd, "any") == 0) {
		_bssid = NULL;
	} else if (hwaddr_aton(cmd, bssid)) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE WPS_PIN: invalid BSSID '%s'",
			   cmd);
		return -1;
	}

#ifdef CONFIG_AP
	if (wpa_s->ap_iface) {
	}
#endif/* CONFIG_AP */
	return 0;

}

static int mtk_supplicant_ctrl_wps_reg(char *cmd)
{
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	u8 bssid[ETH_ALEN];
	char *pin;
	char *new_ssid;
	char ssid_hex[2 * 32 + 1];
	char *new_auth;
	char *new_encr;
	char *new_key;
	char key_hex[2 * 64 + 1];
	int i;
	struct wps_new_ap_settings ap;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE WPS_REG: cmd '%s'",
		   cmd);

	pin = os_strchr(cmd, ' ');
	if (pin == NULL) {
		return -1;
	}
	*pin++ = '\0';

	if (hwaddr_aton(cmd, bssid)) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE WPS_REG: invalid BSSID '%s'",
			   cmd);
		return -1;
	}

	new_ssid = os_strchr(pin, ' ');
	if (new_ssid == NULL) {
		return wpas_wps_start_reg(wpa_s, bssid, pin, NULL);
	}
	*new_ssid++ = '\0';

	new_auth = os_strchr(new_ssid, ' ');
	if (new_auth == NULL) {
		return -1;
	}
	*new_auth++ = '\0';

	new_encr = os_strchr(new_auth, ' ');
	if (new_encr == NULL) {
		return -1;
	}
	*new_encr++ = '\0';

	new_key = os_strchr(new_encr, ' ');
	if (new_key == NULL) {
		return -1;
	}
	*new_key++ = '\0';

	wpa_printf(MSG_DEBUG, "%s %s %s %s %s\n",
		   pin,
		   new_ssid,
		   new_auth,
		   new_encr,
		   new_key);

	os_memset(&ssid_hex[0], 0, sizeof(ssid_hex));
	ssid_hex[0] = '\0';
	for (i = 0; i < 32; i++) {
		if (new_ssid[i] == '\0') {
			break;
		}
		os_snprintf(&ssid_hex[i * 2], 3, "%02x", new_ssid[i]);
	}

	os_memset(&key_hex[0], 0, sizeof(key_hex));
	key_hex[0] = '\0';
	for (i = 0; i < 64; i++) {
		if (new_key[i] == '\0') {
			break;
		}
		os_snprintf(&key_hex[i * 2], 3, "%02x",
			    new_key[i]);
	}

	os_memset(&ap, 0, sizeof(ap));
	ap.ssid_hex = ssid_hex;
	ap.auth = new_auth;
	ap.encr = new_encr;
	ap.key_hex = key_hex;
	return wpas_wps_start_reg(wpa_s, bssid, pin, &ap);
}


#ifdef CONFIG_AP
static int mtk_supplicant_ctrl_wps_ap_pin(char *cmd, char *buf,
		int buflen)
{
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	char *pos;
	int ret = -1;
	int timeout = 300;
	const char *pin_txt;

	if (!wpa_s->ap_iface) {
		wpa_printf(MSG_DEBUG, "No AP Interface");
		return -1;
	}

	pos = os_strchr(cmd, ' ');
	if (pos) {
		*pos++ = '\0';
	}

	if (os_strcmp(cmd, "disable") == 0) {
		wpas_wps_ap_pin_disable(wpa_s);
		return os_snprintf(buf, buflen, "OK\n");
	}

	if (os_strcmp(cmd, "random") == 0) {
		if (pos) {
			timeout = atoi(pos);
		}
		pin_txt = wpas_wps_ap_pin_random(wpa_s, timeout);
		if (pin_txt == NULL) {
			wpa_printf(MSG_DEBUG, "random pin is null.");
			return -1;
		}
		return os_snprintf(buf, buflen, "%s", pin_txt);
	}

	if (os_strcmp(cmd, "get") == 0) {
		pin_txt = wpas_wps_ap_pin_get(wpa_s);
		if (pin_txt == NULL) {
			return -1;
		}
		return os_snprintf(buf, buflen, "%s", pin_txt);
	}

	if (os_strcmp(cmd, "set") == 0) {
		char *pin;
		if (pos == NULL) {
			return -1;
		}
		pin = pos;
		pos = os_strchr(pos, ' ');
		if (pos) {
			*pos++ = '\0';
			timeout = atoi(pos);
		}
		if (os_strlen(pin) > buflen) {
			wpa_printf(MSG_DEBUG, "os_strlen(pin) > buflen");
			return -1;
		}
		ret = wpas_wps_ap_pin_set(wpa_s, pin, timeout);
		if (ret < 0) {
			return ret;
		}
		return os_snprintf(buf, buflen, "%s", pin);
	}

	return ret;
}

void mtk_supplicant_ctrl_wps_ap_reload_config(
	void *eloop_data, void *user_ctx)
{
	struct wps_context *wps = eloop_data;
	uint8_t auth_mode = 0, encrypt_type = 0;
	uint8_t wep_mode = 0;

	wpa_printf(MSG_DEBUG, "%s", __FUNCTION__);
	switch (wps->auth_types) {
	case WPS_AUTH_OPEN:
		auth_mode = WIFI_AUTH_MODE_OPEN;
		break;
	case WPS_AUTH_WPAPSK:
		auth_mode = WIFI_AUTH_MODE_WPA_PSK;
		break;
	case WPS_AUTH_WPA2PSK:
		auth_mode = WIFI_AUTH_MODE_WPA2_PSK;
		break;
	case WPS_AUTH_WPA:
		auth_mode = WIFI_AUTH_MODE_WPA;
		break;
	case WPS_AUTH_WPA2:
		auth_mode = WIFI_AUTH_MODE_WPA2;
		break;
	case (WPS_AUTH_WPAPSK | WPS_AUTH_WPA2PSK):
		auth_mode = WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
		break;
	case (WPS_AUTH_WPA | WPS_AUTH_WPA2):
		auth_mode = WIFI_AUTH_MODE_WPA_WPA2;
		break;
	default:
		wpa_printf(MSG_ERROR, "%s: Unkown auth_types(=0x%04x)",
			   __FUNCTION__, wps->auth_types);
		return;
	}

	switch (wps->encr_types) {
	case WPS_ENCR_NONE:
		encrypt_type = WIFI_ENCRYPT_TYPE_ENCRYPT_DISABLED;
		break;
	case WPS_ENCR_WEP:
		wep_mode = 1;
		encrypt_type = WIFI_ENCRYPT_TYPE_WEP_ENABLED;
		break;
	case WPS_ENCR_TKIP:
		encrypt_type = WIFI_ENCRYPT_TYPE_TKIP_ENABLED;
		break;
	case WPS_ENCR_AES:
		encrypt_type = WIFI_ENCRYPT_TYPE_AES_ENABLED;
		break;
	case (WPS_ENCR_TKIP | WPS_ENCR_AES):
		encrypt_type = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;
		break;
	default:
		wpa_printf(MSG_ERROR, "%s: Unkown encr_types(=0x%04x)",
			   __FUNCTION__, wps->encr_types);
		return;
	}

	mtk_supplicant_set_security(WIFI_PORT_AP, auth_mode, encrypt_type);
	mtk_supplicant_set_ssid(WIFI_PORT_AP, wps->ssid, wps->ssid_len);
	wpa_supplicant_reload_configuration(global_entry->ifaces);
	return;
}

#endif /* CONFIG_AP */

static int mtk_supplicant_ctrl_wps_get_pin(char *cmd, char *buf,
		int buflen)
{
	struct wpa_supplicant *wpa_s = global_entry->ifaces;
	char *pos;
	int ret = -1;

	const char *pin_get;
	char pin_buf[9] = {0};
	pin_get = pin_buf;

	pos = os_strchr(cmd, ' ');
	if (pos) {
		*pos++ = '\0';
	}

#ifdef CONFIG_AP
	if (wpa_s->ap_iface) {
		if (os_strcmp(cmd, "ap") == 0) {
			if (ret != 0) {
				return -1;
			}
			return os_snprintf(buf, buflen, "%s", pin_get);
		}
	}
#endif/* CONFIG_AP */

	if (os_strcmp(cmd, "sta") == 0) {
		if (ret != 0) {
			return -1;
		}
		return os_snprintf(buf, buflen, "%s", pin_get);
	}

	wpa_printf(MSG_ERROR, "%s: unknown or unmatch cli commond.", __FUNCTION__);
	return -1;
}

static void mtk_supplicant_check_dpid_from_wps_ap(
	void *ie_buf,
	uint16_t *device_passwd_id)
{
	PEID_STRUCT eid = (PEID_STRUCT)ie_buf;
	uint8_t *data;
	uint16_t length = 0;
	struct __wsc_ie_st *p_wsc_ie;

	data = (uint8_t *) eid->Octet + 4;
	length = (uint16_t)(eid->Len - 4);

	while (length > 0) {
		struct __wsc_ie_st wsc_ie;
		os_memcpy(&wsc_ie, data, sizeof(struct __wsc_ie_st));

		/* Check for WSC IEs */
		p_wsc_ie = &wsc_ie;

		/* Check for device password ID, PIN = 0x0000, PBC = 0x0004 */
		if (be2cpu16(p_wsc_ie->Type) == WSC_ID_DEVICE_PWD_ID) {
			/* Found device password ID */
			os_memcpy(device_passwd_id, data + 4, sizeof(uint16_t));
			*device_passwd_id = be2cpu16(*device_passwd_id);
			wpa_printf(MSG_EXCESSIVE, "%s: device_passwd_id = 0x%04x"
				   , __FUNCTION__, *device_passwd_id);
		}

		/* Set the offset and look for PBC information */
		/* Since Type and Length are both short type, we need to offset 4, not 2 */
		data += (be2cpu16(p_wsc_ie->Length) + 4);
		length   -= (be2cpu16(p_wsc_ie->Length) + 4);
	}

	return;
}

#endif /* CONFIG_WPS */


#ifdef __NOT_PORTED__
static int mtk_ap_get_sta(char *mac_addr)
{
	return 0;
}
#endif // __NOT_PORTED__


#define MAC_ADDR_IS_GROUP(Addr)    (((Addr[0]) & 0x01))
int mtk_ctrl_get_sta_qos_bit(int32_t inf_num, char *mac_addr)
{
	return 0;
}

void mtk_supplicant_show_interfaces()
{
	wpa_printf(MSG_DEBUG, "mtk_supplicant_show_interfaces \n");
}

int mtk_supplicant_get_bssid(uint8_t *bssid)
{
	return 0;
}

int mtk_supplicant_set_ssid_bssid(unsigned char *ssid, unsigned int ssidlen, uint8_t *bssid)
{
	return 0;
}

#if defined(CONFIG_REPEATER)

int32_t mtk_supplicant_scan_in_repeater_mode(void)
{
	return 0;
}
#endif

#if defined(MTK_WIFI_DIRECT_ENABLE)
static int mtk_supplicant_freq_to_channel(const char *country, unsigned int freq, unsigned char *reg_class,
		int *channel)
{
	/* TODO: more operating classes */
	if (freq >= 2412 && freq <= 2472) {
		*reg_class = 81; /* 2.407 GHz, channels 1..13 */
		*channel = (freq - 2407) / 5;
		return 0;
	}

	if (freq == 2484) {
		*reg_class = 82; /* channel 14 */
		*channel = 14;
		return 0;
	}

	if (freq >= 5180 && freq <= 5240) {
		*reg_class = 115; /* 5 GHz, channels 36..48 */
		*channel = (freq - 5000) / 5;
		return 0;
	}

	if (freq >= 5745 && freq <= 5805) {
		*reg_class = 124; /* 5 GHz, channels 149..161 */
		*channel = (freq - 5000) / 5;
		return 0;
	}

	return -1;
}


#define P2P_MAX_SEARCH_CHANNEL_NUM 5

static int mtk_supplicant_freqs_to_channels(int *freqs, int *channels)
{
	int channelCount;
	int totalNumOfCh = 0;

	for (channelCount = 0 ; channelCount < P2P_MAX_SEARCH_CHANNEL_NUM ; channelCount++) {
		mtk_supplicant_freq_to_channel(NULL, freqs[channelCount], NULL, channels + channelCount);
		if (freqs[channelCount] == 0) {
			totalNumOfCh = channelCount;
			break;
		}
	}
	wpa_printf(MSG_DEBUG, "totalNumOfCh = %d\n", totalNumOfCh);
	return totalNumOfCh;
}

ch_list_t *g_search_channel_list = NULL;
static void mtk_supplicant_generate_channel_list(int *channels, int totalNumOfCh)
{
	int bandcount;

	if (g_search_channel_list != NULL) {
		os_free(g_search_channel_list);
		g_search_channel_list = NULL;
	}

	g_search_channel_list = (ch_list_t *)os_malloc(sizeof(ch_list_t) + totalNumOfCh * sizeof(ch_desc_t));

	g_search_channel_list->num_bg_band_triplet = totalNumOfCh;
	g_search_channel_list->num_a_band_triplet = 0;

	for (bandcount = 0; bandcount < totalNumOfCh; bandcount++) {
		g_search_channel_list->triplet[bandcount].first_channel = channels[bandcount];
		g_search_channel_list->triplet[bandcount].num_of_ch = 1;
		g_search_channel_list->triplet[bandcount].channel_prop = 0;
	}
	return;
}
/**
* @brief scan multiple channel
*
* @param [IN]ssid SSID
* @param [IN]ssid_length Length of SSID
* @param [IN]bssid BSSID
* @param [IN]scan_mode
* @param 0 full
* @param 1 partial
* @param [IN]scan_option
* @param 0: active (passive in regulatory channel)
* @param 1: passive in all channel
* @param 2: force_active (active in all channel)
*
* @return  >=0 means success, <0 means fail
*
* @note When SSID/BSSID specified (not NULL)
* @note 1. ProbeReq carries SSID (for Hidden AP)
* @note 2. Scan Table always keeps the APs of this specified SSID/BSSID
*/
static int mtk_supplicant_scan_multiple_channels(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid,
		uint8_t scan_mode, uint8_t scan_option, int *freqs)
{
	INT32 status = 0, band_count = 0;
	int32_t len_param = 0;
	P_CMD_IOT_START_SCAN_T scan_param = NULL;

	int channels[P2P_MAX_SEARCH_CHANNEL_NUM] = {0};
	int totalNumOfCh = 0;
	/*Generate the channel list*/
	totalNumOfCh = mtk_supplicant_freqs_to_channels(freqs, channels);
	mtk_supplicant_generate_channel_list(channels, totalNumOfCh);

	if (g_search_channel_list != NULL) {
		len_param = (g_search_channel_list->num_a_band_triplet + g_search_channel_list->num_bg_band_triplet) * sizeof(ch_desc_t);
	}
	len_param += sizeof(CMD_IOT_START_SCAN_T);
	scan_param = (P_CMD_IOT_START_SCAN_T)os_malloc(len_param);
	if (scan_param == NULL) {
		LOG_I(minisupp, "malloc memory fail.\n");
		return -1;
	}
	// default scan param
	os_memset((unsigned char *)(scan_param), 0x0, len_param);

	if (ssid && (ssid_length > 0)) {
		os_memcpy(scan_param->SSID, ssid, ssid_length);
		scan_param->enable_ssid = (0x80 | ssid_length);
	}

	if (bssid) {
		os_memcpy(scan_param->BSSID, bssid, WIFI_MAC_ADDRESS_LENGTH);
		scan_param->enable_bssid = 1;
	}

	if (scan_mode > 0) {
		scan_param->partial = 1;
		scan_param->partial_bcn_cnt = 1;
	}

	/*send channel list. */
	if (g_search_channel_list != NULL) {
		(scan_param->channel_list).num_a_band_triplet = g_search_channel_list->num_a_band_triplet;
		(scan_param->channel_list).num_bg_band_triplet = g_search_channel_list->num_bg_band_triplet;

		for (band_count = 0 ; band_count < (g_search_channel_list->num_a_band_triplet +
						    g_search_channel_list->num_bg_band_triplet) ; band_count++) {
			(scan_param->channel_list).triplet[band_count].first_channel = g_search_channel_list->triplet[band_count].first_channel;
			(scan_param->channel_list).triplet[band_count].num_of_ch = g_search_channel_list->triplet[band_count].num_of_ch;
			(scan_param->channel_list).triplet[band_count].channel_prop = g_search_channel_list->triplet[band_count].channel_prop;
		}

	}
	scan_param->scan_mode = scan_option;
	status = inband_queue_exec(WIFI_COMMAND_ID_IOT_START_SCAN, (unsigned char *)(scan_param), len_param, TRUE, FALSE);

	if (g_search_channel_list != NULL) {
		os_free(g_search_channel_list);
		g_search_channel_list = NULL;
	}

	os_free(scan_param);
	scan_param = NULL;
	if (status == 0) {
		g_scanning = 1;
	}
	return status;
}
#endif /*MTK_WIFI_DIRECT_ENABLE)*/


#ifdef __NOT_PORTED__
static void mtk_supplicant_disable_trigger_scan()
{
}
#endif // __NOT_PORTED__


#ifdef __NOT_PORTED__
static void mtk_supplicant_enable_trigger_scan()
{
}
#endif // __NOT_PORTED__


#ifdef __NOT_PORTED__
static void mtk_supplicant_req_scan(int sec, int usec)
{
	struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
	if (wpa_s)
		wpa_supplicant_req_scan(wpa_s, sec, usec);
}
#endif // __NOT_PORTED__


static void handle_read(void *ctx, const u8 *src, const u8 *data, size_t len)
{
	struct wpa_driver_gen4m_data *inband_drv = ctx;
	drv_event_eapol_rx(inband_drv->ctx, src, data, len);
}

#ifdef HOSTAPD
static void *inband_driver_hapd_init(struct hostapd_data *hapd,
				     struct wpa_init_params *params)
{
	struct wpa_driver_gen4m_data *inband_drv;
	//struct inband_driver_data *drv;
	inband_drv = os_zalloc(sizeof(struct wpa_driver_gen4m_data));
	if (inband_drv == NULL) {
		wpa_printf(MSG_ERROR, "Could not allocate memory for iot driver data");
		return NULL;
	}
	/* Init L2 Packet Socket */

	hapd->l2 = l2_packet_init("ra0", NULL, ETH_P_EAPOL,
				  handle_read, inband_drv, 1);

	//  g_ethernetif_eapol_rx_socket = hapd->l2->fd;
	//  printf(">> init hapd [rx socket=%d]\n", g_ethernetif_eapol_rx_socket);
	inband_drv->hapd = hapd;
	inband_drv->ctx = hapd;

	return inband_drv;
}


static void inband_driver_hapd_deinit(void *priv)
{
	struct inband_driver_data *drv = priv;

	os_free(drv);
}
#endif /* HOSTAPD */


/* L2 packet eloop callback ... Michael  */

void l2_packet_receive(int sock, void *eloop_ctx, void *sock_ctx)
{
	struct l2_packet_data *l2 = eloop_ctx;
	struct sockaddr_in addr;
	unsigned char buf[1024];
	int len;
	socklen_t fromlen = sizeof(addr);

	len = recvfrom(sock, buf, sizeof(buf), 0,
		       (struct sockaddr *)&addr, &fromlen);
	wpa_printf(MSG_DEBUG, "wpa_supplicant l2_packet rev  EAPOL \n");
	if (len <= 16) {
		wpa_printf(MSG_DEBUG, "recv (len=%d)", len);
		if (len >= 0)
			wpa_hexdump(MSG_ERROR, "packet:", (buf), len);
		return;
	}
	wpa_printf(MSG_DEBUG, "EAPOL len(%d)", len);
	wpa_hexdump(MSG_DEBUG, "l2_packet_receive [EAPOL-header]", (buf), 32);
	if (l2->rx_callback != NULL)
		l2->rx_callback(l2->rx_callback_ctx, &(buf[6]), &(buf[14]), (len - 14));
}
static void gen4m_register_supplicant_set_api()
{
#if 0
	__g_wpa_supplicant_api.get_bssid = mtk_supplicant_get_bssid;
	__g_wpa_supplicant_api.set_ssid_bssid = mtk_supplicant_set_ssid_bssid;
#endif
}

/*scan table age out is used to update scan list in supplicant before, but we can
do update in supplicant autoically now, so delete the useless code*/


/* Michael: for supplicant: ctx=(wpa_supplicant *)
                 for hostapd:   ctx=(hostapd_data *)
                 you can trace wpa_supplicant_event() in different build
*/

static void *wpa_driver_gen4m_driver_init(void *ctx, const char *ifname)
{
	struct wpa_driver_gen4m_data *gen4m_drv;
	uint8_t mac_addr[ETH_ALEN] = {0x00, 0x0c, 0x43, 0x76, 0x86, 0x09};
	struct sockaddr_in driver_msg_sock_addr;
#ifdef CONFIG_SAE
	struct wpa_supplicant *wpa_s = ctx;
#endif
	struct timeval timeout;

	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;

	/*
	 * allocate inband driver control structure
	 */

	gen4m_drv = os_zalloc(sizeof(struct wpa_driver_gen4m_data));

	if (gen4m_drv == NULL) {
		wpa_printf(MSG_ERROR, "Could not allocate memory for iot driver data");
		return NULL;
	}

	/*
	 * keep the context from caller into belly.
	 */
	gen4m_drv->ctx  = ctx;

#ifdef CONFIG_SAE
	wpa_s->drv_flags |= WPA_DRIVER_FLAGS_SAE;
#endif
#ifdef CONFIG_IEEE80211R
	wpa_s->drv_flags |= WPA_DRIVER_FLAGS_UPDATE_FT_IES;
#endif
	/*
	 * prepare eloop socket for driver events, bound on 127.0.0.1.
	 */

	os_memset(&driver_msg_sock_addr, 0, sizeof(driver_msg_sock_addr));

	if (os_strstr(ifname, "wlan0")) {
		driver_msg_sock_addr.sin_family = AF_INET;
		driver_msg_sock_addr.sin_addr.s_addr = htonl((127 << 24) | 1);
		gen4m_drv->driver_msg_sock = socket(PF_INET, SOCK_DGRAM, 0);
		wpa_printf(MSG_DEBUG, "gen4m_drv->driver_msg_sock wlan0 %d\r\n",
			   gen4m_drv->driver_msg_sock);
		gen4m_drv->driver_msg_port = htons(DRIVER_MSG_PORT_STA_ONLY);
		driver_msg_sock_addr.sin_port = htons(DRIVER_MSG_PORT_STA_ONLY);
		register_process_global_event(gen4m_drv, process_global_event, WIFI_MODE_STA_ONLY);
	} else {
		driver_msg_sock_addr.sin_family = AF_INET;
		driver_msg_sock_addr.sin_addr.s_addr = htonl((127 << 24) | 1);
		gen4m_drv->driver_msg_sock = socket(PF_INET, SOCK_DGRAM, 0);
		wpa_printf(MSG_DEBUG, "gen4m_drv->driver_msg_sock p2p0 %d\n",
			   gen4m_drv->driver_msg_sock);
		gen4m_drv->driver_msg_port = htons(DRIVER_MSG_PORT_AP_ONLY);
		driver_msg_sock_addr.sin_port       = htons(DRIVER_MSG_PORT_AP_ONLY);

		register_process_global_event(gen4m_drv, process_global_event, WIFI_MODE_AP_ONLY);
	}

	if (bind(gen4m_drv->driver_msg_sock,
			(struct sockaddr *)&driver_msg_sock_addr,
			sizeof(driver_msg_sock_addr)) < 0) {
		wpa_printf(MSG_ERROR, "fail to bind socket.");
	}

	if (setsockopt(gen4m_drv->driver_msg_sock, SOL_SOCKET, SO_RCVTIMEO,
			(char *)&timeout, sizeof(timeout)) < 0) {
		wpa_printf(MSG_ERROR, "error set socket timeout");
	}

	eloop_register_read_sock(gen4m_drv->driver_msg_sock,
				 driver_msg_recv, NULL, gen4m_drv);

#ifdef MTK_MINISUPP_ENABLE
	/*
	 * register to wifi_event.c for driver events.
	 *
	 * The callback handler is: process_global_event. It forwards the received
	 * event callback to eloop as messages.
	 */
#if 0
	register_process_global_event(inband_drv,
				      process_global_event,
				      global_entry->op_mode);
#endif
#endif

	if (os_strstr(ifname, sta_ifname)) {
		wifi_config_get_mac_address(WIFI_PORT_STA, mac_addr);
		os_memcpy(gen4m_drv->own_addr, mac_addr, ETH_ALEN);
	}
#ifdef CONFIG_AP
	else if (os_strstr(ifname, ap_ifname)) {
		wifi_config_get_mac_address(WIFI_PORT_AP, mac_addr);
		os_memcpy(gen4m_drv->own_addr, mac_addr, ETH_ALEN);
	}
#endif

	wpa_printf(MSG_DEBUG, "%s: ifname = %s, gen4m_drv: %p, gen4m_drv->ctx: %p\n", __FUNCTION__, ifname, gen4m_drv, gen4m_drv->ctx);

	gen4m_drv->sock_recv = l2_packet_init(ifname, (gen4m_drv->own_addr), ETH_P_EAPOL, handle_read, gen4m_drv, 1);

	gen4m_register_supplicant_set_api();

	return gen4m_drv;
}

static int wpa_driver_gen4m_driver_send_eapol(void      *priv,
		const u8    *dest,
		u16      proto,
		const u8    *data,
		size_t    data_len)
{
	return -1;
}

static void *wpa_driver_gen4m_global_init(void *ctx)
{
	struct gen4m_global *global;

	global = os_zalloc(sizeof(*global));
	if (global == NULL)
		return NULL;
	global->ctx = ctx;
	global->ioctl_sock = -1;
	dl_list_init(&global->interfaces);
	global->if_add_ifindex = -1;
	global->drv_priv = mtk_freertos_wpa_global_init(ctx);
	wpa_printf(MSG_DEBUG, "%s: driver init %p\n", __func__, global->drv_priv);
	return global;
}

static void wpa_driver_gen4m_global_deinit(void *priv)
{
	mtk_freertos_wpa_global_deinit(priv);
	wpa_printf(MSG_DEBUG, "%s: driver de-init\n", __func__);
}
/**
 * wpa_driver_nl80211_get_scan_results - Fetch the latest scan results
 * @priv: Pointer to private wext data from wpa_driver_nl80211_init()
 * Returns: Scan results on success, -1 on failure
 */
static struct wpa_scan_results *wpa_driver_gen4m_get_scan_results(void *priv)
{
#if 1
	int num = 0;
	return mtk_freertos_wpa_scan_results(priv, &num);
#else
	struct wpa_scan_results *res = NULL;
	struct wpa_scan_res *scan_res;
	int i;
	unsigned char scan_result_num = 0, scan_table_size = 0;
	SCAN_ARRAY_ITEM *scan_table_array = NULL;
#if 0
	FULL_SCAN_LIST *scan_list = NULL;// scan_list[MAX_SCAN_LIST_NUM];
#endif
	u8 *ptr = NULL;

	res = os_zalloc(sizeof(*res));
	if (res == NULL) {
		return NULL;
	}

	//  if (wifi_config_get_scan_table_size(&scan_table_size) < 0) {
	//      return NULL;
	//  }
	scan_table_size = MAX_SCAN_ARRAY_ITEM;
#if 0
	scan_list = os_zalloc(scan_table_size * sizeof(FULL_SCAN_LIST));
	if (scan_list == NULL) {
		wpa_printf(MSG_ERROR, "%s: scan_list alloc fail.\n", __FUNCTION__);
		os_free(res);
		return NULL;
	}
	os_memset(scan_list, 0, sizeof(FULL_SCAN_LIST));

	for (i = 0; i < (scan_table_size - 1); i++) {
		scan_list[i].next = (&(scan_list[i + 1]));
	}
	scan_list[(scan_table_size - 1)].next = NULL;

	scan_result_num = wifi_scan_get_full_list(scan_list);
#else
	scan_result_num = scan_table_size;
	scan_table_array = wifi_scan_table_array();

	if (scan_table_array == NULL) {
		printf("%s: cannot get scan table array.\n", __FUNCTION__);
		return NULL;
	}
#endif
	//  printf("--- scan_result_num = %d ---\n", scan_result_num);
	//  printf("%s: g_scan_array = %p\n", __FUNCTION__, scan_table_array);
	res->num = 0;
	res->res = os_zalloc(scan_result_num * sizeof(struct wpa_scan_res *));
	if (res->res == NULL) {
		wpa_printf(MSG_ERROR, "%s: allocate scan result fail.\n", __FUNCTION__);
		os_free(res);
#if 0
		os_free(scan_list);
#endif
		return NULL;
	}
	os_memset(res->res, 0, scan_result_num * sizeof(struct wpa_scan_res *));

	for (i = 0; i < scan_result_num; i++) {
#if 1
		if (!scan_table_array[i].valid)
			continue;

		scan_res = os_zalloc(sizeof(struct wpa_scan_res) + scan_table_array[i].beacon.LengthVIE);
		if (scan_res == NULL) {
			wpa_printf(MSG_ERROR, "%s: allocate scan result 2 fail.\n", __FUNCTION__);
			break;
		}
		os_memcpy(scan_res->bssid, scan_table_array[i].beacon.Bssid, WIFI_MAC_ADDRESS_LENGTH);
		if (scan_table_array[i].beacon.Channel <= 14)
			scan_res->freq = scan_table_array[i].beacon.Channel * 5 + 2407 ;
		else
			scan_res->freq = scan_table_array[i].beacon.Channel * 5 + 5000 ;

		printf("result  %x:%x:%x:%x:%x:%x@freq:%d\n",
		       scan_res->bssid[0], scan_res->bssid[1], scan_res->bssid[2],
		       scan_res->bssid[3], scan_res->bssid[4], scan_res->bssid[5]
		       scan_res->freq);
		scan_res->beacon_int = scan_table_array[i].beacon.BeaconPeriod;
		scan_res->caps = scan_table_array[i].beacon.CapabilityInfo;
		//      scan_res->tsf = scan_table_array[i].beacon.Timestamp.QuadPart;
		scan_res->beacon_ie_len = 0;

		scan_res->ie_len = scan_table_array[i].beacon.LengthVIE;
#if 1 //Pengfei add, for connect without security mode
		scan_res->auth_mode = scan_table_array[i].beacon.auth_mode; //Pengfei add, for connect without security mode
		scan_res->encrypt_type = scan_table_array[i].beacon.encrypt_mode; //Pengfei add, for connect without security mode
#endif

		ptr = (u8 *)(scan_res + 1); // scan_res: pointer of  (sizeof wpa_scan_res)
		os_memcpy(ptr, scan_table_array[i].beacon.VarIE, scan_table_array[i].beacon.LengthVIE);
#else
		scan_res = os_zalloc(sizeof(struct wpa_scan_res) + scan_list[i].beacon.LengthVIE);
		if (scan_res == NULL) {
			wpa_printf(MSG_ERROR, "%s: allocate scan result 2 fail.\n", __FUNCTION__);
			break;
		}

		os_memcpy(scan_res->bssid, scan_list[i].beacon.Bssid, WIFI_MAC_ADDRESS_LENGTH);
		if (scan_list[i].beacon.Channel <= 14)
			scan_res->freq = scan_list[i].beacon.Channel * 5 + 2412 ;
		else
			scan_res->freq = scan_list[i].beacon.Channel * 5 + 5180 ;

		scan_res->beacon_int = scan_list[i].beacon.BeaconPeriod;
		scan_res->caps = scan_list[i].beacon.CapabilityInfo;
		scan_res->tsf = scan_list[i].beacon.Timestamp.QuadPart;
		scan_res->beacon_ie_len = 0;

		scan_res->ie_len = scan_list[i].beacon.LengthVIE;

		ptr = (u8 *)(scan_res + 1); // scan_res: pointer of  (sizeof wpa_scan_res)
		os_memcpy(ptr, scan_list[i].beacon.VarIE, scan_list[i].beacon.LengthVIE);
#endif
		res->res[(res->num)++] = scan_res;
	}
#if 0
	os_free(scan_list);
#endif
	wifi_scan_reset();

	//  g_scan_by_supplicant = 0;
	return res;
#endif
}

#if 0
static void send_scan_event(struct wpa_driver_gen4m_data *drv, int aborted, struct nlattr *tb[])
{
	union wpa_event_data event;
	struct scan_info *info;

#define MAX_REPORT_FREQS 50

	if (drv->scan_for_auth) {
		drv->scan_for_auth = 0;
		wpa_printf(MSG_DEBUG, "inband: Scan results for missing cfg80211 BSS entry");
		return;
	}

	os_memset(&event, 0, sizeof(event));
	info = &(event.scan_info);
	info->aborted = aborted;

#if 1
	{
		const u8 dummy_ssid[] = "dummy_ap";
		//SCAN_LIST_ITEM *item = (SCAN_LIST_ITEM *)body;

		struct wpa_driver_scan_ssid *s = &info->ssids[info->num_ssids];
		s->ssid = dummy_ssid;
		s->ssid_len = sizeof(dummy_ssid);
		//s->ssid = item->ssid;
		//s->ssid_len = item->ssid_len;
		info->num_ssids++;
	}
#else  // TODO: wifi_conn_get_scan_list is NOT ready
	{
		SCAN_LIST scan_list;

		wifi_conn_get_scan_list(&scan_list);
		if (NULL == scan_list) {
			info->num_ssids = 0;
		} else {
			for (i = 0; (i < MAX_SCAN_ARRAY_ITEM) && (i < WPAS_MAX_SCAN_SSIDS) ; i++) {
				if (g_scan_array[i].valid == 1) {
					struct wpa_driver_scan_ssid *s = &info->ssids[info->num_ssids];
					s->ssid = g_scan_array[i].ap_data.ssid;
					s->ssid_len = g_scan_array[i].ap_data.ssid_length;
					info->num_ssids++;

				}
			}

			do {
				struct wpa_driver_scan_ssid *s = &info->ssids[info->num_ssids];
				s->ssid = scan_list.ap_data.ssid;
				s->ssid_len = scan_list.ap_data.ssid_length;
				info->num_ssids++;

				scan_list = scan_list.next;
			} while (NULL != scan_list);
		}
	}
#endif

	wpa_supplicant_event(drv->ctx, EVENT_SCAN_RESULTS, &event);
}
#endif

/**
 * wpa_driver_nl80211_scan_timeout - Scan timeout to report scan completion
 * @eloop_ctx: Driver private data
 * @timeout_ctx: ctx argument given to wpa_driver_nl80211_init()
 *
 * This function can be used as registered timeout when starting a scan to
 * generate a scan completed event if the driver does not report this.
 */
static void wpa_driver_inband_scan_timeout(void *eloop_ctx, void *timeout_ctx)
{
	wpa_printf(MSG_DEBUG, "wpa_driver_inband_scan_timeout\n");
	wpa_supplicant_event(timeout_ctx, EVENT_SCAN_RESULTS, NULL);
}

#if CFG_DEBUG_SETTING_REPEATER /* CONFIG_REPEATER */

static void driver_inband_update_proto_cipher_from_N9(struct wpa_sm *sm, u8 *wpa_ie, size_t wpa_ie_len)
{
}

#endif /* CONFIG_REPEATER */

static void do_process_drv_event(struct wpa_driver_gen4m_data *drv, int cmd, void *data, int len)
{
	switch (cmd) {
	case NL80211_CMD_NEW_SCAN_RESULTS:
		wpa_printf(MSG_DEBUG, "inband: New scan results available");
		drv->scan_complete_events = 1;
		eloop_cancel_timeout(wpa_driver_inband_scan_timeout, drv, drv->ctx);
		wpa_supplicant_event(drv->ctx, EVENT_SCAN_RESULTS, NULL);
		break;

	case NL80211_CMD_CONNECT:
		if (len == 8) {
			union wpa_event_data event;
			uint16_t *status_code;
			uint8_t *bssid;

			os_memset(&event, 0, sizeof(event));
			bssid = (uint8_t *)data;
			status_code = (uint16_t *)((uint8_t *)data + ETH_ALEN);
			event.assoc_reject.bssid = bssid;
			event.assoc_reject.status_code = *status_code;

			wpa_printf(MSG_DEBUG,
				   "[ASSOC REJECT %d] CONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x\n",
				   *status_code, ((char *)data)[0], ((char *)data)[1],
				   ((char *)data)[2], ((char *)data)[3], ((char *)data)[4],
				   ((char *)data)[5]);

			wpa_supplicant_event(drv->ctx, EVENT_ASSOC_REJECT, &event);
			break;
		} else {
			union wpa_event_data event;
			struct wpa_bss *bss;
			uint8_t bssid[ETH_ALEN + 1] = {};

			os_memcpy(bssid, data, ETH_ALEN);
			os_memset(&event, 0, sizeof(event));
			if (len - 6 > 0) {
				event.assoc_info.req_ies = (uint8_t *) data + ETH_ALEN;
				event.assoc_info.req_ies_len = EVENT_ASSOC_REQ_LEN;
				event.assoc_info.resp_ies = (uint8_t *) data + ETH_ALEN + EVENT_ASSOC_REQ_LEN;
				event.assoc_info.resp_ies_len = EVENT_ASSOC_RESP_LEN;
			} else {
				wpa_printf(MSG_DEBUG, "ies is null");
			}
			bss = wpa_bss_get_bssid(drv->ctx, bssid);
			if (bss) {
				event.assoc_info.beacon_ies = bss->ies;
				event.assoc_info.beacon_ies_len = bss->ie_len ?
					bss->ie_len : bss->beacon_ie_len;
			} else {
				wpa_printf(MSG_DEBUG, "bss entry is null");
			}
			drv->associated = 1;
			drv->scan_complete_events = 0;
			wpa_supplicant_event(drv->ctx, EVENT_ASSOC, &event);
			wifi_api_event_trigger(WIFI_PORT_STA, WIFI_EVENT_IOT_CONNECTED, (uint8_t *)data, ETH_ALEN);
		}
		break;
	case NL80211_CMD_DISCONNECT:
	case NL80211_CMD_DISCONNECT_LOCALLY:
		if (global_entry->op_mode == WIFI_MODE_STA_ONLY) {
			uint8_t packet[WIFI_REASON_CODE_LENGTH] = {0};
			uint16_t reason_code = WIFI_REASON_CODE_DEAUTH_LEAVING_BSS;
			os_memcpy(packet, &reason_code, WIFI_REASON_CODE_LENGTH);
			wifi_api_event_trigger(WIFI_PORT_STA, WIFI_EVENT_IOT_CONNECTION_FAILED, (uint8_t *)packet, WIFI_REASON_CODE_LENGTH);
		}
		if (data) {
			union wpa_event_data event_data;
			struct wpa_supplicant* wpa_s = drv->ctx;
			wpa_printf(MSG_DEBUG,
				   "[EVENT_ID_IOT_DISASSOC] CONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x\n",
				   ((char *)data)[0], ((char *)data)[1], ((char *)data)[2], ((char *)data)[3], ((char *)data)[4], ((char *)data)[5]);
			event_data.disassoc_info.addr = (u8 *)&((u8 *)data)[0];
#if CONFIG_FAST_CONNECTION
			if (cmd == NL80211_CMD_DISCONNECT_LOCALLY)
				event_data.disassoc_info.reason_code =
					WLAN_REASON_DEAUTH_LEAVING;
			else
				event_data.disassoc_info.reason_code =
					WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY;
#else
			event_data.disassoc_info.reason_code =
				WLAN_REASON_DEAUTH_LEAVING;
#endif
			event_data.disassoc_info.locally_generated =
				cmd == NL80211_CMD_DISCONNECT_LOCALLY;
			event_data.disassoc_info.ie_len = 0;
			event_data.disassoc_info.ie = NULL;
			wpa_supplicant_event(drv->ctx, EVENT_DISASSOC, &event_data);
			if (os_strcmp(wpa_s->ifname, sta_ifname) == 0)
				wifi_api_event_trigger(WIFI_PORT_STA, WIFI_EVENT_IOT_DISCONNECTED, (uint8_t *)data, ETH_ALEN);
			else
				wifi_api_event_trigger(WIFI_PORT_AP, WIFI_EVENT_IOT_DISCONNECTED, (uint8_t *)data, ETH_ALEN);
		} else {
			wpa_supplicant_event(drv->ctx, EVENT_DISASSOC, NULL);
		}
		break;
	case NL80211_CMD_FT_INDICATION:
#ifdef CONFIG_IEEE80211R
		if (data) {
			union wpa_event_data event_data;

			wpa_printf(MSG_DEBUG, "NL80211_CMD_FT_INDICATION~~~\n");
			os_memset(&event_data, 0, sizeof(event_data));
			os_memcpy(&event_data.ft_ies.target_ap, data, ETH_ALEN);
			event_data.ft_ies.ies = data + ETH_ALEN;
			event_data.ft_ies.ies_len = len - ETH_ALEN;
			wpa_supplicant_event(drv->ctx, EVENT_FT_RESPONSE,
				&event_data);
		}
#endif
		break;
	case NL80211_CMD_NEW_STATION:
		if (data) {
			union wpa_event_data data2;

			wpa_printf(MSG_DEBUG, "NL80211_CMD_NEW_STATION~~~\n");
			wpa_hexdump(MSG_DEBUG, "ASSOC IE", data, 100);
			os_memset(&data2, 0, sizeof(data2));
			data2.assoc_info.addr = data;
			data2.assoc_info.req_ies = data + ETH_ALEN;
			data2.assoc_info.req_ies_len = len - ETH_ALEN;
			data2.assoc_info.reassoc = 0;
			wpa_supplicant_event(drv->ctx, EVENT_ASSOC, &data2);
		}
		break;
	case NL80211_CMD_FRAME_TX_STATUS:
		wpa_printf(MSG_DEBUG, "NL80211_CMD_FRAME_TX_STATUS~~~\n");
		if (data) {
			union wpa_event_data event;
			const struct ieee80211_hdr *hdr;
			u16 fc;

			wpa_printf(MSG_DEBUG, "nl80211: Frame TX status event");
			wpa_hexdump(MSG_DEBUG, "TX STATUS", data, 100);
			os_memset(&event, 0, sizeof(event));

			event.tx_status.ack = *((u8 *)data);
			hdr = (const struct ieee80211_hdr *)((u8 *)data + 5);
			fc = le_to_host16(hdr->frame_control);

			event.tx_status.type = WLAN_FC_GET_TYPE(fc);
			event.tx_status.stype = WLAN_FC_GET_STYPE(fc);
			event.tx_status.dst = hdr->addr1;
			event.tx_status.data = (u8 *)data + 5;
			event.tx_status.data_len = len - 5;

			wpa_supplicant_event(drv->ctx, EVENT_TX_STATUS, &event);
		}
		break;
	case NL80211_CMD_EXTERNAL_AUTH:
		wpa_printf(MSG_DEBUG, "NL80211_CMD_EXTERNAL_AUTH~~~\n");
		if (data) {
			union wpa_event_data event;
			struct cfg80211_external_auth_params *params;
			os_memset(&event, 0, sizeof(event));
			params = (struct cfg80211_external_auth_params *)data;
			event.external_auth.action = params->action;
			event.external_auth.key_mgmt_suite = params->key_mgmt_suite;
			event.external_auth.ssid_len = params->ssid.ssid_len;
			event.external_auth.ssid = params->ssid.ssid;
			event.external_auth.bssid = params->bssid;

			wpa_printf(MSG_DEBUG, "[EXTERN AUTH] [%02x:%02x:%02x:%02x:%02x:%02x][ssid:%s][ssid_len:%d][action:%d][%02x-%02x-%02x-%02x]",
				   params->bssid[0], params->bssid[1], params->bssid[2],
				   params->bssid[3], params->bssid[4], params->bssid[5],
				   params->ssid.ssid, params->ssid.ssid_len, params->action,
				   (uint8_t)(params->key_mgmt_suite & 0x000000FF),
				   (uint8_t)((params->key_mgmt_suite >> 8) & 0x000000FF),
				   (uint8_t)((params->key_mgmt_suite >> 16) & 0x000000FF),
				   (uint8_t)((params->key_mgmt_suite >> 24) & 0x000000FF));

			wpa_supplicant_event(drv->ctx, EVENT_EXTERNAL_AUTH, &event);
		}
		break;
	case NL80211_CMD_FRAME:
		if (data) {
			union wpa_event_data event;
			u32 sig_dbm;
			u32 freq;
			u32 frame_len;
			u8 *buf;

			buf = (u8 *)data;
			freq = *(u32 *)buf;
			buf += 4;
			sig_dbm = *(u32 *)buf;
			buf += 4;
			frame_len = *(u32 *)buf;
			buf += 4;

			wpa_printf(MSG_DEBUG, "[CMD FRAME]freq %d, sig dbm %d, frame len %d", freq, sig_dbm, frame_len);

			os_memset(&event, 0, sizeof(event));

			event.rx_mgmt.freq = freq;
			event.rx_mgmt.ssi_signal = sig_dbm;
			event.rx_mgmt.frame_len = frame_len;
			event.rx_mgmt.frame = buf;

			wpa_supplicant_event(drv->ctx, EVENT_RX_MGMT, &event);
		}
		break;
	case NL80211_CMD_ACS:
		if (data) {
			union wpa_event_data event;
			struct acs_selected_channels *params;

			os_memset(&event, 0, sizeof(event));

			params = (struct acs_selected_channels *)data;
			event.acs_selected_channels.pri_freq = params->pri_freq;
			event.acs_selected_channels.sec_freq = params->sec_freq;
			event.acs_selected_channels.vht_seg0_center_ch = params->vht_seg0_center_ch;
			event.acs_selected_channels.vht_seg1_center_ch = params->vht_seg1_center_ch;
			event.acs_selected_channels.ch_width = params->ch_width;
			event.acs_selected_channels.hw_mode = params->hw_mode;

			wpa_printf(MSG_INFO,
				   "FreeRTOS: ACS Results: PCH: %d SCH: %d BW: %d VHT0: %d VHT1: %d HW_MODE: %d",
				   event.acs_selected_channels.pri_freq,
				   event.acs_selected_channels.sec_freq,
				   event.acs_selected_channels.ch_width,
				   event.acs_selected_channels.vht_seg0_center_ch,
				   event.acs_selected_channels.vht_seg1_center_ch,
				   event.acs_selected_channels.hw_mode);

			wpa_supplicant_event(drv->ctx, EVENT_ACS_CHANNEL_SELECTED, &event);
		}
		break;
	default:
		break;
	}

	return;
}

static int wpa_driver_gen4m_get_ssid(void *priv, u8 *ssid)
{
	struct wpa_driver_gen4m_data *drv = (struct wpa_driver_gen4m_data *)priv;
	char id[33];

	mtk_freertos_wpa_get_ssid(drv->ssid, (uint32_t *)&drv->ssid_len);
	os_memcpy(id, drv->ssid, drv->ssid_len);
	id[drv->ssid_len] = '\0';
	wpa_printf(MSG_DEBUG, "*** wpa_driver_gen4m_get_ssid: %s\n", id);

	os_memcpy(ssid, drv->ssid, drv->ssid_len);

	return drv->ssid_len;
}

static int wpa_driver_gen4m_get_bssid(void *priv, u8 *bssid)
{
	struct wpa_driver_gen4m_data *drv = (struct wpa_driver_gen4m_data *)priv;

	mtk_freertos_wpa_get_bssid(drv->bssid);
	os_memcpy(bssid, drv->bssid, WIFI_MAC_ADDRESS_LENGTH);
	wpa_printf(MSG_DEBUG, "wpa_driver_gen4m_get_bssid  %x:%x:%x:%x:%x:%x\n",
		   bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

	return 0;
}

static const u8 *wpa_driver_inband_get_mac_addr(void *priv)
{
	struct wpa_driver_gen4m_data *drv = (struct wpa_driver_gen4m_data *)priv;
	return drv->own_addr;
}

#ifdef __NOT_PORTED__
static void wpa_driver_inband_raw_receive(
	void *ctx, const u8 *buf, size_t len)
{
	struct wpa_driver_gen4m_data *drv = ctx;
	const struct ieee80211_mgmt *mgmt;
	u16 fc;
	union wpa_event_data event;

	/* Send Probe Request information to WPS processing */

	if (len < IEEE80211_HDRLEN + sizeof(mgmt->u.probe_req))
		return;
	mgmt = (const struct ieee80211_mgmt *) buf;

	fc = le_to_host16(mgmt->frame_control);
	if (WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_MGMT ||
			WLAN_FC_GET_STYPE(fc) != WLAN_FC_STYPE_PROBE_REQ)
		return;

	os_memset(&event, 0, sizeof(event));
	event.rx_probe_req.sa = mgmt->sa;
	event.rx_probe_req.da = mgmt->da;
	event.rx_probe_req.bssid = mgmt->bssid;
	event.rx_probe_req.ie = mgmt->u.probe_req.variable;
	event.rx_probe_req.ie_len =
		len - (IEEE80211_HDRLEN + sizeof(mgmt->u.probe_req));
	wpa_supplicant_event(drv->ctx, EVENT_RX_PROBE_REQ, &event);
}
#endif // __NOT_PORTED__

//static int process_global_event(struct nl_msg *msg, void *arg)
int process_global_event(void *priv, unsigned char evt_id, unsigned char *body, int len)
{
	struct wpa_driver_gen4m_data *inband_drv = priv;
	char buffer[MAX_EVT_HDR_SIZE + EVENT_DATA_MAX_LEN];
	struct sockaddr_in to;
	wpa_printf(MSG_DEBUG, "evt id   %d\n", evt_id);
	if (len > EVENT_DATA_MAX_LEN) {
		wpa_printf(MSG_ERROR, "out of buffer , error\n");
		return -1;
	}

	/* force memory overwrite if overflow */
	memset(&buffer[0], 0xFF, sizeof(buffer));

	buffer[0] = evt_id;
	os_memcpy(buffer + MAX_EVTID_SIZE, &len, MAX_DATA_LEN_SIZE);
	if (body)
		os_memcpy(buffer + MAX_EVT_HDR_SIZE, body, len);

	to.sin_family = PF_INET;
	to.sin_addr.s_addr = htonl((127 << 24) | 1);
	to.sin_port = inband_drv->driver_msg_port;
	sendto(inband_drv->driver_msg_sock, buffer, MAX_EVT_HDR_SIZE + len, 0, (struct sockaddr *)&to, sizeof(to));
	return 0;
}


#if defined(HOSTAPD) || defined(CONFIG_AP)
/*
static int wpa_driver_nl80211_set_freq()
{
	return 0;
}
*/

// NL80211_CMD_GET_KEY                get_key                       CFG80211_OpsKeyGet
#if 0
static int i802_get_seqnum(const char *iface, void *priv, const u8 *addr,
			   int idx, u8 *seq)
{
	return 0;
}
#endif

#if 0
//20160623 masked by Qingchao.Dai.
//for fix Gcc build warning : defined but not used!

static int i802_get_inact_sec(void *priv, const u8 *addr)
{
	struct hostap_sta_driver_data data;
	int ret = 0;

	data.inactive_msec = (unsigned long) -1;
#if 0 // TODO
	ret = i802_read_sta_data(priv, &data, addr);
#endif
	if (ret || data.inactive_msec == (unsigned long) -1)
		return -1;
	return data.inactive_msec / 1000;
}

#if defined(HOSTAPD) || defined(CONFIG_AP)
static int i802_read_sta_data(void *priv, struct hostap_sta_driver_data *data,
			      const u8 *addr)
{
	return 0;
}
#endif

static int i802_flush(void *priv)
{
	return 0;
}
#endif

#endif

int wpa_driver_inband_set_wep_key(void *ssid_ctx, u8 port_mode)
{
#ifdef __NOT_PORTED__
	struct wpa_ssid *ssid = ssid_ctx;
	wep_key_cm4_t   wep_keys;

	os_memset(&wep_keys, 0, sizeof(wep_key_cm4_t));

	wep_keys.ucdefault_key_id = ssid->wep_tx_keyidx;

	if (ssid->wep_key_len[0] != 0) {
		os_memcpy(wep_keys.auckey_0, ssid->wep_key[0], ssid->wep_key_len[0]);
		wep_keys.uckey_0_len = ssid->wep_key_len[0];
	}

	if (ssid->wep_key_len[1] != 0) {
		os_memcpy(wep_keys.auckey_1, ssid->wep_key[1], ssid->wep_key_len[1]);
		wep_keys.uckey_1_len = ssid->wep_key_len[1];
	}

	if (ssid->wep_key_len[2] != 0) {
		os_memcpy(wep_keys.auckey_2, ssid->wep_key[2], ssid->wep_key_len[2]);
		wep_keys.uckey_2_len = ssid->wep_key_len[2];
	}

	if (ssid->wep_key_len[3] != 0) {
		os_memcpy(wep_keys.auckey_3, ssid->wep_key[3], ssid->wep_key_len[3]);
		wep_keys.uckey_3_len = ssid->wep_key_len[3];
	}

	wpa_hexdump(MSG_DEBUG, "WEP Key", (unsigned char *)&wep_keys, sizeof(wep_keys));
	wifi_inband_set_wep_key(port_mode, &wep_keys);
#endif // __NOT_PORTED__

	return 0;
}

static int wpa_driver_gen4m_set_key(void *priv,
				    struct wpa_driver_set_key_params *params)
{
	struct wpa_driver_gen4m_data *drv = (struct wpa_driver_gen4m_data *)priv;
	unsigned char buff[64] = {0x00};
	wifi_cipher_key_t cipher_key;
	const char *ifname = params->ifname;
	enum wpa_alg alg = params->alg;
	const u8 *addr = params->addr;
	int key_idx = params->key_idx;
	int set_tx = params->set_tx;
	const u8 *seq = params->seq;
	size_t seq_len = params->seq_len;
	const u8 *key = params->key;
	size_t key_len = params->key_len;
#if 0
	u8 port_mode = WIFI_PORT_STA;

	if (global_entry->op_mode == WIFI_MODE_AP_ONLY)
		port_mode = WIFI_PORT_AP;
#endif

	if (alg != WPA_ALG_NONE) {
		if (addr == NULL)
			return -1;

		if (key_len == 0) {
			wpa_printf(MSG_DEBUG, "Ignore set key due to key_len=0!\n");
			return 0;
		}
	}
	os_memset(&cipher_key, 0, sizeof(wifi_cipher_key_t));
	switch (alg) {
	case WPA_ALG_WEP:
		if (set_tx == 0) {
			wpa_printf(MSG_DEBUG, "set_tx=%d, ignore set key =0!\n", set_tx);
			return 0;
		}
		if (key_len == 5)
			cipher_key.cipher_type = WIFI_CIPHER_WEP64;
		else if (key_len == 13)
			cipher_key.cipher_type = WIFI_CIPHER_WEP128;
		else
			cipher_key.cipher_type = WIFI_CIPHER_WEP152;
		break;
	case WPA_ALG_TKIP:
		cipher_key.cipher_type = WIFI_CIPHER_TKIP;
		break;
	case WPA_ALG_CCMP:
		cipher_key.cipher_type = WIFI_CIPHER_AES;
		break;
	case WPA_ALG_BIP_CMAC_128:
		cipher_key.cipher_type = WIFI_CIPHER_BIP;
		break;
	case WPA_ALG_NONE:
		mtk_freertos_wpa_set_key(ifname, priv, alg, addr, key_idx,
					 set_tx, seq, seq_len, key, key_len);
		return 0;
	default:
		wpa_printf(MSG_ERROR, "%s: Unsupported encryption "
			   "algorithm %d", __func__, alg);
		return -1;
	}

	buff[0] = (u8) key_idx;
	if (is_broadcast_ether_addr(addr))
		buff[1] = 0; /* Group Key */
	else
		buff[1] = 1; /* Pair Wise Key */

#if 1
	if (alg == WPA_ALG_TKIP) {
		os_memcpy(cipher_key.key, key, 16);
		os_memcpy(cipher_key.rx_mic, key + 16 + 8, 8);
		os_memcpy(cipher_key.tx_mic, key + 16, 8);
	} else {
		os_memcpy(cipher_key.key, key, key_len);
	}
	cipher_key.key_length = key_len;
	os_memcpy(buff + 2, &cipher_key, sizeof(wifi_cipher_key_t));
	if (buff[1] == 0)
		os_memcpy(buff + 2 + sizeof(wifi_cipher_key_t), drv->own_addr, 6);
	else
		os_memcpy(buff + 2 + sizeof(wifi_cipher_key_t), addr, 6);
#else
	os_memcpy(buff + 2, key, key_len);
	buff[2 + 32 + 12] = 4; //alg;
	buff[2 + 32 + 13] = key_len;
	os_memset(&buff[2 + 32 + 14], 0x00, 6);
	buff[2 + 32 + 20] = 1;
#endif

	wpa_printf(MSG_DEBUG, "%s --> Key_idx =%d, KEY LEN = %d, alg = %d, sizeof(wifi_cipher_key_t) = %d",
		   __FUNCTION__, buff[0], key_len, cipher_key.cipher_type, sizeof(wifi_cipher_key_t));
	wpa_hexdump(MSG_MSGDUMP, "cipher_key.key", cipher_key.key, cipher_key.key_length);
	wpa_hexdump(MSG_MSGDUMP, "buff", buff, 64);

	mtk_freertos_wpa_set_key(ifname, priv, alg, addr, key_idx, set_tx, seq, seq_len,
				 key, key_len);

	//  inband_queue_exec(WIFI_COMMAND_ID_IOT_SET_KEY, buff, 64, TRUE, FALSE);
	return 0;
}

static void wpa_driver_gen4m_set_rekey_info(void *priv, const u8 *kek, size_t kek_len,
		const u8 *kck, size_t kck_len,
		const u8 *replay_ctr)
{

	if (kek_len == 0 || kck_len == 0) {
		wpa_printf(MSG_DEBUG, "Ignore set key due to kck_kck_len=0!\n");
		return;
	}

	if (!kek || !kck || !replay_ctr)
		return;

	wpa_hexdump_key(MSG_MSGDUMP, "kek", kek, kek_len);
	wpa_hexdump_key(MSG_MSGDUMP, "kck", kck, kck_len);

	wpa_printf(MSG_DEBUG, "nl80211: Set rekey offload kek-kck len %d-%d",
		   kek_len, kck_len);
	mtk_freertos_wpa_set_rekey_data(priv, kek, kek_len, kck, kck_len, replay_ctr);
}

static int wpa_driver_inband_set_ptk_kck_kek(void *priv,
		const u8 *key, size_t key_len)
{
	unsigned char buff[64] = {0x00};
	wifi_cipher_key_t cipher_key;

	if (key_len == 0) {
		wpa_printf(MSG_DEBUG, "Ignore set key due to key_len=0!\n");
		return -1;
	}

	os_memset(&cipher_key, 0, sizeof(wifi_cipher_key_t));
	buff[0] = 0;
	buff[1] = 2; /* kck+kek */
	os_memcpy(cipher_key.key, key, key_len);
	cipher_key.key_length = key_len;
	os_memcpy(buff + 2, &cipher_key, sizeof(wifi_cipher_key_t));

	wpa_printf(MSG_DEBUG, "\n%s --> \n", __FUNCTION__);
	wpa_hexdump(MSG_MSGDUMP, "cipher_key.key", cipher_key.key, cipher_key.key_length);
	wpa_printf(MSG_DEBUG, "\n");
	//  inband_queue_exec(WIFI_COMMAND_ID_IOT_SET_KEY, buff, 64, TRUE, FALSE); // from ISR
	return 0;
}

#ifdef CONFIG_AP
// NL80211_CMD_SET_BEACON               set_beacon                  CFG80211_OpsSetBeacon
static int wpa_driver_gen4m_set_ap(void *priv,
				   struct wpa_driver_ap_params *params)
{
	int ret = 0;
	wpa_printf(MSG_DEBUG, "wpa_driver_gen4m_set_ap \n");

	ret = mtk_p2p_freertos_wpa_start_ap(priv, params);

	return ret;
}

static int wpa_driver_gen4m_do_acs(void *priv,
				   struct drv_acs_params *params)
{
	int ret = 0;

	wpa_printf(MSG_DEBUG, "wpa_driver_gen4m_do_acs\n");

	ret = mtk_p2p_freertos_wpa_do_acs(priv, params);
	if (ret)
		wpa_printf(MSG_DEBUG, "Do ACS offloading failed\n");

	return ret;
}

// NL80211_CMD_DEL_BEACON               del_beacon                  CFG80211_OpsDelBeacon
// static int wpa_driver_nl80211_del_beacon(struct wpa_driver_gen4m_data *drv)
static int wpa_driver_gen4m_del_beacon()
{
	return 0;
}
#endif

static void wpa_driver_gen4m_deinit(void *priv)
{
	struct wpa_driver_gen4m_data *drv = (struct wpa_driver_gen4m_data *)priv;

	wpa_printf(MSG_DEBUG, "\n%s --> \n", __FUNCTION__);
	eloop_cancel_timeout(wpa_driver_inband_scan_timeout, drv, drv->ctx);
#ifdef MTK_MINISUPP_ENABLE
	unregister_process_global_event();
#endif
#ifdef CONFIG_AP
	wpa_driver_gen4m_del_beacon();
#endif
#ifdef MTK_WIFI_DIRECT_ENABLE
	wpa_printf(MSG_DEBUG, "[%s] wifi_inband_p2p_info - P2P_IDLE, P2P_DEVICE\n", __FUNCTION__);
	wifi_inband_p2p_info(P2P_IDLE, P2P_DEVICE);
#endif

	eloop_unregister_read_sock(drv->driver_msg_sock);
	close(drv->driver_msg_sock);
	os_free(priv);
	return;
}

#ifdef CONFIG_AP
static int wpa_driver_gen4m_deinit_ap(void *priv)
{
	// wpa_driver_nl80211_del_beacon
	mtk_p2p_freertos_wpa_stop_ap();

	return 0;
}
#endif

//NL80211_CMD_GET_STATION               get_station                 CFG80211_OpsStaGet
/* not used

//static int nl80211_get_link_signal(struct wpa_driver_gen4m_data *drv, struct wpa_signal_info *sig)
static int nl80211_get_link_signal()
{
    return 0;
}


//static void nl80211_cqm_event(struct wpa_driver_gen4m_data *drv, struct nlattr *tb[])
static void nl80211_cqm_event()
{
    nl80211_get_link_signal();
    return;

    // static void do_process_drv_event(struct wpa_driver_gen4m_data *drv, int cmd, struct nlattr **tb)
}

static int nl80211_signal_poll(void *priv, struct wpa_signal_info *si)
{
    // res = nl80211_get_link_signal(drv, si);
    return 0;

    // static void do_process_drv_event(struct wpa_driver_gen4m_data *drv, int cmd, struct nlattr **tb)
}
*/


// NL80211_CMD_SET_STATION              change_station              CFG80211_OpsStaChg
// NL80211_CMD_NEW_STATION              add_station                 CFG80211_OpsStaAdd
static int wpa_driver_gen4m_sta_add(void *priv,
				    struct hostapd_sta_add_params *params)
{
	return 0;
}

static int wpa_driver_gen4m_sta_set_flags(void *priv, const u8 *addr,
		unsigned int total_flags,
		unsigned int flags_or, unsigned int flags_and)
{
	return 0;
}

static int wpa_driver_gen4m_set_supp_port(void *priv, int authorized)
{
	return 0;
}

#ifdef HOSTAPD
static int i802_set_sta_vlan(void *priv, const u8 *addr,
			     const char *ifname, int vlan_id)
{
	return 0;
}


static int i802_set_wds_sta(void *priv, const u8 *addr, int aid, int val,
			    const char *bridge_ifname)
{
	//return i802_set_sta_vlan(priv, addr, name, 0);
	return 0;
}
#endif

//NL80211_CMD_DEL_STATION               del_station                 CFG80211_OpsStaDel

static int wpa_driver_gen4m_sta_remove(void *priv, const u8 *addr)
{
	return 0;
}

#if defined(HOSTAPD) || defined(CONFIG_AP)
#ifdef CONFIG_WPS
static int wpa_driver_inband_set_ap_wps_ie(void *priv, const struct wpabuf *beacon,
		const struct wpabuf *proberesp,
		const struct wpabuf *assocresp)
{
	//rtos_porting
#if 0
	uint8_t port = WIFI_PORT_AP;
#ifdef MTK_WIFI_DIRECT_ENABLE
	if (global_entry->op_mode == WIFI_MODE_P2P_ONLY) {
		port = WIFI_PORT_STA; // [ShiuanWen] Should consider GO?
	}
#endif /*MTK_WIFI_DIRECT_ENABLE*/

	if (beacon) {
		wpa_hexdump(MSG_DEBUG, "beacon",
			    (unsigned char *)beacon + sizeof(struct wpabuf), beacon->used);
		if ((beacon->used != 0) &&
				(beacon->used <= EXTRA_IE_SIZE_LIMIT)) {
#ifdef MTK_WIFI_DIRECT_ENABLE
			if (global_entry->op_mode == WIFI_MODE_P2P_ONLY) {
				wifi_inband_set_extra_ie(port, WIFI_EXTRA_P2P_BEACON_IE,
							 (uint8_t *)beacon + sizeof(struct wpabuf), beacon->used);
			} else
#endif /*MTK_WIFI_DIRECT_ENABLE*/
			{
				wifi_inband_set_extra_ie(port, WIFI_EXTRA_WPS_BEACON_IE,
							 (uint8_t *)beacon + sizeof(struct wpabuf), beacon->used);
			}
		} else {
			wpa_printf(MSG_DEBUG, "%s: beacon extra ie is too large(=%d).\n",
				   __FUNCTION__, beacon->used);
			wifi_inband_set_extra_ie(port, WIFI_EXTRA_WPS_BEACON_IE,
						 NULL, 0);
		}
	}

	if (proberesp) {
		wpa_hexdump(MSG_DEBUG, "probe_resp: extra_ies",
			    (unsigned char *)proberesp + sizeof(struct wpabuf), proberesp->used);
		if ((proberesp->used != 0) &&
				(proberesp->used <= EXTRA_IE_SIZE_LIMIT)) {
#ifdef MTK_WIFI_DIRECT_ENABLE
			if (global_entry->op_mode == WIFI_MODE_P2P_ONLY) {
				wifi_inband_set_extra_ie(port, WIFI_EXTRA_P2P_PROBE_RSP_IE,
							 (uint8_t *)proberesp + sizeof(struct wpabuf), proberesp->used);
			} else
#endif  /*MTK_WIFI_DIRECT_ENABLE*/
			{
				wifi_inband_set_extra_ie(port, WIFI_EXTRA_WPS_PROBE_RSP_IE,
							 (uint8_t *)proberesp + sizeof(struct wpabuf), proberesp->used);
			}
		} else {
			wpa_printf(MSG_DEBUG, "%s: proberesp extra ie is too large(=%d).\n",
				   __FUNCTION__, proberesp->used);
			wifi_inband_set_extra_ie(port, WIFI_EXTRA_WPS_PROBE_RSP_IE,
						 (uint8_t *)NULL, 0);
		}
	}

	if (assocresp) {
		wpa_hexdump(MSG_DEBUG, "assocresp",
			    (unsigned char *)assocresp + sizeof(struct wpabuf), assocresp->used);
		if ((assocresp->used != 0) &&
				(assocresp->used <= EXTRA_IE_SIZE_LIMIT)) {
#ifdef MTK_WIFI_DIRECT_ENABLE
			if (global_entry->op_mode == WIFI_MODE_P2P_ONLY) {
				wifi_inband_set_extra_ie(port, WIFI_EXTRA_P2P_ASSOC_RSP_IE,
							 (uint8_t *)assocresp + sizeof(struct wpabuf), assocresp->used);
			} else
#endif /*MTK_WIFI_DIRECT_ENABLE*/
			{
				wifi_inband_set_extra_ie(port, WIFI_EXTRA_WPS_ASSOC_RSP_IE,
							 (uint8_t *)assocresp + sizeof(struct wpabuf), assocresp->used);
			}
		} else {
			wpa_printf(MSG_DEBUG, "%s: assocresp extra ie is too large(=%d).\n",
				   __FUNCTION__, assocresp->used);
			wifi_inband_set_extra_ie(port, WIFI_EXTRA_WPS_ASSOC_RSP_IE,
						 (uint8_t *)NULL, 0);
		}
	}
#endif
	return 0;
}
#endif /* CONFIG_WPS */


static int wpa_driver_gen4m_sta_deauth(void *priv, const u8 *own_addr, const u8 *addr,
				       u16 reason)
{
	struct wpa_driver_gen4m_data *drv = priv;
	int rv = 0;

	if (is_broadcast_ether_addr(addr)) {
		wpa_printf(wpa_debug_level, "%s: This is broadcast addr. Ignore.\n",
			   __FUNCTION__);
		return 0;
	}

	if (is_zero_ether_addr(addr)) {
		wpa_printf(wpa_debug_level, "%s: This is zero addr. Ignore.\n",
			   __FUNCTION__);
		return 0;
	}

	drv->ignore_sta_disconnect_event = 1;
	rv = wifi_connection_disconnect_sta((uint8_t *)addr);
	wpa_printf(MSG_DEBUG, "%s: Send deauth to STA. rv = %d\n",
		   __FUNCTION__, rv);
	return rv;
}


static int wpa_driver_inband_sta_disassoc(void *priv, const u8 *own_addr, const u8 *addr,
		u16 reason)
{
	return 0;
}

#endif

// NL80211_CMD_TRIGGER_SCAN             scan                        CFG80211_OpsScan
/**
 * wpa_driver_nl80211_scan - Request the driver to initiate scan
 * @priv: Pointer to private driver data from wpa_driver_nl80211_init()
 * @params: Scan parameters
 * Returns: 0 on success, -1 on failure
 */
static int wpa_driver_gen4m_scan(void *priv,
				 struct wpa_driver_scan_params *params)
{
	struct wpa_driver_gen4m_data *drv = priv;
	int timeout;
	int ret = 0;

	/*printf("%s: num_ssids=%d, ssid = %s, ssid_len = %d\n",
	   __FUNCTION__,
	   params->num_ssids, params->ssids[0].ssid,
	   params->ssids[0].ssid_len);*/
	wpa_printf(MSG_DEBUG, "%s\r\n", __func__);

	timeout = 15;

	ret = mtk_freertos_wpa_scan(priv, params);
	if (ret)
		wpa_printf(MSG_DEBUG, "Scan failed\n");

	eloop_cancel_timeout(wpa_driver_inband_scan_timeout, drv, drv->ctx);

	if (!ret)
		eloop_register_timeout(timeout, 0, wpa_driver_inband_scan_timeout,
				       drv, drv->ctx);

	return ret;
}


#ifdef __NOT_PORTED__
static int wpa_driver_inband_connect(struct wpa_driver_gen4m_data *drv, struct wpa_driver_associate_params *params)
{
	u8 port_mode = WIFI_PORT_STA, ssid_len, opmode = 0;
	uint8_t channel, wps_enable = 0;
	wpa_printf(MSG_DEBUG, "iverson wpa_driver_inband_connect \n");

	if (params == NULL) {
		wpa_printf(MSG_ERROR, "ERROR!! Null SSID params...\n");
		return -1;
	}

	if (params->freq.freq < 5180) {
		channel = (params->freq.freq - 2412) / 5;
	} else {
		channel = (params->freq.freq - 5180) / 5;
	}

	if (params->mode == IEEE80211_MODE_AP)
		port_mode = WIFI_PORT_AP;

	if (params->ssid_len > WIFI_MAX_LENGTH_OF_SSID) {
		wpa_printf(MSG_ERROR, "ERROR! Invalid len (%d)\n", params->ssid_len);
		return -1;
	}
	wpa_printf(MSG_DEBUG, "driver_inband: SSID [%s], len(%d)\n", params->ssid, (unsigned int)params->ssid_len);

	if (params->bssid) {
		wpa_printf(MSG_DEBUG, "  * bssid=" MACSTR,
			   MAC2STR(params->bssid));
		os_memcpy(drv->bssid, params->bssid, ETH_ALEN);
	}

	os_memcpy(drv->ssid, params->ssid, params->ssid_len);
	drv->ssid_len = params->ssid_len;

	ssid_len = params->ssid_len;
	//wifi_inband_ssid_channel(1, port_mode, (u8 *)params->ssid, &ssid_len, channel);
	//wifi_inband_ssid(1, port_mode, (u8 *)params->ssid, &ssid_len);
	return 0;
}
#endif // __NOT_PORTED__


static int wpa_driver_gen4m_associate(
	void *priv, struct wpa_driver_associate_params *params)
{
	struct wpa_driver_gen4m_data *drv;

	drv   = priv;

	os_memcpy(drv->ssid, params->ssid, params->ssid_len);
	if (params->ssid_len < WIFI_MAX_LENGTH_OF_SSID)
		drv->ssid[params->ssid_len] = '\0';
	drv->ssid_len = params->ssid_len;

	os_memcpy(drv->bssid, params->bssid, WIFI_MAC_ADDRESS_LENGTH);

	wpa_printf(MSG_DEBUG, "gen4m_associate wpa_ie_len %d", params->wpa_ie_len);

	mtk_freertos_wpa_associate(priv, params);

	return 0;
}

//NL80211_CMD_DISCONNECT                disconnect                  CFG80211_OpsDisconnect
//static int wpa_driver_nl80211_disconnect(struct wpa_driver_gen4m_data *drv, const u8 *addr, int reason_code)
#if CFG_DEBUG_SETTING_DISCONNECT /* wpa_driver_inband_disconnect */
static int wpa_driver_inband_disconnect()
{
	if (global_entry->op_mode_change_flag == 1) {
		return 0;
	}
	wifi_inband_disconnect(0, NULL);

	return 0;
}
#endif /* wpa_driver_inband_disconnect */

static int wpa_driver_gen4m_deauthenticate(void *priv, const u8 *addr,
		u16 reason_code)
{
	mtk_freertos_wpa_disconnect(priv, addr, reason_code);
	return 0;
}

static int wpa_driver_gen4m_disassociate(void *priv, const u8 *addr,
		int reason_code)
{
	mtk_freertos_wpa_disconnect(priv, addr, reason_code);
	return 0;
}

static int wpa_driver_gen4m_send_external_auth_status(void *priv,
		struct external_auth *params)
{
	mtk_freertos_wpa_external_auth(params);
	return 0;
}

#ifdef CONFIG_IEEE80211R
static int wpa_driver_gen4m_update_ft_ies(void *priv, const u8 *md,
		const u8 *ies, size_t ies_len)
{
	u16 mdid = WPA_GET_LE16(md);
	return mtk_freertos_update_ft_ies(mdid, ies, ies_len);
}
#endif

/* not used

//static int nl80211_disconnect(struct wpa_driver_gen4m_data *drv, const u8 *bssid)
static int nl80211_disconnect()
{

    wpa_driver_nl80211_disconnect();
    return 0;
}
*/

//NL80211_CMD_SET_PMKSA                 set_pmksa                   CFG80211_OpsPmksaDel
static int wpa_driver_inband_add_pmkid(void *priv, struct wpa_pmkid_params *params)
{
	if (params)
		mtk_freertos_wpa_add_pmkid(priv, params->bssid, params->pmkid);
	return 0;
}

// NL80211_CMD_DEL_PMKSA                    del_pmksa                   CFG80211_OpsPmksaDel
static int wpa_driver_inband_remove_pmkid(void *priv, struct wpa_pmkid_params *params)
{
	if (params)
		mtk_freertos_wpa_remove_pmkid(priv, params->bssid, params->pmkid);
	return 0;
}

// NL80211_CMD_FLUSH_PMKSA                  flush_pmksa                 CFG80211_OpsPmksaFlush
static int wpa_driver_inband_flush_pmkid(void *priv)
{
	mtk_freertos_wpa_flush_pmkid(priv);
	return 0;
}

// NL80211_CMD_REMAIN_ON_CHANNEL            remain_on_channel           CFG80211_OpsRemainOnChannel
#ifdef MTK_WIFI_DIRECT_ENABLE
extern unsigned char g_p2p_listen_channel;
#endif  /*MTK_WIFI_DIRECT_ENABLE*/
static int wpa_driver_inband_remain_on_channel(void *priv, unsigned int freq,
		unsigned int duration)
{
#ifdef MTK_WIFI_DIRECT_ENABLE
	//unsigned char port = WIFI_PORT_STA;
	//struct wpa_supplicant *wpa_s = priv;
	struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, p2p_ifname);
	union wpa_event_data data;
	P2P_LISTEN_PARAM remain_on_channel_param;
	remain_on_channel_param.channel = g_p2p_listen_channel;
	remain_on_channel_param.ucReserved = 0x0;
	remain_on_channel_param.ListenDuration = (uint16_t)duration;

	wpa_printf(MSG_DEBUG, "[%s]inband_p2p_listen_channel: %d \n", __FUNCTION__, remain_on_channel_param.channel);
	wpa_printf(MSG_DEBUG, "[%s]inband_p2p_listen_duration: %d \n", __FUNCTION__, remain_on_channel_param.ListenDuration);

	g_p2p_listen_freq = freq;
	data.remain_on_channel.freq = freq;
	data.remain_on_channel.duration = duration;


	if (global_entry->op_mode == WIFI_MODE_P2P_ONLY) {
		wifi_inband_p2p_listen(&remain_on_channel_param);
	}
	wpa_supplicant_event(wpa_s, EVENT_REMAIN_ON_CHANNEL, &data);

#endif  /*MTK_WIFI_DIRECT_ENABLE*/
	return 0;
}
// NL80211_CMD_CANCEL_REMAIN_ON_CHANNEL     cancel_remain_on_channel    CFG80211_OpsCancelRemainOnChannel
static int wpa_driver_inband_cancel_remain_on_channel(void *priv)
{
#if defined(MTK_WIFI_DIRECT_ENABLE)
	//unsigned char port = WIFI_PORT_STA;
	//struct wpa_supplicant *wpa_s = priv;
	struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, p2p_ifname);
	P2P_LISTEN_PARAM remain_on_channel_param;

	remain_on_channel_param.channel = 0x0;
	remain_on_channel_param.ucReserved = 0x0;
	remain_on_channel_param.ListenDuration = 0x0;

	wpa_printf(MSG_DEBUG, "Inband_driver: Stop p2p listen.\n");

	if (global_entry->op_mode == WIFI_MODE_P2P_ONLY) {
		wifi_inband_p2p_listen(&remain_on_channel_param);
	}

#endif  /*MTK_WIFI_DIRECT_ENABLE*/
	return 0;
}

// NL80211_CMD_SET_TX_BITRATE_MASK          set_bitrate_mask            CFG80211_OpsBitrateSet
//static int nl80211_disable_11b_rates(struct wpa_driver_gen4m_data *drv, int ifindex, int disabled)
static int inband_disable_11b_rates()
{
	return 0;
}

//static int nl80211_create_iface(struct wpa_driver_gen4m_data *drv, const char *ifname, enum nl80211_iftype iftype, const u8 *addr, int wds)
static int inband_create_iface()
{
	inband_disable_11b_rates();
	return 0;
}

static int wpa_driver_inband_if_add(void *priv, enum wpa_driver_if_type type,
				    const char *ifname, const u8 *addr,
				    void *bss_ctx, void **drv_priv,
				    char *force_ifname, u8 *if_addr,
				    const char *bridge, int use_existing, int setup_ap)
{
	/*
	    ifidx = nl80211_create_iface(drv, ifname,
	                 wpa_driver_nl80211_if_type(type), addr,
	                 0);
	*/
	inband_create_iface();
	return 0;
}

// NL80211_CMD_REGISTER_FRAME               mgmt_frame_register         CFG80211_OpsMgmtFrameRegister
// NL80211_CMD_FRAME                        mgmt_tx                  CFG80211_OpsMgmtTx
/*
static int nl80211_send_frame_cmd(struct i802_bss *bss,
                  unsigned int freq, unsigned int wait,
                  const u8 *buf, size_t buf_len,
                  u64 *cookie_out, int no_cck, int no_ack,
                  int offchanok)
*/
#if CFG_DEBUG_SETTING_SEND_FRAME /* inband_send_frame_cmd */
static int inband_send_frame_cmd()
{
	return 0;
}
#endif /* inband_send_frame_cmd */

/*
static int wpa_driver_nl80211_send_frame(struct i802_bss *bss,
                     const void *data, size_t len,
                     int encrypt, int noack,
                     unsigned int freq, int no_cck,
                     int offchanok, unsigned int wait_time)
*/
#if CFG_DEBUG_SETTING_SEND_FRAME /* wpa_driver_inband_send_frame */
static int wpa_driver_inband_send_frame()
{
	// return nl80211_send_frame_cmd(bss, freq, wait_time, data, len,
	//                &cookie, no_cck, noack, offchanok);
	inband_send_frame_cmd();
	return 0;
}
#endif /* wpa_driver_inband_send_frame */

extern struct netif ap_if;

#if defined(HOSTAPD) || defined(CONFIG_AP)
static int wpa_driver_inband_hapd_send_eapol(
	void *priv, const u8 *addr, const u8 *data,
	size_t data_len, int encrypt, const u8 *own_addr, u32 flags)
{
	char buf[1600], *p = buf;
	char proto[2] = {0x88, 0x8E};
	os_memset(p, 0, sizeof(buf));
	//p += 2; /* for ETH header drop */
	os_memcpy(p, addr, ETH_ALEN);
	p += ETH_ALEN;
	os_memcpy(p, own_addr, ETH_ALEN);
	p += ETH_ALEN;
	os_memcpy(p, proto, sizeof(proto));
	p += sizeof(proto);
	os_memcpy(p, data, data_len);
	p += data_len;

#ifdef CONFIG_AP
	wpa_printf(MSG_DEBUG, "wpa_driver_inband_hapd_send_eapol \n");
	mtk_l2_packet_send((unsigned char *)buf, (unsigned int)(p - buf), &ap_netif);
#endif
	return 0;
}
#endif

/*
static int wpa_driver_nl80211_send_mlme_freq(struct i802_bss *bss,
                         const u8 *data,
                         size_t data_len, int noack,
                         unsigned int freq, int no_cck,
                         int offchanok,
                         unsigned int wait_time)
*/

//TODO: If no use anymore, need to remove
//static int wpa_driver_inband_send_mlme_freq()
//{
//  return 0;
//}

//TODO: If no use anymore, need to remove
static int wpa_driver_inband_send_mlme(
	void *priv, const u8 *data, size_t data_len,
	int noack, unsigned int freq, const u16 *csa_offs,
	size_t csa_offs_len, int no_encrypt, unsigned int wait)
{

	wpa_printf(MSG_DEBUG, "wpa_driver_inband_send_mlme\n");
	//struct i802_bss *bss = priv;
	//return wpa_driver_inband_send_mlme_freq(bss, data, data_len, noack,
	//                   0, 0, 0, 0);

	return mtk_freertos_wpa_send_mlme(priv, data, data_len);
}


static int wpa_driver_gen4m_send_action(void *priv, unsigned int freq,
					unsigned int wait_time,
					const u8 *dst, const u8 *src,
					const u8 *bssid, const u8 *data,
					size_t data_len, int no_cck)
{
#if defined(MTK_WIFI_DIRECT_ENABLE) || defined(CONFIG_MBO)
	u64 send_action_cookie = 0;
	struct wpa_driver_gen4m_data *drv = priv;
	int ret = -1;
	u8 *buf;
	struct ieee80211_hdr *hdr;

	wpa_printf(MSG_DEBUG, "nl80211: Send Action frame (ifindex=%d, "
		   "freq=%u MHz wait=%d ms no_cck=%d)",
		   drv->ifindex, freq, wait_time, no_cck);

	buf = os_zalloc(24 + data_len);
	if (buf == NULL)
		return ret;
	os_memcpy(buf + 24, data, data_len);
	hdr = (struct ieee80211_hdr *) buf;
	hdr->frame_control =
		IEEE80211_FC(WLAN_FC_TYPE_MGMT, WLAN_FC_STYPE_ACTION);
	os_memcpy(hdr->addr1, dst, ETH_ALEN);
	os_memcpy(hdr->addr2, src, ETH_ALEN);
	os_memcpy(hdr->addr3, bssid, ETH_ALEN);

	ret = mtk_freertos_mgmt_tx(priv, freq, buf, 24 + data_len,
				   &send_action_cookie, no_cck, 0);

	os_free(buf);
	return ret;
#endif
	return 0;
}


// NL80211_CMD_SET_POWER_SAVE              set_power_mgmt             CFG80211_OpsPwrMgmt
//static int nl80211_set_power_save(struct i802_bss *bss, int enabled)
#ifdef MTK_WIFI_DIRECT_ENABLE
static int _inband_set_power_save()
{
	return 0;
}

static int wpa_driver_inband_set_p2p_powersave(void *priv, int legacy_ps, int opp_ps,
		int ctwindow)
{
	_inband_set_power_save();
	return 0;
}
#endif /* MTK_WIFI_DIRECT_ENABLE */

// NL80211_CMD_FRAME_WAIT_CANCEL            mgmt_cancel_tx_wait      CFG80211_OpsTxCancelWait
static void wpa_driver_inband_send_action_cancel_wait(void *priv)
{
	return;
}

struct l2_packet_data *wpa_driver_inband_get_sock_recv(void *priv)
{
	struct wpa_driver_gen4m_data *inband_drv = priv;
	return inband_drv->sock_recv;
}

void wpa_driver_inband_handle_beacon(void *priv, u8 *target_bssid)
{
	struct wpa_driver_gen4m_data *drv = priv;
	struct wpa_bss *bss = wpa_bss_get_bssid(drv->ctx, (const u8 *)target_bssid);
	struct ieee802_11_elems elems;
	u8 *ie = NULL;

	if (global_entry->op_mode == WIFI_MODE_REPEATER) {
		return;
	}

	if (bss) {
		drv->connect_wmm_ap = 0;
		ie = (u8 *)(bss + 1);
		if (ie) {
			os_memset(&elems, 0, sizeof(elems));
#ifdef CONFIG_AP
			(void) ieee802_11_parse_elems(ie, bss->ie_len,
						      &elems,
						      1);
#endif //CONFIG_AP xin deng add 2016.7.12
			/*
			    Some old HT AP doesn't include WMM IE in Beacon.
			    [WCNCR00050409]
			*/
			if (elems.wmm || elems.ht_capabilities)
				drv->connect_wmm_ap = 1;
		}
	} else {
		wpa_printf(MSG_ERROR, "%s: cannot find this bss???\n", __FUNCTION__);
	}
}

#define PSM_ACTIVE 0
void wpa_driver_inband_set_ps_mode(void *priv, uint8_t do_assoc)
{
	//struct wpa_driver_gen4m_data *drv = priv;
	//struct wpa_supplicant *wpa_s = drv->ctx;

	if (global_entry->op_mode == WIFI_MODE_REPEATER) {
		wpa_printf(MSG_DEBUG, "%s: No need to set ps mode in Repeater Mode.\n",
			   __FUNCTION__);
		return;
	}
#if 0
	if (do_assoc) {
		/*
		    Do assocication now.
		    Set PSMode = 0 to N9.
		*/
		wifi_inband_set_ps_mode(PSM_ACTIVE);
	} else {
		uint8_t ps_mode = wpa_s->conf->ssid->ps_mode;
		wpa_printf(MSG_DEBUG, "%s: ps_mode = %d\n",
			   __FUNCTION__, ps_mode);

		if (ps_mode != PSM_ACTIVE) {
			wpa_printf(MSG_DEBUG, "%s: set ps_mode(=%d) to N9.\n",
				   __FUNCTION__, ps_mode);
			wifi_inband_set_ps_mode(ps_mode);
		}
	}
#endif
}

int mtk_ap_start(void *priv, const u8 *buf, uint8_t buf_len)
{
	int ret = -1;
#ifdef __NOT_PORTED__
	ret = wifi_inband_set_ap_start();
#endif // __NOT_PORTED__
	return ret;
}

int mtk_ap_stop(void *priv, const u8 *buf, uint8_t buf_len)
{
	int ret = -1;
#ifdef __NOT_PORTED__
	ret = wifi_inband_set_ap_stop();
#endif // __NOT_PORTED__
	return ret;
}

int mtk_handle_beacon(void *priv, const u8 *buf, uint8_t buf_len)
{
	struct wpa_driver_gen4m_data *inband_drv = priv;
	struct wpa_supplicant *wpa_s = NULL;
	wpa_s = (struct wpa_supplicant *)inband_drv->ctx;
	wpa_driver_inband_handle_beacon(wpa_s->drv_priv, wpa_s->bssid);
	return 0;
}

int mtk_ps_mode(void *priv, const u8 *buf, uint8_t buf_len)
{
	struct wpa_driver_gen4m_data *inband_drv = priv;
	struct wpa_supplicant *wpa_s = NULL;
	wpa_s = (struct wpa_supplicant *)inband_drv->ctx;
	wpa_driver_inband_set_ps_mode(wpa_s->drv_priv, 0);
	return 0;
}

int mtk_set_ptk_kck_kek(void *priv, const u8 *buf, uint8_t buf_len)
{
	struct wpa_driver_gen4m_data *inband_drv = priv;
	struct wpa_supplicant *wpa_s = NULL;
	int ret;
	wpa_s = (struct wpa_supplicant *)inband_drv->ctx;
	ret = wpa_driver_inband_set_ptk_kck_kek(wpa_s->drv_priv, buf, buf_len);
	return ret;
}

#if CFG_DEBUG_SETTING_DRIVER_CMD /* wpa_driver_inband_driver_cmd */
static int wpa_driver_inband_driver_cmd(void *priv, char *cmd, const u8 *buf, uint8_t buf_len)
{
	int ret = -1;

	wpa_printf(MSG_DEBUG, "%s %s", __FUNCTION__, cmd);

	inband_cmd_t inband_cmd[] = {
		{"ap_start",        mtk_ap_start},
		{"ap_stop",      mtk_ap_stop},
		{"handle_beacon",   mtk_handle_beacon},
		{"stop_ps_mode",    mtk_ps_mode},
		{"ptk_kck_kek",  mtk_set_ptk_kck_kek},
	};

	if (os_strcmp(cmd, "get_socket") == 0) {

	} else if (os_strcmp(cmd, "set_wep") == 0) {

	}

	int32_t count_index = sizeof(inband_cmd) / sizeof(inband_cmd[0]);

	for (int32_t index = 0; index < count_index; index++) {
		if (os_strcmp(cmd, inband_cmd[index].cmd) == 0) {
			ret = inband_cmd[index].fptr(priv, buf, buf_len);
			break;
		} else if (index == (count_index - 1)) {
			wpa_printf(MSG_ERROR, "Unsupport command.");
		}
	}
	return ret;
}
#endif /* wpa_driver_inband_driver_cmd */

static struct hostapd_hw_modes *wpa_driver_gen4m_get_hw_feature_data(void *priv,
		u16 *num_modes, u16 *flags, u8 *dfs)
{
	struct hostapd_hw_modes *modes, *mode;
	u16 mode_cnt = 2; // A and G modes
	int i, clen, rlen;
	uint8_t mode_g_ch_cnt = 0;
	uint8_t mode_g_chs[MAX_2G_BAND_CHN_NUM];
	uint8_t mode_a_ch_cnt = 0;
	uint8_t mode_a_chs[MAX_5G_BAND_CHN_NUM];
	bool no_dfs = global_entry->op_mode != WIFI_MODE_STA_ONLY;
	int ret = 0;

	os_memset(mode_g_chs, 0x00, MAX_2G_BAND_CHN_NUM);
	os_memset(mode_a_chs, 0x00, MAX_5G_BAND_CHN_NUM);

	ret = mtk_freertos_wpa_get_channel_list_full(BAND_2G4,
		MAX_2G_BAND_CHN_NUM, no_dfs, &mode_g_ch_cnt, mode_g_chs);
	if (ret) {
		wpa_printf(MSG_DEBUG, "Get 2.4G channel list failed\n");
		return NULL;
	}
	wpa_printf(MSG_DEBUG, "Get 2.4G channel list (%d)\n", mode_g_ch_cnt);

	ret = mtk_freertos_wpa_get_channel_list_full(BAND_5G,
		MAX_5G_BAND_CHN_NUM, no_dfs, &mode_a_ch_cnt, mode_a_chs);
	if (ret) {
		wpa_printf(MSG_DEBUG, "Get 5G channel list failed\n");
		return NULL;
	}
	wpa_printf(MSG_DEBUG, "Get 5G channel list (%d)\n", mode_a_ch_cnt);

	modes = os_zalloc(sizeof(struct hostapd_hw_modes) * mode_cnt);
	if (modes == NULL)
		return NULL;

	*num_modes = mode_cnt;
	*flags = 0;


	/* G mode */
	mode = &modes[0];
	mode->mode = HOSTAPD_MODE_IEEE80211G;
	mode->num_channels = mode_g_ch_cnt;
	mode->num_rates = 12;

	clen = mode->num_channels * sizeof(struct hostapd_channel_data);
	rlen = mode->num_rates * sizeof(int);

	mode->channels = os_zalloc(clen);
	mode->rates = os_zalloc(rlen);
	if (mode->channels == NULL || mode->rates == NULL) {
		os_free(mode->channels);
		os_free(mode->rates);
		os_free(modes);
		return NULL;
	}

	for (i = 0; i < mode->num_channels; i++) {
		mode->channels[i].chan = mode_g_chs[i];
		mode->channels[i].freq = nicChannelNum2Freq(mode_g_chs[i]) / 1000;
		mode->channels[i].allowed_bw |= HOSTAPD_CHAN_WIDTH_20;
	}

	mode->rates[0] = 10;
	mode->rates[1] = 20;
	mode->rates[2] = 55;
	mode->rates[3] = 110;
	mode->rates[4] = 60;
	mode->rates[5] = 90;
	mode->rates[6] = 120;
	mode->rates[7] = 180;
	mode->rates[8] = 240;
	mode->rates[9] = 360;
	mode->rates[10] = 480;
	mode->rates[11] = 540;


	/* A mode */
	if (mode_cnt > 1) {
		mode = &modes[1];
		mode->mode = HOSTAPD_MODE_IEEE80211A;
		mode->num_channels = mode_a_ch_cnt;
		mode->num_rates = 8;

		clen = mode->num_channels * sizeof(struct hostapd_channel_data);
		rlen = mode->num_rates * sizeof(int);

		mode->channels = os_zalloc(clen);
		mode->rates = os_zalloc(rlen);
		if (mode->channels == NULL || mode->rates == NULL) {
			os_free(mode->channels);
			os_free(mode->rates);
			/* Free mode g as well */
			mode = &modes[0];
			os_free(mode->channels);
			os_free(mode->rates);
			os_free(modes);
			return NULL;
		}

		for (i = 0; i < mode->num_channels; i++) {
			mode->channels[i].chan = mode_a_chs[i];
			mode->channels[i].freq = nicChannelNum2Freq(mode_a_chs[i]) / 1000;
			mode->channels[i].allowed_bw |= HOSTAPD_CHAN_WIDTH_20;
		}

		mode->rates[0] = 60;
		mode->rates[1] = 90;
		mode->rates[2] = 120;
		mode->rates[3] = 180;
		mode->rates[4] = 240;
		mode->rates[5] = 360;
		mode->rates[6] = 480;
		mode->rates[7] = 540;
	}

	return modes;
}
/*****************************************************************************
 *
 * WPA-Supplicant GEN4M driver ops.
 *
 *****************************************************************************/


struct wpa_driver_ops wpa_driver_gen4m_ops = {
	.name = "gen4m",
	.desc = "gen4m cmd",
	/* NOT IMPL YET */
	.init = wpa_driver_gen4m_driver_init,
	/* NOT IMPL YET */
	.send_eapol = wpa_driver_gen4m_driver_send_eapol,
	.global_init = wpa_driver_gen4m_global_init,
	.global_deinit = wpa_driver_gen4m_global_deinit,
	.set_key = wpa_driver_gen4m_set_key,
	/* NOT IMPL YET */
	/*.inband_driver_cmd = wpa_driver_inband_driver_cmd,*/
	/* NEED CHECK USE previous struct */
	.get_sock = wpa_driver_inband_get_sock_recv,
#if defined(CONFIG_AP) || defined(CONFIG_MBO)
	.get_hw_feature_data = wpa_driver_gen4m_get_hw_feature_data,
#endif
#ifdef CONFIG_AP
	.set_ap = wpa_driver_gen4m_set_ap,
	.deinit_ap = wpa_driver_gen4m_deinit_ap,
	.do_acs = wpa_driver_gen4m_do_acs,
#endif
	/* NOT IMPL YET */
	.sta_add = wpa_driver_gen4m_sta_add,
	/* NOT IMPL YET */
	.sta_set_flags = wpa_driver_gen4m_sta_set_flags,
	/* NOT IMPL YET */
	.set_supp_port = wpa_driver_gen4m_set_supp_port,
	/* NOT IMPL YET */
	.sta_remove = wpa_driver_gen4m_sta_remove,
	.scan2 = wpa_driver_gen4m_scan,
	.get_scan_results2 = wpa_driver_gen4m_get_scan_results,
	.get_ssid = wpa_driver_gen4m_get_ssid,
	.get_bssid = wpa_driver_gen4m_get_bssid,
	.get_mac_addr = wpa_driver_inband_get_mac_addr,
	.associate = wpa_driver_gen4m_associate,
	.deauthenticate = wpa_driver_gen4m_deauthenticate,
	.disassociate = wpa_driver_gen4m_disassociate,
	/* NOT IMPL YET */
	.add_pmkid = wpa_driver_inband_add_pmkid,
	/* NOT IMPL YET */
	.remove_pmkid = wpa_driver_inband_remove_pmkid,
	/* NOT IMPL YET */
	.flush_pmkid = wpa_driver_inband_flush_pmkid,
	/* NOT IMPL YET */
	.remain_on_channel = wpa_driver_inband_remain_on_channel,
	/* NOT IMPL YET */
	.cancel_remain_on_channel = wpa_driver_inband_cancel_remain_on_channel,
	/* NOT IMPL YET */
	.deinit = wpa_driver_gen4m_deinit,
	.if_add = wpa_driver_inband_if_add,
	.send_mlme = wpa_driver_inband_send_mlme,
	.send_action = wpa_driver_gen4m_send_action,
#ifdef MTK_WIFI_DIRECT_ENABLE
	.set_p2p_powersave      = wpa_driver_inband_set_p2p_powersave,
#endif
	.send_action_cancel_wait  = wpa_driver_inband_send_action_cancel_wait,
#if defined(HOSTAPD) || defined(CONFIG_AP)
	.hapd_send_eapol = wpa_driver_inband_hapd_send_eapol,
#ifdef CONFIG_WPS
	.set_ap_wps_ie          = wpa_driver_inband_set_ap_wps_ie,
#endif /* CONFIG_WPS */

#ifdef MTK_WIFI_DIRECT_ENABLE
	.probe_req_report        = wpa_driver_inband_probe_req_report,
#endif /*MTK_WIFI_DIRECT_ENABLE*/
	.sta_deauth            = wpa_driver_gen4m_sta_deauth,
	.sta_disassoc            = wpa_driver_inband_sta_disassoc,
#endif /* HOSTAPD || CONFIG_AP */
	.set_rekey_info = wpa_driver_gen4m_set_rekey_info,
	.send_external_auth_status = wpa_driver_gen4m_send_external_auth_status,
#ifdef CONFIG_IEEE80211R
	.update_ft_ies = wpa_driver_gen4m_update_ft_ies,
#endif
};


#ifdef __NOT_PORTED__
void wpa_driver_inband_handle_wow(void *priv, unsigned char b_scan)
{
	struct wpa_supplicant *wpa_s = priv;
	struct wpa_driver_gen4m_data *drv = wpa_s->drv_priv;
	uint8_t link_status = 0, bssid[WIFI_MAC_ADDRESS_LENGTH] = {0};
}
#endif // __NOT_PORTED__

void wpa_driver_gen4m_password_error(void)
{
	if (global_entry->op_mode == WIFI_MODE_STA_ONLY) {
		u16 reason_code = WLAN_REASON_WPA_PASSWORD_ERROR;
		uint8_t packet[WIFI_REASON_CODE_LENGTH] = {0};
		os_memcpy(packet, &reason_code, WIFI_REASON_CODE_LENGTH);
		wifi_api_event_trigger(WIFI_PORT_STA, WIFI_EVENT_IOT_CONNECTION_FAILED, (uint8_t *)packet, WIFI_REASON_CODE_LENGTH);
	}
	return;
}

int static do_process_wifi_event(void *priv, unsigned char evt_id, unsigned char *body, int len)
{
	wpa_printf(MSG_DEBUG, " evt_id=%d\n", evt_id);
	switch (evt_id) {
	case WIFI_EVENT_ID_IOT_DISCONNECT_AP_WAIT: {
		if (WIFI_MODE_STA_ONLY == global_entry->op_mode || WIFI_MODE_REPEATER == global_entry->op_mode) {
			struct wpa_supplicant *wpa_s;
			wpa_s = mtk_supplicant_get_interface_by_port(WIFI_PORT_STA);
			if (wpa_s == NULL)
				break;
			if (wpa_s->current_ssid) {
				if (wpa_s->wpa_state >= WPA_AUTHENTICATING)
					wpa_s->own_disconnect_req = 1;
				wpa_supplicant_deauthenticate(wpa_s,
							      WLAN_REASON_DEAUTH_LEAVING);
			}
			//mtk_supplicant_radio_onoff(0);
			//g_scan_by_supplicant = 0;
		}
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_SECURITY_WAIT: {
		uint8_t port = *body;
		uint8_t auth_mode = *(body + 1);
		uint8_t encrypt_type = *(body + 2);
		mtk_supplicant_set_security(port, auth_mode, encrypt_type);
		wpa_printf(MSG_DEBUG,
			   "port=%d,auth_mode=%d encrypt_type=%d \n", port,
			   auth_mode, encrypt_type);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_MULTI_SECURITY_WAIT: {
		uint8_t port = *body;
		uint8_t encrypt_type = *(body + 5);
		uint32_t key_mgmt = 0;
		for (int i = 4; i > 0; --i)
			key_mgmt = (key_mgmt << 8) | *(body + i);
		mtk_supplicant_set_multi_security(port, key_mgmt, encrypt_type);
		wpa_printf(MSG_DEBUG,
			   "port=%d,key_mgmt=%d encrypt_type=%d \n",
			   port, key_mgmt, encrypt_type);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_IEEE80211W_WAIT: {
		uint8_t port = *body;
		uint8_t ieee80211w = *(body + 1);
		mtk_supplicant_set_ieee80211w(port, ieee80211w);
		wpa_printf(MSG_DEBUG,
			   "port=%d,ieee80211w=%d \n",
			   port, ieee80211w);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_PROTO_WAIT: {
		uint8_t port = *body;
		uint8_t proto = *(body + 1);
		mtk_supplicant_set_proto(port, proto);
		wpa_printf(MSG_DEBUG,
			   "port=%d,proto=%d \n",
			   port, proto);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_PMK_WAIT: {
		uint8_t port = *body;
		uint8_t hex_pmk[WIFI_LENGTH_PMK] = {0};
		os_memcpy(&hex_pmk, (body + 1), WIFI_LENGTH_PMK);
		wpa_hexdump(MSG_DEBUG, " SET PMK: ", body, len);
		wpa_hexdump(MSG_DEBUG, " hex_pmk: ", hex_pmk, WIFI_LENGTH_PMK);
		mtk_supplicant_set_pmk(port, hex_pmk);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_WPAPSK_WAIT: {
		uint8_t port = *body;
		uint8_t passphrase_length = *(body + 1);
		uint8_t passphrase[WIFI_LENGTH_PASSPHRASE + 1] = {0};
		os_memcpy(passphrase, (body + 2), passphrase_length);
		passphrase[passphrase_length] = '\0';
		wpa_hexdump(MSG_DEBUG, " SET WPAPSK: ", body, len);
		wpa_printf(MSG_DEBUG, "port=%d,passphrase_length=%d passphrase=%s \n", port, passphrase_length, passphrase);
		mtk_supplicant_set_wpa_psk_key(port, passphrase, passphrase_length);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_SSID_WAIT: {
		uint8_t port = *body;
		uint8_t ssid_length = *(body + 1);
		uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
		os_memcpy(ssid, (body + 2), ssid_length);
		ssid[ssid_length] = '\0';
		wpa_hexdump(MSG_DEBUG, " SET SSID: ", body, len);
		wpa_printf(MSG_DEBUG, "port=%d,ssid_length=%d ssid=%s \n", port, ssid_length, ssid);
		mtk_supplicant_set_ssid(port, ssid, ssid_length);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_BSSID_WAIT: {
		uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH] = {0};
		os_memcpy(bssid, body, WIFI_MAC_ADDRESS_LENGTH);
		wpa_hexdump(MSG_DEBUG, " SET BSSID: ", body, len);
		wpa_printf(MSG_DEBUG, "bssid: %02x:%02x:%02x:%02x:%02x:%02x \n", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
		mtk_supplicant_set_bssid(bssid);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_BSS_PREFERENCE: {
#ifdef CONFIG_BAND_PREFERENCE
		uint8_t mode = *body;
		wpa_printf(MSG_DEBUG, " SET BSS PREFERENCE: ", mode);
		mtk_supplicant_set_bss_preference(mode);
#else
		wpa_printf(MSG_DEBUG, "Not supported.");
#endif
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_STA_RADIO_ONOFF: {
		uint8_t  radio_onoff = *body;
		mtk_supplicant_radio_onoff(radio_onoff);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_WEPKEY_WAIT: {
		wifi_wep_key_t wep_keys;
		uint8_t port = *body;
		os_memset(&wep_keys, 0x00, sizeof(wifi_wep_key_t));
		os_memcpy(&wep_keys, (body + 1), sizeof(wifi_wep_key_t));
		wpa_hexdump(MSG_DEBUG, " SET WEPKEY: ", body, len);
		wpa_printf(MSG_DEBUG, "wep key =(%s, %s, %s, %s) key id=%d, len=(%d, %d, %d, %d) done \n",
			   wep_keys.wep_key[0], wep_keys.wep_key[1], wep_keys.wep_key[2], wep_keys.wep_key[3],
			   wep_keys.wep_tx_key_index,
			   wep_keys.wep_key_length[0], wep_keys.wep_key_length[1], wep_keys.wep_key_length[2], wep_keys.wep_key_length[3]);
		mtk_supplicant_set_wep_key(port, &wep_keys);
		wifi_api2supp_result_semphr_give();
	}
	break;
	case WIFI_EVENT_ID_IOT_REGISTER_RX_HANDLER: {
		/*when smart connect is ongoing, should not do supplicant scan and auto reconnected until smtcn done.
		side effect is: when STA mode register rx handler, will not do reconnected after link down utill rx handler unregister.*/
		if (global_entry->op_mode == WIFI_MODE_STA_ONLY) {
			struct wpa_supplicant *wpa_s;
			wpa_s = mtk_supplicant_get_interface_by_port(WIFI_PORT_STA);
			if (wpa_s == NULL) {
				wpa_printf(MSG_ERROR, "Get ifac failed, opmode: %u\n", global_entry->op_mode);
				return -1;
			}
			if (!g_rx_handler_mode) {
				wpa_s->auto_reconnect_disabled = 1;
				wpa_s->disconnected = 1;
				wpa_supplicant_cancel_sched_scan(wpa_s);
				wpa_supplicant_cancel_scan(wpa_s);
			}
		}
	}
	break;
	case WIFI_EVENT_ID_IOT_UNREGISTER_RX_HANDLER: {
		if (global_entry->op_mode == WIFI_MODE_STA_ONLY) {
			struct wpa_supplicant *wpa_s;
			wpa_s = mtk_supplicant_get_interface_by_port(WIFI_PORT_STA);
			if (wpa_s == NULL) {
				wpa_printf(MSG_ERROR, "Get ifac failed, opmode: %u\n", global_entry->op_mode);
				return -1;
			}
			wpa_s->auto_reconnect_disabled = 0;
			wpa_s->disconnected = 0;
		}
	}
	break;
	case WIFI_EVENT_ID_IOT_SET_LISTEN: {
		mtk_freertos_wpa_set_power(g_wifi_config_ext.sta_listen_interval_present,
								   g_wifi_config_ext.sta_listen_interval,
								   g_wifi_config_ext.sta_power_save_mode_present,
								   g_wifi_config_ext.sta_power_save_mode);
	}
	break;
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
	case WIFI_EVENT_ID_IOT_SET_NON_PREF_CAHN_WAIT: {
		if (global_entry->op_mode == WIFI_MODE_STA_ONLY) {
			uint8_t non_pref_chan_length = *(body + 1);
			int ret;
			struct wpa_supplicant *wpa_s;
			char non_pref_chan[WIFI_MAX_LENGTH_OF_NON_PREF_CHAN_BUF + 1];

			wpa_s = mtk_supplicant_get_interface_by_port(WIFI_PORT_STA);
			if (wpa_s == NULL) {
				wpa_printf(MSG_ERROR, "set non_pref_chan failed");
				wifi_api2supp_result_semphr_give();
				return -1;
			}
			os_memset(non_pref_chan, 0,
				  WIFI_MAX_LENGTH_OF_NON_PREF_CHAN_BUF + 1);
			os_memcpy(non_pref_chan, body + 2, non_pref_chan_length);
			wpa_printf(MSG_DEBUG, "SET NON_PREF_CHAN: %s", non_pref_chan);
			ret = wpas_mbo_update_non_pref_chan(wpa_s, non_pref_chan);
			wpa_printf(MSG_INFO, "NON_PREF_CHAN: ret:%d", ret);
		}
		wifi_api2supp_result_semphr_give();
	}
	break;
#endif
	default:
		break;
	}

	return 0;
}

int mtk_supplicant_is_connect_ft_ap(void)
{
	struct wpa_supplicant *wpa_s = wpa_supplicant_get_iface(global_entry, sta_ifname);
	struct wpa_sm *sm;

	if (wpa_s && wpa_s->wpa)
		sm = wpa_s->wpa;

	return wpa_key_mgmt_ft(sm->key_mgmt);
}


#if 0 /* TODO: fixme move from gen4m to here */
int wifi_evt_handler_gen4m(unsigned int inf, unsigned char *body, int len)
{
	iwreq_data_t            *wrqu = (iwreq_data_t *)body;
	uint32_t                cmd;
	wifi_event_handler_t    handler = NULL;

	if (len < (sizeof(iwreq_data_t) + sizeof(cmd))) {
		LOG_E(wifi, "ERROR! Invalid wlan evt received, len(%d) < %d",
		      len, sizeof(iwreq_data_t) + 4);
		return -1;
	}

	if (inf >= MAX_INTERFACE_NUM) {
		LOG_E(wifi, "ERROR! Wlan evt received from invalid interface(=%d).", inf);
		return -1;
	}

#ifdef MTK_MINISUPP_ENABLE
	if (__process_global_event[inf].ready == 0) {
		if (wrqu->data.flags == IW_SCAN_COMPLETED_EVENT_FLAG) { //pengfei+, for Sta_Fast_Link feature
			g_scan_done_flag = 1;
		}
		LOG_E(wifi, "Supplicant is not ready to receive event from interface(=%d) yet.", inf);
		return -1;
	}
#endif /* MTK_MINISUPP_ENABLE */

	/* payload: IWREQ + CMD(4) + extra */
	cmd = *(uint32_t *)(&body[sizeof(iwreq_data_t)]);

	LOG_I(wifi, "[ra%d] wireless evt cmd(0x%x), flags(0x%x)...", inf, (unsigned int)cmd, wrqu->data.flags);

	if (cmd == IW_CUSTOM_EVENT_FLAG) {

#ifdef MTK_WIFI_ROM_ENABLE
		if (wrqu->data.flags == RT_RSNA_COMPLETE_EVENT_FLAG) {
			unsigned char *extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			if (__process_global_event[WIFI_PORT_STA].func != NULL) {
				__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_RSNA_COMPLETE, extra, wrqu->data.length);
			}
		}

		if (wrqu->data.flags == RT_AUTH_START_AFTER_JOIN_EVENT_FLAG) {
			unsigned char *extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			if (__process_global_event[WIFI_PORT_STA].func != NULL) {
				__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_AUTH_START_AFTER_JOIN, extra, wrqu->data.length);
			}
		}
#endif


		if (wrqu->data.flags == IW_SCAN_COMPLETED_EVENT_FLAG) {
			g_scanning = 0;
#ifdef MTK_MINISUPP_ENABLE
			sort_scan_array_by_rssi();
			if (__process_global_event[WIFI_PORT_STA].func != NULL) {
				__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_SCAN_LIST, NULL, 0);
			}
#endif /* MTK_MINISUPP_ENABLE */
#if 0   /* TODO fixme KOKO */
			if (g_scan_by_app == 1) {
				handler = wifi_rx_event_handler[WIFI_EVENT_IOT_SCAN_COMPLETE];
				if (handler != NULL) {
					//Castro+
					if (handler(WIFI_EVENT_IOT_SCAN_COMPLETE, g_scan_ssid, g_scan_ssid_len) == 0) {
						LOG_E(wifi, "wifi_wlan_evt_handler:WIFI_EVENT_IOT_SCAN_COMPLETE, fail to call wifi_rx_event_handler()!");
					}
				}
#ifdef MTK_WIFI_ROM_ENABLE
				if (__process_global_event[WIFI_PORT_STA].func != NULL) {
					__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_USER_SCAN_DONE, NULL, 0);
				}
#endif
			}
#ifdef MTK_WIFI_ROM_ENABLE
			else {
				if (__process_global_event[WIFI_PORT_STA].func != NULL) {
					__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_SCAN_LIST, NULL, 0);
				}
			}
#endif
			g_scan_by_app = 0;
#endif
		}

#if 0
		if (wrqu->data.flags == RT_REQIE_EVENT_FLAG) {
			unsigned char *extra = NULL;
			LOG_I(wifi, "======== [ASSOCIATE REQ IEs: %d bytes]  =======", wrqu->data.length);
			if (wrqu->data.length) {
				extra = body + sizeof(union iwreq_data_t) + sizeof(cmd);
			}
			//process_global_event(EVENT_ID_IOT_ASSOCIATE_IE, extra, wrqu->data.length);
			if (__process_global_event != NULL) {
				__process_global_event(EVENT_ID_IOT_ASSOCIATE_IE, extra, wrqu->data.length);
			}
		}
#endif

#ifdef MTK_WIFI_DIRECT_ENABLE
		if (wrqu->data.flags == IW_CANCEL_REMAIN_ON_CHANNEL_FLAG) {
			LOG_I(wifi, "======== [P2P Listen Finish]  =======");
			if (__process_global_event != NULL) {
				if (__process_global_event[WIFI_PORT_STA].func != NULL) {
					__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_CANCEL_REMAIN_ON_CHANNEL, NULL, 0);
				}
			}
		}

#if 1/*Only P2P RX public action (category=0x04, action filed=0x09, OUI=0x50-6F-9A)
                        802.11 raw frame will come from WIFI_HIF_RX_PORT0_IDX*/
		if (wrqu->data.flags == IW_RX_ACTION_FRAME_EVENT_FLAG) {
			LOG_I(wifi, "======== [IW_RX_ACTION_FRAME]  =======");
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}

			if (__process_global_event != NULL) {
				if (__process_global_event[WIFI_PORT_STA].func != NULL) {
					__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_RX_ACTION, extra, wrqu->data.length);
				}
			}
		}
#endif

		if (wrqu->data.flags == IW_TX_ACTION_STATUS_EVENT_FLAG) {
			LOG_I(wifi, "======== [IW_TX_ACTION_STATUS]  =======");
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (__process_global_event != NULL) {
				if (__process_global_event[WIFI_PORT_STA].func != NULL) {
					__process_global_event[WIFI_PORT_STA].func(__process_global_event[WIFI_PORT_STA].priv, WIFI_EVENT_ID_IOT_TX_STATUS, extra, wrqu->data.length);
				}
			}
		}
#endif /*MTK_WIFI_DIRECT_ENABLE*/

		if (wrqu->data.flags == IW_MIC_ERROR_EVENT_FLAG) {
			unsigned char *extra = NULL;

			LOG_I(wifi, "-->IW_MIC_ERROR_EVENT_FLAG");

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}

			if (extra) {
				LOG_I(wifi, "extra - %02x:%02x:%02x:%02x:%02x:%02x",
				      extra[0], extra[1], extra[2], extra[3], extra[4], extra[5]);
				LOG_I(wifi, "unicast  = 0x%x", extra[6]);
			}

#ifdef MTK_MINISUPP_ENABLE
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_MIC_ERROR, extra, wrqu->data.length);
			}
#endif /* MTK_MINISUPP_ENABLE */
		}

		if (wrqu->data.flags == IW_ASSOC_EVENT_FLAG) {
			unsigned char *extra = NULL;
			unsigned char op_mode;
			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}

#if 1
			op_mode = WIFI_MODE_STA_ONLY; // fixme, get mode via driver API.
#else
			/* In STA mode, when connect AP, send Bssid */
			if (wifi_config_get_opmode(&op_mode) < 0) {
				op_mode = 0;
			}
#endif

			uint8_t packet[WIFI_MAC_ADDRESS_LENGTH + 1] = {0};
			if (extra) {
				os_memcpy(packet, extra, WIFI_MAC_ADDRESS_LENGTH);
			} else {
				LOG_E(wifi, "ERROR! IW_ASSOC_EVENT_FLAG extra is NULL");
				return -1;
			}

			packet[WIFI_MAC_ADDRESS_LENGTH] = inf;

#ifdef MTK_WIFI_DIRECT_ENABLE
			if ((op_mode == WIFI_MODE_STA_ONLY) ||
					((op_mode == WIFI_MODE_P2P_ONLY) && ((g_p2p_role == P2P_DEVICE) || (g_p2p_role == P2P_CLIENT))))
#else
			if (op_mode == WIFI_MODE_STA_ONLY)
#endif
			{
#if 0   /* TODO fixme KOKO */
				handler = wifi_rx_event_handler[WIFI_EVENT_IOT_CONNECTED];
				if (handler != NULL) {
					if (extra) {
						LOG_I(wifi, "IW_ASSOC_EVENT_FLAG: CONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x",
						      extra[0], extra[1], extra[2], extra[3], extra[4], extra[5]);
						handler(WIFI_EVENT_IOT_CONNECTED, packet, sizeof(packet));
					}
				}
#else
				LOG_I(wifi, "IW_ASSOC_EVENT_FLAG: CONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x"
				      , 0xde, 0xad, 0xde, 0xad, 0xde, 0xad);
#endif
			}

#if 0   /* TODO fixme KOKO */
			if (op_mode == WIFI_MODE_REPEATER) {
				handler = wifi_rx_event_handler[WIFI_EVENT_IOT_CONNECTED];
				if (handler != NULL) {
					if (extra) {
						LOG_I(wifi, "IW_ASSOC_EVENT_FLAG: CONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x",
						      extra[0], extra[1], extra[2], extra[3], extra[4], extra[5]);
						LOG_I(wifi, "[%s]Repeater Mode QOS byte flag from N9: %d", __FUNCTION__, extra[6]);
						handler(WIFI_EVENT_IOT_CONNECTED, packet, sizeof(packet));
					}
				}
			}
#endif
#ifdef MTK_MINISUPP_ENABLE
			op_mode = __process_global_event[inf].op_mode;
#ifdef MTK_WIFI_DIRECT_ENABLE
			if ((op_mode == WIFI_MODE_AP_ONLY) ||
					((op_mode == WIFI_MODE_P2P_ONLY) && (g_p2p_role == P2P_GO)))
#else
			if (op_mode == WIFI_MODE_AP_ONLY)
#endif
			{
				/* Ignore this event in AP mode. */
				return 0;
			}
#endif /* MTK_MINISUPP_ENABLE */

			LOG_I(wifi, "======== [ASSOCIATED] !! =======");
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_CONNECTED, extra, wrqu->data.length);
			}
		}
#ifdef MTK_WIFI_ROM_ENABLE
		if (wrqu->data.flags ==  RT_FLOW_CONTROL_EVENT_FLAG) {
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_FLOW_CONTROL, extra, wrqu->data.length);
			}

		}
		if (wrqu->data.flags ==  RT_AP_START_UP_EVENT_FLAG) {
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_AP_LINK_UP, extra, wrqu->data.length);
			}

		}

		if (wrqu->data.flags ==  RT_ASSOC_TIMEOUT_EVENT_FLAG) {
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_ASSOC_TIMEOUT, extra, wrqu->data.length);
			}

		}

		if (wrqu->data.flags ==  RT_RSNA_TIMEOUT_EVENT_FLAG) {
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_RSNA_TIMEOUT, extra, wrqu->data.length);
			}
		}

		if (wrqu->data.flags ==  RT_BSSID_NOT_MATCH_EVENT_FLAG) {
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_BSSID_NOT_MATCH, extra, wrqu->data.length);
			}
		}

		if (wrqu->data.flags ==  RT_CIPPHER_ERROR_EVENT_FLAG) {
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_CIPPHER, extra, wrqu->data.length);
			}
		}
#ifdef MTK_WIFI_ROM_ENABLE
		wifi_wps_wlan_evt_handler(body, len);

		wifi_repeater_wlan_evt_handler(body, len);
#endif

#endif
		if (wrqu->data.flags == IW_STA_LINKUP_EVENT_FLAG) {
			unsigned char op_mode = 0;

#ifdef MTK_MINISUPP_ENABLE
			unsigned char *extra = NULL;

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
#endif
			if (wifi_config_get_opmode(&op_mode) < 0) {
				op_mode = __process_global_event[inf].op_mode;
			}

			if (__process_global_event[inf].func != NULL) {
				if (op_mode == WIFI_MODE_AP_ONLY) {
#ifdef MTK_MINISUPP_ENABLE
					__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_AP_LINK_UP, extra, wrqu->data.length);
#endif
					LOG_I(wifi, ">>>>>>>  LINKUP COMPLETED !! (op_mode=%d)  <<<<<<<<", op_mode);
				} else if (op_mode == WIFI_MODE_REPEATER) {
					LOG_I(wifi, ">>>>>>>  LINKUP COMPLETED !! (op_mode=%d)  <<<<<<<<", op_mode);
#ifdef MTK_MINISUPP_ENABLE
					if (__process_global_event[inf].op_mode == WIFI_MODE_STA_ONLY) {

						__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_CONNECTED, extra, wrqu->data.length);

					} else {
						__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_AP_LINK_UP, extra, wrqu->data.length);
					}
#endif
				}
#ifdef MTK_MINISUPP_ENABLE
#ifdef MTK_WIFI_DIRECT_ENABLE
				else if (op_mode == WIFI_MODE_P2P_ONLY) {
					LOG_I(wifi, ">>>>>>>  LINKUP COMPLETED !! (op_mode=%d)(g_p2p_role=%d)  <<<<<<<<", op_mode, g_p2p_role);
					if ((g_p2p_role == P2P_DEVICE) || (g_p2p_role == P2P_CLIENT)) {
						__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_CONNECTED, extra, wrqu->data.length);
					} else if (g_p2p_role == P2P_GO) {
						__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_AP_LINK_UP, extra, wrqu->data.length);
					}
				}
#endif //MTK_WIFI_DIRECT_ENABLE
#endif /* MTK_MINISUPP_ENABLE */
			}
#if 0
			/* In STA mode, when connect AP, send Bssid */
			handler = wifi_rx_event_handler[WIFI_EVENT_IOT_CONNECTED];
			if (handler != NULL) {
				if (extra) {
					LOG_I(wifi, "CONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x",
					      extra[0], extra[1], extra[2], extra[3], extra[4], extra[5]);
					if (handler(WIFI_EVENT_IOT_CONNECTED, extra, WIFI_MAC_ADDRESS_LENGTH) == 0) {
						LOG_E(wifi, "wifi_wlan_evt_handler:WIFI_EVENT_IOT_CONNECTED, fail to call wifi_rx_event_handler()!");
					}
				}
			}
#endif
		}

		if (wrqu->data.flags == IW_STA_LINKDOWN_EVENT_FLAG) {
#ifdef MTK_MINISUPP_ENABLE
			unsigned char op_mode;

			op_mode = __process_global_event[inf].op_mode;
			if (__process_global_event[inf].func != NULL) {
#ifdef MTK_WIFI_DIRECT_ENABLE
				if ((op_mode == WIFI_MODE_STA_ONLY) ||
						((op_mode == WIFI_MODE_P2P_ONLY) && ((g_p2p_role == P2P_DEVICE) || (g_p2p_role == P2P_CLIENT))))
#else
				if (op_mode == WIFI_MODE_STA_ONLY)
#endif
				{
					LOG_W(wifi, ">>>>>>>  Link Down !! <<<<<<<<");
					__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_DEAUTH, NULL, 0);
				}
			}
#endif /* MTK_MINISUPP_ENABLE */

#ifdef MTK_WIFI_ROM_ENABLE
			if (__process_global_event[inf].func != NULL)
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_LINK_DOWN, NULL, 0);
#endif

#if 0   /* TODO fixme KOKO */
			handler = wifi_rx_event_handler[WIFI_EVENT_IOT_DISCONNECTED];
			if (handler != NULL) {
				uint8_t packet[WIFI_MAC_ADDRESS_LENGTH + 1] = {0};
				packet[WIFI_MAC_ADDRESS_LENGTH] = inf;
				handler(WIFI_EVENT_IOT_DISCONNECTED, packet, sizeof(packet));
			}
#endif
		}
#ifdef MTK_WIFI_ROM_ENABLE
		if (wrqu->data.flags == IW_CONNECTION_RETRY_FAIL_EVENT_FLAG) {
			unsigned char *extra = NULL;
			unsigned char  reason = 0;
			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
				reason = extra[0];
			}

			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_CONNECTION_RETRY_FAIL, &reason, wrqu->data.length);
			}

			LOG_I(wifi, "fail to get response!");
			wifi_event_handler handler = NULL;
			uint8_t reason_code = WIFI_REASON_CODE_PREV_AUTH_INVALID;
			handler = wifi_rx_event_handler[WIFI_EVENT_IOT_CONNECTION_FAILED];
			if (handler != NULL) {
				uint8_t packet[3] = {0};
				packet[0] = WIFI_PORT_STA;
				os_memcpy(packet + 1, &reason_code, 2);
				handler(WIFI_EVENT_IOT_CONNECTION_FAILED, packet, sizeof(packet));
			}

		}
#endif

		if (wrqu->data.flags == IW_DEAUTH_EVENT_FLAG) {
			unsigned char *extra = NULL;
			uint8_t data[8];

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}

			if (extra != NULL) {
				os_memcpy(data, extra, WIFI_MAC_ADDRESS_LENGTH);
				os_memcpy(data + WIFI_MAC_ADDRESS_LENGTH, body - 2, 2);
			}
			LOG_W(wifi, "======== [DEAUTH] !! =======");
#ifdef MTK_MINISUPP_ENABLE
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_DEAUTH, extra, wrqu->data.length);
			}
#endif /* MTK_MINISUPP_ENABLE */

#if 0   /* TODO fixme KOKO */
			handler = wifi_rx_event_handler[WIFI_EVENT_IOT_DISCONNECTED];
			if (handler != NULL) {
				if (extra) {
					LOG_E(wifi, "DISCONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x",
					      extra[0], extra[1], extra[2], extra[3], extra[4], extra[5]);
					uint8_t packet[WIFI_MAC_ADDRESS_LENGTH + 1] = {0};
					os_memcpy(packet, extra, WIFI_MAC_ADDRESS_LENGTH);
#ifdef MTK_WIFI_ROM_ENABLE
					packet[WIFI_MAC_ADDRESS_LENGTH] = fw_mlme_get_inf_by_addr(extra);
#else
					packet[WIFI_MAC_ADDRESS_LENGTH] = inf;

#endif

					handler(WIFI_EVENT_IOT_DISCONNECTED, packet, sizeof(packet));
				}
			}
#else
			LOG_E(wifi, "DISCONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x"
			      , 0xde, 0xad, 0xde, 0xad, 0xde, 0xad);
#endif
#ifdef MTK_WIFI_ROM_ENABLE
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_DEAUTH, data, 7);
			}
#endif
		}

		if (wrqu->data.flags == IW_DISASSOC_EVENT_FLAG) {
			unsigned char *extra = NULL;
			uint8_t data[8];

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (extra != NULL) {
				os_memcpy(data, extra, WIFI_MAC_ADDRESS_LENGTH);
				os_memcpy(data + WIFI_MAC_ADDRESS_LENGTH, body - 2, 2);
			}
			LOG_W(wifi, "======== [DISASSOC] !! =======");
#ifdef MTK_MINISUPP_ENABLE
			if (__process_global_event[inf].func != NULL) {
				__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_DISASSOC, extra, wrqu->data.length);
			}
#endif /* MTK_MINISUPP_ENABLE */
#if 0   /* TODO fixme KOKO */
			handler = wifi_rx_event_handler[WIFI_EVENT_IOT_DISCONNECTED];
			if (handler != NULL) {
				if (extra) {
					LOG_I(wifi, "DISCONNECTED MAC - %02x:%02x:%02x:%02x:%02x:%02x",
					      extra[0], extra[1], extra[2], extra[3], extra[4], extra[5]);
					uint8_t packet[WIFI_MAC_ADDRESS_LENGTH + 1] = {0};
					os_memcpy(packet, extra, WIFI_MAC_ADDRESS_LENGTH);
#ifdef MTK_WIFI_ROM_ENABLE
					packet[WIFI_MAC_ADDRESS_LENGTH] = fw_mlme_get_inf_by_addr(extra);
#else
					packet[WIFI_MAC_ADDRESS_LENGTH] = inf;

#endif

					handler(WIFI_EVENT_IOT_DISCONNECTED, packet, sizeof(packet));
				}
			}
#endif
		}

		if (wrqu->data.flags == IW_WPAS_APCLI_RELOAD_FLAG) {
			LOG_I(wifi, "====IW_WPAS_APCLI_RELOAD_FLAG====");
#ifdef MTK_MINISUPP_ENABLE
			if (__process_global_event[0].func != NULL) {
				__process_global_event[0].func(__process_global_event[0].priv, WIFI_EVENT_ID_IOT_APCLI_WPAS_RELOAD, NULL, 0);
			}
#endif /* MTK_MINISUPP_ENABLE */
		}
#if 0//def MTK_ANT_DIV_ENABLE
		if (wrqu->data.flags == IW_ANTENNA_MONITOR_EVENT_FLAG) {
			unsigned char *extra = NULL;
			uint32_t rssi_diff = 0;
			wifi_antenna_status_t antenna_status = {0};
			LOG_I(wifi, "ANTENNA_MONITOR_EVENT:0x0288");

			if (wrqu->data.length) {
				extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
			}
			if (extra != NULL) {
				os_memcpy(&antenna_status.antenna_number, extra, sizeof(uint8_t));
				os_memcpy(&antenna_status.ant0_rssi, extra + 1, sizeof(int8_t));
				os_memcpy(&antenna_status.ant1_rssi, extra + 2, sizeof(int8_t));
				os_memcpy(&antenna_status.pre_selected_rssi, extra + 3, sizeof(int8_t));

				rssi_diff = (uint32_t)((antenna_status.ant0_rssi >= antenna_status.ant1_rssi) ?
						       (antenna_status.ant0_rssi - antenna_status.ant1_rssi) : (antenna_status.ant1_rssi - antenna_status.ant0_rssi));

				LOG_I(wifi, "the new antenna number:%d, ant0_rssi:%d, ant1_rssi:%d, rssi_diff:%d, prev_selected_rssi:%d", antenna_status.antenna_number, antenna_status.ant0_rssi, antenna_status.ant1_rssi,  rssi_diff, antenna_status.pre_selected_rssi);

				if (g_wifi_init_callback.wifi_record_ant_status != NULL) {
					if (0 > g_wifi_init_callback.wifi_record_ant_status(&antenna_status)) {
						LOG_E(wifi, "record antenna status fail!");
					}
					LOG_I(wifi, "record antenna status done");
				}
			} else {
				LOG_E(wifi, "Payload is NULL");
			}
		}
#endif/*MTK_ANT_DIV_ENABLE*/
	} else if (cmd == IW_ASSOCREQ_IE_EVENT_FLAG) {
#ifdef MTK_MINISUPP_ENABLE
		unsigned char *extra = NULL;
		if (wrqu->data.length) {
			extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
		}
		LOG_W(wifi, "====IW_ASSOCREQ_IE_EVENT_FLAG====");
		if (__process_global_event[inf].func != NULL) {
			__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_ASSOCIATED, extra, wrqu->data.length);
		}
#endif /* MTK_MINISUPP_ENABLE */
#if 0   /* TODO fixme KOKO */
		/* In AP mode, when sta connect, send sta's mac addr */
		handler = wifi_rx_event_handler[WIFI_EVENT_IOT_CONNECTED];
		if (handler != NULL) {
			if (extra) {
				LOG_I(wifi, "CONNECTED STA MAC - %02x:%02x:%02x:%02x:%02x:%02x",
				      extra[0], extra[1], extra[2], extra[3], extra[4], extra[5]);
				if (handler(WIFI_EVENT_IOT_CONNECTED, extra, WIFI_MAC_ADDRESS_LENGTH) == 0) {
					LOG_E(wifi, "wifi_wlan_evt_handler:WIFI_EVENT_IOT_CONNECTED, fail to call wifi_rx_event_handler()!");
				}
			}
		}
#endif
	} else if (cmd == IW_REASSOCREQ_IE_EVENT_FLAG) {
#ifdef MTK_MINISUPP_ENABLE
		unsigned char *extra = NULL;
		if (wrqu->data.length) {
			extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
		}

		if (__process_global_event[inf].func != NULL) {
			__process_global_event[inf].func(__process_global_event[inf].priv, WIFI_EVENT_ID_IOT_REASSOCIATED, extra, wrqu->data.length);
		}
#endif /* MTK_MINISUPP_ENABLE */
	}
	return 0;
}
#endif

#endif
