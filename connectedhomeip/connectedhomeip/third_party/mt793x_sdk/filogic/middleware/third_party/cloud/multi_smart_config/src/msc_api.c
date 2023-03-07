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

/******************************************************************************
 * macro typedef
 ******************************************************************************/
log_create_module(multiSmnt, PRINT_LEVEL_INFO);

#define SMTCN_DEBUG 1

#define SMTCN_RX_QUEUE_SIZE        4
//#define USE_SMTCN_STATIC_BUF


typedef struct {
    uint8_t    *payload;
    uint32_t    len;
} RAW_PKT_T, *PRAW_PKT_T;

static struct {
    uint16_t  search_idx;
    uint8_t   cur_chanl;
} t_smt_conn;
#define SMTCN_STATIC_BUF_COUNT     (SMTCN_RX_QUEUE_SIZE + 1)
#define SMTCN_STATIC_BUF_SIZE      64


#define SCAN_TIMES   (2 * 1000)    /* 2s */
#define SCAN_TIMETICKS (SCAN_TIMES/portTICK_PERIOD_MS)

#define SWITCH_CHANNEL_TIMETICKS (SWITCH_CHANNEL_TIMES/portTICK_PERIOD_MS)


#ifdef USE_SMTCN_STATIC_BUF

/******************************************************************************
 * static variable definition
 ******************************************************************************/
#ifdef __ICCARM__
_Pragma("data_alignment=4") static uint8_t  smtcn_rx_buf[SMTCN_STATIC_BUF_COUNT][SMTCN_STATIC_BUF_SIZE] = {{0}};
#else
static uint8_t __attribute__((aligned(4))) smtcn_rx_buf[SMTCN_STATIC_BUF_COUNT][SMTCN_STATIC_BUF_SIZE] = {{0}};
#endif

static uint8_t smtcn_rx_buf_idx = 0;
#endif

#if (SMTCN_DEBUG == 1)
static unsigned char channel_times[14] = {0};
#endif

static SemaphoreHandle_t smart_config_mutex = NULL;
static xQueueHandle smtcn_rx_queue = NULL;

/* smart connection thread wait scan done.*/
static SemaphoreHandle_t scan_done_mutex = NULL;

static multi_smtcn_proto_ops *psmtcn_proto_ops;

static uint32_t origin_rxfilter = 0;
static uint8_t origin_opmode = 0;
static uint8_t origin_bw = 0;

static unsigned char channel_list[] = {
    1, 6, 11, 2, 5, 7, 1, 6, 11, 10, 12, 3, 1, 6, 11, 8, 13, 4, 9, 14
};

static int8_t scan_loop_count = 0;

/*smart connection swtich channel timer */
static TimerHandle_t switch_channel_timer = NULL;

static TimerHandle_t scan_timer = NULL;

static smart_connection_callback_t g_pf_smnt_cb;
/******************************************************************************
 * extern variable definition,declaration
 ******************************************************************************/
multi_smtcn_info_t   saved_multi_smtcn_info;

extern multi_smtcn_proto_ops multi_smart_config_ops;

extern int g_supplicant_ready;


/******************************************************************************
 * function prototype
 ******************************************************************************/
/* static function for scan */
static int _msc_scan_prb_rsp_handler(wifi_event_t event_id, unsigned char *payload, unsigned int len);

static int _msc_check_scan(wifi_event_t event_id, unsigned char *payload, unsigned int len);

static int _msc_smtcn_done(void);

static uint8_t _msc_read_smtcn_flag(void);

static void _msc_deinit(void);

#if MTK_ALINK_ENABLE
void aws_smnt_start(smart_connection_callback_t pf_cb);
#endif
/******************************************************************************
 * static function definition
 ******************************************************************************/
static int _msc_raw_pkt_handler(unsigned char *payload, unsigned int len)
{
    P_HW_MAC_RX_DESC_T        pRxWI;
    uint8_t          ucRxPktType;
    uint8_t          ucMacHeaderLength;
    uint16_t         u2PayloadLength;
    uint8_t *pucMacHeader;       /* 802.11 header  */
    uint8_t *pucPayload;         /* 802.11 payload */
    uint16_t         u2RxStatusOffst;


    pRxWI   = (P_HW_MAC_RX_DESC_T)(payload);
    // =======  Process RX Vector (if any) ========

    // 1. ToDo: Cache RXStatus in SwRfb
    u2RxStatusOffst = sizeof(HW_MAC_RX_DESC_T);
    ucRxPktType = HAL_RX_STATUS_GET_PKT_TYPE(pRxWI);


    if ((ucRxPktType != RX_PKT_TYPE_RX_DATA) && (ucRxPktType != RX_PKT_TYPE_SW_DEFINED)) {
        return 0; // not handled
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

    //LOG_HEXDUMP_I(smnt, "mac header begin", pucMacHeader, ucMacHeaderLength+u2PayloadLength);
    psmtcn_proto_ops->rx_handler((char *)pucMacHeader, ucMacHeaderLength + u2PayloadLength);

    return 1; /* handled */
}

#ifdef MTK_SMTCN_V5_ENABLE
kal_uint8 bsmtcn_adapter_is_channel_locked(void);
#endif

static void _msc_switch_next_channel(TimerHandle_t tmr)
{
    unsigned char channel_list_size;
   
    if (psmtcn_proto_ops && psmtcn_proto_ops->switch_channel_rst) {
        if (0 != psmtcn_proto_ops->switch_channel_rst()) {
            LOG_E(multiSmnt, "switch_channel_rst fail.");
            return;
        }
    }

    channel_list_size = sizeof(channel_list) / sizeof(channel_list[0]);
    if (++t_smt_conn.search_idx >= channel_list_size) {
        t_smt_conn.search_idx = 0;
    }

    t_smt_conn.cur_chanl = channel_list[t_smt_conn.search_idx];

    wifi_config_set_channel(WIFI_PORT_STA, t_smt_conn.cur_chanl);

#if (SMTCN_DEBUG == 1)
    channel_times[t_smt_conn.cur_chanl - 1]++;
    /*
        uint8_t channel;
        wifi_config_get_channel(WIFI_PORT_STA, &channel);
        sc_dbg("now switched to channel :%d\n", channel);
    */
#endif
}

static void _msc_connect_to_ap(void)
{
#ifdef MTK_MINISUPP_ENABLE
    __g_wpa_supplicant_api.wpa_supplicant_disable_trigger_scan();
#endif
    wifi_connection_stop_scan();

    wifi_config_set_ssid(WIFI_PORT_STA, saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len);
    //wifi_config_set_channel(WIFI_PORT_STA, saved_multi_smtcn_info.channel);

    if (saved_multi_smtcn_info.auth_mode == WIFI_AUTH_MODE_OPEN) {
        if(saved_multi_smtcn_info.encrypt_type == WIFI_ENCRYPT_TYPE_WEP_ENABLED) {
            wifi_wep_key_t wep_key = {{{0}}};
            if (saved_multi_smtcn_info.pwd_len == 10 ||
                    saved_multi_smtcn_info.pwd_len == 26) {
                wep_key.wep_key_length[0] = saved_multi_smtcn_info.pwd_len / 2;
                AtoH((char *)saved_multi_smtcn_info.pwd, (char *)&wep_key.wep_key[0], (int)wep_key.wep_key_length[0]);
            } else if (saved_multi_smtcn_info.pwd_len == 5 ||
                       saved_multi_smtcn_info.pwd_len == 13) {
                wep_key.wep_key_length[0] = saved_multi_smtcn_info.pwd_len;
                memcpy(wep_key.wep_key[0], saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);
            } else {
                printf("wep key length error.\n");
                return;
            }

            wep_key.wep_tx_key_index = 0;
            wifi_config_set_wep_key(WIFI_PORT_STA, &wep_key);
            wifi_config_set_security_mode(WIFI_PORT_STA, WIFI_AUTH_MODE_OPEN, WIFI_ENCRYPT_TYPE_WEP_ENABLED);
        } else {
            wifi_config_set_security_mode(WIFI_PORT_STA, WIFI_AUTH_MODE_OPEN, WIFI_ENCRYPT_TYPE_WEP_DISABLED);
        }
    } else {
        wifi_config_set_security_mode(WIFI_PORT_STA, saved_multi_smtcn_info.auth_mode, saved_multi_smtcn_info.encrypt_type);
        wifi_config_set_wpa_psk_key(WIFI_PORT_STA, saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);
    }

    LOG_I(multiSmnt, "\n>>>>>> start connect <<<<<<:\n\n");
    
#ifdef MTK_MINISUPP_ENABLE
    //wifi_connection_stop_scan();
    __g_wpa_supplicant_api.wpa_supplicant_enable_trigger_scan();
#endif
    wifi_config_reload_setting();
}

static int32_t _msc_scan_done(void)
{
    wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE, (wifi_event_handler_t)_msc_scan_prb_rsp_handler);
    //wifi_connection_register_event_handler(WIFI_EVENT_IOT_NONE, (wifi_event_handler_t)_msc_scan_prb_rsp_handler);
    wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE, (wifi_event_handler_t)_msc_check_scan);
    //wifi_connection_register_event_handler(WIFI_EVENT_IOT_NONE, (wifi_event_handler_t)_msc_check_scan);

    return 0;
}

static void _msc_next_scan_loop(const char *reason)
{
    LOG_I(multiSmnt, "%s\n", reason);
    taskENTER_CRITICAL();
    if ((SCAN_FLAG_FIN == saved_multi_smtcn_info.scan_flag) ||
            (SCAN_FLAG_NOT_FOUND == saved_multi_smtcn_info.scan_flag)) {
        taskEXIT_CRITICAL();
        return;
    }

    if (++scan_loop_count >= MAX_SCAN_LOOP_COUNT) {
        saved_multi_smtcn_info.scan_flag = SCAN_FLAG_NOT_FOUND;
        _msc_scan_done();

        if (scan_done_mutex != NULL) {
            xSemaphoreGive(scan_done_mutex);
        }
        taskEXIT_CRITICAL();
    } else {
        if (scan_timer != NULL) {
            xTimerStart(scan_timer, 0);
        }
        taskEXIT_CRITICAL();
        wifi_connection_start_scan(NULL, 0, NULL, 0, 0);
        LOG_I(multiSmnt, "loop %d scan\n", scan_loop_count);
    }

}

static void _msc_scan_timeout(TimerHandle_t tmr)
{
    wifi_connection_stop_scan();

    /* Notice:
    * Here may need a semaphore to protect scan_loop_count against _msc_check_scan.
    */
    _msc_next_scan_loop("scan timeout");
}

static int _msc_check_scan(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    if (event_id != WIFI_EVENT_IOT_SCAN_COMPLETE) {
        return 0;
    }

    /* Notice:
    * Here may need a semaphore to protect scan_loop_count against _msc_scan_timeout.
    */
    xTimerStop(scan_timer, 0);
    _msc_next_scan_loop("scan complete event");
    return 1;
}

static int _msc_scan_prb_rsp_handler(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    if (event_id != WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE) {
        return 1;
    }

    if (len != 0) {
        wifi_scan_list_item_t ap_data;
        memset(&ap_data, 0, sizeof(wifi_scan_list_item_t));
        if (wifi_connection_parse_beacon(payload, len, &ap_data) >= 0) {
            if ((ap_data.ssid_length == saved_multi_smtcn_info.ssid_len) &&
                    !memcmp(ap_data.ssid, saved_multi_smtcn_info.ssid, ap_data.ssid_length)) {
                xTimerStop(scan_timer, 0);
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
                /*
                sc_dbg("channel:[%d], central_channel:[%d]\n",
                    ap_data.channel, ap_data.central_channel);
                    */

                saved_multi_smtcn_info.auth_mode = ap_data.auth_mode;
                saved_multi_smtcn_info.channel = ap_data.channel;
                saved_multi_smtcn_info.encrypt_type = ap_data.encrypt_type;

                LOG_I(multiSmnt, "scan finished!\n");

#if (SMTCN_DEBUG == 1)
                uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
                uint8_t passwd[WIFI_LENGTH_PASSPHRASE + 1] = {0};
                memcpy(ssid, saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len);
                memcpy(passwd, saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);

                LOG_I(multiSmnt, "ssid:[%s], channel:[%d], authMode:[%d], encrypt type:[%d], psk:[%s]\n",
                      ssid, saved_multi_smtcn_info.channel,
                      saved_multi_smtcn_info.auth_mode, saved_multi_smtcn_info.encrypt_type,
                      passwd);
#endif

                taskENTER_CRITICAL();
                saved_multi_smtcn_info.scan_flag = SCAN_FLAG_FIN;
                taskEXIT_CRITICAL();
                _msc_scan_done();
                xSemaphoreGive(scan_done_mutex);
            }
            return 0;
        }
    }

    return 1;
}

static int32_t _msc_scan(void)
{
    LOG_I(multiSmnt, "\n>>>>>> start scan <<<<<<\n\n");

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE,
                                           (wifi_event_handler_t) _msc_scan_prb_rsp_handler);

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE,
                                           (wifi_event_handler_t) _msc_check_scan);

    wifi_connection_start_scan(NULL, 0, NULL, 0, 0);
    xTimerStart(scan_timer, 0);

    return 0;
}

static void _msc_scan_cleanup(void)
{
    if (scan_done_mutex != NULL) {
        vSemaphoreDelete(scan_done_mutex);
        scan_done_mutex = NULL;
    }

    if (scan_timer != NULL) {
        xTimerDelete(scan_timer, 0);
        scan_timer = NULL;
    }
}

static int _msc_scan_init(void)
{
    uint8_t opmode = 0;

    if (scan_done_mutex == NULL) {
        scan_done_mutex = xSemaphoreCreateCounting((UBaseType_t)1, (UBaseType_t)0);
        if (scan_done_mutex == NULL) {
            LOG_E(multiSmnt, "scan_done_mutex create failed.\n");
            return -1;
        }
    }

    saved_multi_smtcn_info.scan_flag = SCAN_FLAG_NONE;

    scan_timer = xTimerCreate("scan_timer",
                              SCAN_TIMETICKS,
                              pdFALSE,
                              NULL,
                              _msc_scan_timeout);
    if (scan_timer == NULL) {
        LOG_E(multiSmnt, "switch_channel_timer create fail.\n");
        goto fail;
    }

    wifi_config_set_ssid(WIFI_PORT_STA, saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len);

    if (saved_multi_smtcn_info.pwd_len != 0) {
        wifi_config_set_wpa_psk_key(WIFI_PORT_STA, saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);
    }

    wifi_config_reload_setting();

    wifi_config_get_opmode(&opmode);

    if((opmode != WIFI_MODE_STA_ONLY) &&
            (wifi_config_set_opmode(WIFI_MODE_STA_ONLY) < 0)) {
        printf("config sta mode failed, cannot continue smart config\n");
        goto fail;
    }

    LOG_I(multiSmnt, "Now STA only mode\n");

    scan_loop_count = 0;
    return 0;

fail:
    _msc_scan_cleanup();
    return -1;
}

static int32_t _msc_scan_connect_ap(void)
{
#ifdef MTK_MINISUPP_ENABLE
    while(!g_supplicant_ready) {
        vTaskDelay(50);
    }
#endif
    LOG_I(multiSmnt, "Now start scan and connect.\n");
    if (_msc_scan_init() != 0) {
        printf("_msc_scan initialize failed.\n");
        return -1;
    }
    _msc_scan();
    xSemaphoreTake(scan_done_mutex, portMAX_DELAY);
    _msc_scan_cleanup();

    if(SCAN_FLAG_FIN == saved_multi_smtcn_info.scan_flag) {
        _msc_connect_to_ap();
    } else if(SCAN_FLAG_NOT_FOUND == saved_multi_smtcn_info.scan_flag) {
#if (SMTCN_DEBUG == 1)
        uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
        memcpy(ssid, saved_multi_smtcn_info.ssid, WIFI_MAX_LENGTH_OF_SSID);
        LOG_I(multiSmnt, "ssid:[%s] not found,"
              " 7687 may connect to the original AP.\n", ssid);
#else
        printf("smart connection target AP not found,"
               " 7687 may connect to the original AP.\n", ssid);
#endif
    }

    return 0;
}

static int32_t _msc_rx_handler(uint8_t *payload, uint32_t len)
{
    RAW_PKT_T     pkt;

#ifdef USE_SMTCN_STATIC_BUF
    pkt.len = len;
    pkt.payload  = smtcn_rx_buf[smtcn_rx_buf_idx];
    memcpy(pkt.payload, payload, SMTCN_STATIC_BUF_SIZE);
    if (xQueueSendToBack(smtcn_rx_queue, (void *)&pkt, (TickType_t)5) == pdPASS) {
        smtcn_rx_buf_idx = (smtcn_rx_buf_idx + 1) % SMTCN_STATIC_BUF_COUNT;
    }
#else
    /* assign pkt */
    pkt.len = len;
    if ((pkt.payload = pvPortMalloc(len)) == NULL) {
        LOG_E(multiSmnt, "%s:%d,pvPortMalloc failed\n", __FILE__, __LINE__);
        return 1;
    }
    memcpy(pkt.payload, payload, len);

    if (xQueueSendToBack(smtcn_rx_queue, (void *)&pkt, (TickType_t)5) != pdPASS) {
        vPortFree(pkt.payload);
    }
#endif

    return 1;
}

static void _msc_rcv_pkt_thread(void *pvParameters)
{
    smt_connt_result_t t_result;

    if (smart_config_mutex == NULL) {
        LOG_E(multiSmnt, "mutex not created, please create the mutex and then call this function.\n");
        vTaskDelete(NULL);
    }
    xSemaphoreTake(smart_config_mutex, portMAX_DELAY);
    LOG_I(multiSmnt, "\n>>>>>> Smart connect begin <<<<<<\n\n");

    memset((void *)&saved_multi_smtcn_info, 0, sizeof(saved_multi_smtcn_info));

    uint8_t smtcn_flag = 0;
    while ((SMTCN_FLAG_FIN != smtcn_flag) &&
            (SMTCN_FLAG_EXIT != smtcn_flag) &&
            (SMTCN_FLAG_FAIL != smtcn_flag)) {
        RAW_PKT_T pkt;
        if (xQueueReceive(smtcn_rx_queue, (void *)&pkt, portMAX_DELAY) == pdPASS) {
            /* process this raw packet */
            _msc_raw_pkt_handler(pkt.payload, pkt.len);
#ifndef USE_SMTCN_STATIC_BUF
            vPortFree(pkt.payload);
#endif
        }

        smtcn_flag = _msc_read_smtcn_flag();
    }

    _msc_smtcn_done();
    LOG_I(multiSmnt, "\n>>>>>> Smart connect finished <<<<<<\n");
    if((SMTCN_FLAG_EXIT == smtcn_flag) || (SMTCN_FLAG_FAIL == smtcn_flag)) {
        goto thread_exit;
    }
    if(g_pf_smnt_cb == NULL) {
        _msc_scan_connect_ap();
    } else {
        memset(&t_result,0,sizeof(t_result));
        t_result.pwd_len = saved_multi_smtcn_info.pwd_len;
        t_result.ssid_len = saved_multi_smtcn_info.ssid_len;
        t_result.tlv_data_len = saved_multi_smtcn_info.tlv_data_len;
        t_result.tlv_data = saved_multi_smtcn_info.tlv_data;
        memcpy(t_result.pwd, saved_multi_smtcn_info.pwd, WIFI_LENGTH_PASSPHRASE);
        memcpy(t_result.ssid, saved_multi_smtcn_info.ssid, WIFI_MAX_LENGTH_OF_SSID);
        g_pf_smnt_cb(WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED, &t_result);
    }

thread_exit:
    LOG_I(multiSmnt, "give smart_config_mutex:[0x%0x]\n", (int)smart_config_mutex);
    _msc_deinit();
    xSemaphoreGive(smart_config_mutex);
    vTaskDelete(NULL);
}

static int _msc_smtcn_done(void)
{
    wifi_config_set_rx_filter(origin_rxfilter);
    wifi_config_set_smart_connection_filter(0);

    wifi_config_set_bandwidth(WIFI_PORT_STA, origin_bw);
    wifi_config_unregister_rx_handler();
    LOG_I(multiSmnt, "unregister rx handler finished.\n");

    RAW_PKT_T pkt;
    while (xQueueReceive(smtcn_rx_queue, (void *)&pkt, (TickType_t)0) == pdPASS) {
#ifndef USE_SMTCN_STATIC_BUF
        vPortFree(pkt.payload);
#endif
    }

    return 0;
}

static uint8_t _msc_read_smtcn_flag(void)
{
    return saved_multi_smtcn_info.smtcn_flag;;
}

static int _msc_init (const uint8_t *key, const uint8_t key_length, smnt_type_e e_type)
{
    int ret;
    if(psmtcn_proto_ops == NULL) {
        ret = msc_ctl_register_multi_proto(e_type);
        if(ret != MSC_OK) {
            return ret;
        }
        psmtcn_proto_ops = &multi_smart_config_ops;
    }

    if (smart_config_mutex == NULL) {
        smart_config_mutex = xSemaphoreCreateMutex();
        if (smart_config_mutex == NULL) {
            LOG_E(multiSmnt, "smart_config_mutex create failed.\n");
            return -1;
        }
    }

    if (smtcn_rx_queue == NULL) {
        smtcn_rx_queue = xQueueCreate(SMTCN_RX_QUEUE_SIZE, sizeof(RAW_PKT_T));
        if (smtcn_rx_queue == NULL) {
            LOG_E(multiSmnt, "smtcn_rx_queue create failed.\n");
            goto fail;
        }
        configASSERT(smtcn_rx_queue);
        vQueueAddToRegistry(smtcn_rx_queue, "smart connect");
    }

    if(switch_channel_timer == NULL) {
        switch_channel_timer = xTimerCreate("switch_channel_timer",
                                            SWITCH_CHANNEL_TIMETICKS,
                                            pdTRUE,
                                            NULL,
                                            _msc_switch_next_channel);
        if (switch_channel_timer == NULL) {
            LOG_E(multiSmnt, "switch_channel_timer create fail.\n");
            goto fail;
        }
    }

    if (psmtcn_proto_ops && psmtcn_proto_ops->init) {
        if (psmtcn_proto_ops->init(key, key_length) != 0) {
            goto fail;
        }
    }
    return 0;

fail:
    _msc_deinit();
    return MSC_FAIL;
}

static void _msc_deinit(void)
{
    if (smtcn_rx_queue != NULL) {
        vQueueDelete(smtcn_rx_queue);
        smtcn_rx_queue = NULL;
    }

    if (switch_channel_timer != NULL) {
        xTimerDelete(switch_channel_timer, 0);
        switch_channel_timer = NULL;
    }

    if (psmtcn_proto_ops && psmtcn_proto_ops->cleanup) {
        psmtcn_proto_ops->cleanup();
    }

    psmtcn_proto_ops = NULL;
}


static int32_t _msc_start (const uint8_t *key, const uint8_t key_length, smnt_type_e e_type)
{
    static TaskHandle_t smtcn_handle = NULL;
    eTaskState smtcn_state;
    int32_t ret = MSC_OK;

    /* Here is just to avoid creating too many pending tasks which consume
     * too much memory.
     */
    if ((smtcn_handle != NULL) &&
            ((smtcn_state = eTaskGetState(smtcn_handle)) != eReady) &&
            (smtcn_state != eDeleted)) {
        LOG_E(multiSmnt, "smtcn task state:%d, cannot create it\n", smtcn_state);
        return MSC_FAIL;
    }

    ret = _msc_init(key, key_length, e_type);

    if(ret != MSC_OK) {
        return ret;
    }

    wifi_config_get_opmode(&origin_opmode);
    if (origin_opmode == WIFI_MODE_AP_ONLY) {
        LOG_W(multiSmnt, "7687 in opmode:[%d], cannot continue smart connection\n",
              origin_opmode);
        return MSC_FAIL;
    }

    if (xTaskCreate(_msc_rcv_pkt_thread, MSC_TASK_NAME, MSC_TASK_STACKSIZE / sizeof(StackType_t), NULL, MSC_TASK_PRIO, &smtcn_handle) != pdPASS) {
        LOG_E(multiSmnt, "_msc_rcv_pkt_thread create failed\n");
        return MSC_FAIL;
    }


    uint32_t rx_filter;
    wifi_config_get_rx_filter(&origin_rxfilter);
    rx_filter = origin_rxfilter;
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_NOT_MY_BSSID);
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_NOT_UC2ME);
    rx_filter |= BIT(WIFI_RX_FILTER_DROP_PROBE_REQ);
    rx_filter |= BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);

#if defined (MTK_SMTCN_V4_ENABLE) && (defined (MTK_SMTCN_V5_ENABLE) || defined(MTK_AIRKISS_ENABLE))
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_BC_FRAME);
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_MC_FRAME);
#elif defined (MTK_SMTCN_V5_ENABLE) || defined(MTK_AIRKISS_ENABLE)
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_BC_FRAME);
    rx_filter |= BIT(WIFI_RX_FILTER_DROP_MC_FRAME);
#elif defined(MTK_SMTCN_V4_ENABLE)
    rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_MC_FRAME);
    rx_filter |= BIT(WIFI_RX_FILTER_DROP_BC_FRAME);
#endif

    wifi_config_set_rx_filter(rx_filter);

    wifi_config_set_smart_connection_filter(1);

    wifi_config_get_bandwidth(WIFI_PORT_STA, &origin_bw);
    wifi_config_set_bandwidth(WIFI_PORT_STA, WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ);

    wifi_config_register_rx_handler((wifi_rx_handler_t)_msc_rx_handler);

    memset((void *)&t_smt_conn, 0, sizeof(t_smt_conn));
    t_smt_conn.cur_chanl = channel_list[0];

    wifi_config_set_channel(WIFI_PORT_STA, t_smt_conn.cur_chanl);
#if (SMTCN_DEBUG == 1)
    memset(channel_times, 0, sizeof(channel_times));
    channel_times[t_smt_conn.cur_chanl - 1]++;
#endif
    xTimerStart(switch_channel_timer, 0);

    return MSC_OK;
}

static void _msc_stop(void)
{
    msc_write_flag(SMTCN_FLAG_EXIT);
}


/******************************************************************************
 * extern function definition
 ******************************************************************************/

void msc_aes_decrypt(uint8_t *cipher_blk, uint32_t cipher_blk_size,
                              uint8_t *key, uint8_t key_len,
                              uint8_t *plain_blk, uint32_t *plain_blk_size)
{
 
#ifdef MTK_WIFI_ROM_ENABLE
    //RT_AES_Decrypt(cipher_blk,cipher_blk_size,key,key_len,plain_blk,plain_blk_size);

    hal_aes_buffer_t * plain_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));
    hal_aes_buffer_t * cipher_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));
    hal_aes_buffer_t * key_buf = (hal_aes_buffer_t *)pvPortMallocNC(sizeof(hal_aes_buffer_t));

    uint8_t * cpt = (uint8_t *)pvPortMallocNC(cipher_blk_size);
    uint8_t * plt = (uint8_t *)pvPortMallocNC(*plain_blk_size);
    uint8_t * p_key = (uint8_t *)pvPortMallocNC(key_len);
    
    if(cpt == NULL || plt == NULL || p_key == NULL || plain_buf == NULL || cipher_buf == NULL || key_buf == NULL)
    {
        configASSERT(0);
        return ;
    }

    memcpy(cpt,cipher_blk,cipher_blk_size);
    memcpy(p_key,key,key_len);

    
    plain_buf->buffer = plt;
    plain_buf->length = *plain_blk_size;
    cipher_buf->buffer = cpt;
    cipher_buf->length = cipher_blk_size;
    key_buf->buffer = p_key;
    key_buf->length = key_len;
    hal_aes_ecb_decrypt(plain_buf, cipher_buf, key_buf); 
    memcpy(plain_blk,plt,*plain_blk_size);
    vPortFreeNC(cpt);
    vPortFreeNC(plt);
    vPortFreeNC(p_key);
    vPortFreeNC(plain_buf);
    vPortFreeNC(cipher_buf);
    vPortFreeNC(key_buf);
    
#else
    hal_aes_buffer_t plain_buf, cipher_buf, key_buf;
    plain_buf.buffer = plain_blk;
    plain_buf.length = *plain_blk_size;
    cipher_buf.buffer = cipher_blk;
    cipher_buf.length = cipher_blk_size;
    key_buf.buffer = key;
    key_buf.length = key_len;
    hal_aes_ecb_decrypt(&plain_buf, &cipher_buf, &key_buf); 
#endif
}


int32_t msc_save_info(void)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", WIFI_MODE_STA_ONLY);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
                         NVDM_DATA_ITEM_TYPE_STRING,
                         (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    memcpy(buf, saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid",
                         NVDM_DATA_ITEM_TYPE_STRING,
                         (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", saved_multi_smtcn_info.ssid_len);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen",
                         NVDM_DATA_ITEM_TYPE_STRING,
                         (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", saved_multi_smtcn_info.auth_mode);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode",
                         NVDM_DATA_ITEM_TYPE_STRING,
                         (uint8_t *)buf, strlen(buf));

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", saved_multi_smtcn_info.encrypt_type);
    nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType",
                         NVDM_DATA_ITEM_TYPE_STRING,
                         (uint8_t *)buf, strlen(buf));

    if(saved_multi_smtcn_info.auth_mode != WIFI_AUTH_MODE_OPEN) {
        memset(buf, 0, sizeof(buf));
        memcpy(buf, saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);
        nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (uint8_t *)buf, strlen(buf));

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", saved_multi_smtcn_info.pwd_len);
        nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (uint8_t *)buf, strlen(buf));
    }

    if((saved_multi_smtcn_info.auth_mode == WIFI_AUTH_MODE_OPEN || saved_multi_smtcn_info.auth_mode == WIFI_AUTH_MODE_SHARED)
        && (saved_multi_smtcn_info.encrypt_type == WIFI_ENCRYPT_TYPE_WEP_ENABLED)) {
        if (saved_multi_smtcn_info.pwd_len == 5 ||
                saved_multi_smtcn_info.pwd_len == 13 ||
                saved_multi_smtcn_info.pwd_len == 10 ||
                saved_multi_smtcn_info.pwd_len == 26) {
            wifi_wep_key_t wep_key;
            wep_key.wep_tx_key_index = 0;
            wep_key.wep_key_length[0] = saved_multi_smtcn_info.pwd_len;
            memcpy(wep_key.wep_key[0], saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);

            memset(buf, 0, sizeof(buf));
            memcpy(buf, wep_key.wep_key[0], wep_key.wep_key_length[0]);
            buf[wep_key.wep_key_length[0]] = ',';
            nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKey",
                                 NVDM_DATA_ITEM_TYPE_STRING,
                                 (uint8_t *)buf, strlen(buf));

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d,", wep_key.wep_key_length[0]);
            nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKeyLen",
                                 NVDM_DATA_ITEM_TYPE_STRING,
                                 (uint8_t *)buf, strlen(buf));

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d", wep_key.wep_tx_key_index);
            nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "DefaultKeyId",
                                 NVDM_DATA_ITEM_TYPE_STRING,
                                 (uint8_t *)buf, strlen(buf));
        }
    }

    return 0;
}

uint8_t msc_get_current_channel(void)
{
    return t_smt_conn.cur_chanl;
}

void msc_stop_switch_channel(void)
{
    xTimerStop(switch_channel_timer, 0);

#if (SMTCN_DEBUG == 1)
    LOG_I(multiSmnt, "channel locked at %d, scaned %d times\n",
          t_smt_conn.cur_chanl,
          channel_times[t_smt_conn.cur_chanl - 1]);
#endif
}

void msc_continue_switch_channel(void)
{
    _msc_switch_next_channel(NULL);
    xTimerStart(switch_channel_timer, 0);
}

void msc_write_flag(uint8_t flag_value)
{
    saved_multi_smtcn_info.smtcn_flag = flag_value;
}

int32_t msc_scan_connect_ap(unsigned char *ssid, unsigned char *passwd)
{
    if(ssid == NULL) {
        return -1;
    }

    if(ssid != saved_multi_smtcn_info.ssid) {
        memcpy(saved_multi_smtcn_info.ssid, ssid, strlen((const char *)ssid));
        saved_multi_smtcn_info.ssid_len = strlen((const char *)ssid);
    }

    if(passwd != saved_multi_smtcn_info.pwd) {
        memcpy(saved_multi_smtcn_info.pwd, passwd, strlen((const char *)passwd));
        saved_multi_smtcn_info.pwd_len = strlen((const char *)passwd);
    }

    return _msc_scan_connect_ap();
}

/**
 * @function wifi_multi_smart_connection_start
 *
 * @brief    This function will start the smart connection task , user can choose one or multi type.
 *           When get the correct information from the app, the task will be terminated, or user can stop the
 *           task by calling #wifi_multi_smart_connection_stop
 *
 * @param    [IN] t_device_info      fill the information needed by the smart connection
 *                                          if e_type == SUPPORT_JOYLINK,please fill t_device_info.joylink_device_info
 *                                          if e_type == SUPPORT_MTK_SMNT , SUPPORT_AIRKISS , SUPPORT_AIRKISS | SUPPORT_MTK_SMNT, please fill t_device_info.am_device_info
 * @param    [IN] e_type             please refer to #smnt_type_e, you can choose one or multi type.
 *                                   currently you can set e_type as below values:
 *                                          SUPPORT_MTK_SMNT                       only start mtk smart connection
 *                                          SUPPORT_AIRKISS                        only start airkiss
 *                                          SUPPORT_MTK_SMNT | SUPPORT_AIRKISS     start both mtk smart connection and airkiss
 *                                          SUPPORT_JOYLINK                        only start joylink
 *                                          SUPPORT_AWS                            only start aws
 *                                          if you set the e_type value none of them, you will get the MSC_SMNT_TYPE_INCORRECT error
 *
 * @param    [IN] pf_callback
 *                     if pf_callback != NULL , please refer to #smart_connection_callback_t
                       if pf_callback == NULL , the default behavior is connect to the ap
 * @return    MSC_FAIL
 *            MSC_OK
 *            MSC_SMNT_TYPE_INCORRECT
 *            MSC_INVALID_ARG
 */
int32_t wifi_multi_smart_connection_start (msc_device_info_t t_device_info, smnt_type_e e_type, smart_connection_callback_t pf_callback)
{
    int32_t i4_ret = MSC_OK;
    LOG_I(multiSmnt, "e_type is %d\n", e_type);

    g_pf_smnt_cb = pf_callback;

    switch(e_type) {
#ifdef MTK_JOYLINK_ENABLE
        case SUPPORT_JOYLINK: {
            DEVICE_INFO_T t_joylink_info;

            memcpy(&t_joylink_info, &t_device_info.joylink_device_info, sizeof(t_joylink_info));
            i4_ret =  joylink_smnt_start(&t_joylink_info,pf_callback);
            break;
        }
#endif

#ifdef MTK_ALINK_ENABLE
        case SUPPORT_AWS:
            aws_smnt_start(pf_callback);
            i4_ret =  MSC_OK;
            break;
#endif

#if defined MTK_SMTCN_V4_ENABLE || defined MTK_AIRKISS_ENABLE || defined MTK_SMTCN_V5_ENABLE
        case SUPPORT_MTK_SMNT:
        case SUPPORT_AIRKISS:
        case SUPPORT_MTK_SMNT | SUPPORT_AIRKISS:
            i4_ret =  _msc_start(t_device_info.am_device_info.key, t_device_info.am_device_info.key_len, e_type);
            break;
#endif

        default:
            LOG_E(multiSmnt, "e_type is not correct\n");
            i4_ret = MSC_SMNT_TYPE_INCORRECT;
    }
    return i4_ret;
}


/**
 * @function wifi_multi_smart_connection_stop
 *
 * @brief    stop smart connection task
 */
void wifi_multi_smart_connection_stop(void)
{
#ifdef MTK_JOYLINK_ENABLE
    joylink_smnt_stop();
#endif

#if defined MTK_SMTCN_V4_ENABLE || defined MTK_AIRKISS_ENABLE || defined MTK_SMTCN_V5_ENABLE
    _msc_stop();
#endif
}

uint8_t _multi_test(uint8_t len, char *param[])
{
    int parm_len = 0;
    int i = 0;
    smnt_type_e smnt = 0;
    cloud_platform_e cloud = 0;
    
    if(len < 2)
    {
        printf("cmd not correct\n");
        return -1;
    }

    parm_len = strlen(param[1]);

    if(strcmp(param[0],"cloud") == 0) {
        for(; i < parm_len; ++i) {
            switch(param[1][i]) {
                case 'a':
                    cloud |= CLOUD_SMART_PLATFORM_ALINK;
                    printf("--------add alink-------\n");
                    break;
                case 't':
                    cloud |= CLOUD_SMART_PLATFORM_AIRKISS;
                    printf("------add airkiss-------\n");
                    break;
                case 'j':
                    cloud |= CLOUD_SMART_PLATFORM_JOYLINK;
                    printf("------add jollink-------\n");
                    break;
                case ' ':
                case '\t':
                    continue;
                default:
                    printf("--------------wrong param--------\n");
                    return -1;
            }
        }

        msc_config_set_cloud_platform(cloud);
    } else if(strcmp(param[0],"smnt") == 0) {
        for(; i < parm_len; ++i) {
            switch(param[1][i]) {
                case 'm':
                    smnt |= SUPPORT_MTK_SMNT;
                    printf("------add mtk smart-------\n");
                    break;
                case 't':
                    smnt |= SUPPORT_AIRKISS;
                    printf("------add airkiss -------\n");
                    break;
                case 'j':
                    smnt |= SUPPORT_JOYLINK;
                    printf("------add joylink -------\n");
                    break;
                case 'a':
                    smnt |= SUPPORT_AWS;
                    printf("---------add aws -------\n");
                    break;
                case ' ':
                case '\t':
                    continue;
                default:
                    printf("--------------wrong param--------\n");
                    return -1;
            }
        }

        msc_config_set_smnt_type(smnt);
    } else {
        printf("--------------wrong param---------------\n");
        return -1;
    }

    msc_config_set_smnt_autostart(1);
    return 0;
}

