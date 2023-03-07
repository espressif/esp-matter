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
/* MediaTek restricted information */

#ifndef __BT_A2DP_H__
#define __BT_A2DP_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothA2DP A2DP
 * @{
 * This section introduces the Advanced Audio Distribution Profile (A2DP) APIs including terms, supported features and details on how to use the APIs to set up a Bluetooth Audio instance.
 * The A2DP defines the protocol to implement distribution of high-quality audio content in mono or stereo on ACL channels.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SBC                        | The Low Complexity Subband Coding (SBC) is an audio subband codec specified by the Bluetooth Special Interest Group (SIG) for the Advanced Audio Distribution Profile (A2DP). SBC is a digital audio encoder and decoder used to transfer data to Bluetooth audio output devices like headphones or loudspeakers. For more information, please refer to <a href="https://en.wikipedia.org/wiki/SBC_(codec)">Wikipedia</a>.|
 * |\b AAC                        | Advanced Audio Coding (AAC) is an audio coding standard for lossy digital audio compression. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Audio_Coding">Wikipedia</a>.|
 * |\b SEID                       | Stream End Point Identifier (SEID) is a SEP identifier. |
 * |\b SEP                        | Stream End Point (SEP) is a concept to expose available Transport Services and AV capabilities of the Application in order to negotiate a stream. |
 * |\b SNK                        | Sink. A device is the SNK when it acts as a sink of a digital audio stream delivered from the SRC on the same piconet. |
 * |\b SRC                        | Source. A device is the SRC when it acts as a source of a digital audio stream that is delivered to the SNK of the piconet. |
 * |\b Piconet                    | A piconet is a computer network which links a wireless user group of devices using Bluetooth technology protocols. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Piconet">Wikipedia</a>.|
 *
 * @section bt_a2dp_api_usage How to use this module
 *
 * - The A2DP confirmation/indication notifies the upper layer that the process has finished or an indication is received.
 *   BT_A2DP_XXX_CNF represents the events that a procedure has finished.
 *   BT_A2DP_XXX_IND represents the events that an indication has been received.
 * - The A2DP confirmation/indication structures provide related information to the upper layer.
 *   The data is obtained from the events parameter.
 *
 *  - 1. Mandatory, implement #bt_a2dp_get_init_params() to set the initial configuration (device role, SEP number, SEP detail, etc). This is a user-defined API, which is invoked when the Bluetooth is powered on. It should be implemented by the application.
 *   - Sample code:
 *    @code
 *       bt_status_t bt_a2dp_get_init_params(bt_a2dp_init_params_t *params)
 *       {
 *           static bt_a2dp_codec_capability_t init_codec;
 *           // Fill codec infomation into init_codec.
 *
 *           params->codec_number = 1;
 *           params->codec_list = &init_codec;
 *
 *           return BT_STATUS_SUCCESS;
 *       }
 *    @endcode
 *  - 2. Mandatory, implement #bt_app_event_callback() to handle the A2DP events, such as
 *           connect, disconnect, streaming start indication, media data received, etc.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t msg_id, bt_status_t status, void *buff)
 *       {
 *           switch (msg_id)
 *           {
 *               case BT_A2DP_CONNECT_CNF:
 *               {
 *                   bt_a2dp_connect_cnf_t *params = (bt_a2dp_connect_cnf_t *)buff;
 *                   // Check connection result and save handle if connection is successful.
 *                   break;
 *               }
 *               case BT_A2DP_DISCONNECT_CNF:
 *               {
 *                   bt_a2dp_disconnect_cnf_t *params = (bt_a2dp_disconnect_cnf_t *)buff;
 *                   // Check disconnection result and clear the A2DP context.
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - 3. Optional, initiate a connection to a remote device, as shown in the figure titled as "The A2DP connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // A BT_A2DP_CONNECT_CNF event notifies the function bt_app_event_callback() once the device is connected.
 *       ret = bt_a2dp_connect(&a2dp_handle, address, role);
 *    @endcode
 *  - 4. Mandatory, similar to sink, handles the events in #bt_app_event_callback().
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t msg_id, bt_status_t status, void *buff)
 *       {
 *           switch (msg_id)
 *           {
 *               case BT_A2DP_START_STREAMING_IND:
 *               {
 *                   bt_a2dp_start_streaming_ind_t *params = (bt_a2dp_start_streaming_ind_t *)buff;
 *                   // Must accept the request by sending the response with either true or false.
 *                   bt_a2dp_start_streaming_response(params->handle, true);
 *                   // Open sink codec (initialize the audio driver).
 *                   handle = bt_codec_a2dp_open(callback, codec_a2dp);
 *                   break;
 *               }
 *               case BT_A2DP_SUSPEND_STREAMING_IND:
 *               {
 *                   bt_a2dp_suspend_streaming_ind_t *params = (bt_a2dp_suspend_streaming_ind_t *)buff;
 *                   // Must accept the request by sending the response with either true or false.
 *                   bt_a2dp_suspend_streaming_response(params->handle, true);
 *                   break;
 *               }
 *               case BT_A2DP_RECONFIGURE_IND:
 *               {
 *                   bt_a2dp_reconfigure_ind_t *params = (bt_a2dp_reconfigure_ind_t *)buff;
 *                   // Must accept the request by sending the response with either true or false.
 *                   bt_a2dp_reconfigure_response(params->handle, true);
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - 5. Mandatory, handle media event and write data to audio driver using #bt_app_event_callback().
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t msg_id, bt_status_t status, void *buff)
 *       {
 *           switch (msg_id)
 *           {
 *               case BT_A2DP_STREAMING_RECEIVED_IND:
 *               {
 *                   bt_a2dp_streaming_received_ind_t *params = (bt_a2dp_streaming_received_ind_t *)buff;
 *                   // Hold the received data node to prevent the node from being released before it is consumed.
 *                   // Remember that bt_a2dp_release_media_data_node() must be called after the data node is consumed.
 *                   bt_a2dp_hold_media_data_node(params->data_node);
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - 6. Optional, initiate the disconnection from a remote device, as shown in the figure titled as "The A2DP connection release message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       //  A BT_A2DP_DISCONNECT_CNF event notifies the function bt_app_event_callback() once the device is disconnected.
 *       ret = bt_a2dp_disconnect(handle);
 *    @endcode
 */

#include <stdint.h>
#include <stdbool.h>
#include "bt_type.h"

/**
 * @defgroup Bluetoothbt_a2dp_define Define
 * @{
 */

/**
 * @brief The event report to user
 */

#define BT_A2DP_CONNECT_IND             (BT_MODULE_A2DP | 0x1200)       /**< An attempt to connect from a remote device, with #bt_a2dp_connect_ind_t as the payload in the callback function.
                                                                            Call #bt_a2dp_connect_response() to accept or reject the attempt. */
#define BT_A2DP_CONNECT_CNF             (BT_MODULE_A2DP | 0x1201)       /**< The result of a connect attempt initiated by a local or remote device is available, with #bt_a2dp_connect_cnf_t as the payload in the callback function. */
#define BT_A2DP_DISCONNECT_IND          (BT_MODULE_A2DP | 0x1202)       /**< A disconnect attempt is initiated by a local or remote device or a link loss occurred, with #bt_a2dp_disconnect_ind_t as the payload in the callback function. */
#define BT_A2DP_DISCONNECT_CNF          (BT_MODULE_A2DP | 0x1203)       /**< The result of a disconnect attempt initiated by a local device is available, with #bt_a2dp_disconnect_cnf_t as the payload in the callback function. */
#define BT_A2DP_START_STREAMING_IND     (BT_MODULE_A2DP | 0x1204)       /**< A start streaming request is initiated from a remote device, with #bt_a2dp_start_streaming_ind_t as the payload in the callback function.
                                                                            Call #bt_a2dp_start_streaming_response() to accept or reject the request. */
#define BT_A2DP_START_STREAMING_CNF     (BT_MODULE_A2DP | 0x1205)       /**< The result of a start streaming attempt initiated by a local device is available, with #bt_a2dp_start_streaming_cnf_t as the payload in the callback function. */
#define BT_A2DP_SUSPEND_STREAMING_IND   (BT_MODULE_A2DP | 0x1206)       /**< A suspend streaming request is initiated from a remote device, with #bt_a2dp_suspend_streaming_ind_t as the payload in the callback function.
                                                                            Call #bt_a2dp_suspend_streaming_response() to accept or reject the request. */
#define BT_A2DP_SUSPEND_STREAMING_CNF   (BT_MODULE_A2DP | 0x1207)       /**< The result of a suspend streaming attempt initiated by a local device is available, with #bt_a2dp_suspend_streaming_cnf_t as the payload in the callback function. */
#define BT_A2DP_RECONFIGURE_IND         (BT_MODULE_A2DP | 0x1208)       /**< A reconfigure codec capability request is initiated from a remote device, with #bt_a2dp_reconfigure_ind_t as the payload in the callback function.
                                                                            Call #bt_a2dp_reconfigure_response() to accept or reject the request. */
#define BT_A2DP_RECONFIGURE_CNF         (BT_MODULE_A2DP | 0x1209)       /**< The result of a reconfigure codec capability attempt initiated by a local device is available, with #bt_a2dp_reconfigure_cnf_t as the payload in the callback function. */
#define BT_A2DP_STREAMING_RECEIVED_IND  (BT_MODULE_A2DP | 0x120A)       /**< The A2DP streaming information is available, with #bt_a2dp_streaming_received_ind_t as the payload in the callback function. */
#define BT_A2DP_DELAY_REPORT_CNF        (BT_MODULE_A2DP | 0x120B)       /**< The result of a delay reporting to a source device from a sink, with #bt_a2dp_delay_report_cnf_t as the payload in the callback function. */
/**
 *  @brief Define for A2DP failed status.
 */
#define BT_STATUS_A2DP_TIMEOUT          (BT_MODULE_A2DP|0xF0)           /**< Timeout for the A2DP operation. */
#define BT_STATUS_A2DP_BAD_STATE        (BT_MODULE_A2DP|0xF1)           /**< The state for the operation is not correct. */
#define BT_STATUS_A2DP_NO_CONNECTION    (BT_MODULE_A2DP|0xF2)           /**< The A2DP is not connected. */

/**
 *  @brief Define for A2DP codec type.
 */
#define BT_A2DP_CODEC_SBC  (0)           /**< SBC codec. */
#define BT_A2DP_CODEC_AAC  (2)           /**< AAC codec. */
#define BT_A2DP_CODEC_VENDOR (0xFF)      /**< Vendor codec. */
typedef uint8_t bt_a2dp_codec_type_t;    /**< The type of A2DP codec. */

/**
 *  @brief Define for Content Protection Method type.
 */
#define BT_A2DP_CP_UNDEF    (0x00)       /**< Content Protection Method none. */
#define BT_A2DP_CP_DTCP     (0x01)       /**< Content Protection Method DTCP. */
#define BT_A2DP_CP_SCMS_T   (0x02)       /**< Content Protection Method SCMS-T. */
typedef uint16_t bt_a2dp_content_protection_t;      /**< The type of Content Protection Method. */

/**
 *  @brief Define for A2DP role type.
 */

#define BT_A2DP_SOURCE          (0)    /**< SRC role. */
#define BT_A2DP_SINK            (1)    /**< SNK role. */
#define BT_A2DP_SOURCE_AND_SINK (2)    /**< Both roles for a single device (series case). */
#define BT_A2DP_INVALID_ROLE    (0xFF) /**< Invalid role. */
typedef uint8_t bt_a2dp_role_t;        /**< The type of A2DP role. */

/**
 *  @brief Define for A2DP state type.
 */

#define BT_A2DP_IDLE            (0)       /**< Idle state. */
#define BT_A2DP_OPENED          (1)       /**< Open state. */
#define BT_A2DP_STREAMING       (2)       /**< Streaming state. */
#define BT_A2DP_INVALID_STATE   (0xFF)    /**< Invalid state. */
typedef uint8_t bt_a2dp_state_t;          /**< The type of A2DP state. */
/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_a2dp_struct Struct
 * @{
 */

/**
 *  @brief This structure defines the SBC codec details.
 */
BT_PACKED(
typedef struct {
    uint8_t channel_mode: 4;    /**< The channel mode (b0~b3), b0: Joint stereo, b1: Stereo, b2: Dual channel,
    b3: Mono. */
    uint8_t sample_freq: 4;     /**< The sample frequency (b4~b7), b4: 48kHz, b5: 44.1kHz, b6: 32kHz, b7: 16kHz. */
    uint8_t alloc_method: 2;    /**< The allocation method (b0, b1), b0: Loudness, b1:SNR. */
    uint8_t subbands: 2;        /**< The subbands (b2, b3), b2: 8 subbands, b3: 4 subbands. */
    uint8_t block_len: 4;       /**< The block length (b4~b7), b4: 16, b5: 12, b6: 8, b7: 4. */
    uint8_t min_bitpool;        /**< The minimum bit pool. */
    uint8_t max_bitpool;        /**< The maximum bit pool. */
}) bt_a2dp_sbc_codec_t;

/**
 *  @brief This structure defines the AAC codec details.
 */
BT_PACKED(
typedef struct {
    uint8_t object_type;        /**< The object type, b4: MPEG4 AAC scalable, b5: MPEG4 AAC LTP, b6: MPEG4 AAC LC, b7: MPEG2 AAC LC, b0~b3: reserved. */
    uint8_t freq_h;             /**< The first part of frequency, b0: 44.1kHz, b1: 32kHz, b2: 24kHz, b3: 22.05kHz, b4: 16kHz, b5: 12kHz, b6: 11.025kHz, b7: 8kHz. */
    uint8_t reserved: 2;        /**< Pad bit (b0, b1): reserved. */
    uint8_t channels: 2;        /**< The channels (b2,b3), b2: 2, b3: 1. */
    uint8_t freq_l: 4;          /**< The last part of frequency (b4~b7), b4: 96kHz, b5: 88.2kHz, b6: 64kHz, b7: 48kHz. */
    uint8_t br_h: 7;            /**< The first part of bit rate: b22~b16. */
    uint8_t vbr: 1;             /**< The variable bit rate. */
    uint8_t br_m;               /**< The second part of bit rate: b15~b8. */
    uint8_t br_l;               /**< The last part of bit rate: b7~b0. */
}) bt_a2dp_aac_codec_t;

/**
 *  @brief This structure defines the Vendor codec details in A2DP V1.3.
 */
BT_PACKED(
typedef struct {
    uint32_t vendor_id;         /**< The vendor id. It contains a 16-bit company id assigned by Bluetooth SIG. */
    uint16_t codec_id;          /**< The codec id. It contains a 16-bit codec ID administered by the Vendor. */
    uint8_t value[3];           /**< The codec value. It contains values specifically defined by the Vendor. */
}) bt_a2dp_vendor_codec_t;

/**
 *  @brief This structure defines the A2DP codec capability.
 */
typedef struct {
    bt_a2dp_codec_type_t type;     /**< Codec type. SBC:0, AAC:2, vendor: 0xFF */
    uint8_t sep_type;              /**< A2DP role: SOURCE, SINK. */
    uint8_t   length;              /**< The length of the following codec.*/
    union {
        bt_a2dp_aac_codec_t aac; /**< AAC codec. */
        bt_a2dp_sbc_codec_t sbc; /**< SBC codec. */
        bt_a2dp_vendor_codec_t vendor; /**< Vendor codec. */
    } codec;                       /**< Codec information. */
    bt_a2dp_content_protection_t sec_type; /**< Content Protection Method. */
    bool                         delay_report;   /**< The flag to indicate whether to support the delay report. */
} bt_a2dp_codec_capability_t;

/**
 *  @brief This structure defines the A2DP capabilities.
 */
typedef bt_a2dp_codec_capability_t bt_a2dp_capabilities_t;
/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_CONNECT_IND event.
 */
typedef struct {
    uint32_t handle;            /**< A2DP handle. */
    bt_a2dp_role_t role;        /**< SEP A2DP role. */
    bt_bd_addr_t *address;     /**< Bluetooth address of a remote device. */
} bt_a2dp_connect_ind_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_CONNECT_CNF event.
 */
typedef struct {
    uint32_t handle;           /**< A2DP handle. */
    bt_status_t status;        /**< Connection status. */
    bt_a2dp_codec_capability_t *codec_cap;   /**< Streaming channel codec capability. */
} bt_a2dp_connect_cnf_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_DISCONNECT_IND event.
 */
typedef struct {
    uint32_t handle;           /**< A2DP handle. */
} bt_a2dp_disconnect_ind_t;

/**
 *  @brief his structure defines the data type of the parameter in the callback for the #BT_A2DP_DISCONNECT_CNF event.
 */
typedef struct {
    uint32_t handle;           /**< A2DP handle. */
    bt_status_t status;        /**< Disconnect status. */
} bt_a2dp_disconnect_cnf_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_START_STREAMING_IND event.
 */
typedef struct {
    uint32_t handle;                             /**< A2DP handle. */
    bt_a2dp_codec_capability_t *codec_cap;       /**< Streaming channel codec capability. */
} bt_a2dp_start_streaming_ind_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_START_STREAMING_CNF event.
 */
typedef struct {
    uint32_t handle;                            /**< A2DP handle. */
    bt_status_t status;                         /**< Start streaming status. */
    bt_a2dp_codec_capability_t *codec_cap;      /**< Streaming channel codec capability. */
} bt_a2dp_start_streaming_cnf_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_SUSPEND_STREAMING_IND event.
 */
typedef struct {
    uint32_t handle;                           /**< A2DP handle. */
} bt_a2dp_suspend_streaming_ind_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_SUSPEND_STREAMING_CNF event.
 */
typedef struct {
    uint32_t handle;                            /**< A2DP handle. */
    bt_status_t status;                         /**< Suspend streaming status. */
} bt_a2dp_suspend_streaming_cnf_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_RECONFIGURE_IND event.
 */
typedef struct {
    uint32_t handle;                             /**< A2DP handle. */
    bt_a2dp_codec_capability_t *codec_cap;       /**< Streaming channel codec capability. */
} bt_a2dp_reconfigure_ind_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_RECONFIGURE_CNF event.
 */
typedef struct {
    uint32_t handle;                            /**< A2DP handle. */
    bt_status_t status;                         /**< Reconfigure streaming status. */
} bt_a2dp_reconfigure_cnf_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_STREAMING_RECEIVED_IND event.
 */
typedef struct {
    uint32_t handle;                             /**< A2DP handle. */
    uint8_t *data_node;                          /**< Media data pointer. */
    uint16_t total_length;                       /**< The total length of Media packet. */
    uint16_t media_offset;                       /**< The offset of media payload from the whole packet. */
} bt_a2dp_streaming_received_ind_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for the #BT_A2DP_DELAY_REPORT_CNF event.
 */
typedef struct {
    uint32_t handle;                            /**< A2DP handle. */
    bt_status_t status;                         /**< Delay reporting status. */
} bt_a2dp_delay_report_cnf_t;
/**
 *  @brief This structure defines the initialization parameters.
 */
typedef struct {
    uint32_t codec_number;                         /**< The codec num. */
    bt_a2dp_codec_capability_t *codec_list;        /**< The codec array. */
    uint16_t sink_delay;                           /**< The delay time between the receive and the rendering, united by 0.1ms. */
    uint16_t sink_feature;                         /**< Supported features for the sink role. */
    uint16_t source_feature;                       /**< Supported features for the source role. */
} bt_a2dp_init_params_t;

/**
 * @}
 */

BT_EXTERN_C_BEGIN

/**
 * @brief               This function connects to the specified remote device. The #BT_A2DP_CONNECT_CNF event will be sent to the upper layer with connection request result.
 * @param[out] handle    is the connection handle of the specified remote device. The handle can be used to abort the connection before #BT_A2DP_CONNECT_CNF arrives.
 * @param[in] address   is the Bluetooth address of a remote device.
 * @param[in] role      is local device's A2DP role.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */
bt_status_t bt_a2dp_connect(uint32_t *handle, const bt_bd_addr_t *address, bt_a2dp_role_t role);

/**
 * @brief               This function starts streaming with the specified remote device. The #BT_A2DP_START_STREAMING_CNF event will be sent to the upper layer with Suspend streaming status.
 * @param[in] handle    is the connection handle of the specified remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */
bt_status_t bt_a2dp_start_streaming(uint32_t handle);

/**
 * @brief               This function suspends the current streaming with the specified remote device. The #BT_A2DP_SUSPEND_STREAMING_CNF event will be sent to the upper layer with Suspend streaming status.
 * @param[in] handle    is the connection handle of the specified remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */

bt_status_t bt_a2dp_suspend_streaming(uint32_t handle);

/**
 * @brief               This function reconfigures the already configured stream and the API must
 *                      be called only in stream open state, not in streaming state. The #BT_A2DP_RECONFIGURE_CNF event
 *                      is sent to the upper layer with the reconfigured stream result.
 * @param[in] handle    is the connection handle of the specified remote device.
 * @param[in] codec_cap    is the codec capabilities for renegotiating with the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */

bt_status_t bt_a2dp_reconfigure_stream(uint32_t handle, bt_a2dp_codec_capability_t *codec_cap);

/**
 * @brief               This function sends media data to the remote device.
 * @param[in] handle    is the connection handle of the specified remote device.
 * @param[in] buffer     is the media payload data for the remote device.
 * @param[in] payload_size    is the size of media payload data.
 * @param[in] timestamp    is the timestamp value to add in the media header.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */

bt_status_t bt_a2dp_send_media_data(uint32_t handle, uint8_t *buffer, uint32_t payload_size, uint32_t timestamp);

/**
 * @brief               This function returns the maximum A2DP payload data value (in bytes) on the media channel so that the application can send data less than this value.
 * @param[in] handle    is the connection handle of the specified remote device.
 * @return              value of the MTU.
 */

uint32_t bt_a2dp_get_mtu_size(uint32_t handle);

/**
 * @brief               This function sets the MTU size for A2DP (in bytes) on the media channel.
 * @param[in] mtu_size  is the MTU size for A2DP media channel.
 * @return              value of the MTU.
 */
void bt_a2dp_set_mtu_size(uint32_t mtu_size);

/**
 * @brief               This function aborts the current connection channel with the specified remote device. The #BT_A2DP_DISCONNECT_IND event will be sent to the upper layer.
 * @param[in] handle    is the connection handle of the specified remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */
bt_status_t bt_a2dp_abort_streaming(uint32_t handle);

/**
 * @brief               This function responds to the specified remote device's incoming connection. The #BT_A2DP_CONNECT_CNF event will be sent to the upper layer with connection request result.
 * @param[in] handle    is the A2DP handle for the specified remote device.
 * @param[in] accept    defines whether accept or reject remote device's connection request.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_a2dp_connect_response(uint32_t handle, bool accept);

/**
 * @brief               This function disconnects from the specified remote device. The #BT_A2DP_DISCONNECT_CNF event will be sent to the upper layer with disconnection request result.
 * @param[in] handle    is the A2DP handle for the specified remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */
bt_status_t bt_a2dp_disconnect(uint32_t handle);

/**
 * @brief               This function responds to the incoming streaming from the specified remote device.
 * @param[in] handle    is the A2DP handle for the specified remote device.
 * @param[in] accept    defines whether to accept or reject the streaming request.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_a2dp_start_streaming_response(uint32_t handle, bool accept);

/**
 * @brief               This function responds to suspended streaming from the specified remote device.
 * @param[in] handle    is the A2DP handle for the specified remote device.
 * @param[in] accept    defines whether to accept or reject the suspension request.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */
bt_status_t bt_a2dp_suspend_streaming_response(uint32_t handle, bool accept);

/**
 * @brief               This function responds to the reconfiguration of the specified remote device.
 * @param[in] handle    is the A2DP handle for the specified remote device.
 * @param[in] accept    defines whether to accept or reject the reconfiguration request.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
 */
bt_status_t bt_a2dp_reconfigure_response(uint32_t handle, bool accept);

/**
 * @brief               This function is an A2DP initialization API. Implementation is required.
 * @param[out] param    is the initialization parameter. The value is provided once it is called by A2DP.
 * @return              #BT_STATUS_SUCCESS, A2DP is enabled successfully.
 *                      #BT_STATUS_FAIL, enabling A2DP failed.
 *                      #BT_STATUS_OUT_OF_MEMORY, enabling A2DP failed because there is not enough memory.
 */
bt_status_t bt_a2dp_get_init_params(bt_a2dp_init_params_t *param);

/**
 * @brief               This function holds a media packet from A2DP, preventing from that the packet is released before consumed.
 * @param[in] data_node is media packet node which needs to be kept, preventing that it is released before it is consumed.
 * @return              None.
 */
void bt_a2dp_hold_media_data_node(uint8_t *data_node);

/**
* @brief               This function releases a media packet after it is consumed.
* @param[in] data_node is media packet node which needs be released after it is consumed.
* @return              None.
*/
void bt_a2dp_release_media_data_node(uint8_t *data_node);

/**
* @brief               This function sets the audio delay at the sink part.
* @param[in] handle    is the A2DP handle for the specified remote device.
* @param[in] sink_delay is the delay time united by 0.1ms.
* @return              #BT_STATUS_SUCCESS, setting delay report is successfully.
*                      #BT_STATUS_FAIL, setting delay report failed.
*                      #BT_STATUS_OUT_OF_MEMORY, the operation has failed because there is not enough memory.
*/
bt_status_t bt_a2dp_set_delay_report(uint32_t handle, uint16_t sink_delay);

/**
* @brief               This function updates A2DP SEPs. This API is only called before A2DP is connected with a remote device.
* @param[out] params   is the initialization parameter.
* @return              #BT_STATUS_SUCCESS, A2DP SEPs are successfully updated.
*                      #BT_STATUS_FAIL, updating SEPs failed.
*/
bt_status_t bt_a2dp_update_sep(bt_a2dp_init_params_t *params);

/**
 * @brief                 This function is used to enable or disable A2DP sdp record (sink and source).
 * @param[in] enable      The settings to enable or disable the A2DP service record.
 */
void bt_a2dp_enable_service_record(bool enable);

BT_EXTERN_C_END
/**
 * @}
 * @}
 */
#endif /*__BT_A2DP_H__*/

