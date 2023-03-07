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

#ifndef __HAL_AUDIO_MESSAGE_STRUCT_H__
#define __HAL_AUDIO_MESSAGE_STRUCT_H__

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include "hal_audio.h"

//--------------------------------------------
// Message queue
//--------------------------------------------
#define AUDIO_MESSAGE_QUEUE_SIZE 32

typedef struct {
    uint16_t message16;
    uint16_t data16;
    uint32_t data32;
} audio_message_element_t;

typedef struct {
    uint32_t                read_index;
    uint32_t                write_index;
    audio_message_element_t message[AUDIO_MESSAGE_QUEUE_SIZE];
} audio_message_queue_t;

//--------------------------------------------
// Share buffer information
//--------------------------------------------
typedef enum {
    SHARE_BUFFER_INFO_INDEX_BT_AUDIO_DL,
    SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL,
    SHARE_BUFFER_INFO_INDEX_BT_VOICE_DL,
#ifdef MTK_BT_CODEC_BLE_ENABLED
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_UL,
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_DL,
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_UL,
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_DL,
#endif /* #ifdef MTK_BT_CODEC_BLE_ENABLED */
    SHARE_BUFFER_INFO_INDEX_PROMPT,
    SHARE_BUFFER_INFO_INDEX_RECORD,
    SHARE_BUFFER_INFO_INDEX_RINGTONE,
    SHARE_BUFFER_INFO_INDEX_NVKEY_PARAMETER,
    SHARE_BUFFER_INFO_INDEX_MAX
} n9_dsp_share_info_index_t;

// N9-DSP share information buffer (32 bytes)
typedef struct {
    uint32_t start_addr;       // start address of N9-DSP share buffer
    uint32_t read_offset;      // read pointer of N9-DSP share buffer
    uint32_t write_offset;     // write pointer of N9-DSP share buffer
    uint32_t next;             // next read position in buf for DSP
    uint32_t sampling_rate;    // for AWS clock skew
    uint16_t length;           // total length of N9-DSP share buffer
    uint8_t  bBufferIsFull;    // buffer full flag, when N9 find there is no free buffer for putting a packet,set this flag = 1, DSP will reset this flag when data be taken by DSP
    uint8_t  notify_count;     // notify count
    int32_t  drift_comp_val;   // long term drift compensation value
    uint32_t anchor_clk;       // long term drift anchor clk
    uint32_t asi_base;
    uint32_t asi_cur;
} n9_dsp_share_info_t;

#ifdef MTK_BT_CODEC_BLE_ENABLED
#define SHARE_BUFFER_BT_AUDIO_DL_SIZE      (40*1024) //** 40*1024 Reduce to BLE
#else
#define SHARE_BUFFER_BT_AUDIO_DL_SIZE      (40*1024)
#endif
#ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
#define SHARE_BUFFER_VENDOR_CODEC_SIZE (7*1024)
#endif

#define SHARE_BUFFER_BT_VOICE_UL_SIZE      (272)
#define SHARE_BUFFER_BT_VOICE_DL_SIZE      (272)
#ifdef MTK_BT_CODEC_BLE_ENABLED
#define SHARE_BUFFER_BLE_AUDIO_UL_SIZE     (20*1024)
#define SHARE_BUFFER_BLE_AUDIO_DL_SIZE     (20*1024)
#endif
#define SHARE_BUFFER_PROMPT_SIZE           (4*1024)
#define SHARE_BUFFER_RECORD_SIZE           (2*1024)
#define SHARE_BUFFER_RINGTONE_SIZE         (2*1024)
#define SHARE_BUFFER_MCU2DSP_PARAMETER_SIZE (1*1024)
#define SHARE_BUFFER_CLK_INFO_SIZE          (8)
#define SHARE_BUFFER_AIRDUMP_SIZE           (400)
#define SHARE_BUFFER_DSP2MCU_PARAMETER_SIZE (1*1024-SHARE_BUFFER_CLK_INFO_SIZE-SHARE_BUFFER_AIRDUMP_SIZE)
#define SHARE_BUFFER_NVKEY_PARAMETER_SIZE   (3*1024)

typedef struct {
    uint32_t bt_audio_dl[(SHARE_BUFFER_BT_AUDIO_DL_SIZE + 3) / 4];
    uint32_t bt_voice_ul[(SHARE_BUFFER_BT_VOICE_UL_SIZE + 3) / 4];
    uint32_t bt_voice_dl[(SHARE_BUFFER_BT_VOICE_DL_SIZE + 3) / 4];
#ifdef MTK_BT_CODEC_BLE_ENABLED
uint32_t ble_audio_ul[(SHARE_BUFFER_BLE_AUDIO_UL_SIZE + 3) / 4];
uint32_t ble_audio_dl[(SHARE_BUFFER_BLE_AUDIO_DL_SIZE + 3) / 4];
#endif
uint32_t prompt[(SHARE_BUFFER_PROMPT_SIZE + 3) / 4];
uint32_t record[(SHARE_BUFFER_RECORD_SIZE + 3) / 4];
uint32_t ringtone[(SHARE_BUFFER_RINGTONE_SIZE + 3) / 4];
uint32_t mcu2dsp_param[(SHARE_BUFFER_MCU2DSP_PARAMETER_SIZE + 3) / 4];
uint32_t dsp2mcu_param[(SHARE_BUFFER_DSP2MCU_PARAMETER_SIZE + 3) / 4];
uint32_t clk_info[(SHARE_BUFFER_CLK_INFO_SIZE + 3) / 4];
uint32_t airdump[(SHARE_BUFFER_AIRDUMP_SIZE + 3) / 4];
uint32_t nvkey_param[(SHARE_BUFFER_NVKEY_PARAMETER_SIZE + 3) / 4];
} audio_share_buffer_t;

    //--------------------------------------------
    // Error report
    //--------------------------------------------
    typedef enum {
        DSP_ERROR_REPORT_ERROR,
        DSP_ERROR_REPORT_END,
    } dsp_error_report_t;

    //--------------------------------------------
    // Codec type
    //--------------------------------------------
    typedef enum {
        AUDIO_DSP_CODEC_TYPE_CVSD = 0,
        AUDIO_DSP_CODEC_TYPE_MSBC,
        AUDIO_DSP_CODEC_TYPE_LC3,

        AUDIO_DSP_CODEC_TYPE_PCM = 0x100,
        AUDIO_DSP_CODEC_TYPE_SBC,
        AUDIO_DSP_CODEC_TYPE_MP3,
        AUDIO_DSP_CODEC_TYPE_AAC,
        AUDIO_DSP_CODEC_TYPE_VENDOR,
        AUDIO_DSP_CODEC_TYPE_OPUS
    } audio_dsp_codec_type_t;

    typedef enum {
        ENCODER_BITRATE_16KBPS  = 16,
        ENCODER_BITRATE_32KBPS  = 32,
        ENCODER_BITRATE_64KBPS  = 64,
        ENCODER_BITRATE_MAX     = 0xFFFFFFFF,
    } encoder_bitrate_t;

    //--------------------------------------------
    // Start Parameters
    //--------------------------------------------
    typedef struct {
        uint32_t                    start_time_stamp;
        uint32_t                    time_stamp_ratio;
        bool                        content_protection_exist;
        bool                        alc_enable;
        bool                        latency_monitor_enable;
    } audio_dsp_a2dp_dl_start_param_t, *audio_dsp_a2dp_dl_start_param_p;

    typedef struct {
        hal_audio_bits_per_sample_t bit_type;
        uint32_t                    sampling_rate;
        uint32_t                    channel_number;
    } audio_dsp_file_pcm_param_t;

    typedef struct {
        n9_dsp_share_info_t         *p_share_info;
        hal_audio_channel_number_t  channel_number;
        hal_audio_bits_per_sample_t bit_type;
        hal_audio_sampling_rate_t   sampling_rate;
        uint8_t                     codec_type;  //KH: should use audio_dsp_codec_type_t
    } audio_dsp_playback_info_t, *audio_dsp_playback_info_p;

    typedef struct {
        uint32_t                    header;
    } audio_dsp_file_mp3_param_t;

    typedef struct {
        uint32_t                    header;
    } audio_dsp_file_aac_param_t;

    typedef struct {
        uint32_t                    header;
    } audio_dsp_file_vendor_param_t;

    //===CM4 to DSP message structure==
    /* Open message member parameter structure */
    typedef enum {
        STREAM_IN_AFE  = 0,
        STREAM_IN_HFP,
        STREAM_IN_BLE,
        STREAM_IN_A2DP,
        STREAM_IN_PLAYBACK,
        STREAM_IN_VP,
        STREAM_IN_DUMMY = 0xFFFFFFFF,
    } mcu2dsp_stream_in_selection;

    typedef enum {
        STREAM_OUT_AFE  = 0,
        STREAM_OUT_HFP,
        STREAM_OUT_BLE,
        STREAM_OUT_RECORD,
        STREAM_OUT_DUMMY = 0xFFFFFFFF,
    } mcu2dsp_stream_out_selection;

    typedef struct {
        mcu2dsp_stream_in_selection     stream_in;
        mcu2dsp_stream_out_selection    stream_out;
        uint32_t                        *Feature;
    }  mcu2dsp_param_t, *mcu2dsp_param_p;

    typedef struct {
        hal_audio_device_t               audio_device;
        hal_audio_device_t               audio_device1;
        hal_audio_device_t               audio_device2;
        hal_audio_device_t               audio_device3;
        hal_audio_channel_selection_t    stream_channel;
        hal_audio_memory_t                      memory;
        hal_audio_interface_t                   audio_interface;
#ifdef ENABLE_2A2D_TEST
hal_audio_interface_t                   audio_interface1;
hal_audio_interface_t                   audio_interface2;
hal_audio_interface_t                   audio_interface3;
#endif
hal_audio_format_t                      format;
uint32_t                                misc_parms;
uint32_t                                sampling_rate;
uint32_t                                stream_out_sampling_rate;
uint16_t                                frame_size;
uint8_t                                 frame_number;
uint8_t                                 irq_period;
bool                                    hw_gain;
} au_afe_open_param_t, *au_afe_open_param_p;

        typedef struct {
            uint8_t param[20];//TEMP!! align bt_codec_a2dp_audio_t
        } audio_dsp_a2DP_codec_param_t;

        typedef struct {
            audio_dsp_a2DP_codec_param_t    codec_info;
            n9_dsp_share_info_t             *p_share_info;
            uint32_t                        *p_asi_buf;
            uint32_t                        *p_min_gap_buf;
            uint32_t                        *p_current_bit_rate;
            uint32_t                        sink_latency;
            uint32_t                        bt_inf_address;
            uint32_t                        *clk_info_address;
            uint32_t                        *p_lostnum_report;
        } audio_dsp_a2dp_dl_open_param_t, *audio_dsp_a2dp_dl_open_param_p;

        typedef uint8_t audio_dsp_hfp_codec_param_t;

        typedef struct {
            audio_dsp_hfp_codec_param_t     codec_type;
            n9_dsp_share_info_t             *p_share_info;
            uint32_t                        bt_inf_address;
            uint32_t                        *clk_info_address;
            uint32_t                        *p_air_dump_buf;
        } audio_dsp_hfp_open_param_t, *audio_dsp_hfp_open_param_p;

        typedef uint8_t audio_dsp_ble_codec_param_t;

        typedef struct {
            audio_dsp_ble_codec_param_t     codec_type;
            uint8_t  channel_num;
            uint8_t  channel_mode;
            uint16_t frame_payload_length;
            uint32_t frame_ms;
            uint32_t sampling_frequency;
            uint32_t context_type;
            uint8_t  dual_cis_mode;
            uint8_t  decode_mode;
            n9_dsp_share_info_t             *p_share_info;
            n9_dsp_share_info_t             *p_sub_share_info;                   /**< Codec information for second CIS. */
        } audio_dsp_ble_open_param_t, *audio_dsp_ble_open_param_p;

        typedef struct {
            n9_dsp_share_info_t *p_share_info;
            uint32_t frames_per_message;
            encoder_bitrate_t bitrate;
        } cm4_record_open_param_t, *cm4_record_open_param_p;

        typedef union {
            au_afe_open_param_t             afe;
            audio_dsp_hfp_open_param_t      hfp;
            audio_dsp_ble_open_param_t      ble;
            audio_dsp_a2dp_dl_open_param_t  a2dp;
            audio_dsp_playback_info_t       playback;
        } mcu2dsp_open_stream_in_param_t, *mcu2dsp_open_stream_in_param_p;

        typedef union {
            au_afe_open_param_t             afe;
            audio_dsp_hfp_open_param_t      hfp;
            audio_dsp_ble_open_param_t      ble;
            cm4_record_open_param_t         record;
        } mcu2dsp_open_stream_out_param_t, *mcu2dsp_open_stream_out_param_p;

        /* Open message parameter structure */
        typedef struct {
            mcu2dsp_param_t                 param;
            mcu2dsp_open_stream_in_param_t  stream_in_param;
            mcu2dsp_open_stream_out_param_t stream_out_param;
        } mcu2dsp_open_param_t, *mcu2dsp_open_param_p;


        /* Start message member parameter structure */
        typedef struct {
            bool                        aws_flag;
            bool                        aws_sync_request;
            uint32_t                    aws_sync_time;
        } audio_dsp_afe_start_param_t, *audio_dsp_afe_start_param_p;

        typedef union {
            audio_dsp_a2dp_dl_start_param_t a2dp;
            audio_dsp_afe_start_param_t     afe;
        } mcu2dsp_start_stream_in_param_t, *mcu2dsp_start_stream_in_param_p;

        typedef union {
            audio_dsp_afe_start_param_t     afe;
        } mcu2dsp_start_stream_out_param_t, *mcu2dsp_start_stream_out_param_p;

        /* Start message parameter structure */
        typedef struct {
            mcu2dsp_param_t                     param;
            mcu2dsp_start_stream_in_param_t     stream_in_param;
            mcu2dsp_start_stream_out_param_t    stream_out_param;
        } mcu2dsp_start_param_t, *mcu2dsp_start_param_p;

        /* SideTone message parameter structure */
        typedef struct {
            hal_audio_device_t                      in_device;
            hal_audio_interface_t                   in_interface;
            uint32_t                                in_misc_parms;
            hal_audio_device_t                      out_device;
            hal_audio_interface_t                   out_interface;
            uint32_t                                out_misc_parms;
            hal_audio_channel_selection_t           in_channel; /*HW out channel default R+L*/
            uint32_t                                gain;
            uint32_t                                sample_rate;
        } mcu2dsp_sidetone_param_t, *mcu2dsp_sidetone_param_p;

#ifdef MTK_ANC_ENABLE
typedef enum {
ANC_COEF_L = 0,
ANC_COEF_R,
ANC_COEF_NUM
} anc_coef_t;

        /* ANC message parameter structure */
        typedef struct {
            hal_audio_device_t                      device;
            uint16_t                                keyid_l;
            uint16_t                                keyid_r;
            uint8_t                                 *p_coef_buf[ANC_COEF_NUM];
            int16_t                                 gain_index_l;   //sw gain
            int16_t                                 gain_index_r;   //sw gain
            int16_t                                 ramp_config_gain;   //ramp gain
            int16_t                                 runtime_gain;       //ramp gain
            uint8_t                                 ramp_dly_sel;
            uint8_t                                 ramp_step;
            uint8_t                                 adc_chexc;
            uint8_t                                 support_hybrid_enable;
            uint8_t                                 hybrid_enable;
            uint8_t                                 deq_enable;
            uint8_t                                 earbuds_ch; //0:headset  1:earbuds-L  2: earbuds-R
            uint16_t                                in_analog_gain;
            uint16_t                                out_analog_gain;

            /* for ANC howling detection */
            uint8_t                                 power_detect_enable;
            uint8_t                                 smooth;
            int16_t                                 high_threshold_in_dbfs;
            int16_t                                 lower_bound_in_dbfs;
            uint8_t                                 ramp_up_dly_sel;
            uint8_t                                 ramp_up_step;
            uint8_t                                 ramp_down_dly_sel;
            uint8_t                                 ramp_down_step;
            uint8_t                                 overflow_check_enable;
            int16_t                                 overflow_threshold;

            /* for ANC howling detection but can't config */
            uint16_t                                reduce_gain_step;
            uint16_t                                enlarge_gain_step;
            uint32_t                                check_irq_period_in_us;
            uint32_t                                reset_delay_in_us;
            uint8_t                                 howling_tolerance;
        } mcu2dsp_anc_param_t, *mcu2dsp_anc_param_p;
#endif

#ifdef MTK_PEQ_ENABLE
#define PEQ_DIRECT      (0)
#define PEQ_SYNC        (1)
typedef struct {
uint8_t         *nvkey_addr;
uint16_t        peq_nvkey_id;
uint8_t         drc_enable;
uint8_t         setting_mode;
uint32_t        target_bt_clk;
uint8_t         phase_id;
uint8_t         drc_force_disable;
} mcu2dsp_peq_param_t, *mcu2dsp_peq_param_p;
#endif

        //--------------------------------------------
        // Config structure
        //--------------------------------------------
        typedef enum {
            AUDIO_PLAYBACK_CONFIG_EOF
        } audio_playback_config_index_t;

        //--------------------------------------------
        // Audio Sync Parameters
        //--------------------------------------------
        typedef struct {
            uint32_t                    time_stamp;
            uint32_t                    sample;
        } audio_dsp_a2dp_dl_time_param_t;

        //--------------------------------------------
        // A2DP LTCS data
        //--------------------------------------------
        typedef struct {
            uint32_t *p_ltcs_asi_buf;
            uint32_t *p_ltcs_min_gap_buf;
        } audio_dsp_a2dp_ltcs_report_param_t;

#ifdef __cplusplus
}
#endif

#endif /*__HAL_AUDIO_MESSAGE_STRUCT_H__ */
