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

 


#ifndef __WIFI_EVENT_ID_H__
#define __WIFI_EVENT_ID_H__


#define IW_CONNECTION_RETRY_FAIL_EVENT_FLAG 0x0199
#define IW_SYS_EVENT_FLAG_START             0x0200
#define IW_ASSOC_EVENT_FLAG                 0x0200
#define IW_DISASSOC_EVENT_FLAG              0x0201
#define IW_DEAUTH_EVENT_FLAG                0x0202
#define IW_MIC_ERROR_EVENT_FLAG             0x0209
#define IW_STA_LINKUP_EVENT_FLAG            0x020F
#define IW_STA_LINKDOWN_EVENT_FLAG          0x0210
#define IW_SCAN_COMPLETED_EVENT_FLAG        0x0211
#define IW_CANCEL_REMAIN_ON_CHANNEL_FLAG    0x0212
#define IW_TX_ACTION_STATUS_EVENT_FLAG      0x0213
#define IW_RX_ACTION_FRAME_EVENT_FLAG       0x0214
#define IW_ANTENNA_MONITOR_EVENT_FLAG       0x0288


/*if auth is wpa2psk/aes, it means 4way complete,  if auth is wpapsk/tkip, it means 6ways complete*/
#define RT_RSNA_COMPLETE_EVENT_FLAG         0x0333   

#define RT_AUTH_START_AFTER_JOIN_EVENT_FLAG 0x0334

#define RT_AP_START_UP_EVENT_FLAG           0x0335

#define RT_ASSOC_TIMEOUT_EVENT_FLAG           0x0336

#define RT_RSNA_TIMEOUT_EVENT_FLAG           0x0337

#define RT_BSSID_NOT_MATCH_EVENT_FLAG   0x0338

#define RT_CIPPHER_ERROR_EVENT_FLAG    0x0339

#define RT_SCAN_DROPPED_EVENT_FLAG      0x0340


#define RT_WSC_DONE_EVENT_FLAG              0x0342

#define RT_WSC_INFO_CONNECTED_EVENT_FLAG    0x0343

#define RT_WSC_M2D_RECEIVED    0x0344

#define RT_FLOW_CONTROL_EVENT_FLAG  0x0345

#define IW_CUSTOM_EVENT_FLAG                0x8C02
#define IW_ASSOCREQ_IE_EVENT_FLAG           0x8C07
#define IW_REASSOCREQ_IE_EVENT_FLAG         0x8C0A

#define   WIFI_EVENT_ID_IOT_TRIGGER_WPS    0x4E            /* trigger WPS */
#define   WIFI_EVENT_ID_IOT_OPMODE_SWITCH  0x4F            /* opmode swtich */

/* Define EVENT ID from firmware to Host (v0.09) */
#define   WIFI_EVENT_ID_IOT_SCAN_LIST  0x50            /* 0x50 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_PROBE_REQ  0x51
#define   WIFI_EVENT_ID_IOT_MGMT_WITH_REASON_CODE  0x52
#define   WIFI_EVENT_ID_IOT_FRAME_TYPE_FILTER  0x53     /* rx filter for specified frame type */
#define   WIFI_EVENT_ID_IOT_CF_CREDENTIAL  0x55          /* Save ConfigFree Credentials and Reboot */
#define   WIFI_EVENT_ID_IOT_CF_IP_REQ      0x56         /* Request IP via DHCP, issued by CM4, instead of N9  */
#define   WIFI_EVENT_ID_IOT_WLAN_EVT  0x60               /* 0x60 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_CONNECTED  0x61              /* 0x61 (Unsoiicited) */

#define   WIFI_EVENT_ID_IOT_ASSOCIATED  0x63            /* 0x63 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_DEAUTH  0x64              /* 0x64 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_DISASSOC  0x65             /* 0x65 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_RELOAD_CONFIGURATION  0x66   /* 0x66 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_AP_LINK_UP  0x67   /* 0x67 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_REASSOCIATED  0x68   /* 0x68 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_MIC_ERROR  0x69
#define   WIFI_EVENT_ID_IOT_CANCEL_REMAIN_ON_CHANNEL  0x70
#define   WIFI_EVENT_ID_IOT_APCLI_WPAS_RELOAD  0x71
#define   WIFI_EVENT_ID_IOT_RX_ACTION  0x72
#define   WIFI_EVENT_ID_IOT_TX_STATUS  0x73
#define   WIFI_EVENT_ID_IOT_LINK_DOWN  0x74               /* 0x74 (Unsoiicited) */
#define   WIFI_EVENT_ID_IOT_N9_CONSOLE_LOG  0x80
#if  defined(MTK_WIFI_ROM_ENABLE)
#define   WIFI_EVENT_ID_IOT_USER_SCAN_DONE      0x81
#define   WIFI_EVENT_ID_IOT_DISCONNECT      0x82
#endif



#define   WIFI_EVENT_ID_LAYER_0_EXT_MAGIC_NUM   0xED    /* magic number for Extending MT6630 original EVENT header  */
#define   WIFI_EVENT_ID_LAYER_1_MAGIC_NUM       0xEE    /* magic number for backward compatible with MT76xx EVENT  */

#define   WIFI_EVENT_ID_IOT_IP_READY               0xEF
#define   WIFI_EVENT_ID_IOT_AUTH_START_AFTER_JOIN  0xFA
#define   WIFI_EVENT_ID_IOT_CONNECTION_RETRY_FAIL  0xFB

#define   WIFI_EVENT_ID_IOT_REGISTER_RX_HANDLER   0xFC
#define   WIFI_EVENT_ID_IOT_UNREGISTER_RX_HANDLER      0xFD

#define   WIFI_EVENT_ID_IOT_RSNA_COMPLETE  0xFE 
#define   WIFI_EVENT_ID_IOT_ASSOC_TIMEOUT  0xFF
#define   WIFI_EVENT_ID_IOT_RSNA_TIMEOUT   0xF1
#define   WIFI_EVENT_ID_IOT_BSSID_NOT_MATCH 0xF2
#define   WIFI_EVENT_ID_IOT_CIPPHER         0xF3
#define   WIFI_EVENT_ID_IOT_SCAN_DRPOPPED   0xF4
#define   WIFI_EVENT_ID_IOT_FLOW_CONTROL    0xF5

#define   WIFI_EVENT_ID_IOT_WSC_DONE        0xF6
#define   WIFI_EVENT_ID_IOT_WSC_INFO_CONNECTED  0xF7
#define   WIFI_EVENT_ID_IOT_WSC_M2D_RECEIEVED  0xF8
#endif
