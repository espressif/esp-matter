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

#ifndef __HAL_AUDIO_INTERNAL_PCM2WAY_H__
#define __HAL_AUDIO_INTERNAL_PCM2WAY_H__

#include "hal_audio.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#include<stdint.h>
#include<stdbool.h>
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_internal_service.h"
#include "hal_audio_internal_pcm.h"
#include "hal_log.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

typedef void (*audio_pcm2way_stream_in_callback_t)(void);

typedef enum {
    PCM2WAY_WIDEBAND = 0,
    PCM2WAY_NARROWBAND
} audio_pcm2way_band_t;

typedef enum {
    PCMEX_OFF = 0,
    PCMEX_ON,
    PCMEX_READY
} pcm_ex_state_t;

typedef enum {
    UL_FIFO = 0,
    I2S_FIFO
} audio_pcm2way_afe_uplink_fifo;

typedef enum {
    PCM2WAY_FORMAT_NORMAL = 0,  //NB
    PCM2WAY_FORMAT_WB_NORMAL,   //normal usage for WB p2w
    PCM2WAY_FORMAT_CAL,         //calibration
    PCM2WAY_FORMAT_WB_CAL       //WB calibration
} audio_pcm2way_format_t;

typedef struct {
    bool stream_in_d2m_on;
    bool stream_in_is_after_enhancement;
    bool stream_out_m2d_on;
    bool stream_out_is_after_enhancement;
    audio_pcm2way_band_t band;
    audio_pcm2way_format_t format;
} audio_pcm2way_config_t;

typedef struct {
    audio_pcm2way_config_t audio_pcm2way_config;
    uint16_t audio_id;
    uint32_t dsp_pcm_ex_rx_len;
    uint32_t dsp_pcm_ex_tx_len;
    speech_path_type_t path;
    audio_pcm2way_afe_uplink_fifo pcm2way_uplink_fifo;
    audio_pcm2way_stream_in_callback_t stream_in_callback;
} audio_pcm2way_t;

#define PCM2WAY_PRESILENCE_NUM 7

void audio_pcm2way_set_dsp_uplink_fifo(audio_pcm2way_afe_uplink_fifo uplink_fifo);
void audio_pcm2way_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data);
void audio_pcm2way_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data);
hal_audio_status_t audio_pcm2way_stream_in_read(const void *buffer, uint32_t size);
hal_audio_status_t audio_pcm2way_stream_out_write(const void *buffer, uint32_t size);
void audio_pcm2way_get_from_stream_in(uint16_t *stream_in_buffer);
void audio_pcm2way_get_from_stream_in_second_mic(uint16_t *stream_in_buffer);
void audio_pcm2way_put_to_stream_out(uint16_t *stream_out_buffer);
void audio_pcm2way_start(void (*pcm2way_dl_hdlr)(void), void (*pcm2way_ul_hdlr)(void), audio_pcm2way_config_t *config);
void audio_pcm2way_stop(void);
void audio_pcm2way_play_voice(void);
void audio_pcm2way_stop_voice(void);
speech_path_type_t audio_pcm2way_get_path_type(void);
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */

#endif /* #ifndef __HAL_AUDIO_INTERNAL_PCM2WAY_H__ */
