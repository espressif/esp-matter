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
#include "smt_conn.h"
#include "airkiss.h"
#include "semphr.h"
#include "syslog.h"

log_create_module(airkiss, PRINT_LEVEL_INFO);

#define ak_debug_log 1

typedef enum AK_STATUS{
    AK_RCV_PRE,
    AK_RCV_DATA,
    AK_FIN,
}ak_status_t;

static ak_status_t ak_status;
static airkiss_context_t *ak_context = NULL;
static TimerHandle_t ak_lock_timer = NULL;

extern smtcn_info   saved_smtcn_info;
extern void atomic_write_smtcn_flag(uint8_t flag_value);

static airkiss_config_t ak_conf = {
    .memset = memset,
    .memcpy = memcpy,
    .memcmp = memcmp,
    .printf = NULL,
};

static void ak_lock_timeout( TimerHandle_t tmr )
{
    LOG_I(airkiss, "<INFO> Airkiss lock channel timeout.</INFO>\n");
    smtcn_continue_switch();
}

static int ak_init(const uint8_t *key, const uint8_t key_length)
{
    int ret = 0;

    ak_context = (airkiss_context_t *) pvPortMalloc(sizeof(airkiss_context_t));
    if(ak_context == NULL){
        LOG_E(airkiss, "<ERR>Airkiss malloc ak_context failed!</ERR>\n");
        return -1;
    }

    ret = airkiss_init(ak_context, &ak_conf);
    if(ret < 0){
        LOG_E(airkiss, "<ERR>Airkiss init failed!</ERR>\n");
        return -1;
    }

    ak_lock_timer = xTimerCreate( "ak_lock_timer",
                                    (locked_channel_timems/portTICK_PERIOD_MS), /*the period being used.*/
                                    pdFALSE,
                                    NULL,
                                    ak_lock_timeout);
    if (ak_lock_timer == NULL) {
        LOG_E(airkiss, "<ERR>ak_lock_timer create fail.</ERR>\n");
        return -1;
    }

#if AIRKISS_ENABLE_CRYPT
    const char* key = "1234567890123456";
    airkiss_set_key(ak_context, key, strlen(key));
#endif

    ak_status = AK_RCV_PRE;
    return ret;
}

static void ak_cleanup(void)
{
    if (ak_lock_timer != NULL) {
        xTimerDelete(ak_lock_timer, tmr_nodelay);
        ak_lock_timer = NULL;
    }

    if(ak_context != NULL){
        vPortFree(ak_context);
        ak_context = NULL;
    }
}

static int ak_channel_rst(void)
{
    airkiss_init(ak_context, &ak_conf);

    ak_status = AK_RCV_PRE;
    airkiss_change_channel(ak_context);

    return 0;
}

static int ak_get_info(void)
{
    airkiss_result_t result;

    xTimerStop(ak_lock_timer, tmr_nodelay);
    if(airkiss_get_result(ak_context, &result) < 0){
        LOG_E(airkiss, "<ERR>Airkiss get result failed.</ERR>\n");
        atomic_write_smtcn_flag(SMTCN_FLAG_FAIL);
        return -1;
    }

    /*SSID*/
    saved_smtcn_info.ssid_len = result.ssid_length;
    if(saved_smtcn_info.ssid_len > WIFI_MAX_LENGTH_OF_SSID)
        saved_smtcn_info.ssid_len = WIFI_MAX_LENGTH_OF_SSID;
    memcpy(saved_smtcn_info.ssid, result.ssid, saved_smtcn_info.ssid_len);

    /*password*/
    saved_smtcn_info.pwd_len = result.pwd_length;
    if(saved_smtcn_info.pwd_len > WIFI_LENGTH_PASSPHRASE)
        saved_smtcn_info.pwd_len = WIFI_LENGTH_PASSPHRASE;
    memcpy(saved_smtcn_info.pwd, result.pwd, saved_smtcn_info.pwd_len);

#if (ak_debug_log == 1)
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID+1] = {0};
    uint8_t passwd[WIFI_LENGTH_PASSPHRASE+1] = {0};

    memcpy(ssid, saved_smtcn_info.ssid, saved_smtcn_info.ssid_len);
    memcpy(passwd, saved_smtcn_info.pwd, saved_smtcn_info.pwd_len);

    LOG_I(airkiss, "ssid:%s/%d, passwd:%s/%d\n",
            ssid, saved_smtcn_info.ssid_len,
            passwd, saved_smtcn_info.pwd_len );
#endif

    atomic_write_smtcn_flag(SMTCN_FLAG_FIN);
    return 0;
}

static int ak_rx_handler(char* phdr, int len)
{
    //printf_high("enter ak_rx_handler\n");
    int ret;
    ret = airkiss_recv(ak_context, phdr, len);
    //ak_dbg("<INFO>airkiss_recv result is:%d</INFO>\n", ret);

    switch(ak_status){
    case AK_RCV_PRE:
        if(ret == AIRKISS_STATUS_CHANNEL_LOCKED){
            LOG_I(airkiss, "<INFO>Airkiss channel locked</INFO>\n");
            smtcn_stop_switch();
            xTimerStart(ak_lock_timer, tmr_nodelay);
            ak_status = AK_RCV_DATA;
        }
        break;

    case AK_RCV_DATA:
        if(ret == AIRKISS_STATUS_COMPLETE){
		    LOG_I(airkiss, "<INFO>Airkiss Finished</INFO>\n");
            ak_status = AK_FIN;
		    ak_get_info();
	    }
        break;

    case AK_FIN:
        break;
    }

    return ret;
}

smtcn_proto_ops ak_proto_ops = {
    .init               =   &ak_init,
    .cleanup            =   &ak_cleanup,
    .switch_channel_rst =   &ak_channel_rst,
    .rx_handler         =   &ak_rx_handler,
};
