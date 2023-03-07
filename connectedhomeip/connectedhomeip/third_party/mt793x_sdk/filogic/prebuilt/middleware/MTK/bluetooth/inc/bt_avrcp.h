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


#ifndef __BT_AVRCP_H__
#define __BT_AVRCP_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothAVRCP AVRCP
 * @{
 * The Audio/Video Remote Control Profile (AVRCP) defines the features and procedures
 * to ensure interoperability between Bluetooth devices with audio/video control functions.
 * The AVRCP API only supports controller role (CT) in the specification of Audio/Video remote control profile V1.6.0.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                              |
 * |------------------------------|-----------------------------------------------------|
 * |\b CT                         | Controller role or the device playing the Controller role. |
 * |\b TG                         | Target role or the device playing the Target role. |
 * |\b PDU                        | Protocol Data Unit. In AVRCP, it indicates the data unit for Metadata transfer data. |
 * |\b AV/C                       | The Audio/Video Digital Interface Command set. For more information, please refer to <a href="http://1394ta.org/wp-content/uploads/2015/07/2004006.pdf">AV/C Digital Interface Command Set</a>. |
 * |\b SDP                        | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth#SDP">SDP</a> in Wikipedia. |
 *
 * @section bt_avrcp_api_usage How to use this module
 *
 *  - Step 1. (Mandatory) Implement the function #bt_app_event_callback() to handle the AVRCP events, such as connect, disconnect, send pass through, register notification, etc.
 *   - Sample code:
 *    @code
 *       void bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg)
 *           {
 *               case BT_AVRCP_CONNECT_CNF:
 *               {
 *                   bt_avrcp_connect_cnf_t *params = (bt_avrcp_connect_cnf_t *)buff;
 *                   // Save the params->handle if params->result is BT_STATUS_SUCCESS.
 *                   break;
 *               }
 *               case BT_AVRCP_DISCONNECT_IND:
 *               {
 *                   bt_avrcp_disconnect_ind_t *params = (bt_avrcp_disconnect_ind_t *)buff;
 *                   // The AVRCP disconnect handler to clear the application state, context or other items.
 *                   break;
 *               }
 *               case BT_AVRCP_PASS_THROUGH_CNF:
 *               {
 *                   bt_avrcp_pass_through_cnf_t *cnf = (bt_avrcp_pass_through_cnf_t *)buff;
 *                   if (status == BT_STATUS_SUCCESS)
 *                   {
 *                       // Send a key release pass through command if the key press command confirmation is received.
 *                       if (cnf->op_state == BT_AVRCP_OPERATION_STATE_PUSH)
 *                       {
 *                           bt_avrcp_send_pass_through_command(cnf->conn_id, cnf->key_code, BT_AVRCP_OPERATION_STATE_RELEASED);
 *                           // Keep the current AVRCP application state and context until the BT_AVRCP_OPERATION_STATE_RELEASED event is confirmed.
 *                       }
 *                   }
 *                   break;
 *               }
 *               case BT_AVRCP_EVENT_NOTIFICATION_IND:
 *               {
 *                   bt_avrcp_event_notification_t *cnf = (bt_avrcp_event_notification_t *)buff;
 *                   if (status  == BT_STATUS_AVRCP_INTERIM)
 *                   {
 *                       // Do something.
 *                   } else if (status == BT_STATUS_SUCCESS)
 *                       // Response to a successfully registered notification.
 *                       // The registered notification is changed.
 *                       // An additional NOTIFY command is expected to be sent.
 *                       // Handle the notification.
 *
 *                   } else {
 *                       // Error handing.
 *                   }
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step 2. (Mandatory) Connect to the remote device, as shown in the figure titled as "AVRCP connection establishment" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // Connect to the remote device with a remote address and connection handle given as input parameters.
 *       // The event BT_AVRCP_CONNECT_CNF is in bt_avrcp_common_callback().
 *       ret = bt_avrcp_connect(&handle, &dev_addr);
 *
 *    @endcode
 *  - Step 3. (Mandatory) Disconnect from the remote device as shown in the figure titled as "AVRCP connection release" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // Disconnect the remote device, with a given connection handle.
 *       ret = bt_avrcp_disconnect(handle);
 *
 *    @endcode
 *  - Step 4. (Mandatory) In the CT role, register notification and send pass through
 *           command when the AVRCP connection is established, as shown in the figure titled as "AV/C command procedure" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // Pass through command.
 *       // Send a pass through command, pass the connection handle, operation ID, and operation state,
 *       // then handle the event BT_AVRCP_PASS_THROUGH_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_PLAY, BT_AVRCP_OPERATION_STATE_PUSH);
 *
 *       // Register a notification event.
 *       // Register a notification event, pass the connection handle, event ID and interval, then
 *       // handle the event BT_AVRCP_EVENT_NOTIFICATION_IND in #bt_app_event_callback().
 *       // Note: According to the AVRCP specification, interval is applicable only for BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED.
 *       ret = bt_avrcp_register_notification(handle, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 255);
 *    @endcode
 *  - Step 5. (Optional) List the player application setting attributes and get
 *           or set the player application setting values, as shown in the figure titled as "AV/C command procedure" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *       // A command to list the player application setting attributes.
 *       // Send a list player application setting attributes command, pass the connection handle
 *       // then handle the event BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_list_app_setting_attributes(handle);
 *
 *
 *       // A command to get the player application setting values.
 *       // Send a get player application setting values command, pass the connection handle and list of attribute IDs
 *       // then handle the event BT_AVRCP_GET_APP_SETTING_VALUE_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_get_app_setting_value(handle, atrribute_list_len,  &attributes_id_list);
 *    @endcode
 *  - Step 6. (Optional) Play element attributes, as shown in the figure titled as "AV/C command procedure" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *   - Sample code:
 *    @code
 *
 *       // A command to get the element attributes.
 *       // Send a get element attributes command, passing connection handle and media attribute IDs
 *       // then handle the event BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF in #bt_app_event_callback().
 *       ret = bt_avrcp_get_element_attributes(conn_id, attribute_list_len, &attribute_id_list);
 *    @endcode
 */


#include "stdint.h"
#include "stdbool.h"
#include "bt_type.h"

BT_EXTERN_C_BEGIN
/**
 * @defgroup Bluetoothbt_avrcp_define Define
 * @{
 */

/**
 * @brief Event reported to the user
 */
#define BT_AVRCP_CONNECT_CNF                            (BT_MODULE_AVRCP | 0x0000)   /**< The connection confirmation event is triggered when the connection is established between the local and remote devices. The structure for this event is defined as #bt_avrcp_connect_cnf_t. */
#define BT_AVRCP_CONNECT_IND                            (BT_MODULE_AVRCP | 0x0001)   /**< The connection indication event shows whether the local device will be connected with the remote device. The structure for this event is defined as #bt_avrcp_connect_ind_t. */
#define BT_AVRCP_DISCONNECT_IND                         (BT_MODULE_AVRCP | 0x0002)   /**< The disconnect indication event shows the local device and the remote device are disconnected from each other.
                                                                                                                                                A new connection cannot be started in the same call tree because the AVRCP must have more time to free the resources. 
                                                                                                                                                The structure for this event is defined as #bt_avrcp_disconnect_ind_t. */
#define BT_AVRCP_CONNECTED_IND                          (BT_MODULE_AVRCP | 0x0003)   /**< The connection connected indication event shows the control channel has connected with the remote device after calling #bt_avrcp_browse_connect() without control channel connected. The structure for this event is defined as #bt_avrcp_connected_ind_t. */
#define BT_AVRCP_PASS_THROUGH_CNF                       (BT_MODULE_AVRCP | 0x0004)   /**< The pass through confirmation event after calling #bt_avrcp_send_pass_through_command(). The structure for this event is defined as #bt_avrcp_pass_through_cnf_t. */
#define BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF        (BT_MODULE_AVRCP | 0x0005)   /**< The list application setting attributes confirmation event after calling #bt_avrcp_list_app_setting_attributes(). The structure for this event is defined as #bt_avrcp_list_attruibutes_response_t. */
#define BT_AVRCP_GET_APP_SETTING_VALUE_CNF              (BT_MODULE_AVRCP | 0x0006)   /**< The get application setting attributes confirmation event after calling #bt_avrcp_get_app_setting_value(). The structure for this event is defined as #bt_avrcp_get_app_setting_value_response_t. */
#define BT_AVRCP_SET_APP_SETTING_VALUE_CNF              (BT_MODULE_AVRCP | 0x0007)   /**< The set application setting attributes confirmation event after calling #bt_avrcp_set_app_setting_value(). The structure for this event is defined as #bt_avrcp_set_app_setting_value_response_t. */
#define BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF             (BT_MODULE_AVRCP | 0x0008)   /**< The get element attributes confirmation event after calling #bt_avrcp_get_element_attributes(). The structure for this event is defined as #bt_avrcp_get_element_attributes_response_t. */
#define BT_AVRCP_EVENT_NOTIFICATION_IND                 (BT_MODULE_AVRCP | 0x0009)   /**< The event notification indication event shows the notification to register an event after calling #bt_avrcp_register_notification(). The structure for this event is defined as #bt_avrcp_event_notification_t. */
#define BT_AVRCP_REQUEST_CONTINUING_CNF                 (BT_MODULE_AVRCP | 0x000a)   /**< The request continuing confirmation event is only triggered after calling #bt_avrcp_request_continuing_response() has failed. The structure for this event is defined as #bt_avrcp_metadata_error_response_t. */
#define BT_AVRCP_ABORT_CONTINUING_CNF                   (BT_MODULE_AVRCP | 0x000b)   /**< The abort continuing confirmation event after calling #bt_avrcp_abort_continuing_response(). The structure for this event is defined as #bt_avrcp_abort_continuing_response_t. */
#define BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF                (BT_MODULE_AVRCP | 0x000c)   /**< The set absolute volume confirmation event after calling #bt_avrcp_set_absolute_volume(). */
#define BT_AVRCP_GET_PLAY_STATUS_CNF                    (BT_MODULE_AVRCP | 0x000d)   /**< The get play status confirmation event after calling #bt_avrcp_get_play_status(). */
#define BT_AVRCP_PLAY_ITEM_CNF                          (BT_MODULE_AVRCP | 0x000e)   /**< The play item confirmation event after calling #bt_avrcp_play_item(). */
#define BT_AVRCP_GET_CAPABILITY_CNF                     (BT_MODULE_AVRCP | 0x000f)   /**< The get capability confirmation event after calling #bt_avrcp_get_capability(). */

#define BT_AVRCP_PASS_THROUGH_COMMAND_IND               (BT_MODULE_AVRCP | 0x0010)   /**< The pass through command indication event shows that the pass through command is received from CT. */
#define BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND        (BT_MODULE_AVRCP | 0x0011)   /**< The event shows that the set absolute volume command is received from CT. */
#define BT_AVRCP_REGISTER_NOTIFICATION_IND              (BT_MODULE_AVRCP | 0x0012)   /**< The event shows that the register notification command is received from CT. */
#define BT_AVRCP_GET_PLAY_STATUS_NOTIFICATION_IND       (BT_MODULE_AVRCP | 0x0013)   /**< The event shows that the get play status notification command is received from CT. */
#define BT_AVRCP_ELEMENT_METADATA_IND                   (BT_MODULE_AVRCP | 0x0014)   /**< The event shows that the element metadata indication command from CT is received from CT. */
#define BT_AVRCP_GET_CAPABILITY_IND                     (BT_MODULE_AVRCP | 0x0015)   /**< The event shows that the get capability command indication from CT is received. */
#define BT_AVRCP_ELEMENT_METADATA_CONT_IND              (BT_MODULE_AVRCP | 0x0016)   /**< The event shows that the element metadata CONTINUATION indication command from CT is received */
#define BT_AVRCP_CONTINUATION_ABORT_IND                 (BT_MODULE_AVRCP | 0x0017)   /**< Abort the continuation is received from CT. */
#define BT_AVRCP_PLAY_ITEM_COMMAND_IND                  (BT_MODULE_AVRCP | 0x0018)   /**< The event shows that the play item command is received from CT. */
#define BT_AVRCP_SET_ADDRESSED_PLAYER_COMMAND_IND       (BT_MODULE_AVRCP | 0x0019)   /**< The event shows that the set addressed player command is received from CT. */

#define BT_AVRCP_BROWSE_CONNECT_CNF                     (BT_MODULE_AVRCP | 0x0020)   /**< The connection confirmation event is triggered when the browsing channel connection is established between the local and remote devices. The structure for this event is defined as #bt_avrcp_browse_connect_cnf_t. */
#define BT_AVRCP_BROWSE_CONNECT_IND                     (BT_MODULE_AVRCP | 0x0021)   /**< The connection indication event shows whether the local device will be connected with the remote device for a browsing channel. The structure for this event is defined as #bt_avrcp_browse_connect_ind_t. */
#define BT_AVRCP_BROWSE_DISCONNECT_IND                  (BT_MODULE_AVRCP | 0x0022)   /**< The disconnect indication event shows the connection for a browsing channel between the local device and the remote device has disconnected. */

#define BT_AVRCP_BROWSE_SET_BROWSED_PLAYER_CNF          (BT_MODULE_AVRCP | 0x0030)   /**< The get folder items confirmation event after calling #bt_avrcp_browse_set_browsed_player(). The structure for this event is defined as #bt_avrcp_browse_set_browsed_player_cnf_t. */
#define BT_AVRCP_BROWSE_GET_FOLDER_ITEMS_CNF            (BT_MODULE_AVRCP | 0x0031)   /**< The get folder items confirmation event after calling #bt_avrcp_browse_get_folder_items(). The structure for this event is defined as #bt_avrcp_browse_get_folder_items_cnf_t. */
#define BT_AVRCP_BROWSE_CHANGE_PATH_CNF                 (BT_MODULE_AVRCP | 0x0032)   /**< The change path confirmation event after calling #bt_avrcp_browse_change_path(). The structure for this event is defined as #bt_avrcp_browse_change_path_cnf_t. */
#define BT_AVRCP_BROWSE_GET_ITEM_ATTRIBUTES_CNF         (BT_MODULE_AVRCP | 0x0033)   /**< The get item attributes confirmation event after calling #bt_avrcp_browse_get_item_attributes(). The structure for this event is defined as #bt_avrcp_browse_get_item_attributes_cnf_t. */

#define BT_AVRCP_BROWSE_SET_BROWSED_PLAYER_IND          (BT_MODULE_AVRCP | 0x0040)   /**< The event shows that the set browsed player command is received from CT. */
#define BT_AVRCP_BROWSE_GET_FOLDER_ITEMS_IND            (BT_MODULE_AVRCP | 0x0041)   /**< The event shows that the get folder items command is received from CT. */
#define BT_AVRCP_BROWSE_CHANGE_PATH_IND                 (BT_MODULE_AVRCP | 0x0042)   /**< The event shows that the change path command is received from CT. */
#define BT_AVRCP_BROWSE_GET_ITEM_ATTRIBUTES_IND         (BT_MODULE_AVRCP | 0x0043)   /**< The event shows that the get item attributes command is received from CT. */
#define BT_AVRCP_BROWSE_GET_TOTAL_NUMBER_OF_ITEMS_IND   (BT_MODULE_AVRCP | 0x0044)   /**< The event shows that the get total number of items command is received from CT. */

/**
 * @brief The status of the AVRCP.
 */
#define BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER     (BT_MODULE_AVRCP | 0x01)   /**< Invalid input parameter. */
#define BT_STATUS_AVRCP_FAILED                      (BT_MODULE_AVRCP | 0x02)   /**< Command execution failed with an unknown reason. */
#define BT_STATUS_AVRCP_BUSY                        (BT_MODULE_AVRCP | 0x03)   /**< The last command did not receive a response. */
#define BT_STATUS_AVRCP_LINK_ALREADY_EXISTED        (BT_MODULE_AVRCP | 0x04)   /**< Attempt to connect to an already connected device. */
#define BT_STATUS_AVRCP_SDP_QUERY_FAILED            (BT_MODULE_AVRCP | 0x05)   /**< The SDP query has failed. */
#define BT_STATUS_AVRCP_BROWSE_UNSUPPORTED          (BT_MODULE_AVRCP | 0x06)   /**< AVRCP browse is unsupported. */
#define BT_STATUS_AVRCP_SDP_QUERY_NUMER_ERROR       (BT_MODULE_AVRCP | 0x07)   /**< AVRCP query number is 0*/

#define BT_STATUS_AVRCP_NOT_IMPLEMENTED             (BT_MODULE_AVRCP | 0x28)   /**< The command is not implemented by a remote device. */
#define BT_STATUS_AVRCP_REJECTED                    (BT_MODULE_AVRCP | 0x29)   /**< The command is rejected by a remote device. */
#define BT_STATUS_AVRCP_INTERIM                     (BT_MODULE_AVRCP | 0x2F)   /**< An interim response. */

#define BT_STATUS_AVRCP_OFFSET                                   (BT_MODULE_AVRCP | 0x30)   /**< AVRCP status offset. */
#define BT_STATUS_AVRCP_INVALID_COMMAND                          (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_INVALID_COMMAND)    /**< Invalid command. Sent if TG received a PDU that it did not understand. Valid for Commands: All*/
#define BT_STATUS_AVRCP_INVALID_PARAMETER                        (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_INVALID_PARAMETER)    /**< Invalid parameter. Sent if the TG received a PDU with a parameter ID that it did not understand. Valid for Commands: All*/
#define BT_STATUS_AVRCP_PARAMETER_CONTENT_ERROR                  (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_PARAMETER_CONTENT_ERROR)    /**< Parameter content error. Sent if the parameter ID is understood, but content is wrong or corrupted. Valid for Commands: All*/
#define BT_STATUS_AVRCP_INTERNAL_ERROR                           (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_INTERNAL_ERROR)    /**< Internal Error. Sent if there are error conditions not covered by a more specific error code. Valid for Commands: All*/
#define BT_STATUS_AVRCP_OPERATION_COMPLETED_WITHOUT_ERROR        (BT_STATUS_SUCCESS)    /**< Operation completed without error. This status that is returned when the operation is successful. Valid for Commands: All except where the response CType is AV/C REJECTED*/
#define BT_STATUS_AVRCP_UID_CHANGED                              (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_UID_CHANGED)    /**< UID Changed !V The UIDs on the device have changed. Valid for Commands: All*/
#define BT_STATUS_AVRCP_RESERVED                                 (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_RESERVED)    /**< Reserved. Valid for Commands: All*/
#define BT_STATUS_AVRCP_INVALID_DIRCTION                         (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_INVALID_DIRCTION)    /**< Invalid Direction !V The Direction parameter is invalid. Valid for Commands: Change Path*/
#define BT_STATUS_AVRCP_NOT_A_DIRECTION                          (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_NOT_A_DIRECTION)    /**< Not a Directory !V The given UID does not refer to a folder item. Valid for Commands: Change Path*/
#define BT_STATUS_AVRCP_DOSE_NOT_EXIST                           (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_DOSE_NOT_EXIST)    /**< Does Not Exist !V The given UID does not refer to any currently valid item. Valid for Commands: Change Path, PlayItem, AddToNowPlaying, GetItemAttributes*/
#define BT_STATUS_AVRCP_INVALID_SCOPE                            (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_INVALID_SCOPE)    /**< Invalid Scope !V The scope parameter is invalid. Valid for Commands: All*/
#define BT_STATUS_AVRCP_RANGE_OUT_OF_BOUNDS                      (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_RANGE_OUT_OF_BOUNDS)    /**< Range Out of Bounds !V The start of given range is not valid. Valid for Commands: GetFolderItems*/
#define BT_STATUS_AVRCP_FOLDER_ITEM_IS_NOT_PLAYABLE              (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_FOLDER_ITEM_IS_NOT_PLAYABLE)    /**< Folder Item is not playable !V The given UID refers to a folder item which cannot be handled by this media player. Valid for Commands: Play Item, AddToNowPlaying*/
#define BT_STATUS_AVRCP_MEDIA_IN_USE                             (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_MEDIA_IN_USE)    /**< Media in Use !V The media cannot be used for this operation at this time. Valid for Commands: PlayItem, AddToNowPlaying*/
#define BT_STATUS_AVRCP_NOW_PLAYING_LIST_FULL                    (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_NOW_PLAYING_LIST_FULL)    /**< Now Playing List Full !V No more items can be added to the Now Playing List. Valid for Commands: AddToNowPlaying*/
#define BT_STATUS_AVRCP_SEARCH_NOT_SUPPORTED                     (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_SEARCH_NOT_SUPPORTED)    /**< Search Not Supported !V The Browsed Media Player does not support search. Valid for Commands: Search*/
#define BT_STATUS_AVRCP_SEARCH_IN_PROGRESS                       (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_SEARCH_IN_PROGRESS)    /**< Search in Progress !V A search operation is already in progress. Valid for Commands: Search*/
#define BT_STATUS_AVRCP_INVALID_PLAYER_ID                        (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_INVALID_PLAYER_ID)    /**< Invalid Player Id !V The specified Player Id does not refer to a valid player. Valid for Commands: SetAddressedPlayer, SetBrowsedPlayer*/
#define BT_STATUS_AVRCP_PLAYER_NOT_BROWSABLE                     (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_PLAYER_NOT_BROWSABLE)    /**< Player Not Browsable !V The given Player Id refers to a Media Player which does not support browsing. Valid for Commands: SetBrowsedPlayer*/
#define BT_STATUS_AVRCP_PLAYER_NOT_ADDRESSED                     (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_PLAYER_NOT_ADDRESSED)    /**< Player Not Addressed. The given Player Id refers to a player which is not currently addressed and the command cannot be performed if the player is not set as addressed. Valid for Commands: Search, SetBrowsedPlayer*/
#define BT_STATUS_AVRCP_NO_VALID_SEARCH_RESULTS                  (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_NO_VALID_SEARCH_RESULTS)    /**< No valid Search Results !V The Search result list does not contain valid entries (e.g. the entries on the list are invalidated because the browsed player has changed). Valid for Commands: GetFolderItems*/
#define BT_STATUS_AVRCP_NO_AVAILABLE_PLAYERS                     (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_NO_AVAILABLE_PLAYERS)    /**< No available players. Valid for Commands: All*/
#define BT_STATUS_AVRCP_ADDRESSED_PLAYER_CHANGED                 (BT_STATUS_AVRCP_OFFSET | BT_AVRCP_ERR_CODE_ADDRESSED_PLAYER_CHANGED)    /**< Addressed Player Changed. Valid for Commands: Register Notification*/

/**
 * @brief The operation ID of the AVRCP pass through command.
 */
#define BT_AVRCP_OPERATION_ID_SELECT  0x00 /**< Select. */
#define BT_AVRCP_OPERATION_ID_UP  0x01 /**< Up. */
#define BT_AVRCP_OPERATION_ID_DOWN 0x02 /**< Down. */
#define BT_AVRCP_OPERATION_ID_LEFT 0x03 /**< Left. */
#define BT_AVRCP_OPERATION_ID_RIGHT 0x04 /**< Right. */
#define BT_AVRCP_OPERATION_ID_RIGHT_UP 0x05 /**< Right up. */
#define BT_AVRCP_OPERATION_ID_RIGHT_DOWN 0x06 /**< Right down. */
#define BT_AVRCP_OPERATION_ID_LEFT_UP 0x07 /**< Left up. */
#define BT_AVRCP_OPERATION_ID_LEFT_DOWN 0x08 /**< Left down. */
#define BT_AVRCP_OPERATION_ID_ROOT_MENU 0x09 /**< Root menu. */
#define BT_AVRCP_OPERATION_ID_SETUP_MENU 0x0A /**< Setup menu. */
#define BT_AVRCP_OPERATION_ID_CONTENTS_MENU 0x0B /**< Contents menu. */
#define BT_AVRCP_OPERATION_ID_FAVORITE_MENU 0x0C /**< Favorite menu. */
#define BT_AVRCP_OPERATION_ID_EXIT 0x0D /**< Exit. */

#define BT_AVRCP_OPERATION_ID_0 0x20 /**< Number 0. */
#define BT_AVRCP_OPERATION_ID_1 0x21 /**< Number 1. */
#define BT_AVRCP_OPERATION_ID_2 0x22 /**< Number 2. */
#define BT_AVRCP_OPERATION_ID_3 0x23 /**< Number 3. */
#define BT_AVRCP_OPERATION_ID_4 0x24 /**< Number 4. */
#define BT_AVRCP_OPERATION_ID_5 0x25 /**< Number 5. */
#define BT_AVRCP_OPERATION_ID_6 0x26 /**< Number 6. */
#define BT_AVRCP_OPERATION_ID_7 0x27 /**< Number 7. */
#define BT_AVRCP_OPERATION_ID_8 0x28 /**< Number 8. */
#define BT_AVRCP_OPERATION_ID_9 0x29 /**< Number 9. */
#define BT_AVRCP_OPERATION_ID_DOT 0x2A /**< Dot. */
#define BT_AVRCP_OPERATION_ID_ENTER 0x2B /**< Enter. */
#define BT_AVRCP_OPERATION_ID_CLEAR 0x2C /**< Clear. */

#define BT_AVRCP_OPERATION_ID_CHANNEL_UP 0x30 /**< Channel up. */
#define BT_AVRCP_OPERATION_ID_CHANNEL_DOWN 0x31 /**< Channel down. */
#define BT_AVRCP_OPERATION_ID_PREVIOUS_CHANNEL 0x32 /**< Previous channel. */
#define BT_AVRCP_OPERATION_ID_SOUND_SELECT 0x33 /**< Sound select. */
#define BT_AVRCP_OPERATION_ID_INPUT_SELECT 0x34 /**< Input select. */
#define BT_AVRCP_OPERATION_ID_DISPLAY_INFO 0x35 /**< Display information. */
#define BT_AVRCP_OPERATION_ID_HELP 0x36 /**< Help. */
#define BT_AVRCP_OPERATION_ID_PAGE_UP 0x37 /**< Page up. */
#define BT_AVRCP_OPERATION_ID_PAGE_DOWN 0x38 /**< Page down. */

#define BT_AVRCP_OPERATION_ID_POWER 0x40 /**< Power. */
#define BT_AVRCP_OPERATION_ID_VOLUME_UP 0x41 /**< Volume up. */
#define BT_AVRCP_OPERATION_ID_VOLUME_DOWN 0x42 /**< Volume down. */
#define BT_AVRCP_OPERATION_ID_MUTE 0x43 /**< Mute. */
#define BT_AVRCP_OPERATION_ID_PLAY 0x44 /**< Play. */
#define BT_AVRCP_OPERATION_ID_STOP 0x45 /**< Stop. */
#define BT_AVRCP_OPERATION_ID_PAUSE 0x46 /**< Pause. */
#define BT_AVRCP_OPERATION_ID_RECORD 0x47 /**< Record. */
#define BT_AVRCP_OPERATION_ID_REWIND 0x48 /**< Rewind. */
#define BT_AVRCP_OPERATION_ID_FAST_FORWARD 0x49 /**< Fast forward.*/
#define BT_AVRCP_OPERATION_ID_EJECT 0x4A /**< Eject. */
#define BT_AVRCP_OPERATION_ID_FORWARD 0x4B /**< Forward. */
#define BT_AVRCP_OPERATION_ID_BACKWARD 0x4C /**< Backward. */

#define BT_AVRCP_OPERATION_ID_ANGLE 0x50 /**< Angle. */
#define BT_AVRCP_OPERATION_ID_SUBPICTURE 0x51 /**< Sub picture. */

#define BT_AVRCP_OPERATION_ID_F1 0x71 /**< F1. */
#define BT_AVRCP_OPERATION_ID_F2 0x72 /**< F2. */
#define BT_AVRCP_OPERATION_ID_F3 0x73 /**< F3. */
#define BT_AVRCP_OPERATION_ID_F4 0x74 /**< F4. */
#define BT_AVRCP_OPERATION_ID_F5 0x75 /**< F5. */

#define BT_AVRCP_OPERATION_ID_RESERVED 0x7F /**< Reserved. */
typedef uint8_t bt_avrcp_operation_id_t;    /**< The operation ID type. */

#define BT_AVRCP_FALSE 0    /**< False. */
#define BT_AVRCP_TRUE 1     /**< True. */

#define BT_AVRCP_MAX_CHANNEL        2           /**< One Target and one Controller. */

/* 1. role: add TG role to avrcp connect */
#define BT_AVRCP_ROLE_CT             0          /**< Controller role. */
#define BT_AVRCP_ROLE_TG             1          /**< Target role. */
#define BT_AVRCP_ROLE_CT_AND_TG      2          /**< As CT role and TG role at the same time. */
#define BT_AVRCP_ROLE_UNDEF         255         /**< Undefine the role. */
typedef uint8_t bt_avrcp_role_t;                /**< The role. */

/**
* @brief The key state of the pass through command.
*/
#define BT_AVRCP_OPERATION_STATE_PUSH 0             /**< The push state. */
#define BT_AVRCP_OPERATION_STATE_RELEASED 1         /**< The released state. */
typedef uint8_t bt_avrcp_operation_state_t;  /**< The key state type of the operation. */

/**
* @brief The packet type indicating the fragmented state of the packet.
*/
#define BT_AVRCP_METADATA_PACKET_TYPE_NON_FRAGMENT 0x00     /**< The packet is not fragmented. */
#define BT_AVRCP_METADATA_PACKET_TYPE_START 0x01            /**< The starting packet of the fragmented packets. */
#define BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE 0x02         /**< The packet is in the middle of the fragmented packets. */
#define BT_AVRCP_METADATA_PACKET_TYPE_END 0x03              /**< The final packet of the fragmented packets. */
typedef uint16_t bt_avrcp_metadata_packet_type_t;           /**< The packet type to indicate the packet fragmentation state. */

/**
* @brief 3. METADATA.
*/
typedef enum {
    BT_AVRCP_FIRST_OR_ONLY_IND_REQ = 0,/**< First/only metadata packet.*/
    BT_AVRCP_CONTINUATION_IND_REQ,/**< Continuation request, continue packets.*/
    BT_AVRCP_CONTINUATION_ABORT_IND_REQ,/**< Indication type, just delete the pending PDU data.*/
    BT_AVRCP_PACKET_TYPE_IND_TOTAL
} bt_avrcp_continuation_data_req_type;

typedef bt_avrcp_continuation_data_req_type data_type;  /**< Data type. */

/**
* @brief The media attribute IDs.
*/
#define BT_AVRCP_MEDIA_ATTRIBUTE_TITLE 0x01                 /**< Display the title of the media. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_ARTIST_NAME 0x02           /**< Display the name of the artist. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_ALBUM_NAME 0x03            /**< Display the name of the album. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_MEDIA_NUMBER 0x04          /**< Display the number of the media, such as the track number of the CD. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_TOTAL_MEDIA_NUMBER 0x05    /**< Display the total number of the media, such as the total number of tracks on the CD. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_GENRE 0x06                 /**< Display the music genre of the media. */
#define BT_AVRCP_MEDIA_ATTRIBUTE_PLAYING_TIME 0x07          /**< Display the playing duration in milliseconds. */
typedef uint32_t bt_avrcp_media_attribute_t;                /**< The type of media attributes IDs. */

/**
* @brief The event of register notification.
*/
#define    BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED 0x01      /**< The playback status of the current track has changed. */
#define    BT_AVRCP_EVENT_TRACK_CHANGED 0x02                /**< The current track is changed. */
#define    BT_AVRCP_EVENT_TRACK_REACHED_END 0x03            /**< End of a track reached. */
#define    BT_AVRCP_EVENT_TRACK_REACHED_START 0x04          /**< Start of a track. */
#define    BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED 0x05         /**< The playback position has changed. */
#define    BT_AVRCP_EVENT_BATT_STATUS_CHANGED 0x06          /**< The battery status has changed. */
#define    BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED 0x07        /**< The system status has changed. */
#define    BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED 0x08   /**< The player application settings are changed. */
#define    BT_AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED 0x09  /**< The now playing content has changed*/
#define    BT_AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED 0x0a    /**< Available Players Changed Notification: event enables the CT to be informed if a new player is available to address (e.g. to start or install).*/
#define    BT_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED 0x0b     /**< Addressed Player Changed Notification: The response format for this event is the Player Id of the current addressed player.*/
#define    BT_AVRCP_EVENT_UIDS_CHANGED 0x0c                 /**< The uid has changed. */
#define    BT_AVRCP_EVENT_VOLUME_CHANGED 0x0D               /**< The absolute volume has changed. */
typedef uint8_t bt_avrcp_event_t;                           /**< The event type of register notification. */

/**
* @brief The System status for a registered event BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED.
*/
#define    BT_AVRCP_STATUS_SYSTEM_POWER_ON 0x00         /**< The player is on */
#define    BT_AVRCP_STATUS_SYSTEM_POWER_OFF 0x01        /**< The player is off */
#define    BT_AVRCP_STATUS_SYSTEM_UNPLUGGED 0x02        /**< The player is not connected to bluetooth*/

/**
* @brief batterystatus for registered event BT_AVRCP_EVENT_BATT_STATUS_CHANGED.
*/
#define    BT_AVRCP_STATUS_BATT_NORMAL 0x00             /**< Battery operation is in normal state */
#define    BT_AVRCP_STATUS_BATT_WARNING 0x01            /**< The battery level is low, operation will terminate soon.*/
#define    BT_AVRCP_STATUS_BATT_CRITICAL 0x02           /**< Critical battery level, the operation will terminate soon.  */
#define    BT_AVRCP_STATUS_BATT_EXTERNAL 0x03           /**< The device is connected to an external power supply. */
#define    BT_AVRCP_STATUS_BATT_FULL_CHARGE 0x04        /**< The device is completely charged by an external power supply. */

/**
* @brief playstatus for registered event BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED.
*/
#define    BT_AVRCP_STATUS_PLAY_STOPPED 0x00            /**< The Player has stopped */
#define    BT_AVRCP_STATUS_PLAY_PLAYING 0x01            /**< A song is playing */
#define    BT_AVRCP_STATUS_PLAY_PAUSED 0x02             /**< The song is paused */
#define    BT_AVRCP_STATUS_PLAY_FWD_SEEK 0x03           /**< Forward seek */
#define    BT_AVRCP_STATUS_PLAY_REV_SEEK 0x04           /**< Reverse seek */
#define    BT_AVRCP_STATUS_PLAY_ERROR 0xFF              /**< An error occurred. */

typedef uint8_t bt_avrcp_status_t;  /**< The event type of register notification. */

/**
* @brief The PDU ID of the VENDOR DEPENDENT command.
*/
#define BT_AVRCP_PDU_ID_GET_CAPABILITIES 0X10                       /**< The PDU ID to get TG capabilities and provide it to CT. */
#define BT_AVRCP_PDU_ID_LIST_APP_SETTING_ATTRIBUTES 0X11            /**< The PDU ID for #bt_avrcp_list_app_setting_attributes(). */
#define BT_AVRCP_PDU_ID_LIST_APP_SETTING_VALUES 0X12                /**< The PDU ID to request the TG to list the set of possible values for the requested player application setting attribute. */
#define BT_AVRCP_PDU_ID_GET_APP_SETTING_VALUE 0X13                  /**< The PDU ID for #bt_avrcp_get_app_setting_value(). */
#define BT_AVRCP_PDU_ID_SET_APP_SETTING_VALUE 0X14                  /**< The PDU ID for #bt_avrcp_set_app_setting_value(). */
#define BT_AVRCP_PDU_ID_GET_PLAYER_APP_SETTING_ATT_TXT 0X15         /**< The PDU ID to request the TG to provide the text that shows the supported player application setting attribute. */
#define BT_AVRCP_PDU_ID_GET_PLAYER_APP_SETTING_VALUE_TXT 0X16       /**< The PDU ID to request the TG to provide the text that shows the supported player application setting values for the target. */
#define BT_AVRCP_PDU_ID_INFORM_DISPLAYBLE_CHAR_SET 0X17             /**< The PDU ID to provide the list of character sets supported by the CT to the TG. */
#define BT_AVRCP_PDU_ID_INFORM_BATTERY_STATUS_OF_CT 0X18            /**< The PDU ID to be sent by the CT to TG when the CT's battery status changes. */
#define BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES 0x20                 /**< The PDU ID for #bt_avrcp_get_element_attributes(). */
#define BT_AVRCP_PDU_ID_GET_PLAY_STATUS 0x30                        /**< The PDU ID for #bt_avrcp_media_send_play_status_response(). */
#define BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION 0X31                  /**< The PDU ID for #bt_avrcp_register_notification(). */
#define BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RSP 0X40                 /**< The PDU ID for #bt_avrcp_request_continuing_response(). */
#define BT_AVRCP_PDU_ID_ABORT_CONTINUING_RSP  0X41                  /**< The PDU ID for #bt_avrcp_abort_continuing_response(). */
#define BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME   0x50                  /**< The PDU ID for #bt_avrcp_set_absolute_volume(). */
#define BT_AVRCP_PDU_ID_SET_ADDRESSED_PLAYER  0x60                  /**< The PDU ID for #bt_avrcp_send_set_addressed_player_response(). */
#define BT_AVRCP_PDU_ID_PLAY_ITEM   0x74                            /**< The PDU ID for #bt_avrcp_play_item(). */
typedef uint8_t bt_avrcp_pdu_id_t;                              /**< The type of the PDU ID. */

#define BT_AVRCP_RESPONSE_NOT_IMPLEMENTED     0x08          /**< Response code for AVRCP command. TG does not implement the command sent by CT. */
#define BT_AVRCP_RESPONSE_ACCEPTED            0x09          /**< Response code for AVRCP command. TG accepts and executes the command.*/
#define BT_AVRCP_RESPONSE_REJECTED            0x0A          /**< Response code for AVRCP command. TG implements the requested command but cannot respond because the current state doesn't allow it. */
#define BT_AVRCP_RESPONSE_CHANGED             0x0D          /**< Response code for AVRCP command. TG sends the notification that the state of TG has changed. */
#define BT_AVRCP_RESPONSE_INTERIM             0x0F          /**< Response code for AVRCP command. TG has accepted the requested command but cannot return information within 100 milliseconds. */
typedef uint8_t bt_avrcp_response_t;                        /**< The response type. */

/*2. notification */
#define    BT_AVRCP_EVENT_MEDIA_PLAY_STOPPED 0x00           /**< Media play stopped. */
#define    BT_AVRCP_EVENT_MEDIA_PLAYING 0x01                /**< Media is playing. */
#define    BT_AVRCP_EVENT_MEDIA_PAUSED 0x02                 /**< Media paused. */
#define    BT_AVRCP_EVENT_MEDIA_FWD_SEEK 0x03               /**< Media seek forward. */
#define    BT_AVRCP_EVENT_MEDIA_REV_SEEK 0x04               /**< Media seek revert. */
#define    BT_AVRCP_EVENT_MEDIA_ERROR 0xFF                  /**< Media error. */
typedef uint8_t bt_avrcp_media_play_status_event_t;         /**< The event type of register notification. */

/*4. Capabilities */
#define BT_AVRCP_CAPABILITY_COMPANY_ID 0x2                  /**< The capability company id. */
#define BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED 0x3            /**< Indicates the capability request from CT. */
typedef uint8_t bt_avrcp_capability_types_t;                /**< The capability type. */

/* The maximum length of individual attribute content, including the maximum length of the title, artist name, album name and genre.*/
#define BT_AVRCP_MAX_MEDIA_ATTRIBUTE_LEN 64                 /**< The maximum media attribute length. */

/**
* @brief The item type.
*/
#define BT_AVRCP_ITEM_TYPE_MEDIA_PLAYER_ITEM        1       /**< Media player item. */
#define BT_AVRCP_ITEM_TYPE_FOLDER_ITEM              2       /**< Folder item. */
#define BT_AVRCP_ITEM_TYPE_MEDIA_ELEMENT_ITEM       3       /**< Media element item. */
typedef uint8_t bt_avrcp_item_type_t;                       /**< The item type. */

/**
* @brief The major player type.
*/
#define BT_AVRCP_MAJOR_PLAYER_TYPE_AUDIO                    0x01    /**< Audio. */
#define BT_AVRCP_MAJOR_PLAYER_TYPE_VIDEO                    0x02    /**< Video. */
#define BT_AVRCP_MAJOR_PLAYER_TYPE_BROADCASTING_AUDIO       0x04    /**< Broadcasting audio. */
#define BT_AVRCP_MAJOR_PLAYER_TYPE_BROADCASTING_VIDEO       0x08    /**< Broadcasting video. */
typedef uint8_t bt_avrcp_major_player_type_t;                       /**< The major player type. */

/**
* @brief The player sub type.
*/
#define BT_AVRCP_PLAYER_SUB_TYPE_AUDIO_BOOK     1       /**< Audio book. */
#define BT_AVRCP_PLAYER_SUB_TYPE_PODCAST        2       /**< Podcast. */
typedef uint32_t bt_avrcp_player_sub_type_t;            /**< The player sub type. */

/**
* @brief The play status.
*/
#define BT_AVRCP_PLAY_STATUS_STOPPED        0           /**< Stopped. */
#define BT_AVRCP_PLAY_STATUS_PLAYING        1           /**< Playing. */
#define BT_AVRCP_PLAY_STATUS_PAUSED         2           /**< Paused. */
#define BT_AVRCP_PLAY_STATUS_FWD_SEEK       3           /**< Seek forward. */
#define BT_AVRCP_PLAY_STATUS_REV_SEEK       4           /**< Seek reverse. */
#define BT_AVRCP_PLAY_STATUS_ERROR          0xFF        /**< Error. */
typedef uint8_t bt_avrcp_play_status_t;                 /**< The play status. */

/**
* @brief The scope.
*/
#define BT_AVRCP_SCOPE_MEDIA_PLAYER_LIST                    0       /**< Contains all available media players. */
#define BT_AVRCP_SCOPE_MEDIA_PLAYER_VIRTUAL_FILESYSTEM      1       /**< The virtual filesystem containing the media content of the browsed player. */
#define BT_AVRCP_SCOPE_SEARCH                               2       /**< The results of a search operation on the browsed player. */
#define BT_AVRCP_SCOPE_NOW_PLAYING                          3       /**< The Now Playing list (or queue) of the addressed player. */
typedef uint8_t bt_avrcp_scope_t;                                   /**< The scope. */

/**
* @brief The direction.
*/
#define BT_AVRCP_DIRECTION_UP       0                   /**< Folder up. */
#define BT_AVRCP_DIRECTION_DOWN     1                   /**< Folder down. */
typedef uint8_t bt_avrcp_direction_t;                   /**< The direction. */

/**
* @brief The folder type.
*/
#define BT_AVRCP_FOLDER_TYPE_MIXED          0           /**< Mixed folder type. */
#define BT_AVRCP_FOLDER_TYPE_TITLES         1           /**< Titles folder type. */
#define BT_AVRCP_FOLDER_TYPE_ALBUMS         2           /**< Albums folder type. */
#define BT_AVRCP_FOLDER_TYPE_ARTISTS        3           /**< Artists folder type. */
#define BT_AVRCP_FOLDER_TYPE_GENRES         4           /**< Genres folder type. */
#define BT_AVRCP_FOLDER_TYPE_PLAYLISTS      5           /**< Playlists folder type. */
#define BT_AVRCP_FOLDER_TYPE_YEARS          6           /**< Years folder type. */
typedef uint8_t bt_avrcp_folder_type_t;                 /**< The folder type. */

/**
* @brief The is playable.
*/
#define BT_AVRCP_IS_NOT_PLAYABLE            0           /**< Is not playable. */
#define BT_AVRCP_IS_PLAYABLE                1           /**< Is playable. */
typedef uint8_t bt_avrcp_is_playable_t;                 /**< Is playable. */



/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_avrcp_struct Struct
 * @{
 */


/**
* @brief The parameter of #bt_avrcp_init.
*/
typedef struct {
    bt_avrcp_role_t role;                                          /**< The role of the AVRCP. */
    bool support_browse;                                    /**< Shows whether AVRCP supports the browsing channel. */
} bt_avrcp_init_t;

/**
* @brief The struct for #BT_AVRCP_CONNECT_CNF.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
} bt_avrcp_connect_cnf_t;

/**
* @brief The struct for #BT_AVRCP_CONNECT_IND.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_bd_addr_t *address;                                  /**< The address of a remote device to start a connection. */
} bt_avrcp_connect_ind_t;

/**
* @brief The struct for #BT_AVRCP_DISCONNECT_IND.
*/
typedef bt_avrcp_connect_cnf_t bt_avrcp_disconnect_ind_t;

/**
* @brief The struct for #BT_AVRCP_CONNECTED_IND.
*/
typedef bt_avrcp_connect_cnf_t bt_avrcp_connected_ind_t;

/**
* @brief The struct for #BT_AVRCP_PASS_THROUGH_CNF.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_operation_id_t op_id;                          /**< The operation ID of the pass through command. */
    bt_avrcp_operation_state_t op_state;                    /**< The key state of the operation. */
} bt_avrcp_pass_through_cnf_t;

/**
 *@brief The struct for #BT_AVRCP_PASS_THROUGH_COMMAND_IND.
 */
typedef bt_avrcp_pass_through_cnf_t bt_avrcp_pass_through_command_ind_t;

/**
* @brief The parameter of #bt_avrcp_get_app_setting_value.
*/
BT_PACKED(
typedef struct{
    uint8_t attribute_id;                                   /**< The player application setting attribute ID of the requested settings. */
})bt_avrcp_get_app_setting_value_t;

/**
* @brief The parameter of #bt_avrcp_play_item.
*/
typedef struct {
    bt_avrcp_scope_t scope;                                 /**< The scope in which the UID of the media element item or folder item, if supported, is valid. */
    uint64_t uid;                                           /**< The UID of the media element item or folder item, if supported, to be played as defined in Section 6.10.3. */
    uint16_t uid_counter;                                   /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes.  For database unaware players uid_counter=0. */
} bt_avrcp_play_item_t;

/**
* @brief The struct of player application setting value, with attribute ID and the corresponding value.
*/
BT_PACKED(
typedef struct{
    uint8_t attribute_id;                                   /**< The player application setting attribute ID. */
    uint8_t value_id;                                       /**< The player application setting value ID. */
})bt_avrcp_app_setting_value_t;

/**
* @brief The parameter of #bt_avrcp_get_element_attributes.
*/
BT_PACKED(
typedef struct{
    bt_avrcp_media_attribute_t attribute_id;                /**< The attribute ID for the media attribute to be retrieved. */
})bt_avrcp_get_element_attributes_t;

/**
* @brief The struct to list player application setting attribute.
*/
BT_PACKED(
typedef struct{
    uint8_t attribute_id;                                   /**< The player application setting attribute ID. */
})bt_avrcp_list_attributes_response_value_t;

/**
* @brief The struct of #BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    bt_avrcp_metadata_packet_type_t  packet_type;                       /**< The packet type indicates if it is fragmented. */
    uint16_t length;                                                    /**< The total length of the attribute_list. */
    union {
        struct {
            uint8_t number;                                             /**< The number of members in the attribute_list. */
            bt_avrcp_list_attributes_response_value_t *attribute_list;  /**< The list of player application setting attribute IDs. */
        };
        uint8_t *data;                                                  /**< The fragmented data. If the packet type is BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE or
                                                                                                                        BT_AVRCP_METADATA_PACKET_TYPE_END, the application should use the data to compose a complete attribute list. */
    };
} bt_avrcp_list_attruibutes_response_t;

/**
* @brief The struct of #BT_AVRCP_GET_APP_SETTING_VALUE_CNF.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_metadata_packet_type_t packet_type;            /**< The packed type indicates if it is fragmented. */
    uint16_t length;                                        /**< The total length of the attribute_list. */
    union {
        struct {
            uint8_t number;                                 /**< The number of members in the attribute_list. */
            bt_avrcp_app_setting_value_t *attribute_list;   /**< The list of requested player application setting attribute ID and corresponding setting value. */
        };
        uint8_t *data;                                      /**< The fragmented data. If the packet type is BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE or
                                                                                                    BT_AVRCP_METADATA_PACKET_TYPE_END, the application should use the data to compose a complete attribute list. */
    };
} bt_avrcp_get_app_setting_value_response_t;

/**
* @brief The struct of #BT_AVRCP_SET_APP_SETTING_VALUE_CNF.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
} bt_avrcp_set_app_setting_value_response_t;


/**
*@brief The struct of #BT_AVRCP_ELEMENT_METADATA_IND.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    uint8_t number;                                         /**< If NumAttributes is set to zero, all attribute information is returned. Otherwise attribute information for the specified attribute IDs is returned by the TG.*/
    uint8_t data[1];                                        /**< attribute ID*/
} bt_avrcp_get_element_attribute_t;

/**
* @brief The struct of bt_avrcp_get_element_attributes_response_t.
*/
BT_PACKED(
typedef struct{
    bt_avrcp_media_attribute_t attribute_id;                /**< The media attribute ID. */
    uint16_t character_set_id;                              /**< The character set ID to be displayed on Control. */
    uint16_t attribute_value_length;                        /**< The length of the attribute value. */
    /* For big data PDUs: Metadata Attributes for Current Media Item and Player Application Settings.

    There are two options to create AVRCP data (PDU): bt_avrcp_media_attribute_t
    Either restrict attribute_value length to 64 bytes OR restrict Total size of bt_avrcp_get_element_attributes_response_t to 512 bytes
    BT_AVRCP_MAX_PDU_LEN 512 //total size
    BT_AVRCP_ELEMENT_ATTRIBUTE_MAX_DATA_SIZE 400 //Combined attribute size: Title, Artist name, Album name, Genre
    BT_AVRCP_MAX_MEDIA_ATTRIBUTE_LEN 64

    if PDU size exceeds 512, app will receive SUCCESS status (BT_STATUS_CONDITIONAL_SUCCESS) with condition:
    AVRCP has truncated the data with its own condition [data exceeding allowed length will be dropped]
    */
    /* The maximum AVRCP PDU length. */
    /* Fill BT_AVRCP_MEDIA_ATTRIBUTE_TITLE as first attribute in packet/data in attribute_value array. */

    uint8_t attribute_value[1]/*[8]*/;                              /**< The attribute value with a specified character set. */

})bt_avrcp_get_element_attributes_response_value_t;

/**
* @brief The struct of #BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF.
*/
typedef struct {
    uint32_t handle;                                                          /**< The connection handle of the AVRCP. */
    bt_avrcp_metadata_packet_type_t packet_type;                                   /**< The packet type indicates if it is fragmented. */
    uint16_t length;                                                          /**< The total length of attribute_list. */
    union {
        struct {
            uint8_t number;                                                   /**< The number of members in the attribute_list. */
            bt_avrcp_get_element_attributes_response_value_t *attribute_list;  /**< The list of response values for requested media attribute ID. */
        };
        uint8_t *data;                                                        /**< The fragmented data. If the packet type is BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE or
                                                                                   BT_AVRCP_METADATA_PACKET_TYPE_END, the application should use the data to compose a complete attribute list. */
    };
} bt_avrcp_get_element_attributes_response_t;

/**
* @brief The struct of response data format for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED.
*/
typedef struct {
    uint8_t number;                                         /**< The number of members in the attribute_list. */
    bt_avrcp_app_setting_value_t *attribute_list;           /**< The list of changed player application setting values and corresponding attribute ID. */
} bt_avrcp_application_setting_changed_t;

/**
* @brief The struct of #BT_AVRCP_EVENT_NOTIFICATION_IND.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_event_t event_id;                              /**< The register event ID for notification. */
    union {
        bt_avrcp_application_setting_changed_t setting;     /**< The response data for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED. */
        uint64_t id;                                        /**< The response data for #BT_AVRCP_EVENT_TRACK_CHANGED. */
        uint32_t playback_position;                         /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED. */
        uint8_t status;                                     /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, #BT_AVRCP_EVENT_BATT_STATUS_CHANGED,
                                                                                                        #BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED. */
        uint8_t volume;                                     /**< The response data for #BT_AVRCP_EVENT_VOLUME_CHANGED. */
        uint16_t uid_counter;                               /**< The response data for #BT_AVRCP_EVENT_UIDS_CHANGED. */
    };
} bt_avrcp_event_notification_t;

/**
 *@brief The struct of #BT_AVRCP_REGISTER_NOTIFICATION_IND.
 */
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_event_t event_id;                              /**< The event ID for registering notification. */
    uint16_t parameter_length;                              /**< Length of parameters in the registering event. */
    uint8_t *parameters;                                    /**< Parameters of the registering event. */
} bt_avrcp_register_notification_commant_t;

/**
 *@brief The struct of #BT_AVRCP_PLAY_ITEM_COMMAND_IND.
 */
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_scope_t scope;                                 /**< The scope in which the UID of the media element item or folder item, if supported, is valid. */
    uint64_t uid;                                           /**< The UID of the media element item or folder item, if supported, to be played as defined in Section 6.10.3. */
    uint16_t uid_counter;                                   /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes.  For database unaware players uid_counter=0. */
} bt_avrcp_play_item_command_t;

/**
 *@brief The struct of #BT_AVRCP_SET_ADDRESSED_PLAYER_COMMAND_IND.
 */
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    uint16_t player_id;                                     /**< The addressed player id. */
} bt_avrcp_set_addressed_player_command_t;

/**
* @brief The struct of #BT_AVRCP_PLAY_ITEM_CNF.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    bt_avrcp_status_t status;                               /**< The status of the play item. */
} bt_avrcp_play_item_response_t;

/**
 *@brief The struct of #BT_AVRCP_GET_PLAY_STATUS_NOTIFICATION_IND.
 */

typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
} bt_avrcp_get_play_status_commant_t;

/**
* @brief The struct of #BT_AVRCP_ABORT_CONTINUING_CNF.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
} bt_avrcp_abort_continuing_response_t;

/**
 *@brief The struct of #BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF.
 */
typedef struct {
    uint32_t handle;                                        /**< The connection handle of the AVRCP. */
    uint8_t  volume;                                        /**< The absolute volume that has actually been set in the rendering device. Values ranging from 0x00 to 0x7F represent a percentage from 0% to 100%. Other values are invalid. */
} bt_avrcp_set_absolute_volume_response_t;

/**
 *@brief The struct BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND.
 */
typedef bt_avrcp_set_absolute_volume_response_t bt_avrcp_set_absolute_volume_event_t;

/**
* @brief The error structure of all metadata commands when the status of corresponding response is not #BT_STATUS_SUCCESS.
*/
typedef struct {
    uint32_t handle;                                        /**< The connection handle for the AVRCP. */
    bt_status_t rejection_reason;                           /**< The reason to reject the metadata commands when the response status is #BT_STATUS_AVRCP_REJECTED. */
} bt_avrcp_metadata_error_response_t;

/**
* @brief The struct of response data format for #BT_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED.
*/
typedef struct {
    uint16_t player_id;                                     /**< The addressed player id. */
    uint16_t uid_counter;                                   /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
} bt_avrcp_addressed_player_changed_t;

/**
 * @brief The struct of sending register notification response parameters.
 */
typedef struct {
    bt_avrcp_response_t response_type;                              /**< Type of the response. */
    uint16_t parameter_length;                                      /**< Parameter length of the corresponding event. Includes the event ID field. */
    bt_avrcp_event_t event_id;                                      /**< The register event ID for notification. */
    union {
        bt_avrcp_application_setting_changed_t setting;             /**< The response data for #BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED. */
        uint64_t id;                                                /**< The response data for #BT_AVRCP_EVENT_TRACK_CHANGED. */
        uint32_t playback_position;                                 /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED. */
        bt_avrcp_status_t status;                                   /**< The response data for #BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, #BT_AVRCP_EVENT_BATT_STATUS_CHANGED,
                                                                                                                    #BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED. */
        uint8_t volume;                                             /**< Parameter for #BT_AVRCP_EVENT_VOLUME_CHANGED. */
        uint16_t uid_counter;                                       /**< Parameter for #BT_AVRCP_EVENT_UIDS_CHANGED. */
        bt_avrcp_addressed_player_changed_t addressed_player;       /**< Parameter for #BT_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED. */
    };
} bt_avrcp_send_register_notification_response_t;

/**
 * @brief The struct of sending play status response parameters.
 */
typedef struct {
    uint32_t song_length;                                           /**< The total length (in milliseconds) of the song that is currently playing. */
    uint32_t song_position;                                         /**< The current position (in milliseconds) of the currently playing song (i.e. time elapsed). */
    bt_avrcp_media_play_status_event_t status;                     /**< Current Status of playing. */
} bt_avrcp_media_play_status_notification_t;

/**
* @brief The struct for #BT_AVRCP_GET_PLAY_STATUS_CNF.
*/
typedef struct {
    uint32_t handle;                                                /**< The connection handle of the AVRCP. */
    uint32_t song_length;                                           /**< The total length (in milliseconds) of the song that is currently playing. */
    uint32_t song_position;                                         /**< The current position (in milliseconds) of the currently playing song (i.e. time elapsed). */
    bt_avrcp_media_play_status_event_t status;                       /**< Current Status of playing. */
} bt_avrcp_media_play_status_response_t;


/**
* @brief The struct for compandy_id assigned from SIG.
*/
typedef uint8_t bt_avrcp_company_id_t[3];

/**
* @brief The struct for #BT_AVRCP_GET_CAPABILITY_CNF.
*/
typedef struct {
    uint32_t handle;                                                 /**< The connection handle of the AVRCP. */
    bt_avrcp_capability_types_t type;                                /**< The capability type that remote deivce supproted. */
    uint8_t capability_count;                                        /**< The count for the capability type. */
    union {
        bt_avrcp_company_id_t *company_id;                           /**< The company id of remote device. */
        bt_avrcp_event_t *event;                                      /**< The supported events of remote device. */
    };

} bt_avrcp_get_capability_response_t;

/**
* @brief The struct of send capability paramters
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint16_t length;                                                    /**< Specifies the number of CompanyID returned: 1-255
                                                                        1 for BT_AVRCP_CAPABILITY_COMPANY_ID; 2-255 for BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED*/
    union {
        struct {
            uint8_t number;                                             /**< The number of members in the attribute_list. */
            bt_avrcp_event_t *attribute_list;                           /**< The list of capability attribute IDs. : bt_avrcp_event_t*/
        };
        uint8_t *data;                                                  /**< Data. */

    };
} bt_avrcp_capability_attributes_response_t;

/**
* @brief The error code.
*/
#define BT_AVRCP_ERR_CODE_INVALID_COMMAND                       0x00    /**< Invalid command. Sent if TG received a PDU that it did not understand. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_INVALID_PARAMETER                     0x01    /**< Invalid parameter. Sent if the TG received a PDU with a parameter ID that it did not understand. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_PARAMETER_CONTENT_ERROR               0x02    /**< Parameter content error. Sent if the parameter ID is understood, but content is wrong or corrupted. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_INTERNAL_ERROR                        0x03    /**< Internal error. Sent if there are error conditions not covered by a more specific error code. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_OPERATION_COMPLETED_WITHOUT_ERROR     0x04    /**< Operation completed without error. This is the status that should be returned when the operation is successful. Valid for Commands: All except where the response CType is AV/C REJECTED*/
#define BT_AVRCP_ERR_CODE_UID_CHANGED                           0x05    /**< UID Changed !V The UIDs on the device have changed. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_RESERVED                              0x06    /**< Reserved. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_INVALID_DIRCTION                      0x07    /**< Invalid Direction !V The Direction parameter is invalid. Valid for Commands: Change Path*/
#define BT_AVRCP_ERR_CODE_NOT_A_DIRECTION                       0x08    /**< Not a Directory !V The given UID does not refer to a folder item. Valid for Commands: Change Path*/
#define BT_AVRCP_ERR_CODE_DOSE_NOT_EXIST                        0x09    /**< Does Not Exist !V The given UID does not refer to any currently valid item. Valid for Commands: Change Path, PlayItem, AddToNowPlaying, GetItemAttributes*/
#define BT_AVRCP_ERR_CODE_INVALID_SCOPE                         0x0A    /**< Invalid Scope !V The scope parameter is invalid. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_RANGE_OUT_OF_BOUNDS                   0x0B    /**< Range Out of Bounds !V The given start of range is not valid. Valid for Commands: GetFolderItems*/
#define BT_AVRCP_ERR_CODE_FOLDER_ITEM_IS_NOT_PLAYABLE           0x0C    /**< Folder Item is not playable !V The given UID refers to a folder item which cannot be handled by this media player. Valid for Commands: Play Item, AddToNowPlaying*/
#define BT_AVRCP_ERR_CODE_MEDIA_IN_USE                          0x0D    /**< Media in Use !V The media cannot to be used for this operation at this time. Valid for Commands: PlayItem, AddToNowPlaying*/
#define BT_AVRCP_ERR_CODE_NOW_PLAYING_LIST_FULL                 0x0E    /**< Now Playing List Full !V No more items can be added to the Now Playing List. Valid for Commands: AddToNowPlaying*/
#define BT_AVRCP_ERR_CODE_SEARCH_NOT_SUPPORTED                  0x0F    /**< Search Not Supported !V The Browsed Media Player does not support search. Valid for Commands: Search*/
#define BT_AVRCP_ERR_CODE_SEARCH_IN_PROGRESS                    0x10    /**< Search in Progress !V A search operation is already in progress. Valid for Commands: Search*/
#define BT_AVRCP_ERR_CODE_INVALID_PLAYER_ID                     0x11    /**< Invalid Player Id !V The specified Player Id does not refer to a valid player. Valid for Commands: SetAddressedPlayer, SetBrowsedPlayer*/
#define BT_AVRCP_ERR_CODE_PLAYER_NOT_BROWSABLE                  0x12    /**< Player Not Browsable !V The given Player Id refers to a Media Player which does not support browsing. Valid for Commands: SetBrowsedPlayer*/
#define BT_AVRCP_ERR_CODE_PLAYER_NOT_ADDRESSED                  0x13    /**< Player Not Addressed. The given Player Id refers to a player which is not currently addressed, and the command cannot to be performed if the player is not set as addressed. Valid for Commands: Search, SetBrowsedPlayer*/
#define BT_AVRCP_ERR_CODE_NO_VALID_SEARCH_RESULTS               0x14    /**< No valid Search Results !V The Search result list does not contain valid entries (e.g. the entries are invalidated because the browsed player changed). Valid for Commands: GetFolderItems*/
#define BT_AVRCP_ERR_CODE_NO_AVAILABLE_PLAYERS                  0x15    /**< No available players. Valid for Commands: All*/
#define BT_AVRCP_ERR_CODE_ADDRESSED_PLAYER_CHANGED              0x16    /**< Addressed Player Changed. Valid for Commands: Register Notification*/
typedef uint8_t bt_avrcp_err_code_t;                                    /**< The error code. */

/**
* @brief The struct for #BT_AVRCP_BROWSE_CONNECT_CNF.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint16_t list_mtu;                                                  /**< The maximum size of size_of_item_list for get folder item response of TG or the maximum size of size_of_attribute_list for get item attributes response of TG. */
} bt_avrcp_browse_connect_cnf_t;

/**
* @brief The struct for #BT_AVRCP_BROWSE_CONNECT_IND.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    bt_bd_addr_t *address;                                              /**< The address of the remote device to start a connection. */
} bt_avrcp_browse_connect_ind_t;

/**
* @brief The struct for #BT_AVRCP_BROWSE_DISCONNECT_IND.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
} bt_avrcp_browse_disconnect_ind_t;

/**
* @brief The struct of folder name pair
*/
BT_PACKED(
typedef struct{
    uint16_t length;                                                    /**< The length of the folder name (in octets) which follows. */
    uint8_t name[1];                                                    /**< The name of this folder. */
})bt_avrcp_folder_name_pair_t;

/**
* @brief The struct of set browsed player response
*/
typedef struct {
    bt_avrcp_err_code_t status;                                         /**< The result of the SetBrowsedPlayer operation. This is the only field present in the response when an error occurs. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    uint32_t number_of_items;                                           /**< The number of items in the current folder. This is only applicable if the SetBrowsedPlayer operation was successful. Otherwise, the value of this parameter is ignored. */
    uint16_t character_set_id;                                          /**< Specifies the character set ID to be displayed on CT as defined in IANA character set document. */
    uint8_t folder_depth;                                               /**< The number of Folder Name Length/Folder Name pairs which follow. */
    uint16_t size_of_folder_path;                                       /**< The size of folder path. */
    bt_avrcp_folder_name_pair_t *folder_path;                           /**< The folder name pair. For example, folder path A/BC/DEF is represented in UTF8 as 0x01, 'A', 0x02, "BC",0x03, "DEF" */
} bt_avrcp_browse_set_browsed_player_response_t;

/**
* @brief The struct of media player item
*/
BT_PACKED(
typedef struct{
    bt_avrcp_item_type_t item_type;                                     /**< Media player item. */
    uint16_t item_length;                                               /**< Length of item in octets, not including Item Type and Item Length fields. */
    uint16_t player_id;                                                 /**< The player id. */
    bt_avrcp_major_player_type_t major_player_type;                     /**< The major player type. */
    bt_avrcp_player_sub_type_t player_sub_type;                         /**< The player sub type. */
    bt_avrcp_play_status_t play_status;                                 /**< The player status. */
    uint8_t feature_bit_mask[16];                                       /**< The player feature bitmask. */
    uint16_t character_set_id;                                          /**< Specifies the character set ID to be displayed on CT as defined in IANA character set document. */
    uint16_t displayable_name_length;                                   /**< Length of Displayable Name in octets. The name length is limited so that a response to a GetFolderItems containing one media player item fits within the maximum size of AVRCP PDU which can be received by the CT. */
    uint8_t displayable_name[1];                                        /**< Displayable name of player. */
})bt_avrcp_media_player_item_t;

/**
* @brief The struct of folder item
*/
BT_PACKED(
typedef struct{
    bt_avrcp_item_type_t item_type;                                     /**< Folder item. */
    uint16_t item_length;                                               /**< Length of item in octets, not including Item Type and Item Length fields. */
    uint64_t folder_uid;                                                /**< UID as defined in Section 6.10.3. */
    bt_avrcp_folder_type_t folder_type;                                 /**< The folder type. */
    bt_avrcp_is_playable_t is_playable;                                 /**< Is playable. */
    uint16_t character_set_id;                                          /**< Specifies the character set ID to be displayed on CT as defined in IANA character set document. */
    uint16_t displayable_name_length;                                   /**< Length of Displayable Name in octets. The name is limited so that a response to a GetFolderItems containing one folder item fits within the maximum size of PDU which can be received by the CT. */
    uint8_t displayable_name[1];                                        /**< Displayable name of Media Element Item. */
})bt_avrcp_folder_item_t;

/**
* @brief The media type.
*/
#define BT_AVRCP_MEDIA_TYPE_AUDIO           0                           /**< Audio media. */
#define BT_AVRCP_MEDIA_TYPE_VIDEO           1                           /**< Video media. */
typedef uint8_t bt_avrcp_media_type_t;                                  /**< The media type. */

/**
* @brief The struct of attribute value entry
*/
BT_PACKED(
typedef struct{
    uint32_t id;                                                        /**< Attribute ID as defined in Section 26. */
    uint16_t character_set_id;                                          /**< Specifies the character set ID to be displayed on CT as defined in IANA character set document. */
    uint16_t value_length;                                              /**< Length of the value of this attribute. */
    uint8_t value[1];                                                   /**< The value of this attribute, in the specified character set. */
})bt_avrcp_attribute_value_entry_t;

/**
* @brief The struct of media element item
*/
BT_PACKED(
typedef struct{
    bt_avrcp_item_type_t item_type;                                     /**< Media element item. */
    uint16_t item_length;                                               /**< Length of item in octets, not including Item Type and Item Length fields. */
    uint64_t media_element_uid;                                         /**< UID as defined in Section 6.10.3. */
    bt_avrcp_media_type_t media_type;                                   /**< The media type. */
    uint16_t character_set_id;                                          /**< Specifies the character set ID to be displayed on CT as defined in IANA character set document. */
    uint16_t displayable_name_length;                                   /**< Length of Displayable Name in octets. The name is limited so that a response to a GetFolderItems containing one media player item fits within the maximum size of PDU which can be received by the CT. */
    uint8_t displayable_name[1];                                        /**< Displayable name of Media Element Item. */
    uint8_t number_of_attributes;                                       /**< The number of attributes included with this media element item. */
    bt_avrcp_attribute_value_entry_t *attribute_value_entry_list;       /**< The attribute value entry list of media element item. */
})bt_avrcp_media_element_item_t;

/**
* @brief The struct of browseable items.
*/
typedef struct {
    union {
        bt_avrcp_item_type_t item_type;                                 /**< Item item. */
        bt_avrcp_media_player_item_t media_player_item;                 /**< Media player item. */
        bt_avrcp_folder_item_t folder_item;                             /**< Folder item. */
        bt_avrcp_media_element_item_t media_element_item;               /**< Media element item. */
    };
} bt_avrcp_get_folder_items_response_value_t;

/**
* @brief The struct of sending get folder items response paramters
*/
typedef struct {
    bt_avrcp_err_code_t status;                                         /**< The result of the GetFolderItems operation. This is the only field present in the response when an error occurs. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes.  For database unaware players uid_counter=0. */
    uint16_t number_of_items;                                           /**< The number of items returned in this listing. */
    uint16_t size_of_item_list;                                         /**< The size of item list. */
    bt_avrcp_get_folder_items_response_value_t *item_list;              /**< The attributes returned with each item are the supported attributes from the list provided in the attribute list parameter of the request. */
} bt_avrcp_browse_get_folder_items_response_t;

/**
* @brief The struct of sending change path response paramters
*/
typedef struct {
    bt_avrcp_err_code_t status;                                         /**< The result of the ChangePath operation. This is the only field present in the response when an error occurs. */
    uint32_t number_of_items;                                           /**< If the ChangePath succeeded the number of items in the folder which has been changed to (i.e., the new current folder). */
} bt_avrcp_browse_change_path_response_t;

/**
* @brief The struct of sending get item attributes response parameters
*/
typedef struct {
    bt_avrcp_err_code_t status;                                         /**< The result of the GetItemAttributes operation. This is the only field present in the response when an error occurs. */
    uint8_t number_of_attributes;                                       /**< The number of members in the attribute_list. */
    uint16_t size_of_attribute_list;                                    /**< The size of attribute_list. */
    bt_avrcp_attribute_value_entry_t *attribute_value_entry_list;       /**< The attribute value entry list of the item. */
} bt_avrcp_browse_get_item_attributes_response_t;

/**
* @brief The struct of sending get total number of items response parameters
*/
typedef struct {
    bt_avrcp_err_code_t status;                                         /**< The result of the GetItemAttributes operation. This is the only field present in the response when an error occurs. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    uint32_t number_of_items;                                           /**< The number of items in this folder/scope. */
} bt_avrcp_browse_get_total_number_of_items_response_t;

/**
*@brief The struct of get folder items command parameters.
*/
typedef struct {
    bt_avrcp_scope_t scope;                                            /**< The scope in which the UID of the media element item or folder item is valid. */
    uint32_t start_item;                                               /**< The offset within the listing of the item, which should be the first returned item. The first element in the listing is at offset 0. */
    uint32_t end_item;                                                 /**< The offset within the listing of the item which should be the final returned item. If this is set to a value beyond what is available, the TG returns items from the given Start Item index to the index of the final item. If the End Item index is smaller than the Start Item index, the TG returns an error. If CT requests too many items, TG can respond with a sub-set of the requested items. */
    uint8_t attribute_count;                                           /**< 0x00: All attributes are requested. There is no following Attribute List. 0x01-0xFE: The following Attribute List contains this number of attributes. 0xFF: No attributes are requested. There is no following Attribute List.*/
    uint32_t *attribute_list;                                          /**< Attributes which are requested to be returned for each item returned. */
} bt_avrcp_browse_get_folder_items_t;

/**
*@brief The struct of change path command parameters.
*/
typedef struct {
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    bt_avrcp_direction_t direction;                                     /**< The direction of change path. */
    uint64_t folder_uid;                                                /**< The UID of the folder to navigate to. This can be retrieved by a GetFolderItems command. This field is reserved when the navigation command is Folder Up. */
} bt_avrcp_browse_change_path_t;

/**
*@brief The struct of get item attributes command parameters.
*/
typedef struct {
    bt_avrcp_scope_t scope;                                             /**< The scope in which the UID of the media element item or folder item is valid. */
    uint64_t uid;                                                       /**< The UID of the media element item or folder item to return the attributes for as defined in Section 6.10.3. UID 0 shall not be used. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    uint8_t num_of_attributes;                                          /**< The number of attribute IDs in the following Attribute ID list. All attributes are requested if this value is zero. */
    uint32_t *attribute_list;                                           /**< The attribute ID list. */
} bt_avrcp_browse_get_item_attributes_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_SET_BROWSED_PLAYER_IND.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint16_t player_id;                                                 /**< Unique Media Player Id. */
} bt_avrcp_browse_set_browsed_player_ind_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_GET_FOLDER_ITEMS_IND.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    bt_avrcp_scope_t scope;                                             /**< The scope in which the UID of the media element item or folder item is valid. */
    uint32_t start_item;                                                /**< The offset within the listing of the item, which should be the first returned item. The first element in the listing is at offset 0. */
    uint32_t end_item;                                                  /**< The offset within the listing of the item which should be the final returned item. If this is set to a value beyond what is available, the TG returns items from the given Start Item index to the index of the final item. The TG returns an error if the End Item index is smaller than the Start Item index. If CT requests too many items, TG can respond with a sub-set of the requested items. */
    uint8_t attribute_count;                                            /**< 0x00: All attributes are requested. There is no following Attribute List. 0x01-0xFE: The following Attribute List contains this number of attributes. 0xFF: No attributes are requested. There is no following Attribute List.*/
    uint32_t *attribute_list;                                           /**< The pointer of the attribute list. Attributes which are requested to be returned for each item returned. */
} bt_avrcp_browse_get_folder_items_ind_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_CHANGE_PATH_IND.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    bt_avrcp_direction_t direction;                                     /**< The direction of change path. */
    uint64_t folder_uid;                                                /**< The UID of the folder to navigate to. This can be retrieved by a GetFolderItems command. This field is reserved when the navigation command is Folder Up. */
} bt_avrcp_browse_change_path_ind_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_GET_ITEM_ATTRIBUTES_IND.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    bt_avrcp_scope_t scope;                                             /**< The scope in which the UID of the media element item or folder item is valid. */
    uint64_t uid;                                                       /**< The UID of the media element item or folder item to return the attributes for as defined in Section 6.10.3. UID 0 cannot be used. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    uint8_t num_of_attributes;                                          /**< The number of attribute IDs in the following Attribute ID list. All attributes are requested if this value is zero. */
    uint32_t *attribute_list;                                           /**< The pointer of attribute id list. The attribute ID list. */
} bt_avrcp_browse_get_item_attributes_ind_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_GET_TOTAL_NUMBER_OF_ITEMS_IND.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    bt_avrcp_scope_t scope;                                             /**< The scope in which the UID of the media element item or folder item is valid. */
} bt_avrcp_browse_get_total_number_of_items_ind_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_SET_BROWSED_PLAYER_CNF.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    uint32_t number_of_items;                                           /**< The number of items in the current folder. This is only applicable if the SetBrowsedPlayer operation was successful. Otherwise, the value of this parameter is ignored. */
    uint16_t character_set_id;                                          /**< Specifies the character set ID to be displayed on CT as defined in IANA character set document. */
    uint8_t folder_depth;                                               /**< The number of Folder Name Length/Folder Name pairs which follow. */
    uint16_t size_of_folder_path;                                       /**< The size of the folder path. */
    bt_avrcp_folder_name_pair_t *folder_path;                           /**< The folder name pair. */
} bt_avrcp_browse_set_browsed_player_cnf_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_GET_FOLDER_ITEMS_CNF.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    uint16_t number_of_items;                                           /**< The number of items returned in this listing. */
    uint16_t size_of_item_list;                                         /**< The size of the item list. */
    bt_avrcp_get_folder_items_response_value_t *items_list;             /**< The attributes returned with each item are the supported attributes from the list provided in the attribute list parameter of the request. */
} bt_avrcp_browse_get_folder_items_cnf_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_CHANGE_PATH_CNF.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint32_t number_of_items;                                           /**< If the ChangePath succeeded the number of items in the folder which has been changed to (i.e., the new current folder). */
} bt_avrcp_browse_change_path_cnf_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_GET_ITEM_ATTRIBUTES_CNF.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
    uint8_t number_of_attributes;                                       /**< The number of members in the attribute_list. */
    uint16_t size_of_attribute_list;                                    /**< The size of the attribute list. */
    bt_avrcp_attribute_value_entry_t *attribute_value_entry_list;       /**< The attribute list. */
} bt_avrcp_browse_get_item_attributes_cnf_t;

/**
*@brief The struct of #BT_AVRCP_BROWSE_SET_BROWSED_PLAYER_CNF when receive general reject response.
*/
typedef struct {
    uint32_t handle;                                                    /**< The connection handle of the AVRCP. */
} bt_avrcp_browse_general_cnf_t;

/**
 * @}
 */


/**
 * @brief                 This function connects to the specified remote
 *                        device. The #BT_AVRCP_CONNECT_CNF event is sent to the upper layer
 *                        with the connection request result.
 * @param[out] handle     is the connection handle of the specified remote device. The handle is only valid when the return value is #BT_STATUS_SUCCESS.
 * @param[in]  address    is a pointer to the remote device's address.
 * @return                #BT_STATUS_SUCCESS, the connection request started successfully. #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                        #BT_STATUS_AVRCP_LINK_ALREADY_EXISTED, the AVRCP channel already exists.
 */
bt_status_t bt_avrcp_connect(uint32_t *handle, const bt_bd_addr_t *address);

/**
 * @brief                 This function responds to the specified remote device's incoming connection.
 *                        The function should be called after #BT_AVRCP_CONNECT_IND is received. The #BT_AVRCP_CONNECT_CNF event is sent to the
 *                        upper layer with the connection request result.
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] accept      is whether to accept or reject the remote device's connection request.
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 */
bt_status_t bt_avrcp_connect_response(uint32_t handle, bool accept);

/**
 * @brief             This function disconnects the specified remote device.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @return            #BT_STATUS_SUCCESS, the disconnect request completed successfully. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
 */
bt_status_t bt_avrcp_disconnect(uint32_t handle);

/**
 * @brief               This function sends pass through command to
 *                      the specified TG device. #BT_AVRCP_PASS_THROUGH_CNF event is
 *                      sent to the upper layer with the request result.
 * @param[in] handle    is the AVRCP handle for the specified remote device.
 * @param[in] op_id     is the operation ID.
 * @param[in] op_state  is the operation state.
 * @return              #BT_STATUS_SUCCESS, the pass through command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                      #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_send_pass_through_command(uint32_t handle, bt_avrcp_operation_id_t op_id, bt_avrcp_operation_state_t op_state);

/**
 * @brief               This function send pass through response after receiving pass through command from the remote CT device.
 *                      The function should only be called after #BT_AVRCP_PASS_THROUGH_COMMAND_IND is received.
 * @param[in] handle    is the AVRCP handle of the remote device.
 * @param[in] response  is the type of response. #BT_AVRCP_RESPONSE_NOT_IMPLEMENTED or #BT_AVRCP_RESPONSE_ACCEPTED or #BT_AVRCP_RESPONSE_REJECTED.
 * @param[in] op_id     is the corresponding operation ID.
 * @param[in] op_state  is the corresponding operation state. #BT_AVRCP_OPERATION_STATE_PUSH or #BT_AVRCP_OPERATION_STATE_RELEASED.
 * @return              #BT_STATUS_SUCCESS, the pass through command is sent successfully. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
                        #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_send_pass_through_response(uint32_t handle, bt_avrcp_response_t response, bt_avrcp_operation_id_t op_id, bt_avrcp_operation_state_t op_state);

/**
 * @brief             This function gets player application attributes
 *                    supported by the TG device. #BT_AVRCP_LIST_APP_SETTING_ATTRIBUTES_CNF event
 *                    is sent to the upper layer with the request result.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @return            #BT_STATUS_SUCCESS, the list app setting command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_list_app_setting_attributes(uint32_t handle);

/**
 * @brief                    This function gets the player application setting values
 *                           for specified attribute ID at the TG device. #BT_AVRCP_GET_APP_SETTING_VALUE_CNF event
 *                           is sent to the upper layer with the request result.
 * @param[in] handle         is the AVRCP handle for the specified remote device.
 * @param[in] attribute_size is the total size of the list of attribute ID.
 * @param[in] attribute_list is the list of specified attribute ID.
 * @return                   #BT_STATUS_SUCCESS, the get app setting command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                           #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_get_app_setting_value(uint32_t handle, uint16_t attribute_size, bt_avrcp_get_app_setting_value_t *attribute_list);

/**
 * @brief                          This function sets the player application setting values
 *                                 to specified attribute ID at the TG device. #BT_AVRCP_SET_APP_SETTING_VALUE_CNF event
 *                                 is sent to upper layer with the request result.
 * @param[in] handle               is the AVRCP handle for the specified remote device.
 * @param[in] attribute_size       is the total size of the list of attribute value and corresponding attribute ID.
 * @param[in] attribute_value_list is the list of attribute value and corresponding attribute ID.
 * @return                         #BT_STATUS_SUCCESS, the set app setting command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                                 #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_set_app_setting_value(uint32_t handle, uint16_t attribute_size, bt_avrcp_app_setting_value_t *attribute_value_list);

/**
 * @brief                          This function gets the status of the currently playing media
 *                                 #BT_AVRCP_GET_PLAY_STATUS_CNF event is sent to upper layer with the request result.
 *
 * @param[in] handle               is the AVRCP handle for the specified remote device.
 * @return                         #BT_STATUS_SUCCESS, the set app setting command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                                 #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_get_play_status(uint32_t handle);

/**
 * @brief                          This function gets the specified capability of the currently playing media,
 *                                 #BT_AVRCP_GET_CAPABILITY_CNF event is sent to upper layer with the request result.
 *
 * @param[in] handle               is the AVRCP handle for the specified remote device.
 * @param[in] type                 is the specified capability type.
 * @return                         #BT_STATUS_SUCCESS, the get capability command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                                 #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */

bt_status_t bt_avrcp_get_capability(uint32_t handle, bt_avrcp_capability_types_t type);
/**
 * @brief                     This function obtains the detailed information on a
 *                            particular media file playing currently at the TG device. The event
 *                            #BT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF is sent to the upper layer
 *                            with the request result.
 * @param[in] handle          is the AVRCP handle for the specified remote device.
 * @param[in] attribute_size  is the total size of the list of media attribute ID.
 * @param[in] attribute_list  is the list of media attribute ID.
 * @return                    #BT_STATUS_SUCCESS, the get element attributes command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                            #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_get_element_attributes(uint32_t handle, uint16_t attribute_size, bt_avrcp_get_element_attributes_t *attribute_list);

/**
 * @brief                        This function sends a register notification command to the
 *                               TG device. #BT_AVRCP_EVENT_NOTIFICATION_IND event
 *                               is sent to the upper layer with the request result.
 * @param[in] handle             is the AVRCP handle for the specified remote device.
 * @param[in] event_id           is the related AVRCP event ID.
 * @param[in] play_back_interval is the time interval (in seconds) at which the change in
 *                               playback position is noticed.
 * @return                       #BT_STATUS_SUCCESS, the register command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                               #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_register_notification(uint32_t handle, bt_avrcp_event_t event_id, uint32_t play_back_interval);

/**
 * @brief             This function requests TG device to send continuing response packet for current PDU command.
 *                    The function should be called when the packet type of metadata response is start or continue.
 *                    The response of current PDU command is sent to the upper layer with the request.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @param[in] pdu_id  is the current PDU id for the corresponding command.
 * @return            #BT_STATUS_SUCCESS, the request continuing command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_request_continuing_response(uint32_t handle, bt_avrcp_pdu_id_t pdu_id);

/**
 * @brief             This function requests TG device to abort continuing response packet for current PDU command.
 *                    The function should be called when the packet type of metadata response is start or continue.
 *                    #BT_AVRCP_ABORT_CONTINUING_CNF is sent to the upper layer with the request.
 * @param[in] handle  is the AVRCP handle for the specified remote device.
 * @param[in] pdu_id  is the current PDU id for the corresponding command.
 * @return            #BT_STATUS_SUCCESS, the abort continuing command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_abort_continuing_response(uint32_t handle, bt_avrcp_pdu_id_t pdu_id);

/**
 * @brief             This function sets the volume in percentage for the rendering device.
 *                    Note that the actual volume level set in the rendering device is returned through the volume field of #bt_avrcp_set_absolute_volume_response_t in the event #BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF.
 * @param[in] handle  is the AVRCP handle of the remote device.
 * @param[in] volume  is the volume to set in percentage. The top bit (bit 7) is reserved. Values ranging from 0x00 to 0x7F specify 0% to 100% of volume. Other values are invalid.
 * @return            #BT_STATUS_SUCCESS, the set absolute volume command is sent successfully. #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 *                    #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL. #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_set_absolute_volume(uint32_t handle, uint8_t volume);

/**
 * @brief                   This function starts playing an item indicated by the UID. It is routed to the Addressed Player.
 *                          The #BT_AVRCP_PLAY_ITEM_CNF event is sent to the upper layer with the command result.
 *
 * @param[in] handle        is the AVRCP handle for the specified remote device.
 * @param[in] param         is the parameter for the play item command
 * @return                  #BT_STATUS_SUCCESS, the set browsed player command is sent successfully.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_AVRCP_BUSY, a browsing command is already sent.
 */
bt_status_t bt_avrcp_play_item(uint32_t handle, bt_avrcp_play_item_t *param);

/**
 * @brief                 This function sends the response of the play item to the remote device .
 *                          The function should be called after #BT_AVRCP_PLAY_ITEM_COMMAND_IND is received.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] status         the status of the response
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 */
bt_status_t bt_avrcp_send_play_item_response(uint32_t handle, bt_avrcp_err_code_t status);

/**
 * @brief             This function sends the response after receiving the set absolute volume command from the CT device.
 *                    The function should only be called after #BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND is received.
 *
 * @param[in] handle  is the AVRCP handle of the remote device.
 * @param[in] volume  is the actual volume to set (as a percentage). TG can accept the assigned volume in #BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND command or
 *                    return the actual set volume value.
 * @return            #BT_STATUS_SUCCESS, the set absolute volume response is sent successfully. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
                      #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_send_set_absoulte_volume_response(uint32_t handle, uint8_t volume);

/**
 * @brief              This function sends the register notification response to the CT device.
 *                     The function should only be called in the following two cases:
 *                     1. Register notification command #BT_AVRCP_REGISTER_NOTIFICATION_IND is received with valid parameters.
 *                     2. The register notification command is received and the corresponding registered event occurs.
 *
 * @param[in] handle   is the AVRCP handle of the remote device.
 * @param[in] rsp_data is the pointer to the register notification response parameters.
 * @return             #BT_STATUS_SUCCESS, the register notification response is sent successfully. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
                       #BT_STATUS_OUT_OF_MEMORY, the TX memory is not enough.
 */
bt_status_t bt_avrcp_send_register_notification_response(uint32_t handle, bt_avrcp_send_register_notification_response_t *rsp_data);

/**
 * @brief                 This function sends the response of the set addressed player to the remote device .
 *                          The function should be called after #BT_AVRCP_SET_ADDRESSED_PLAYER_COMMAND_IND is received.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] status         the status of the response
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 */
bt_status_t bt_avrcp_send_set_addressed_player_response(uint32_t handle, bt_avrcp_err_code_t status);

/**
 * @brief                   This function connects to the specified remote device for a browsing channel device.
 *                          The #BT_AVRCP_BROWSE_CONNECT_CNF event is sent to the upper layer with the connection request result.
 *
 * @param[out] handle       is the connection handle of the specified remote device. The handle is only valid when the return value is #BT_STATUS_SUCCESS.
 * @param[in]  address      is a pointer to the remote device's address.
 * @return                  #BT_STATUS_SUCCESS, the connection request started successfully.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_AVRCP_LINK_ALREADY_EXISTED, the AVRCP browsing channel already exists.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init().
 *                          #BT_STATUS_UNSUPPORTED, the AVRCP browsing channel is not supported by the remote device.
 */
bt_status_t bt_avrcp_browse_connect(uint32_t *handle, const bt_bd_addr_t *address);

/**
 * @brief                   This function responds to the specified remote device's incoming connection for a browsing channel.
 *                          The function should be called after #BT_AVRCP_BROWSE_CONNECT_IND is received. The #BT_AVRCP_BROWSE_CONNECT_CNF event is sent to the upper layer with the connection request result.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] accept        is whether to accept or reject the remote device's connection request.
 * @return                  #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 */
bt_status_t bt_avrcp_browse_connect_response(uint32_t handle, bool accept);

/**
 * @brief                   This function disconnects the specified remote device for a browsing channel.
  *                         The #BT_AVRCP_BROWSE_DISCONNECT_IND event is sent to the upper layer with the connection that is disconnected.
 *
 * @param[in] handle        is the AVRCP handle for the specified remote device.
 * @return                  #BT_STATUS_SUCCESS, the disconnect request completed successfully.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_FAIL, the state is not connected.
 */
bt_status_t bt_avrcp_browse_disconnect(uint32_t handle);

/**
 * @brief                   This function sets the browsed player for a browsing channel.
 *                          The #BT_AVRCP_BROWSE_SET_BROWSED_PLAYER_CNF event is sent to the upper layer with the connection request result.
 *
 * @param[in] handle        is the AVRCP handle for the specified remote device.
 * @param[in] player_id     is the browsed player id
 * @return                  #BT_STATUS_SUCCESS, the set browsed player command is sent successfully.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 *                          #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 */
bt_status_t bt_avrcp_browse_set_browsed_player(uint32_t handle, uint16_t player_id);

/**
 * @brief             This function gets the folder items for a browsing channel.
 *                          The #BT_AVRCP_BROWSE_GET_FOLDER_ITEMS_CNF event is sent to the upper layer with the connection request result.
 *
 * @param[in] handle        is the AVRCP handle for the specified remote device.
 * @param[in] param         is the parameters of the get folder items command
 * @return            #BT_STATUS_SUCCESS, the get folder items command is sent successfully.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 *                          #BT_STATUS_AVRCP_BUSY, the last command did not receive a response.
 */
bt_status_t bt_avrcp_browse_get_folder_items(uint32_t handle, bt_avrcp_browse_get_folder_items_t *param);

/**
 * @brief             This function changes the path for a browsing channel.
 *                          The #BT_AVRCP_BROWSE_CHANGE_PATH_CNF event is sent to the upper layer with the connection request result.
 *
 * @param[in] handle        is the AVRCP handle for the specified remote device.
 * @param[in] param         is the parameters of the change path command
 * @return                  #BT_STATUS_SUCCESS, the change path command is sent successfully.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 *                          #BT_STATUS_AVRCP_BUSY, the last command did not receive a response.
 */
bt_status_t bt_avrcp_browse_change_path(uint32_t handle, bt_avrcp_browse_change_path_t *param);

/**
 * @brief             This function gets item attributes for a browsing channel.
 *                          The #BT_AVRCP_BROWSE_GET_ITEM_ATTRIBUTES_CNF event is sent to the upper layer with the connection request result.
 *
 * @param[in] handle        is the AVRCP handle for the specified remote device.
 * @param[in] param         is the parameters of the get item attributes command
 * @return                  #BT_STATUS_SUCCESS, the get item attributes command is sent successfully.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle is NULL.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 *                          #BT_STATUS_AVRCP_BUSY, the last command did not receive any response.
 */
bt_status_t bt_avrcp_browse_get_item_attributes(uint32_t handle, bt_avrcp_browse_get_item_attributes_t *param);

/**
 * @brief                 This function sends the response of the set browsed player to the remote device.
 *                          The function should be called after #BT_AVRCP_BROWSE_SET_BROWSED_PLAYER_IND is received.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] param         the parameters of the response
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL or size_of_folder_path exceeds list_mtu parameter of #BT_AVRCP_BROWSE_CONNECT_CNF.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 */
bt_status_t bt_avrcp_browse_set_browsed_player_response(uint32_t handle, bt_avrcp_browse_set_browsed_player_response_t *param);

/**
 * @brief                 This function sends the response of the get folder items to the remote device.
 *                          The function should be called after #BT_AVRCP_BROWSE_GET_FOLDER_ITEMS_IND is received.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] param         the parameters of the response
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL or size_of_item_list exceeds list_mtu parameter of #BT_AVRCP_BROWSE_CONNECT_CNF.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 */
bt_status_t bt_avrcp_browse_get_folder_items_response(uint32_t handle, bt_avrcp_browse_get_folder_items_response_t *param);

/**
 * @brief                 This function sends the response of the change path to the remote device.
 *                          The function should be called after #BT_AVRCP_BROWSE_CHANGE_PATH_IND is received.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] param         the parameters of the response
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 */
bt_status_t bt_avrcp_browse_change_path_response(uint32_t handle, bt_avrcp_browse_change_path_response_t *param);

/**
 * @brief                 This function sends the response of the get item attributes to the remote device.
 *                          The function should be called after #BT_AVRCP_BROWSE_GET_ITEM_ATTRIBUTES_IND is received.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] param         the parameters of the response
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL or size_of_attribute_list exceeds list_mtu parameter of #BT_AVRCP_BROWSE_CONNECT_CNF..
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 */
bt_status_t bt_avrcp_browse_get_item_attributes_response(uint32_t handle, bt_avrcp_browse_get_item_attributes_response_t *param);

/**
 * @brief                 This function sends the response of the get total number of items to the remote device.
 *                          The function should be called after #BT_AVRCP_BROWSE_GET_TOTAL_NUMBER_OF_ITEMS_IND is received.
 *
 * @param[in] handle        is the AVRCP connection handle for the specified remote device.
 * @param[in] param         the parameters of the response
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request.
 *                          #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 *                          #BT_STATUS_AVRCP_BROWSE_UNSUPPORTED, the AVRCP browsing channel is not supported by the parameter of function bt_avrcp_init.
 *                          #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 *                          #BT_STATUS_FAIL, the state is not connected.
 */
bt_status_t bt_avrcp_browse_get_total_number_of_items_response(uint32_t handle, bt_avrcp_browse_get_total_number_of_items_response_t *param);

/**
 * @brief                 This function init the avrcp context with the role we want to follow like controller or target
 * @param[in] init_data      is initialization data.
 */
void bt_avrcp_init(bt_avrcp_init_t *init_data);

/**
 * @brief                 This function remove the role from the AVRCP context.
 */
void bt_avrcp_deinit(void);

/**
 * @brief                 This function sends notification to the remote device for currenct status of playing media.//SongLength, SongPosition etc.
 *                        The function should be called after #BT_AVRCP_GET_PLAY_STATUS_NOTIFICATION_IND is received.
 *
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] data        notification data to be transfered to the remote device:
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 */
bt_status_t bt_avrcp_media_send_play_status_response(uint32_t handle, bt_avrcp_media_play_status_notification_t *data);

/**
 * @brief                 This function sends a response for the abort continuation pdu.
 *                        The function should be called after #BT_AVRCP_CONTINUATION_ABORT_IND is received.
 *
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_OUT_OF_MEMORY, not enough memory allocated for the AVRCP.
 */
bt_status_t bt_avrcp_send_abort_data_response(uint32_t handle);

/**
 * @brief                 This function provides metadata attributes to the remote device for some specific element(s).
 *                        // bt_avrcp_media_attribute_t : 0x01 : Title, 0x02 : Artist Name, 0x3 : Album Name etc.
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] rsp_data    metadata attributes to transfer/send to the remote device:
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 */
bt_status_t
bt_avrcp_element_metadata_attributes_response(uint32_t handle, bt_avrcp_get_element_attributes_response_t *rsp_data);

/**
 * @brief                 This function sends device/profile capability to the remote device .
 *                        bt_avrcp_capability_types_t
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[in] response_data_t    device/profile capability attributes to transfer/send to the remote device:
 * @param[in] type        the capability type.
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL.
 */
bt_status_t bt_avrcp_send_capability_response(uint32_t handle, void *response_data_t, bt_avrcp_capability_types_t type);

/**
 * @brief                 This is a user defined function to acquire the company ID for sending VENDOR DEPENDENT commands.
 *                        If user don't define this function, the company ID will fill with the SIG default company ID.
 * @return                A pointer to the company ID.
 */
const uint8_t *bt_avrcp_get_company_id(void);

/**
 * @brief                 This function gets the remote device version.
 * @param[in] handle      is the AVRCP connection handle for the specified remote device.
 * @param[out] version    the version of remote device.
 * @return                #BT_STATUS_SUCCESS, successfully responded to the connection request. #BT_STATUS_AVRCP_INVALID_INPUT_PARAMETER, the handle or address is NULL. #BT_STATUS_FAIL, the status of AVRCP is incorrect.
 */
bt_status_t bt_avrcp_get_remote_device_version(uint32_t handle, uint16_t *version);

/**
 * @brief                 This is a user defined function to enable or disable avrcp sdp record.
 * @param[in] is_disable      The settings to enable or disable the avrcp service record.
 */
void bt_avrcp_disable_sdp(bool is_disable);

BT_EXTERN_C_END

/**
 * @}
 * @}
 */


#endif

