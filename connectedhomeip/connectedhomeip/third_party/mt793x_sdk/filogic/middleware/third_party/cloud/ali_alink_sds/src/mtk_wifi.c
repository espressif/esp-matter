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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"

#include "msc_api.h"
#include "msc_internal.h"
#include "platform.h"
#ifdef MTK_JOYLINK_ENABLE
#include "joylink_smnt_api.h"
#endif

#include "wifi_api.h"
#include "wifi_scan.h"
#include "semphr.h"
#include "syslog.h"
#include "wifi_rx_desc.h"
#include "nvdm.h"
#include "task_def.h"
#include "hal_aes.h"
#include "hal_gpt.h"
#include "alink_export.h"
#include "aws_lib.h"
#include "misc.h"
/******************************************************************************
 * macro typedef
 ******************************************************************************/
//log_create_module(multiSmnt, PRINT_LEVEL_INFO);

#define SMTCN_DEBUG 1
#ifdef ALINK_SDS_PRINTF
#define alink_log  printf
#else
#define alink_log(fmt, args...)     LOG_I(alink_sds, "[alink] "fmt, ##args)
#define alink_loge(fmt, args...)    LOG_E(alink_sds, "[alink] "fmt, ##args)
#endif

/* smart connection thread wait scan done.*/
static SemaphoreHandle_t scan_done_mutex = NULL;


/******************************************************************************
 * extern variable definition,declaration
 ******************************************************************************/
extern multi_smtcn_info_t   saved_multi_smtcn_info;

#ifdef MTK_MINISUPP_ENABLE
extern int g_supplicant_ready;
#endif

#define AWSS_NEW_VERSION
#ifdef AWSS_NEW_VERSION

//#define PLATFORM_SCAN_RESULT_CALLBACK

#define _IN_			/**< indicate that this is a input parameter. */
#define _OUT_			/**< indicate that this is a output parameter. */
#define _INOUT_			/**< indicate that this is a io parameter. */
#define _IN_OPT_		/**< indicate that this is a optional input parameter. */
#define _OUT_OPT_		/**< indicate that this is a optional output parameter. */
#define _INOUT_OPT_		/**< indicate that this is a optional io parameter. */


/* ssid: 32 octets at most, include the NULL-terminated */
#define PLATFORM_MAX_SSID_LEN			(32 + 1)
/* password: 8-63 ascii */
#define PLATFORM_MAX_PASSWD_LEN			(64 + 1)
#ifndef ETH_ALEN
#define ETH_ALEN                        (6)
#endif

typedef int (*platform_wifi_scan_result_cb_t)(
        const char ssid[PLATFORM_MAX_SSID_LEN],
        const uint8_t bssid[ETH_ALEN],
        enum AWSS_AUTH_TYPE auth,
        enum AWSS_ENC_TYPE encry,
        uint8_t channel, char rssi,
        int is_last_ap);

typedef struct{
	void *next;
	wifi_scan_list_item_t ap;
}platform_scan_result;



platform_wifi_scan_result_cb_t platform_alink_scan_cb=NULL;
platform_scan_result platform_ap_list;


#define promiscuous_rxfilter (0x1002)
static uint32_t saved_rxfilter = 0;
static uint8_t saved_bw = 0;

void vendor_data_callback(unsigned char *payload, int len)
{
    P_HW_MAC_RX_DESC_T        pRxWI;
    uint8_t          ucRxPktType;
    uint8_t          ucMacHeaderLength;
    uint16_t         u2PayloadLength;
    uint8_t	*pucMacHeader;       /* 802.11 header  */
    uint8_t	*pucPayload;         /* 802.11 payload */
    uint16_t         u2RxStatusOffst;


    pRxWI   = (P_HW_MAC_RX_DESC_T) (payload);
    // =======  Process RX Vector (if any) ========

    // 1. ToDo: Cache RXStatus in SwRfb
    u2RxStatusOffst = sizeof(HW_MAC_RX_DESC_T);
    ucRxPktType = HAL_RX_STATUS_GET_PKT_TYPE(pRxWI);

    /*
        hex_dump("raw packet", payload, len);
        printf("pRxWI.u2RxByteCount:[0x%04x]\n", pRxWI->u2RxByteCount);
        printf("pRxWI.u2PktTYpe:[0x%04x]\n", pRxWI->u2PktTYpe);
        printf("pRxWI.ucMatchPacket:[0x%02x]\n", pRxWI->ucMatchPacket);
        printf("pRxWI.ucChanFreq:[0x%02x]\n", pRxWI->ucChanFreq);
        printf("pRxWI.ucHeaderLen:[0x%02x]\n", pRxWI->ucHeaderLen);
        printf("pRxWI.ucBssid:[0x%02x]\n", pRxWI->ucBssid);
        printf("ucRxPktType:[0x%02x]\n", ucRxPktType);
    */

    if ((ucRxPktType != RX_PKT_TYPE_RX_DATA) && (ucRxPktType != RX_PKT_TYPE_SW_DEFINED)) {
        return; // not handled
    }

    if (ucRxPktType == RX_PKT_TYPE_RX_DATA ||
            ucRxPktType == RX_PKT_TYPE_DUP_RFB) {
        uint16_t          u2GrpOffst;
        uint8_t              ucGroupVLD;

        // RX Status Group
        u2GrpOffst = sizeof(HW_MAC_RX_DESC_T);
        ucGroupVLD = HAL_RX_STATUS_GET_GROUP_VLD(pRxWI);
        if (ucGroupVLD & BIT(RX_GROUP_VLD_4)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_4_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_1)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_1_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_2)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_2_T);
        }

        if (ucGroupVLD & BIT(RX_GROUP_VLD_3)) {
            u2GrpOffst += sizeof(HW_MAC_RX_STS_GROUP_3_T);
        }

        u2RxStatusOffst = u2GrpOffst;
    }


    // Locate the MAC Header based on GroupVLD
    pucMacHeader = (uint8_t *)(((uint32_t)pRxWI + (HAL_RX_STATUS_GET_HEADER_OFFSET(pRxWI)) + u2RxStatusOffst) & 0xFFFFFFFE);
    ucMacHeaderLength = HAL_RX_STATUS_GET_HEADER_LEN(pRxWI);

    pucPayload = (uint8_t *)((uint32_t)(pucMacHeader + ucMacHeaderLength) & 0xFFFFFFFC);
    u2PayloadLength = (uint16_t)(HAL_RX_STATUS_GET_RX_BYTE_CNT(pRxWI) - ((uint32_t)pucPayload - (uint32_t)pRxWI));


    //hex_dump("mac header begin", pucMacHeader, ucMacHeaderLength+u2PayloadLength);
    aws_80211_frame_handler((char *)pucMacHeader, ucMacHeaderLength + u2PayloadLength, 0, 0);

    return; /* handled */
}


//一键配置超时时间, 建议超时时间1-3min, APP侧一键配置1min超时
int platform_awss_get_timeout_interval_ms(void)
{
    return 1 * 60 * 1000;
}

//默认热点配网超时时间
int platform_awss_get_connect_default_ssid_timeout_interval_ms( void )
{
    return 0;
}

//一键配置每个信道停留时间, 建议200ms-400ms
int platform_awss_get_channelscan_interval_ms(void)
{
    return 200;
}

//wifi信道切换，信道1-13
void platform_awss_switch_channel(char primary_channel,
		char secondary_channel, uint8_t bssid[ETH_ALEN])
{
    if(primary_channel == 0) {
        LOG_E(sds, "CH=%d Error",primary_channel);
        return;
    }
	wifi_config_set_channel(WIFI_PORT_STA, primary_channel);
}


//进入monitor模式, 并做好一些准备工作，如
//设置wifi工作在默认信道6
//若是linux平台，初始化socket句柄，绑定网卡，准备收包
//若是rtos的平台，注册收包回调函数aws_80211_frame_handler()到系统接口
void platform_awss_open_monitor(platform_awss_recv_80211_frame_cb_t cb)
{
	wifi_config_set_opmode(WIFI_MODE_STA_ONLY);

    wifi_config_get_bandwidth(WIFI_PORT_STA, &saved_bw);
    wifi_config_set_bandwidth(WIFI_PORT_STA, WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ);

    wifi_config_get_rx_filter(&saved_rxfilter);
    wifi_config_set_rx_filter(promiscuous_rxfilter);

    wifi_config_register_rx_handler((wifi_rx_handler_t)vendor_data_callback);
    wifi_config_set_channel(WIFI_PORT_STA, 6);
}

//退出monitor模式，回到station模式, 其他资源回收
void platform_awss_close_monitor(void)
{
	wifi_config_set_rx_filter(saved_rxfilter);
    wifi_config_unregister_rx_handler();
}


void platform_authmode_mapping_mtk2aws(wifi_auth_mode_t *auth_mtk, enum AWSS_AUTH_TYPE *auth_aws){
	switch(*auth_mtk){
	case WIFI_AUTH_MODE_OPEN:
		*auth_aws = AWSS_AUTH_TYPE_OPEN;
		break;
	case WIFI_AUTH_MODE_SHARED:
		*auth_aws = AWSS_AUTH_TYPE_SHARED;
		break;
	case WIFI_AUTH_MODE_WPA_PSK:
		*auth_aws = AWSS_AUTH_TYPE_WPAPSK;
		break;
	case WIFI_AUTH_MODE_WPA2_PSK:
		*auth_aws = AWSS_AUTH_TYPE_WPA2PSK;
		break;
	case WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK:
		*auth_aws = AWSS_AUTH_TYPE_WPAPSKWPA2PSK;
		break;
	default:
		*auth_aws = AWSS_AUTH_TYPE_INVALID;
		break;
	}
}
void platform_authmode_mapping_aws2mtk(wifi_auth_mode_t *auth_mtk, enum AWSS_AUTH_TYPE *auth_aws){
	switch(*auth_aws){
	case AWSS_AUTH_TYPE_OPEN:
		*auth_mtk = WIFI_AUTH_MODE_OPEN;
		break;
	case AWSS_AUTH_TYPE_SHARED :
		*auth_mtk = WIFI_AUTH_MODE_SHARED;
		break;
	case AWSS_AUTH_TYPE_WPAPSK :
		*auth_mtk = WIFI_AUTH_MODE_WPA_PSK;
		break;
	case AWSS_AUTH_TYPE_WPA2PSK :
		*auth_mtk = WIFI_AUTH_MODE_WPA2_PSK;
		break;
	case AWSS_AUTH_TYPE_WPAPSKWPA2PSK :
		*auth_mtk = WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
		break;
	default:
		*auth_mtk = WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK;
		break;
	}
}

void platform_encrymode_mapping_mtk2aws(wifi_encrypt_type_t *encry_mtk, enum AWSS_ENC_TYPE *encry_aws){

	switch(*encry_mtk){
	case WIFI_ENCRYPT_TYPE_WEP_ENABLED:
		*encry_aws = AWSS_ENC_TYPE_WEP;
		break;
	case WIFI_ENCRYPT_TYPE_WEP_DISABLED:
		*encry_aws = AWSS_ENC_TYPE_NONE;
		break;
	case WIFI_ENCRYPT_TYPE_TKIP_ENABLED:
		*encry_aws = AWSS_ENC_TYPE_TKIP;
		break;
	case WIFI_ENCRYPT_TYPE_AES_ENABLED:
		*encry_aws = AWSS_ENC_TYPE_AES;
		break;
	case WIFI_ENCRYPT_TYPE_TKIP_AES_MIX:
		*encry_aws = AWSS_ENC_TYPE_TKIPAES;
		break;
	default:
		*encry_aws = AWSS_ENC_TYPE_INVALID;
		break;
	}
}
void platform_encrymode_mapping_aws2mtk(wifi_encrypt_type_t *encry_mtk, enum AWSS_ENC_TYPE *encry_aws){
	switch(*encry_aws){
	case AWSS_ENC_TYPE_WEP :
		*encry_mtk = WIFI_ENCRYPT_TYPE_WEP_ENABLED;
		break;
	case AWSS_ENC_TYPE_NONE :
		*encry_mtk = WIFI_ENCRYPT_TYPE_WEP_DISABLED;
		break;
	case AWSS_ENC_TYPE_TKIP :
		*encry_mtk = WIFI_ENCRYPT_TYPE_TKIP_ENABLED;
		break;
	case AWSS_ENC_TYPE_AES :
		*encry_mtk = WIFI_ENCRYPT_TYPE_AES_ENABLED;
		break;
	case AWSS_ENC_TYPE_TKIPAES :
		*encry_mtk = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;
		break;
	default:
		*encry_mtk = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX;
		break;
	}
}

static int platform_msc_check_scan_complete(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    if (event_id != WIFI_EVENT_IOT_SCAN_COMPLETE) {
        return 0;
    }

    /* Notice:
    * Here may need a semaphore to protect scan_loop_count against _msc_scan_timeout.
    */
    xSemaphoreGive(scan_done_mutex);
    return 1;
}

static int platform_msc_scan_prb_rsp_handler(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    if (event_id != WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE) {
        return 1;
    }

    if (len != 0) {
        wifi_scan_list_item_t ap_data;
        memset(&ap_data, 0, sizeof(wifi_scan_list_item_t));
        if (wifi_connection_parse_beacon(payload, len, &ap_data) >= 0) {
			platform_scan_result *temp_platform_scan_result=NULL;
			temp_platform_scan_result=pvPortMalloc(sizeof(platform_scan_result));
			if(NULL == temp_platform_scan_result){
				printf("platform_msc_scan_prb_rsp_handler malloc failure\r\n");
				return 1;
			}else{
				temp_platform_scan_result->next=NULL;
				temp_platform_scan_result->ap=ap_data;

				platform_scan_result *temp_list=&platform_ap_list;
				while(NULL != temp_list->next){
					temp_list=temp_list->next;
				}
				temp_list->next=temp_platform_scan_result;
			}

#if (SMTCN_DEBUG == 1)
                printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS");
                printf("%-4d", ap_data.channel);
                printf("%-33s", ap_data.ssid);
                printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                       ap_data.bssid[0],
                       ap_data.bssid[1],
                       ap_data.bssid[2],
                       ap_data.bssid[3],
                       ap_data.bssid[4],
                       ap_data.bssid[5]);
                printf("%-8d", ap_data.auth_mode);
                printf("%-8d", ap_data.encrypt_type);
                printf("%-8d", ap_data.rssi);
                printf("%-8d", ap_data.is_wps_supported);
                printf("\n");
#endif

#ifdef PLATFORM_SCAN_RESULT_CALLBACK
            if (platform_alink_scan_cb) {
				enum AWSS_AUTH_TYPE auth =AWSS_AUTH_TYPE_INVALID;
				enum AWSS_ENC_TYPE encry =AWSS_ENC_TYPE_INVALID;
				platform_authmode_mapping_mtk2aws(&ap_data.auth_mode ,&auth);
				platform_encrymode_mapping_mtk2aws(&ap_data.encrypt_type,&encry);
                (platform_alink_scan_cb)(ap_data.ssid,ap_data.bssid,auth,encry,ap_data.channel,ap_data.rssi,0);
            }
#endif
            return 0;
        }
    }

    return 1;
}

static int platform_msc_scan_init(void)
{
    if (scan_done_mutex == NULL) {
        scan_done_mutex = xSemaphoreCreateCounting((UBaseType_t)1, (UBaseType_t)0);
        if (scan_done_mutex == NULL) {
            LOG_E(multiSmnt, "scan_done_mutex create failed.\n");
            return -1;
        }
    }
	memset(&platform_ap_list, 0, sizeof(platform_ap_list));
	return 0;
}

static void platform_msc_scan_deinit(void)
{
    if (scan_done_mutex != NULL) {
        vSemaphoreDelete(scan_done_mutex);
        scan_done_mutex = NULL;
    }

}

int platform_wifi_scan(platform_wifi_scan_result_cb_t cb){
#ifdef MTK_MINISUPP_ENABLE
	while(!g_supplicant_ready) {
		vTaskDelay(50);
	}
#endif

	if(NULL == cb){
		LOG_I(multiSmnt, "Now stop scan.\n");
#ifdef MTK_MINISUPP_ENABLE
		__g_wpa_supplicant_api.wpa_supplicant_disable_trigger_scan();
#endif
    	wifi_connection_stop_scan();
		wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE, (wifi_event_handler_t)platform_msc_scan_prb_rsp_handler);
		wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE, (wifi_event_handler_t)platform_msc_check_scan_complete);
		platform_msc_scan_deinit();
		platform_alink_scan_cb = NULL;
		return -1;
	}else{
		LOG_I(multiSmnt, "Now start scan.\n");
		platform_alink_scan_cb = cb;
		wifi_connection_register_event_handler(WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE,(wifi_event_handler_t) platform_msc_scan_prb_rsp_handler);
		wifi_connection_register_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE,(wifi_event_handler_t) platform_msc_check_scan_complete);
		platform_msc_scan_init();
		wifi_connection_start_scan(NULL, 0, NULL, 0, 0);

		xSemaphoreTake(scan_done_mutex, portMAX_DELAY);

#ifndef PLATFORM_SCAN_RESULT_CALLBACK
		if (platform_alink_scan_cb) {
				enum AWSS_AUTH_TYPE auth =AWSS_AUTH_TYPE_INVALID;
				enum AWSS_ENC_TYPE encry =AWSS_ENC_TYPE_INVALID;

				platform_scan_result *temp_list=&platform_ap_list;

				while(NULL != temp_list->next){
					platform_authmode_mapping_mtk2aws(&((platform_scan_result *)temp_list->next)->ap.auth_mode ,&auth);
					platform_encrymode_mapping_mtk2aws(&((platform_scan_result *)temp_list->next)->ap.encrypt_type,&encry);

					(platform_alink_scan_cb)(
						(const char *)(((platform_scan_result *)temp_list->next)->ap.ssid),
						((platform_scan_result *)temp_list->next)->ap.bssid,
						auth,
						encry,
						((platform_scan_result *)temp_list->next)->ap.channel,
						((platform_scan_result *)temp_list->next)->ap.rssi,
						(NULL == ((platform_scan_result *)temp_list->next)->next)?1:0
					);

					temp_list=temp_list->next;
				}


				temp_list=platform_ap_list.next;
				platform_scan_result *temp_list_next=NULL;
				while(NULL != temp_list){

					temp_list_next=temp_list->next;
					vPortFree(temp_list);
					temp_list=temp_list_next;

				}

		}
#endif
		return 0;
	}


}
int g_ip_ready = 0;
int platform_awss_connect_ap(
        _IN_ uint32_t connection_timeout_ms,
        _IN_ char ssid[PLATFORM_MAX_SSID_LEN],
        _IN_ char passwd[PLATFORM_MAX_PASSWD_LEN],
        _IN_OPT_ enum AWSS_AUTH_TYPE auth,
        _IN_OPT_ enum AWSS_ENC_TYPE encry,
        _IN_OPT_ uint8_t bssid[ETH_ALEN],
        _IN_OPT_ uint8_t channel){

    strcpy((char *)saved_multi_smtcn_info.ssid,ssid);
    saved_multi_smtcn_info.ssid_len=strlen((char *)saved_multi_smtcn_info.ssid);
    strcpy((char *)saved_multi_smtcn_info.pwd,passwd);
    saved_multi_smtcn_info.pwd_len=strlen((char *)saved_multi_smtcn_info.pwd);
    LOG_I(multiSmnt, "ssid=%s; ssid_len=%d; passwd=%s; pwd_len=%d; auth=%d; encry=%d; ch=%d\n",ssid, saved_multi_smtcn_info.ssid_len, passwd, saved_multi_smtcn_info.pwd_len, auth, encry, channel);

    platform_authmode_mapping_aws2mtk(&saved_multi_smtcn_info.auth_mode, &auth);
    platform_encrymode_mapping_aws2mtk(&saved_multi_smtcn_info.encrypt_type, &encry);
    saved_multi_smtcn_info.channel=channel;

    //wifi_config_set_channel(WIFI_PORT_STA, saved_multi_smtcn_info.channel);

    if(wifi_config_set_ssid(WIFI_PORT_STA, saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len) < 0)
    {
        alink_loge("smt set ssid fail\n");
        return -1;
    }

#ifdef MTK_MINISUPP_ENABLE
    uint8_t hex[32] = {0};
    wifi_config_set_pmk(WIFI_PORT_STA,hex);
#endif

    if (saved_multi_smtcn_info.pwd_len != 0) {
        if((saved_multi_smtcn_info.pwd_len >= 8) && (wifi_config_set_wpa_psk_key(WIFI_PORT_STA, saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len) < 0))
        {
            alink_loge("smt set wpa psk key fail\n");
            return -1;
        }

        if(saved_multi_smtcn_info.pwd_len == 10 || saved_multi_smtcn_info.pwd_len == 26 ||
             saved_multi_smtcn_info.pwd_len == 5 || saved_multi_smtcn_info.pwd_len == 13) {
            wifi_wep_key_t wep_key = {{{0}}};
             if (saved_multi_smtcn_info.pwd_len == 10 || saved_multi_smtcn_info.pwd_len == 26) {
                wep_key.wep_key_length[0] = saved_multi_smtcn_info.pwd_len / 2;
                AtoH((char *)saved_multi_smtcn_info.pwd, (char *)&wep_key.wep_key[0], (int)wep_key.wep_key_length[0]);
            } else {
                wep_key.wep_key_length[0] = saved_multi_smtcn_info.pwd_len;
                memcpy(wep_key.wep_key[0], saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);
            }

            wep_key.wep_tx_key_index = 0;
            if(wifi_config_set_wep_key(WIFI_PORT_STA, &wep_key) < 0) {
                alink_loge("set wep key fail\n");
                return -1;
            }
        }
    }
    LOG_I(multiSmnt, "\n>>>>>> start connect <<<<<<:\n\n");

    //wifi_connection_stop_scan();
#ifdef MTK_MINISUPP_ENABLE
    __g_wpa_supplicant_api.wpa_supplicant_enable_trigger_scan();
#endif
    wifi_config_reload_setting();

#ifdef MTK_MINISUPP_ENABLE
    if(g_ip_ready == 0)
    {
        if(lwip_net_ready_timeout(connection_timeout_ms) != 0 ) {
            return -1;
        }
        g_ip_ready = 1;
    }
    LOG_I(multiSmnt, "\n>>>>>> Got IP <<<<<<:\n\n");
#else
    if(g_ip_ready == 0)
    {
        lwip_net_ready();
        g_ip_ready = 1;
    }
#endif
    return 0;
}


int platform_wifi_get_ap_info(
    char ssid[PLATFORM_MAX_SSID_LEN],
    char password[PLATFORM_MAX_PASSWD_LEN],
    uint8_t bssid[ETH_ALEN])
{
		int32_t ret = 0;
		uint8_t port = 0;
		uint8_t length = 0;


		if(NULL != ssid){
	        ret = wifi_config_get_ssid(port, (uint8_t *)ssid, &length);
	        if (ret < 0) {
	            LOG_E(multiSmnt, "Get ssid failed\n");
	            ret = -1;
	        }
		}

		if(NULL != password){
			ret = wifi_config_get_wpa_psk_key(port, (uint8_t *)password, &length);
	        if (ret < 0) {
	            LOG_E(multiSmnt, "Get password failed\n");
	            ret = -1;
	        } else {
	            LOG_I(multiSmnt, "[password]:%s\n", password);
	        }
		}

		if(NULL != bssid){
			ret = wifi_config_get_bssid((uint8_t *)bssid);
		    if (ret < 0) {
				LOG_E(multiSmnt, "get bssid fail \n");
		        ret = -1;
		    }
		}

		return ret;

}

int platform_wifi_send_80211_raw_frame(_IN_ enum platform_awss_frame_type type,
        _IN_ uint8_t *buffer, _IN_ int len)
{
    if (NULL == buffer) {
        LOG_E(multiSmnt, "payload is null");
        return -1;;
    }

	return wifi_connection_send_raw_packet(buffer, len);
}

int platform_wifi_enable_mgnt_frame_filter(
            _IN_ uint32_t filter_mask,
            _IN_OPT_ uint8_t vendor_oui[3],
            _IN_ platform_wifi_mgnt_frame_cb_t callback)
{
#if 0
    //To be improve as API set
    uint32_t current_filter = 0;

    LOG_I(multiSmnt, "filter_mask:%d\n", filter_mask);
    
    if (wifi_config_get_rx_filter(&current_filter) < 0) {
        LOG_E(multiSmnt, "get filter fail\n");
        return -1;
    }

    LOG_E(multiSmnt, "filter before %x\n", current_filter);

    if(filter_mask & FRAME_BEACON_MASK) {
        LOG_I(multiSmnt, "set BCN filter\n");
        current_filter |= BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);
    }

    if(filter_mask & FRAME_PROBE_REQ_MASK){
        LOG_I(multiSmnt, "set REQ filter\n");
        current_filter |= BIT(WIFI_RX_FILTER_DROP_PROBE_REQ);
    }


    wifi_config_set_rx_filter(current_filter);

    wifi_config_get_rx_filter(&current_filter);
    LOG_I(multiSmnt, "filter end %x\n", current_filter);
#else
    LOG_E(multiSmnt, "Not support filter=%d",filter_mask);
	return -2;
#endif
}


typedef struct
{
    unsigned char *key;
    size_t key_byte_length;
}
alink_aes_context;


hal_aes_buffer_t aes_plain_text;
uint8_t *aes_key;
hal_aes_buffer_t aes_encrypted_buffer;
uint8_t *aes_cbc_iv;//[HAL_AES_CBC_IV_LENGTH];
uint8_t aes_mode;
alink_aes_context *g_alink_ctx;


p_aes128_t platform_aes128_init(
    const uint8_t* key,
    const uint8_t* iv,
    AES_DIR_t dir)
{
    g_alink_ctx = platform_malloc(sizeof(alink_aes_context));

	memset( g_alink_ctx, 0, sizeof(alink_aes_context));

	aes_cbc_iv = platform_malloc(HAL_AES_CBC_IV_LENGTH);
	if( aes_cbc_iv == NULL)
	{
		return NULL;
	}
	else
		memcpy(aes_cbc_iv,iv,HAL_AES_CBC_IV_LENGTH);

	aes_key = platform_malloc(HAL_AES_KEY_LENGTH_128);
	if(aes_key ==NULL)
	{
		return NULL;
	}
	else
		memcpy(aes_key,key,HAL_AES_KEY_LENGTH_128);

	aes_mode = dir;

	g_alink_ctx->key = aes_key;
	g_alink_ctx->key_byte_length = HAL_AES_KEY_LENGTH_128;

	return (p_aes128_t)g_alink_ctx;
}

int platform_aes128_destroy(_IN_ p_aes128_t aes)
{
	if(NULL != aes_cbc_iv)
		platform_free(aes_cbc_iv);
    
	if(NULL != aes_key)
		platform_free(aes_key);
    
    if(NULL != g_alink_ctx)
        platform_free(g_alink_ctx);
	return 0;
}

static int __aes128_function(
    p_aes128_t aes,
    const void *src,
    size_t blockNum,
    void *dst ,
    AES_DIR_t dir)
{
    hal_aes_status_t ret = 0;
    unsigned char *input_temp  = NULL;
    unsigned char *output_temp = NULL;
    hal_aes_buffer_t *output_buf = NULL;
    hal_aes_buffer_t *input_buf  = NULL;
    hal_aes_buffer_t *key_buf    = NULL;
    uint8_t * p_iv  = NULL;
    uint8_t * p_key = NULL;
    
    if(blockNum <= 0)
    {
        LOG_E(multiSmnt, "invid input length\n");
        return ALINK_ERR;
    }
    
    do 
    {
        output_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));
        input_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));
        key_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));

        output_temp = (uint8_t *)pvPortMallocNC(blockNum * 16 + 32);
        input_temp = (uint8_t *)pvPortMallocNC(blockNum * 16);
        p_key = (uint8_t *)pvPortMallocNC(HAL_AES_KEY_LENGTH_128);
        p_iv = (uint8_t *)pvPortMallocNC(HAL_AES_CBC_IV_LENGTH);


        if(NULL == output_temp || 
           NULL == input_temp  ||
           NULL == p_iv        ||
           NULL == output_buf  || 
           NULL == input_buf   ||
           NULL == key_buf     || 
           NULL == p_key )
        {
            ret =  ALINK_ERR_NO_MEMORY;
            break;
        }

        memset(output_temp, 0, blockNum * 16 + 32);
        memcpy(input_temp, src, blockNum * 16);
        memcpy(p_iv,aes_cbc_iv,HAL_AES_CBC_IV_LENGTH);
        memcpy(p_key,((alink_aes_context *)aes)->key,HAL_AES_KEY_LENGTH_128);


        output_buf->buffer = output_temp;
        output_buf->length = blockNum * 16 + 32;
        input_buf->buffer = input_temp;
        input_buf->length = blockNum * 16;
        key_buf->buffer = p_key;
        key_buf->length = HAL_AES_KEY_LENGTH_128;

        do 
        {
            if(dir == PLATFORM_AES_ENCRYPTION)
                ret = hal_aes_cbc_encrypt(output_buf, input_buf, key_buf, p_iv);
            else
                ret = hal_aes_cbc_decrypt( output_buf, input_buf, key_buf, p_iv );
            if(ret == -100){
                hal_gpt_delay_ms(1);
            }
            
        } while(ret == -100);

        memcpy( dst, output_temp, blockNum * 16 );

    }while(0);

    if(output_temp)
        vPortFreeNC(output_temp);
    if(input_temp)
        vPortFreeNC(input_temp);
    if(p_key)
        vPortFreeNC(p_key);
    if(p_iv)
        vPortFreeNC(p_iv);
    if(output_buf)
        vPortFreeNC(output_buf);
    if(input_buf)
        vPortFreeNC(input_buf);
    if(key_buf)
        vPortFreeNC(key_buf);
    
    return ret;
}


int platform_aes128_cbc_encrypt(
    p_aes128_t aes,
    const void *src,
    size_t blockNum,
    void *dst )
{
    return __aes128_function(aes,src,blockNum,dst,PLATFORM_AES_ENCRYPTION);
}


int platform_aes128_cbc_decrypt(
   p_aes128_t aes,
   const void *src,
   size_t blockNum,
   void *dst )
{
    return __aes128_function(aes,src,blockNum,dst,PLATFORM_AES_DECRYPTION);
}

#endif


