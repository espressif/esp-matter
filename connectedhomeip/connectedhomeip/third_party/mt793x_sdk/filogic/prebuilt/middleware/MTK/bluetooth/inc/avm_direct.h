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

#ifndef __AVM_DIRECT_H__
#define __AVM_DIRECT_H__

#include "bt_type.h"
#include "bt_system.h"
#include "bt_debug.h"
#include "bt_platform.h"



uint32_t avm_direct_us_2_asi(uint32_t freq, uint32_t dur);

void avm_direct_bt_clock_normalize(bt_clock_t *src, bt_clock_t *dst, bt_role_t role);

void avm_direct_bt_clock_add_duration(bt_clock_t *dest, bt_clock_t *base, bt_clock_t *dur, bt_role_t role);

void avm_direct_cal_1st_pkt_play_time(uint32_t freq, uint32_t ratio, uint32_t ts_base, uint32_t ts_cur,bt_clock_t *clk_cur,
    bt_clock_t *bt_clk_target, bt_clock_t *dur,
    uint32_t samples_per_pkt);

bool avm_direct_cal_play_time_according_base(uint32_t freq, uint32_t ratio, uint32_t asi_base,
    uint32_t ts_cur, bt_clock_t *bt_clk_base, 
    bt_clock_t *bt_clk_play, uint32_t samples_per_pkt);

void avm_direct_cal_pta(uint32_t asi_dur, bt_clock_t *pta, bt_clock_t *clk_base);

bt_clock_t *avm_direct_get_sink_latency_by_tick(void);

int16_t avm_direct_cal_drift(int32_t n, const uint32_t *x, const int32_t *y);

void avm_direct_clear_drift_parameters(void);

int16_t avm_direct_get_drift(void);

void avm_direct_set_drift(int16_t drift_val);

void avm_direct_cancel_audio_play_en(uint32_t gap_hd);

bool avm_direct_refine_timestamp(uint32_t *ts_packeted,uint32_t ts_base, uint32_t ratio, uint32_t ts_cur);


/**
 * @brief                           Function to set sink latency, if sink latency is set, it would be valid after next time to start playing.
 * @param[in] latency_us    Is the sink latency value,it`s unit is us, and it should be the integer multiple of 1250us, default value is 140000us.
 * @return                         The status to set sink latency, if return value is not BT_STATUS_SUCCESS, please check the parameter.
 */
bt_status_t avm_direct_set_sink_latency(uint32_t latency_value);

/**
 * @brief                           Function to get sink latency.
 * @return                         The current sink latency value.
 */
uint32_t avm_direct_get_sink_latency(void);

#endif /*__AVM_DIRECT_H__*/

