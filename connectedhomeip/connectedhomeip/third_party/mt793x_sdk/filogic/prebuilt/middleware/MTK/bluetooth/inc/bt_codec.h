/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __BT_CODEC_H__
#define __BT_CODEC_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothCommon Common
 * @{
 * @addtogroup BluetoothCommon_4 Codec
 * @{
 * This section introduces the Bluetooth codec APIs including terms and acronyms,
 * supported features, details on how to use the codec, function groups, enums, structures and functions.
 *
 * The Bluetooth codec is used for A2DP and HFP profiles.
 *
 * @section BT_CODEC_Terms_Chapter Terms and acronyms
 *
 * |Terms               |Details                                                                                                                                                 |
 * |--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |\b A2DP             | The Advanced Audio Distribution Profile (A2DP) defines the protocols and procedures that implement distribution of audio content of high-quality in mono or stereo on ACL channels. |
 * |\b HFP              | The Hands-free Profile (HFP) defines the device acting as the audio gateway's remote audio input and output mechanism. |
 * |\b AAC              | Advanced Audio Coding (AAC) is an audio coding standard for lossy digital audio compression. |
 * |\b SBC              | The Low Complexity Subband Coding (SBC) is an audio subband codec specified by the Bluetooth Special Interest Group (SIG) for the Advanced Audio Distribution Profile (A2DP). SBC is a digital audio encoder and decoder used to transfer data to Bluetooth audio output devices like headphones or loudspeakers. |
 * |\b VBR              | Variable bitrate (VBR) is a term used in telecommunications and computing that relates to the bitrate used in sound or video encoding. |
 *
 * @section bt_codec_api_usage How to use this module
 *
 * - The A2DP sink codec
 *   - Open and close the A2DP sink codec using #bt_codec_a2dp_open() and #bt_codec_a2dp_close().
 *   - The audio streaming data can be filled through a shared buffer (Method 1) or through a callback function (Method 2).
 *    - Sample code:
 *     @code
 *       bt_media_handle_t  handle;
 *
 *       void bt_audio_callback(bt_media_handle_t *handle, bt_codec_media_event_t event)
 *       {
 *           if (event == BT_CODEC_MEDIA_ERROR ) {
 *               // Error handling, such as notify the Bluetooth module or stop the A2DP codec.
 *           }
 *       }
 *
 *      int32_t bt_sink_codec_get_data_callback(volatile uint16_t *buf, uint32_t len);
 *
 *       void bt_audio_sink_sbc_open(void)
 *       {
 *           bt_codec_a2dp_audio_t sbc_sink_config;
 *           bt_codec_sbc_t   sbc_codec_parameter;
 *           uint8_t* ring_buf =  NULL;
 *
 *           // Configure the SBC codec capability parameters.
 *           sbc_codec_parameter.min_bit_pool = 2;
 *           sbc_codec_parameter.max_bit_pool = 75;
 *           sbc_codec_parameter.block_length = 0xf;
 *           sbc_codec_parameter.subband_num  = 0xf;
 *           sbc_codec_parameter.alloc_method = 0x3;
 *           sbc_codec_parameter.sample_rate  = 0xf;
 *           sbc_codec_parameter.channel_mode = 0xf;
 *
 *           sbc_sink_config.role                = BT_A2DP_SINK;
 *           sbc_sink_config.codec_cap.type      = BT_A2DP_CODEC_SBC;
 *           sbc_sink_config.codec_cap.codec.sbc = sbc_codec_parameter;
 *
 *           // Open the A2DP codec.
 *           handle = bt_codec_a2dp_open(bt_audio_callback, &sbc_sink_config);
 *           if (handle == NULL) {
 *               // Error handling, failed to open the codec.
 *           }
 *
 *           // Fill the audio streaming data through shared buffer (Method 1) or through callback function (Method 2).
 *           // Method 1: Set a shared buffer.
 *           // Apply a user-defined buffer to the codec with the ring buffer size (BT_RING_BUFFER_SIZE).
 *           ring_buf = (uint8_t *)malloc(BT_RING_BUFFER_SIZE);
 *           handle->set_buffer(handle, (uint8_t*)ring_buf, BT_RING_BUFFER_SIZE);
 *
 *           // Method 2: Set callback function to get data.
 *           handle->set_get_data_function(handle,bt_sink_codec_get_data_callback);
 *
 *           // Start the A2DP codec.
 *           handle->play(handle);
 *       }
 *
 *       void bt_audio_sink_sbc_close(void)
 *       {
 *           handle->stop(handle);
 *           bt_codec_a2dp_close(handle);
 *       }
 *
 *      // Fill the audio streaming data through a shared buffer (Method 1).
 *       void bt_a2dp_sink_fill_data(void)
 *       {
 *           uint32_t ring_buffer_free_space = 0, available_size = 0, data_length = 0;
 *           uint8_t *data_buffer = 0;
 *
 *           // Data length and data management at Bluetooth driver itself.
 *
 *           ring_buffer_free_space = handle->get_free_space(handle);
 *           // Get the current length of buffer to write the data.
 *           handle->get_write_buffer(handle, &data_buffer, &available_size);
 *
 *           // Fill the buffer #data_buffer with a proper amount of data depending on ring_buffer_free_space, available_size and data_length.
 *           handle->write_data_done(handle, data_length);
 *       }
 *
 *
 *       // Fill the audio streaming data using a callback function (Method 2).
 *       int32_t bt_sink_codec_get_data_callback(volatile uint16_t *buf, uint32_t len)
 *      {
 *            // The user can fill audio streaming data into buf. The length of audio streaming data must be less than or equal to len.
 *      }
 *
 *     @endcode
 * - The HFP codec
 *   - Open and close the HFP codec using #bt_codec_hfp_open() and #bt_codec_hfp_close().
 *    - Sample code:
 *     @code
 *       bt_media_handle_t handle;
 *
 *       void bt_audio_codec_hfp_callback(bt_media_handle_t *handle, bt_codec_media_event_t event)
 *       {
 *           if (event == BT_CODEC_MEDIA_ERROR ) {
 *               // Error handling, such as notify the Bluetooth module or stop the HFP codec.
 *           }
 *       }
 *
 *       void bt_audio_codec_hfp_start(void)
 *       {
 *           bt_codec_hfp_audio_t hfp_config;
 *           bt_codec_media_status_t status;
 *           hfp_config.type = BT_HFP_CODEC_TYPE_CVSD;
 *           // Open the HFP codec.
 *           handle = bt_codec_hfp_open(bt_audio_codec_hfp_callback, &hfp_config);
 *           if (handle == NULL) {
 *               // Error handling, failed to open the codec.
 *           }
 *           // Start the HFP codec.
 *           status = handle->play(handle);
 *           if (status != BT_CODEC_MEDIA_STATUS_OK) {
 *               // Error handling, failed to play the media.
 *           }
 *       }
 *
 *       void bt_audio_codec_hfp_stop(void)
 *       {
 *           bt_codec_media_status_t status;
 *           status = handle->stop(handle);
 *           if (status != BT_CODEC_MEDIA_STATUS_OK) {
 *               // Error handling, failed to stop the codec.
 *           }
 *           status = bt_codec_hfp_close(handle);
 *           if (status != BT_CODEC_MEDIA_STATUS_OK) {
 *               // Error handling, failed to close the codec.
 *           }
 *       }
 *     @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "bt_hfp.h"
#include "bt_a2dp.h"

/**
 * @defgroup Bluetoothbt_codec_media_events_define Define
 * @{
 */

/** @brief Defines the codec media event. */
#define BT_CODEC_MEDIA_EVENT_START    (120)     /**< The start of the Media events. */

/** @brief Bluetooth codec state. */
typedef enum {
    BT_CODEC_STATE_IDLE,      /**< The Bluetooth codec is inactive. */
    BT_CODEC_STATE_READY,     /**< The Bluetooth codec is ready to play the media. */
    BT_CODEC_STATE_PLAY,      /**< The Bluetooth codec is in a playing state. */
    BT_CODEC_STATE_ERROR,     /**< The Bluetooth codec reported an error. */
    BT_CODEC_STATE_STOP       /**< The Bluetooth codec has stopped. */
} bt_codec_state_t;


/** @brief This structure defines the Bluetooth codec media events. */
typedef enum {
    BT_CODEC_MEDIA_ERROR = BT_CODEC_MEDIA_EVENT_START,          /**< Failed, due to a bitstream error. */
    BT_CODEC_MEDIA_OK,                                    /**< Successful. */
    BT_CODEC_MEDIA_UNDERFLOW,                             /**< The bitstreaming is underflow and the codec output is on silent. */
    BT_CODEC_MEDIA_REQUEST,                               /**< Bitstreaming request from the user. */
    BT_CODEC_MEDIA_DATA_NOTIFICATION,                     /**< Bitstreaming notification to the user. */
    BT_CODEC_MEDIA_TERMINATED,                            /**< The Bluetooth codec is terminated. */
    BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW,                   /**< Advanced wireless stereo underflow-checking mechanism is triggered. */
    BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW,                  /**< Advanced wireless stereo clock-skew-checking mechanism is triggered. */
    BT_CODEC_MEDIA_GET_PAYLOAD,                           /**< A2dp source notification for payload. */
    BT_CODEC_MEDIA_TIME_REPORT,                           /**< Report time information for advanced wireless stereo */
    BT_CODEC_MEDIA_LTCS_DATA_REPORT,                      /**< Report LTCS data */
    BT_CODEC_MEDIA_AUDIO_DL_REINIT_REQUEST,               /**< Request to re-init downlink path for BT audio sync. */
    BT_CODEC_MEDIA_AUDIO_DL_ALC_REQUEST,                  /**< Request to active latency control. */
    BT_CODEC_MEDIA_HFP_AVC_PARA_SEND,                     /**< Request to send AVC_vol from agent to partner. */
} bt_codec_media_event_t;


/** @brief This structure defines the Bluetooth codec API return status. */
typedef enum {
    BT_CODEC_MEDIA_STATUS_BASE = -0x1B00,                     /**< The base of the status. */
    BT_CODEC_MEDIA_STATUS_OK = 0,                             /**< Successful. */
    BT_CODEC_MEDIA_STATUS_ERROR = -0x1B01,                    /**< Error. */
    BT_CODEC_MEDIA_STATUS_INVALID_PARAM = -0x1B02,            /**< Invalid parameters. */
    BT_CODEC_MEDIA_STATUS_END                                 /**< The end of the status. */
} bt_codec_media_status_t;

/** @brief Bluetooth codec AWS clock skew status. */
typedef enum {
    BT_CODEC_AWS_CLOCK_SKEW_STATUS_IDLE,    /**< The Bluetooth codec AWS clock skew is inactive. */
    BT_CODEC_AWS_CLOCK_SKEW_STATUS_BUSY     /**< The Bluetooth codec AWS clock skew is compensating. */
} bt_codec_aws_clock_skew_status_t;

/** @brief Tis is hfp stream out mixer method struct. */
typedef enum {
    BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_NO_MIX = 0,      /**< Maintain original stream out. */
    BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_REPLACE,         /**< Replace original stream out. */
    BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_WEIGHTED_MEAN    /**< Weighted mean. */
} bt_codec_media_hfp_mix_stream_out_method_t;


/**
 * @}
 */


/** @defgroup bt_codec_enum Enum
  * @{
  */

/** @brief Bluetooth codec HFP device path. */
typedef enum {
    BT_CODEC_HFP_DEVICE_PATH_INTERNAL = 0,                /**< The stream out and stream in path of HFP is to/from internal ABB. */
    BT_CODEC_HFP_DEVICE_PATH_MASTER_I2S,                  /**< The stream out and stream in path of HFP is from MASTER I2S, note stream out also can be to internal ABB. */
    BT_CODEC_HFP_DEVICE_PATH_SLAVE_I2S,                   /**< Not supported. The stream out and stream in path of HFP is from SLAVE I2S. */
} bt_codec_hfp_device_path_t;

/**
 * @}
 */


/** @defgroup Bluetoothbt_codec_struct Struct
  * @{
  */

/** @brief This structure defines the shared buffer structure. */
typedef struct {
    uint8_t         *buffer_base;         /**< Pointer to the ring buffer. */
    int32_t         buffer_size;          /**< Size of the ring buffer. */
    int32_t         write;                /**< Index of the ring buffer to write the data. */
    int32_t         read;                 /**< Index of the ring buffer to read the data. */
    void            *param;               /**< Extended parameter or information. */
} bt_audio_buffer_t;

/** @brief A2DP direct access to DSP buffer function type. */
typedef int32_t (*bt_codec_get_data)(volatile uint16_t *buf, uint32_t len);

/** @brief A2DP get data count function type. */
typedef int32_t (*bt_codec_get_data_count)(void);

/** @brief A2DP access to share buffer function type.*/
typedef int32_t (*bt_codec_get_data_in_byte)(volatile uint8_t *buf, uint32_t len);

/** @brief A2DP audio handle structure type. */
typedef struct _bt_media_handle_t {
    bt_codec_state_t      state;          /**< The Bluetooth codec state. */
    bt_audio_buffer_t     buffer_info;    /**< The shared buffer information for the Bluetooth codec. */
    bool                  underflow;      /**< The Bluetooth codec data underflow occurred. */
    bool                  waiting;        /**< The Bluetooth codec is waiting to fill data. */
    uint16_t              audio_id;       /**< The audio ID of the Bluetooth codec. */

    void (*handler)(struct _bt_media_handle_t *handle, bt_codec_media_event_t event_id);                           /**< The Bluetooth codec handler. */
    void (*set_buffer)(struct _bt_media_handle_t *handle, uint8_t  *buffer, uint32_t  length);                     /**< Set the shared buffer for bitstream. The audio streaming data can be filled through a shared buffer or through a callback function. */
    void (*set_get_data_function)(struct _bt_media_handle_t *handle, bt_codec_get_data func);                      /**< Set the callback function for bitstream. The audio streaming data can be filled through a shared buffer or through a callback function. */
    void (*set_get_data_in_byte_function)(struct _bt_media_handle_t *handle, bt_codec_get_data_in_byte func);      /**< Set the callback function for bitstream. The audio streaming data can be filled through a shared buffer or through a callback function.*/
    void (*set_get_data_count_function)(struct _bt_media_handle_t *handle, bt_codec_get_data_count func);          /**< Set the callback function for bitstream. The amount of streaming data can be queried. */
    void (*get_write_buffer)(struct _bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length);               /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(struct _bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length);                /**< Get the available length to read from a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(struct _bt_media_handle_t *handle, uint32_t length);                                   /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(struct _bt_media_handle_t *handle);                                                  /**< Indicate the last data transfer. */
    void (*reset_share_buffer)(struct _bt_media_handle_t *handle);                                                 /**< Reset the shared buffer's information. */
    void (*read_data_done)(struct _bt_media_handle_t *handle, uint32_t length);                                    /**< Update the read pointer to the shared buffer. */
    int32_t (*get_free_space)(struct _bt_media_handle_t *handle);                                                  /**< Get the free space length available in the shared buffer. */
    int32_t (*get_data_count)(struct _bt_media_handle_t *handle);                                                  /**< Get the available data amount of the shared buffer. */
    bt_codec_get_data directly_access_dsp_function;                                                                /**< Set function to access DSP buffer directly. */
    bt_codec_get_data_count get_data_count_function;                                                               /**< Set function to get available data amount of the uplayer total buffer size. */
    bt_codec_get_data_in_byte access_share_buffer_function;                                                        /**< Set function to access share buffer. */

    uint32_t (*query_memory_size)(struct _bt_media_handle_t *handle);                                                           /**< Query A2DP source total memory usage.*/
    uint32_t (*set_bit_rate)(struct _bt_media_handle_t *handle, uint32_t bit_rate);                                             /**< For A2DP source runtime change bit rate.*/
    uint32_t (*get_payload)(struct _bt_media_handle_t *handle, uint8_t *buffer, uint32_t buffer_size, uint32_t *sample_count);  /**< Get payload for A2DP source.*/
    void     (*get_payload_done)(struct _bt_media_handle_t *handle);                                                            /**< Prepare for next BT_CODEC_MEDIA_GET_PAYLOAD notification.*/
    void     (*query_payload_size)(struct _bt_media_handle_t *handle, uint32_t *minimum, uint32_t *total);                      /**< Get minimum payload size and current total payload size .*/

    bt_codec_media_status_t (*play)(struct _bt_media_handle_t *handle);                                            /**< The Bluetooth codec play function. */
    bt_codec_media_status_t (*process)(struct _bt_media_handle_t *handle, bt_codec_media_event_t event_id);        /**< The Bluetooth codec process function. */
    bt_codec_media_status_t (*stop)(struct _bt_media_handle_t *handle);                                            /**< The Bluetooth codec stop function. */

    void (*set_aws_flag)(struct _bt_media_handle_t *handle, bool is_aws);                              /**< Set the information of AWS flag. */
    uint32_t (*get_ts_ratio)(struct _bt_media_handle_t *handle, uint32_t ts0, uint32_t ts1);           /**< Get the ratio of time stamp and sample. */
    void (*set_ts_ratio)(struct _bt_media_handle_t *handle, uint32_t ts_ratio);                        /**< Set the ratio of time stamp and sample. */
    uint32_t (*get_sampling_rate)(struct _bt_media_handle_t *handle);                                  /**< Get the sampling rate for audio sync. */

    void (*set_start_time_stamp)(struct _bt_media_handle_t *handle, uint32_t time_stamp);              /**< Set the start time for initial sync. */
    void (*set_content_protection)(struct _bt_media_handle_t *handle, bool is_cp);                     /**< Set the information of content protection */
    void (*set_special_devicce)(struct _bt_media_handle_t *handle, bool is_special);                   /**< set special device flag for walkman NW-A45  */
    uint32_t (*get_max_frame_decoding_time)(struct _bt_media_handle_t *handle);                        /**< Get the max frame decoding time (unit is 0.1 ms)  */

    void (*trigger_mic)(struct _bt_media_handle_t *handle);                                             /**< Trigger Mic On. */
    void (*side_tone_enable)(struct _bt_media_handle_t *handle);                                        /**< Enable side tone feature. */
    void (*side_tone_disable)(struct _bt_media_handle_t *handle);                                       /**< Disable side tone feature. */
} bt_media_handle_t;


/**
 *  @brief This structure defines the SBC codec details.
 */
typedef struct {
    uint8_t min_bit_pool;       /**< The minimum bit pool. */
    uint8_t max_bit_pool;       /**< The maximum bit pool. */
    uint8_t block_length;       /**< b0: 16, b1: 12, b2: 8, b3: 4. */
    uint8_t subband_num;        /**< b0: 8, b1: 4. */
    uint8_t alloc_method;       /**< b0: loudness, b1: SNR. */
    uint8_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint8_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} bt_codec_sbc_t;

/**
 *  @brief This structure defines the AAC codec details.
 */
typedef struct {
    bool vbr;                   /**< Indicates if VBR is supported or not. */
    uint8_t object_type;        /**< b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC. */
    uint8_t channels;           /**< b0: 2, b1: 1. */
    uint16_t sample_rate;       /**< b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000. */
    uint32_t bit_rate;          /**< Constant/peak bits per second in 23 bit UiMsbf. A value of 0 indicates that the bit rate is unknown. */
} bt_codec_aac_t;

/**
 *  @brief This structure defines the VENDOR codec details.
 */
typedef struct {
    bool vbr;                   /**< Indicates if VBR is supported or not. */
    uint8_t object_type;        /**< b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC. */
    uint8_t channels;           /**< b0: 2(stereo), b1: (dual_channel), b0: 1(mono). */
    uint8_t sample_rate;        /**< b0~b5: 192000,176400,96000,882000,48000,44100. */
    uint32_t bit_rate;          /**< Constant/peak bits per second in 23 bit UiMsbf. A value of 0 indicates that the bit rate is unknown. */
} bt_codec_vendor_t;

/**
 *  @brief This union defines the A2DP codec.
 */
typedef union {
    bt_codec_sbc_t sbc;    /**< SBC codec. */
    bt_codec_aac_t aac;    /**< AAC codec. */
    bt_codec_vendor_t vendor;  /**< VENDOR codec. */
} bt_codec_t;

/**
 *  @brief This structure defines the A2DP codec capability.
 */
typedef struct {
    bt_a2dp_codec_type_t type;  /**< Codec type. */
    bt_codec_t codec;      /**< Codec information. */
} bt_codec_capability_t;


/** @brief This structure defines the A2DP codec. */
typedef struct {
    bt_codec_capability_t codec_cap;  /**< The capabilities of Bluetooth codec. */
    bt_a2dp_role_t             role;       /**< The Bluetooth codec roles. */
} bt_codec_a2dp_audio_t;

/** @brief This structure defines the HFP codec. */
typedef struct {
    bt_hfp_audio_codec_type_t type;   /**< 1: CVSD, 2: mSBC. */
} bt_codec_hfp_audio_t;

#ifdef MTK_BT_CODEC_BLE_ENABLED
/**
 * @brief This structure defines the Le Audio codec type.
 */
typedef uint8_t bt_codec_type_le_audio_t;
#define BT_CODEC_TYPE_LE_AUDIO_NONE  (0x00)               /**<  None. */
#define BT_CODEC_TYPE_LE_AUDIO_LC3   (0x01)               /**<  LC3. */


/** @brief This structure defines the LE Audio codec. */
typedef struct {
    bt_codec_type_le_audio_t  codec;    /**< Codec type. */
    uint8_t  sample_rate;               /**< Sample rate, unit: Hz. */
    uint8_t  ul_sample_rate;            /**< Sample rate, unit: Hz. */
    uint8_t  channel_num;               /**< 0: mono, 1: stereo */
    uint8_t  ul_channel_num;            /**< 0: mono, 1: stereo */
    uint16_t frame_payload_length;      /**< frame length. unit: byte. */
    uint16_t ul_frame_payload_length;   /**< frame length. unit: byte. */
    uint32_t context_type;              /**<configuration between gaming/call. */
    uint32_t ul_context_type;           /**<configuration between gaming/call. */
    uint8_t  channel_mode;              /**< 0: downlink only, 1: downlink and uplink. */
    uint8_t  frame_duration;            /**< frame duration. 0: 7.5ms, 1: 10ms. */
    uint32_t  presentation_delay;        /**<presenation delay, unite:us*/
} bt_codec_le_audio_t;
#endif
/** @brief The A2DP audio codec data count structure. */
typedef struct {
    uint32_t sample_count;  /**< One pair of L / R counts as one sample. */
    uint32_t sampling_rate; /**< Sampling rate, unit: Hz. */
} bt_codec_a2dp_data_count_t;

/** @brief The A2DP audio codec bitstream information structure. */
typedef struct {
    uint32_t sample_count;  /**< One pair of L / R counts as one sample. */
    uint32_t byte_count;    /**< Byte count. */
} bt_codec_a2dp_bitstream_t;

/** @brief The A2DP audio codec buffer information structure. */
typedef struct {
    uint8_t *buffer;        /**< Buffer pointer. */
    uint32_t byte_count;    /**< Buffer byte count. */
} bt_codec_a2dp_buffer_t;

/** @brief  This defines the A2DP codec callback function prototype.
 *          The user should register a callback function while opening the A2DP codec. Please refer to #bt_codec_a2dp_open().
 *  @param[in] handle is the media handle of the A2DP codec.
 *  @param[in] event_id is the value defined in #bt_msg_type_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*bt_codec_a2dp_callback_t)(bt_media_handle_t *handle, bt_codec_media_event_t event_id);


/**
 *  @brief  This defines the HFP codec HFP mix stream out callback function prototype.
 *            The user should register a callback function while after opened a HFP codec. Please refer to #bt_codec_hfp_open().
 *  @param[in] handle is the media HFP handle of the HFP codec.
 *  @param[in] user_data is a user defined parameter provided by #bt_codec_hfp_mix_stream_out_data_callback_t() function.
 */
typedef void (*bt_codec_hfp_mix_stream_out_data_callback_t)(bt_codec_media_event_t event, void *user_data);


/**
 *  @brief  This defines the HFP codec HFP mix stream in callback function prototype.
 *            The user should register a callback function while after opened a HFP codec. Please refer to #bt_codec_hfp_open().
 *  @param[in] handle is the media HFP handle of the HFP codec.
 *  @param[in] user_data is a user defined parameter provided by #bt_codec_hfp_get_stream_in_data_callback_t() function.
 */
typedef void (*bt_codec_hfp_get_stream_in_data_callback_t)(bt_codec_media_event_t event, void *user_data);
/**
 *  @brief  This defines the HFP codec hfp mix stream out callback function prototype.
 *            The user should register a callback function while after opened a HFP codec. Please refer to #bt_codec_hfp_open().
 *  @param[in] handle is the media hfp handle of the HFP codec.
 *  @param[in] user_data is a user defined parameter provided by #bt_codec_hfp_get_stream_out_data_callback_t() function.
 */
typedef void (*bt_codec_hfp_get_stream_out_data_callback_t)(bt_codec_media_event_t event, void *user_data);

/**
 *  @brief  This defines the HFP codec hfp customer enhancement callback function prototype.
 *            The user should register a callback function while after opened a HFP codec. Please refer to #bt_codec_hfp_open().
 *  @param[in] user_data is a user defined parameter provided by #bt_codec_hfp_customer_enhancement_function_callback_t() function.
 */
typedef void (*bt_codec_hfp_customer_enhancement_function_callback_t)(void *user_data);

/** @brief  This defines the HFP codec callback function prototype.
 *          The user should register a callback function while opening the HFP codec. Please refer to #bt_codec_hfp_open().
 *  @param[in] handle is the media handle of the HFP codec.
 *  @param[in] event_id is the value defined in #bt_msg_type_t. This parameter is given by the driver to notify the user of data flow processing behavior.
 */
typedef void (*bt_codec_hfp_callback_t)(bt_media_handle_t *handle, bt_codec_media_event_t event_id);
#ifdef MTK_BT_CODEC_BLE_ENABLED
/** @brief  This type defines the LE Audio codec callback function prototype.
 *          The user should register a callback function while opening the LE Audio codec. Please refer to #bt_codec_le_audio_open().
 *  @param[in] handle is the media handle of the LE Audio codec.
 *  @param[in] event_id is the value defined in #bt_msg_type_t. This parameter is given by the driver to notify the user of data flow processing behavior.
 */
typedef void (*bt_codec_le_audio_callback_t)(bt_media_handle_t *handle, bt_codec_media_event_t event_id);
#endif

/**
  * @}
  */

/**
 * @brief     This function opens the A2DP audio codec.
 * @param[in] callback is a pointer to the callback function for the A2DP data control.
 * @param[in] param is the settings of the A2DP codec.
 * @return    Media handle of the A2DP. Please refer to #bt_media_handle_t.
 */
bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t callback, const bt_codec_a2dp_audio_t *param);

/**
 * @brief     This function closes the A2DP audio codec.
 * @param[in] handle is the A2DP codec handle.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_close(bt_media_handle_t *handle);



/**
 * @brief     This function mixes a tone to the stream out tone.
 * @param[in] mix_method is to indicate which mix method will be used.
 * @param[in] *buffer is the tone buffer that will mix with stream out tone.
 * @param[in] sample_count is the size of *buffer that will mix with stream out tone (in bytes). It will equal or less than the sample_count that get from #bt_codec_hfp_get_mix_stream_out_sample_count().
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully.
 * @sa        #bt_codec_hfp_get_mix_stream_out_sample_count()
 */
bt_codec_media_status_t bt_codec_hfp_mix_stream_out(bt_codec_media_hfp_mix_stream_out_method_t mix_method, void *buffer, uint32_t sample_count);


/**
 * @brief      This function queries the sample_count of mix stream out buffer.
 * @param[out] sample_count is the size of the mix stream out buffer (in bytes).
 * @return     #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully.
 * @note       Call this function before #bt_codec_hfp_mix_stream_out() to find the maximum mix stream out buffer size.
 * @sa         #bt_codec_hfp_mix_stream_out()
 */
bt_codec_media_status_t bt_codec_hfp_get_mix_stream_out_sample_count(uint32_t *sample_count);



/**
 * @brief     This function unregisters the callback function for mix stream out data.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully.
 */
bt_codec_media_status_t bt_codec_hfp_unregister_mix_stream_out_data_callback(void);


/**
 * @brief     This function registers a callback function for mixing stream out data.
 * @param[in] callback is the function pointer of callback for mix stream out data control.
 * @param[in] user_data is a user defined parameter, user can use it if necessary.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully.
 */
bt_codec_media_status_t bt_codec_hfp_register_mix_stream_out_data_callback(bt_codec_hfp_mix_stream_out_data_callback_t callback, void *user_data);



/**
 * @brief     This function receives the streamed in data.
 * @param[in] buffer is a pointer to a user defined buffer.
 * @param[in] sample_count is the amount of data received (in bytes).
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if if the operation completed successfully.
 * @sa         #bt_codec_hfp_get_stream_in_sample_count()
 */
bt_codec_media_status_t bt_codec_hfp_read_stream_in(void *buffer, uint32_t sample_count);


/**
 * @brief      This function queries the sample_count of the streamed in buffer.
 * @param[out] sample_count is the size of stream in buffer (in bytes).
 * @return     #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully.
 * @note       Call this function before #bt_codec_hfp_read_stream_in() to know the maximum stream in buffer size.
 * @sa          #bt_codec_hfp_read_stream_in()
 */
bt_codec_media_status_t bt_codec_hfp_get_stream_in_sample_count(uint32_t *sample_count);

/**
 * @brief     This function unregisters the callback function that receives streamed in data.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully.
 */
bt_codec_media_status_t bt_codec_hfp_unregister_get_stream_in_data_callback(void);

/**
 * @brief     This function registers the callback function that receives streamed in data.
 * @param[in] callback is a pointer to the callback function that receives the streamed in data.
 * @param[in] user_data is a user defined parameter, user can use it if necessary.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully.
 */
bt_codec_media_status_t bt_codec_hfp_register_get_stream_in_data_callback(bt_codec_hfp_get_stream_in_data_callback_t callback, void *user_data);

/**
 * @brief     Recieve data from stream in.
 * @param[in] buffer is a pointer to a user defined buffer.
 * @param[in] sample_count is the amount of data received (in bytes).
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if OK.
 * @sa         #bt_codec_hfp_get_stream_in_sample_count()
 */
bt_codec_media_status_t bt_codec_hfp_read_stream_out(void *buffer, uint32_t sample_count);


/**
 * @brief      Query the sample_count of stream in buffer.
 * @param[out] sample_count is the size of stream in buffer (in bytes).
 * @return     #BT_CODEC_MEDIA_STATUS_OK, if OK.
 * @note       Call this function before #bt_codec_hfp_read_stream_in() to know the maximum stream in buffer.
 * @sa          #bt_codec_hfp_read_stream_in()
 */
bt_codec_media_status_t bt_codec_hfp_get_stream_out_sample_count(uint32_t *sample_count);

/**
 * @brief     Unregister the callback function for getting stream out data
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if OK.
 */
bt_codec_media_status_t bt_codec_hfp_unregister_get_stream_out_data_callback(void);

/**
 * @brief     Register the callback function for get stream out data
 * @param[in] callback is the function pointer of callback for getting stream out data.
 * @param[in] user_data is extended parameter for user.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if OK.
 */
bt_codec_media_status_t bt_codec_hfp_register_get_stream_out_data_callback(bt_codec_hfp_get_stream_out_data_callback_t callback, void *user_data);



/**
 * @brief     set enhanced stream in data stream in.
 * @param[in] buffer is a pointer to a user defined buffer.
 * @param[in] sample_count is the amount of enhanced data (in bytes). It will equal or less than the sample_count that get from #bt_codec_hfp_get_customer_enhanced_stream_in_sample_count().
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if OK.
 * @sa         #bt_codec_hfp_get_customer_enhanced_stream_in_sample_count()
 */
bt_codec_media_status_t bt_codec_hfp_set_customer_enhanced_stream_in_data(void *buffer, uint32_t sample_count);


/**
 * @brief      Query the sample count of enhanced stream in buffer that customer need feedback.
 * @param[out] sample_count is the size of enhanced stream in buffer (in bytes).
 * @return     #BT_CODEC_MEDIA_STATUS_OK, if OK.
 * @note       Call this function before #bt_codec_hfp_set_customer_enhanced_stream_in_data() to know the maximum stream in buffer.
 * @sa          #bt_codec_hfp_set_customer_enhanced_stream_in_data()
 */
bt_codec_media_status_t bt_codec_hfp_get_customer_enhanced_stream_in_sample_count(uint32_t *sample_count);



/**
 * @brief     Unregister customer enhancement callback function.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if OK.
 */
bt_codec_media_status_t bt_codec_hfp_unregister_customer_enhancement_function_callback(void);

/**
 * @brief     Register customer enhancement function.
 * @param[in] callback is the function pointer of customer enhancement function.
 * @param[in] user_data is extended parameter for user.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if OK.
 */
bt_codec_media_status_t bt_codec_hfp_register_customer_enhancement_function_callback(bt_codec_hfp_customer_enhancement_function_callback_t callback, void *user_data);



/**
 * @brief     This function sets the HFP device path.
 * @param[in] device_path is the HFP device path.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_hfp_set_device_path(bt_codec_hfp_device_path_t device_path);

/**
 * @brief     This function opens the HFP audio codec.
 * @param[in] callback is a pointer to the callback function for HFP error handling.
 * @param[in] param is the HFP codec settings.
 * @return    Media handle of the HFP.
 */
bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_codec_hfp_audio_t *param);

/**
 * @brief     This function closes the HFP codec.
 * @param[in] handle is the HFP codec handle.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully, #BT_CODEC_MEDIA_STATUS_ERROR, if an error occurred.
 */
bt_codec_media_status_t bt_codec_hfp_close(bt_media_handle_t *handle);

#ifdef MTK_BT_CODEC_BLE_ENABLED
/**
 * @brief     This function opens the LE Audio audio codec.
 * @param[in] callback is a pointer to the callback function for BLE error handling.
 * @param[in] param is the LE Audio codec settings.
 * @return    Media handle of the BLE.
 */
bt_media_handle_t *bt_codec_le_audio_open(bt_codec_le_audio_callback_t callback, const bt_codec_le_audio_t *param);

/**
 * @brief     This function closes the LE Audio codec.
 * @param[in] handle is the LE Audio codec handle.
 * @return    #BT_CODEC_MEDIA_STATUS_OK, if the operation completed successfully, #BT_CODEC_MEDIA_STATUS_ERROR, if an error occurred.
 */
bt_codec_media_status_t bt_codec_le_audio_close(bt_media_handle_t *handle);
#endif
/**
 * @brief     This function sets the advanced wireless stereo flag.
 * @param[in] handle is the A2DP codec handle.
 * @param[in] flag is used to determine to turn on or turn off AWS mechanism. 1: turn on AWS, 0: turn off AWS.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_set_flag(bt_media_handle_t *handle, bool flag);

/**
 * @brief     This function sets the advanced wireless stereo initial synchronization.
 * @param[in] handle is the A2DP codec handle.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_set_initial_sync(bt_media_handle_t *handle);

/**
 * @brief     This function gets the advanced wireless stereo consumed data count.
 * @param[in] handle is the A2DP codec handle.
 * @param[out] information points to the consumed data count information.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_get_consumed_data_count(bt_media_handle_t *handle, bt_codec_a2dp_data_count_t *information);

/**
 * @brief     This function gets the advanced wireless stereo silence frame information for one silence frame.
 * @param[in] handle is the A2DP codec handle.
 * @param[out] information points to the silence frame information for one silence frame.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_get_silence_frame_information(bt_media_handle_t *handle, bt_codec_a2dp_bitstream_t *information);

/**
 * @brief     This function fills the advanced wireless stereo silence frames to target data buffer.
 * @param[in] handle is the A2DP codec handle.
 * @param[out] data points to the target data buffer structure.
 * @param[in] frame_count is the silence frame count.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_fill_silence_frame(bt_media_handle_t *handle, bt_codec_a2dp_buffer_t *data, uint32_t frame_count);

/**
 * @brief     This function parses the advanced wireless stereo packet data information.
 * @param[in] handle is the A2DP codec handle.
 * @param[in] data points to the packet data buffer structure.
 * @param[out] information points to the packet data buffer information.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_parse_data_information(bt_media_handle_t *handle, bt_codec_a2dp_buffer_t *data, bt_codec_a2dp_bitstream_t *information);

/**
 * @brief     This function gets the advanced wireless stereo clock skew status.
 * @param[in] handle is the A2DP codec handle.
 * @param[out] status points to the clock skew status.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_get_clock_skew_status(bt_media_handle_t *handle, bt_codec_aws_clock_skew_status_t *status);

/**
 * @brief     This function sets the advanced wireless stereo clock skew compensation value.
 * @param[in] handle is the A2DP codec handle.
 * @param[in] sample_count is the audio sample count to be compensated. One pair of L / R counts as one audio sample.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_set_clock_skew_compensation_value(bt_media_handle_t *handle, int32_t sample_count);

/**
 * @brief     This function gets the advanced wireless stereo audio latency.
 * @param[in] handle is the A2DP codec handle.
 * @param[in] sampling_rate is the audio sampling rate in Hz.
 * @param[out] latency_us points to the audio latency in microsecond.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_get_audio_latency(bt_media_handle_t *handle, uint32_t sampling_rate, uint32_t *latency_us);


/**
 * @brief     This function sets the software AAC DECODER for Bluetooth A2DP.
 * @param[in] flag is used to determine whether the software AAC DECODER mechanism is turned on. true: Decode AAC by Cortex-M4, false: Decode AAC by DSP.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_set_sw_aac_flag(bool flag);

/**
 * @brief     This function gets the software AAC DECODER for Bluetooth A2DP.
 * @return    If the software AAC DECODER is enabled, the return value is true, otherwise the return value is false.
 */
bool bt_codec_a2dp_get_sw_aac_flag(void);

/**
 * @brief     This function sets the clock skew flag.
 * @param[in] flag is used to turn on or turn off clock skew. 1: turn on clock skew, 0: turn off clock skew.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
bt_codec_media_status_t bt_codec_a2dp_aws_set_clock_skew(bool flag);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
* @}
*/

#endif /* __BT_CODEC_H__ */

