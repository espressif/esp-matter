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
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
#include "semphr.h"
#include "syslog.h"

#include "joylink_innet_ctl.h"
#include "joylink_smnt_api.h"

#include "wifi_api.h"
#include "wifi_scan.h"
#include "wifi_rx_desc.h"


/*************************************************************
 * macro typedef
 *************************************************************/

log_create_module(jdSmtc, PRINT_LEVEL_INFO);

#define CHECK_FAIL(_statement) do\
{\
	int _ret = _statement;\
	if(_ret < 0)\
	{\
		LOG_E(jdSmtc,"fail ,ret value is  %d\n",_ret);\
		return _ret;\
	}\
}while(0)

#define LOG_FAIL(_statement) do\
{\
	int _ret = _statement;\
	if(_ret < 0)\
	{\
		LOG_E(jdSmtc,"fail ,ret value is  %d\n",_ret);\
	}\
}while(0)

#define IS_MULTICAST_ADDR(Addr) 	((((Addr[0]) & 0x01) == 0x01) && ((Addr[0]) != 0xff))

#define switch_timems   			(50) /* 50ms*/
#define switch_timeticks 			(switch_timems/portTICK_PERIOD_MS)

#define promiscuous_rxfilter 		(0x1002)

#define JDSMTCN_RX_QUEUE_SIZE       4
#define JDSMTCN_STATIC_BUF_COUNT    (JDSMTCN_RX_QUEUE_SIZE + 1)
#define JDSMTCN_STATIC_BUF_SIZE     64

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_PRINT(mac) mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]

#define SSID_PWD_MAX_LEN 			33

typedef enum {
    JD_NONE,
    JD_FINISH,
    JD_EXIT
} JDSMTCN_FLAG_E;

typedef struct {
    uint8_t    *payload;
    uint32_t    len;
} raw_pkt_t;

#ifdef __ICCARM__
_Pragma("data_alignment=4") uint8_t  jdSmtcn_rx_buf[JDSMTCN_STATIC_BUF_COUNT][JDSMTCN_STATIC_BUF_SIZE] = {{0}};
#else
uint8_t __attribute__((aligned(4))) jdSmtcn_rx_buf[JDSMTCN_STATIC_BUF_COUNT][JDSMTCN_STATIC_BUF_SIZE] = {{0}};
#endif


/*************************************************************
 * variable definition
 *************************************************************/

static uint8_t		 jdSmtcn_rx_buf_idx = 0;

static uint32_t		 ui4_saved_rxfilter = 0;
static uint8_t 		 ui1_saved_bw = 0;
static TimerHandle_t h_switch_timer = NULL;
static TaskHandle_t  h_smtc_handle = NULL;
static xQueueHandle  h_smtcn_rx_queue = NULL;
static bool 		 b_init = false;

static JDSMTCN_FLAG_E e_joylink_smnt_flag = JD_NONE;

SemaphoreHandle_t h_wait_wifi_ready_sem = NULL;

joylink_innet_result_t t_result;

static smart_connection_callback_t g_pf_event_cb;

void * queue_mutex = NULL;

/*************************************************************
 * function prototype
 *************************************************************/

static int _joylink_smnt_raw_pkt_handler(unsigned char *payload, unsigned int len);
static int _joylink_smnt_frame_tx_cb(JL_FRAME_SUBTYPE frame_type,  void *frame);
static int _joylink_smnt_change_channel_cb (uint8_t channel);
static void _joylink_smnt_innet_result_cb(joylink_innet_result_t result_info);
static int _joylink_smnt_rx_data_callback(unsigned char *payload, int len);
static void _joylink_smnt_switch_timer_cb(TimerHandle_t tmr);
static int _joylink_smnt_save_env();
static int _joylink_smnt_restore_env();
static int _joylink_smnt_init(const DEVICE_INFO_T *pt_info);
static void _joylink_smnt_deinit();
static void _joylink_smnt_rcv_pkt_thread(void *pvParameters);
static void _joylink_smnt_connect_to_ap(joylink_innet_result_t *pt_result);


/*************************************************************
 * static function definition
 *************************************************************/

/**
 * @function _joylink_smnt_raw_pkt_handler
 *
 * @brief	 process the raw packet
 *
 * @param	 [IN] payload
 * @param	 [IN] len
 * @return   0 succss , -1 fail
 */
static int _joylink_smnt_raw_pkt_handler(unsigned char *payload, unsigned int len)
{
    P_HW_MAC_RX_DESC_T		  pRxWI;
    uint8_t 		 	      ucRxPktType;
    uint8_t 				  ucMacHeaderLength;
    uint16_t			      u2PayloadLength;
    uint8_t 	             *pucMacHeader;		 /* 802.11 header  */
    uint8_t 		         *pucPayload;		 /* 802.11 payload */
    uint16_t			      u2RxStatusOffst;
    joylink_frame_udp_t       t_udp_broadcast_frame;


    pRxWI	= (P_HW_MAC_RX_DESC_T)(payload);
    // =======	Process RX Vector (if any) ========

    // 1. ToDo: Cache RXStatus in SwRfb
    u2RxStatusOffst = sizeof(HW_MAC_RX_DESC_T);
    ucRxPktType = HAL_RX_STATUS_GET_PKT_TYPE(pRxWI);

    if ((ucRxPktType != RX_PKT_TYPE_RX_DATA) && (ucRxPktType != RX_PKT_TYPE_SW_DEFINED)) {
        return 0; // not handled
    }

    if (ucRxPktType == RX_PKT_TYPE_RX_DATA ||
            ucRxPktType == RX_PKT_TYPE_DUP_RFB) {
        uint16_t		  u2GrpOffst;
        uint8_t 		  ucGroupVLD;

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

    if ( (IS_MULTICAST_ADDR((pucMacHeader + 4)) ||
            IS_MULTICAST_ADDR((pucMacHeader + 16)) ) &&
            (pucMacHeader[0] & 0x0C) == 0x08) {
        t_udp_broadcast_frame.length = u2PayloadLength;
        t_udp_broadcast_frame.head_802_11 = (PHEADER_802_11_SMNT)pucMacHeader;
        joylink_innet_rx_handler(FRAME_SUBTYPE_UDP_MULTIANDBROAD,&t_udp_broadcast_frame);
    }
    return 0;
}

/**
 * @function _joylink_smnt_change_channel_cb
 *
 * @brief    registered by #joylink_innet_init
 *
 * @param    [IN] frame_type
 * @param    [IN] frame
 */
static int _joylink_smnt_frame_tx_cb(JL_FRAME_SUBTYPE frame_type,  void *frame)
{
    LOG_I(jdSmtc,"tx cb\n");
    return 0;
}

/**
 * @function _joylink_smnt_change_channel_cb
 *
 * @brief    registered by #joylink_innet_init
 *			 used to change the channel
 *
 * @param    [IN] channel
 *
 * @return  >=0 success, <0 fail
 */
static int _joylink_smnt_change_channel_cb (uint8_t channel)
{
    CHECK_FAIL(wifi_config_set_channel(WIFI_PORT_STA,channel));
    return 0;
}

/**
 * @function _joylink_smnt_innet_result_cb
 *
 * @brief    registered by #joylink_innet_init
 *			 when smart connection finished, this function will be called
 *
 * @param    [IN] result_info
 */
static void _joylink_smnt_innet_result_cb(joylink_innet_result_t result_info)
{
    if(strstr(result_info.pwd,"erro") == NULL)
    {
        t_result = result_info;
        e_joylink_smnt_flag = JD_FINISH;
    }
}

/**
 * @function _joylink_smnt_rx_data_callback
 *
 * @brief    registered by #wifi_config_register_rx_handler
 *
 * @param    [IN] payload
 * @param    [IN] len
 */
static int _joylink_smnt_rx_data_callback(unsigned char *payload, int len)
{
    xSemaphoreTake(queue_mutex, portMAX_DELAY);
    raw_pkt_t  pkt;
    pkt.len = len;
    pkt.payload  = jdSmtcn_rx_buf[jdSmtcn_rx_buf_idx];

    if(h_smtcn_rx_queue == NULL)
        return 1;

    memcpy(pkt.payload, payload, JDSMTCN_STATIC_BUF_SIZE);

    if (xQueueSendToBack(h_smtcn_rx_queue, (void *)&pkt, (TickType_t)5) == pdPASS) {
        jdSmtcn_rx_buf_idx = (jdSmtcn_rx_buf_idx + 1) % JDSMTCN_STATIC_BUF_COUNT;
    }
    xSemaphoreGive(queue_mutex);
    return 1;
}

/**
 * @function _joylink_smnt_switch_timer_cb
 *
 * @brief    call back function of the switch timer
 */
static void _joylink_smnt_switch_timer_cb(TimerHandle_t tmr)
{
    joylink_innet_timingcall();
}

/**
 * @function _joylink_smnt_save_env
 *
 * @brief    save the enviroment before smart connection process
 *
 * @return   0 success
 *			 -1 fail
 */
static int _joylink_smnt_save_env()
{
    CHECK_FAIL(wifi_config_get_bandwidth(WIFI_PORT_STA, &ui1_saved_bw));
    CHECK_FAIL(wifi_config_get_rx_filter(&ui4_saved_rxfilter));
    return 0;
}

/**
 * @function _joylink_smnt_restore_env
 *
 * @brief    restore the enviroment after smart connection process
 *
 * @return   0 success
 *			 -1 fail
 */
static int _joylink_smnt_restore_env()
{
    if(ui1_saved_bw != 0) {
        LOG_FAIL(wifi_config_set_bandwidth(WIFI_PORT_STA, ui1_saved_bw));
    }

    if(ui4_saved_rxfilter != 0) {
        LOG_FAIL(wifi_config_set_rx_filter(ui4_saved_rxfilter));
    }

    LOG_FAIL(wifi_config_unregister_rx_handler());
    return 0;
}

/**
 * @function _joylink_smnt_init
 *
 * @brief    init function
 *
 * @param    [IN]pt_info
 *
 * @return   0 success
 *			 -1 fail
 */
static int _joylink_smnt_init(const DEVICE_INFO_T *pt_info)
{
    if(!b_init) {
        joylink_innet_param_t t_param;

        memset(&t_param,0,sizeof(t_param));

        t_param.printf_func			 = printf;
        t_param.config_finish_cb 	 = _joylink_smnt_innet_result_cb;
        t_param.frame_tx_cb			 = _joylink_smnt_frame_tx_cb;
        t_param.switch_channel_cb	 = _joylink_smnt_change_channel_cb;

        if(pt_info != NULL) {
            memcpy(t_param.smnt_key,pt_info->smnt_key,SIZE_SMNT_KEY);
            memcpy(t_param.cid,pt_info->cid,SIZE_CID);
            memcpy(t_param.puid,pt_info->puid,SIZE_PUID);
        }

        memset(&t_result,0,sizeof(t_result));

        ui1_saved_bw = 0;
        ui4_saved_rxfilter = 0;
        h_switch_timer = NULL;
        h_smtcn_rx_queue = NULL;
        jdSmtcn_rx_buf_idx = 0;
        e_joylink_smnt_flag = JD_NONE;
        h_smtc_handle = NULL;

        CHECK_FAIL(wifi_config_get_mac_address(WIFI_PORT_STA,t_param.mac));

        CHECK_FAIL(joylink_innet_init(t_param));

        CHECK_FAIL(_joylink_smnt_save_env());

        h_switch_timer = xTimerCreate("switch_timer",
                                      switch_timeticks,
                                      pdTRUE,
                                      NULL,
                                      _joylink_smnt_switch_timer_cb);

        if(h_switch_timer == NULL) {
            LOG_E(jdSmtc,"create timer fail\n");
            return -1;
        }

        h_smtcn_rx_queue = xQueueCreate(JDSMTCN_RX_QUEUE_SIZE, sizeof(raw_pkt_t));
        if (h_smtcn_rx_queue == NULL) {
            LOG_E(jdSmtc,"create queue fail\n");
            return -1;
        }
        queue_mutex = xSemaphoreCreateMutex();
        if(queue_mutex == NULL) {
            LOG_E(jdSmtc,"create mutex fail\n");
            return -1;
        }
        CHECK_FAIL(wifi_config_set_opmode(WIFI_MODE_STA_ONLY));
        CHECK_FAIL(wifi_config_set_bandwidth(WIFI_PORT_STA, WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ));

        CHECK_FAIL(wifi_config_set_rx_filter(promiscuous_rxfilter));
        CHECK_FAIL(wifi_config_register_rx_handler((wifi_rx_handler_t)_joylink_smnt_rx_data_callback));
        CHECK_FAIL(wifi_config_set_channel(WIFI_PORT_STA, 6));

        b_init = true;
    }
    return 0;
}

/**
 * @function _joylink_smnt_deinit
 *
 * @brief    delect the resources
 */
static void _joylink_smnt_deinit()
{
    _joylink_smnt_restore_env();

    if(h_switch_timer != NULL) {
        xTimerDelete(h_switch_timer, 0);
        h_switch_timer = NULL;
    }

    if(h_smtcn_rx_queue != NULL) {
        vQueueDelete(h_smtcn_rx_queue);
        h_smtcn_rx_queue = NULL;
    }

    b_init = false;

}

/**
 * @function _joylink_smnt_connect_to_ap
 *
 * @brief    connect to the access point
 *
 * @param    [IN]pt_result
 */
static void _joylink_smnt_connect_to_ap(joylink_innet_result_t *pt_result)
{

    LOG_FAIL(wifi_config_set_ssid(WIFI_PORT_STA, pt_result->ssid, pt_result->ssid_len));

    if (pt_result->pwd_len != 0) {
        LOG_FAIL(wifi_config_set_wpa_psk_key(WIFI_PORT_STA, pt_result->pwd, pt_result->pwd_len));
        if(pt_result->pwd_len == 10 || pt_result->pwd_len == 26 || pt_result->pwd_len == 5 || pt_result->pwd_len == 13) {
            wifi_wep_key_t wep_key = {{{0}}};

            if (pt_result->pwd_len == 10 || pt_result->pwd_len == 26) {
                wep_key.wep_key_length[0] = pt_result->pwd_len / 2;
                AtoH((char *)pt_result->pwd, (char *)&wep_key.wep_key[0], (int)wep_key.wep_key_length[0]);
            } else if (pt_result->pwd_len == 5 || pt_result->pwd_len == 13) {
                wep_key.wep_key_length[0] = pt_result->pwd_len;
                memcpy(wep_key.wep_key[0], pt_result->pwd, pt_result->pwd_len);
            }

            wep_key.wep_tx_key_index = 0;
            LOG_FAIL(wifi_config_set_wep_key(WIFI_PORT_STA, &wep_key));
        }

    }

    LOG_FAIL(wifi_config_reload_setting());
}


/**
 * @function _joylink_smnt_rcv_pkt_thread
 *
 * @brief    The entry point of the joylink smart connection task
 */
static void _joylink_smnt_rcv_pkt_thread(void *pvParameters)
{
    configASSERT(h_smtcn_rx_queue);

    smt_connt_result_t t_cb_result;

    while (e_joylink_smnt_flag != JD_FINISH && e_joylink_smnt_flag!= JD_EXIT) {
        raw_pkt_t pkt;
        if (xQueueReceive(h_smtcn_rx_queue, (void *)&pkt, portMAX_DELAY) == pdPASS) {
            /* process this raw packet */
            _joylink_smnt_raw_pkt_handler(pkt.payload, pkt.len);
        } else {
            LOG_E(jdSmtc,"error\n");
        }
    }

    if(h_switch_timer != NULL) {
        xTimerStop(h_switch_timer, 0);
    }
    xSemaphoreTake(queue_mutex, portMAX_DELAY);
    _joylink_smnt_deinit();
    xSemaphoreGive(queue_mutex);

    if(e_joylink_smnt_flag == JD_FINISH) {
        LOG_I(jdSmtc,"connect to ap, ssid is %s,pwd is %s\n",t_result.ssid,t_result.pwd);
        if(g_pf_event_cb == NULL) {
            _joylink_smnt_connect_to_ap(&t_result);
        } else {
            memset(&t_cb_result,0,sizeof(t_cb_result));
            t_cb_result.pwd_len = t_result.pwd_len;
            t_cb_result.ssid_len = t_result.ssid_len;
            memcpy(t_cb_result.pwd, t_result.pwd, t_result.pwd_len);
            if(t_result.ssid_len <=  WIFI_MAX_LENGTH_OF_SSID) {
                memcpy(t_cb_result.ssid, t_result.ssid, t_result.ssid_len);
            } else {
                memcpy(t_cb_result.ssid, t_result.ssid,WIFI_MAX_LENGTH_OF_SSID);
            }
            g_pf_event_cb(WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED, &t_cb_result);
        }
    }

    g_pf_event_cb = NULL;
    vSemaphoreDelete(queue_mutex);
    if(h_smtc_handle != NULL) {
        vTaskDelete(h_smtc_handle);
    }
}


/*************************************************************
 * extern function definition
 *************************************************************/


/**
 * @function joylink_smnt_stop
 *
 * @brief    stop the joylink smart connection task
 */
void joylink_smnt_stop()
{
    e_joylink_smnt_flag = JD_EXIT;
}


/**
 * @function joylink_smnt_start
 *
 * @brief    This function will start the joylink smart connection task
 *           When get the correct information from the app, the task will be terminated, or user can stop the
 *           task by calling #joylink_smnt_stop function
 *
 * @param    [IN]pt_info ,  if pt_info == NULL,  CID,PUID,KEY will be set to zero
 *
 * @return   0 success
 *			 -1 fail
 */
int joylink_smnt_start(const DEVICE_INFO_T *pt_info,smart_connection_callback_t pf_callback)
{
    eTaskState e_smtcn_state;

    do {
        if ( h_smtc_handle != NULL
                && (e_smtcn_state = eTaskGetState(h_smtc_handle)) != eReady
                && e_smtcn_state != eDeleted) {
            LOG_E(jdSmtc,"smtcn task state:%d, cannot create it\n", e_smtcn_state);
            return -1;
        }

        g_pf_event_cb = pf_callback;

        if(_joylink_smnt_init(pt_info) < 0) {
            LOG_E(jdSmtc,"init fail\n");
            break;
        }

        if (xTaskCreate(_joylink_smnt_rcv_pkt_thread, "jdSmtc thread", 1048, NULL, configTIMER_TASK_PRIORITY, &h_smtc_handle) != pdPASS) {
            LOG_E(jdSmtc,"create task fail\n");
            break;
        }

        if(xTimerStart(h_switch_timer, 0)!= pdPASS) {
            LOG_E(jdSmtc,"start timer fail\n");
            break;
        }
        return 0;
    } while(0);

    _joylink_smnt_deinit();
    if(h_smtc_handle != NULL) {
        vTaskDelete(h_smtc_handle);
    }
    return -1;
}

