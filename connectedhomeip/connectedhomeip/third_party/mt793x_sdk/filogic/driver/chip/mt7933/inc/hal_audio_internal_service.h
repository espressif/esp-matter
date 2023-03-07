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

#ifndef __HAL_AUDIO_INTERNAL_SERVICE_H__
#define __HAL_AUDIO_INTERNAL_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal_audio.h"
#include "hal_dvfs_internal.h"
#include "hal_audio_post_processing_internal.h"
#include <assert.h>

#ifndef NULL
#define NULL 0
#endif /* #ifndef NULL */

#define UPPER_BOUND(in,up)      ((in) > (up) ? (up) : (in))
#define LOWER_BOUND(in,lo)      ((in) < (lo) ? (lo) : (in))
#define BOUNDED(in,up,lo)       ((in) > (up) ? (up) : (in) < (lo) ? (lo) : (in))
//#define MAXIMUM(a,b)            ((a) > (b) ? (a) : (b))
//#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))
#define FOUR_BYTE_ALIGNED(size) (((size) + 3) & ~0x3)

typedef void (*isr)(void *user_data);
#define  MAXISR 6
#define  MAX_AUDIO_FUNCTIONS 16
#define  MAX_PCM_ROUTE_FUNCTIONS 4

#define  VBI_RESET() SHERIF_WRITE(SHARE_M2DI1,0x0838)
#define  VBI_END()   SHERIF_WRITE(SHARE_M2DI1,0x0839)
#define  AUDIO_GET_CHIP_ID() ((volatile uint16_t *)0xA2000000)

#if defined(__HAL_AUDIO_AWS_SUPPORT__)
#define AWS_CLOCK_SKEW_MAX_SAMPLE_COUNT (48)            /* 1 ms for 48kHz*/
#define AWS_CLOCK_SKEW_RING_BUFFER_SAMPLE_COUNT (1440)  /* 30 ms for 48kHz */

#define AWS_SRC_PHASE_BITS  11
#define AWS_SRC_TOTAL_VALUE (1 << AWS_SRC_PHASE_BITS)
#define AWS_SRC_PHASE_MASK  (AWS_SRC_TOTAL_VALUE - 1)
#define AWS_SRC_INTERPOLATION(val_lo, val_hi, phase) (((val_hi) * (phase) + (val_lo) * (AWS_SRC_TOTAL_VALUE - (phase))) >> AWS_SRC_PHASE_BITS)
#define AWS_ADD_POINT_BUF_SIZE 48 /*by sample*/
#endif /* #if defined(__HAL_AUDIO_AWS_SUPPORT__) */

#ifdef MTK_BT_A2DP_AAC_ENABLE
#define SILENCE_DATA_LENGTH     9
#define ADTS_HEADER_LENGTH      7
#define SILENCE_TOTAL_LENGTH         ADTS_HEADER_LENGTH + SILENCE_DATA_LENGTH
extern const uint8_t AAC_ADTS_HEADER[ADTS_HEADER_LENGTH];
extern const uint8_t aac_silence_data_mono[SILENCE_DATA_LENGTH];
extern const uint8_t aac_silence_data_stereo[SILENCE_DATA_LENGTH];
#endif /* #ifdef MTK_BT_A2DP_AAC_ENABLE */
#define SBC_SILENCE_FIX_PART_LENGTH    4
#define SBC_SILENCE_MODIFY_LENGTH      1
#define SBC_SILENCE_FRAME_NUM_CNT      1
#define SBC_MAX_FRAME_PER_PAYLOAD      0xF

typedef enum {
    AUDIO_COMMON_SAMPLING_RATE_8KHZ      = 0, /**< 8000Hz  */
    AUDIO_COMMON_SAMPLING_RATE_11_025KHZ = 1, /**< 11025Hz */
    AUDIO_COMMON_SAMPLING_RATE_12KHZ     = 2, /**< 12000Hz */
    AUDIO_COMMON_SAMPLING_RATE_16KHZ     = 3, /**< 16000Hz */
    AUDIO_COMMON_SAMPLING_RATE_22_05KHZ  = 4, /**< 22050Hz */
    AUDIO_COMMON_SAMPLING_RATE_24KHZ     = 5, /**< 24000Hz */
    AUDIO_COMMON_SAMPLING_RATE_32KHZ     = 6, /**< 32000Hz */
    AUDIO_COMMON_SAMPLING_RATE_44_1KHZ   = 7, /**< 44100Hz */
    AUDIO_COMMON_SAMPLING_RATE_48KHZ     = 8, /**< 48000Hz */
    AUDIO_COMMON_SAMPLING_RATE_96KHZ     = 9  /**< 96000Hz */
} common_sampling_rate_t;

typedef enum {
    SPH_VOICE_NONE   = 0,
    SPH_VOICE_RECORD,
    SPH_VOICE_PCM2WAY_UL,
    SPH_VOICE_PCM2WAY_DL,
    SPH_BOTH_PATH = 0xFFFF
} speech_path_type_t;

typedef enum {
    AUDIO_DSP_ASSERT_NONE = 0,
    AUDIO_DSP_ASSERT_FROM_MCU,
    AUDIO_DSP_ASSERT_FROM_DSP
} audio_dsp_assert_source_t;

typedef struct {
    uint16_t audio_id;
    uint16_t num[MAXISR];
    isr function[MAXISR];
    void *userdata[MAXISR];
} audio_isr_t;

typedef struct {
    int32_t pcm_route_id;
    uint32_t counter;
    int32_t num[MAX_PCM_ROUTE_FUNCTIONS];
    isr function[MAX_PCM_ROUTE_FUNCTIONS];
    void *userdata[MAX_PCM_ROUTE_FUNCTIONS];
} audio_pcm_route_isr_t;

typedef struct {
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t buffer_byte_count;
    uint8_t *buffer_base_pointer;
} ring_buffer_information_t;

typedef enum {
    AWS_CODEC_TYPE_AAC_FORMAT,
    AWS_CODEC_TYPE_SBC_FORMAT,
    AWS_CODEC_TYPE_MP3_FORMAT,
    AWS_CODEC_TYPE_PCM_FORMAT
} aws_codec_type_t;

typedef enum {
    AWS_CLOCK_SKEW_STATUS_IDLE,
    AWS_CLOCK_SKEW_STATUS_BUSY
} aws_clock_skew_status_t;

typedef enum {
    CODEC_AWS_CHECK_CLOCK_SKEW,
    CODEC_AWS_CHECK_UNDERFLOW
} aws_event_t;

typedef void (*aws_callback_t)(aws_event_t event, void *user_data);

typedef struct {
    uint32_t                    running_state;
    uint8_t                     sleep_handle;
    audio_dsp_assert_source_t   assert_source;
    uint16_t                    active_type;
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    int32_t                     apply_iir;
#endif /* #if defined(MTK_AUDIO_IIR_FILTER_ENABLE) */
    bool                        init;
    bool                        is_dsp_codec;
    uint32_t                    mcu_sample_counter;
    uint16_t                    dsp_clock_counter;
    uint32_t                    frame_sample_count; /* One pair of L / R counts as one sample. */
    uint8_t                     sbc_frame_sample;
    uint32_t                    sd_frame_len;
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    aws_callback_t              aws_callback;
    void                        *aws_user_data;
    bool                        aws_flag;
    bool                        aws_init_flag;
    bool                        aws_skip_clk_skew_flag;
    bool                        aws_clock_skew_flag;
    uint32_t                    accumulated_sample_count;
    aws_clock_skew_status_t     clock_skew_status;
    int32_t                     remained_sample_count;
    int32_t                     prebuffer_sample_count;
    uint16_t                    *pcm_route_frame_buffer;
    uint32_t                    pcm_route_frame_size;
    uint16_t                    *pcm_route_src_buffer;
    uint32_t                    pcm_route_src_size;
    ring_buffer_information_t   pcm_route_ring_buffer;
    int32_t                     aws_pcm_route_id;
    uint32_t                    aws_pcm_route_counter; /* For debug */
#endif /* #if defined(__HAL_AUDIO_AWS_SUPPORT__) */
#ifdef MTK_BT_A2DP_AAC_ENABLE
    uint8_t                     aac_silence_pattern[SILENCE_TOTAL_LENGTH];
#endif /* #ifdef MTK_BT_A2DP_AAC_ENABLE */
} audio_common_t;

/*DSP to MCU interrupt ID*/
#define IO4567_D2C_ID_SPEECH_BASE        1          //1~30
#define IO4567_D2C_ID_AUDIO_BASE        31          //31~60
#define IO4567_D2C_ID_OTHER_BASE        61          //61~63
#define PSEUDO_SAL_DSPINT_PRIO_REC_VMEPL 1
#define PSEUDO_SAL_DSPINT_PRIO_REC_PCM  2
#define PSEUDO_SAL_DSPINT_PRIO_REC_VM   3
#define PSEUDO_SAL_DSPINT_PRIO_PNW_DL   6
#define PSEUDO_SAL_DSPINT_PRIO_PNW_UL   7
#define DSP_IID_SPEECH_UL_ID            1
#define DSP_IID_SPEECH_DL_ID            2
#define PSEUDO_SAL_DSPINT_PRIO_MAX      13

#define PSEUDO_SAL_DSPINT_ID_REC_EPL    (1 << 1)
#define PSEUDO_SAL_DSPINT_ID_REC_PCM    (1 << 2)
#define PSEUDO_SAL_DSPINT_ID_REC_VM     (1 << 3)
#define PSEUDO_SAL_DSPINT_ID_PNW_DL     (1 << 6)
#define PSEUDO_SAL_DSPINT_ID_PNW_UL     (1 << 7)

#define DSP_SD_DONE                   (IO4567_D2C_ID_SPEECH_BASE+3) /*IDLE mode playback  */ /* SD1+SD2 is done, MCU can WRITE data TO SHERIF */
#define DSP_D2M_PCM_INT               (IO4567_D2C_ID_SPEECH_BASE+4) /*PCM to MCU interrup*/
#define DSP_INT_MAPPING_BASIC         (IO4567_D2C_ID_SPEECH_BASE+5) /*Speech Driver Mapping which can be changed. It is independent of SpeechDSP's setting(D2C_Status_80.h)*/
#define DSP_D2M_PCM_ROUTE_INT         (IO4567_D2C_ID_AUDIO_BASE+1)  /*PCM Route D2M interrupt*/
#define DSP_D2C_AAC_DEC_INT           (IO4567_D2C_ID_AUDIO_BASE+3)  /*AAC Decoder D2M interrupt*/
#define DSP_D2C_SBC_DEC_INT           (IO4567_D2C_ID_AUDIO_BASE+11) /*SBC Decoder D2M Interrupt*/
#define DSP_D2C_PCM_SD_PLAYBACK_INT   (IO4567_D2C_ID_AUDIO_BASE+12) /* PCM SD Playback D2M Interrupt */
#define DSP_D2C_I2S_RX_AUD_DEC_INT    (IO4567_D2C_ID_AUDIO_BASE+13) /*I2S Rx Audio DSP Decode D2M Interrupt*/
#define DSP_DSP_EXCEPTION_ID          (IO4567_D2C_ID_OTHER_BASE+1)  /*DSP dead interrupt. 0x003E(DD44)*/
#define DSP_VMEPL_REC_INT               (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_REC_VMEPL)  /*8 PCMRECORF 8k*/
#define DSP_PCM_REC_INT               (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_REC_PCM)  /*8 PCMRECORF 8k*/
#define DSP_VM_REC_INT                (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_REC_VM)   /*9 PCMRECORF 16k*/
#define DSP_PCM_EX_DL                 (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_PNW_DL)   /*12 PCMNWAY for DL*/
#define DSP_PCM_EX_UL                 (DSP_INT_MAPPING_BASIC+PSEUDO_SAL_DSPINT_PRIO_PNW_UL)   /*13 PCMNWAY for UL*/

/*Delay margin definition */
#define  ASP_DELM_PCM_HIGFS     576
#define  ASP_DELM_PCM_MEDFS     288
#define  ASP_DELM_PCM_LOWFS     144
#define  ASP_DELM_I2SBypass     1020
#define  ASP_DELM_SBC_DEC       2048
#define  ASP_DELM_AAC_DEC       2048

/*DSP frame length*/
#define  ASP_FRAMELEN_PCM           0x2000
#define  ASP_FRAMELEN_I2S           ASP_DELM_I2SBypass /*1020*/
#define  ASP_FRAMELEN_SBC_DEC       ASP_DELM_SBC_DEC
#define  ASP_FRAMELEN_AAC_DEC       ASP_DELM_AAC_DEC

/* Audio type */
#define  ASP_TYPE_VOICE             0
#define  ASP_TYPE_PCM_HI            1
#define  ASP_TYPE_I2SBypass         5
#define  ASP_TYPE_SBC_DEC           10
#define  ASP_TYPE_PCM_SD_PLAYBACK   11
#define  ASP_TYPE_AAC_DEC           32

/* Speech WB/NB buffer size */
#define  NB_BUFFER_SIZE       160 /*narrow band 20ms * 8k*/
#define  WB_BUFFER_SIZE       320 /*width band 20ms * 16k*/
/*Frequency mapping for DSP */
#define  ASP_FS_8K            0x00
#define  ASP_FS_11K           0x11
#define  ASP_FS_12K           0x12
#define  ASP_FS_16K           0x20
#define  ASP_FS_22K           0x21
#define  ASP_FS_24K           0x22
#define  ASP_FS_32K           0x40
#define  ASP_FS_44K           0x41
#define  ASP_FS_48K           0x42
#define  ASP_FS_96K           0x43

/* Flexible control bit map, DSP_AUDIO_FLEXI_CTRL*/
#define  FLEXI_VBI_ENABLE     0x0001
#define  FLEXI_SE_ENABLE      0x0002
#define  FLEXI_SD_ENABLE      0x0004

/* DSP_AUDIO_CTRL2 bit map */
#define  DSP_AAC_CTRL_ASP     0x0001
#define  DSP_PCM_R_DIS        0x0002

#define AUD_BIT_COMFLG1_DSPMEM_NEED_CACHE_INVALID               0
#define AUD_BIT_COMFLG1_AUDLP_ENABLE                            1

#define AUDIO_DSP_PM_PAGE_COUNT 2
#define AUDIO_DSP_DM_PAGE_COUNT 3

/* DVFS CPU frequency */
#define AUDIO_CPU_FREQ_L_BOUND 104000

#define AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_BIT (7)
#define AUDIO_DSP_POST_PROCESSING_ENABLE_BIT    (0)

#define AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_MASK    (0x1 << AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_BIT)
#define AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_ON      (0x1 << AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_BIT)
#define AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_OFF     (0x0 << AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_BIT)

#define AUDIO_DSP_POST_PROCESSING_ENABLE_MASK       (0x1 << AUDIO_DSP_POST_PROCESSING_ENABLE_BIT)
#define AUDIO_DSP_POST_PROCESSING_ENABLE_ON         (0x1 << AUDIO_DSP_POST_PROCESSING_ENABLE_BIT)
#define AUDIO_DSP_POST_PROCESSING_ENABLE_OFF        (0x0 << AUDIO_DSP_POST_PROCESSING_ENABLE_BIT)

#define SPH_UL_ALIGN_DL_STATE_NORMAL    0x0
#define SPH_UL_ALIGN_DL_STATE_WAIT      0x1
#define SPH_UL_ALIGN_DL_STATE_COUNTDOWN 0x2
#define HAL_AUDIO_AWS_NORMAL        0
#define HAL_AUDIO_AWS_NOT_SUPPORT  -1
#define HAL_AUDIO_AWS_ERROR        -2

// PCM Route
#define AUDIO_PCM_ROUTE_ORDER_AWS          (1)
#define AUDIO_PCM_ROUTE_ORDER_VOICE_I2S    (2)
#define AUDIO_PCM_ROUTE_ORDER_VOICE_PROMPT (3)

/*Functions prototype*/
void audio_playback_on(uint16_t active_type, uint16_t sample_rate);
void audio_playback_off(void);
void audio_service_hook_isr(uint16_t isrtype, isr function, void *user_data);
void audio_service_unhook_isr(uint16_t isr_type);
void audio_service_dsp_to_mcu_interrupt(void);
uint16_t audio_get_id(void);
void audio_free_id(uint16_t audio_id);
void audio_service_setflag(uint16_t audio_id);
void audio_service_clearflag(uint16_t audio_id);
void audio_dsp_write_anti_alias_filter(void);
void audio_dsp_reload_coeficient(void);
void audio_dsp_speech_set_delay(uint8_t type);
void audio_clear_dsp_common_flag(void);
uint16_t audio_transfer_sample_rate(uint16_t sample_rate);
uint8_t audio_transfer_channel_number(uint16_t channel);
uint32_t audio_dsp_get_audio_sample_counter(void);
void audio_dsp_update_audio_counter(int32_t inc_cnt);
void audio_dsp_assign_audio_channel(hal_audio_channel_number_t channel_number);
void audio_service_set_post_process_control(uint16_t mask, uint16_t control);
uint32_t audio_service_get_frame_sample_count(void);
void audio_service_aws_set_flag(bool flag);
bool audio_service_aws_query_flag(void);
void audio_service_aws_set_clock_skew(bool flag);
void audio_service_aws_set_initial_sync(void);
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
int32_t audio_service_aws_init(uint8_t *buffer, aws_codec_type_t codec_type, aws_callback_t callback, void *user_data);
int32_t audio_service_aws_get_silence_frame_information(aws_codec_type_t codec_type, uint32_t *sample_count,
                                                        uint32_t *byte_count);
int32_t audio_service_aws_fill_silence_frame(uint8_t *buffer, uint32_t *byte_count, aws_codec_type_t codec_type,
                                             uint32_t *target_frm_cnt);
int32_t audio_service_aws_parse_bitstream_information(uint8_t *buffer, uint32_t buf_byte_count, aws_codec_type_t codec_type,
                                                      uint32_t *sample_count, uint32_t *pcm_byte_count);
int32_t audio_service_aws_get_buffer_size(aws_codec_type_t codec_type);
aws_clock_skew_status_t audio_service_aws_get_clock_skew_status(void);
int32_t audio_service_aws_deinit(void);
uint32_t audio_service_aws_get_accumulated_sample_count(void);
void sbc_set_silence_pattern_frame_sample(uint8_t block_len, uint8_t sub_bands);
int32_t audio_service_aws_set_clock_skew_compensation_value(int32_t sample_count);
void audio_service_a2dp_aws_pcm_route_isr(void *data);
void audio_service_aws_skip_clock_skew(bool skip_flag);
#endif /* #if defined(__HAL_AUDIO_AWS_SUPPORT__) */
#ifdef MTK_BT_A2DP_AAC_ENABLE
void aac_get_silence_pattern(uint32_t sample_rate, uint16_t channel_number, uint8_t *silence_frame);
#endif /* #ifdef MTK_BT_A2DP_AAC_ENABLE */
int32_t audio_pcm_route_register_isr(isr function, void *user_data, uint32_t order);
int32_t audio_pcm_route_deregister_isr(int32_t pcm_route_id);
void audio_pcm_route_start(void);
void audio_pcm_route_stop(void);

bool audio_is_dsp_codec(void);
void audio_get_current_stream_config(hal_audio_stream_t **config);

extern uint32_t save_and_set_interrupt_mask(void);
extern void restore_interrupt_mask(uint32_t mask);

void audio_service_dsp_memory_dump_init(void);

void audio_turn_on_pcm_interface(bool is_wideband);
void audio_turn_off_pcm_interface(void);
void audio_get_last_two_write_sample(ring_buffer_information_t *p_info, uint16_t *last_sample, uint16_t *second_last_sample);
void audio_sd_write_sample_to_dsp(ring_buffer_information_t *p_dsp_info, uint32_t byte_count, uint16_t last_sample,
                                  uint16_t second_last_sample, bool mono_to_stereo);

/*IDMA part*/
void audio_idma_read_from_dsp(uint16_t *dst, volatile uint16_t *src, uint32_t length);
void audio_idma_read_from_dsp_dropR(uint16_t *dst, volatile uint16_t *src, uint32_t length);
void audio_idma_write_to_dsp(volatile uint16_t *dst, uint16_t *src, uint32_t length);
void audio_idma_write_to_dsp_duplicate(volatile uint16_t *dst, uint16_t *src, uint32_t length);
void audio_idma_fill_to_dsp(volatile uint16_t *dst, uint16_t value, uint32_t length);

/*
Function Name:
    ring_buffer_get_data_byte_count

Return Type:
    uint32_t

Parameters:
    ring_buffer_information_t *p_info

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t data_byte_count;
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    write_pointer     = p_info->write_pointer;      // ASSERT_IF_FALSE(write_pointer < (buffer_byte_count << 1));
    read_pointer      = p_info->read_pointer;       // ASSERT_IF_FALSE(read_pointer < (buffer_byte_count << 1));
    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else { // write_pointer < read_pointer
        data_byte_count = (buffer_byte_count << 1) - read_pointer + write_pointer;
    }
    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    return data_byte_count;

Function Name:
    ring_buffer_get_space_byte_count

Return Type:
    uint32_t

Parameters:
    ring_buffer_information_t *p_info

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t data_byte_count;
    uint32_t space_byte_count;
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    data_byte_count   = ring_buffer_get_data_byte_count(p_info);    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    space_byte_count  = buffer_byte_count - data_byte_count;
    return space_byte_count;

Function Name:
    ring_buffer_get_write_information

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint8_t **pp_buffer
    uint32_t *p_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t space_byte_count;
    uint8_t *buffer_pointer;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t tail_byte_count;
    uint32_t write_byte_cnt;
    // ASSERT_IF_FALSE(p_info != NULL);
    // ASSERT_IF_FALSE(pp_buffer != NULL);
    // ASSERT_IF_FALSE(p_byte_count != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    space_byte_count  = ring_buffer_get_space_byte_count(p_info);   // ASSERT_IF_FALSE(space_byte_count <= buffer_byte_count);
    buffer_pointer    = p_info->buffer_base_pointer;                // ASSERT_IF_FALSE(buffer_pointer != NULL);
    write_pointer     = p_info->write_pointer;                      // ASSERT_IF_FALSE(write_pointer < (buffer_byte_count << 1));
    read_pointer      = p_info->read_pointer;                       // ASSERT_IF_FALSE(read_pointer < (buffer_byte_count << 1));
    if (write_pointer < buffer_byte_count) {    // 1st mirror part
        buffer_pointer += write_pointer;
        tail_byte_count = buffer_byte_count - write_pointer;
    } else {                                    // 2nd mirror part
        buffer_pointer += write_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - write_pointer;
    }
    write_byte_cnt = MINIMUM(data_byte_count, tail_byte_count);
    *pp_buffer = buffer_pointer;
    *p_byte_count = write_byte_cnt;
    return;

Function Name:
    ring_buffer_get_read_information

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint8_t **pp_buffer
    uint32_t *p_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t data_byte_count;
    uint8_t *buffer_pointer;
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t tail_byte_count;
    uint32_t read_byte_cnt;
    // ASSERT_IF_FALSE(p_info != NULL);
    // ASSERT_IF_FALSE(pp_buffer != NULL);
    // ASSERT_IF_FALSE(p_byte_count != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    data_byte_count   = ring_buffer_get_data_byte_count(p_info);    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    buffer_pointer    = p_info->buffer_base_pointer;                // ASSERT_IF_FALSE(buffer_pointer != NULL);
    write_pointer     = p_info->write_pointer;                      // ASSERT_IF_FALSE(write_pointer < (buffer_byte_count << 1));
    read_pointer      = p_info->read_pointer;                       // ASSERT_IF_FALSE(read_pointer < (buffer_byte_count << 1));
    if (read_pointer < buffer_byte_count) { // 1st mirror part
        buffer_pointer += read_pointer;
        tail_byte_count = buffer_byte_count - read_pointer;
    } else {                                // 2nd mirror part
        buffer_pointer += read_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - read_pointer;
    }
    read_byte_cnt = MINIMUM(data_byte_count, tail_byte_count);
    *pp_buffer = buffer_pointer;
    *p_byte_count = read_byte_cnt;
    return;

Function Name:
    ring_buffer_write_done

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint32_t write_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t buffer_end;
    // uint32_t space_byte_count;
    uint32_t write_pointer;
    // uint32_t tail_byte_count
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    buffer_end        = buffer_byte_count << 1;
    // space_byte_count  = ring_buffer_get_space_byte_count(p_info);   // ASSERT_IF_FALSE(space_byte_count <= buffer_byte_count);
    //                                                                 // ASSERT_IF_FALSE(write_byte_count <= space_byte_count);
    write_pointer = p_info->write_pointer; // ASSERT_IF_FALSE(write_pointer < buffer_end);
    // if (write_pointer < buffer_byte_count) { // 1st mirror part
    //     tail_byte_count = buffer_byte_count - write_pointer;
    // } else {                                // 2nd mirror part
    //     tail_byte_count = buffer_end - write_pointer;
    // }
    // ASSERT_IF_FALSE(write_byte_count <= tail_byte_count);
    write_pointer += write_byte_count;
    if (write_pointer >= buffer_end) {
        write_pointer -= buffer_end;
    }
    p_info->write_pointer = write_pointer;
    return;

Function Name:
    ring_buffer_read_done

Return Type:
    void

Parameters:
    ring_buffer_information_t *p_info
    uint32_t read_byte_count

Function Content with Full Assertion Protection:
    uint32_t buffer_byte_count;
    uint32_t buffer_end;
    // uint32_t data_byte_count;
    uint32_t read_pointer;
    // uint32_t tail_byte_count
    // ASSERT_IF_FALSE(p_info != NULL);
    buffer_byte_count = p_info->buffer_byte_count;                  // ASSERT_IF_FALSE(buffer_byte_count < 0x80000000);
    buffer_end        = buffer_byte_count << 1;
    // data_byte_count   = ring_buffer_get_data_byte_count(p_info);    // ASSERT_IF_FALSE(data_byte_count <= buffer_byte_count);
    //                                                                 // ASSERT_IF_FALSE(read_byte_count <= data_byte_count);
    read_pointer = p_info->read_pointer; // ASSERT_IF_FALSE(read_pointer < buffer_end);
    // if (read_pointer < buffer_byte_count) { // 1st mirror part
    //     tail_byte_count = buffer_byte_count - read_pointer;
    // } else {                                // 2nd mirror part
    //     tail_byte_count = buffer_end - read_pointer;
    // }
    // ASSERT_IF_FALSE(read_byte_count <= tail_byte_count);
    read_pointer += read_byte_count;
    if (read_pointer >= buffer_end) {
        read_pointer -= buffer_end;
    }
    p_info->read_pointer = read_pointer;
    return;
*/

uint32_t ring_buffer_get_data_byte_count(ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count(ring_buffer_information_t *p_info);
void ring_buffer_get_write_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_get_read_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_write_done(ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done(ring_buffer_information_t *p_info, uint32_t read_byte_count);

uint32_t ring_buffer_get_data_byte_count_non_mirroring(ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count_non_mirroring(ring_buffer_information_t *p_info);
void ring_buffer_get_write_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer,
                                                     uint32_t *p_byte_count);
void ring_buffer_get_read_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer,
                                                    uint32_t *p_byte_count);
void ring_buffer_write_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t read_byte_count);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef __HAL_AUDIO_INTERNAL_SERVICE_H__ */
