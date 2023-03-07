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
/* MediaTek restricted information */

#ifndef __BT_GATT_H__
#define __BT_GATT_H__

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothGATT GATT
 * @{
 * @addtogroup BluetoothBLE_GATTC GATT Client
 * @{
 * This section introduces the GATT Client driver APIs including terms and acronyms, supported features, details on how to use this
 * driver, GATT Client function groups, enums, GATT Client request and response structures and functions, API prototypes to discover
 * primary services, included services, characteristics and descriptors and to read or write characteristics or descriptors.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b GATTC                      | GATT Client is the device that initiates commands and requests to the server. |
 * |\b MTU                        | Maximum Transmission Unit, specifies the maximum size of a packet to send. |
 * |\b ATT | Attribute Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Low_Energy_Attribute_Protocol_.28ATT.29">Wikipedia</a>.|
 *
 * @section bt_gattc_api_usage How to use this module
 * Each GATTC operation starts with sending a request to the server, then processing the corresponding response from the server
 * in the callback function #bt_app_event_callback().
 * - Step 1: GATT Client sends a read by group type request to the GATT Server, as shown in the figure of Primary service discovery event sequence in the Bluetooth developer's guide under [sdk_root]/doc folder.
 *  - Sample code:
 *     @code
 *               BT_GATTC_NEW_DISCOVER_PRIMARY_SERVICE_REQ(req);
 *               bt_gattc_discover_primary_service(conn_handle, &req);
 *     @endcode
 *
 * - Step 2: GATT Client processes the response from the GATT Server, as shown in the figure of Primary service discovery event sequence in the Bluetooth developer's guide under [sdk_root]/doc folder.
 *  - Sample code:
 *     @code
 *              bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *              {
 *                  switch (msg) {
 *                      case BT_GATTC_DISCOVER_PRIMARY_SERVICE:
 *                      {
 *                          printf("[GATT]BT_GATTC_DISCOVER_PRIMARY_SERVICE status = %d\n", status);
 *                          if (status != BT_STATUS_SUCCESS && status != BT_ATT_ERRCODE_CONTINUE) {
 *                              // Any other status means there is no more primary service to discover.
 *                              printf("BT_GATTC_DISCOVER_PRIMARY_SERVICE FINISHED!!\n");
 *                              return BT_STATUS_SUCCESS;
 *                          }
 *                          // Exact starting handle, ending handle and UUID according to read by group type response and service declaration.
 *                          bt_gattc_read_by_group_type_rsp_t *rsp = (bt_gattc_read_by_group_type_rsp_t *)buff;
 *                          uint16_t end_group_handle = 0, starting_handle = 0, uuid = 0;
 *                          bt_uuid_t uuid128;
 *                          uint8_t *attribute_data_list = rsp->att_rsp->attribute_data_list;
 *                          uint8_t num_of_data = (rsp->length - 2) / rsp->att_rsp->length;
 *                          int i;
 *
 *                          for (i = 0; i < num_of_data; i++){
 *                              memcpy(&starting_handle, attribute_data_list + i * rsp->att_rsp->length, 2);
 *                              memcpy(&end_group_handle, attribute_data_list + i * rsp->att_rsp->length + 2, 2);
 *                              printf("[GATT]data : %d\n", i);
 *                              printf("[GATT]starting_handle = 0x%04x, end_group_handle = 0x%04x\n", starting_handle, end_group_handle);
 *                              if (rsp->att_rsp->length == 6) {
 *                                  memcpy(&uuid, attribute_data_list + i * rsp->att_rsp->length + 4, rsp->att_rsp->length - 4);
 *                                  printf("[GATT]uuid = 0x%04x\n", uuid);
 *                              } else {
 *                                  memcpy(&uuid128.uuid, attribute_data_list + i * rsp->att_rsp->length + 4, rsp->att_rsp->length - 4);
 *                              }
 *                          }
 *                          if (status == BT_STATUS_SUCCESS) {
 *                              printf("BT_GATTC_DISCOVER_PRIMARY_SERVICE FINISHED!!\n");
 *                              return BT_STATUS_SUCCESS;
 *                          }
 *                      }
 *                      break;
 *                      default:
 *                      break;
 *                  }
 *                  return BT_STATUS_SUCCESS;
 *              }
 *     @endcode
 *
 *
 */

#include <stdint.h>
#include "bt_att.h"
#include "bt_gatt.h"
#include "bt_system.h"
#include "bt_platform.h"
#include "bt_type.h"
#include "bt_uuid.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup BluetoothBLE_GATTC_define Define
 * @{
 * This section defines macros for the GATT Client.
 */

#define BT_GATTC_EXCHANGE_MTU                                 (BT_MODULE_GATT | 0x00010000)      /**< This is the confirmation event, generated from #bt_gattc_exchange_mtu(), with #bt_gatt_exchange_mtu_rsp_t as the payload. */
#define BT_GATTC_DISCOVER_PRIMARY_SERVICE                     (BT_MODULE_GATT | 0x00020000)      /**< This is the confirmation event, generated from #bt_gattc_discover_primary_service(), with #bt_gattc_read_by_group_type_rsp_t as the payload. */
#define BT_GATTC_DISCOVER_PRIMARY_SERVICE_BY_UUID             (BT_MODULE_GATT | 0x00030000)      /**< This is the confirmation event, generated from #bt_gattc_discover_primary_service_by_uuid(), with #bt_gattc_find_by_type_value_rsp_t as the payload. */
#define BT_GATTC_FIND_INCLUDED_SERVICES                       (BT_MODULE_GATT | 0x00040000)      /**< This is the confirmation event, generated from #bt_gattc_find_included_services(), with #bt_gattc_read_by_type_rsp_t as the payload. */
#define BT_GATTC_DISCOVER_CHARC                               (BT_MODULE_GATT | 0x00050000)      /**< This is the confirmation event, generated from #bt_gattc_discover_charc(), with #bt_gattc_read_by_type_rsp_t as the payload. */
#define BT_GATTC_DISCOVER_CHARC_DESCRIPTOR                    (BT_MODULE_GATT | 0x00060000)      /**< This is the confirmation event, generated from #bt_gattc_discover_charc_descriptor(), with #bt_gattc_find_info_rsp_t as the payload. */
#define BT_GATTC_READ_CHARC                                   (BT_MODULE_GATT | 0x00070000)      /**< This is the confirmation event, generated from #bt_gattc_read_charc(), with #bt_gattc_read_rsp_t as the payload. */
#define BT_GATTC_READ_LONG_CHARC                              (BT_MODULE_GATT | 0x00080000)      /**< This is the confirmation event, generated from #bt_gattc_read_long_charc(), with #bt_gattc_read_rsp_t as the payload. */
#define BT_GATTC_READ_USING_CHARC_UUID                        (BT_MODULE_GATT | 0x00090000)      /**< This is the confirmation event, generated from #bt_gattc_read_using_charc_uuid(), with #bt_gattc_read_by_type_rsp_t as the payload. */
#define BT_GATTC_READ_MULTI_CHARC_VALUES                      (BT_MODULE_GATT | 0x000A0000)      /**< This is the confirmation event, generated from #bt_gattc_read_multi_charc_values(), with #bt_gattc_read_multiple_rsp_t as the payload. */
#define BT_GATTC_WRITE_CHARC                                  (BT_MODULE_GATT | 0x000D0000)      /**< This is the confirmation event, generated from #bt_gattc_write_charc(), with #bt_gattc_write_rsp_t as the payload. */
#define BT_GATTC_WRITE_LONG_CHARC                             (BT_MODULE_GATT | 0x000E0000)      /**< This is the confirmation event, generated from #bt_gattc_prepare_write_charc() that writes long characteristic value or writes long characteristic descriptors, with #bt_gattc_execute_write_rsp_t as the payload. */
#define BT_GATTC_RELIABLE_WRITE_CHARC                         (BT_MODULE_GATT | 0x000F0000)      /**< This is the confirmation event, generated from #bt_gattc_prepare_write_charc() as the reliable writes, with #bt_gattc_execute_write_rsp_t as the payload. */
#define BT_GATTC_CHARC_VALUE_NOTIFICATION                     (BT_MODULE_GATT | 0x00110000)      /**< This is the notification event with #bt_gatt_handle_value_notification_t as the payload. */
#define BT_GATTC_CHARC_VALUE_INDICATION                       (BT_MODULE_GATT | 0x00120000)      /**< This is the indication event with #bt_gatt_handle_value_indication_t as the payload. */
#define BT_GATTC_CHARC_VALUE_CONFIRMATION                     (BT_MODULE_GATT | 0x00130000)      /**< This is the confirmation event, generated from #bt_gatts_send_charc_value_notification_indication(), with (uint32_t* connect_handle) as the payload. */
#define BT_GATTC_READ_MULTI_VARIABLE_CHARC_VALUES             (BT_MODULE_GATT | 0x00140000)      /**< This is the confirmation event, generated from #bt_gattc_read_multi_variable_charc_values(), with #bt_gattc_read_multiple_variable_rsp_t as the payload. */
#define BT_GATTC_MULTI_VARIABLE_CHARC_VALUE_NOTIFICATION      (BT_MODULE_GATT | 0x00150000)      /**< This is the notification event with #bt_gatt_multiple_variable_length_notification_t as the payload. */
/**
 * @}
 */

/**
 * @defgroup BluetoothBLE_GATTC_struct Struct
 * @{
 * Basic data structures for the GATT Client.
 */

/**
 *  @brief Exchange MTU request.
 */
typedef bt_att_exchange_mtu_req_t bt_gattc_exchange_mtu_req_t;

/**
 *  @brief Discover Primary Service request.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< Can only be of type #BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST. */
    uint16_t starting_handle;   /**< Starting handle. */
    uint16_t ending_handle;     /**< Ending handle. */
    uint16_t type16;            /**< Attribute type. */
}) bt_gattc_discover_primary_service_req_t;

/**
 *  @brief Discover Primary Service By UUID request.
 */
BT_PACKED(
typedef struct {
    uint16_t attribute_value_length;             /**< The length of the attribute value. */
    bt_att_find_by_type_value_req_t *att_req;   /**< A pointer to the structure to find by type value request. */
}) bt_gattc_discover_primary_service_by_uuid_req_t;

/**
 *  @brief Find Included Service request.
 */
typedef bt_gattc_discover_primary_service_req_t bt_gattc_find_included_services_req_t;

/**
 *  @brief Discover All Characteristics of a Service or Discover Characteristics by UUID request.
 */
BT_PACKED(
typedef struct {
    uint8_t opcode;             /**< #BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST. */
    uint16_t starting_handle;   /**< Starting handle. */
    uint16_t ending_handle;     /**< Ending handle. */
    bt_uuid_t type;             /**< Attribute type. */
}) bt_gattc_discover_charc_req_t;

/**
 *  @brief Discover All Characteristic Descriptors request.
 */
typedef bt_att_find_info_req_t bt_gattc_discover_charc_descriptor_req_t;

/**
 *  @brief Read Characteristic Value or Read Characteristic Descriptors request.
 */
typedef bt_att_read_req_t bt_gattc_read_charc_req_t;

/**
 *  @brief Read Long Characteristic Values or Read Long Characteristic Descriptors request.
 */
typedef bt_att_read_blob_req_t bt_gattc_read_long_charc_value_req_t;

/**
 *  @brief Read Using Characteristic UUID request.
 */
typedef bt_att_read_by_type_req_t bt_gattc_read_using_charc_uuid_req_t;

/**
 *  @brief Read Multiple Characteristic Values request.
 */
BT_PACKED(
typedef struct {
    uint16_t handle_length;                  /**< The length of value handle. */
    bt_att_read_multiple_req_t *att_req;    /**< A pointer to the structure to request to read multiple attribute values. */
}) bt_gattc_read_multi_charc_values_req_t;

/**
 *  @brief Read Multiple Variable Characteristic Values request.
 */
BT_PACKED(
typedef struct {
    uint16_t handle_length;                          /**< The length of value handle. */
    bt_att_read_multiple_variable_req_t *att_req;    /**< A pointer to the structure to request to read multiple variable attribute values. */
}) bt_gattc_read_multi_variable_charc_values_req_t;

/**
 *  @brief Write Without Response or Signed Write Without Response request.
 */
BT_PACKED(
typedef struct {
    uint16_t attribute_value_length;         /**< The length of attribute value. */
    bt_att_write_command_t *att_req;        /**< A pointer to the structure to request for an attribute write command. */
}) bt_gattc_write_without_rsp_req_t;

/**
 *  @brief Write Characteristic Value or Write Characteristic Descriptors request.
 */
BT_PACKED(
typedef struct {
    uint16_t attribute_value_length;         /**< The length of attribute value. */
    bt_att_write_req_t *att_req;            /**< A pointer to the structure to request attribute write. */
}) bt_gattc_write_charc_req_t;

/**
 *  @brief Write Long Characteristic Value or Write Long Characteristic Descriptors request.
 */
BT_PACKED(
typedef struct {
    uint16_t attribute_value_length;        /**< The length of attribute value. */
    bt_att_prepare_write_req_t *att_req;    /**< A pointer to the structure to prepare an attribute write request. */
}) bt_gattc_prepare_write_charc_req_t;

/**
 *  @brief Execute Write request.
 */
typedef bt_att_execute_write_req_t bt_gattc_execute_write_req_t;

/**
 *  @brief Notification or Indication.
 */
BT_PACKED(
typedef struct {
    uint16_t attribute_value_length;                /**< The length of attribute value. */
    bt_att_handle_value_notification_t att_req;     /**< A pointer to the structure to handle value notification. */
}) bt_gattc_charc_value_notification_indication_t;

/**
 *  @brief Multiple Variable Length Notifications.
 */
BT_PACKED(
typedef struct {
    uint16_t attribute_value_length;                         /**< The length of attribute value. */
    bt_att_multiple_handle_value_notification_t att_req;     /**< A pointer to the structure to multiple handle value notification. */
}) bt_gattc_multiple_variable_length_notification_t;

/**
 *  @brief Error response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                         /**< Total response length. */
    bt_handle_t connection_handle;          /**< Connection handle. */
    bt_att_error_rsp_t *att_rsp;            /**< A pointer to the structure for the error response. */
}) bt_gattc_error_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_EXCHANGE_MTU) which indicates exchange MTU response.
 */
BT_PACKED(
typedef struct {
    bt_handle_t connection_handle;          /**< Connection handle. */
    uint16_t server_rx_mtu;                 /**< Attribute server receiver MTU size. */
}) bt_gatt_exchange_mtu_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_DISCOVER_CHARC_DESCRIPTOR) which indicates find information response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                        /**< The length of total response. */
    bt_handle_t connection_handle;          /**< Connection handle. */
    bt_att_find_info_rsp_t *att_rsp;        /**< A pointer to the structure for the find information response. */
}) bt_gattc_find_info_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_DISCOVER_PRIMARY_SERVICE_BY_UUID) which indicates find by type value response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                                /**< The length of total response. */
    bt_handle_t connection_handle;                  /**< Connection handle. */
    bt_att_find_by_type_value_rsp_t *att_rsp;       /**< A pointer to the structure for the find by type value response. */
}) bt_gattc_find_by_type_value_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_FIND_INCLUDED_SERVICES or #BT_GATTC_DISCOVER_CHARC or #BT_GATTC_READ_USING_CHARC_UUID) which indicates read by type value response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                        /**< The length of total response. */
    bt_handle_t connection_handle;          /**< Connection handle. */
    bt_att_read_by_type_rsp_t *att_rsp;     /**< A pointer to the structure for the read by type response. */
}) bt_gattc_read_by_type_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_READ_CHARC or # BT_GATTC_READ_LONG_CHARC) which indicates read response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                    /**< The length of total response. */
    bt_handle_t connection_handle;      /**< Connection handle. */
    bt_att_read_rsp_t *att_rsp;         /**< A pointer to the structure for the read response. */
}) bt_gattc_read_rsp_t;

/**
 *  @brief Read blob response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                    /**< The length of total response. */
    bt_handle_t connection_handle;      /**< Connection handle. */
    bt_att_read_blob_rsp_t *att_rsp;    /**< A pointer to the structure for the read blob response. */
}) bt_gattc_read_blob_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_READ_MULTI_CHARC_VALUES) which indicates read multiple response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                        /**< The length of total response. */
    bt_handle_t connection_handle;          /**< Connection handle. */
    bt_att_read_multiple_rsp_t *att_rsp;    /**< A pointer to the structure for the read multiple response. */
}) bt_gattc_read_multiple_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_READ_MULTI_VARIABLE_CHARC_VALUES) which indicates read multiple variable response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                        /**< The length of total response. */
    bt_handle_t connection_handle;          /**< Connection handle. */
    bt_att_read_multiple_variable_rsp_t *att_rsp;    /**< A pointer to the structure for the read multiple response. */
}) bt_gattc_read_multiple_variable_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_DISCOVER_PRIMARY_SERVICE) which indicates read by group type response.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                            /**< The length of total response. */
    bt_handle_t connection_handle;              /**< Connection handle. */
    bt_att_read_by_group_type_rsp_t *att_rsp;   /**< A pointer to the structure for the read by group type response. */
}) bt_gattc_read_by_group_type_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_WRITE_CHARC) which indicates write response.
 */
BT_PACKED(
typedef struct {
    bt_handle_t connection_handle;              /**< Connection handle. */
    bt_att_write_rsp_t *att_rsp;                /**< A pointer to the structure for the write response. */
}) bt_gattc_write_rsp_t;

/**
 *  @brief Prepare write response.
 */
BT_PACKED(
typedef struct {
    uint16_t attribute_value_length;        /**< The length of total response. */
    bt_handle_t connection_handle;          /**< Connection handle. */
    bt_att_prepare_write_rsp_t *att_rsp;    /**< A pointer to the structure for the prepare write response. */
}) bt_gattc_prepare_write_rsp_t;


/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_WRITE_LONG_CHARC or #BT_GATTC_RELIABLE_WRITE_CHARC) which indicates execute write response.
 */
BT_PACKED(
typedef struct {
    bt_handle_t connection_handle;              /**< Connection handle. */
    bt_att_execute_write_rsp_t *att_rsp;        /**< A pointer to the structure for the execute write response. */
}) bt_gattc_execute_write_rsp_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_CHARC_VALUE_NOTIFICATION) which indicates handle value notification.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                                /**< The length of total response. */
    bt_handle_t connection_handle;                  /**< Connection handle. */
    bt_att_handle_value_notification_t *att_rsp;    /**< A pointer to the structure for the handle value notification. */
}) bt_gatt_handle_value_notification_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_CHARC_VALUE_INDICATION) which indicates handle value indication.
 */
typedef bt_gatt_handle_value_notification_t bt_gatt_handle_value_indication_t;

/**
 *  @brief Handle value confirmation.
 */
typedef bt_att_handle_value_confirmation_t bt_gattc_handle_value_confirmation_t;

/**
 *  @brief This structure defines the data type of the callback parameter for the event (#BT_GATTC_MULTI_VARIABLE_CHARC_VALUE_NOTIFICATION) which indicates multiple handle value notification.
 */
BT_PACKED(
typedef struct {
    uint16_t length;                                         /**< The length of total response. */
    bt_handle_t connection_handle;                           /**< Connection handle. */
    bt_att_multiple_handle_value_notification_t *att_rsp;    /**< A pointer to the structure for the multiple handle value notification. */
}) bt_gatt_multiple_variable_length_notification_t;

/**
 * @}
 */

/**
* @defgroup BluetoothBLE_GATTC_define Define
* @{
* This section defines macros for the GATT Client.
*/

/**
* @brief This macro creates an exchange MTU request.
* @param[in] name      is the name of the request.
* @param[in] mtu       is the client RX MTU to exchange.
*/
#define BT_GATTC_NEW_EXCHANGE_MTU_REQ(name, mtu)        \
    static bt_gattc_exchange_mtu_req_t name;            \
    name.opcode = BT_ATT_OPCODE_EXCHANGE_MTU_REQUEST;   \
    name.client_rx_mtu = mtu;                           \

/**
* @brief This macro creates a discover primary service request.
* @param[in] name      is the name of the request.
*/
#define BT_GATTC_NEW_DISCOVER_PRIMARY_SERVICE_REQ(name)             \
    static const bt_gattc_discover_primary_service_req_t name = {   \
    .opcode = BT_ATT_OPCODE_READ_BY_GROUP_TYPE_REQUEST,             \
    .starting_handle = 0x0001,                                      \
    .ending_handle = 0xFFFF,                                        \
    .type16 = BT_GATT_UUID16_PRIMARY_SERVICE                        \
    }

/**
* @brief This macro creates a discover primary service by UUID16 request.
* @param[in] name      is the name of the request.
* @param[in] uuid16    is the 16-bit UUID to discover.
*/
#define BT_GATTC_NEW_DISCOVER_PRIMARY_SERVICE_BY_UUID16_REQ(name, uuid16)       \
    static bt_gattc_discover_primary_service_by_uuid_req_t name;                \
    static uint8_t name##buf[9];                                                \
    name.attribute_value_length = 2;                                            \
    name.att_req = (bt_att_find_by_type_value_req_t *)(name##buf);              \
    name.att_req->opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;            \
    name.att_req->starting_handle = 0x0001;                                     \
    name.att_req->ending_handle = 0xFFFF;                                       \
    name.att_req->uuid = BT_GATT_UUID16_PRIMARY_SERVICE;                        \
    memcpy(&(name.att_req->attribute_value), &uuid16, 2);

/**
* @brief This macro creates a discover primary service by UUID128 request.
* @param[in] name      is the name of the request.
* @param[in] uuid128   is the 128-bit UUID to discover.
*/
#define BT_GATTC_NEW_DISCOVER_PRIMARY_SERVICE_BY_UUID128_REQ(name, uuid128)       \
    static bt_gattc_discover_primary_service_by_uuid_req_t name;                \
    static uint8_t name##buf[30];                                                \
    name.attribute_value_length = 16;                                            \
    name.att_req = (bt_att_find_by_type_value_req_t *)(name##buf);              \
    name.att_req->opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;            \
    name.att_req->starting_handle = 0x0001;                                     \
    name.att_req->ending_handle = 0xFFFF;                                       \
    name.att_req->uuid = BT_GATT_UUID16_PRIMARY_SERVICE;                        \
    memcpy(&(name.att_req->attribute_value), &uuid128, 16);

/**
* @brief This macro defines a find included service request.
* @param[in] name           is the name of the request.
* @param[in] start_handle   is the starting handle to search.
* @param[in] end_handle     is the ending handle to search.
*/
#define BT_GATTC_NEW_FIND_INCLUDED_SERVICE_REQ(name, start_handle, end_handle)       \
    static bt_gattc_find_included_services_req_t name;          \
    name.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;           \
    name.type16 = BT_GATT_UUID16_INCLUDE;                       \
    name.starting_handle = start_handle;                        \
    name.ending_handle = end_handle;

/**
* @brief This macro creates a discover characteristic request.
* @param[in] name           is the name of the request.
* @param[in] start_handle   is the starting handle to search.
* @param[in] end_handle     is the ending handle to search.
*/
#define BT_GATTC_NEW_DISCOVER_CHARC_REQ(name, start_handle, end_handle)       \
    static bt_gattc_discover_charc_req_t name;              \
    name.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;       \
    name.starting_handle = start_handle;                    \
    name.ending_handle = end_handle;                        \
    uint16_t name##uuid_16 = BT_GATT_UUID16_CHARC;          \
    bt_uuid_load(&(name.type), (void *)&(name##uuid_16), 2);

/**
* @brief This macro creates a discover characteristic by UUID request.
* @param[in] name           is the name of the request.
* @param[in] start_handle   is the starting handle to search.
* @param[in] end_handle     is the ending handle to search.
* @param[in] uuid           is the buffer to hold the UUID to discover.
* @param[in] len            is the buffer length to hold the UUID.
*/
#define BT_GATTC_NEW_DISCOVER_CHARC_BY_UUID_REQ(name, start_handle, end_handle, uuid, len)       \
    static bt_gattc_discover_charc_req_t name;              \
    name.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;       \
    name.starting_handle = start_handle;                    \
    name.ending_handle = end_handle;                        \
    bt_uuid_load(&(name.type), (void *)uuid, len);

/**
* @brief This macro creates a discover characteristic descriptor request.
* @param[in] name           is the name of the request.
* @param[in] start_handle   is the starting handle to search.
* @param[in] end_handle     is the ending handle to search.
*/
#define BT_GATTC_NEW_DISCOVER_CHARC_DESCRIPTOR_REQ(name, start_handle, end_handle)  \
    static bt_gattc_discover_charc_descriptor_req_t name;    \
    name.opcode = BT_ATT_OPCODE_FIND_INFORMATION_REQUEST;    \
    name.starting_handle = start_handle;                     \
    name.ending_handle = end_handle;

/**
* @brief This macro creates a read characteristic request.
* @param[in] name           is the name of the request.
* @param[in] attr_handle    is the attribute handle to read.
*/
#define BT_GATTC_NEW_READ_CHARC_REQ(name, attr_handle)  \
    static bt_gattc_read_charc_req_t name;              \
    name.opcode = BT_ATT_OPCODE_READ_REQUEST;           \
    name.attribute_handle = attr_handle;

/**
* @brief This macro creates a read long characteristic request.
* @param[in] name           is the name of the request.
* @param[in] attr_handle    is the attribute handle to read.
* @param[in] offset         is the offset to start reading.
*/
#define BT_GATTC_NEW_READ_LONG_CHARC_REQ(name, attr_handle, offset)  \
    static bt_gattc_read_long_charc_value_req_t name;              \
    name.opcode = BT_ATT_OPCODE_READ_BLOB_REQUEST;          \
    name.attribute_handle = attr_handle;                    \
    name.value_offset = offset;

/**
* @brief This macro creates a read using characteristic UUID request.
* @param[in] name           is the name of the request.
* @param[in] start_handle   is the starting handle to search.
* @param[in] end_handle     is the ending handle to search.
* @param[in] uuid           is the buffer to hold the UUID to read.
* @param[in] len            is the buffer length to hold the UUID.
*/
#define BT_GATTC_NEW_READ_USING_CHARC_UUID_REQ(name, start_handle, end_handle, uuid, len)  \
    static bt_gattc_read_using_charc_uuid_req_t name;       \
    name.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;       \
    name.starting_handle = start_handle;                    \
    name.ending_handle = end_handle;                        \
    bt_uuid_load(&name.type, (void *)uuid, len);

/**
* @brief This macro creates a write command request.
* @param[in] name               is the name of the request.
* @param[in] buffer             is the buffer to hold the content of the #bt_att_write_command_t.
* @param[in] type               is the type of the write command, 0-WRITE_COMMAND, 1-SIGNED_WRITE_COMMAND.
* @param[in] attribute_handle   is the handle of the attribute to write.
* @param[in] attribute_value    is the value to write.
* @param[in] attribute_len      is the length of the value to write.
*/
#define BT_GATTC_NEW_WRITE_WITHOUT_RSP_REQ(name, buffer, type, attribute_handle, attribute_value, attribute_len)  \
    static bt_gattc_write_without_rsp_req_t name;               \
    name.attribute_value_length = attribute_len;                \
    name.att_req = (bt_att_write_command_t *)(buffer);          \
    name.att_req->opcode = (type == 0 ? BT_ATT_OPCODE_WRITE_COMMAND : BT_ATT_OPCODE_SIGNED_WRITE_COMMAND);  \
    name.att_req->attribute_handle = attribute_handle;          \
    memcpy(name.att_req->attribute_value, attribute_value, attribute_len);

/**
* @brief This macro creates a write request.
* @param[in] name               is the name of the request.
* @param[in] buffer             is the buffer to hold the content of the #bt_gattc_write_charc_req_t.
* @param[in] attr_handle        is the handle of the attribute to write.
* @param[in] attr_value         is the value to write.
* @param[in] attr_len           is the length of the value to write.
*/
#define BT_GATTC_NEW_WRITE_CHARC_REQ(name, buffer, attr_handle, attr_value, attr_len)  \
    static bt_gattc_write_charc_req_t name;                 \
    name.attribute_value_length = attr_len;            \
    name.att_req = (bt_att_write_req_t *)(buffer);          \
    name.att_req->opcode = BT_ATT_OPCODE_WRITE_REQUEST;     \
    name.att_req->attribute_handle = attr_handle;          \
    memcpy(name.att_req->attribute_value, attr_value, attr_len);

/**
* @brief This macro creates a prepare write request.
* @param[in] name               is the name of the request.
* @param[in] attr_handle        is the handle of the attribute to write.
* @param[in] offset             is the offset to begin writing.
* @param[in] value              is the value to write.
* @param[in] value_len          is the length of the value to write.
*/
#define BT_GATTC_NEW_PREPARE_WRITE_REQ(name, attr_handle, offset, value, value_len) \
    static bt_gattc_prepare_write_charc_req_t name;                 \
    static bt_att_prepare_write_req_t name##att_req;                \
    (name##att_req).opcode = BT_ATT_OPCODE_PREPARE_WRITE_REQUEST;   \
    (name##att_req).attribute_handle = attr_handle;            \
    (name##att_req).value_offset = offset;                          \
    (name##att_req).part_attribute_value = value;                   \
    name.attribute_value_length = value_len;                        \
    name.att_req = &(name##att_req);

/**
* @brief This macro creates an execute write request.
* @param[in] name           is the name of the request.
* @param[in] flag           is the flag to determine whether to write or cancel all the prepared values.
*/
#define BT_GATTC_NEW_EXECUTE_WRITE_REQ(name, flag)      \
    static bt_gattc_execute_write_req_t name;           \
    name.opcode = BT_ATT_OPCODE_EXECUTE_WRITE_REQUEST;  \
    name.flags = flag;

/**
* @}
*/

/**
 * @brief This function sends an exchange MTU request to the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the GATT exchange MTU request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_exchange_mtu(bt_handle_t connection_handle,
                                  const bt_gattc_exchange_mtu_req_t *req);

/**
 * @brief This function gets the MTU of this connection.
 * @param[in] connection_handle     The connection handle.
 * @return                          MTU of this connection if connection exists, otherwise return the default MTU.
 */
uint32_t bt_gattc_get_mtu(bt_handle_t connection_handle);

/**
 * @brief This function discovers the primary service of the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is the pointer to the GATT discover primary service request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_discover_primary_service(
    bt_handle_t connection_handle,
    const bt_gattc_discover_primary_service_req_t *req);

/**
 * @brief This function discovers the primary service of the GATT Server by the UUID.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the GATT discover primary service by UUID request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_discover_primary_service_by_uuid(
    bt_handle_t connection_handle,
    const bt_gattc_discover_primary_service_by_uuid_req_t *req);

/**
 * @brief This function finds the included service on the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the GATT find included service request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_find_included_services(
    bt_handle_t connection_handle,
    const bt_gattc_find_included_services_req_t *req);
/**
 * @brief This function discovers all characteristics or a specific characteristic by UUID of the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the discover characteristic request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_discover_charc(
    bt_handle_t connection_handle,
    const bt_gattc_discover_charc_req_t *req);

/**
 * @brief This function discovers characteristic descriptors.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the discover characteristic descriptors request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_discover_charc_descriptor(
    bt_handle_t connection_handle,
    const bt_gattc_discover_charc_descriptor_req_t *req);

/**
 * @brief This function reads the value or the descriptor of a characteristic of the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the read characteristic request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_read_charc(
    bt_handle_t connection_handle,
    const bt_gattc_read_charc_req_t *req);

/**
 * @brief This function reads a long characteristic value or a descriptor of the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the read long characteristic request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_read_long_charc(
    bt_handle_t connection_handle,
    const bt_gattc_read_long_charc_value_req_t *req);

/**
 * @brief This function reads the characteristics of the GATT Server by UUID.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the read using characteristic UUID request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_read_using_charc_uuid(
    bt_handle_t connection_handle,
    const bt_gattc_read_using_charc_uuid_req_t *req);

/**
 * @brief This function reads multiple characteristic values from the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the read multiple characteristic values request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_read_multi_charc_values(
    bt_handle_t connection_handle,
    const bt_gattc_read_multi_charc_values_req_t *req);

/**
 * @brief This function reads multiple variable characteristic values from the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the read multiple characteristic values request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_read_multi_variable_charc_values(
    bt_handle_t connection_handle,
    const bt_gattc_read_multi_variable_charc_values_req_t *req);

/**
 * @brief This function writes characteristic values without a response from the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] is_signed             is a flag to decide whether to use signed write. True , use signed write, False , use unsigned write.
 * @param[in] req                   is a pointer to the write without response request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_write_without_rsp(
    bt_handle_t connection_handle,
    uint8_t is_signed,
    const bt_gattc_write_without_rsp_req_t *req);


/**
 * @brief This function writes values or descriptors of a characteristic on the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the write characteristic request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_write_charc(
    bt_handle_t connection_handle,
    const bt_gattc_write_charc_req_t *req);

/**
 * @brief This function prepares to write characteristic values to the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] is_reliable           is a flag to decide whether it is a reliable prepare write request.
 * @param[in] offset                is the offset to write.
 * @param[in] req                   is a pointer to the prepare write request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_prepare_write_charc(
    bt_handle_t connection_handle,
    uint8_t is_reliable,
    uint16_t offset,
    const bt_gattc_prepare_write_charc_req_t *req);

/**
 * @brief This function sends an execute write request to the GATT Server.
 * @param[in] connection_handle     is the connection handle.
 * @param[in] req                   is a pointer to the execute write request structure.
 * @return                          #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                  #BT_STATUS_FAIL, the operation has failed.
 *                                  #BT_STATUS_OUT_OF_MEMORY, out of memory.
 *                                  #BT_STATUS_CONNECTION_IN_USE, an ongoing request.
 */
bt_status_t bt_gattc_send_execute_write_req(
    bt_handle_t connection_handle,
    const bt_gattc_execute_write_req_t *req);

BT_EXTERN_C_END

/**
 * @}
 * @}
 * @}
 */

#endif /*__BT_GATT_H__*/



