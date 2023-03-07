/***************************************************************************//**
 * @file
 * @brief Object Transfer Service Object Client API
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_BT_OTS_CLIENT_H
#define SL_BT_OTS_CLIENT_H

#include <stdbool.h>
#include <stdint.h>
#include "sl_bt_ots_datatypes.h"
#include "sl_bt_ots_client_config.h"
#include "sl_slist.h"
#include "sl_bt_l2cap_transfer.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Object Client type definitions

/// OTS Object Client handle
typedef struct sl_bt_ots_client *sl_bt_ots_client_handle_t;

/***************************************************************************//**
 * OTS Object Client callback function prototype for connection or disconnection
 * @param[in] client Client handle.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_connection_callback_t)(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * OTS Object Client callback function prototype for indication subscription
 * status
 * @param[in] client Client handle.
 * @param[in] status Subscription status.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_subscription_callback_t)(sl_bt_ots_client_handle_t       client,
                                                         sl_bt_ots_subscription_status_t status);

/***************************************************************************//**
 * OTS Object Client callback function prototype to handle Object Change event
 *
 * @param[in] client      Client handle.
 * @param[in] flags       Change flags.
 *                        The following masks can be used to indicate the nature
 *                        of change:
 *                        - SL_BT_OTS_OBJECT_CHANGE_CONTENTS_MASK : object contents changed
 *                        - SL_BT_OTS_OBJECT_CHANGE_METADATA_MASK : metadata changed
 *                        - SL_BT_OTS_OBJECT_CHANGE_CREATION_MASK : object creation
 *                        - SL_BT_OTS_OBJECT_CHANGE_DELETION_MASK : object deletion
 * @param[in] object      Object ID.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_object_changed_callback_t)(sl_bt_ots_client_handle_t        client,
                                                           sl_bt_ots_object_changed_flags_t flags,
                                                           sl_bt_ots_object_id_t            *object);

/***************************************************************************//**
 * OTS Object Client callback function prototype for OTS Feature read
 * status
 * @param[in] client   Client handle.
 * @param[in] status   Result of the read.
 * @param[in] features Features supported by the Server.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_features_callback_t)(sl_bt_ots_client_handle_t client,
                                                     sl_status_t               status,
                                                     sl_bt_ots_features_t      features);

/***************************************************************************//**
 * OTS Object Client callback function prototype List Filter Write response
 *
 * @param[in] client   Client handle.
 * @param[in] status   GATT result of the write operation.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_list_filter_write_callback_t)(sl_bt_ots_client_handle_t client,
                                                              uint16_t                  status);

/***************************************************************************//**
 * OTS Object Client callback function prototype List Filter Write response
 *
 * @param[in] client   Client handle.
 * @param[in] status   Result of the write operation.
 * @param[in] filter   Filter data.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_list_filter_read_callback_t)(sl_bt_ots_client_handle_t              client,
                                                             sl_status_t                            status,
                                                             sl_bt_ots_object_list_filter_content_t filter);

/***************************************************************************//**
 * OTS Object Client callback function prototype to handle OLCP responses
 *
 * @param[in] client             Client handle.
 * @param[in] object             Object ID.
 * @param[in] opcode             OLCP Opcode.
 * @param[in] status             GATT result of the operation.
 * @param[in] parameters         OLCP response parameters.
 * @param[in] number_of_objects  Number of objects
 ******************************************************************************/
typedef void (*sl_bt_ots_client_olcp_callback_t)(sl_bt_ots_client_handle_t      client,
                                                 sl_bt_ots_object_id_t          *object,
                                                 sl_bt_ots_olcp_opcode_t        opcode,
                                                 uint16_t                       status,
                                                 sl_bt_ots_olcp_response_code_t response,
                                                 uint32_t                       number_of_objects);

/***************************************************************************//**
 * OTS Object Client callback function prototype to handle OACP responses
 *
 * @param[in] client      Client handle.
 * @param[in] object      Object ID.
 * @param[in] opcode      OACP Opcode.
 * @param[in] status      GATT result of the operation.
 * @param[in] event       OACP response.
 * @param[in] event       OACP response parameter.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_oacp_callback_t)(sl_bt_ots_client_handle_t      client,
                                                 sl_bt_ots_object_id_t          *object,
                                                 sl_bt_ots_oacp_opcode_t        opcode,
                                                 uint16_t                       status,
                                                 sl_bt_ots_oacp_response_code_t response,
                                                 sl_bt_ots_oacp_response_data_t *data);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle metadata writes
 * @param[in] client     Client handle.
 * @param[in] object     Object ID.
 * @param[in] event      Metadata type.
 * @param[in] status     GATT status of the write request. 0 means success.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_object_metadata_write_callback_t)(sl_bt_ots_client_handle_t                    client,
                                                                  sl_bt_ots_object_id_t                        *object,
                                                                  sl_bt_ots_object_metadata_write_event_type_t event,
                                                                  uint16_t                                     status);

/***************************************************************************//**
 * OTS Object Client callback function prototype to handle metadata reads
 * @param[in]  client     Client handle.
 * @param[in]  object     Object ID.
 * @param[in]  status     GATT status. 0 means success.
 * @param[in]  event      Metadata read event.
 * @param[in]  parameters Metadata event parameters. NULL, if status differs
 *                        from 0 (success).
 ******************************************************************************/
typedef void (*sl_bt_ots_client_object_metadata_read_callback_t)(sl_bt_ots_client_handle_t                   client,
                                                                 sl_bt_ots_object_id_t                       *object,
                                                                 uint16_t                                    status,
                                                                 sl_bt_ots_object_metadata_read_event_type_t event,
                                                                 sl_bt_ots_object_metadata_read_parameters_t *parameters);

/***************************************************************************//**
 * OTS Object Client callback function prototype to handle data reception
 * @param[in] client         Client handle.
 * @param[in] object         Object ID.
 * @param[in] current_offset Current offset from the beginning of the object.
 * @param[in] data           Pointer to the received data.
 * @param[in] size           Size of the received data.
 * @return                   Credits to give to the transmitter.
 ******************************************************************************/
typedef sl_bt_ots_l2cap_credit_t (*sl_bt_ots_client_data_receive_callback_t)(sl_bt_ots_client_handle_t client,
                                                                             sl_bt_ots_object_id_t     *object,
                                                                             uint32_t                  current_offset,
                                                                             uint8_t                   *data,
                                                                             uint32_t                  size);
/***************************************************************************//**
 * OTS Object Client callback function prototype to handle data transmission
 * @param[in] client     Client handle.
 * @param[in] object     Object ID reference.
 * @param[in]  offset    Current offset of requested data
 * @param[in]  size      Maximum size of requested data
 * @param[out] data      Pointer of pointer to data
 * @param[out] data_size Size of the provided data
 ******************************************************************************/
typedef void (*sl_bt_ots_client_data_transmit_callback_t)(sl_bt_ots_client_handle_t client,
                                                          sl_bt_ots_object_id_t     *object,
                                                          uint32_t                  current_offset,
                                                          uint32_t                  size,
                                                          uint8_t                   **data,
                                                          uint32_t                  *data_size);
/***************************************************************************//**
 * OTS Object Client callback function prototype to handle transmission status
 * @param[in] client Client handle.
 * @param[in] object Object ID.
 * @param[in] result Status of the finish.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_data_transfer_finished_t)(sl_bt_ots_client_handle_t   client,
                                                          sl_bt_ots_object_id_t       *object,
                                                          sl_bt_ots_transfer_result_t result);

/***************************************************************************//**
 * OTS Object Client callback function prototype to handle metadata read.
 * @param[in] client           Client handle.
 * @param[in] object           Object reference that passed to read metadata.
 * @param[in] remaining_fields Bitfield of remaining fields. The value is 0 if
 *                             all the fields was read successfully.
 * @param[in] result           Status of the read operation.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_group_metadata_read_t)(sl_bt_ots_client_handle_t   client,
                                                       sl_bt_ots_object_t          *object,
                                                       sl_bt_ots_metadata_fields_t remaining_fields,
                                                       sl_status_t                 result);

/***************************************************************************//**
 * OTS Object Client callback function prototype to handle initialization
 * status.
 * @param[in] client           Client handle.
 * @param[in] result           Status of the initialization.
 ******************************************************************************/
typedef void (*sl_bt_ots_client_init_callback_t)(sl_bt_ots_client_handle_t client,
                                                 sl_status_t               result);

/// OTS Object Client Callbacks
typedef struct {
  sl_bt_ots_client_init_callback_t                      on_init;                    ///< Callback to handle initialization.
  sl_bt_ots_client_connection_callback_t                on_connect;                 ///< Callback to handle connection to the server.
  sl_bt_ots_client_connection_callback_t                on_disconnect;              ///< Callback to handle disconnection from the server.
  sl_bt_ots_client_subscription_callback_t              on_subscription_change;     ///< Callback to handle subscription status changes.
  sl_bt_ots_client_features_callback_t                  on_features_read;           ///< Callback to handle server feature read.
  sl_bt_ots_client_object_metadata_read_callback_t      on_metadata_read_finished;  ///< Callback to handle metadata read results.
  sl_bt_ots_client_object_metadata_write_callback_t     on_metadata_write_finished; ///< Callback to handle metadata write results.
  sl_bt_ots_client_olcp_callback_t                      on_olcp_response;           ///< Callback to handle OLCP responses.
  sl_bt_ots_client_oacp_callback_t                      on_oacp_response;           ///< Callback to handle OACP responses.
  sl_bt_ots_client_data_transmit_callback_t             on_data_transmit;           ///< Callback to handle outgoing data.
  sl_bt_ots_client_data_receive_callback_t              on_data_receive;            ///< Callback to handle incoming data.
  sl_bt_ots_client_data_transfer_finished_t             on_data_transfer_finished;  ///< Callback to handle the end of a data transfer.
  sl_bt_ots_client_object_changed_callback_t            on_object_change;           ///< Callback to handle object change events.
  sl_bt_ots_client_list_filter_read_callback_t          on_filter_read;             ///< Callback to handle filter reads.
  sl_bt_ots_client_list_filter_write_callback_t         on_filter_write;            ///< Callback to handle filter writes.
  sl_bt_ots_client_group_metadata_read_t                on_group_metadata_read;     ///< Callback to handle grouped metadata read.
} sl_bt_ots_client_callbacks_t;

/// OTS Object Client status
SL_ENUM(sl_bt_ots_client_status_t) {
  CLIENT_STATUS_BEGIN,
  CLIENT_STATUS_DISCOVERY,
  CLIENT_STATUS_SUBSCRIBE_OLCP,
  CLIENT_STATUS_SUBSCRIBE_OACP,
  CLIENT_STATUS_SUBSCRIBE_OBJECT_CHANGED,
  CLIENT_STATUS_READ_FEATURES,
  CLIENT_STATUS_INITIALIZED,
  CLIENT_STATUS_WAIT_READ,
  CLIENT_STATUS_WAIT_WRITE,
  CLIENT_STATUS_WAIT_OACP,
  CLIENT_STATUS_WAIT_OACP_INDICATION,
  CLIENT_STATUS_WAIT_OACP_TRANSFER,
  CLIENT_STATUS_WAIT_OLCP,
  CLIENT_STATUS_WAIT_OLCP_INDICATION,
  CLIENT_STATUS_ERROR,
  CLIENT_STATUS_DISCONNECTED
};

/// OTS Object Client instance type
typedef struct sl_bt_ots_client {
  sl_slist_node_t                 node;                   ///< Client List node.
  sl_bt_ots_client_status_t       status;                 ///< Client status
  uint8_t                         connection;             ///< Connection
  sl_bt_ots_gattdb_handles_t      gattdb_handles;         ///< GATT database handles for the Object Client
  sl_bt_ots_client_callbacks_t    *callbacks;             ///< Callbacks for the higher layers
  sl_bt_ots_object_id_t           current_object;         ///< Selected Current Object ID
  bool                            single_object;          ///< Single object feature
  sl_bt_ots_subscription_status_t subscription;           ///< Subscription status
  uint8_t                         active_handle_index;    ///< Selected handle index
  uint8_t                         active_opcode;          ///< Selected olcp/oacp opcode
  sl_bt_ots_object_t              *read_object;           ///< Pointer to the target of the read operation
  sl_bt_l2cap_transfer_transfer_t l2cap_transfer;         ///< Current object transfer on L2CAP channel
  uint32_t                        active_transfer_size;   ///< Size of the current L2CAP transfer
  uint32_t                        active_transfer_offset; ///< Offset of the current L2CAP transfer
  uint16_t                        active_transfer_sdu;    ///< SDU of the current L2CAP transfer
  uint16_t                        active_transfer_pdu;    ///< PDU of the current L2CAP transfer
  uint8_t                         received_buffer[SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_DATA_SIZE];
} sl_bt_ots_client_t;

/***************************************************************************//**
 * Initialize Object Client.
 * @param[in] client     Client handle.
 * @param[in] connection Client connection handle.
 * @param[in] service    GATT database handle for the OTS service.
 * @param[in] callbacks  Client callbacks' structure.
 * @return               Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_init(sl_bt_ots_client_handle_t    client,
                                  uint8_t                      connection,
                                  uint32_t                     service,
                                  sl_bt_ots_client_callbacks_t *callbacks);

/***************************************************************************//**
 * Read Object Transfer Service Features supported by the Server.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_ots_features(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read Object Name of the Current Object.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_name(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read Object Type of the Current Object.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_type(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read Object Size of the Current Object.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_size(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read Object First Created time of the Current Object.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_first_created(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read Object Last Modifed time of the Current Object.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_last_modified(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read Object ID of the Current Object.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_id(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read Object Properties of the Current Object.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_properties(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Write Object Name of the Current Object.
 *
 * Write response is given in callback.
 * @param[in] client    Client handle.
 * @param[in] name      String containing the new name of the object.
 * @param[in] size      The size of the name argument.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_write_name(sl_bt_ots_client_handle_t client,
                                        char                      *name,
                                        uint8_t                   size);

/***************************************************************************//**
 * Write Object First Created field of the Current Object.
 *
 * Write response is given in callback.
 * @param[in] client    Client handle.
 * @param[in] time      Pointer to time structure.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_write_object_first_created(sl_bt_ots_client_handle_t client,
                                                        sl_bt_ots_time_t          *time);

/***************************************************************************//**
 * Write Object Last Modifed field of the Current Object.
 *
 * Write response is given in callback.
 * @param[in] client    Client handle.
 * @param[in] time      Pointer to time structure.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_write_object_last_modified(sl_bt_ots_client_handle_t client,
                                                        sl_bt_ots_time_t          *time);

/***************************************************************************//**
 * Write Object Properties field of the Current Object.
 *
 * Write response is given in callback.
 * @param[in] client     Client handle.
 * @param[in] properties Properties bitfield.
 *                       Possible values:
 *                       - SL_BT_OTS_OBJECT_PROPERTY_DELETE_MASK : Object is deletable
 *                       - SL_BT_OTS_OBJECT_PROPERTY_EXECUTE_MASK : Object is executable
 *                       - SL_BT_OTS_OBJECT_PROPERTY_READ_MASK : Object is readable
 *                       - SL_BT_OTS_OBJECT_PROPERTY_WRITE_MASK : Object is writable
 *                       - SL_BT_OTS_OBJECT_PROPERTY_APPEND_MASK : Object is appendable
 *                       - SL_BT_OTS_OBJECT_PROPERTY_TRUNCATE_MASK : Object can be truncated
 *                       - SL_BT_OTS_OBJECT_PROPERTY_PATCH_MASK : Object can be patched
 *                       - SL_BT_OTS_OBJECT_PROPERTY_MARK_MASK : Object can be marked
 * @return               Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_write_object_properties(sl_bt_ots_client_handle_t     client,
                                                     sl_bt_ots_object_properties_t properties);

/***************************************************************************//**
 * Select the first object as Current Object.
 *
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_first(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Select the last object as Current Object.
 *
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_last(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Select the previous object as Current Object.
 *
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_previous(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Select the next object as Current Object.
 *
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_next(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Select the specified object as Current Object.
 *
 * @param[in] client    Client handle.
 * @param[in] object    Object ID.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_go_to(sl_bt_ots_client_handle_t client,
                                        sl_bt_ots_object_id_t     *object);

/***************************************************************************//**
 * Select the List Sort Order.
 *
 * @param[in] client    Client handle.
 * @param[in] order     List sort order.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_order(sl_bt_ots_client_handle_t   client,
                                        sl_bt_ots_list_sort_order_t order);

/***************************************************************************//**
 * Request total number of objects.
 *
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_request_number_of_objects(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Clear marking of objects.
 *
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_olcp_clear_marking(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Read object list filter.
 *
 * Read response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_read_object_list_filter(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Write/set object list filter.
 *
 * Write response is given in callback.
 * @param[in] client    Client handle.
 * @param[in] filter    Filter type and arguments to set.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_write_object_list_filter(sl_bt_ots_client_handle_t              client,
                                                      sl_bt_ots_object_list_filter_content_t filter);

/***************************************************************************//**
 * Create new object.
 *
 * Write response is given in callback.
 * @param[in] client    Client handle.
 * @param[in] size      Object size.
 * @param[in] type      Object type.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_oacp_create_object(sl_bt_ots_client_handle_t client,
                                                uint32_t                  size,
                                                sl_bt_ots_object_type_t   type);

/***************************************************************************//**
 * Delete current object.
 *
 * Write response is given in callback.
 * @param[in] client    Client handle.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_oacp_delete_object(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Calculate checksum for Current Object.
 *
 * @param[in] client    Client handle.
 * @param[in] offset    Offset in bytes to calculate checksum from.
 * @param[in] length    Length of calculation in bytes.
 * @return              Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_oacp_calculate_checksum(sl_bt_ots_client_handle_t client,
                                                     uint32_t                  offset,
                                                     uint32_t                  length);

/***************************************************************************//**
 * Execute Current Object.
 *
 * @param[in] client             Client handle.
 * @param[in] optional_data      Optional data content for execution.
 * @param[in] optional_data_size Length of the optional data in bytes.
 * @return                       Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_oacp_execute(sl_bt_ots_client_handle_t client,
                                          uint8_t                   *optional_data,
                                          uint8_t                    optional_data_size);

/***************************************************************************//**
 * Read Current Object.
 *
 * @param[in] client  Client handle.
 * @param[in] offset  Offset to read the object from in bytes.
 * @param[in] length  Length of data in bytes to be read.
 * @param[in] max_sdu @parblock
 *   The Maximum Service Data Unit size the local channel endpoint can accept
 *
 *   Range: 23 to 65533.
 *   @endparblock
 * @param[in] max_pdu @parblock
 *   The maximum PDU payload size the local channel endpoint can accept
 *
 *   Range:23 to 252.
 *   @endparblock
 * @return            Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_oacp_read(sl_bt_ots_client_handle_t client,
                                       uint32_t                  offset,
                                       uint32_t                  length,
                                       uint16_t                  max_sdu,
                                       uint16_t                  max_pdu);

/***************************************************************************//**
 * Write Current Object.
 *
 * @param[in] client  Client handle.
 * @param[in] offset  Offset to write the object from in bytes.
 * @param[in] length  Length of data in bytes to be written.
 * @param[in] mode    Write mode.
 * @param[in] max_sdu @parblock
 *   The Maximum Service Data Unit size the local channel endpoint can accept
 *
 *   Range: 23 to 65533.
 *   @endparblock
 * @param[in] max_pdu @parblock
 *   The maximum PDU payload size the local channel endpoint can accept
 *
 *   Range:23 to 252.
 *   @endparblock
 * @return            Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_oacp_write(sl_bt_ots_client_handle_t   client,
                                        uint32_t                    offset,
                                        uint32_t                    length,
                                        sl_bt_ots_oacp_write_mode_t mode,
                                        uint16_t                    max_sdu,
                                        uint16_t                    max_pdu);

/***************************************************************************//**
 * Abort current read.
 *
 * @param[in] client Client handle.
 * @return           Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_oacp_abort(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Increase the credit for the L2CAP transfer that is in progress
 * @param[in] client     Client handle.
 * @param[in] credit     Number of credit to give (in packets).
 * @return               Response code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_increase_credit(sl_bt_ots_client_handle_t client,
                                             uint16_t                  credit);

/***************************************************************************//**
 * Abort current write or read operation. Close the L2CAP channel in case of
 * Write operation is in progress or execute OACP Abort in case of Read
 * operation is in progress.
 *
 * @param[in] client Client handle.
 * @return           Status code
 ******************************************************************************/
sl_status_t sl_bt_ots_client_abort(sl_bt_ots_client_handle_t client);

/***************************************************************************//**
 * Internal Bluetooth event handler.
 * @param[in] evt  Bluetooth event.
 ******************************************************************************/
void sli_bt_ots_client_on_bt_event(sl_bt_msg_t *evt);

/***************************************************************************//**
 * OTS Client - internal init.
 ******************************************************************************/
void sli_bt_ots_client_init(void);

/***************************************************************************//**
 * OTS Client - process internal action.
 ******************************************************************************/
void sli_bt_ots_client_step(void);

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT

/***************************************************************************//**
 * Internal routine for power manager handler
 * @return SL_POWER_MANAGER_WAKEUP if the test has been started
 ******************************************************************************/
sl_power_manager_on_isr_exit_t sli_bt_ots_client_sleep_on_isr_exit(void);

/***************************************************************************//**
 * Internal check if it is ok to sleep now
 * @return false if the test has been started
 ******************************************************************************/
bool sli_bt_ots_client_is_ok_to_sleep(void);

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

#ifdef __cplusplus
};
#endif

#endif // SL_BT_OTS_CLIENT_H
