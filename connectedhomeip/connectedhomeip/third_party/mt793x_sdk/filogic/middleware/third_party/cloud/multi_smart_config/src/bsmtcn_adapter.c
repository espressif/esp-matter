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

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
#include "hal_aes.h"
#include "wifi_api.h"
#include "smt_conn.h"
#include "smt_core.h"
#include "semphr.h"
#include "syslog.h"
#include "bsmtcn_adapter.h"
#include "bsmtcn_block_check.h"
#include <stdio.h>

#include "msc_internal.h"

static TimerHandle_t bsmtcn_rst_timer = NULL;
#define M80211_HEADER_LEN_MIN 24
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_PRINT(mac) mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]
#define IS_MULTICAST_ADDR(Addr) ((((Addr[0]) & 0x01) == 0x01) && ((Addr[0]) != 0xff))
#define IS_BROADCAST_ADDR(Addr) ((Addr[0] == 0xff) && (Addr[1] == 0xff) && (Addr[2] == 0xff) && (Addr[3] == 0xff) && (Addr[4] == 0xff) && (Addr[5] == 0xff))


#define ESUCCESS    0x0000
#define ESTATE      0x3000
#define ELOCK       0x3001
#define EMAC        0x3002
#define EPARAM      0x3003
#define ETIMEOUTRST 0x3004
#define ELENGTH     0x3005
#define ENOTREADY   0x3006
#define ENOSUCHTYPE 0x3007
#define EDROPPED    0x3008
#define EIDXOOR     0x3009

static multi_config_evt_cb evt_cb = NULL;

static sub_proto_sm_t bsmnt_status = SUB_INIT;

static void bsmtcn_get_time(uint32_t *ptime)
{
    *ptime = xTaskGetTickCount();
}

static void aes128_decrypt(uint8_t *p, uint32_t len, uint8_t *key)
{
    //uint8_t *pCipter, Plain[256] = {0};
    uint8_t pCipter[256] = {0};
    uint8_t Plain[256] = {0};
    int32_t i, encrypt_data_len;
    uint32_t iCipterLen = AES_BLK_SIZE;

    //pCipter = (uint8_t *)p;
    memcpy(pCipter, p, len);

    encrypt_data_len = (len + AES_BLK_SIZE - 1) & (~(AES_BLK_SIZE - 1));
    for (i = 0; i < encrypt_data_len / AES_BLK_SIZE; i++) {
        msc_aes_decrypt(pCipter + i * AES_BLK_SIZE,
                                       AES_BLK_SIZE,
                                       (uint8_t *)key,
                                       AES_BLK_SIZE,
                                       Plain + i * AES_BLK_SIZE,
                                       &iCipterLen);
    }

    memcpy(p, Plain, encrypt_data_len);

    return;
}
extern multi_smtcn_info_t   saved_multi_smtcn_info;

static void bsmtcn_get_info(uint8_t *payload, uint16_t len)
{
	struct bsmtcn_payload_struct *p = (struct bsmtcn_payload_struct *)payload;
	BSMTCN_ADAPTER_T *bsmtcn_adapter = bsmtcn_get_adapter();
	bool crc_result;
    bool success = false;
    do{
        
#if ENCRYTION
    	uint16_t aes_len = (len-6) - (len-6)%AES_BLK_SIZE;

    	crc_result = bsmtcn_payload_crc_check(payload, len);
    	if(crc_result == false) {
    	   printf("payload crc error\r\n");
    	   bsmtcn_adapter_reset();
    	   return;
    	}
        aes128_decrypt(&payload[6], aes_len, bsmtcn_adapter->key);
#endif
        if(p->total_length < (p->mix_len.ssid_len + p->mix_len.password_len + p->mix_len.extend_data_len)) {
            printf("get infor failed\n");
            break;
        }
        
    	saved_multi_smtcn_info.ssid_len = p->mix_len.ssid_len;
    	saved_multi_smtcn_info.pwd_len = p->mix_len.password_len;
    	saved_multi_smtcn_info.tlv_data_len = p->mix_len.extend_data_len;

        if(saved_multi_smtcn_info.ssid_len > WIFI_MAX_LENGTH_OF_SSID) {
    	   printf("ssid_len error\r\n");
           break;
        }

    	if(saved_multi_smtcn_info.pwd_len > WIFI_LENGTH_PASSPHRASE) {
            printf("pwd_len error\r\n");
            break;
    	}

    	if(saved_multi_smtcn_info.tlv_data_len > BSMTCN_MAX_CUSTOM_LEN) {
            printf("custom_len error\r\n");
            break;
    	}

        success = true;
    }while(0);
    if(!success)
    {
        saved_multi_smtcn_info.tlv_data_len = 0;
        saved_multi_smtcn_info.pwd_len = 0;
        saved_multi_smtcn_info.ssid_len = 0;
    }
    else
    {
		memcpy(saved_multi_smtcn_info.ssid, &payload[9], saved_multi_smtcn_info.ssid_len);
		printf("ssid: %s\r\n", saved_multi_smtcn_info.ssid);
		bsmtcn_hex_dump("ssid hex \r\n", saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len);

		memcpy(saved_multi_smtcn_info.pwd, &payload[9] + saved_multi_smtcn_info.ssid_len, saved_multi_smtcn_info.pwd_len);
		printf("pwd: %s\r\n", saved_multi_smtcn_info.pwd);
		bsmtcn_hex_dump("pwd hex \r\n", saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);

		memcpy(saved_multi_smtcn_info.tlv_data, &payload[9] + saved_multi_smtcn_info.ssid_len + saved_multi_smtcn_info.pwd_len, saved_multi_smtcn_info.tlv_data_len);
		printf("custom: %s\r\n", saved_multi_smtcn_info.tlv_data);
		bsmtcn_hex_dump("custom hex \r\n", saved_multi_smtcn_info.tlv_data, saved_multi_smtcn_info.tlv_data_len);
    }

}


static void mcsmt_report_env(enum eevent_id evt) 
{
    switch (evt)
    {
        case EVT_ID_SYNCSUC:
            msc_stop_switch_channel();
            bsmnt_status = SUB_SYNC_SUCC;
            if(evt_cb != NULL)
                evt_cb(EVT_BC_SYNC);
            break;
        case EVT_ID_INFOGET:
        {
            bsmnt_status = SUB_FIN;
            BSMTCN_ADAPTER_T *bsmtcn_adapter = bsmtcn_get_adapter();
            bsmtcn_get_info(bsmtcn_adapter->bsmtcn_payload, bsmtcn_adapter->block_count * BLOCK_DATA_COUNT);
            msc_write_flag(SMTCN_FLAG_FIN);
            if(evt_cb != NULL)
                evt_cb(EVT_FINISHED);
            break;
        }
        case EVT_ID_TIMEOUT:
            bsmnt_status = SUB_INIT;
            if(evt_cb != NULL)
                evt_cb(EVT_SYNC_TIME_OUT);
            msc_continue_switch_channel();
            break;
        case EVT_ID_SYNFAIL:
        default :
            break;
    }
}


static struct efunc_table bcfunc_tbl = {
    .report_evt     = mcsmt_report_env,
    .start_timer    = NULL,
    .stop_timer     = NULL,
    .aes128_decrypt = msc_aes_decrypt,
    .os_get_time = bsmtcn_get_time,
};



static int32_t bsmtcn_init(const uint8_t *key, const uint8_t key_length)
{
	if(bsmtcn_adapter_init() < 0)
        return -1;

	bsmtcn_adapter_set_key((uint8_t *)key, key_length);

    bsmtcn_register_evt_func_table(&bcfunc_tbl);

    bsmnt_status = SUB_INIT;

    return 0;
}

static void bsmtcn_cleanup(void)
{
	bsmtcn_adapter_deinit();
}

static int32_t bsmtcn_channel_reset(void)
{

	//printf("bsmtcn_channel_reset\r\n");
	bsmtcn_adapter_channel_reset();
    bsmnt_status = SUB_INIT;

	return 0;
}

static int32_t bsmtcn_input(char *p, int32_t len)
{
	char *s_addr,*mac1,*mac3;

    int32_t ret = EDROPPED;

	uint8_t from_ds_flag = 0;

    /*
     *TO DS:    | FC(2B) | DI(2B) |   BSSID   |    SA     |     DA    | ... |
     *FROM DS:  | FC(2B) | DI(2B) |    DA     |   BSSID   |     SA    | ... |
    */
    
    /*we drop the data if it's NOT multicast data or NOT data frame. */

    if ((p[1] & 3) == 2) {
        /* from DS. */
        s_addr = p + 16;
        from_ds_flag = 1;
    } else if ((p[1] & 3) == 1) {
        /* To DS. */
        s_addr = p + 10;
		from_ds_flag = 0;
    } else {
        //printf("[MSC] SC IBSS/WDS data not support. %x\n", p[1]);
        return -EPARAM;
    }

	mac1 = p + 4;
	mac3 = p + 16;
    
    if (p == NULL || len < M80211_HEADER_LEN_MIN ||
            (!IS_BROADCAST_ADDR((p + 4)) && !IS_BROADCAST_ADDR((p + 16))) ||
            (p[0] & 0x0C) != 0x08) {
        return -EPARAM;
    }

    if(smtcn_is_debug_on()) {
        printf("Source MAC:"MAC_FMT"\r\n", MAC_PRINT(s_addr));
		printf("MAC1:"MAC_FMT"\r\n", MAC_PRINT(mac1));
		printf("MAC3:"MAC_FMT"\r\n", MAC_PRINT(mac3));
    }

	bsmtcn_packet_decoder((uint8_t *)s_addr, len, from_ds_flag);

    return ret;
}

static sub_proto_sm_t bsmnt_sub_proto_rcv(char *data, int len)
{
    bsmtcn_input(data,len);
    return bsmnt_status;
}

static void bsmtcn_sub_proto_rx_timeout(void)
{
    bsmtcn_adapter_reset();
    bsmnt_status = SUB_INIT;
}

msc_sub_proto_ops bsmtcn_sub_proto = {
    .sub_proto_init         =   &bsmtcn_init,
    .sub_proto_cleanup      =   &bsmtcn_cleanup,
    .sub_proto_rst_channel  =   &bsmtcn_channel_reset,
    .sub_proto_rcv          =   &bsmnt_sub_proto_rcv,
    .sub_proto_rx_timeout   =   &bsmtcn_sub_proto_rx_timeout,
};


