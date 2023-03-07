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

#pragma once

#undef MTK_GATT_JITTER_TEST


#include "bt_hci.h"
#include "bt_system.h"
#include "bt_gap_le.h"
#include "bt_gatts.h"
#include "gatt_service.h"
#include "connection_info.h"
#include "bt_gap.h"
#include "bt_init.h"

/* Colors */
#define BT_COLOR_SET(color)
#define BT_COLOR_GREEN
#define BT_COLOR_RED
#define BT_COLOR_BLUE
#define BT_COLOR_WHITE

#define UT_APP_CONN_MAX                            10
#define UT_APP_CMP(_cmd)                           (strncmp((_cmd), cmd, strlen(_cmd)) == 0)
#define ADVERTISING_INTERVAL_MIN                   0x0800
#define ADVERTISING_INTERVAL_MAX                   0x1000
#define ADVERTISING_TYPE_DIRECT_IND_HIGH_DUTY      1
#define ADVERTISING_TYPE_DIRECT_IND_LOW_DUTY       4

typedef enum {
    BT_APP_POWER_STATE_INIT_NONE,
    BT_APP_POWER_STATE_INIT_START,
    BT_APP_POWER_STATE_POWER_ON_START,
    BT_APP_POWER_STATE_POWER_ON,
    BT_APP_POWER_STATE_POWER_OFF_START,
    BT_APP_POWER_STATE_POWER_OFF
} bt_app_power_state_t;

void copy_str_to_addr(uint8_t *addr, const char *str);

extern const uint8_t lt_addr_type;
extern uint8_t lt_addr[6];
extern const uint8_t oob_data[];
#ifdef MTK_BT_BAS_SERVICE_ENABLE
extern uint16_t g_bas_ccc_value_tmp;
#endif /* #ifdef MTK_BT_BAS_SERVICE_ENABLE */

extern bt_gap_le_smp_pairing_config_t pairing_config_req;

extern bt_gap_le_local_key_t local_key_req;

extern bt_hci_cmd_le_set_advertising_enable_t adv_enable;

extern bt_hci_cmd_le_set_advertising_parameters_t adv_para;

extern bt_hci_cmd_le_set_scan_enable_t scan_enable;

extern const bt_hci_cmd_le_set_scan_enable_t scan_disable;

extern bt_hci_cmd_le_set_scan_parameters_t scan_para;

extern bt_hci_cmd_le_create_connection_t connect_para;

extern bt_hci_cmd_disconnect_t disconnect_para;

extern bt_hci_cmd_le_connection_update_t conn_update_para;

extern bt_hci_cmd_le_set_advertising_data_t adv_data;
extern bt_hci_cmd_le_set_scan_response_data_t scan_data;

//extern bt_hci_cmd_le_set_multi_advertising_data_t multi_adv_data;
//extern bt_hci_cmd_le_set_multi_scan_response_data_t multi_scan_data;
//extern bt_hci_cmd_le_set_multi_advertising_parameters_t multi_adv_para;
//extern bt_hci_cmd_le_set_multi_advertising_enable_t multi_adv_enable;

extern bt_status_t (*ut_app_callback)(bt_msg_type_t, bt_status_t, void *);

bt_status_t bt_app_io_callback(void *input, void *output);

