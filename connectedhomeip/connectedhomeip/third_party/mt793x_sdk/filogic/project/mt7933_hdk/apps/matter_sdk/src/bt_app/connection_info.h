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

#ifndef __CONNECTION_INFO_H__
#define __CONNECTION_INFO_H__

#include "bt_system.h"
#include "bt_gap_le.h"
#include "project_config.h"

BT_EXTERN_C_BEGIN
#define BT_ADDR_TYPE_UNKNOW 0xFF
BT_PACKED(
typedef struct {
    bt_handle_t       connection_handle;
    bt_role_t         role;
    bt_addr_t         peer_addr;
    uint8_t           gatts_wait_att_rx_opcode;//use to wait handle value confirmation.
    uint8_t           subscribed;
}) app_bt_connection_cb_t;

BT_PACKED(
typedef struct {
    bt_addr_t bt_addr;
    bt_gap_le_bonding_info_t info;
}) app_bt_bonded_info_t;

extern app_bt_connection_cb_t connection_cb[BT_LE_CONNECTION_MAX];

void init_connection_info(void);
void add_connection_info(void *buff);
bool delete_connection_info(void *buff);
app_bt_connection_cb_t *find_connection_info_by_handle(bt_handle_t target_handle);
app_bt_bonded_info_t *get_bonded_info(const bt_addr_t *target_bt, uint8_t create);
app_bt_bonded_info_t *find_bonded_info_by_index(uint8_t idx);
app_bt_bonded_info_t *find_bonded_info_by_handle(bt_handle_t target_handle);
void cancel_bonded_info(const bt_addr_t *target_bt);
void clear_bonded_info(void);
void dump_bonded_info_list(void);
void dump_connection_info_list(void);
void dump_bonded_info(const app_bt_bonded_info_t *bonded_info);
uint16_t num_connection_info(void);

BT_EXTERN_C_END

#endif /* #ifndef __CONNECTION_INFO_H__ */
