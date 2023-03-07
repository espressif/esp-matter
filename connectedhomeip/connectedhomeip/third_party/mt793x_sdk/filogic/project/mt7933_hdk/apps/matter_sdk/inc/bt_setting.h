/* Copyright Statement:
 *
 * (C) 2020-2021  MediaTek Inc. All rights reserved.
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

#ifndef __BT_SETTING_H__
#define __BT_SETTING_H__


typedef enum
{
    BT_SETTING_ON_OFF        =    0,
    BT_SETTING_KEY_LOG_SNOOP,
#ifdef __MTK_BT_MESH_ENABLE__
    BT_SETTING_KEY_LOG_MESH_MUST,//group start, group must define together, beacause for loop need ther first one and the last one.
    BT_SETTING_KEY_LOG_MESH_BEARER,
    BT_SETTING_KEY_LOG_MESH_BEARER_GATT,
    BT_SETTING_KEY_LOG_MESH_NETWORK,
    BT_SETTING_KEY_LOG_MESH_TRANSPORT,
    BT_SETTING_KEY_LOG_MESH_ACCESSS,
    BT_SETTING_KEY_LOG_MESH_MODEL,
    BT_SETTING_KEY_LOG_MESH_PROVISION,
    BT_SETTING_KEY_LOG_MESH_BEACON,
    BT_SETTING_KEY_LOG_MESH_PROXY,
    BT_SETTING_KEY_LOG_MESH_CONFIG,
    BT_SETTING_KEY_LOG_MESH_MIDDLEWARE,
    BT_SETTING_KEY_LOG_MESH_FRIEND,
    BT_SETTING_KEY_LOG_MESH_UTILS,//group end
    BT_SETTING_KEY_LOG_MESH_FILTER,
#endif
    BT_SETTING_KEY_MAX
}bt_setting_key_t;

typedef enum
{
    BT_SETTING_VALUE_TYPE_BOOL = 0,
    BT_SETTING_VALUE_TYPE_U8,
    BT_SETTING_VALUE_TYPE_U32,
    BT_SETTING_VALUE_TYPE_STRING,
    BT_SETTING_VALUE_TYPE_MAX,
}bt_setting_value_type_t;

typedef union {
        bool     value_bool;
        uint8_t  value_u8;
        uint32_t value_u32;
        char     value_str[32];
}bt_setting_value_t;

typedef struct {
    const char* itemname;
    bt_setting_value_type_t value_type;
    bt_setting_value_t value;
}bt_setting_t;

typedef bt_status_t (*bt_setting_log_callback_p)(void);


extern bool bt_setting_onoff_set(bool enable);
extern void bt_setting_dump(void);
extern bool bt_setting_value_set(uint8_t setting_idx, bt_setting_value_t set_value);
bt_setting_value_t bt_setting_value_get(bt_setting_key_t setting_idx);
bool bt_setting_onoff_get(void);
#endif //__BT_SETTING_H__