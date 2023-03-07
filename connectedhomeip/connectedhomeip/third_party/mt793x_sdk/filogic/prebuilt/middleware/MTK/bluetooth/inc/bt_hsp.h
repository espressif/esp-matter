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

#ifndef __BT_HSP_H__
#define __BT_HSP_H__

#include "bt_type.h"

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothHSP HSP
 * @{
 * The Headset Profile (HSP) defines the proctocals and procedures that are used by devices requiring
 * a full-duplex audio connection combined with minimal device control command. The headsets can be wirelessly
 * connected for the purpose of acting as the devices's audio input and output mechanism, providing full-duplex audio.
 * The following roles are defined for this profile.
 * Audio Gateway (AG). A device that acts as a gateway of the audio, both for input and output. Typical devices acting as AGs are cellular phones.
 * Headset (HS). A device that acts as the AG's remote audio input and output.
 * - Supported profile version. 1.2
 * - Supported role.Headset.
 * - Supported features:
 *  - Incoming audio connection.
 *  - Outgoing audio connection.
 *  - Audio connection transfer.
 *  - Remote audio volume control.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b AG                         | Audio Gateway, the remote device.|
 * |\b HS                         | Headset, the AG's remote audio input and output.|
 * |\b HSP                        | Headset Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Headset_Profile_(HSP)">Headset profile</a> in Wikipedia. |
 *
 * @section bt_hsp_api_usage How to use this module
 * This section presents the HSP connection handle and the method for sending commands to the remote device.
 * -1. (Mandatory) The AG initiates a connection to the HS, as shown in the figure titled as "HSP connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Step 1. If a remote AG initiates a connection to the HS, the #BT_HSP_CONNECT_IND event notifies the function #bt_app_event_callback().
 *   - Step 2. Call the function #bt_hsp_connect_response() to accept or reject this connection request from the remote AG.
 *   - Step 3. The #BT_HSP_CONNECT_CNF event notifies the function #bt_app_event_callback() whether the HSP successfully connected.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t mgs, bt_status_t status, void *buff)
 *       {
 *           switch (event_id)
 *           {
 *              case BT_HSP_CONNECT_IND: {
 *                  bt_hsp_connect_ind_t *re_ind = (bt_hsp_connect_ind_t *)buff;
 *                  bt_hsp_connect_response(re_ind->handle,true);
 *                  break;
 *              }
 *              case BT_HSP_CONNECT_CNF: {
 *                  bt_hsp_connect_cnf_t* conn_cnf_p = (bt_spp_connect_cnf_t*) buff;
 *                  break;
 *              }
 *              default:
 *                  break;
 *            }
 *        }
 *    @endcode
 * - 2. (Mandatory) The HS initiates a connection to the AG, as shown in figure titled as "HSP connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Step 1. Call the function #bt_hsp_connect() to initiate a connection to a remote AG.
 *   - Sample code:
 *    @code
 *       ret = bt_hsp_connect(&handle, &addr);
 *    @endcode
 *   Step 2. The #BT_HSP_CONNECT_CNF event notifies the function #bt_app_event_callback() if the device is connected successfully or not.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t mgs, bt_status_t status, void *buff)
 *       {
 *           switch (event_id)
 *           {
 *              case BT_HSP_CONNECT_CNF: {
 *                  bt_hsp_connect_cnf_t* conn_cnf_p = (bt_hsp_connect_cnf_t*) buff;
 *                  break;
 *              }
 *              default:
 *                  break;
 *            }
 *        }
 *    @endcode
 *  - 3. (Optional) When the #BT_HSP_CONNECT_CNF event is received, call the API #bt_hsp_send_command() to send a command for a user action, such as release a call by 'AT+CKPD=200'.
 *   - Sample code:
 *    @code
 *       ret = bt_hsp_send_command(handle, (const char*)"AT+CKPD=200");
 *    @endcode
 *  - 4. (Mandatory) Activate the audio link when the #BT_HSP_AUDIO_CONNECT_IND event is received. The application must activate the audio link status, as shown in the figure titled as "Incoming audio connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   Step 1.
 *  Sample code:
 *    @code
 *       ret = bt_hsp_set_audio_status(handle, BT_HSP_AUDIO_STATUS_ACTIVE);
 *    @endcode
 *   Step 2. The event #BT_HSP_AUDIO_STATUS_CHANGE_IND captured by #bt_app_event_callback() indicates the audio link status changed.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t mgs, bt_status_t status, void *buff)
 *       {
 *           switch (event_id) {
 *               case BT_HSP_AUDIO_STATUS_CHANGE_IND: {
 *                  bt_hsp_audio_connected_ind_t *status_changed = (bt_hsp_audio_connected_ind_t *)buf;
 *                  break;
 *               }
 *               default:
 *                  break;
 *           }
 *        }
 *    @endcode
 *  - 5. (Mandatory) Disconnect the HSP connection, as shown in the figure titled as "HSP connection release message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *        Disconnect the HSP connection by passing the connection handle.
 *        The event #BT_HSP_DISCONNECT_IND captured by #bt_app_event_callback() indicates the HSP has disconnected.
 *    @code
 *       ret = bt_hsp_disconnect(handle);
 *    @endcode
 */

/**
 * @defgroup Bluetoothbt_hsp_define Define
 * @{
 * This section defines the HSP events and error codes.
 */

/**
 * @brief The event report to user
 */
#define BT_HSP_INVALID_HANDLE                 0x00000000               /**< Invalid handle.*/

#define BT_HSP_CONNECT_IND                   (BT_MODULE_HSP | 0x0001)  /**< The HSP Connect Request Indication event is triggered when the AG tries to connect with the HS with the #bt_hsp_connect_ind_t payload. */
#define BT_HSP_CONNECT_CNF                   (BT_MODULE_HSP | 0x0002)  /**< The HSP Connected Indication event is triggered when the HSP is connected with the #bt_hsp_connect_cnf_t payload. */
#define BT_HSP_DISCONNECTED_IND              (BT_MODULE_HSP | 0x0003)  /**< The HSP Disconnected Indication is triggered when the HSP is disconnected with the #bt_hsp_disconnected_ind_t payload. */
#define BT_HSP_AUDIO_CONNECTED_IND           (BT_MODULE_HSP | 0x0004)  /**< The HSP Audio Connected Indication is triggered when the audio connection is connected with the #bt_hsp_audio_connected_ind_t payload. */
#define BT_HSP_AUDIO_DISCONNECTED_IND        (BT_MODULE_HSP | 0x0005)  /**< The HSP Audio Disconnected Indication is triggered when the audio connection is disconnected with the #bt_hsp_audio_disconnected_ind_t payload. */
#define BT_HSP_RING_IND                      (BT_MODULE_HSP | 0x0006)  /**< The Ring Indication is triggered when the AG is ringing with the #bt_hsp_ring_ind_t payload. */
#define BT_HSP_VOLUME_SYNC_SPEAKER_GAIN_IND  (BT_MODULE_HSP | 0x0007)  /**< The Volume Sync Speaker Indication is triggered when the value of the remote speaker volume is changed with the #bt_hsp_volume_sync_speaker_gain_ind_t payload. */
#define BT_HSP_VOLUME_SYNC_MIC_GAIN_IND      (BT_MODULE_HSP | 0x0008)  /**< The Volume Sync Microphone Indication is triggered when the value of the remote microphone volume is changed with the #bt_hsp_volume_sync_mic_gain_ind_t payload. */
#define BT_HSP_ACTION_CMD_CNF                (BT_MODULE_HSP | 0x0009)  /**< The HSP Action Command Confirmation is triggered when the AG responds to the command request with the #bt_hsp_action_cmd_cnf_t payload. */
#define BT_HSP_AUDIO_STATUS_CHANGE_IND       (BT_MODULE_HSP | 0x000A)  /**< The HSP audio connection status change indication event to indicate the audio connection status has changed with the #bt_hsp_audio_status_change_ind_t payload. */

/**
 * @brief This structure defines the audio connection status.
 */
typedef uint8_t   bt_hsp_audio_status_t;
#define BT_HSP_AUDIO_STATUS_INACTIVE         0  /**<  Inactive audio connection. */
#define BT_HSP_AUDIO_STATUS_ACTIVE           1  /**<  Active audio connection.  */

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_hsp_struct Struct
 * @{
 * This section defines the data structures for the HSP.
 */

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_CONNECT_IND to indicate the remote connection request.
 */
typedef struct {
    uint32_t    handle;    /**<  The HSP handle of the current connection. */
    bt_bd_addr_t *address; /**<  The Bluetooth address of a remote device. */
} bt_hsp_connect_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_CONNECT_CNF to indicate the HSP connection is connected.
 */
typedef struct {
    uint32_t    handle;  /**<  The HSP handle of the current connection. */
} bt_hsp_connect_cnf_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_DISCONNECTED_IND to indicate the HSP connection is disconnected.
 */
typedef struct {
    uint32_t    handle; /**<  The HSP handle of the current connection. */
} bt_hsp_disconnected_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_AUDIO_CONNECTED_IND to indicate the audio connection is connected.
 */
typedef struct {
    uint32_t    handle; /**<  The HSP handle of the current connection. */
} bt_hsp_audio_connected_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_AUDIO_CONNECTED_IND to indicate the audio connection is disconnected.
 */
typedef struct {
    uint32_t    handle; /**<  The HSP handle of the current connection. */
} bt_hsp_audio_disconnected_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_RING_IND to indicate the remote is ringing.
 */
typedef struct {
    uint32_t    handle;  /**<  The HSP handle of the current connection. */
} bt_hsp_ring_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_VOLUME_SYNC_SPEAKER_GAIN_IND to indicate the speaker volume of the AG.
 */
typedef struct {
    uint32_t    handle;     /**<  The HSP handle of the current connection. */
    uint8_t     volume;     /**<  The speaker volume of a remote device. */
} bt_hsp_volume_sync_speaker_gain_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_VOLUME_SYNC_MIC_GAIN_IND to indicate the microphone volume of the AG.
 */
typedef struct {
    uint32_t    handle;     /**<  The HSP handle of the current connection. */
    uint8_t     volume;     /**<  The microphone volume of a remote device. */
} bt_hsp_volume_sync_mic_gain_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_AUDIO_STATUS_CHANGE_IND to indicate the audio connection status has changed.
 */
typedef struct {
    uint32_t                  handle;       /**<  The HSP handle of the current connection. */
    bt_hsp_audio_status_t     status;       /**<  The audio link status of the current connection. */
} bt_hsp_audio_status_change_ind_t;

/**
 *  @brief This structure defines the parameter type in the callback for the event #BT_HSP_ACTION_CMD_CNF.
 */
typedef struct {
    uint32_t    handle;           /**<  The HSP handle of the current connection. */
} bt_hsp_action_cmd_cnf_t;

/**
 * @}
 */

BT_EXTERN_C_BEGIN

/**
 * @brief                        This function sends a HSP connect request to the AG.
 *                               The event #BT_HSP_CONNECT_CNF returns when the HSP connection is complete.
 * @param[out] handle             is the connection handle returned.
 * @param[in] address             is the Bluetooth address of the AG.
 * @return                           #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                   #BT_STATUS_FAIL, the operation failed.
 *                                   #BT_STATUS_OUT_OF_MEMORY, the operation failed because there is not enough memory.
 */
bt_status_t bt_hsp_connect(uint32_t *handle, const bt_bd_addr_t *address);

/**
 * @brief                            This function responds to a connection request from the AG.
 *                                   Call this API to accept or reject the connection when #BT_HSP_CONNECT_IND event is received.
 * @param[in] handle                 is the handle of the current connection.
 * @param[in] accept                 is the operation of accepting or rejecting the current connection request.
 * @return                            #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                    #BT_STATUS_FAIL, the operation failed.
 */
bt_status_t bt_hsp_connect_response(uint32_t handle, bool accept);

/**
 * @brief                            This function sends a HSP disconnect request to the AG.
 *                                   The #BT_HSP_DISCONNECTED_IND event is sent back to show the result of the disconnection.
 * @param[in] handle                 is the handle of the current connection.
 * @return                            #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                    #BT_STATUS_FAIL, the operation failed.
 */
bt_status_t bt_hsp_disconnect(uint32_t handle);

/**
 * @brief                            This function sets the current audio connection status.
 * @param[in] handle                 is the handle of the current connection.
 * @param[in] status                 is the audio status to set.
 * @return                            #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                    #BT_STATUS_FAIL, the operation failed.
 */
bt_status_t bt_hsp_set_audio_status(uint32_t handle,  uint8_t status);

/**
 * @brief                              This function sends a HSP command to the AG. The #BT_HSP_ACTION_CMD_CNF event is sent back when the remote device responds.
 * @param[in] handle                   is the handle of the current connection.
 * @param[in] command                  is the command to send.
 * @return                              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                      #BT_STATUS_FAIL, the operation failed.
 *                                      #BT_STATUS_OUT_OF_MEMORY, the operation failed because there is not enough memory.
 */
bt_status_t bt_hsp_send_command(uint32_t handle, const char *command);

/**
 * @brief                               This function checks the status of HSP service record.
 * @return                              If return true, the HSP service record is enabled. Otherwise, it is disabled.
 */
bool bt_hsp_check_service_record(void);

/**
 * @brief                               This function enables or disables the HSP service record.
 * @param[in] enable                    is the settings to enable or disable the HSP service record.
 */
void bt_hsp_enable_service_record(bool enable);

BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__BT_HSP_H__*/


