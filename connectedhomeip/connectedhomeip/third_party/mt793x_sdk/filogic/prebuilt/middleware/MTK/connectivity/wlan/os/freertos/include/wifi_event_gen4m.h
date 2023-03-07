/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its
 * licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek
 * Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so
 * within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
 * DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL
 * PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR
 * ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A
 * PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER
 * WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __WIFI_EVENT_H__
#define __WIFI_EVENT_H__

#include "wifi_api_ex.h"
#include <stdint.h>

#include "mac.h"


#define IW_CUSTOM_EVENT_FLAG 0x8C02
#define IW_ASSOCREQ_IE_EVENT_FLAG 0x8C07
#define IW_REASSOCREQ_IE_EVENT_FLAG 0x8C0A

#define IW_CONNECTION_RETRY_FAIL_EVENT_FLAG 0x0199
#define IW_SYS_EVENT_FLAG_START 0x0200
#define IW_ASSOC_EVENT_FLAG 0x0200
#define IW_DISASSOC_EVENT_FLAG 0x0201
#define IW_DEAUTH_EVENT_FLAG 0x0202
#define IW_EXTERNAL_AUTH_EVENT_FLAG 0x0206
#define IW_TX_STATUS_EVENT_FLAG 0x0207
#define IW_RX_MGMT_EVENT_FLAG 0x0208

#define IW_MIC_ERROR_EVENT_FLAG 0x0209
#define IW_STA_LINKUP_EVENT_FLAG 0x020F
#define IW_STA_LINKDOWN_EVENT_FLAG 0x0210
#define IW_SCAN_COMPLETED_EVENT_FLAG 0x0211
#define IW_CANCEL_REMAIN_ON_CHANNEL_FLAG 0x0212
#define IW_TX_ACTION_STATUS_EVENT_FLAG 0x0213
#define IW_RX_ACTION_FRAME_EVENT_FLAG 0x0214
#define IW_ASSOC_REJECT_EVENT_FLAG 0x0215
#define IW_ACS_EVENT_FLAG 0x0216
#define IW_DISASSOC_LOCALLY_EVENT_FLAG 0x0217
#define IW_FT_INDICATION_EVENT_FLAG 0x0218
#define IW_SET_LISTEN_EVENT_FLAG 0x0219

/* Define EVENT ID */
#define WIFI_EVENT_ID_IOT_SCAN_LIST 0x01
#define WIFI_EVENT_ID_IOT_CONNECTED 0x02
#define WIFI_EVENT_ID_IOT_ASSOCIATED 0x03
#define WIFI_EVENT_ID_IOT_DEAUTH 0x04
#define WIFI_EVENT_ID_IOT_DISASSOC 0x05
#define WIFI_EVENT_ID_IOT_RELOAD_CONFIGURATION  0x06
#define WIFI_EVENT_ID_IOT_EXTERNAL_AUTH 0x07
#define WIFI_EVENT_ID_IOT_TX_STATUS 0x08
#define WIFI_EVENT_ID_IOT_RX_MGMT 0x09
#define WIFI_EVENT_ID_IOT_ASSOC_REJECT 0x0A
#define WIFI_EVENT_ID_IOT_ACS 0x0B
#define WIFI_EVENT_ID_IOT_DISASSOC_LOCALLY 0x0C
#define WIFI_EVENT_ID_IOT_FT_INDICATION 0x0D
#define WIFI_EVENT_ID_IOT_SET_LISTEN 0x0E

/* Define EVENT ID from wifi_api to minisupp (v0.01) */
#define   WIFI_EVENT_ID_IOT_WPS_SET_CRDENTIAL_HDL_WAIT  0x3D
/* WPS set crdential_handler, need wait, Format: buf[strlen(cmd)+1]=cmd+\0 */
#define   WIFI_EVENT_ID_IOT_WPS_SET_DEVICE_INFO_WAIT    0x3E
/* WPS set device info, need wait, Format: buf[sizeof(wifi_wps_device_info_t)]
=device_info */
#define   WIFI_EVENT_ID_IOT_WPS_SET_AUTO_CONN_WAIT      0x3F
/* WPS set auto connect, need wait, Format: buf[0] = auto_conn */
#define   WIFI_EVENT_ID_IOT_WPS_REG_PIN        0x40
/* WPS reg pin, Format: buf[strlen(cmd)+1]=cmd+\0 */
#define   WIFI_EVENT_ID_IOT_SET_SSID_BSSID_WAIT         0x41
/* Set ssid, bssid, need wait, Format: buf[1 + WIFI_MAX_LENGTH_OF_SSID +
WIFI_MAC_ADDRESS_LENGTH] = 1byte ssidlen+ssid+bssid */
#define   WIFI_EVENT_ID_IOT_SET_WEPKEY_WAIT    0x42
/* Set wepkey, need wait, Format: buf[1 + sizeof(wifi_wep_key_t) ] =
1byte port+wepkey */
#define   WIFI_EVENT_ID_IOT_SET_BSSID_WAIT     0x43
/* Set bssid, need wait, Format: buf[WIFI_MAC_ADDRESS_LENGTH ] = bssid */
#define   WIFI_EVENT_ID_IOT_SET_SSID_WAIT      0x44
/* Set ssid, need wait, Format: buf[2 + WIFI_MAX_LENGTH_OF_SSID] =
1byte port+1byte ssidlen+ssid */
#define   WIFI_EVENT_ID_IOT_SET_WPAPSK_WAIT    0x45
/* Set wpapsk, need wait,  Format: buf[2 + WIFI_LENGTH_PASSPHRASE] =
1byte port+1byte wpapsklen+wpapsk */
#define   WIFI_EVENT_ID_IOT_SET_PMK_WAIT       0x46
/* Set PMK, need wait, Format: buf[1 + WIFI_LENGTH_PMK] = 1byte port+pmk */
#define   WIFI_EVENT_ID_IOT_SET_SECURITY_WAIT  0x47
/* Set security mode, need wait, Format: buf[3] =
1byte port+1byte auth_mode+1byte encrypt_type */
#define   WIFI_EVENT_ID_IOT_CTRL_TRIGGER_SCAN  0x48
/* Enable/disable trigger scan, Format: buf[0] = enable/disable trigger scan*/
#define   WIFI_EVENT_ID_IOT_REQ_SCAN           0x49
/* Req scan, Format: buf[2] = 1bytesec+ 1byte usec */
#define   WIFI_EVENT_ID_IOT_START_STOP_SCAN    0x4A
/* Start_stop scan, Format: buf[0] = stop/start scan */
#define   WIFI_EVENT_ID_IOT_AP_RADIO_ONOFF     0x4B
/* AP radio swtich, Format: buf[0] = ap radio on/off */
#define   WIFI_EVENT_ID_IOT_STA_RADIO_ONOFF    0x4C
/* STA radio swtich, Format: buf[0] = sta radio on/off */
#define   WIFI_EVENT_ID_IOT_DISCONNECT_AP_WAIT          0x4D
/* Disconnect ap, need wait, Format: NULL  */

#define   WIFI_EVENT_ID_IOT_OPMODE_SWITCH  0x4F

#define   WIFI_EVENT_ID_IOT_SET_BSS_PREFERENCE  0x50

#define   WIFI_EVENT_ID_IOT_SET_MULTI_SECURITY_WAIT  0x51

#define   WIFI_EVENT_ID_IOT_SET_IEEE80211W_WAIT  0x52

#define   WIFI_EVENT_ID_IOT_SET_PROTO_WAIT  0x53

#define   WIFI_EVENT_ID_IOT_REGISTER_RX_HANDLER   0xFC
#define   WIFI_EVENT_ID_IOT_UNREGISTER_RX_HANDLER      0xFD

#define   WIFI_EVENT_ID_IOT_SET_NON_PREF_CAHN_WAIT  0xFE

#define IF_NAME_SIZE 16

#define IW_WPAS_APCLI_RELOAD_FLAG 0x0102 // align N9

typedef struct {
	int32_t value;
	uint8_t fixed;
	uint8_t disabled;
	uint16_t flags;
} iw_param_t;

typedef struct {
	int32_t m;
	int16_t e;
	uint8_t i;
	uint8_t flags;
} iw_freq_t;

typedef struct {
	void *pointer;
	uint16_t length;
	uint16_t flags;
} iw_point_t;

typedef struct {
	uint8_t sa_family;
	int8_t sa_data[14];
} sockaddr;

/* linked list to function structure */
typedef struct wifi_event_handler_struct {
	int32_t events;
	wifi_event_handler_t func;
	struct wifi_event_handler_struct *next;
} wifi_event_handler_struct;


struct __attribute__((packed)) iw_quality {

	uint8_t qual;
	uint8_t level;
	uint8_t noise;
	uint8_t updated;
};

typedef union {
	char name[IF_NAME_SIZE];
	iw_point_t essid;
	iw_param_t nwid;
	iw_freq_t freq;
	iw_param_t sens;
	iw_param_t bitrate;
	iw_param_t txpower;
	iw_param_t rts;
	iw_param_t frag;
	uint32_t mode;
	iw_param_t retry;
	iw_point_t encoding;
	iw_param_t power;
	struct iw_quality qual;
	sockaddr ap_addr;
	sockaddr addr;
	iw_point_t data;
	iw_param_t param;
} iwreq_data_t;

#if (CONFIG_WIFI_TEST_TOOL == 1)
struct iwreq {
	iwreq_data_t u;
};
#define iwreq_t			struct iwreq
#define	SIOCIWFIRSTPRIV		0x8BE0
#endif

/* for event packet */
#define EVENT_FIX_LEN			\
	(sizeof(iwreq_data_t) + sizeof(uint32_t) + MAC_ADDR_LEN)

#define EVENT_RSN_LEN			\
	(ELEM_HDR_LEN + ELEM_MAX_LEN_RSN + 4)
#define EVENT_RSNXE_LEN			\
	(ELEM_HDR_LEN + ELEM_MAX_LEN_RSNXE)
#define EVENT_MDIE_LEN			\
	(ELEM_HDR_LEN + ELEM_MDIE_LEN)
#define EVENT_FTIE_LEN			\
	(ELEM_HDR_LEN + ELEM_FTIE_LEN)


/* for WPA3 RSN and RSNXE */
#define EVENT_ASSOC_REQ_LEN		\
	(EVENT_RSN_LEN + EVENT_RSNXE_LEN)

/* for MDIE, FTIE, RSN, RSNXE */
#define EVENT_ASSOC_RESP_LEN		\
	(EVENT_MDIE_LEN + EVENT_FTIE_LEN + EVENT_ASSOC_REQ_LEN)

#define EVENT_DATA_MAX_LEN		\
	(MAC_ADDR_LEN + EVENT_ASSOC_REQ_LEN + EVENT_ASSOC_RESP_LEN)

#define EVENT_PKT_MAX_LEN		\
	(EVENT_FIX_LEN + EVENT_DATA_MAX_LEN)


/*[JCB] need check if it is necessary here or move to other files */
#define MAX_INTERFACE_NUM 2

typedef struct __global_event_callback {
	void *priv;
	int (*func)(void *priv, unsigned char evt_id, unsigned char *body,
		    int len);
	unsigned char op_mode;
	unsigned char ready;
} global_event_callback;

extern global_event_callback __process_global_event[];

/* end of copy from inband_queue.h*/
#define COMPACT_RACFG_CMD_WIRELESS_SEND_EVENT                                  \
	WIFI_EVENT_ID_IOT_ASSOCIATED // 0x63, just test w/ E2 ROM

int wifi_api_set_event_handler(uint8_t enabled, wifi_event_t idx,
					wifi_event_handler_t handler);
int wifi_event_notification(unsigned char port, unsigned char evt_id,
					unsigned char *body, int len);
int wifi_event_notification_wait(unsigned char port, unsigned char evt_id,
					unsigned char *body, int len);
int wifi_evt_handler_gen4m(unsigned int inf, unsigned char *body, int len);
int wifi_api_event_trigger(uint8_t port, wifi_event_t idx, uint8_t *address,
					unsigned int length);
void wifi_api2supp_result_semphr_give(void);
void unregister_process_global_event(void);
void register_process_global_event(void *priv,
				   int (*func)(void *, unsigned char,
					       unsigned char *, int),
				   unsigned char op_mode);

#endif /* __WIFI_EVENT_H__ */
