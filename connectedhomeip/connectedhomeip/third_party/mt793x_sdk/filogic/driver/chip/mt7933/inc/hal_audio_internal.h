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

#ifndef __HAL_AUDIO_INTERNAL_H__
#define __HAL_AUDIO_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#include "hal_audio.h"
#include "hal_audio_message_struct.h"

#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
#define PACKED __attribute__((packed))

/*NVDM ID for Hal config*/
#define HAL_NVDM_ID_HW_TABLE_STRING      "F28F"
#define HAL_NVDM_ID_AUDIO_CH_SEL_STRING  "F2B5"
#endif /* #ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT */

//==== Definition ====
#define UPPER_BOUND(in,up)      ((in) > (up) ? (up) : (in))
#define LOWER_BOUND(in,lo)      ((in) < (lo) ? (lo) : (in))
#define BOUNDED(in,up,lo)       ((in) > (up) ? (up) : (in) < (lo) ? (lo) : (in))
//#define MAXIMUM(a,b)            ((a) > (b) ? (a) : (b))
//#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))
#define FOUR_BYTE_ALIGNED(size) (((size) + 3) & ~0x3)

//== hal_audio.c related ==
typedef struct {
    hal_audio_sampling_rate_t   stream_sampling_rate;  /**< Specifies the sampling rate of audio data.*/
    hal_audio_bits_per_sample_t stream_bit_rate;       /**< Specifies the number of bps of audio data.*/
    hal_audio_channel_number_t  stream_channel;        /**< Specifies the number of channel.*/
    hal_audio_channel_number_t  stream_channel_mode;   /**< Specifies the mode of channel.*/
    hal_audio_device_t          audio_device;          /**< Specifies the device.*/
    bool                        mute;                  /**< Specifies whether the device is mute or not.*/
    uint32_t                    digital_gain_index;    /**< Digital gain index of the audio stream.*/
    uint32_t                    analog_gain_index;     /**< Analog gain index of the audio stream.*/
} hal_audio_stream_info_t;

typedef struct {
    bool                    init;
    void                    *allocated_memory;

    // stream in/out information
    hal_audio_stream_info_t stream_in;
    hal_audio_stream_info_t stream_out;
    hal_audio_stream_info_t stream_out_DL2;

} audio_common_t;

//== hal_audio_dsp_controller.c related ==
typedef struct {
    uint32_t mState;
    int32_t  mUserCount;
} audiosys_clkmux_control;

typedef enum {
    AUDIO_CLKMUX_BLOCK_INTERNAL_BUS = 0,
    AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES,
    AUDIO_CLKMUX_BLOCK_UPLINK_HIRES,
    AUDIO_CLKMUX_BLOCK_HW_SRC,
    AUDIO_CLKMUX_BLOCK_NUM_OF_CLKMUX_BLOCK,
} audio_clkmux_block_t;

/** @brief audio structure */
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
typedef enum {
    AU_DSP_AUDIO = 0,
    AU_DSP_VOICE = 1,
} audio_scenario_sel_t;
typedef enum {
    AU_DSP_CH_LR = 0,       /**< */
    AU_DSP_CH_L,            /**< */
    AU_DSP_CH_R,            /**< */
    AU_DSP_CH_SWAP,         /**< */
    AU_DSP_CH_MIX,
    AU_DSP_CH_MIX_SHIFT,
} audio_channel_sel_t;

typedef struct HAL_AUDIO_CH_SEL_HW_MODE_s {
    uint8_t                     audioChannelGPIOH;      /**< Channel select when GPIO is high. 0:None, 1:L CH, 2:R CH, 3:Two CH*/
    uint8_t                     audioChannelGPIOL;      /**< Channel select when GPIO is low.  0:None, 1:L CH, 2:R CH, 3:Two CH*/
    uint8_t                     gpioIndex;              /**< GPIO index for audio channel select in HW mode. */
} PACKED HAL_AUDIO_CH_SEL_HW_MODE_t;

typedef struct HAL_AUDIO_CHANNEL_SELECT_s {
    uint8_t                     modeForAudioChannel;    /**< NVkey_0     Channel select mode. 0:SW_mode, 1: HW_mode */
    uint8_t                     audioChannel;           /**< NVkey_1     Channel select. 0:None, 1:L CH, 2:R CH, 3:Two CH */
    HAL_AUDIO_CH_SEL_HW_MODE_t  hwAudioChannel;         /**< NVkey_2-4  Channel select param in HW_Mode.*/
} PACKED HAL_AUDIO_CHANNEL_SELECT_t;

typedef struct HAL_DSP_PARA_AU_AFE_CTRL_s {
    uint8_t       Audio_InputDev;               /**< NVkey_0   Stream in device.   [main mic], [line in playback], [digital mic], [I2S master], [I2S slave] */
    uint8_t       Audio_OutputDev;              /**< NVkey_1   Stream in device I2S interface.   Default(0)    */
    uint8_t       Voice_InputDev;               /**< NVkey_2   Stream out device. [DAC (speaker)], [external amp.&codec], [I2S master], [I2S slave]*/
    uint8_t       Voice_OutputDev;              /**< NVkey_3   Stream out device I2S interface. Default(0)    */
    uint8_t       Voice_Sidetone_EN;
    uint8_t       Voice_Sidetone_Gain;
    uint8_t       Reserve_Sidetone_Gain;
    uint8_t       UL_AFE_PARA;
    uint8_t       DL_AFE_PARA;
} PACKED HAL_DSP_PARA_AU_AFE_CTRL_t;
#endif /* #ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT */


#ifdef MTK_PORTING_AB
typedef struct {
    uint8_t dummy; // TODO
} PACKED HAL_AUDIO_DVFS_CLK_SELECT_t;
#endif

//== Message related ==
#define MSG_TYPE_BASE_MASK              0x0F00
#define MSG_TYPE_SHIFT_BIT              8


//== Callback related ==
typedef void (*hal_audio_callback_t)(hal_audio_event_t event, void *user_data);
typedef void (*hal_bt_audio_dl_open_callback_t)(void);
typedef void (*hal_audio_notify_task_callback_t)(void);
typedef void (*hal_audio_task_ms_delay_function_t)(uint32_t ms);

//== Ring buffer ==
typedef struct {
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t buffer_byte_count;
    uint8_t *buffer_base_pointer;
} ring_buffer_information_t;

//== AWS related ==
// ToDo: workaround. Not necessary
#define HAL_AUDIO_AWS_NORMAL        0
#define HAL_AUDIO_AWS_NOT_SUPPORT  -1
#define HAL_AUDIO_AWS_ERROR        -2

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

//==== API ====
void hal_audio_dsp_controller_init(void);
void hal_audio_dsp_controller_deinit(void);

void hal_audio_dsp_controller_send_message(uint16_t message, uint16_t data16, uint32_t data32, bool wait);
void *hal_audio_dsp_controller_put_paramter(const void *p_param_addr, uint32_t param_size, audio_message_type_t msg_type);
void hal_audio_dsp_auout_clkmux_control(hal_audio_device_t out_device, uint32_t device_rate, bool isEnable);

void hal_audio_dsp_internalbus_clkmux_control(uint32_t device_rate, bool isEnable);
void hal_audio_dsp_hwsrc_clkmux_control(uint32_t device_rate, bool isEnable);
void hal_audio_dsp_dl_clkmux_control(audio_message_type_t type, hal_audio_device_t out_device, uint32_t device_rate, bool isEnable);
void hal_audio_dsp_ul_clkmux_control(hal_audio_device_t out_device, uint32_t device_rate, bool isEnable);

void hal_audio_set_task_notification_callback(hal_audio_notify_task_callback_t callback);
void hal_audio_dsp_message_process(void);
void hal_audio_set_task_ms_delay_function(hal_audio_task_ms_delay_function_t delay_func);
extern uint32_t hal_audio_dsp2mcu_data_get(void);
uint32_t hal_audio_dsp2mcu_AUDIO_DL_ACL_data_get(void);

#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
/**
 * @brief     Get stream in device, MemInterface, i2s_interface from static HW I/O config table.
 * @param[out] Device is a pointer to the audio Device.
 * @param[out] MemInterface is a pointer to the audio MemInterface.
 * @param[out] i2s_interface is a pointer to the audio i2s_interface.
 * @return     #HAL_AUDIO_STATUS_OK, if OK. #HAL_AUDIO_STATUS_ERROR, if wrong.
 */
hal_audio_status_t hal_audio_get_stream_in_setting_config(audio_scenario_sel_t Audio_or_Voice, hal_audio_device_t *Device, hal_audio_channel_selection_t *MemInterface, hal_audio_interface_t *i2s_interface);

/**
 * @brief     Get stream out device, MemInterface, i2s_interface from static HW I/O config table.
 * @param[out] Device is a pointer to the audio Device.
 * @param[out] MemInterface is a pointer to the audio MemInterface.
 * @param[out] i2s_interface is a pointer to the audio i2s_interface.
 * @return     #HAL_AUDIO_STATUS_OK, if OK. #HAL_AUDIO_STATUS_ERROR, if wrong.
 */
hal_audio_status_t hal_audio_get_stream_out_setting_config(audio_scenario_sel_t Audio_or_Voice, hal_audio_device_t *Device, hal_audio_channel_selection_t *MemInterface, hal_audio_interface_t *i2s_interface);
#endif /* #ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT */

//== Audio Service ==
void hal_audio_service_hook_callback(audio_message_type_t type, hal_audio_callback_t callback, void *user_data);
void hal_audio_service_unhook_callback(audio_message_type_t type);

//== Share buffer ==
n9_dsp_share_info_t *hal_audio_query_bt_audio_dl_share_info(void);
n9_dsp_share_info_t *hal_audio_query_bt_voice_ul_share_info(void);
n9_dsp_share_info_t *hal_audio_query_bt_voice_dl_share_info(void);
n9_dsp_share_info_t *hal_audio_query_ble_audio_ul_share_info(void);
n9_dsp_share_info_t *hal_audio_query_ble_audio_dl_share_info(void);
n9_dsp_share_info_t *hal_audio_query_playback_share_info(void);
n9_dsp_share_info_t *hal_audio_query_record_share_info(void);
uint32_t *hal_audio_query_rcdc_share_info(void);
uint32_t *hal_audio_query_hfp_air_dump(void);
n9_dsp_share_info_t *hal_audio_query_prompt_share_info(void);
n9_dsp_share_info_t *hal_audio_query_nvkey_parameter_share_info(void);
void hal_audio_reset_share_info(n9_dsp_share_info_t *p_info);
void hal_audio_a2dp_reset_share_info(n9_dsp_share_info_t *p_info);
void hal_audio_set_sysram(void);
uint32_t *hal_audio_query_ltcs_asi_buf(void);
uint32_t *hal_audio_query_ltcs_min_gap_buf(void);
uint32_t *hal_audio_report_bitrate_buf(void);
uint32_t *hal_audio_report_lostnum_buf(void);
hal_audio_status_t hal_audio_write_audio_drift_val(int32_t val);
hal_audio_status_t hal_audio_write_audio_anchor_clk(uint32_t val);
hal_audio_status_t hal_audio_write_audio_asi_base(uint32_t val);
hal_audio_status_t hal_audio_write_audio_asi_cur(uint32_t val);

//== Buffer management related ==
uint32_t hal_audio_buf_mgm_get_data_byte_count(n9_dsp_share_info_t *p_info);
uint32_t hal_audio_buf_mgm_get_free_byte_count(n9_dsp_share_info_t *p_info);
void hal_audio_buf_mgm_get_free_buffer(
    n9_dsp_share_info_t *p_info,
    uint8_t **pp_buffer,
    uint32_t *p_byte_count);
void hal_audio_buf_mgm_get_data_buffer(
    n9_dsp_share_info_t *p_info,
    uint8_t **pp_buffer,
    uint32_t *p_byte_count);
void hal_audio_buf_mgm_get_write_data_done(n9_dsp_share_info_t *p_info, uint32_t byte_count);
void hal_audio_buf_mgm_get_read_data_done(n9_dsp_share_info_t *p_info, uint32_t byte_count);

//== Status control ==
void hal_audio_status_set_running_flag(audio_message_type_t type, bool is_running);
void hal_audio_status_set_notify_flag(audio_message_type_t type, bool is_notify);
bool hal_audio_status_query_running_flag(audio_message_type_t type);
bool hal_audio_status_query_notify_flag(audio_message_type_t type);

//== Data path ==
hal_audio_status_t hal_audio_write_stream_out_by_type(audio_message_type_t type, const void *buffer, uint32_t size);

//== AM A2DP open callback ==
void hal_audio_am_register_a2dp_open_callback(hal_bt_audio_dl_open_callback_t callback);

//== Speech related parameter ==
void speech_update_common(const uint16_t *common);
void speech_update_nb_param(const uint16_t *param);
void speech_update_wb_param(const uint16_t *param);
void speech_update_nb_fir(const int16_t *in_coeff, const int16_t *out_coeff);
void speech_update_wb_fir(const int16_t *in_coeff, const int16_t *out_coeff);

int32_t audio_update_iir_design(const uint32_t *parameter);

//== Ring buffer operation ==
uint32_t ring_buffer_get_data_byte_count(ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count(ring_buffer_information_t *p_info);
void ring_buffer_get_write_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_get_read_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_write_done(ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done(ring_buffer_information_t *p_info, uint32_t read_byte_count);

uint32_t ring_buffer_get_data_byte_count_non_mirroring(ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count_non_mirroring(ring_buffer_information_t *p_info);
void ring_buffer_get_write_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_get_read_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_write_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t read_byte_count);

//== Time report ==
audio_dsp_a2dp_dl_time_param_t *hal_audio_a2dp_dl_get_time_report(void);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef __HAL_AUDIO_INTERNAL_H__ */
