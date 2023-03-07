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

#ifndef __VENDORDEVICE_DEF_HH
#define __VENDORDEVICE_DEF_HH

#ifndef SUPPORT_CTL
#define SUPPORT_CTL 1
#endif

#ifndef SUPPORT_HSL
#define SUPPORT_HSL 0//remove HSL for SQC, keep CTL only
#endif

//move generic onoff and dtt model to CTL, keep CTL only for SQC
//#define MESH_APP_ADD_GENERIC_ON_OFF_IN_MAIN

#define VD_DEBUG_DUMP_PDU(pdu_name, pdu_len, pdu) \
    { \
        bt_mesh_app_util_dump_pdu(pdu_name, pdu_len, pdu); \
    }

#define VD_DEBUG_COLOR_PRINTF(type, color) \
    { \
        LOG_I(mesh_app, "%s\n", type); \
    }

#define VD_ERROR_COLOR_PRINTF(type, color) \
    { \
        LOG_E(mesh_app, "%s\n", type); \
    }

#define VD_DEBUG_EVT_SIG(opcode, color) \
    { \
        LOG_I(mesh_app, "SigMsg[%04x]\n", opcode); \
    }

#define VD_DEBUG_EVT_VENDOR(companyid, opcode, color) \
    { \
        LOG_I(mesh_app, "VendorMsg[%02x:%04x]\n", opcode, companyid); \
    }

#define VD_DEBUG_EVT_PRINTF(evt_str, color) \
    LOG_I(mesh_app,  "%s\n", evt_str); \

#define MESH_VENDOR_MODEL_ID(companyid, modelid) ((companyid << 16) | modelid)

typedef struct {
    uint8_t onOff;
    uint8_t TID;
    uint8_t transTime;
    uint8_t delay;
} mesh_onoff_server_model_t;

typedef struct {
    int16_t level;
    uint8_t TID;
    uint8_t transTime;
    uint8_t delay;
} mesh_level_server_model_t;

typedef struct {
    uint8_t present_on_off;
    uint8_t target_on_off;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
} generic_onoff_server_model_t;

typedef struct {
    int16_t present_level;
    int16_t target_level;
    int16_t original_present_level;
    int16_t original_target_level;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
} generic_level_server_model_t;

typedef struct {
    uint8_t on_power_up;
} generic_on_power_up_server_model_t;

typedef struct {
    uint8_t default_transition_time;
} default_transition_time_server;

typedef struct {
    uint16_t present_lightness;
    uint16_t target_lightness;
    uint16_t present_linear_lightness;
    uint16_t target_linear_lightness;
    uint16_t default_lightness;
    uint16_t last_lightness;
    uint16_t range_min;
    uint16_t range_max;
    // power onoff
    generic_onoff_server_model_t onoff_server;
    generic_level_server_model_t level_server;
    generic_on_power_up_server_model_t onpowerup_server;
    default_transition_time_server *dtt_server;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
} lighting_lightness_server_t;

typedef struct {
    uint16_t present_ctl_temperature;
    uint16_t target_ctl_temperature;
    int16_t present_ctl_delta_uv;
    int16_t target_ctl_delta_uv;
    uint16_t default_temperature;
    int16_t default_delta_uv;
    uint16_t range_min;
    uint16_t range_max;
    lighting_lightness_server_t lightness_server;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
    uint16_t element_index;
} lighting_ctl_server_t;

typedef struct {
    uint16_t present_ctl_temperature;
    uint16_t target_ctl_temperature;
    int16_t present_ctl_delta_uv;
    int16_t target_ctl_delta_uv;
    generic_level_server_model_t gLevel_server;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
    uint16_t element_index;
} lighting_ctl_temperature_server_t;

typedef struct {
    uint16_t present_hsl_hue;
    uint16_t present_hsl_saturation;
    uint16_t target_hsl_hue;
    uint16_t target_hsl_saturation;
    uint16_t default_hue;
    uint16_t default_saturation;
    uint16_t hue_range_min;
    uint16_t hue_range_max;
    uint16_t saturation_range_min;
    uint16_t saturation_range_max;
    lighting_lightness_server_t lightness_server;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
    uint16_t element_index;
} lighting_hsl_server_t;

typedef struct {
    uint16_t present_hsl_hue;
    uint16_t target_hsl_hue;
    generic_level_server_model_t gLevel_server;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
    uint16_t element_index;
} lighting_hsl_hue_server_t;

typedef struct {
    uint16_t present_hsl_saturation;
    uint16_t target_hsl_saturation;
    generic_level_server_model_t gLevel_server;
    uint8_t TID;
    uint8_t transition_time;
    uint8_t delay;
    uint16_t element_index;
} lighting_hsl_saturation_server_t;

#endif // __VENDORDEVICE_DEF_HH
