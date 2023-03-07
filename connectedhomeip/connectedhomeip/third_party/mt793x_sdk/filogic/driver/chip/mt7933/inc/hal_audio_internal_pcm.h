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

#ifndef __HAL_AUDIO_PCM_INTERNAL_H__
#define __HAL_AUDIO_PCM_INTERNAL_H__

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "hal_audio_internal_service.h"
#include "hal_nvic.h"
#include "hal_audio_post_processing_internal.h"
#include "hal_audio_fw_internal_common.h"
#include "hal_gpt.h"

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#define PCM_RING_BUFFER_SIZE          1024*12//bytes
#else /* #ifdef HAL_AUDIO_LOW_POWER_ENABLED */
#define PCM_RING_BUFFER_SIZE          1024*4  //bytes
#endif /* #ifdef HAL_AUDIO_LOW_POWER_ENABLED */

#define AUDIO_PCM_STREAM_IN_MEMORY_SIZE   FOUR_BYTE_ALIGNED(PCM_RING_BUFFER_SIZE)
#define AUDIO_PCM_STREAM_OUT_MEMORY_SIZE  FOUR_BYTE_ALIGNED(PCM_RING_BUFFER_SIZE)
#define LZMA_INTERNAL_MEMORY_SIZE         FOUR_BYTE_ALIGNED(9836)
#define LZMA_BUFFER_MEMORY_SIZE           FOUR_BYTE_ALIGNED(38916)
#ifndef __DSP_COMPRESS_7Z__
#define AUDIO_PCM_MEMORY_SIZE             (AUDIO_PCM_STREAM_OUT_MEMORY_SIZE + AUDIO_PCM_STREAM_IN_MEMORY_SIZE)
#else /* #ifndef __DSP_COMPRESS_7Z__ */
#define AUDIO_PCM_MEMORY_SIZE             (AUDIO_PCM_STREAM_OUT_MEMORY_SIZE + AUDIO_PCM_STREAM_IN_MEMORY_SIZE+LZMA_BUFFER_MEMORY_SIZE+LZMA_INTERNAL_MEMORY_SIZE)
#endif /* #ifndef __DSP_COMPRESS_7Z__ */


#define PCM_SOUND_INTERRUPT_PERIOD  20 //ms
#define PCM_PLAYBACK_DSP_PAGE_NUM   5
#define PCM_RECORD_RINGBUFFER_DEFAULT_THRESHOLD   320
#define RECORD_PRESILENCE_NUM 7

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
#define PCM_SD_PLAYBACK_DSP_PAGE    5
#define PCM_SD_PLAYBACK_DSP_ADDR    0000

#define PCM_SD_PLAYBACK_STATE_SHIFT_BIT 12
#define PCM_SD_PLAYBACK_STATE_MASK      (0xF << PCM_SD_PLAYBACK_STATE_SHIFT_BIT)
#define PCM_SD_PLAYBACK_STATE_CLR       (~PCM_SD_PLAYBACK_STATE_MASK)
#define PCM_SD_PLAYBACK_STATE_IDLE      (0x0 << PCM_SD_PLAYBACK_STATE_SHIFT_BIT)
#define PCM_SD_PLAYBACK_STATE_START     (0x1 << PCM_SD_PLAYBACK_STATE_SHIFT_BIT)
#define PCM_SD_PLAYBACK_STATE_RUNNING   (0xF << PCM_SD_PLAYBACK_STATE_SHIFT_BIT)
#define PCM_SD_PLAYBACK_STATE_FLUSH     (0xE << PCM_SD_PLAYBACK_STATE_SHIFT_BIT)
#endif /* #if defined(MTK_AUDIO_IIR_FILTER_ENABLE) */

#define  ASP_DSP_PCM_BUFFER_BASE_PCM 0x0
#define  WAV_DSP_STATE_IDLE    0x0
#define  WAV_DSP_STATE_INIT    0x1
#define  WAV_DSP_STATE_READY   0x2
#define  WAV_DSP_STATE_START   0x4
#define  WAV_DSP_STATE_RUNING  0x8

typedef enum {
    PCM_STATE_IDLE = 0,
    PCM_STATE_INIT,
    PCM_STATE_RUNNING,
    PCM_STATE_FLUSHED,
    PCM_STATE_STOP,
    PCM_STATE_ERROR
} pcm_state_t;

typedef struct {
    hal_audio_stream_out_callback_t callback;
    ring_buffer_information_t       mcu_buffer_info;
    ring_buffer_information_t       dsp_buffer_info;
    pcm_state_t         state;
    bool                underflow;
    bool                waiting;
    bool                end_of_data;
    uint16_t            audio_id;
    uint32_t            sample_rate;
    uint8_t             channel;
    void               *user_data;
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    uint32_t            is_sd_task;
#endif /* #if defined(MTK_AUDIO_IIR_FILTER_ENABLE) */
    // temp solution
    hal_audio_stream_copy_callback_t copy_callback;
    void                             *copy_user_data;
} pcm_playback_internal_t;

typedef struct {
    hal_audio_stream_in_callback_t callback;
    ring_buffer_information_t       mcu_buffer_info;
    bool                waiting;
    uint16_t            audio_id;
    void               *user_data;
    uint8_t             presilence;
} pcm_record_internal_t;

typedef struct {
    hal_audio_stream_out_callback_t out_callback;
    hal_audio_stream_in_callback_t  in_callback;
    ring_buffer_information_t       dsp_buffer_info;
    pcm_state_t         state;
    bool                underflow;
    bool                out_waiting;
    bool                in_waiting;
    uint16_t            audio_id;
    uint32_t            sample_rate;
    uint8_t             channel;
    void               *out_user_data;
    void               *in_user_data;
    uint8_t             presilence;
} pcm2way_internal_t;

/*function prototype*/
hal_audio_status_t audio_pcm_set_memory(uint8_t *memory);
uint32_t audio_pcm_get_memory_size(void);
void audio_pcm_get_stream_out_free_space(uint32_t *sample_count);
hal_audio_status_t audio_pcm_stream_out_write(const void *buffer, uint32_t size);
hal_audio_status_t audio_pcm_stream_in_read(const void *buffer, uint32_t size);
void audio_pcm_set_eof(void);
/*PCM playback*/
hal_audio_status_t audio_pcm_play_open_device(void);
void audio_pcm_play_close_device(void);
void audio_pcm_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data);
void audio_pcm_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data);
void audio_pcm_set_dsp_sd_task_playback(uint16_t frame_length);
void audio_pcm_stop_dsp_sd_task_playback(void);
void audio_pcm_copy_stream_out_enable(hal_audio_stream_copy_callback_t callback, void *user_data);
/*PCM record*/
void audio_pcm_get_stream_in_record_size(uint32_t *sample_count);
void audio_pcm_record_handle(bool enable);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifdef HAL_AUDIO_MODULE_ENABLED */

#endif /* #ifndef __HAL_AUDIO_PCM_INTERNAL_H__ */
