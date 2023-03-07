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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ip4_addr.h"
#include "smt_conn.h"
#include "netif.h"
#include "dhcp.h"
#include "task_def.h"

#include "airkiss_cloudapi.h"

xTaskHandle host_thread;

#if 0
uint8_t networkconnected = 0;

void dhcpc_callback(const struct netif *inp) {
	printf("Finish DHCP!!\r\n");
    printf("  ip      %s\n", ip4addr_ntoa(&inp->ip_addr));
    printf("  netmask %s\n", ip4addr_ntoa(&inp->netmask));
    printf("  gateway %s\n", ip4addr_ntoa(&inp->gw));
	networkconnected = 1;

    // deregister callback
    wifi_register_ip_ready_callback(NULL);
}
#endif

void airkiss_wait_network_up(void)
{
    int link_status = 0;
    struct netif *sta_if = NULL;

    printf("%s : start\n", __FUNCTION__);

    //check wifi link
    while(!link_status){
        wifi_connection_get_link_status(&link_status);
        if(!link_status){
            /*wait until wifi connect*/
            printf("Waiting for STA link up...\n");
            vTaskDelay(1000);
        }
    }

    //check netif up
    while(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
        sta_if = netif_find("st2");
        if(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
            /*wait until netif up*/
            printf("Waiting for netif up...\n");
            vTaskDelay(1000);
        }
    }

    while (!(sta_if->dhcp && (sta_if->dhcp->state == DHCP_STATE_BOUND || sta_if->dhcp->state == DHCP_STATE_RENEWING))) {
        /*wait until got ip address*/
        printf("Waiting for got ip address...\n");
        vTaskDelay(1000);
    }

    printf("%s : end\n", __FUNCTION__);
}

void printfstrlen(const uint8_t *data, uint32_t len) {
	uint32_t i;
	for(i=0;i<len;i++) {
		printf("%c", data[i]);
	}
}

void ReceiveResponseCB(uint32_t taskid, uint32_t errcode, uint32_t funcid, const uint8_t* body, uint32_t bodylen) {
	printf("Enter Resp Callback:id:%d, err:%d, funcid:%d, len:%d\r\n", taskid, errcode, funcid, bodylen);
	printfstrlen(body, bodylen);
	printf("\r\n");
}

void ReceiveNotifyCB(uint32_t funcid, const uint8_t* body, uint32_t bodylen) {
	printf("Recv notify Callback funcid:%d, len:%d\r\nData:", funcid, bodylen);
	printfstrlen(body, bodylen);
	printf("\r\n");
}

void ReceiveEventCB(EventValue event_value) {
	printf("Recv Event Callback:%d\r\n", event_value);
	printf("\r\n");
	switch (event_value) {
		case EVENT_VALUE_LOGIN:
			printf("Device Login!\r\n");
			break;
		case EVENT_VALUE_LOGOUT:
			printf("Device Logout!\r\n");
			break;
	}
}


const uint8_t testdevlicence[] = "xxxxxxxxxxxxxxxxxxxxxxxxx";
const uint8_t testmsg[] = "{\"msg_id\":1000,\"msg_type\":\"set\",\"services\":{\"air_conditioner\":{\"tempe_indoor\":1.20000004768,\"tempe_outdoor\":1.10000002384,\"tempe_target\":0.10000000149,\"ac_mode\":1,\"fan_speed\":2,\"is_horiz_fan_on\":true,\"is_verti_fan_on\":true},\"operation_status\":{\"status\":0}}}";
//uint32_t heapbuf[1024*8];//8k for airkiss heap

xTaskHandle cloud_thread;
char ak_loop_run_sign = 0;
extern ak_mutex_t m_task_mutex;
extern ak_mutex_t m_malloc_mutex;//define in airkiss_porting_freertos.c

void airkiss_cloud_thread(void *args) {
    printf("%s : heap free size:%d\n", __FUNCTION__, xPortGetFreeHeapSize());

	uint32_t sleep_time;
	while(ak_loop_run_sign == 0) {
		vTaskDelay(2000);
	}
	printf("Everything is ready!!\r\n");
	for (;;) {
		sleep_time = airkiss_cloud_loop();
        printf("airkiss_cloud_loop: sleep time:%d\r\n", sleep_time);
		vTaskDelay(sleep_time);
	}
}

void task_execute_sdk_loop() {
 	uint32_t taskid = 0;
    void *heapbuf = NULL;

#if 0
    // start smart connect
    mtk_smart_connect();
    wifi_register_ip_ready_callback(dhcpc_callback);

	while(0 == networkconnected) {
    	printf("Wait network to connect---------------\n");
        vTaskDelay(1000);
	}
#endif
    // wait netowrk ready
    airkiss_wait_network_up();

    heapbuf = pvPortMalloc(10*1024); //10k for airkiss heap
    if(NULL == heapbuf) {
        printf("malloc() Error!");
        return;
    }

	while (1) {
        int32_t ret;
        ret = airkiss_cloud_init((uint8_t *)testdevlicence, (uint32_t)strlen((const char *)testdevlicence), &m_task_mutex, &m_malloc_mutex, heapbuf, sizeof(heapbuf));
        if (0 == ret)
            break;
        else {
		    printf("SDK init failed!!!, error code:%d\r\n", ret);
            vTaskDelay(1000);
        }
	}

	//regist callback functions
	airkiss_callbacks_t cbs;
	cbs.m_notifycb = ReceiveNotifyCB;
	cbs.m_respcb = ReceiveResponseCB;
	cbs.m_eventcb = ReceiveEventCB;
	airkiss_regist_callbacks(&cbs);

	//ready to run airkiss_cloud_loop
	ak_loop_run_sign = 1;
	for (;;) {
		printf("App thread sleep!!\r\n");
		vTaskDelay(15000);
		taskid = airkiss_cloud_sendmessage(1, (uint8_t *)testmsg, strlen((const char *)testmsg));
		printf("Wake up from app thread and start a task:%d!\r\n", taskid);
	}
}


//#define APP_BUILD_VERSION  "(" __DATE__ " " __TIME__ ")"

void
airkiss_cloud_entry(void *args)
{
    printf("%s : heap free size:%d\n", __FUNCTION__, xPortGetFreeHeapSize());

    printf("enter cloud thread app---------------\n");
//    printf("enter cloud thread---------------%s\n", APP_BUILD_VERSION);
    printf("Airkiss lib version:%s\n", airkiss_cloud_version());
    task_execute_sdk_loop();
    /* Never returns */
}


void user_main(void)
{
    // TODO: Canot test by error license.
    return;

    xTaskCreate(
        airkiss_cloud_entry,
        AIRKISS_HOST_TASK_NAME,
        AIRKISS_HOST_TASK_STACKSIZE/sizeof(portSTACK_TYPE),
        NULL,
        AIRKISS_HOST_TASK_PRIO,
        &host_thread);

    xTaskCreate(
        airkiss_cloud_thread,
        AIRKISS_CLOUD_TASK_NAME,
        AIRKISS_CLOUD_TASK_STACKSIZE/sizeof(portSTACK_TYPE),
        NULL,
        AIRKISS_CLOUD_TASK_PRIO,
        &cloud_thread);
}


