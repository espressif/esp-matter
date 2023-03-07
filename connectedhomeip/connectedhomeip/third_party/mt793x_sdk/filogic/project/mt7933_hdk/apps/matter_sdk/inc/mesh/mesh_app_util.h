/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#ifndef __MESH_APP_UTIL_H__
#define __MESH_APP_UTIL_H__
#include "FreeRTOS.h"
#include <timers.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "bt_type.h"
#include "bt_mesh_os_layer_api.h"

/* Set mesh mode to controller (MTK specific VND(0xFD80)). */
typedef enum {
    BLE_CTR_MESH_MODE_NONE = 0,         /**< w/o. mesh. FW default mode*/
    BLE_CTR_MESH_MODE_WORKING = 1,      /**<  mesh working mode. */
    BLE_CTR_MESH_MODE_STANDBY = 2,      /**< mesh standby mode. */
} ble_ctr_mesh_mode_t;


typedef struct {
    const char *name;
    bt_status_t (*io_callback)(void *no, uint16_t argc, char **argv);
} mesh_app_callback_table_t;

bt_status_t cmd_friend_request(void *no, uint16_t argc, char **argv);
bt_status_t cmd_friend_offer(void *no, uint16_t argc, char **argv);
bt_status_t cmd_friend_poll(void *no, uint16_t argc, char **argv);
bt_status_t cmd_friend_update(void *no, uint16_t argc, char **argv);
bt_status_t cmd_provision_add(void *no, uint16_t argc, char **argv);
bt_status_t cmd_provision_remove(void *no, uint16_t argc, char **argv);
bt_status_t cmd_control_heartbeat(void *no, uint16_t argc, char **argv);
bt_status_t cmd_control_ack(void *no, uint16_t argc, char **argv);
bt_status_t cmd_access_attention(void *no, uint16_t argc, char **argv);
bt_status_t cmd_dump(void *no, uint16_t argc, char **argv);
bt_status_t cmd_power(void *no, uint16_t argc, char **argv);
bt_status_t cmd_bearer(void *no, uint16_t argc, char **argv);

void bt_mesh_app_util_dump_pdu(char *pdu_name, uint8_t pdu_len, uint8_t *pdu);
void bt_mesh_app_util_dump_u16_pdu(char *pdu_name, uint8_t pdu_len, uint16_t *pdu);
bool bt_mesh_app_util_str2u8HexNum(const char *src, uint8_t *dst);
bool bt_mesh_app_util_str2u16HexNum(const char *src, uint16_t *dst);
bool bt_mesh_app_util_str2u32HexNum(const char *src, uint32_t *dst);
uint8_t bt_mesh_app_util_str2u8HexNumArray(const char *src, uint8_t *dst);
uint8_t bt_mesh_app_util_str2u16HexNumArray(uint16_t argc, char **argv, uint16_t *dst_list);
void bt_mesh_app_util_str2u8HexAddrArray(const char *src, uint8_t *dst);
void bt_mesh_app_util_display_hex_log(const uint8_t *buffer, uint16_t len);

uint8_t *bt_mesh_app_util_alloc(uint16_t size);
void bt_mesh_app_util_free(uint8_t *p);

void bt_mesh_app_util_init_timer(TimerHandle_t *timer_id, char *name, bool auto_reload, TimerCallbackFunction_t pxCallbackFunction);
void bt_mesh_app_util_deinit_timer(TimerHandle_t timer_id);
void bt_mesh_app_util_stop_timer(TimerHandle_t timer_id);
uint32_t bt_mesh_app_util_is_timer_active(TimerHandle_t timer_id);
void bt_mesh_app_util_start_timer(TimerHandle_t timer_id, uint32_t ms);
void bt_mesh_app_util_start_timer_ext(char *name, TimerHandle_t *timer_id, bool is_repeat, uint32_t ms, TimerCallbackFunction_t pxCallbackFunction);

void bt_mesh_app_util_check_device_uuid(uint8_t *device_uuid);
void bt_mesh_app_util_change_wifi_service_time(uint8_t time);
void bt_mesh_app_util_set_ctr_mesh_mode(ble_ctr_mesh_mode_t mode);

bool bt_app_mesh_is_enabled(void);
void bt_app_mesh_set_enable(bool is_enabled);
#endif // __MESH_APP_UTIL_H__
