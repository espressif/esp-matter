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

#ifndef __SWITCH_MESH_MSG_HH__
#define __SWITCH_MESH_MSG_HH__

#include "mesh_app_switch_def.h"

void msg_Light_Lightness_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl);

void msg_Light_Lightness_Set(uint16_t model_handle, uint16_t dst_addr,
                             uint16_t Lightness, uint8_t tid, uint8_t transTime,
                             uint8_t delay, bool reliable, uint8_t ttl);

void msg_Light_Lightness_Linear_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl);

void msg_Light_Lightness_Last_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl);

void msg_Light_Lightness_Default_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl);

void msg_Light_Lightness_Range_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl);


void msg_Light_Hsl_Set(uint16_t model_handle, uint16_t dst_addr,
                       uint16_t Lightness, uint16_t Hue, uint16_t Saturation, uint8_t tid,
                       uint8_t transTime, uint8_t delay, bool acknowledged, uint8_t ttl);

void msg_Light_Hsl_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl);

void msg_Light_Ctl_Set(uint16_t model_handle, uint16_t dst_addr, uint16_t Lightness, uint16_t Temperature, int16_t DeltaUV, uint8_t tid, uint8_t transTime,
                       uint8_t delay, bool acknowledged, uint8_t ttl);

void msg_Light_Ctl_Get(uint16_t model_handle, uint16_t dst_addr, uint8_t ttl);

#endif // __SWITCH_MESH_MSG_HH__


