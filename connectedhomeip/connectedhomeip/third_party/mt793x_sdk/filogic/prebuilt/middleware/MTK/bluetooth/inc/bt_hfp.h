/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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

#ifndef __BT_HFP_H__
#define __BT_HFP_H__

#include <stdint.h>
#include "bt_type.h"

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothHFP HFP
 * @{
 * The Hands-Free Profile (HFP) defines the minimum set of functions such that a mobile phone can be used in conjunction with a Hands-Free device,
 * with a Bluetooth link providing a wireless means for both remote control of the mobile phone by the Hands-Free device and voice connection between the mobile phone and the Hands-Free device.
 * The following roles are defined for this profile.
 * Audio Gateway (AG). A device that acts as a gateway of the audio, both for input and output. Typical devices acting as AGs are cellular phones.
 * Hands-Free unit (HF). A device that acts as the AG's remote audio input and output. It also provides remote control means.
 * - Supported profile version. 1.6
 * - Supported role. Hands-free unit.
 * - Supported features:
 *  - Call waiting and 3-way calling.
 *  - CLI presentation capability.
 *  - Voice recognition activation.
 *  - Remote volume control.
 *  - Enhanced call status.
 *  - Enhanced call control.
 *  - Codec negotiation.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b BATTCHG                    | Battery Charge indicator of AG to provide the battery level. |
 * |\b BRSF                       | Bluetooth Retrieve Supported Features. |
 * |\b SYNC                       | Synchronization. |
 * |\b MIC                        | Microphone. |
 * |\b DTMF                       | Dual Tone Multi-Frequency, is an in-band telecommunication signaling system using the voice-frequency band over telephone lines. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling">Dual tone multi-frequency signaling</a> in Wikipedia. |
 * |\b AG                         | Audio Gateway, the remote device.|
 * |\b NREC                       | Noise Reduction and Echo Canceling, the feature to disable or enable in AG. |
 * |\b WBS                        | Wide Band Speech. |
 * |\b CLI                        | Calling Line Identification, the feature to disable or enable in AG. |
 * |\b DIR                        | Director, the call's director, an attribute of a call. |
 * |\b HFP                        | Hands-Free Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Hands-Free_Profile_.28HFP.29">Hands-free profile</a> in Wikipedia. |
 * |\b CVSD                       | Continuously Variable Slope Delta modulation, a codec of audio. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Continuously_variable_slope_delta_modulation">Continuously variable slope delta modulation</a> in Wikipedia. |
 * |\b MSBC                       | Modified Sub Band Codec, a codec of audio. |
 * |\b CHLD                       | Call Hold, the 3-way call action in AG. |
 * |\b ISDN                       | Integrated Services Digital Network. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Integrated_Services_Digital_Network"> Integrated services digital network</a> in Wikipedia. |
 * |\b FAX                        | Facsimile. |
 * |\b SLC                        | Service Level Connection, the HFP connection with status negotiation. |
 *
 * @section bt_hfp_api_usage How to use this module
 * This section presents the HFP connection handle and the method to send the command to dial a number.
 *  - 1. Mandatory, implement #bt_app_event_callback() to handle the HFP events, such as connect, disconnect, indication and more.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t mgs, bt_status_t status, void *buff)
 *       {
 *           switch (event_id)
 *           {
 *              case BT_HFP_CONNECT_REQUEST_IND:
 *              {
 *                  bt_hfp_connect_request_ind_t *re_ind =
 *                      (bt_hfp_connect_request_ind_t *)buff;
 *                  bt_hfp_connect_response(re_ind->handle,true);
 *                  break;
 *              }
 *              default:
 *              {
 *                  break;
 *              }
 *            }
 *        }
 *    @endcode
 *  - 2. Mandatory, implement the initialization callback #bt_hfp_get_init_params().
 *   - Sample code:
 *    @code
 *       bt_status_t bt_hfp_get_init_params(bt_hfp_init_param_t *init_params)
 *       {
 *           init_params->supported_codecs = BT_HFP_CODEC_TYPE_MSBC;
 *
 *           init_params->indicators.battery = BT_HFP_INDICATOR_ON;
 *           init_params->indicators.roaming = BT_HFP_INDICATOR_ON;
 *           init_params->indicators.signal = BT_HFP_INDICATOR_ON;
 *           init_params->indicators.service = BT_HFP_INDICATOR_ON;
 *
 *           init_params->support_features = BT_HFP_INIT_SUPPORT_ALL;
 *
 *           init_params->disable_nrec = true;
 *           init_params->enable_call_waiting = true;
 *           init_params->enable_cli = true;
 *           return BT_STATUS_SUCCESS;
 *       }
 *
 *    @endcode
 *  - 3. Mandatory, connect to the remote device, as shown in figure titled as "HFP connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *        Connect to a remote device by passing the remote address and get the connection handle.
 *        The event #BT_HFP_SLC_CONNECTING_IND captured by #bt_app_event_callback() indicates the RFCOMM is connected and SLC is to be created.
 *        The event #BT_HFP_SLC_CONNECTED_IND captured by #bt_app_event_callback() indicates the SLC is connected.
 *    @code
 *
 *       ret = bt_hfp_connect(&handle, &addr);
 *
 *    @endcode
 *  - 4. Optional, after receiving the event #BT_HFP_SLC_CONNECTED_IND, call the API #bt_hfp_send_command() to send a command to dial a number, such as 'ATDXXXX', 'ATD>XXX', 'AT+BLDN', as shown in figure titled as "Initiate an outgoing voice call message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       uint8_t hfp_cmd[7] = "ATD114";
 *       ret = bt_hfp_send_command(handle, hfp_cmd, strlen(hfp_cmd));
 *
 *    @endcode
 *  - 5. Mandatory, activate the audio link after the #BT_HFP_AUDIO_CONNECT_IND event is received. The application needs to activate the audio link status, as shown in figure titled as "HFP connection release message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *        The event #BT_HFP_AUDIO_STATUS_CHANGE_IND captured by #bt_app_event_callback() indicates the audio link status has changed.
 *    @code
 *       ret = bt_hfp_set_audio_status(handle, BT_HFP_AUDIO_STATUS_ACTIVE);
 *    @endcode
 *  - 6. Mandatory, disconnect the remote device, as shown in figure titled as "HFP connection release message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *        Disconnect the remote device by passing the connection handle.
 *        The event #BT_HFP_DISCONNECT_IND captured by #bt_app_event_callback() indicates the RFCOMM channel has disconnected.
 *    @code
 *       ret = bt_hfp_disconnect(handle);
 *    @endcode
 */


/**
 * @defgroup Bluetoothbt_hfp_define Define
 * @{
 * This section defines the HFP events and error codes.
 */

/**
 * @brief The event report to user
 */

#define BT_HFP_INVALID_HANDLE                  0                                    /**< Invalid handle.*/
#define BT_HFP_PHONE_NUMBER_MAX_LEN            64                                   /**< The maximum length of a phone number.*/

#define BT_HFP_SLC_CONNECTING_IND           (BT_MODULE_HFP | 0x002)                 /**< The SLC Connecting Indication event is triggered while the SLC is connecting with the #bt_hfp_slc_connecting_ind_t payload. */
#define BT_HFP_SLC_CONNECTED_IND            (BT_MODULE_HFP | 0x003)                 /**< The SLC Connected Indication event is triggered once the SLC is connected with the #bt_hfp_slc_connected_ind_t payload. */
#define BT_HFP_CONNECT_REQUEST_IND          (BT_MODULE_HFP | 0x004)                 /**< The HFP Connect Request Indication event is triggered when AG attempts to connect with HF with the #bt_hfp_connect_request_ind_t payload. */
#define BT_HFP_DISCONNECT_IND               (BT_MODULE_HFP | 0x005)                 /**< The HFP Disconnected Indication is triggered when RFCOMM link is disconnected with the #bt_hfp_disconnect_ind_t payload. */
#define BT_HFP_AUDIO_CONNECT_IND            (BT_MODULE_HFP | 0x006)                 /**< The HFP Audio Connected Indication is triggered when the audio connection is connected with the #bt_hfp_audio_connect_ind_t payload. */
#define BT_HFP_AUDIO_DISCONNECT_IND         (BT_MODULE_HFP | 0x007)                 /**< The HFP Audio Disconnected Indication is triggered when the audio connection is disconnected with the #bt_hfp_audio_disconnect_ind_t payload. */
#define BT_HFP_BRSF_FEATURES_IND            (BT_MODULE_HFP | 0x008)                 /**< The BRSF Feature Indication is triggered when the AG supported features are fetched with the #bt_hfp_feature_ind_t payload. */
#define BT_HFP_CALL_HOLD_FEATURES_IND       (BT_MODULE_HFP | 0x009)                 /**< The Call Hold Features Indication is triggered when fetching the call hold actions that AG supports with the #bt_hfp_hold_feature_ind_t payload. */
#define BT_HFP_CIEV_CALL_SETUP_IND          (BT_MODULE_HFP | 0x00a)                 /**< The Call Setup Indication is triggered when the call setup status is changed with the #bt_hfp_call_setup_ind_t payload. */
#define BT_HFP_CIEV_CALL_IND                (BT_MODULE_HFP | 0x00b)                 /**< The Call Indication is triggered when the call status is changed with the #bt_hfp_call_ind_t payload. */
#define BT_HFP_CIEV_CALL_HOLD_IND           (BT_MODULE_HFP | 0x00c)                 /**< The Call Hold Indication is triggered when the call hold status is changed with the #bt_hfp_call_hold_ind_t payload. */
#define BT_HFP_CIEV_SERVICE_IND             (BT_MODULE_HFP | 0x00d)                 /**< The Service Indication is triggered when the service status is changed with the #bt_hfp_service_ind_t payload. */
#define BT_HFP_CIEV_SIGNAL_IND              (BT_MODULE_HFP | 0x00e)                 /**< The Signal Indication is triggered when the signal value is changed with the #bt_hfp_signal_ind_t payload. */
#define BT_HFP_CIEV_ROAMING_IND             (BT_MODULE_HFP | 0x00f)                 /**< The Roaming Indication is triggered when the roaming status is changed with the #bt_hfp_roaming_ind_t payload. */
#define BT_HFP_CIEV_BATTCHG_IND             (BT_MODULE_HFP | 0x010)                 /**< The Battery Changed Indication is triggered when the battery level is changed with the #bt_hfp_battery_ind_t payload. */
#define BT_HFP_RING_IND                     (BT_MODULE_HFP | 0x011)                 /**< The Ring Indication is triggered when the AG is ringing with the #bt_hfp_ring_ind_t payload. */
#define BT_HFP_CALL_WAITING_IND             (BT_MODULE_HFP | 0x012)                 /**< The Call Waiting Indication with the waiting call information is triggered when the AG gets the waiting call with the #bt_hfp_call_waiting_ind_t payload. */
#define BT_HFP_CALLER_ID_IND                (BT_MODULE_HFP | 0x013)                 /**< The Caller Indication with the caller information is triggered when the AG gets an incoming call with the #bt_hfp_caller_id_ind_t payload. */
#define BT_HFP_CURRENT_CALLS_IND            (BT_MODULE_HFP | 0x014)                 /**< The Current Calls Indication with the current call's details is triggered when the HF has queried current calls with the #bt_hfp_call_list_ind_t payload. */
#define BT_HFP_VOICE_RECOGNITION_IND        (BT_MODULE_HFP | 0x015)                 /**< The Voice Recognition Indication is triggered when the voice recognition status is changed with the #bt_hfp_voice_recognition_ind_t payload. */
#define BT_HFP_VOLUME_SYNC_SPEAKER_GAIN_IND (BT_MODULE_HFP | 0x016)                 /**< The Volume Sync Speaker Indication is triggered when the value of remote speaker volume is changed with the #bt_hfp_volume_sync_speaker_gain_ind_t payload. */
#define BT_HFP_VOLUME_SYNC_MIC_GAIN_IND     (BT_MODULE_HFP | 0x017)                 /**< The Volume Sync Microphone Indication is triggered when the value of remote microphone volume is changed with the #bt_hfp_volume_sync_mic_gain_ind_t payload. */
#define BT_HFP_IN_BAND_RING_IND             (BT_MODULE_HFP | 0x018)                 /**< The In Band Ring Indication is triggered when in-band ring feature is changed with the #bt_hfp_in_band_ring_status_ind_t payload. */
#define BT_HFP_ACTION_CMD_CNF               (BT_MODULE_HFP | 0x019)                 /**< The HFP Action Command Confirmation is triggered when the AG responds to the command request with the #bt_hfp_action_cnf_t payload. */
#define BT_HFP_AUDIO_STATUS_CHANGE_IND      (BT_MODULE_HFP | 0x01a)                 /**< The HFP audio connection status change indication event to indicate the audio connection status has changed with the #bt_hfp_audio_status_change_ind_t payload. */
#define BT_HFP_CUSTOM_COMMAND_RESULT_IND    (BT_MODULE_HFP | 0x01b)                 /**< The custom command result indication is trigged when the result of a custom command is received with the #bt_hfp_custom_command_result_ind_t payload. */
#define BT_HFP_BIND_FEATURE_IND             (BT_MODULE_HFP | 0x01c)                 /**< The BIND Feature Indication is triggered when the AG supported HF indicators are fetched with the the #bt_hfp_hf_indicators_feature_ind_t payload. */
#define BT_HFP_HF_INDICATOS_ENABLE_IND      (BT_MODULE_HFP | 0x01d)                 /**< The HF Indicators Enable Indicaton is triggered when the AG enabled HF indicators are fetched when the #bt_hfp_hf_indicators_enable_ind_t payload. */

/**
 * @brief This structure defines the phone number format.
 */
typedef uint8_t bt_hfp_phone_num_format_t;
#define BT_HFP_PHONE_NUM_FORMAT_TYPE_UNKNOWN          0x80  /**< Unknown. */
#define BT_HFP_PHONE_NUM_FORMAT_TYPE_INTERNATIONAL    0x90  /**< International. */
#define BT_HFP_PHONE_NUM_FORMAT_TYPE_NATIONAL         0xA0  /**< National. */
#define BT_HFP_PHONE_NUM_FORMAT_TYPE_NETWORK_SPEC     0xB0  /**< Network specification. */
#define BT_HFP_PHONE_NUM_FORMAT_TYPE_DEDICATED_ACC    0xC0  /**< Dedicate access, short code. */
#define BT_HFP_PHONE_NUM_FORMAT_PLAN_UNKNOWN          0x00  /**< Unknown. */
#define BT_HFP_PHONE_NUM_FORMAT_PLAN_ISDN_TELEPHONY   0x01  /**< ISDN telephony. */
#define BT_HFP_PHONE_NUM_FORMAT_PLAN_DATA_NUMBERING   0x03  /**< Data numbering. */
#define BT_HFP_PHONE_NUM_FORMAT_PLAN_TELEX_NUMBERING  0x04  /**< Telex numbering. */
#define BT_HFP_PHONE_NUM_FORMAT_PLAN_NATIONAL         0x08  /**< National numbering. */
#define BT_HFP_PHONE_NUM_FORMAT_PLAN_PRIVATE          0x09  /**< Private numbering. */


/**
 * @brief This structure defines the audio direction.
 */
typedef uint8_t bt_hfp_audio_direction_t;
#define BT_HFP_AUDIO_TO_HF      0x00       /**<  The audio direction to HF. */
#define BT_HFP_AUDIO_TO_AG      0x01       /**<  The audio direction to AG. */

/**
 * @brief This structure defines the audio connection status.
 */
typedef uint8_t bt_hfp_audio_status_t;
#define BT_HFP_AUDIO_STATUS_INACTIVE    0x00    /**<  Inactive audio connection. */
#define BT_HFP_AUDIO_STATUS_ACTIVE      0x01    /**<  Active audio connection.  */

/**
 * @brief This structure defines the supported features to initialize.
 */

typedef uint16_t bt_hfp_init_support_feature_t;
#define BT_HFP_INIT_SUPPORT_3_WAY                   0x0010     /**< Three way calling.*/
#define BT_HFP_INIT_SUPPORT_CODEC_NEG               0x0020     /**< Codec negotiation.*/
#define BT_HFP_INIT_SUPPORT_ALL                     0x0030     /**< Both three way calling and codec negotiation.*/
#define BT_HFP_INIT_SUPPORT_ENHANCED_CALL_STATUS    0x0040     /**< Enhanced call status.*/
#define BT_HFP_INIT_SUPPORT_ENHANCED_CALL_CONTROL   0x0080     /**< Enhanced call control.*/
#define BT_HFP_INIT_SUPPORT_CLI_PRESENTATION        0x0100     /**< CLI presentation capability.*/
#define BT_HFP_INIT_SUPPORT_HF_INDICATORS           0x0200     /**< HF indicators.*/


/**
 * @brief This structure defines the indicator status.
 */
typedef uint8_t bt_hfp_indicator_status_t;
#define BT_HFP_INDICATOR_OFF     0x00            /**<  Off.  */
#define BT_HFP_INDICATOR_ON      0x01            /**<  On, the default value.  */
#define BT_HFP_INDICATOR_IGNORE  0x02            /**<  Ignore.  */


/**
 * @brief This structure defines the HFP codec type.
 */
typedef uint8_t bt_hfp_audio_codec_type_t;
#define BT_HFP_CODEC_TYPE_NONE  0x00               /**<  None. */
#define BT_HFP_CODEC_TYPE_CVSD  0x01               /**<  CVSD. */
#define BT_HFP_CODEC_TYPE_MSBC  0X02               /**<  MSBC. */


/**
 * @brief This structure defines the HFP profile version.
 */
typedef uint8_t bt_hfp_version_t;
#define BT_HFP_VERSION_V15      0x01                      /**< The HFP profile version is 1.5. */
#define BT_HFP_VERSION_V16      0x02                      /**< The HFP profile version is 1.6. */
#define BT_HFP_VERSION_V17      0x03                      /**< The HFP profile version is 1.7. */
#define BT_HFP_VERSION_V18      0x04                      /**< The HFP profile version is 1.8. */

/**
 * @brief The structure defines the HFP audio gateway features.
 */
typedef uint16_t bt_hfp_ag_feature_t;
#define BT_HFP_AG_FEATURE_3_WAY                  0x0001        /**< 3-way calling. */
#define BT_HFP_AG_FEATURE_ECHO_NOISE             0x0002        /**< Echo canceling and noise reduction function. */
#define BT_HFP_AG_FEATURE_VOICE_RECOGNITION      0x0004        /**< Voice recognition function. */
#define BT_HFP_AG_FEATURE_IN_BAND_RING           0x0008        /**< In-band ring tone. */
#define BT_HFP_AG_FEATURE_VOICE_TAG              0x0010        /**< Voice tag. */
#define BT_HFP_AG_FEATURE_CALL_REJECT            0x0020        /**< Reject a call. */
#define BT_HFP_AG_FEATURE_ENHANCED_CALL_STATUS   0x0040        /**< Enhanced call status. */
#define BT_HFP_AG_FEATURE_ENHANCED_CALL_CTRL     0x0080        /**< Enhanced call control. */
#define BT_HFP_AG_FEATURE_EXTENDED_ERROR         0x0100        /**< Extended error. */
#define BT_HFP_AG_FEATURE_CODEC_NEGOTIATION      0x0200        /**< Codec negotiation. */
#define BT_HFP_AG_FEATURE_HF_INDICATORS          0x0400        /**< HF indicators to notify AG. */
#define BT_HFP_AG_FEATURE_ESCO_S4_SETTINGS       0x0800        /**< eSCO S4 settings supported. */

/**
 * @brief The structure defines the audio gateway's 3-way calling (hold) feature set.
 */
typedef uint8_t bt_hfp_ag_hold_feature_t;
#define BT_HFP_AG_HOLD_FEATURE_RELEASE_HELD_CALL        0x01          /**< Releases all held calls or sets User Determined User Busy for a waiting call. */
#define BT_HFP_AG_HOLD_FEATURE_RELEASE_ACTIVE_CALL      0x02          /**< Releases all active calls and accepts the other held or waiting call. */
#define BT_HFP_AG_HOLD_FEATURE_RELEASE_SPECIFIC_CALL    0x04          /**< Releases a specific call. */
#define BT_HFP_AG_HOLD_FEATURE_HOLD_ACTIVE_CALL         0x08          /**< Places all active calls on hold and accepts the other held or waiting call. */
#define BT_HFP_AG_HOLD_FEATURE_HOLD_SPECIFIC_CALL       0x10          /**< Places a specific call on hold. */
#define BT_HFP_AG_HOLD_FEATURE_ADD_HELD_CALL            0x20          /**< Adds a held call to the conversation. */
#define BT_HFP_AG_HOLD_FEATURE_CALL_TRANSFER            0x40          /**< Connects the two calls and disconnects the AG from both calls. */


/**
 * @brief The structure defines the current call state indicated by the AG.
 */
typedef uint8_t bt_hfp_ciev_call_state_t;
#define BT_HFP_CIEV_CALL_STATE_NONE                  0x00          /**<  No existing call on the AG.*/
#define BT_HFP_CIEV_CALL_STATE_CALL                  0x01          /**<  At least one call is active on the AG.*/


/**
 * @brief The structure defines the current call setup state indicated by the AG.
 */
typedef uint8_t bt_hfp_ciev_call_setup_state_t;
#define BT_HFP_CIEV_CALL_SETUP_STATE_NONE            0x00          /**<  No outgoing or incoming call is present on the AG.*/
#define BT_HFP_CIEV_CALL_SETUP_STATE_INCOMING        0x01          /**<  An incoming call is present on the AG.*/
#define BT_HFP_CIEV_CALL_SETUP_STATE_OUTGOING        0x02          /**<  An outgoing call is present on the AG.*/
#define BT_HFP_CIEV_CALL_SETUP_STATE_REMOTE_ALERT    0x03          /**<  An outgoing call is being alerted on the AG.*/


/**
 * @brief The structure defines the current call hold state indicated by the AG.
 */
typedef uint8_t bt_hfp_ciev_call_hold_state_t;
#define BT_HFP_CIEV_CALL_HOLD_STATE_NONE             0x00          /**<  No call is on hold state. */
#define BT_HFP_CIEV_CALL_HOLD_STATE_SOME             0x01          /**<  Some calls are on hold state. */
#define BT_HFP_CIEV_CALL_HOLD_STATE_ALL              0x02          /**<  All calls are on hold state. */


/**
 * @brief This structure defines the current state of a call. Not all states are supported by all
 * audio gateways. At the very minimum, #BT_HFP_CALL_STATUS_NONE,
 * #BT_HFP_CALL_STATUS_DIALING, #BT_HFP_CALL_STATUS_INCOMING and #BT_HFP_CALL_STATUS_ACTIVE
 * are supported.
 */
typedef uint8_t bt_hfp_call_status_t;
#define BT_HFP_CALL_STATUS_ACTIVE       0x00          /**<  An existing active call. */
#define BT_HFP_CALL_STATUS_HOLD         0x01          /**<  The call is on hold.*/
#define BT_HFP_CALL_STATUS_DIALING      0x02          /**<  An outgoing call. Attempting to call using any of the dialing commands. */
#define BT_HFP_CALL_STATUS_ALERTING     0x03          /**<  The remote party is being alerted. */
#define BT_HFP_CALL_STATUS_INCOMING     0x04          /**<  An incoming call. */
#define BT_HFP_CALL_STATUS_WAITING      0x05          /**<  The call is waiting.  This state occurs only when the AG supports 3-Way calling.  */
#define BT_HFP_CALL_STATUS_NONE         0x06          /**<  There is no active call. */
#define BT_HFP_CALL_STATUS_UNKNOWN      0x07          /**<  Unknown call state. */


/**
 * @brief This structure defines the call mode.
 */
typedef uint8_t bt_hfp_call_mode_t;
#define BT_HFP_CALL_MODE_VOICE      0x00             /**<  Voice call.  */
#define BT_HFP_CALL_MODE_DATA       0x01             /**<  Data call.  */
#define BT_HFP_CALL_MODE_FAX        0x02             /**<  FAX call.  */

/**
 * @brief This structure defines the hf indicators.
 */
typedef uint8_t bt_hfp_hf_indicators_feature_t;
#define BT_HFP_HF_INDICATORS_FEATURE_ENHANCED_SAFETY    0x01    /**<  Enhanced Safety. */
#define BT_HFP_HF_INDICATORS_FEATURE_BATTERY_LEVEL      0x02    /**<  Battery Level.  */

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_hfp_struct Struct
 * @{
 * This section defines data structures for the HFP.
 */


/**
 *  @brief This structure defines the status of optional indicators.
 */
typedef struct {
    bt_hfp_indicator_status_t service;               /**<  The service indicator status in HF. */
    bt_hfp_indicator_status_t signal;                /**<  The signal indicator status in HF. */
    bt_hfp_indicator_status_t roaming;               /**<  The roaming indicator status in HF. */
    bt_hfp_indicator_status_t battery;               /**<  The battery charge indicator status in HF. */
} bt_hfp_indicator_t;



/**
 *  @brief This structure defines the initialization parameters of the HFP.
 */
typedef struct {
    bt_hfp_init_support_feature_t support_features;        /**<  The supported features in HF. */
    bt_hfp_audio_codec_type_t supported_codecs;            /**<  The supported codec type in HF. */
    bt_hfp_indicator_t indicators;                         /**<  The supported optional indicator types in HF. */
    bool disable_nrec;                                     /**<  To disable NREC in AG. */
    bool enable_call_waiting;                              /**<  To enable call waiting notification in AG. */
    bool enable_cli;                                       /**<  To get call line identification in AG. */
} bt_hfp_init_param_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_RING_IND to indicate the remote is ringing.
 */
typedef struct {
    uint32_t              handle;                 /**<  The HFP handle of the current connection. */
} bt_hfp_ring_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CONNECT_REQUEST_IND to indicate the remote connection request.
 */
typedef struct {
    uint32_t              handle;                 /**<  The HFP handle of the current connection. */
    bt_bd_addr_t          *address;               /**<  The Bluetooth address of a remote device. */
} bt_hfp_connect_request_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_SLC_CONNECTING_IND to indicate the SLC is connecting.
 */
typedef struct {
    uint32_t              handle;                 /**<  The HFP handle of the current connection. */
} bt_hfp_slc_connecting_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_SLC_CONNECTED_IND to indicate the service level connection is connected.
 */
typedef struct {
    uint32_t                handle;                    /**<  The HFP handle of the current connection. */
    bt_hfp_version_t        hfp_version;               /**<  The connected profile version. */
} bt_hfp_slc_connected_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_DISCONNECT_IND to indicate the service level connection is disconnected.
 */
typedef struct {
    uint32_t              handle;                      /**<  The HFP handle of the current connection. */
} bt_hfp_disconnect_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_AUDIO_CONNECT_IND to indicate the SCO is connected.
 */
typedef struct {
    uint32_t                     handle;               /**<  The HFP handle of the current connection. */
    bt_hfp_audio_codec_type_t     codec;               /**<  The codec type of the current connection. */
} bt_hfp_audio_connect_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_AUDIO_DISCONNECT_IND to indicate the SCO is disconnected.
 */
typedef struct {
    uint32_t              handle;                      /**<  The HFP connection handle. */
} bt_hfp_audio_disconnect_ind_t;



/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_BRSF_FEATURES_IND to indicate the supported features of a remote device.
 */
typedef struct {
    uint32_t                    handle;               /**<  The HFP handle of the current connection. */
    uint32_t                    ag_feature;           /**<  The supported features of a remote device. */
} bt_hfp_feature_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CALL_HOLD_FEATURES_IND to indicate the remote device supports call hold features.
 */
typedef struct {
    uint32_t                    handle;             /**<  The HFP handle of the current connection. */
    uint8_t                     feature;            /**<  The supported call hold operations of a remote device. */
} bt_hfp_hold_feature_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CIEV_CALL_IND to indicate the call indicator's value.
 */
typedef struct {
    uint32_t                       handle;            /**<  The HFP handle of the current connection. */
    bt_hfp_ciev_call_state_t       state ;            /**<  The call indicator value of a remote device. */
} bt_hfp_call_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CIEV_CALL_SETUP_IND to indicate the call setup indicator's value.
 */
typedef struct {
    uint32_t                         handle;             /**<  The HFP handle of the current connection. */
    bt_hfp_ciev_call_setup_state_t   state ;             /**<  The call setup indicators value of a remote device. */
} bt_hfp_call_setup_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CIEV_CALL_HOLD_IND to indicate the call on hold indicator's value.
 */
typedef struct {
    uint32_t                         handle;              /**<  The HFP handle of the current connection. */
    bt_hfp_ciev_call_hold_state_t    state ;              /**<  The call hold indicators value of a remote device. */
} bt_hfp_call_hold_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CALL_WAITING_IND to indicate the call waiting information, if any.
 */
typedef struct {
    uint32_t              handle;                                 /**<  The HFP handle of the current connection. */
    uint16_t              num_size;                               /**<  The phone number size of the current waiting call. */
    uint8_t               number[BT_HFP_PHONE_NUMBER_MAX_LEN];    /**<  The phone number of the current waiting call. */
    uint8_t               class;                                  /**<  The Voice parameters of the current waiting call. */
    uint8_t               type;                                   /**<  The address type of the current waiting call. */
} bt_hfp_call_waiting_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CALLER_ID_IND to indicate the call information if there's an incoming call.
 */
typedef struct {
    uint32_t              handle;                                  /**<  The HFP handle of the current connection. */
    uint16_t              num_size;                                /**<  The phone number size of the call. */
    uint8_t               number[BT_HFP_PHONE_NUMBER_MAX_LEN];     /**<  The phone number of the call. */
    bt_hfp_phone_num_format_t   type;                              /**<  The address type of the call. */
} bt_hfp_caller_id_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CURRENT_CALLS_IND to indicate the current call information.
 */
typedef struct {
    uint32_t              handle;                                   /**<  The HFP handle of the current connection. */
    uint8_t               index;                                    /**<  Index of the call on the audio gateway.  */
    uint8_t               director;                                 /**<  The director of the call, 0 - Mobile Originated, 1 - Mobile Terminated. */
    bt_hfp_call_status_t  state;                                    /**<  The call state of the call. */
    bt_hfp_call_mode_t    mode;                                     /**<  The call mode of the call. */
    uint8_t               multiple_party;                           /**<  If the call is multiple party, 0 - Not Multiparty, 1 - Multiparty. */
    uint16_t              num_size;                                 /**<  The phone number size of the call. */
    uint8_t               number[BT_HFP_PHONE_NUMBER_MAX_LEN];      /**<  The phone number of the call. */
    bt_hfp_phone_num_format_t   type;                               /**<  The address type of the call. */
} bt_hfp_call_list_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CIEV_SERVICE_IND to indicate the service indicator's value.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    uint8_t               state;                           /**<  The service state of a remote device. */
} bt_hfp_service_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CIEV_BATTCHG_IND to indicate the battery charge indicator's value.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    uint8_t               level;                           /**<  The battery charge level of a remote device. */
} bt_hfp_battery_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CIEV_SIGNAL_IND to indicate the signal indicator's value.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    uint8_t               level ;                          /**<  The signal level of a remote device. */
} bt_hfp_signal_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CIEV_ROAMING_IND to indicate the roaming indicator's value.
 */
typedef struct {
    uint32_t               handle;                         /**<  The HFP handle of the current connection. */
    uint8_t                state ;                         /**<  The roaming state of a remote device. */
} bt_hfp_roaming_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_VOICE_RECOGNITION_IND to indicate the voice recognition state of the AG.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    bool                  enable;                          /**<  The voice recognition state of a remote device. */
} bt_hfp_voice_recognition_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_IN_BAND_RING_IND to indicate the in-band ring state of the AG.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    bool                  enable;                          /**<  The in-band ring state of a remote device. */
} bt_hfp_in_band_ring_status_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_VOLUME_SYNC_SPEAKER_GAIN_IND to indicate the speaker volume of the AG.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    uint8_t               data;                            /**<  The speaker volume of a remote device. */
} bt_hfp_volume_sync_speaker_gain_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_VOLUME_SYNC_MIC_GAIN_IND to indicate the microphone volume of the AG.
 */
typedef struct {
    uint32_t             handle;                          /**<  The HFP handle of the current connection. */
    uint8_t              data;                            /**<  The microphone volume of a remote device. */
} bt_hfp_volume_sync_mic_gain_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_AUDIO_STATUS_CHANGE_IND to indicate the audio connection status has changed.
 */
typedef struct {
    uint32_t             handle;                         /**<  The HFP handle of the current connection. */
    bt_hfp_audio_status_t audio_status;                  /**<  The audio link status of the current connection. */
} bt_hfp_audio_status_change_ind_t;


/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_ACTION_CMD_CNF.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    int32_t               result;                          /**<  The confirmation result. */
} bt_hfp_action_cnf_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HFP_CUSTOM_COMMAND_RESULT_IND.
 */
typedef struct {
    uint32_t              handle;                          /**<  The HFP handle of the current connection. */
    const char           *result;                          /**<  The custom command result. */
} bt_hfp_custom_command_result_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback of the event #BT_HFP_BIND_FEATURE_IND.
 */
typedef struct {
    uint32_t                        handle;                 /**< The HFP handle of the current connection. */
    bt_hfp_hf_indicators_feature_t  hf_indicators_feature;  /**< The HF indicators supported feature mask. */
} bt_hfp_hf_indicators_feature_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback of the event #BT_HFP_HF_INDICATOS_ENABLE_IND.
 */
typedef struct {
    uint32_t                        handle;                 /**< The HFP handle of the current connection. */
    bt_hfp_hf_indicators_feature_t  hf_indicators;          /**< The HF indicators feature are enable or disable. */
    bool                            enable;                 /**< The enable or disable of current HF indicatos. */
} bt_hfp_hf_indicators_enable_ind_t;

/**
 * @}
 */
BT_EXTERN_C_BEGIN

/**
 * @brief                                         This function sends a HFP connect request to the AG.
 *                                                The event #BT_HFP_SLC_CONNECTING_IND returns when the SLC is connecting.
 *                                                The event #BT_HFP_SLC_CONNECTED_IND returns when HFP negotiation is complete.
 * @param[out] handle                             is the channel handle returned.
 * @param[in] address                             is the Bluetooth address of the AG.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 *                                                #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_hfp_connect(uint32_t *handle, bt_bd_addr_t *address);


/**
 * @brief                                         This function responds to a connection request from AG.
 *                                                Calll this API to accept or reject the connection, when #BT_HFP_CONNECT_REQUEST_IND is received.
 * @param[in] handle                              is the handle of the current connection.
 * @param[in] accept                              is the operation of accepting or rejecting the current connection request.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_hfp_connect_response(uint32_t handle, bool accept);


/**
 * @brief                                         This function sends a HFP disconnect request to AG.
 *                                                The event #BT_HFP_DISCONNECT_IND returns the current request?™s result.
 * @param[in] handle                              is the handle of the current connection.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_hfp_disconnect(uint32_t handle);


/**
 * @brief                                         This function transfers the audio direction to AG or to HF.
 * @param[in] handle                              is the handle of the current connection.
 * @param[in] audio_dir                           is the switch direction, either to AG or HF, please refer to @ref bt_hfp_audio_direction_t.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_hfp_audio_transfer(uint32_t handle, bt_hfp_audio_direction_t audio_dir);

/**
 * @brief                                         This function sets the current audio connection status.
 * @param[in] handle                              is the handle of the current connection.
 * @param[in] status                              is the audio status to set.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 */

bt_status_t bt_hfp_set_audio_status(uint32_t handle, bt_hfp_audio_status_t status);


/**
 * @brief                                         This function sends an HFP command to the AG. The event #BT_HFP_ACTION_CMD_CNF returns when the remote device responds.
 * @param[in] handle                              is the handle of the current connection.
 * @param[in] command                             is the command to send.
 * @param[in] command_length                      is the command's length.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                                #BT_STATUS_FAIL, the operation has failed.
 *                                                #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_hfp_send_command(uint32_t handle, uint8_t *command, uint16_t command_length);


/**
 * @brief                                         This function gets the remote device's Bluetooth address of current connection.
 * @param[in] handle                              is the handle of the current connection.
 * @return                                        The Bluetooth address.
 */
bt_bd_addr_t *bt_hfp_get_bd_addr_by_handle(uint32_t handle);


/**
 * @brief                                         This function gets the initialization settings. This API invoked by the SDP process should be implemented by the application.
 * @param[out] init_params                         is the settings to initialize.
 * @return                                        #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_status_t bt_hfp_get_init_params(bt_hfp_init_param_t *init_params);

/**
 * @brief                                         This function enables and disables the HFP service record.
 * @param[in] enable                              The settings to enable or disable the HFP service record.
 */
void bt_hfp_enable_service_record(bool enable);

/**
 * @brief                                         This function checks whether the HFP service record is enabled.
 * @return                                       If return true, the HFP service record is enabled. Otherwise, it is disabled.
 */
bool bt_hfp_check_service_record(void);

BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif


