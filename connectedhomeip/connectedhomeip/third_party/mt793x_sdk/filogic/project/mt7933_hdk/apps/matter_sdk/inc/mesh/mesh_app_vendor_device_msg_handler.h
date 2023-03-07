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

#ifndef __VENDORDEVICE_MSG_HANDLER_H__
#define __VENDORDEVICE_MSG_HANDLER_H__

#include <stdio.h>
#include <math.h>

#include "bt_mesh_access.h"
#include "mesh_app_vendor_device_def.h"

#define MESH_MODEL_BINDING_PRESENT_VALUE   0x1
#define MESH_MODEL_BINDING_TARGET_VALUE    0x2
#define MESH_MODEL_BINDING_BOTH_VALUE      (MESH_MODEL_BINDING_PRESENT_VALUE | MESH_MODEL_BINDING_TARGET_VALUE)
#define MESH_MODEL_BINDING_MASK            0x3

#define MESH_MODEL_STATE_GENERIC_ON_OFF                  0x1
#define MESH_MODEL_STATE_GENERIC_LEVEL                   0x2
#define MESH_MODEL_STATE_GENERIC_ON_POWER_UP             0x4
#define MESH_MODEL_STATE_LIGHTING_LIGHTNESS_ACTUAL       0x8
#define MESH_MODEL_STATE_LIGHTING_LIGHTNESS_LINEAR       0x10
#define MESH_MODEL_STATE_LIGHTING_LIGHTNESS_RANGE        0x20
#define MESH_MODEL_STATE_LIGHTING_CTL_LIGHTNESS          0x40
#define MESH_MODEL_STATE_LIGHTING_CTL_TEMPERATURE        0x80
#define MESH_MODEL_STATE_LIGHTING_HSL_LIGHTNESS          0x100
#define MESH_MODEL_STATE_LIGHTING_HSL_HUE                0x200
#define MESH_MODEL_STATE_LIGHTING_HSL_SATURATION         0x400
#define MESH_MODEL_STATE_MASK                            0x7FF

extern lighting_ctl_server_t *gCTL_server;
extern lighting_ctl_temperature_server_t *gCTL_temperature_server;
extern lighting_hsl_server_t *gHSL_server;
extern lighting_hsl_hue_server_t *gHSL_hue_server;
extern lighting_hsl_saturation_server_t *gHSL_saturation_server;

typedef enum {
    BT_MESH_MODEL_STATUS_SUCCESS,
    BT_MESH_MODEL_STATUS_CANNOT_SET_RANGE_MIN,
    BT_MESH_MODEL_STATUS_CANNOT_SET_RANGE_MAX,
    BT_MESH_MODEL_STATUS_RFU,
} bt_mesh_model_status_code_t;

/******************************************************************************/
/* prototype                                                                  */
/******************************************************************************/

void light_hsl_server_msg_handler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg);
void light_ctl_server_msg_handler(uint16_t model_handle, const bt_mesh_access_message_rx_t *msg, const void *arg);

void bind_generic_power_up(uint16_t state, uint8_t binding);
void bind_ctl_temperature(lighting_ctl_server_t *ctl_server,
                          lighting_ctl_temperature_server_t *ctl_temp_server, uint16_t state, uint8_t binding);
void bind_hsl_hue(lighting_hsl_hue_server_t *hsl_hue_server,
                  lighting_hsl_server_t *hsl_server, uint16_t state, uint8_t binding);
void bind_hsl_saturation(lighting_hsl_saturation_server_t *hsl_saturation_server,
                         lighting_hsl_server_t *hsl_server, uint16_t state, uint8_t binding);

#endif // __VENDORDEVICE_MSG_HANDLER_H__

