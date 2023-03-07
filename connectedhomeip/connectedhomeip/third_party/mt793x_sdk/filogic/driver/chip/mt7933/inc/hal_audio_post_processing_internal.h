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

#ifndef __HAL_AUDIO_POST_PROCESSING_INTERNAL_H__
#define __HAL_AUDIO_POST_PROCESSING_INTERNAL_H__

#include <stdint.h>

#define MTK_AUDIO_IIR_FILTER_ENABLE
/* #define MTK_AUDIO_IIR_FILTER_UNIT_TEST */

#define ACF_TRUE    (1)
#define ACF_FALSE   (0)

#define ACF_EVERYTHING_IS_VALID         ( 0)
#define ACF_NO_UPDATE_FLAG              (-1)
#define ACF_INVALID_SAMPLING_RATE       (-2)
#define ACF_INVALID_COEFFICIENT_TABLE   (-3)

#define ACF_SAMPLING_RATE_48000HZ   (48000)
#define ACF_SAMPLING_RATE_44100HZ   (44100)
#define ACF_SAMPLING_RATE_32000HZ   (32000)
#define ACF_SAMPLING_RATE_24000HZ   (24000)
#define ACF_SAMPLING_RATE_22050HZ   (22050)
#define ACF_SAMPLING_RATE_16000HZ   (16000)
#define ACF_SAMPLING_RATE_12000HZ   (12000)
#define ACF_SAMPLING_RATE_11025HZ   (11025)
#define ACF_SAMPLING_RATE_08000HZ   ( 8000)

#define HPF_FREQ_MAX    (  800) /* Unit: Hz */
#define HPF_FREQ_MIN    (   10) /* Unit: Hz */
#define LPF_FREQ_MAX    (20000) /* Unit: Hz */
#define LPF_FREQ_MIN    ( 8000) /* Unit: Hz */
#define BPF_FC_MAX      (20000) /* Unit: Hz */
#define BPF_FC_MIN      (   10) /* Unit: Hz */
#define BPF_BW_MAX      (10000) /* Unit: Hz */
#define BPF_BW_MIN      (   10) /* Unit: Hz */
#define BPF_GAIN_MAX    (    6) /* Unit: dB */
#define BPF_GAIN_MIN    (   -6) /* Unit: dB */

#define HPF_NUM             2   /* Unit: 2nd-order IIR filter */
#define LPF_NUM             1   /* Unit: 2nd-order IIR filter */
#define BPF_NUM             8   /* Unit: 2nd-order IIR filter */
#define IIR_NUM             (HPF_NUM + LPF_NUM + BPF_NUM)
#define ACF_ONE_COEF_LEN    (12)
#define ACF_HPF_COEF_LEN    (HPF_NUM * ACF_ONE_COEF_LEN)
#define ACF_ALL_COEF_LEN    (IIR_NUM * ACF_ONE_COEF_LEN)

#define ACF_PI          ((double)3.14159265358979323846264338328)
#define COS_1PI_8TH_X2  ((double)1.84775906502257)
#define COS_3PI_8TH_X2  ((double)0.76536686473018)
#define ACF_TICK1       ((double)(1 << 27))
#define ACF_TICK2       ((double)(1 << 14))
#define ACF_TICK3       (ACF_TICK1 / ACF_TICK2)
#define ACF_TICK4       ((double)(1 << 30))
#define ACF_TICK5       (ACF_TICK1 / ACF_TICK4)
#define ACF_U32_VALUE   ((double)4294967296.0)
#define ACF_U16_VALUE   ((double)(1 << 16))

#define AUDIO_IIR_STATE_SHIFT_BIT   12
#define AUDIO_IIR_STATE_MASK        (0xF << AUDIO_IIR_STATE_SHIFT_BIT)
#define AUDIO_IIR_STATE_CLR         (~AUDIO_IIR_STATE_MASK)
#define AUDIO_IIR_STATE_IDLE        (0x0 << AUDIO_IIR_STATE_SHIFT_BIT)
#define AUDIO_IIR_STATE_START       (0x1 << AUDIO_IIR_STATE_SHIFT_BIT)
#define AUDIO_IIR_STATE_RUNNING     (0xF << AUDIO_IIR_STATE_SHIFT_BIT)
#define AUDIO_IIR_STATE_STOP        (0xD << AUDIO_IIR_STATE_SHIFT_BIT)

#define AUDIO_IIR_POSITION_BIT                  11
#define AUDIO_IIR_POSITION_MASK                 (0x1 << AUDIO_IIR_POSITION_BIT)
#define AUDIO_IIR_POSITION_CLR                  (~AUDIO_IIR_POSITION_MASK)
#define AUDIO_IIR_POSITION_BEFORE_PCM_ROUTER    (0x1 << AUDIO_IIR_POSITION_BIT)
#define AUDIO_IIR_POSITION_AFTER_PCM_ROUTER     (0x0 << AUDIO_IIR_POSITION_BIT)

#define AUDIO_IIR_ENABLE_MASK       0x7FF

#define AUDIO_IIR_PM_TABLE_PAGE     3
#define AUDIO_IIR_PM_TABLE_ADDR     0x15BA

typedef struct {
    uint32_t acf_par_bpf_id_len;
    uint32_t acf_par_bpf_fc_bw;
    uint32_t acf_par_bpf_gain;
    uint32_t acf_par_bpf_blank1;
    uint32_t acf_par_bpf_blank2;
} acf_bpf_format_t;

typedef struct {
    uint32_t acf_par_ascii_naming;
    uint32_t acf_par_bitlen_ver;
    uint32_t acf_par_intver_sosnum;
    uint32_t acf_par_hpf1_id_len;
    uint32_t acf_par_hpf1_fc;
    uint32_t acf_par_hpf2_blank1;
    uint32_t acf_par_hpf2_blank2;
    uint32_t acf_par_hpf2_blank3;
    uint32_t acf_par_hpf2_blank4;
    uint32_t acf_par_hpf2_blank5;
    uint32_t acf_par_lpf1_id_len;
    uint32_t acf_par_lpf1_fc;
    uint32_t acf_par_lpf1_blank1;
    uint32_t acf_par_lpf1_blank2;
    uint32_t acf_par_lpf1_blank3;
    acf_bpf_format_t acf_par_bpf[BPF_NUM];
} acf_parameter_format_t;

typedef struct {
    uint32_t fc;
    uint32_t bw;
    float gain;
} acf_bpf_design_t;

typedef struct {
    uint32_t hpf_order;
    uint32_t hpf_fc;
    uint32_t lpf_order;
    uint32_t lpf_fc;
    acf_bpf_design_t bpf[BPF_NUM];
} acf_design_t;

typedef struct {
    uint32_t update_flag;
    acf_parameter_format_t format;
    acf_design_t design;
    uint16_t dsp_ctrl;
    uint16_t coef[ACF_ALL_COEF_LEN];
} acf_information_t;

void audio_post_processing_init(void);
void audio_iir_generate_coefficient_table(uint32_t sampling_rate);
int32_t is_applying_audio_iir(uint32_t sr_val);
void audio_write_iir_coefficients_to_dsp(void);
void audio_iir_start(void);
void audio_iir_stop(void);
void audio_iir_set_position(bool is_before_pcm_route);

#endif /* #ifndef __HAL_AUDIO_POST_PROCESSING_INTERNAL_H__ */
