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

#ifndef __BT_SPP_H__
#define __BT_SPP_H__

#include "bt_type.h"
#include "bt_sdp.h"
/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup BluetoothSPP SPP
 * @{
 * The Serial Port Profile (SPP) can be used to emulate a serial port connection using Radio Frequency Communication(RFCOMM) protocol between two peer devices.
 * It supports server and client roles. The SPP server is implemented for a server side connection (Acceptor). It can wait
 * for another device to take initiative to connect. The SPP client is implemented for a client side connection (Initiator) to initiate a connection with another device.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b UUID                      | A Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 * |\b SPP                        | Serial Port Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Serial_Port_Profile_.28SPP.29">Wikipedia</a>. |
 * |\b RFCOMM                | The Radio Frequency Communication. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Radio_frequency_communication_.28RFCOMM.29">Wikipedia</a>. |
 * |\b SDP                       | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Service_discovery_protocol_.28SDP.29">Wikipedia</a>. |
 *
 * @section bt_spp_api_usage How to use this module
 * - SPP connection in server mode, as shown in the figure titled as "SPP connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Mandatory, implement #bt_sdps_get_customized_record() to add SDP records of SPP Servers. The SPP's SDP record contains five attributes: the service class ID list,
 *    the protocol description list, the public browse group, the language list and  the service name. In general, user needs to customize the service class ID list, the protocol description list
 *    and the service name, while the other two attributes are constant. In the service class ID list, user needs to fill the 128-bit UUID to identify the SPP server. User can assign their own unique
 *    and private UUID for the purpose of customization. In the protocol description list, user needs to fill the SPP server ID with the value between the #BT_SPP_SERVER_ID_START and
 *    the #BT_SPP_SERVER_ID_END. It is noted that each SPP record must be assigned with a unique server ID. In the service name, user can assign a name to the SPP server.
 *    For more information about the SDP record, please refer to \ref BluetoothSDP.
 * - Sample code:
 *    @code
 *      #define BT_SPP_UUID    0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB
 *      static const uint8_t bt_spp_service_class_id_pattern[] =
 *      {
 *          BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
 *          BT_SPP_SDP_ATTRIBUTE_UUID(BT_SPP_STANDARD_UUID)
 *      };
 *      static const uint8_t bt_spp_protocol_descriptor_list_pattern[] =
 *      {
 *          BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_SPP_SERVER_ID_START)
 *      };
 *      static const uint8_t bt_spp_browse_group_pattern[] =
 *      {
 *          BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
 *      };
 *      static const uint8_t bt_spp_language_pattern[] =
 *      {
 *          BT_SPP_SDP_ATTRIBUTE_LANGUAGE
 *      };
 *      static const uint8_t bt_spp_service_name_pattern[] =
 *      {
 *          BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(11),
 *          'S', 'e', 'r', 'i', 'a', 'l', ' ', 'P', 'o', 'r', 't'
 *      };
 *      static const bt_sdps_attribute_t bt_spp_sdp_attributes_pattern[] =
 *      {
 *          BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_spp_service_class_id_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(bt_spp_protocol_descriptor_list_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(bt_spp_browse_group_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(bt_spp_language_pattern),
 *          BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(bt_spp_service_name_pattern)
 *      };
 *      static const bt_sdps_record_t bt_spp_sdp_record_pattern =
 *      {
 *          .attribute_list_length = sizeof(bt_spp_sdp_attributes_pattern),
 *          .attribute_list = bt_spp_sdp_attributes_pattern,
 *      };
 *      static const bt_sdps_record_t *sdps_spp_record[]= {
 *          &bt_spp_sdp_record_pattern,
 *          //&bt_spp_sdp_record_custom, user can add multiple SPP servers here.
 *      };
 *      uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list)
 *      {
 *          *record_list = sdps_spp_record;
 *          return sizeof(sdps_spp_record)/sizeof(bt_sdps_record_t*);
 *      }
 *     @endcode
 *  - Step 2. If a remote SPP client initiates a connection to the SPP server, the #BT_SPP_CONNECT_IND event notifies the function #bt_app_event_callback().
 *  - Step 3. Call the function #bt_spp_connect_response() to accept or reject this connection request from a remote SPP client.
 *  - Step 4. The #BT_SPP_CONNECT_CNF event notifies the function #bt_app_event_callback() if the device is connected successfully or not.
 *   - Sample code:
 *     @code
 *       bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg) {
 *               case BT_SPP_CONNECT_IND:
 *                    bt_spp_connect_ind_t* conn_ind_p = (bt_spp_connect_ind_t*) buff;
 *                    bt_spp_connect_response(conn_ind_p->handle, true);
 *                    break;
 *               case BT_SPP_CONNECT_CNF:
 *                    bt_spp_connect_cnf_t* conn_cnf_p = (bt_spp_connect_cnf_t*) buff;
 *                    break;
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 * - SPP connection in client mode, as shown in the figure titled as "SPP connection establishment message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Call the function #bt_spp_connect() to initiate a connection to a remote server.
 *   - Sample code:
 *    @code
 *      static const uint8_t spp_uuid[16] = {0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB};
 *      bt_bd_addr_t remote_addr = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
 *      uint32_t spp_handle = BT_SPP_INVALID_HANDLE;
 *      bt_spp_connect(&(spp_handle), &remote_addr, spp_uuid);
 *    @endcode
 *  - Step 2. The #BT_SPP_CONNECT_CNF event notifies the function #bt_app_event_callback() if the device is connected successfully or not.
 * -  Send data, as shown in the figure titled as "SPP data transfer message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Call the function #bt_spp_send() to send application data to the remote device.
 *           If returned value is #BT_STATUS_SPP_TX_NOT_AVAILABLE, the application must wait for the #BT_SPP_READY_TO_SEND_IND event before the next send operation.
 *   - Sample code:
 *    @code
 *        uint8_t app_send_data_buffer[100] = {0};
 *        bt_status_t result = bt_spp_send(spp_handle, app_send_data_buffer, 100);
 *    @endcode
 *  - Step 2. If the application received #BT_SPP_READY_TO_SEND_IND event, it can continue to write the rest of application data.
 *   - Sample code:
 *    @code
 *         bt_status_t result = bt_spp_send(spp_handle, app_send_data_buffer, 100);
 *    @endcode
 * - Received data, as shown in the figure titled as "SPP data transfer message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. The #BT_SPP_DATA_RECEIVED_IND event notifies the function #bt_app_event_callback() if the device received data from the remote.
 *   - Sample code:
 *    @code
 *      uint8_t app_receive_data_buffer[900] = {0};
 *       bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg) {
 *               case BT_SPP_DATA_RECEIVED_IND:
 *                   bt_spp_data_received_ind_t* data_ind_p = (bt_spp_data_received_ind_t*) buff;
 *                   memcpy(app_receive_data_buffer, data_ind_p->packet, data_ind_p->packet_length);
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step 2. If there is no buffer for the application to save the SPP packet from the #BT_SPP_DATA_RECEIVED_IND, call the function #bt_spp_hold_data()
 *      to hold the packet in the Bluetooth RX buffer.
 *   - Sample code:
 *    @code
 *       bt_spp_data_received_ind_t saved_data = {0};
 *       bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg) {
 *               case BT_SPP_DATA_RECEIVED_IND:
 *                   bt_spp_data_received_ind_t* data_ind_p = (bt_spp_data_received_ind_t*) buff;
 *                   bt_spp_hold_data(data_ind_p->packet);
  *                  saved_data.packet = data_ind_p->packet;
 *                   saved_data.packet_length = data_ind_p->packet_length
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step 3. Release the SPP packet after it is stored in the application by calling the function #bt_spp_release_data().
 *      If not released, the Bluetooth RX buffer may be exhausted and the application cannot receive data from a remote device.
 *   - Sample code:
 *    @code
 *       uint8_t app_new_data_buffer[900] = {0};
 *       bt_spp_data_received_ind_t* data_p = &saved_data;
 *       memcpy(app_new_data_buffer, data_p->packet, data_p->packet_length);
 *       bt_spp_release_data(saved_data.packet);
 *    @endcode
 * -  Disconnect the SPP connection, as shown in the figure titled as "SPP connection release message sequence" in the Bluetooth developer's guide under the [sdk_root]/doc folder.
 *  - Step 1. Call the function #bt_spp_disconnect() to disconnect the SPP connection from a remote device.
 *   - Sample code:
 *    @code
 *       bt_status_t result = bt_spp_disconnect(spp_handle);
 *    @endcode
 *  - Step 2. The #BT_SPP_DISCONNECT_IND event will notify the function #bt_app_event_callback() to indicate the SPP connection has been released.
 *   - Sample code:
 *    @code
 *       bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
 *       {
 *           switch (msg) {
 *               case BT_SPP_DISCONNECT_IND:
 *                  bt_spp_disconnect_ind_t* disc_ind_p = (bt_spp_disconnect_ind_t*)buff;
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 */

/**
 * @defgroup Bluetoothbt_spp_define Define
 * @{
 * This section defines the macros for the SPP.
 */

/**
 * @brief An invalid value for the SPP handle.
 */
#define BT_SPP_INVALID_HANDLE 0x00000000

/**
 * @brief  Attribute value length of the SPP service class ID list.
 */
#define BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH  BT_SDP_ATTRIBUTE_HEADER_8BIT(17)

/**
 * @brief  Attribute value of the SPP service class ID list.
 * @param[in] uuid128            is a 128-bit UUID of the SPP server.
 */
#define BT_SPP_SDP_ATTRIBUTE_UUID BT_SDP_UUID_128BIT

/**
 * @brief Attribute value of the SPP protocol descriptor list.
 * @param[in] server_id  is a uint8 RFCOMM channel number between 0x06 and 0x1E.
 */
#define BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(server_id)    \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(12),     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),    \
    BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_L2CAP),     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(5),           \
    BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_RFCOMM),     \
    BT_SDP_UINT_8BIT(server_id)

/**
 * @brief Attribute value of the SPP Public Browse Group. Any SDP attribute structure can include this
 *        attribute to add itself to the root level of the Public Browse Group.
 */
#define BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),   \
    BT_SDP_UUID_16BIT(BT_SDP_SERVICE_CLASS_PUBLIC_BROWSE_GROUP)

/**
 * @brief Attribute value of the SPP language base.
 */
#define BT_SPP_SDP_ATTRIBUTE_LANGUAGE     \
    BT_SDP_ATTRIBUTE_HEADER_8BIT(9),     \
    BT_SDP_UINT_16BIT(0x656E),          \
    BT_SDP_UINT_16BIT(0x006A),          \
    BT_SDP_UINT_16BIT(0x0100)

/**
 * @brief The SPP service name's size.
 */
#define BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(size)     \
    BT_SDP_TEXT_8BIT(size)

/**
 * @brief The SPP service class ID list attribute.
 * @param[in] uuid_array  is an array to hold the #BT_SPP_SDP_ATTRIBUTE_UUID.
 */
#define BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(uuid_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST, uuid_array)

/**
 * @brief The SPP protocol descriptor list attribute.
 * @param[in] protocol_descriptor_array   is an array to hold the #BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR.
 */
#define BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(protocol_descriptor_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST, protocol_descriptor_array)

/**
 * @brief The SPP Public Browse Group attribute.
 * @param[in] browse_group_array  is an array to hold the #BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP.
 */
#define BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(browse_group_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_BROWSE_GROUP_LIST, browse_group_array)

/**
 * @brief The SPP language base ID list attribute.
 * @param[in] language_array  is an array to hold the #BT_SPP_SDP_ATTRIBUTE_LANGUAGE.
 */
#define BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(language_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_LANG_BASE_ID_LIST, language_array)

/**
 * @brief The SPP service name attribute.
 * @param[in] name_array  is an array including the size of name(#BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME)
 *      and its text or string name.
 */
#define BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(name_array)  \
    BT_SDP_ATTRIBUTE(BT_SDP_ATTRIBUTE_ID_SERVICE_NAME + 0x0100, name_array)

/**
 * @brief The SPP event definitions.
 */
#define BT_SPP_CONNECT_IND          (BT_MODULE_SPP | 0x0000)  /**< A connect attempt is initiated from a remote device. Call the function #bt_spp_connect_response() 
                                                                                                             to accept or reject the attempt. The structure for this event is defined as #bt_spp_connect_ind_t.*/
#define BT_SPP_CONNECT_CNF          (BT_MODULE_SPP | 0x0001)  /**< The result of a connect attempt initiated by a local or remote device is available. 
                                                                                                               The structure for this event is defined as #bt_spp_connect_cnf_t.*/
#define BT_SPP_DISCONNECT_IND       (BT_MODULE_SPP | 0x0002)  /**< A disconnect attempt is initiated by a local or remote device or a link loss occurred. 
                                                                                                                The structure for this event is defined as #bt_spp_disconnect_ind_t.*/
#define BT_SPP_DATA_RECEIVED_IND    (BT_MODULE_SPP | 0x0003)  /**< Data was received from the remote device. The "packet" and "pakcet_length" fields describe
                                                                                                                 the received data. The structure for this event is defined as #bt_spp_data_received_ind_t.*/
#define BT_SPP_READY_TO_SEND_IND    (BT_MODULE_SPP | 0x0004)  /**< The Bluetooth TX buffer has free space for sending data. Call the function #bt_spp_send() to
                                                                                                                     send the application data.  The structure for this event is defined as #bt_spp_ready_to_send_ind_t.*/

/**
 * @brief Define for SPP failed status.
 */
#define BT_STATUS_SPP_TX_NOT_AVAILABLE  (BT_MODULE_SPP |0xE0) /**< The error code after calling the function #bt_spp_send() to send the application data.*/
#define BT_STATUS_SPP_CONNECTION_EXISTS (BT_MODULE_SPP |0xE1) /**< The error code indicates that the spp connection already exists.*/

/**
 * @brief This enumeration defines the SPP server ID as an input parameter assigned to #BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR.
 * If multiple SPP SDP records exist,  there must be a different server ID in each record.
 */
#define BT_SPP_SERVER_ID_START  0x06 /**< Beginning of the SPP server ID.*/
#define BT_SPP_SERVER_ID_END    0x1E    /**< End of the SPP server ID.*/

/**
 * @brief Define for SPP RHO flags
 */
#define BT_SPP_RHO_FLAG_UPDATE_CONTEXT  0x01 /**<SPP context updated, application should update its own SPP handle.*/

/**
 * @}
 */

/**
 * @defgroup Bluetoothbt_spp_struct Struct
 * @{
 * This section defines the structures for the SPP.
 */
/**
 *  @brief This structure defines #BT_SPP_CONNECT_CNF.
 */
typedef struct {
    uint32_t handle;            /**< The SPP handle of the current connection.*/
    uint16_t max_packet_length;   /**< The maximum length of a TX/RX packet after a SPP connection is established. */
    uint8_t server_id; /**< The SPP server id of the current connection. */
} bt_spp_connect_cnf_t;

/** @brief This structure defines #BT_SPP_CONNECT_IND. */
typedef struct {
    uint32_t handle;      /**< The SPP handle of the current connection.*/
    uint8_t  local_server_id;  /**< The SPP server id of a SDP record which the remote device connect to*/
    bt_bd_addr_t *address; /**< The Bluetooth address of a remote device. */
} bt_spp_connect_ind_t;

/** @brief This structure defines #BT_SPP_DISCONNECT_IND. */
typedef struct {
    uint32_t handle;     /**< The SPP handle of the current connection.*/
} bt_spp_disconnect_ind_t;

/** @brief This structure defines #BT_SPP_READY_TO_SEND_IND. */
typedef struct {
    uint32_t handle;     /**< The SPP handle of the current connection.*/
} bt_spp_ready_to_send_ind_t;

/** @brief This structure defines #BT_SPP_DATA_RECEIVED_IND. */
typedef struct {
    uint32_t handle;   /**< The SPP handle of the current connection.*/
    uint8_t *packet;   /**< The packet is received from a remote device.*/
    uint16_t packet_length;  /**< The length of the received packet.*/
} bt_spp_data_received_ind_t;
/**
 * @}
 */

BT_EXTERN_C_BEGIN
/**
 * @brief                       This function is for the SPP client to connect to a remote server, the #BT_SPP_CONNECT_CNF event is reported to
 *                              indicate the result of the connection. Note that this API can only be used by an SPP client.
 * @param[out] handle           is the SPP handle of the current connection.
 * @param[in] address           is the Bluetooth address of a remote device.
 * @param[in] uuid128           is a 128-bit UUID of remote server, such as {0x00,0x00,0x11,0x01,0x00,0x00,
 *                              0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB}.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 *                              #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_spp_connect(
    uint32_t *handle,
    const bt_bd_addr_t *address,
    const uint8_t *uuid128);

/**
 * @brief                 This function is for the SPP server to accept or reject connection from a remote client, the #BT_SPP_CONNECT_CNF event
 *                        is reported to indicate the result of the response. Note that this API can only be used by the SPP server.
 * @param[in] handle      is the SPP handle of the current connection.
 * @param[in] accept      defines whether the connection is accepted.
 * @return                #BT_STATUS_SUCCESS, the operation completed successfully.
 *                        #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_spp_connect_response(uint32_t handle, bool accept);

/**
 * @brief                    This function sends data to a remote device.
 * @param[in] handle         is the SPP handle of the current connection.
 * @param[in] packet         is a pointer to the packet to send to a remote device.
 * @param[in] packet_length  is the length of a packet to send.
 * @return                   #BT_STATUS_SUCCESS, the operation completed successfully.
 *                           #BT_STATUS_FAIL, the operation has failed.
 *                           #BT_STATUS_SPP_TX_NOT_AVAILABLE, if the TX buffer in the local device or the RX buffer in the remote
 *                           device has no space available. The application should wait for the #BT_SPP_READY_TO_SEND_IND event before
 *                           the next send operation.
 */
bt_status_t bt_spp_send(
    uint32_t handle,
    uint8_t *packet,
    uint16_t packet_length);

/**
 * @brief               This function disconnects an existing connection, the #BT_SPP_DISCONNECT_IND event is reported
 *                      to indicate the result of the disconnection.
 * @param[in] handle    is the SPP handle of the current connection.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_spp_disconnect(uint32_t handle);

/**
 * @brief           This function holds the SPP packet until it is fully consumed.
 * @param[in] data  is the received data. The value is provided in the #BT_SPP_DATA_RECEIVED_IND event.
 * @return          None.
 */
void bt_spp_hold_data(uint8_t *data);

/**
* @brief           This function releases the SPP packet after it is consumed.
* @param[in] data  is the received data which has been held.
* @return          None.
*/
void bt_spp_release_data(uint8_t *data);

/**
 * @brief           This function holds the SPP packet until it is fully consumed, and the RFCOMM credit will be kept.
 * @param[in] handle    is the SPP handle of the current connection.
 * @param[in] data       is the received data. The value is provided in the #BT_SPP_DATA_RECEIVED_IND event.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                          #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_spp_hold_data_ext(uint32_t handle, uint8_t *data);

/**
* @brief           This function releases the SPP packet after it is consumed, and the RFCOMM credit will be consumed.
* @param[in] handle  is the SPP handle of the current connection.
* @param[in] data     is the received data which has been held.
* @return              #BT_STATUS_SUCCESS, the operation completed successfully.
*                          #BT_STATUS_FAIL, the operation has failed.
*/
bt_status_t bt_spp_release_data_ext(uint32_t handle, uint8_t *data);

/**
 * @brief           This function can be used to get SPP connection handle by local server id.
 * @param[in] address           is the Bluetooth address of a remote device.
 * @param[in] local_server_id   is the local server id of SPP connection.
 * @return          a SPP connection handle found by the local server id provided.
 */
uint32_t bt_spp_get_handle_by_local_server_id(const bt_bd_addr_t *address, uint8_t local_server_id);

/**
 * @brief               This function can be used to get the remote device's Bluetooth address of current connection.
 * @param[in] handle    is the handle of current connection.
 * @return              The Bluetooth address.
 *
 */
const bt_bd_addr_t *bt_spp_get_bd_addr_by_handle(uint32_t handle);

BT_EXTERN_C_END

/**
 * @}
 * @}
 *
*/

#endif /*__BT_SPP_H__*/

