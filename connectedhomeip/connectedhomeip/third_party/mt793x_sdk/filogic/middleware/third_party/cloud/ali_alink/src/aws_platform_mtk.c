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
#include <time.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
//#include "smt_conn.h"
#include "semphr.h"
#include "syslog.h"
#include "wifi_api.h"
#include "wifi_rx_desc.h"

#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"

#include "aws_platform.h"
#include "aws_lib.h"
#include "alink_export.h"
#include "platform_porting.h"
#include "task_def.h"

#include "msc_internal.h"
#include "msc_api.h"

#define promiscuous_rxfilter (0x1002)
static uint32_t saved_rxfilter = 0;
static uint8_t saved_bw = 0;
static smart_connection_callback_t g_pf_event_cb;

#define dev_id		0

//ams timeout
int aws_timeout_period_ms = 1 * 60 * 1000;

//time stayed in each channel
int aws_chn_scanning_period_ms = 200;

//relative time
unsigned int vendor_get_time_ms(void)
{
    struct timeval tv = { 0 };
    unsigned int time_ms;

    gettimeofday(&tv, NULL);
    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return time_ms;
}

void *vendor_malloc(int size)
{
    return alink_malloc_func(size);
}

void vendor_free(void *ptr)
{
    if (ptr) {
        alink_free_func(ptr);
        ptr = NULL;
    }
}

//Not need implemented
void vendor_printf(int log_level, const char *log_tag, const char *file,
                   const char *fun, int line, const char *fmt, ...)
{
#if 1
    return;
#else
    va_list ap;
    char msg[256];

    /*
        if (log_level > LOGLEVEL_INFO)
            return;
    */

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    printf("%s", msg);
#endif
}

//aws sleep in ms
void vendor_msleep(int ms)
{
    vTaskDelay((TickType_t)(ms / portTICK_PERIOD_MS));
}

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
    aws_80211_frame_handler((char *)pucMacHeader, ucMacHeaderLength + u2PayloadLength, AWS_LINK_TYPE_NONE, 0);

    return; /* handled */
}

int vendor_recv_80211_frame(void)
{
    vendor_msleep(100);
    return 0;
}

//enter monitor mode
void vendor_monitor_open(void)
{
    wifi_config_set_opmode(WIFI_MODE_STA_ONLY);

    wifi_config_get_bandwidth(WIFI_PORT_STA, &saved_bw);
    wifi_config_set_bandwidth(WIFI_PORT_STA, WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ);

    wifi_config_get_rx_filter(&saved_rxfilter);
    wifi_config_set_rx_filter(promiscuous_rxfilter);

    wifi_config_register_rx_handler((wifi_rx_handler_t)vendor_data_callback);
    wifi_config_set_channel(WIFI_PORT_STA, 6);
}

//exit monitor mode
void vendor_monitor_close(void)
{
    wifi_config_set_rx_filter(saved_rxfilter);
    wifi_config_unregister_rx_handler();
}

//wifi switch channel 1~13
void vendor_channel_switch(char primary_channel,
                           char secondary_channel, char bssid[6])
{
    wifi_config_set_channel(WIFI_PORT_STA, primary_channel);
}

int vendor_broadcast_notification(char *msg, int msg_num)
{
    struct sockaddr_in sock_in;
    int fd, ret, socklen, i;
    //int yes = 1;

    int buf_len = 512;
    char *buf = vendor_malloc(buf_len);

    socklen = sizeof(struct sockaddr_in);

    memset(&sock_in, 0, sizeof(sock_in));
    sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_in.sin_port = htons(UDP_RX_PORT);
    sock_in.sin_family = PF_INET;

    fd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    //bug_on(fd < 0);

    ret = lwip_bind(fd, (struct sockaddr *)&sock_in, sizeof(sock_in));
    //bug_on(ret);
#if 0
    ret = qcom_setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(int) );
    bug_on(ret);
#endif

    /*
     * -1 = 255.255.255.255 this is a BROADCAST address,
     * a local broadcast address could also be used.
     * you can comput the local broadcat using NIC address and its NETMASK
     */
    sock_in.sin_addr.s_addr = htonl((uint32_t)-1); /* send message to 255.255.255.255 */
    sock_in.sin_port = htons(UDP_TX_PORT); /* port number */
    sock_in.sin_family = PF_INET;
    //send notification
    for (i = 0; i < msg_num; i++) {
        ret = lwip_sendto(fd, msg, strlen(msg), 0,
                          (struct sockaddr *)&sock_in, socklen);
        if (ret < 0) {
            printf("send msg error!!!!\n");
            alink_sleep(1000); // 1s
            continue;
        } else {
            printf("broadcast: %s\n", msg);
        }

        do {
            struct timeval tv;
            fd_set rfds;

            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            tv.tv_sec = 0;
            tv.tv_usec = 1000 * (200 + i * 100); //from 200ms to N * 100ms

            ret = lwip_select(fd + 1, &rfds, NULL, NULL, &tv);
            if (ret > 0) {
                ret = lwip_recv(fd, buf, buf_len, 0);
                if (ret) {
                    buf[ret] = '\0';
                    printf("rx: %s\n", buf);
                    goto out;
                }
            }
        } while (0);
    }

out:
    lwip_close(fd);
    vendor_free(buf);

    ret = 0;

    return ret;
}

//////////////////////////////////////////////////////////////////////////////
//sample code

//TODO: update these product info
//#define product_model		"ALINKTEST_LIVING_LIGHT_SMARTLED"
//#define product_secret		"YJJZjytOCXDhtQqip4EjWbhR95zTgI92RVjzjyZF"
//#define device_mac		"00:11:22:33:44:54"
//#define device_sn		"0001020304050607"

#define wait_wifi_timeout_ms     (1*60*1000)  // 60s 
SemaphoreHandle_t wait_wifi_ready_sem = NULL;

char *vendor_get_model(void)
{
    return DEV_MODEL;
}
char *vendor_get_secret(void)
{
    return ALINK_SECRET;
}
char *vendor_get_mac(void)
{
    return alink_get_device_mac();
}
char *vendor_get_sn(void)
{
    return DEV_SN;
}
int vendor_alink_version(void)
{
    return 11;    // alink 1.1
}

void aws_wifi_ready_callback(const struct netif *netif)
{
    if(wait_wifi_ready_sem != NULL) {
        xSemaphoreGive(wait_wifi_ready_sem);
    }

    wifi_connection_unregister_event_notifier(WIFI_EVENT_IOT_PORT_SECURE, (wifi_event_handler_t)aws_wifi_ready_callback);
}

int wait_for_wifi_ready(int wait_ms)
{
    int ret;
    int wait_ticks = wait_ms / portTICK_PERIOD_MS;

    if((wait_wifi_ready_sem = xSemaphoreCreateBinary()) == NULL) {
        return -1;
    }

    wifi_connection_register_event_notifier(WIFI_EVENT_IOT_PORT_SECURE, (wifi_event_handler_t)aws_wifi_ready_callback);
    if(xSemaphoreTake(wait_wifi_ready_sem, wait_ticks) != pdTRUE) {
        ret = -1;
    } else {
        ret = 0;
    }

    if(wait_wifi_ready_sem != NULL) {
        vSemaphoreDelete(wait_wifi_ready_sem);
        wait_wifi_ready_sem = NULL;
    }

    return ret;
}

extern struct netif sta_if;
extern int is_alink_network_up;
extern int aws_config_softap(char *ssid_buf, char *passwd_buf);

int aws_run(void)
{
    char ssid[32 + 1] = {0};
    char passwd[64 + 1] = {0};
    char bssid[6];
    char auth;
    char encry;
    char channel;
    smt_connt_result_t t_cb_result;

    int ret;

    printf("aws start\n");
    aws_start(vendor_get_model(), vendor_get_secret(), vendor_get_mac(), vendor_get_sn());

    if(aws_get_ssid_passwd(&ssid[0], &passwd[0], &bssid[0], &auth, &encry, &channel) == 0) {
        printf("alink wireless setup timeout!\n");
        if(aws_config_softap(ssid, passwd) < 0) {
            printf("aws softap timeout!\n");
            ret = -1;
            goto out;
        }
    }

    printf("ssid:%s, passwd:%s\n", ssid, passwd);

    if(g_pf_event_cb == NULL) {
        if(msc_scan_connect_ap((unsigned char *)ssid, (unsigned char *)passwd) < 0) {
            printf("%s connect failed\n", ssid);
            ret = -1;
            goto out;
        }
    } else {
        memset(&t_cb_result,0,sizeof(t_cb_result));
        t_cb_result.pwd_len = strlen(passwd);
        t_cb_result.ssid_len = strlen(ssid);
        memcpy(t_cb_result.pwd, passwd, t_cb_result.pwd_len);
        memcpy(t_cb_result.ssid, ssid,t_cb_result.ssid_len);    
        g_pf_event_cb(WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED, &t_cb_result);
    }

    if(wait_for_wifi_ready(wait_wifi_timeout_ms) < 0) {
        printf("wifi connect timeout\n");
        ret = -1;
        goto out;
    }

    msc_save_info();

    printf("wifi connected\n");
    // printf("should notify app now\n");
    // aws_notify_app();

    ret = 0;
out:
    aws_destroy();

    return ret;
}



#if defined(MTK_AWS_ENABLE)
void aws_test_thread( void *pvParameters )
{
    aws_run();
    vTaskDelete(NULL);
}

void aws_smnt_start(smart_connection_callback_t pf_cb)
{
    static TaskHandle_t aws_test_handle = NULL;
    g_pf_event_cb = pf_cb;
    eTaskState aws_test_state;
    /* smart_config_thread has been protected by smart_config_mutex.
     * Here is just to avoid create too many pending tasks which resume
     * much memory.
     */
    if((aws_test_handle != NULL) &&
            ((aws_test_state=eTaskGetState(aws_test_handle)) != eReady) &&
            (aws_test_state != eDeleted)) {
        printf("aws task state:%d, cannot create it\n", aws_test_state);
        return;
    }

    if(xTaskCreate(aws_test_thread, ALINK_AWS_TASK_NAME, ALINK_AWS_TASK_STACKSIZE/sizeof(portSTACK_TYPE), NULL, ALINK_AWS_TASK_PRIO, &aws_test_handle) != pdPASS) {
        printf("aws_thread create failed\n");
        return;
    }
}

void aws_test_start(void)
{
    static TaskHandle_t aws_test_handle = NULL;

    eTaskState aws_test_state;
    /* smart_config_thread has been protected by smart_config_mutex.
     * Here is just to avoid create too many pending tasks which resume
     * much memory.
     */
    if((aws_test_handle != NULL) &&
            ((aws_test_state=eTaskGetState(aws_test_handle)) != eReady) &&
            (aws_test_state != eDeleted)) {
        printf("aws task state:%d, cannot create it\n", aws_test_state);
        return;
    }

    if(xTaskCreate(aws_test_thread, ALINK_AWS_TASK_NAME, ALINK_AWS_TASK_STACKSIZE/sizeof(portSTACK_TYPE), NULL, ALINK_AWS_TASK_PRIO, &aws_test_handle) != pdPASS) {
        printf("aws_test_thread create failed\n");
        return;
    }
}

#endif


#if defined(MTK_AWS_ENABLE)
uint8_t _aws_test(uint8_t len, char *param[])
{
    if (len == 1) {
        if (!os_strcmp(param[0], "start")){
            aws_test_start();
        }/* else if(!os_strcmp(param[0], "stop")){
            mtk_smart_stop();
        } */else{
            printf("Not supported cmd\n");
            return 1;
        }
    } else {
        printf("Not supported cmd\n");
        return 1;
    }
    return 0;
}
#endif

