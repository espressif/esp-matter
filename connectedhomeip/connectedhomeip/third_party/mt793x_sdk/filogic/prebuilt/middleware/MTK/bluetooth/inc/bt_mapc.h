/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
/* Airoha restricted information */

#ifndef __BT_MAPC_H__
#define __BT_MAPC_H__
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothMAP MAP
 * @{
 * The Message Access Profile (MAP) defines the procedures to get and send messages from a peer phone device.
 * The MAP client is implemented for the client side, defined as MCE in the MAP specification which supports the MAS client and the MNS server.
 *
 * Terms and Acronyms
 * ======
 * |Terms                        |Details                                                                  |
 * |-----------------------------|-------------------------------------------------------------------------|
 * |\b MAP                       | Message Access Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Message_Access_Profile_(MAP)">Wikipedia</a>. |
 * |\b MCE                       | Message Client Equipment. The device that uses the message repository to browse existing messages and uploading messages. |
 * |\b MSE                       | Message Server Equipment. The device that provides the message repository. |
 * |\b MAS                       | Message Access Server. The service that the MCE uses to control the MSE. |
 * |\b MNS                       | Message Notification Server. The service that the MSE sends event to the MCE. |
 * @section bt_mapc_api_usage How to use this module
 * - Establishing MAS connection with an MSE.
 *  - Step1: Mandatory, call the function #bt_mapc_sdp_search_mas() to search if there is any MAS on the SDP record.
 *   - Sample code:
 *      @code
 *         bt_mapc_sdp_search_mas((const bt_bd_addr_t *)&addr);
 *         bt_mapc_connect(&handle, (const bt_bd_addr_t *)&addr, index);
 *      @endcode
 *   - The event #BT_MAPC_MAS_SDP_SEARCH_CNF is received first with the number of MAS, and the event #BT_MAPC_MAS_SDP_SEARCHED_IND is received when each MAS record is parsed.
 *   - Then call the function #bt_mapc_connect() to connect a specific MAS.
 *   - Sample code:
 *      @code
 *        bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *        {
 *            switch (event_id)
 *            {
 *                 case BT_MAPC_MAS_SDP_SEARCH_CNF:
 *                 {
 *                     bt_mapc_sdp_search_cnf_t *evt = (bt_mapc_sdp_search_cnf_t *)buff;
 *                      // Show the number of MAS.
 *                     break;
 *                 }
 *                 case BT_MAPC_MAS_SDP_SEARCHED_IND:
 *                 {
 *                     bt_mapc_sdp_searched_ind_t *evt = (bt_mapc_sdp_searched_ind_t *)buff;
 *                     // Show the name and the instance ID of MAS.
 *                     break;
 *                 }
 *                default:
 *                    break;
 *            }
 *        }
 *      @endcode
 *  - Step2: Optional, call the function #bt_mapc_set_notification_registration() to register notification. The connection of the MNS will be established by the peer device.
 *   - Sample code:
 *      @code
 *         bt_mapc_set_notification_registration(handle, notification_registration);
 *      @endcode
 *  - Step3: Optional, browsing the repository of MSE. Call the functions #bt_mapc_get_folder_listing() #bt_mapc_set_folder() to change the directory.
 *           The directory should be changed one layer by one layer.
 *           Then use the functions #bt_mapc_get_message_listing() and #bt_mapc_get_message() to browse.
 *   - Sample code:
 *      @code
 *         bt_mapc_get_folder_listing(handle, max_count, offset);
 *         bt_mapc_set_folder(handle, 0, path, length);
 *         bt_mapc_get_message_listing(handle, NULL, 0, max_count, offset, 0);
 *         bt_mapc_get_message(handle, message_handle, (bt_mapc_charset_t)charset);
 *      @endcode
 *  - Step4: Optional, uploading a message via MSE, call the functions #bt_mapc_put_message().
 *   - Sample code:
 *      @code
 *         bt_mapc_put_message(handle, NULL, 0, charset, &bmessage);
 *      @endcode
 */

#include "bt_type.h"
#include "bt_system.h"
#include "bt_debug.h"
#include "bt_platform.h"
/**
 * @defgroup Bluetoothbt_mapc_define Define
 * @{
 */

/**
 * @brief The following macros define the MAPC event types.
 */
#define BT_MAPC_MAS_SDP_SEARCH_CNF                         (BT_MODULE_MAP|0x01) /**<  The confirmation event as a result of sending the SDP search request, with #bt_mapc_sdp_search_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_CONNECT_CNF                            (BT_MODULE_MAP|0x02) /**<  The confirmation event as a result of connecting the MAS, with #bt_mapc_connect_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_DISCONNECT_IND                         (BT_MODULE_MAP|0x03) /**<  The MAS disconnection indication event, initiated by a local or a remote device, with #bt_mapc_disconnect_ind_t as the payload in the callback function. */
#define BT_MAPC_MAS_SDP_SEARCHED_IND                       (BT_MODULE_MAP|0x04) /**<  The indication event when the MAS is found is SDP record, with #bt_mapc_sdp_searched_ind_t as the payload in the callback function. */
#define BT_MAPC_MAS_SET_NOTIFICATION_REGISTRATION_CNF      (BT_MODULE_MAP|0x05) /**<  The confirmation event as a result of setting the notification registration, with #bt_mapc_set_notification_registration_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_SET_FOLDER_CNF                         (BT_MODULE_MAP|0x06) /**<  The confirmation event as a result of setting folder, with #bt_mapc_set_folder_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_GET_FOLDER_LISTING_CNF                 (BT_MODULE_MAP|0x07) /**<  The confirmation event as a result of getting folder listing, with #bt_mapc_get_folder_listing_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_GET_MESSAGES_LISTING_CNF               (BT_MODULE_MAP|0x08) /**<  The confirmation event as a result of getting messages listing, with #bt_mapc_get_message_listing_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_GET_MESSAGE_CNF                        (BT_MODULE_MAP|0x09) /**<  The confirmation event as a result of getting a message, with #bt_mapc_get_message_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_PUT_MESSAGE_CNF                        (BT_MODULE_MAP|0x0A) /**<  The confirmation event as a result of putting a message, with #bt_mapc_put_message_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_UPDATE_INBOX_CNF                       (BT_MODULE_MAP|0x0B) /**<  The confirmation event as a result of updating the inbox, with #bt_mapc_update_inbox_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_SET_MESSAGE_STATUS_CNF                 (BT_MODULE_MAP|0x0C) /**<  The confirmation event as a result of setting the message status, with #bt_mapc_set_message_status_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_SET_NOTIFICATION_FILTER_CNF            (BT_MODULE_MAP|0x0D) /**<  The confirmation event as a result of setting the notification filter, with #bt_mapc_set_notification_filter_cnf_t as the payload in the callback function. */
#define BT_MAPC_MAS_ABORT_CNF                              (BT_MODULE_MAP|0x0E) /**<  The confirmation event as a result of sending abort request to the peer device, with #bt_mapc_abort_cnf_t as the payload in the callback function. */
#define BT_MAPC_MNS_CONNECT_IND                            (BT_MODULE_MAP|0x0F) /**<  The indication event that the MNS is connected by the peer device, with #bt_mapc_connect_ind_t as the payload in the callback function. */
#define BT_MAPC_MNS_DISCONNECT_IND                         (BT_MODULE_MAP|0x10) /**<  The MNS disconnection indication event, initiated by a local or a remote device, with #bt_mapc_disconnect_ind_t as the payload in the callback function. */
#define BT_MAPC_MNS_SEND_EVENT_IND                         (BT_MODULE_MAP|0x11) /**<  The indication event that an MNS event is received, with #bt_mapc_send_event_ind_t as the payload in the callback function. */

/**
 * @brief Filter out message type: where 0 = "no filtering, get this type",  1 = "filter out this type".
 */
typedef uint8_t bt_mapc_filter_out_message_t;
#define BT_MAPC_FILTER_OUT_TYPE_SMS_GSM         0x01 /**< Fiter out SMS GSM message type. */
#define BT_MAPC_FILTER_OUT_TYPE_SMS_CDMA        0x02 /**< Fiter out SMS CDMA message type. */
#define BT_MAPC_FILTER_OUT_TYPE_EMAIL           0x04 /**< Fiter out Email message type. */
#define BT_MAPC_FILTER_OUT_TYPE_MMS             0x08 /**< Fiter out MMS message type. */
#define BT_MAPC_FILTER_OUT_TYPE_IM              0x10 /**< Fiter out IM message type. */

/**
 * @brief Message status.
 */
typedef uint8_t bt_mapc_message_status_t;
#define BT_MAPC_MESSAGE_STATUS_UNREAD           0x00 /**< Message is unread. */
#define BT_MAPC_MESSAGE_STATUS_READ             0x01 /**< Message is read. */
#define BT_MAPC_MESSAGE_STATUS_UNKNOWN          0xFF /**< Message is unknown. */

/**
 * @brief Message charset.
 */
typedef uint8_t bt_mapc_charset_t;
#define BT_MAPC_CHARSET_NATIVE                  0x00 /**< Charset is native. */
#define BT_MAPC_CHARSET_UTF8                    0x01 /**< Charset is UTF-8. */
#define BT_MAPC_CHARSET_UNKNOWN                 0xFF /**< Charset is unknown. */

/**
 * @brief Message priority.
 */
typedef uint8_t bt_mapc_message_priority_t;
#define BT_MAPC_MESSAGE_PRIORITY_NOT_HIGH       0x00 /**< Message is not of high priority. */
#define BT_MAPC_MESSAGE_PRIORITY_HIGH           0x01 /**< Message is of high priority. */
#define BT_MAPC_MESSAGE_PRIORITY_UNKNOWN        0xFF /**< Message is of unknown priority. */

/**
 * @brief Message type.
 */
typedef uint8_t bt_mapc_message_t;
#define BT_MAPC_MESSAGE_TYPE_SMS_GSM         0x00 /**< SMS GSM message type. */
#define BT_MAPC_MESSAGE_TYPE_SMS_CDMA        0x01 /**< SMS CDMA message type. */
#define BT_MAPC_MESSAGE_TYPE_EMAIL           0x02 /**< Email message type. */
#define BT_MAPC_MESSAGE_TYPE_MMS             0x03 /**< MMS message type. */
#define BT_MAPC_MESSAGE_TYPE_IM              0x04 /**< IM message type. */
#define BT_MAPC_MESSAGE_TYPE_UNKNOWN         0xFF /**< Unknown message type. */

/**
 * @brief Folder-listing type.
 */
typedef uint8_t bt_mapc_folder_listing_t;
#define BT_MAPC_FOLDER_LISTING_TYPE_FILE         0x00 /**< A file type object. */
#define BT_MAPC_FOLDER_LISTING_TYPE_FOLDER       0x01 /**< A folder type object. */

/**
 * @brief Notification type.
 */
typedef uint16_t bt_mapc_event_report_t;
#define BT_MAPC_EVENT_REPORT_NEW_MESSAGE                        0x0001 /**< Notification for new message. */
#define BT_MAPC_EVENT_REPORT_MESSAGE_DELETED                    0x0002 /**< Notification for message deleted. */
#define BT_MAPC_EVENT_REPORT_MESSAGE_SHIFT                      0x0004 /**< Notification for message shift. */
#define BT_MAPC_EVENT_REPORT_SENDING_SUCCESS                    0x0008 /**< Notification for sending success. */
#define BT_MAPC_EVENT_REPORT_SENDING_FAILURE                    0x0010 /**< Notification for sending failure. */
#define BT_MAPC_EVENT_REPORT_DELIVERY_SUCCESS                   0x0020 /**< Notification for delivery success. */
#define BT_MAPC_EVENT_REPORT_DELIVERY_FAILURE                   0x0040 /**< Notification for delivery failure. */
#define BT_MAPC_EVENT_REPORT_MEMORY_FULL                        0x0080 /**< Notification for memory full. */
#define BT_MAPC_EVENT_REPORT_MEMORY_AVAILABLE                   0x0100 /**< Notification for memory available. */
#define BT_MAPC_EVENT_REPORT_READ_STATUS_CHANGED                0x0200 /**< Notification for read status changed. */
#define BT_MAPC_EVENT_REPORT_CONVERSATION_CHANGED               0x0400 /**< Notification for conversation changed. */
#define BT_MAPC_EVENT_REPORT_PARTICIPANT_PRESENCE_CHANGED       0x0800 /**< Notification for participant presence changed. */
#define BT_MAPC_EVENT_REPORT_PARTICIPANT_CHAT_STATE_CHANGED     0x1000 /**< Notification for participant chat state changed. */
#define BT_MAPC_EVENT_REPORT_MESSAGE_EXTENDED_DATA_CHANGED      0x2000 /**< Notification for message extended data changed. */
#define BT_MAPC_EVENT_REPORT_MESSAGE_REMOVED                    0x4000 /**< Notification for message removed. */
#define BT_MAPC_EVENT_REPORT_UNKNOWN                            0x0000 /**< Notification for unknown. */
/**
 * @}
*/
/**
 * @defgroup Bluetoothbt_mapc_struct Struct
 * @{
 */

/**
 *  @brief This structure defines the name and number of a contact information.
 */
typedef struct {
    uint8_t number_length;                   /**< The length of the number. */
    uint8_t name_length;                     /**< The length of the name. */
    uint8_t *number;                         /**< The content of the number. */
    uint8_t *name;                           /**< The content of the name. */
} bt_mapc_contact_information_t;

/**
 *  @brief This structure defines the message list object.
 */
typedef struct {
    uint8_t handle_length;                     /**< The length of the message handle. */
    uint8_t subject_length;                    /**< The length of the subject. */
    uint8_t time_length;                       /**< The length of the date time. */
    uint8_t sender_name_length;                /**< The length of the sender name. */
    uint8_t sender_addressing_length;          /**< The length of the sender addressing. */
    bt_mapc_message_t message_type;            /**< The message type. */
    uint8_t *handle;                           /**< The content of the message handle. */
    uint8_t *subject;                          /**< The content of the subject of the message. */
    uint8_t *time;                             /**< The content of the date time of the message. */
    uint8_t *sender_name;                      /**< The content of the sender name of the message. */
    uint8_t *sender_addressing;                /**< The content of the sender addressing of the message. */
    uint32_t size;                             /**< The size of the message. */
} bt_mapc_message_list_object_t;

/**
 *  @brief This structure defines the event report object.
 */
typedef struct {
    bt_mapc_event_report_t type;               /**< The event report type.*/
    uint8_t handle_length;                     /**< The length of the message handle. */
    uint8_t folder_length;                     /**< The length of the folder. */
    bt_mapc_message_t message_type;            /**< Message type. */
    uint8_t time_length;                       /**< The length of the date time. */
    uint8_t subject_length;                    /**< The length of the subject. */
    uint8_t sender_name_length;                /**< The length of the sender name. */
    bt_mapc_message_priority_t priority;       /**< Message priority. */
    uint8_t conversation_name_length;          /**< The length of the conversation name. */
    uint8_t conversation_id_length;            /**< The length of the conversation id. */
    bt_mapc_message_status_t read_status;      /**< Read status. */
    uint8_t participant_uci_length;            /**< The length of the participant uci. */
    uint8_t contact_uid_length;                /**< The length of the contact uid. */
    uint8_t *handle;                           /**< The content of the message handle. */
    uint8_t *folder;                           /**< The content of the folder of the message. */
    uint8_t *time;                             /**< The content of the date time of the message. */
    uint8_t *subject;                          /**< The content of the subject of the message. */
    uint8_t *sender_name;                      /**< The content of the sender name of the message. */
    uint8_t *conversation_name;                /**< The content of the conversation name. */
    uint8_t *conversation_id;                  /**< The content of the conversation id. */
    uint8_t *participant_uci;                  /**< The content of the participant uci. */
    uint8_t *contact_uid;                      /**< The content of the contact uid. */
} bt_mapc_event_report_object_t;

/**
 *  @brief This structure defines the folder list object.
 */
typedef struct {
    bt_mapc_folder_listing_t type;            /**< The folder listing type.*/
    uint8_t name_length;                      /**< The length of the name. */
    uint8_t *name;                            /**< The content of the name. */
    uint32_t size;                            /**< The size of the file. */
} bt_mapc_folder_list_object_t;

/**
 *  @brief This structure defines the bmessage object.
 */
typedef struct {
    bt_mapc_message_status_t read_status;        /**< read status of the bmessage. */
    bt_mapc_message_t message_type;              /**< message type of the bmessage. */
    bt_mapc_contact_information_t *originator;   /**< sender of the bmessage. */
    bt_mapc_contact_information_t *recipient;    /**< recipient of the bmessage. */
    bt_mapc_charset_t charset;                   /**< character set of the bmessage content. */
    uint8_t encoding_length;                     /**< The length of the bmessage encoding. */
    uint8_t *encoding;                           /**< The encoding of the bmessage. */
    uint8_t message_language_length;             /**< The length of the bmessage language. */
    uint8_t *message_language;                   /**< The language of the bmessage. */
    uint16_t message_content_length;             /**< The length of the bmessage content. */
    uint8_t *message_content;                    /**< The content of the bmessage content. */
} bt_mapc_bmessage_object_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_SDP_SEARCH_CNF event.
 */
typedef struct {
    uint8_t mas_record_count;                /**< The count of the MAS record . */
} bt_mapc_sdp_search_cnf_t;

/**
 *  @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_SDP_SEARCHED_IND event.
 */
typedef struct {
    uint8_t service_name_length;             /**< The length of the service name. */
    uint8_t *service_name;                   /**< The content of the service name. */
    uint8_t mas_instance_id;                 /**< MASInstanceID. */
    uint8_t supported_message_types;         /**< Supported message types. */
} bt_mapc_sdp_searched_ind_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_CONNECT_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAS handle. */
} bt_mapc_connect_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MNS_CONNECT_IND event.
 */
typedef struct {
    uint32_t handle;                         /**< MNS handle. */
} bt_mapc_connect_ind_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_SET_NOTIFICATION_REGISTRATION_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
} bt_mapc_set_notification_registration_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_SET_FOLDER_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
} bt_mapc_set_folder_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_UPDATE_INBOX_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
} bt_mapc_update_inbox_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_ABORT_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
} bt_mapc_abort_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_DISCONNECT_IND
 or #BT_MAPC_MNS_DISCONNECT_IND event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
} bt_mapc_disconnect_ind_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_SET_MESSAGE_STATUS_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
} bt_mapc_set_message_status_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_SET_NOTIFICATION_FILTER_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
} bt_mapc_set_notification_filter_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_PUT_MESSAGE_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
    uint8_t file_handle_len;                 /**< The length of the folder handle. */
    uint8_t *file_handle;                    /**< folder handle of the Message object. */
} bt_mapc_put_message_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_GET_MESSAGE_CNF event.
 */
typedef struct {
    uint32_t handle;                         /**< MAPC handle. */
    bt_mapc_bmessage_object_t message;       /**< bmessage object. */
} bt_mapc_get_message_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_GET_FOLDER_LISTING_CNF event.
 */
typedef struct {
    uint32_t handle;                            /**< MAPC handle. */
    uint8_t list_count;                         /**< folder list count. */
    bt_mapc_folder_list_object_t *folder_list;  /**< folder list object. */
} bt_mapc_get_folder_listing_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MAS_GET_MESSAGES_LISTING_CNF event.
 */
typedef struct {
    uint32_t handle;                              /**< MAPC handle. */
    uint8_t list_count;                           /**< message list count. */
    bt_mapc_message_list_object_t *message_list;  /**< message list object. */
} bt_mapc_get_message_listing_cnf_t;

/**
 * @brief This structure defines the data type of the parameter in the callback for #BT_MAPC_MNS_SEND_EVENT_IND event.
 */
typedef struct {
    uint32_t handle;                              /**< MAPC handle. */
    uint8_t instance_id;                          /**< instance id. */
    bt_mapc_event_report_object_t event_report;   /**< event report object. */
} bt_mapc_send_event_ind_t;
/**
* @}
*/

BT_EXTERN_C_BEGIN

/**
 * @brief               This function search the SDP record of a MSE device.
 *                      #BT_MAPC_MAS_SDP_SEARCH_CNF event is sent to the upper layer with the result.
 * @param[in]  address  is the Bluetooth address of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_sdp_search_mas(const bt_bd_addr_t *address);

/**
 * @brief               This function clear the SDP record of a MSE device.
 * @param[in]  address  is the Bluetooth address of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_mapc_clear_sdp_database(const bt_bd_addr_t *address);

/**
 * @brief               This function enables or disables notification received by MNS.
 *                      #BT_MAPC_MAS_SET_NOTIFICATION_REGISTRATION_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  enable   defines whether to enable notification.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_set_notification_registration(uint32_t handle, bool enable);

/**
 * @brief               This function sets the folder for browsing or pushing a message.
 *                      #BT_MAPC_MAS_SET_FOLDER_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  back     defines whether to go back or to go down.
 * @param[in]  name     is the name of the child folder for going down. Note that the name cannot be a path.
 * @param[in]  name_len is the length of the folder name.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_set_folder(uint32_t handle,
                               bool back,
                               const uint8_t *name,
                               uint16_t name_len);

/**
 * @brief               This function gets the folder list from MSE.
 *                      #BT_MAPC_MAS_GET_FOLDER_LISTING_CNF event is sent to the upper layer with the result.
 * @param[in]  handle          is the identifier of the remote device.
 * @param[in]  max_list_count  is the maximum folder count of the returned list.
 * @param[in]  offset          is the start offset of the folder list.
 * @return               #BT_STATUS_SUCCESS, the operation completed successfully.
 *                           #BT_STATUS_FAIL, the operation has failed.
 *                           #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_get_folder_listing(uint32_t handle, uint16_t max_list_count, uint16_t offset);

/**
 * @brief               This function gets the message list of a folder.
 *                      #BT_MAPC_MAS_GET_MESSAGES_LISTING_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  name     is the name of the child folder or NULL for the current folder.
 * @param[in]  name_len is the length of the folder name.
 * @param[in]  max_list_count  is the maximum message count of the returned list.
 * @param[in]  offset   is the start offset of the message list.
 * @param[in]  filter_message_type is the filter mask for message types, use ORed mask for #bt_mapc_filter_out_message_t.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_get_message_listing(uint32_t handle,
                                        const uint8_t *name,
                                        uint16_t name_len,
                                        uint16_t max_list_count,
                                        uint16_t offset,
                                        uint8_t filter_message_type);
/**
 * @brief               This function gets the specified message.
 *                      #BT_MAPC_MAS_GET_MESSAGE_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  message_handle  is the handle of bmessage.
 * @param[in]  charset  is the charset of the bmessage content.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_get_message(uint32_t handle,
                                const uint8_t *message_handle,
                                bt_mapc_charset_t charset);

/**
 * @brief               This function initiate an update of the MSE's inbox.
 *                      #BT_MAPC_MAS_UPDATE_INBOX_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_update_inbox(uint32_t handle);

/**
 * @brief               This function sets the status of a specified message.
 *                      #BT_MAPC_MAS_SET_MESSAGE_STATUS_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  message_handle     is the 16-byte handle of a message.
 * @param[in]  status   is the read status of a message.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_set_message_status(uint32_t handle,
                                       const uint8_t *message_handle,
                                       bt_mapc_message_status_t status);

/**
 * @brief               This function uploads the message to the folder.
 *                      #BT_MAPC_MAS_PUT_MESSAGE_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  name     is the name of the child folder or NULL for the current folder.
 * @param[in]  name_len is the length of the folder name.
 * @param[in]  charset  is the charset of the bmessage content.
 * @param[in]  message  is the bmessage object.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_put_message(uint32_t handle,
                                const uint8_t *name,
                                uint16_t name_len,
                                bt_mapc_charset_t charset,
                                bt_mapc_bmessage_object_t *message);

/**
 * @brief               This function sets the notification filter to specify which events to receive from the MSE.
 *                      #BT_MAPC_MAS_SET_NOTIFICATION_FILTER_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @param[in]  event_mask  is the event mask that MAS wants to receive.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_set_notification_filter(uint32_t handle, uint32_t event_mask);

/**
 * @brief               This function connects to the specified remote device.
 *                      #BT_MAPC_MAS_CONNECT_CNF event is sent to the upper layer with the connection result.
 * @param[out]  handle   is the identifier of the remote device.
 * @param[in]  address  is the Bluetooth address of the remote device.
 * @param[in]  instance_id  is the instance id of the MAS.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_connect(uint32_t *handle, const bt_bd_addr_t *address, uint8_t instance_id);

/**
 * @brief               This function disconnects from the specified remote device.
 *                      #BT_MAPC_MAS_DISCONNECT_IND event is sent to the upper layer with the result of the disconnection.
 * @param[in]  handle   is the identifier of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_disconnect(uint32_t handle);

/**
 * @brief               This function aborts current operation.
 *                      #BT_MAPC_MAS_ABORT_CNF event is sent to the upper layer with the result.
 * @param[in]  handle   is the identifier of the remote device.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 *                          #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_mapc_abort(uint32_t handle);

BT_EXTERN_C_END
/**
 * @}
 * @}
 */
#endif
