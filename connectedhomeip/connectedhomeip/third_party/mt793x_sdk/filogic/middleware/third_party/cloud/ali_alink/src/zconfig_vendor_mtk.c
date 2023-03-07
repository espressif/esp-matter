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
#include "smt_conn.h"
//#include "semphr.h"
#include "syslog.h"
#include "zconfig_lib.h"

#define ZC_MODEL "ALINKTEST_LIVING_LIGHT_SMARTLED_LUA"
#define ZC_TPSK "Pu99UdYLaxIzIdcF6kE69IY7KamoBFef2lVSq5wkGBc="

#define zc_debug_log 1

#if (zc_debug_log == 1)
#define zc_dbg printf
#else
#define zc_dbg(...)
#endif

typedef enum {
    ZCONFIG_INIT,
    ZCONFIG_SCANNING,
    ZCONFIG_CHN_LOCKED,
    ZCONFIG_DONE
}zc_status_t;

static TimerHandle_t zc_lock_timer = NULL;
static zc_status_t zconfig_status = ZCONFIG_INIT;
char* dev_tpsk = NULL;

log_create_module(zconfig, PRINT_LEVEL_INFO);

extern void * os_memcpy(void *dest, const void *src, size_t n);
extern void * os_memset(void *s, int c, size_t n);
extern int os_memcmp(const void *s1, const void *s2, size_t n);
extern void * os_malloc(size_t size);
extern void os_free(void *ptr);

extern smtcn_info   saved_smtcn_info;

unsigned int mtk_zconfig_get_time(void)
{
    struct timeval tv = { 0 };
    unsigned int time_ms;

    gettimeofday(&tv, NULL);
    time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return time_ms;
}

char *mtk_zconfig_get_tpsk(void)
{
    return (char *)dev_tpsk;
}

void mtk_zconfig_msleep(int ms)
{
    TickType_t xDelay;

    xDelay = ms / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
}

void mtk_zconfig_channel_locked(u8 channel)
{
    //LOG_I(zconfig, "zconfig channel locked [%d].\n", channel);
    smtcn_stop_switch();
    xTimerStart(zc_lock_timer, tmr_nodelay);
    zconfig_status = ZCONFIG_CHN_LOCKED;
}

void mtk_zconfig_printf(int log_level, const char* log_tag, const char* file,
            const char* fun, int line, const char* fmt, ...)
{
    va_list ap;
    char msg[256];

    if (log_level > LOGLEVEL_INFO)
        return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    printf("%s", msg);
}

void mtk_zconfig_got_ssid_passwd(u8 *ssid, u8 *passwd, u8 *bssid, u8 auth, u8 encry, u8 channel)
{
    xTimerStop(zc_lock_timer, tmr_nodelay);
    zconfig_status = ZCONFIG_DONE;

    /*SSID*/
    saved_smtcn_info.ssid_len = os_strlen(ssid);
    if(saved_smtcn_info.ssid_len > WIFI_MAX_LENGTH_OF_SSID)
        saved_smtcn_info.ssid_len = WIFI_MAX_LENGTH_OF_SSID;
    os_memcpy(saved_smtcn_info.ssid, ssid, saved_smtcn_info.ssid_len);

    /*password*/
    saved_smtcn_info.pwd_len = os_strlen(passwd);
    if(saved_smtcn_info.pwd_len > WIFI_LENGTH_PASSPHRASE)
        saved_smtcn_info.pwd_len = WIFI_LENGTH_PASSPHRASE;
    os_memcpy(saved_smtcn_info.pwd, passwd, saved_smtcn_info.pwd_len);

    /* authmode and encrypt type not used. */
    /* channel not used. */
    saved_smtcn_info.channel = channel;

#if (zc_debug_log == 1)
    if (bssid) {
        LOG_I(zconfig, "ssid: %s, passwd:%s, bssid:%02x%02x%02x%02x%02x%02x, %s, %s, %d\r\n",
				ssid, passwd,
				bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
				zconfig_auth_str(auth), zconfig_encry_str(encry), channel);
	} else {
		LOG_I(zconfig, "ssid: %s, passwd:%s, bssid:--, %s, %s, %d\r\n",
				ssid, passwd,
				zconfig_auth_str(auth), zconfig_encry_str(encry), channel);
	}
#endif

    atomic_write_smtcn_flag(SMTCN_FLAG_FIN);
    return 0;
}

struct zconfig_constructor zconfig_cb = {
    .zc_get_time            =   &mtk_zconfig_get_time,   // get system time, in ms.
    .zc_malloc              =   &os_malloc,
    .zc_free                =   &os_free,
    .zc_get_tpsk            =   &mtk_zconfig_get_tpsk,   //tpsk, released by alibaba
    .zc_printf              =   &mtk_zconfig_printf,
    .zc_cb_channel_locked   =   &mtk_zconfig_channel_locked,
    .zc_cb_got_ssid_passwd  =   &mtk_zconfig_got_ssid_passwd,
};

static void zc_lock_timeout( TimerHandle_t tmr )
{
    LOG_I(zconfig, "zconfig lock channel timeout.\n");
    zconfig_status = ZCONFIG_SCANNING;
    smtcn_continue_switch();
}

static int zc_init(const uint8_t *key, const uint8_t key_length)
{
    dev_tpsk = os_malloc(TPSK_LEN + 1);
    if(dev_tpsk == NULL){
        LOG_E(zconfig, "zconfig malloc tpsk failed.\n");
        return -1;
    }
    os_memset(dev_tpsk, 0, TPSK_LEN + 1);
    os_strncpy((char *)dev_tpsk, ZC_TPSK, TPSK_LEN);    // it should be replaced by tpsk get-function

    zc_lock_timer = xTimerCreate( "zc_lock_timer",
                                    (locked_channel_timems/portTICK_PERIOD_MS), /*the period being used.*/
                                    pdFALSE,
                                    NULL,
                                    zc_lock_timeout);
    if (zc_lock_timer == NULL) {
        LOG_E(zconfig, "zc_lock_timer create fail.\n");
        return -1;
    }

    zconfig_status = ZCONFIG_SCANNING;
    zconfig_init(&zconfig_cb); //init this func as soon as possible

    zc_dbg("zconfig_init success\n");

    return 0;
}

static void zc_cleanup(void)
{
    if(dev_tpsk != NULL){
        os_free(dev_tpsk);
        dev_tpsk = NULL;
    }

    if(zc_lock_timer != NULL){
        xTimerDelete(zc_lock_timer, tmr_nodelay);
        zc_lock_timer = NULL;
    }

    zconfig_destroy();
}

static int  zc_channel_rst(void)
{
    //zc_dbg("switch channel:[%d]\n", smtcn_current_channel());
    zconfig_status = ZCONFIG_SCANNING;
    return 0;
}

static int  zc_rx_handler(char *payload, int len)
{
    return zconfig_recv_callback(payload, len, smtcn_current_channel());
}

smtcn_proto_ops zc_proto_ops = {
    .init               =   &zc_init,
    .cleanup            =   &zc_cleanup,
    .switch_channel_rst =   &zc_channel_rst,
    .rx_handler         =   &zc_rx_handler,
};
