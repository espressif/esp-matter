/***************************************************************************//**
 * @file
 * @brief Object Transfer Service Object Client implementation
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
#include <string.h>
#include "sl_bt_ots_client.h"
#include "sl_bt_ots_datatypes.h"
#include "sli_bt_ots_datatypes.h"
#include "sl_status.h"
#include "app_queue.h"
#include "em_core.h"

// -----------------------------------------------------------------------------
// Definitions

#define OTS_CLIENT_BUFFER_SIZE        SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_DATA_SIZE

#define CHECK_STATE(c)                            \
  if ((c->status) != CLIENT_STATUS_INITIALIZED) { \
    return SL_STATUS_INVALID_STATE;               \
  }

#define SET_STATE_READ(c, x)             \
  if (sc == SL_STATUS_OK) {              \
    c->active_handle_index = x;          \
    c->status = CLIENT_STATUS_WAIT_READ; \
  }

#define SET_STATE_WRITE(c, x)             \
  if (sc == SL_STATUS_OK) {               \
    c->active_handle_index = x;           \
    c->status = CLIENT_STATUS_WAIT_WRITE; \
  }

#define SET_STATE_OACP(c)                                              \
  if (sc == SL_STATUS_OK) {                                            \
    c->active_handle_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OACP; \
    c->status = CLIENT_STATUS_WAIT_OACP;                               \
  }

#define SET_STATE_OACP_INDICATION(c)                                   \
  do {                                                                 \
    c->active_handle_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OACP; \
    c->status = CLIENT_STATUS_WAIT_OACP_INDICATION;                    \
  } while (0);

#define SET_STATE_OLCP(c)                                              \
  if (sc == SL_STATUS_OK) {                                            \
    c->active_handle_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OLCP; \
    c->status = CLIENT_STATUS_WAIT_OLCP;                               \
  }

#define SET_STATE_OLCP_INDICATION(c)                                   \
  do {                                                                 \
    c->active_handle_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OLCP; \
    c->status = CLIENT_STATUS_WAIT_OLCP_INDICATION;                    \
  } while (0);

#define SET_STATE(c, x, s)      \
  if (sc == SL_STATUS_OK) {     \
    c->active_handle_index = x; \
    c->status = s;              \
  }

#define CLEAR_STATE(c)                                                    \
  do {                                                                    \
    c->active_handle_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID; \
    c->status = CLIENT_STATUS_INITIALIZED;                                \
  } while (0);

#define CHECK_CHARACTERISTIC(x)                                                            \
  if (client->gattdb_handles.characteristics.handles.x == INVALID_CHARACTERISTIC_HANDLE) { \
    return false;                                                                          \
  }

#define CALL_SAFE(handle, cb, ...)      \
  if (handle->callbacks->cb != NULL) {  \
    handle->callbacks->cb(__VA_ARGS__); \
  }

#define INDEX_TO_HANDLE(i) (i + 1)
#define HANDLE_TO_INDEX(h) (h - 1)

#define L2CAP_SPSM  0x81

// -----------------------------------------------------------------------------
// Type Definitions

typedef struct {
  sl_bt_ots_client_t *client;
  uint16_t           attribute_handle;
} read_queue_item_t;

typedef struct {
  sl_bt_ots_client_t *client;
  uint16_t           attribute_handle;
  uint16_t           size;
  uint8_t            data[SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_DATA_SIZE];
} write_queue_item_t;

// -----------------------------------------------------------------------------
// Forward declaration of private functions

// L2CAP transfer callback for data transmit
static void l2cap_transfer_data_transmit(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                         uint32_t                               offset,
                                         uint32_t                               size,
                                         uint8_t                                **data,
                                         uint32_t                               *data_size);
// L2CAP transfer callback for data reception
static uint16_t l2cap_transfer_data_received(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                             uint32_t                               offset,
                                             uint8array                             *data);
// L2CAP transfer callback for transfer finish
static void l2cap_transfer_transfer_finished(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                             sl_status_t error_code);
// L2CAP transfer callback for channel open
static void l2cap_transfer_channel_opened(sl_bt_l2cap_transfer_transfer_handle_t transfer_object);
// Check characteristics discovery after discovery
static bool check_characteristics(sl_bt_ots_client_handle_t client);
// Subscribe to characteristics
static void subscribe(sl_bt_ots_client_t *client,
                      sl_bt_ots_client_status_t status);
static sl_bt_ots_client_status_t finish_init(sl_bt_ots_client_t *client,
                                             sl_status_t error);
static void handle_gatt_read_response(sl_bt_ots_client_t *client,
                                      uint16_t           characteristic,
                                      uint16_t           offset,
                                      uint8array         *data);
static void handle_gatt_write_response(sl_bt_ots_client_t *client,
                                       uint16_t result);
static sl_status_t send_write_request(sl_bt_ots_client_t                  *client,
                                      sl_bt_ots_characteristic_uuid_index handle_index,
                                      uint16_t                            size,
                                      uint8_t                             *data);
static sl_status_t send_read_request(sl_bt_ots_client_t                  *client,
                                     sl_bt_ots_characteristic_uuid_index handle_index);
static sl_status_t send_oacp_request(sl_bt_ots_client_t                  *client,
                                     uint16_t                            size,
                                     uint8_t                             *data);
static sl_status_t send_olcp_request(sl_bt_ots_client_t                  *client,
                                     uint16_t                            size,
                                     uint8_t                             *data);
static void clear_queue(uint8_t connection);

// -----------------------------------------------------------------------------
// Private variables

// Queues for read and write operations
static app_queue_t write_queue[SL_BT_CONFIG_MAX_CONNECTIONS];
static app_queue_t read_queue[SL_BT_CONFIG_MAX_CONNECTIONS];
static write_queue_item_t write_data[SL_BT_CONFIG_MAX_CONNECTIONS][SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_QUEUE_SIZE];
static read_queue_item_t  read_data[SL_BT_CONFIG_MAX_CONNECTIONS][SL_BT_OTS_CLIENT_CONFIG_READ_REQUEST_QUEUE_SIZE];

// Active clients for connections
static sl_bt_ots_client_t *active_client[SL_BT_CONFIG_MAX_CONNECTIONS] = { 0 };

// Callbacks for L2CAP Transfer
static sl_bt_l2cap_transfer_callbacks_t l2cap_transfer_callbacks = {
  .on_transmit = l2cap_transfer_data_transmit,
  .on_receive  = l2cap_transfer_data_received,
  .on_open     = l2cap_transfer_channel_opened,
  .on_finish   = l2cap_transfer_transfer_finished
};

// Start of the linked list which contains the queue
static sl_slist_node_t *client_list = NULL;

// -----------------------------------------------------------------------------
// Public functions

sl_status_t sl_bt_ots_client_init(sl_bt_ots_client_handle_t    client,
                                  uint8_t                      connection,
                                  uint32_t                     service,
                                  sl_bt_ots_client_callbacks_t *callbacks)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check arguments
  CHECK_NULL(client);
  CHECK_NULL(callbacks);
  if (service == INVALID_SERVICE_HANDLE) {
    return SL_STATUS_INVALID_PARAMETER;
  }
  if (connection == INVALID_CONNECTION_HANDLE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Clear GATT data
  memset(&client->gattdb_handles, 0, sizeof(client->gattdb_handles));

  // Assign requested service
  client->gattdb_handles.service = service;

  // Assign connection
  client->connection = connection;

  // Set callbacks
  client->callbacks = callbacks;

  // Set invalid handle
  memset(client->current_object.data, 0, sizeof(client->current_object.data));
  client->current_object.id.rfu = SL_BT_OTS_INVALID_OBJECT_RFU;

  client->read_object    = NULL;

  // Set up L2CAP transfer
  client->l2cap_transfer.callbacks = &l2cap_transfer_callbacks;
  client->l2cap_transfer.connection = client->connection;

  client->status = CLIENT_STATUS_BEGIN;

  // Active parameter clear
  client->active_handle_index    = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID;
  client->active_opcode          = 0;
  client->active_transfer_size   = 0;
  client->active_transfer_offset = 0;
  client->active_transfer_sdu    = 0;
  client->active_transfer_pdu    = 0;

  // Add client to the list
  sl_slist_push_back(&client_list, &client->node);

  return sc;
}

sl_status_t sl_bt_ots_client_read_ots_features(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OTS_FEATURE);
  return sc;
}

sl_status_t sl_bt_ots_client_read_object_name(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME);
  return sc;
}

sl_status_t sl_bt_ots_client_read_object_type(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_TYPE);

  return sc;
}

sl_status_t sl_bt_ots_client_read_object_size(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_SIZE);
  return sc;
}

sl_status_t sl_bt_ots_client_read_object_first_created(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Check if the characteristic is supported
  if (client->gattdb_handles.characteristics.handles.object_first_created == INVALID_CHARACTERISTIC_HANDLE) {
    return SL_STATUS_NOT_SUPPORTED;
  }
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED);
  return sc;
}

sl_status_t sl_bt_ots_client_read_object_last_modified(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Check if the characteristic is supported
  if (client->gattdb_handles.characteristics.handles.object_last_modified == INVALID_CHARACTERISTIC_HANDLE) {
    return SL_STATUS_NOT_SUPPORTED;
  }
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED);
  return sc;
}

sl_status_t sl_bt_ots_client_read_object_id(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Check if the characteristic is supported
  if (client->gattdb_handles.characteristics.handles.object_id == INVALID_CHARACTERISTIC_HANDLE) {
    return SL_STATUS_NOT_SUPPORTED;
  }
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_ID);
  return sc;
}

sl_status_t sl_bt_ots_client_read_object_properties(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Read characteristic
  sc = send_read_request(client,
                         SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES);
  return sc;
}

sl_status_t sl_bt_ots_client_write_name(sl_bt_ots_client_handle_t client,
                                        char                      *name,
                                        uint8_t                   size)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check arguments
  CHECK_NULL(client);
  CHECK_NULL(name);
  if (size == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Check status of the client
  CHECK_STATE(client);

  // Write characteristic
  sc = send_write_request(client,
                          SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME,
                          size,
                          (uint8_t *)name);
  return sc;
}

sl_status_t sl_bt_ots_client_write_object_first_created(sl_bt_ots_client_handle_t client,
                                                        sl_bt_ots_time_t          *time)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  CHECK_NULL(time);
  // Check status of the client
  CHECK_STATE(client);

  // Write characteristic
  sc = send_write_request(client,
                          SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED,
                          SL_BT_OTS_TIME_SIZE,
                          (uint8_t *)time->data);
  return sc;
}

sl_status_t sl_bt_ots_client_write_object_last_modified(sl_bt_ots_client_handle_t client,
                                                        sl_bt_ots_time_t          *time)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  CHECK_NULL(time);

  // Check status of the client
  CHECK_STATE(client);

  // Write characteristic
  sc = send_write_request(client,
                          SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED,
                          SL_BT_OTS_TIME_SIZE,
                          (uint8_t *)time->data);
  return sc;
}

sl_status_t sl_bt_ots_client_write_object_properties(sl_bt_ots_client_handle_t     client,
                                                     sl_bt_ots_object_properties_t properties)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Write characteristic
  sc = send_write_request(client,
                          SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES,
                          sizeof(properties),
                          (uint8_t *)&properties);
  return sc;
}

sl_status_t sl_bt_ots_client_olcp_first(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);

  sl_bt_ots_olcp_opcode_t opcode = SL_BT_OTS_OLCP_OPCODE_FIRST;
  sc = send_olcp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_olcp_last(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);

  sl_bt_ots_olcp_opcode_t opcode = SL_BT_OTS_OLCP_OPCODE_LAST;
  sc = send_olcp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_olcp_previous(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);

  sl_bt_ots_olcp_opcode_t opcode = SL_BT_OTS_OLCP_OPCODE_PREVIOUS;
  sc = send_olcp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_olcp_next(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);

  sl_bt_ots_olcp_opcode_t opcode = SL_BT_OTS_OLCP_OPCODE_NEXT;
  sc = send_olcp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_olcp_go_to(sl_bt_ots_client_handle_t client,
                                        sl_bt_ots_object_id_t     *object)
{
  (void)object;
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  CHECK_NULL(object);
  // Check status of the client
  CHECK_STATE(client);

  uint8_t content[SL_BT_OTS_OBJECT_ID_SIZE + sizeof(sl_bt_ots_olcp_opcode_t)];

  sl_bt_ots_olcp_message_t *message = (sl_bt_ots_olcp_message_t *)&content;
  message->opcode = SL_BT_OTS_OLCP_OPCODE_GO_TO;
  memcpy(message->data, object->data, sizeof(object->data));
  sc = send_olcp_request(client,
                         sizeof(object->data) + sizeof(sl_bt_ots_olcp_opcode_t),
                         (uint8_t *)message);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = message->opcode;
  }
  return sc;
}

sl_status_t sl_bt_ots_client_olcp_order(sl_bt_ots_client_handle_t   client,
                                        sl_bt_ots_list_sort_order_t order)
{
  (void)order;
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);

  uint8_t content[sizeof(sl_bt_ots_list_sort_order_t) + sizeof(sl_bt_ots_olcp_opcode_t)];

  sl_bt_ots_olcp_message_t *message = (sl_bt_ots_olcp_message_t *)&content;
  message->opcode = SL_BT_OTS_OLCP_OPCODE_ORDER;
  *((sl_bt_ots_list_sort_order_t *)message->data) = order;
  sc = send_olcp_request(client,
                         sizeof(sl_bt_ots_list_sort_order_t) + sizeof(sl_bt_ots_olcp_opcode_t),
                         (uint8_t *)&message);
  if (sc == SL_STATUS_OK) {
    client->active_opcode = message->opcode;
  }
  return sc;
}

sl_status_t sl_bt_ots_client_olcp_request_number_of_objects(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);

  sl_bt_ots_olcp_opcode_t opcode = SL_BT_OTS_OLCP_OPCODE_REQUEST_NUMBER_OF_OBJECTS;
  sc = send_olcp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_olcp_clear_marking(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);

  sl_bt_ots_olcp_opcode_t opcode = SL_BT_OTS_OLCP_OPCODE_CLEAR_MARKING;
  sc = send_olcp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_read_object_list_filter(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;
  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Check if the characteristic is supported
  if (client->gattdb_handles.characteristics.handles.object_list_filter == INVALID_CHARACTERISTIC_HANDLE) {
    return SL_STATUS_NOT_SUPPORTED;
  }
  // Read characteristic
  sc = sl_bt_gatt_read_characteristic_value(client->connection,
                                            client->gattdb_handles.characteristics.handles.object_list_filter);
  // Set state
  SET_STATE_READ(client, client->gattdb_handles.characteristics.handles.object_list_filter);

  return sc;
}

sl_status_t sl_bt_ots_client_write_object_list_filter(sl_bt_ots_client_handle_t              client,
                                                      sl_bt_ots_object_list_filter_content_t filter)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t buffer[OTS_CLIENT_BUFFER_SIZE];

  // Check arguments
  CHECK_NULL(client);
  // Check status of the client
  CHECK_STATE(client);
  // Check if the characteristic is supported
  if (client->gattdb_handles.characteristics.handles.object_list_filter == INVALID_CHARACTERISTIC_HANDLE) {
    return SL_STATUS_NOT_SUPPORTED;
  }

  uint16_t length = 0;
  buffer[0] = filter.filter_type;

  switch (filter.filter_type) {
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NO_FILTER:
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MARKED_OBJECTS:
      // No parameters
      break;
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_CONTAINS:
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_IS_EXACTLY:
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_STARTS_WITH:
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_ENDS_WITH:
      length = filter.parameters.name.length;
      // Check for NULL
      CHECK_NULL(filter.parameters.name.name);
      // Copy data after type
      memcpy(&buffer[1], filter.parameters.name.name, length);
      break;
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_OBJECT_TYPE:
      // Check for NULL
      CHECK_NULL(filter.parameters.type.uuid_data);
      if (filter.parameters.type.uuid_is_sig) {
        length = SL_BT_OTS_UUID_SIZE_16;
      } else {
        length = SL_BT_OTS_UUID_SIZE_128;
      }
      // Copy data after type
      memcpy(&buffer[1], filter.parameters.type.uuid_data, length);
      break;
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CREATED_BETWEEN:
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MODIFIED_BETWEEN:
      length = sizeof(filter.parameters.time);
      // Copy data after type
      memcpy(&buffer[1], &filter.parameters.time, length);
      break;
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CURRENT_SIZE_BETWEEN:
    case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_ALLOCATED_SIZE_BETWEEN:
      length = sizeof(filter.parameters.size);
      // Copy data after type
      memcpy(&buffer[1], &filter.parameters.size, length);
      break;
  }

  // Write characteristic
  sc = sl_bt_gatt_write_characteristic_value(client->connection,
                                             client->gattdb_handles.characteristics.handles.object_list_filter,
                                             length + 1,
                                             buffer);
  // Set state
  SET_STATE_WRITE(client, client->gattdb_handles.characteristics.handles.object_list_filter);

  return sc;
}

sl_status_t sl_bt_ots_client_oacp_create_object(sl_bt_ots_client_handle_t client,
                                                uint32_t                  size,
                                                sl_bt_ots_object_type_t   type)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check arguments
  CHECK_NULL(client);
  CHECK_STATE(client);

  if (size == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t content[sizeof(sl_bt_ots_olcp_opcode_t) + sizeof(sl_bt_ots_oacp_create_parameters_t)];
  sl_bt_ots_oacp_message_t *message = (sl_bt_ots_oacp_message_t *)&content;
  sl_bt_ots_oacp_create_parameters_t *parameters = (sl_bt_ots_oacp_create_parameters_t *)message->data;

  message->opcode  = SL_BT_OTS_OACP_OPCODE_CREATE;
  parameters->size = size;

  uint8_t type_size  = type.uuid_is_sig ? SL_BT_OTS_UUID_SIZE_16 : SL_BT_OTS_UUID_SIZE_128;
  uint8_t total_size = sizeof(message->opcode) + sizeof(size) + type_size;
  memcpy(parameters->type, type.uuid_data, type_size);

  sc = send_oacp_request(client, total_size, (uint8_t *)message);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = message->opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_oacp_delete_object(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check arguments
  CHECK_NULL(client);
  CHECK_STATE(client);

  sl_bt_ots_oacp_opcode_t opcode = SL_BT_OTS_OACP_OPCODE_DELETE;

  sc = send_oacp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_oacp_calculate_checksum(sl_bt_ots_client_handle_t client,
                                                     uint32_t                  offset,
                                                     uint32_t                  length)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check arguments
  CHECK_NULL(client);
  CHECK_STATE(client);

  if (length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t content[sizeof(sl_bt_ots_olcp_opcode_t) + sizeof(sl_bt_ots_oacp_calculate_checksum_parameters_t)];
  sl_bt_ots_oacp_message_t *message = (sl_bt_ots_oacp_message_t *)&content;
  sl_bt_ots_oacp_calculate_checksum_parameters_t *parameters
    = (sl_bt_ots_oacp_calculate_checksum_parameters_t *)message->data;

  message->opcode    = SL_BT_OTS_OACP_OPCODE_CALCULATE_CHECKSUM;
  parameters->length = length;
  parameters->offset = offset;

  uint8_t total_size = sizeof(message->opcode) + sizeof(sl_bt_ots_oacp_calculate_checksum_parameters_t);

  sc = send_oacp_request(client, total_size, (uint8_t *)message);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = message->opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_oacp_execute(sl_bt_ots_client_handle_t client,
                                          uint8_t                   *optional_data,
                                          uint8_t                    optional_data_size)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;

  // Check arguments
  CHECK_NULL(client);
  CHECK_STATE(client);

  if (optional_data_size != 0 && optional_data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  uint16_t total_size = sizeof(sl_bt_ots_oacp_opcode_t) + optional_data_size;
  if (total_size > SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_DATA_SIZE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t content[SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_DATA_SIZE];
  sl_bt_ots_oacp_message_t *message = (sl_bt_ots_oacp_message_t *)&content;

  message->opcode = SL_BT_OTS_OACP_OPCODE_EXECUTE;
  memcpy(message->data, optional_data, optional_data_size);

  sc = send_oacp_request(client, total_size, (uint8_t *)&message);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = message->opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_oacp_read(sl_bt_ots_client_handle_t client,
                                       uint32_t                  offset,
                                       uint32_t                  length,
                                       uint16_t                  max_sdu,
                                       uint16_t                  max_pdu)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check arguments
  CHECK_NULL(client);
  CHECK_STATE(client);

  if (max_sdu < 23 || max_sdu > 65533 || max_pdu < 23 || max_pdu > 252) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t total_size = sizeof(sl_bt_ots_oacp_opcode_t) + sizeof(sl_bt_ots_oacp_read_parameters_t);
  uint8_t content[sizeof(sl_bt_ots_oacp_opcode_t) + sizeof(sl_bt_ots_oacp_read_parameters_t)];
  sl_bt_ots_oacp_message_t *message = (sl_bt_ots_oacp_message_t *)&content;
  sl_bt_ots_oacp_read_parameters_t *parameters
    = (sl_bt_ots_oacp_read_parameters_t *)message->data;

  message->opcode    = SL_BT_OTS_OACP_OPCODE_READ;
  parameters->length = length;
  parameters->offset = offset;

  sc = send_oacp_request(client, total_size, (uint8_t *)message);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = message->opcode;
    client->active_transfer_sdu = max_sdu;
    client->active_transfer_pdu = max_pdu;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_oacp_write(sl_bt_ots_client_handle_t   client,
                                        uint32_t                    offset,
                                        uint32_t                    length,
                                        sl_bt_ots_oacp_write_mode_t mode,
                                        uint16_t                    max_sdu,
                                        uint16_t                    max_pdu)
{
  sl_status_t sc = SL_STATUS_OK;

  (void)mode;
  // Check arguments
  CHECK_NULL(client);
  CHECK_STATE(client);
  if (max_sdu < 23 || max_sdu > 65533 || max_pdu < 23 || max_pdu > 252) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  uint8_t total_size = sizeof(sl_bt_ots_oacp_opcode_t) + sizeof(sl_bt_ots_oacp_write_parameters_t);
  uint8_t content[sizeof(sl_bt_ots_oacp_opcode_t) + sizeof(sl_bt_ots_oacp_write_parameters_t)];
  sl_bt_ots_oacp_message_t *message = (sl_bt_ots_oacp_message_t *)&content;
  sl_bt_ots_oacp_write_parameters_t *parameters
    = (sl_bt_ots_oacp_write_parameters_t *)message->data;

  message->opcode    = SL_BT_OTS_OACP_OPCODE_WRITE;
  parameters->length = length;
  parameters->offset = offset;
  parameters->mode   = mode;

  sc = send_oacp_request(client, total_size, (uint8_t *)message);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = message->opcode;
    client->active_transfer_size = length;
    client->active_transfer_offset = offset;
    client->active_transfer_sdu = max_sdu;
    client->active_transfer_pdu = max_pdu;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_oacp_abort(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_OK;

  // Check arguments
  CHECK_NULL(client);

  // Check status for read in progress
  if ((client->status) != CLIENT_STATUS_WAIT_OACP_TRANSFER
      || client->active_opcode != SL_BT_OTS_OACP_OPCODE_READ) {
    return SL_STATUS_INVALID_STATE;
  }

  sl_bt_ots_oacp_opcode_t opcode = SL_BT_OTS_OACP_OPCODE_ABORT;

  sc = send_oacp_request(client, sizeof(opcode), (uint8_t *)&opcode);

  if (sc == SL_STATUS_OK) {
    client->active_opcode = opcode;
  }

  return sc;
}

sl_status_t sl_bt_ots_client_increase_credit(sl_bt_ots_client_handle_t client,
                                             uint16_t                  credit)
{
  sl_status_t sc = SL_STATUS_INVALID_STATE;
  // Check arguments
  CHECK_NULL(client);

  if (client->status == CLIENT_STATUS_WAIT_OACP_TRANSFER) {
    sc = sl_bt_l2cap_transfer_check_progress(&client->l2cap_transfer);
    if (sc == SL_STATUS_IN_PROGRESS) {
      sc = sl_bt_l2cap_transfer_increase_credit(&client->l2cap_transfer,
                                                credit);
    }
  }
  return sc;
}

sl_status_t sl_bt_ots_client_abort(sl_bt_ots_client_handle_t client)
{
  sl_status_t sc = SL_STATUS_INVALID_STATE;

  // Check arguments
  CHECK_NULL(client);

  // Check state
  if (client->status == CLIENT_STATUS_WAIT_OACP_TRANSFER) {
    if (client->active_opcode == SL_BT_OTS_OACP_OPCODE_READ) {
      // Abort read in a gentle way.
      sc = sl_bt_ots_client_oacp_abort(client);
    } else if (client->active_opcode == SL_BT_OTS_OACP_OPCODE_WRITE) {
      // Check progress of the operation
      sc = sl_bt_l2cap_transfer_check_progress(&client->l2cap_transfer);
      if (sc == SL_STATUS_IN_PROGRESS) {
        // Abort the L2CAP transfer
        sc = sl_bt_l2cap_transfer_abort_transfer(&client->l2cap_transfer);
      }
    }
  }
  return sc;
}

void sli_bt_ots_client_init(void)
{
  for (uint8_t connection_index = 0; connection_index < SL_BT_CONFIG_MAX_CONNECTIONS; connection_index++) {
    // Read queue
    app_queue_init(&read_queue[connection_index],
                   SL_BT_OTS_CLIENT_CONFIG_READ_REQUEST_QUEUE_SIZE,
                   sizeof(read_queue_item_t),
                   (uint8_t *)read_data[connection_index]);
    // Write queue
    app_queue_init(&write_queue[connection_index],
                   SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_QUEUE_SIZE,
                   sizeof(write_queue_item_t),
                   (uint8_t *)write_data[connection_index]);
  }
}

void sli_bt_ots_client_step(void)
{
  sl_status_t sc = SL_STATUS_OK;
  sl_bt_ots_client_t *handle;

  // Check init needs
  SL_SLIST_FOR_EACH_ENTRY(client_list, handle, sl_bt_ots_client_t, node) {
    uint8_t connection_index = HANDLE_TO_INDEX(handle->connection);
    // Check if needs and can init
    if (handle->status == CLIENT_STATUS_BEGIN
        && active_client[connection_index] == NULL) {
      // Try to start init with the discovery
      sc = sl_bt_gatt_discover_characteristics(handle->connection,
                                               handle->gattdb_handles.service);
      if (sc == SL_STATUS_OK) {
        // Initialize status to discovery
        active_client[connection_index] = handle;
        handle->status = CLIENT_STATUS_DISCOVERY;
      } else {
        finish_init(handle, SL_STATUS_ABORT);
      }
    }
  }

  // Iterate over queues
  for (uint8_t connection_index = 0; connection_index < SL_BT_CONFIG_MAX_CONNECTIONS; connection_index++) {
    sc = SL_STATUS_OK;

    if (active_client[connection_index] == NULL) {
      // Send out read requests for the given connection
      if (!app_queue_is_empty(&read_queue[connection_index])) {
        read_queue_item_t item;
        sc = app_queue_peek(&read_queue[connection_index], (uint8_t *)&item);
        if (sc == SL_STATUS_OK) {
          sc = sl_bt_gatt_read_characteristic_value(INDEX_TO_HANDLE(connection_index),
                                                    item.attribute_handle);
          if (sc == SL_STATUS_OK) {
            (void)app_queue_remove(&read_queue[connection_index], (uint8_t *)&item);
            // Set active client for the connection
            active_client[connection_index] = item.client;
          }
        }
      }
    }

    sc = SL_STATUS_OK;
    if (active_client[connection_index] == NULL) {
      // Send out write requests for the given connection
      if (!app_queue_is_empty(&write_queue[connection_index])) {
        write_queue_item_t item;
        sc = app_queue_peek(&write_queue[connection_index], (uint8_t *)&item);
        if (sc == SL_STATUS_OK) {
          sc = sl_bt_gatt_write_characteristic_value(INDEX_TO_HANDLE(connection_index),
                                                     item.attribute_handle,
                                                     item.size,
                                                     item.data);
          if (sc == SL_STATUS_OK) {
            (void)app_queue_remove(&write_queue[connection_index], (uint8_t *)&item);
            // Set active client for the connection
            active_client[connection_index] = item.client;
          }
        }
      }
    }
  }
}

void sli_bt_ots_client_on_bt_event(sl_bt_msg_t *evt)
{
  sl_bt_ots_client_t *handle;
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      break;
    case sl_bt_evt_connection_opened_id:
      SL_SLIST_FOR_EACH_ENTRY(client_list, handle, sl_bt_ots_client_t, node) {
        if (handle->connection == evt->data.evt_connection_opened.connection) {
          CALL_SAFE(handle, on_connect, handle);
        }
      }
      break;
    case sl_bt_evt_connection_closed_id:
      SL_SLIST_FOR_EACH_ENTRY(client_list, handle, sl_bt_ots_client_t, node) {
        if (handle->connection == evt->data.evt_connection_closed.connection) {
          // Clear outgoing queues
          clear_queue(handle->connection);
          // Clear active client
          if (active_client[HANDLE_TO_INDEX(handle->connection)] == handle) {
            active_client[HANDLE_TO_INDEX(handle->connection)] = NULL;
          }
          // Set status
          handle->status = CLIENT_STATUS_DISCONNECTED;

          // Remove client from the list
          sl_slist_remove(&client_list, &handle->node);

          // Do callback
          CALL_SAFE(handle, on_disconnect, handle);
        }
      }
      break;
    case sl_bt_evt_gatt_procedure_completed_id:
      // Check for active client
      handle = active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_procedure_completed.connection)];
      if (handle != NULL) {
        switch (handle->status) {
          case CLIENT_STATUS_WAIT_READ:
            if (evt->data.evt_gatt_procedure_completed.result != 0) {
              // Send read response with failure
              CALL_SAFE(handle,
                        on_metadata_read_finished,
                        handle,
                        &handle->current_object,
                        evt->data.evt_gatt_procedure_completed.result,
                        handle->active_handle_index,
                        NULL);
            } else {
              handle_gatt_read_response(handle,
                                        handle->gattdb_handles.characteristics.array[handle->active_handle_index],
                                        0,
                                        (uint8array *)&handle->received_buffer);
            }

            // Clear state
            CLEAR_STATE(handle);
            // Clear active client
            active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_procedure_completed.connection)] = NULL;
            break;
          case CLIENT_STATUS_WAIT_WRITE:
            handle_gatt_write_response(handle,
                                       evt->data.evt_gatt_procedure_completed.result);
            // Clear state
            CLEAR_STATE(handle);
            // Clear active client
            active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_procedure_completed.connection)] = NULL;
            break;
          case CLIENT_STATUS_WAIT_OACP:
            // OACP write finished
            if (evt->data.evt_gatt_procedure_completed.result != 0) {
              CALL_SAFE(handle,
                        on_oacp_response,
                        handle,
                        &handle->current_object,
                        handle->active_opcode,
                        evt->data.evt_gatt_procedure_completed.result,
                        SL_BT_OTS_OACP_RESPONSE_CODE_OPERATION_FAILED,
                        NULL);
              CLEAR_STATE(handle);
              // Clear active client
              active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_procedure_completed.connection)] = NULL;
            } else {
              // Wait for indication
              SET_STATE_OACP_INDICATION(handle);
            }
            break;
          case CLIENT_STATUS_WAIT_OLCP:
            if (evt->data.evt_gatt_procedure_completed.result != 0) {
              CALL_SAFE(handle,
                        on_olcp_response,
                        handle,
                        &handle->current_object,
                        handle->active_opcode,
                        evt->data.evt_gatt_procedure_completed.result,
                        SL_BT_OTS_OLCP_RESPONSE_CODE_OPEATION_FAILED,
                        0);
              CLEAR_STATE(handle);
              // Clear active client
              active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_procedure_completed.connection)] = NULL;
            } else {
              SET_STATE_OLCP_INDICATION(handle);
            }
            break;
          case CLIENT_STATUS_DISCOVERY:

            // Discovery finished, check the result
            if (check_characteristics(handle)) {
              if (handle->gattdb_handles.characteristics.handles.object_id == INVALID_CHARACTERISTIC_HANDLE
                  || handle->gattdb_handles.characteristics.handles.object_list_control_point == INVALID_CHARACTERISTIC_HANDLE) {
                handle->single_object = true;
              }
              // Subscribe to next
              subscribe(handle, CLIENT_STATUS_SUBSCRIBE_OACP);
            } else {
              finish_init(handle, SL_STATUS_ABORT);
            }
            break;
          case CLIENT_STATUS_SUBSCRIBE_OACP:
            handle->subscription.subscribed.oacp = 1;
            CALL_SAFE(handle,
                      on_subscription_change,
                      handle,
                      handle->subscription);
            // Subscribe to next
            subscribe(handle, CLIENT_STATUS_SUBSCRIBE_OLCP);
            break;
          case CLIENT_STATUS_SUBSCRIBE_OLCP:
            handle->subscription.subscribed.olcp = 1;
            CALL_SAFE(handle,
                      on_subscription_change,
                      handle,
                      handle->subscription);
            // Subscribe to next
            subscribe(handle, CLIENT_STATUS_SUBSCRIBE_OBJECT_CHANGED);
            break;
          case CLIENT_STATUS_SUBSCRIBE_OBJECT_CHANGED:
            handle->subscription.subscribed.object_changed = 1;
            CALL_SAFE(handle,
                      on_subscription_change,
                      handle,
                      handle->subscription);
            finish_init(handle, SL_STATUS_OK);
          default:
            break;
        }
      }
      break;
    case sl_bt_evt_gatt_characteristic_value_id:
      handle = active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_characteristic_value.connection)];
      // Handle response
      if (handle != NULL && handle->status == CLIENT_STATUS_WAIT_READ) {
        uint8array *buffer = (uint8array *)&handle->received_buffer;
        buffer->len = evt->data.evt_gatt_characteristic_value.value.len;
        memcpy(buffer->data, evt->data.evt_gatt_characteristic_value.value.data, buffer->len);
      }
      // Handle indication
      if (evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication
          && handle != NULL) {
        uint8_t att_error = ATT_ERR_SUCCESS;
        // OLCP indication
        if (handle->status == CLIENT_STATUS_WAIT_OLCP_INDICATION
            && evt->data.evt_gatt_characteristic_value.characteristic
            == handle->gattdb_handles.characteristics.handles.object_list_control_point) {
          sl_bt_ots_olcp_response_message_t *response
            = (sl_bt_ots_olcp_response_message_t *)evt->data.evt_gatt_characteristic_value.value.data;
          uint16_t size_minimum = sizeof(sl_bt_ots_olcp_opcode_t)
                                  + sizeof(sl_bt_ots_olcp_opcode_t)
                                  + sizeof (sl_bt_ots_olcp_response_code_t);
          uint32_t data = 0;
          // Check for the opcode
          if (response->opcode == handle->active_opcode) {
            // Check minimum size
            if (evt->data.evt_gatt_characteristic_value.value.len >= size_minimum) {
              // Check for invalid size
              if (response->opcode == SL_BT_OTS_OLCP_OPCODE_REQUEST_NUMBER_OF_OBJECTS
                  && evt->data.evt_gatt_characteristic_value.value.len
                  != sizeof(sl_bt_ots_olcp_response_message_t)) {
                att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
              }
              // Set data for number of objects
              if (response->opcode == SL_BT_OTS_OLCP_OPCODE_REQUEST_NUMBER_OF_OBJECTS
                  && att_error == ATT_ERR_SUCCESS) {
                data = response->number_of_objects;
              }
            } else {
              att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
            }
          } else {
            // Set an error
            att_error = ATT_ERR_VALUE_NOT_ALLOWED;
          }
          // Set state back
          CLEAR_STATE(handle);
          // Clear active client, release the connection
          active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_procedure_completed.connection)] = NULL;
          // Call back with the calculated ATT error and data
          CALL_SAFE(handle,
                    on_olcp_response,
                    handle,
                    &handle->current_object,
                    response->opcode,
                    att_error,
                    response->response,
                    data);
        }
        // OACP indication
        if (handle->status == CLIENT_STATUS_WAIT_OACP_INDICATION
            && evt->data.evt_gatt_characteristic_value.characteristic
            == handle->gattdb_handles.characteristics.handles.object_action_control_point) {
          // Response arrived for OACP
          sl_bt_ots_oacp_response_message_t *response
            = (sl_bt_ots_oacp_response_message_t *)evt->data.evt_gatt_characteristic_value.value.data;
          sl_bt_ots_oacp_response_data_t data;
          sl_bt_ots_oacp_response_code_t response_code = response->response;

          uint16_t size_minimum = sizeof(sl_bt_ots_oacp_opcode_t)
                                  + sizeof(sl_bt_ots_oacp_opcode_t)
                                  + sizeof (sl_bt_ots_oacp_response_code_t);
          uint16_t size_required = size_minimum;

          CLEAR_STATE(handle);

          // Check if the opcode matches
          if (response->opcode == handle->active_opcode) {
            // Check opcode minimum length
            if (evt->data.evt_gatt_characteristic_value.value.len >= size_minimum) {
              // Calculate required length
              switch (response->opcode) {
                case SL_BT_OTS_OACP_OPCODE_ABORT:
                case SL_BT_OTS_OACP_OPCODE_CREATE:
                case SL_BT_OTS_OACP_OPCODE_DELETE:
                  break;
                case SL_BT_OTS_OACP_OPCODE_READ:
                case SL_BT_OTS_OACP_OPCODE_WRITE:
                  break;
                case SL_BT_OTS_OACP_OPCODE_CALCULATE_CHECKSUM:
                  size_required += sizeof(uint32_t);
                  data.checksum = *((uint32_t *)response->data);
                  break;
                case SL_BT_OTS_OACP_OPCODE_EXECUTE:
                  size_required = evt->data.evt_gatt_characteristic_value.value.len;
                  data.execute.len = size_required - size_minimum;
                  data.execute.data = response->data;
                  break;
                default:
                  // Unknown opcode response
                  break;
              }
              // Check required length
              if (evt->data.evt_gatt_characteristic_value.value.len != size_required) {
                att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
              }
            } else {
              att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
            }
            // For Read and Write, if there is no error, L2CAP transfer should
            // be created
            if (att_error == ATT_ERR_SUCCESS
                && response_code == SL_BT_OTS_OACP_RESPONSE_CODE_SUCCESS
                && (response->opcode == SL_BT_OTS_OACP_OPCODE_READ
                    || response->opcode == SL_BT_OTS_OACP_OPCODE_WRITE)) {
              // Set transfer parameters
              handle->l2cap_transfer.callbacks   = &l2cap_transfer_callbacks;
              handle->l2cap_transfer.connection  = handle->connection;
              handle->l2cap_transfer.data_length = handle->active_transfer_size;
              handle->l2cap_transfer.data_offset = handle->active_transfer_offset;
              handle->l2cap_transfer.mode
                = (response->opcode == SL_BT_OTS_OACP_OPCODE_WRITE) ? SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT : SL_BT_L2CAP_TRANSFER_MODE_RECEIVE;

              handle->l2cap_transfer.max_pdu = handle->active_transfer_pdu;
              handle->l2cap_transfer.max_sdu = handle->active_transfer_sdu;

              handle->l2cap_transfer.credit = 1;
              handle->l2cap_transfer.spsm = L2CAP_SPSM;

              // Start data transfer
              sc = sl_bt_l2cap_transfer_start_data_transfer(&handle->l2cap_transfer,
                                                            true);
              if (sc == SL_STATUS_OK) {
                // Overwrite state to wait for the Object Transfer
                handle->status = CLIENT_STATUS_WAIT_OACP_TRANSFER;
              } else {
                // Clear state
                response_code = SL_BT_OTS_OACP_RESPONSE_CODE_CHANNEL_UNAVAILABLE;
              }
            }
          } else {
            // Set an error
            att_error = ATT_ERR_VALUE_NOT_ALLOWED;
          }
          // Clear active client, release the connection
          active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_procedure_completed.connection)] = NULL;
          // Call back using OACP response and ATT error code set before
          CALL_SAFE(handle,
                    on_oacp_response,
                    handle,
                    &handle->current_object,
                    response->opcode,
                    att_error,
                    response_code,
                    &data);
        }
        // Object Changed indication
        if (evt->data.evt_gatt_characteristic_value.characteristic
            == handle->gattdb_handles.characteristics.handles.object_changed) {
          if (evt->data.evt_gatt_characteristic_value.value.len == sizeof(sl_bt_ots_object_changed_content_t)) {
            sl_bt_ots_object_changed_content_t *content
              = (sl_bt_ots_object_changed_content_t *)evt->data.evt_gatt_characteristic_value.value.data;
            CALL_SAFE(handle,
                      on_object_change,
                      handle,
                      content->flags,
                      &content->object);
          }
        }
        // Send back Confirmation
        sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
      }
      break;
    case sl_bt_evt_gatt_characteristic_id:
      // Check status
      handle = active_client[HANDLE_TO_INDEX(evt->data.evt_gatt_characteristic.connection)];
      if (handle != NULL && handle->status == CLIENT_STATUS_DISCOVERY) {
        // Characteristic found
        if (evt->data.evt_gatt_characteristic.uuid.len == SL_BT_OTS_UUID_SIZE_16) {
          // Search for UUID
          uint16_t uuid = *((uint16_t *)evt->data.evt_gatt_characteristic.uuid.data);
          for (uint8_t i = 0; i < SL_BT_OTS_CHARACTERISTIC_UUID_COUNT; i++ ) {
            if (uuid == sl_bt_ots_characteristic_uuids[i] ) {
              handle->gattdb_handles.characteristics.array[i]
                = evt->data.evt_gatt_characteristic.characteristic;
            }
          }
        }
      }
      break;
  }
}
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT

static bool client_has_task(void)
{
  // Check for active queue
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (!app_queue_is_empty(&write_queue[i])
        || !app_queue_is_empty(&read_queue[i])) {
      return true;
    }
  }
  return false;
}

sl_power_manager_on_isr_exit_t sli_bt_ots_client_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;
  if (client_has_task()) {
    ret = SL_POWER_MANAGER_WAKEUP;
  }
  return ret;
}

bool sli_bt_ots_client_is_ok_to_sleep(void)
{
  bool ret = true;
  if (client_has_task()) {
    ret = false;
  }
  return ret;
}

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

// -----------------------------------------------------------------------------
// Private functions

static bool check_characteristics(sl_bt_ots_client_handle_t client)
{
  // Check mandatory characteristics
  CHECK_CHARACTERISTIC(ots_feature);
  CHECK_CHARACTERISTIC(object_name);
  CHECK_CHARACTERISTIC(object_type);
  CHECK_CHARACTERISTIC(object_size);
  CHECK_CHARACTERISTIC(object_properties);
  CHECK_CHARACTERISTIC(object_action_control_point);
  if (client->gattdb_handles.characteristics.handles.object_id == INVALID_CHARACTERISTIC_HANDLE
      || client->gattdb_handles.characteristics.handles.object_list_control_point == INVALID_CHARACTERISTIC_HANDLE) {
    client->single_object = true;
  }
  return true;
}

// L2CAP transfer callback for data transmission
static void l2cap_transfer_data_transmit(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                         uint32_t                               offset,
                                         uint32_t                               size,
                                         uint8_t                                **data,
                                         uint32_t                               *data_size)
{
  sl_bt_ots_client_t *handle;

  // Search for owner and call back
  SL_SLIST_FOR_EACH_ENTRY(client_list, handle, sl_bt_ots_client_t, node) {
    if (transfer_object == &handle->l2cap_transfer) {
      CALL_SAFE(handle,
                on_data_transmit,
                handle,
                &handle->current_object,
                offset,
                size,
                data,
                data_size);
    }
  }
}

// L2CAP transfer callback for data reception
static uint16_t l2cap_transfer_data_received(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                             uint32_t                               offset,
                                             uint8array                             *data)
{
  sl_bt_ots_client_t *handle;

  // Search for owner and call back
  SL_SLIST_FOR_EACH_ENTRY(client_list, handle, sl_bt_ots_client_t, node) {
    if (transfer_object == &handle->l2cap_transfer
        && handle->callbacks->on_data_receive != NULL) {
      uint16_t credit = handle->callbacks->on_data_receive(handle,
                                                           &handle->current_object,
                                                           offset,
                                                           data->data,
                                                           data->len);
      return credit;
    }
  }
  return 0;
}

// L2CAP transfer callback for transfer finish
static void l2cap_transfer_transfer_finished(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                             sl_status_t                            error_code)
{
  sl_bt_ots_client_t *handle;

  // Search for owner and call back
  SL_SLIST_FOR_EACH_ENTRY(client_list, handle, sl_bt_ots_client_t, node) {
    if (transfer_object == &handle->l2cap_transfer) {
      sl_bt_ots_transfer_result_t result = SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_SUCCESS;
      if (error_code != SL_STATUS_OK) {
        result = SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_CHANNEL_ERROR;
      }
      // Active transfer clear
      handle->active_transfer_size   = 0;
      handle->active_transfer_offset = 0;
      handle->active_transfer_sdu    = 0;
      handle->active_transfer_pdu    = 0;

      CALL_SAFE(handle,
                on_data_transfer_finished,
                handle,
                &handle->current_object,
                result);
      // Clear state of the client
      CLEAR_STATE(handle);
    }
  }
}

// L2CAP transfer callback for channel open
static void l2cap_transfer_channel_opened(sl_bt_l2cap_transfer_transfer_handle_t transfer_object)
{
  (void)transfer_object;
}

static void handle_gatt_write_response(sl_bt_ots_client_t *client,
                                       uint16_t result)
{
  if (client->active_handle_index == SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LIST_FILTER) {
    if (client->callbacks->on_filter_write != NULL) {
      client->callbacks->on_filter_write(client, result);
    }
  } else {
    sl_bt_ots_object_metadata_write_event_type_t event_type;
    event_type = (sl_bt_ots_object_metadata_write_event_type_t)client->active_handle_index;
    CALL_SAFE(client,
              on_metadata_write_finished,
              client,
              &client->current_object,
              event_type,
              result);
  }
}

static void handle_gatt_read_response(sl_bt_ots_client_t *client,
                                      uint16_t           characteristic,
                                      uint16_t           offset,
                                      uint8array         *data)
{
  (void)offset;
  sl_bt_ots_object_metadata_read_parameters_t parameter;
  if (characteristic == client->gattdb_handles.characteristics.handles.ots_feature) {
    if (data->len == sizeof(sl_bt_ots_features_t)) {
      if (client->callbacks->on_features_read != NULL) {
        CALL_SAFE(client,
                  on_features_read,
                  client,
                  SL_STATUS_OK,
                  *((sl_bt_ots_features_t *) data->data));
      }
    }
  } else if (characteristic == client->gattdb_handles.characteristics.handles.object_name) {
    if (data->len > 0) {
      if (client->callbacks->on_metadata_read_finished != NULL) {
        sl_bt_ots_object_metadata_read_parameters_t parameter;
        parameter.object_name.length = data->len;
        parameter.object_name.name   = (char *)data->data;
        CALL_SAFE(client,
                  on_metadata_read_finished,
                  client,
                  &client->current_object,
                  SL_STATUS_OK,
                  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_NAME,
                  &parameter);
      }
    }
  } else if (characteristic == client->gattdb_handles.characteristics.handles.object_type) {
    if (data->len == SL_BT_OTS_UUID_SIZE_16
        || data->len == SL_BT_OTS_UUID_SIZE_128 ) {
      if (client->callbacks->on_metadata_read_finished != NULL) {
        parameter.object_type.uuid_is_sig = (data->len == SL_BT_OTS_UUID_SIZE_16);
        parameter.object_type.uuid_data   = data->data;
        CALL_SAFE(client,
                  on_metadata_read_finished,
                  client,
                  &client->current_object,
                  SL_STATUS_OK,
                  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_TYPE,
                  &parameter);
      }
    }
  } else if (characteristic == client->gattdb_handles.characteristics.handles.object_size) {
    if (data->len == sizeof(sl_bt_ots_object_size_t)) {
      if (client->callbacks->on_metadata_read_finished != NULL) {
        CALL_SAFE(client,
                  on_metadata_read_finished,
                  client,
                  &client->current_object,
                  SL_STATUS_OK,
                  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_SIZE,
                  (sl_bt_ots_object_metadata_read_parameters_t *) data->data);
      }
    }
  } else if (characteristic == client->gattdb_handles.characteristics.handles.object_first_created) {
    if (data->len == sizeof(sl_bt_ots_time_t)) {
      if (client->callbacks->on_metadata_read_finished != NULL) {
        CALL_SAFE(client,
                  on_metadata_read_finished,
                  client,
                  &client->current_object,
                  SL_STATUS_OK,
                  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_FIRST_CREATED,
                  ((sl_bt_ots_object_metadata_read_parameters_t *)data->data));
      }
    }
  } else if (characteristic == client->gattdb_handles.characteristics.handles.object_last_modified) {
    if (data->len == sizeof(sl_bt_ots_time_t)) {
      if (client->callbacks->on_metadata_read_finished != NULL) {
        CALL_SAFE(client,
                  on_metadata_read_finished,
                  client,
                  &client->current_object,
                  SL_STATUS_OK,
                  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_LAST_MODIFIED,
                  ((sl_bt_ots_object_metadata_read_parameters_t *)data->data));
      }
    }
  } else if (characteristic ==  client->gattdb_handles.characteristics.handles.object_id) {
    if (data->len == sizeof(sl_bt_ots_object_id_t)) {
      if (client->callbacks->on_metadata_read_finished != NULL) {
        // Set current object
        memcpy(client->current_object.data,
               data->data,
               sizeof(sl_bt_ots_object_id_t));
        // Call back
        CALL_SAFE(client,
                  on_metadata_read_finished,
                  client,
                  &client->current_object,
                  SL_STATUS_OK,
                  SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_ID,
                  ((sl_bt_ots_object_metadata_read_parameters_t *)data->data));
      }
    }
  } else if (characteristic == client->gattdb_handles.characteristics.handles.object_properties) {
    if (data->len == sizeof(sl_bt_ots_object_properties_t)) {
      CALL_SAFE(client,
                on_metadata_read_finished,
                client,
                &client->current_object,
                SL_STATUS_OK,
                SL_BT_OTS_OBJECT_METADATA_READ_OBJECT_PROPERTIES,
                ((sl_bt_ots_object_metadata_read_parameters_t *)data->data));
    }
  } else if (characteristic ==  client->gattdb_handles.characteristics.handles.object_list_filter) {
    if (data->len > 0 && client->callbacks->on_filter_read != NULL) {
      uint8_t filter   = data->data[0];
      uint8_t parameter_length   = data->len - 1;
      bool filter_is_valid = false;
      sl_bt_ots_object_list_filter_content_t content;
      content.filter_type = filter;
      switch (filter) {
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NO_FILTER:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MARKED_OBJECTS:
          if (parameter_length == 0) {
            filter_is_valid = true;
          }
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_CONTAINS:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_STARTS_WITH:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_ENDS_WITH:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_IS_EXACTLY:
          content.parameters.name.length = parameter_length;
          content.parameters.name.name   = (char *)&(data->data[1]);
          filter_is_valid = true;
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CREATED_BETWEEN:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MODIFIED_BETWEEN:
          if (parameter_length == sizeof(sl_bt_ots_time_t)) {
            memcpy(content.parameters.time.from.data, &(data->data[1]), parameter_length);
            filter_is_valid = true;
          }
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_OBJECT_TYPE:
          if (parameter_length == SL_BT_OTS_UUID_SIZE_16
              || parameter_length == SL_BT_OTS_UUID_SIZE_128) {
            content.parameters.type.uuid_is_sig = (parameter_length == SL_BT_OTS_UUID_SIZE_16);
            content.parameters.type.uuid_data = &data->data[1];
            filter_is_valid = true;
          }
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CURRENT_SIZE_BETWEEN:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_ALLOCATED_SIZE_BETWEEN:
          if (parameter_length == sizeof(sl_bt_ots_object_list_filter_size_parameters_t)) {
            memcpy(&content.parameters.size, &data->data[1], parameter_length);
            filter_is_valid = true;
          }
          break;
      }
      if (filter_is_valid) {
        CALL_SAFE(client, on_filter_read, client, SL_STATUS_OK, content);
      }
    }
  }
}

static sl_bt_ots_client_status_t finish_init(sl_bt_ots_client_t *client,
                                             sl_status_t error)
{
  // Set status accordingly
  client->status = (error == SL_STATUS_OK) ? CLIENT_STATUS_INITIALIZED : CLIENT_STATUS_ERROR;

  // Call back
  CALL_SAFE(client, on_init, client, error);

  // Clear active client for connection
  active_client[HANDLE_TO_INDEX(client->connection)] = NULL;

  if (client->status == CLIENT_STATUS_ERROR) {
    // Remove client from the list
    sl_slist_remove(&client_list, &client->node);
  }

  return client->status;
}

static void subscribe(sl_bt_ots_client_t *client,
                      sl_bt_ots_client_status_t status)
{
  sl_status_t sc;
  uint16_t characteristic = INVALID_CHARACTERISTIC_HANDLE;
  sl_bt_ots_client_status_t next_status = status;

  // Find next applicable subscription
  while (characteristic == INVALID_CHARACTERISTIC_HANDLE
         && next_status != CLIENT_STATUS_INITIALIZED) {
    sc = SL_STATUS_OK;

    switch (next_status) {
      case CLIENT_STATUS_SUBSCRIBE_OACP:
        characteristic = client->gattdb_handles.characteristics.handles.object_action_control_point;
        if (characteristic == INVALID_CHARACTERISTIC_HANDLE) {
          next_status = CLIENT_STATUS_SUBSCRIBE_OLCP;
        }
        break;
      case CLIENT_STATUS_SUBSCRIBE_OLCP:
        characteristic = client->gattdb_handles.characteristics.handles.object_list_control_point;
        if (characteristic == INVALID_CHARACTERISTIC_HANDLE) {
          next_status = CLIENT_STATUS_SUBSCRIBE_OBJECT_CHANGED;
        }
        break;
      case CLIENT_STATUS_SUBSCRIBE_OBJECT_CHANGED:
        characteristic = client->gattdb_handles.characteristics.handles.object_changed;
        if (characteristic == INVALID_CHARACTERISTIC_HANDLE) {
          next_status = CLIENT_STATUS_INITIALIZED;
        }
        break;
      default:
        break;
    }
    // If there is an applicable subscription
    if (characteristic != INVALID_CHARACTERISTIC_HANDLE) {
      sc = sl_bt_gatt_set_characteristic_notification(client->connection,
                                                      characteristic,
                                                      sl_bt_gatt_indication);
      if (sc != SL_STATUS_OK) {
        finish_init(client, sc);
      } else {
        client->status = next_status;
      }
      return;
    }
  }

  if (next_status == CLIENT_STATUS_INITIALIZED) {
    finish_init(client, SL_STATUS_OK);
  }
}

static sl_status_t send_write_request(sl_bt_ots_client_t                  *client,
                                      sl_bt_ots_characteristic_uuid_index handle_index,
                                      uint16_t                            size,
                                      uint8_t                             *data)
{
  sl_status_t sc = SL_STATUS_IN_PROGRESS;

  uint16_t characteristic = client->gattdb_handles.characteristics.array[handle_index];
  uint8_t connection_index = HANDLE_TO_INDEX(client->connection);

  // If the connection is not used
  if (active_client[connection_index] == NULL) {
    // Try to send out the request
    sc = sl_bt_gatt_write_characteristic_value(client->connection,
                                               characteristic,
                                               size,
                                               data);
  }
  if (sc == SL_STATUS_IN_PROGRESS) {
    // An operation is in progress, add to queue to send out later

    // Construct queue item
    write_queue_item_t queue_item;
    queue_item.client = client;
    queue_item.attribute_handle = characteristic;
    queue_item.size = size;
    memcpy(queue_item.data, data, size);

    // Add to the queue
    sc = app_queue_add(&write_queue[connection_index], (uint8_t*)&queue_item);

    // Set write state if adding was successful
    if (sc == SL_STATUS_OK) {
      SET_STATE_WRITE(client, handle_index);
    }
  } else if (sc == SL_STATUS_OK) {
    // Set write state if sending request was successful
    SET_STATE_WRITE(client, handle_index);
    // Set active client for the connection
    active_client[connection_index] = client;
  }
  return sc;
}

static sl_status_t send_oacp_request(sl_bt_ots_client_t *client,
                                     uint16_t           size,
                                     uint8_t            *data)
{
  sl_status_t sc = SL_STATUS_IN_PROGRESS;

  // Set OACP as index
  uint8_t handle_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OACP;
  uint8_t connection_index = HANDLE_TO_INDEX(client->connection);

  uint16_t characteristic = client->gattdb_handles.characteristics.array[handle_index];

  // If the connection is not used
  if (active_client[connection_index] == NULL) {
    // Try to send out the request
    sc = sl_bt_gatt_write_characteristic_value(client->connection,
                                               characteristic,
                                               size,
                                               data);
  }

  if (sc == SL_STATUS_IN_PROGRESS) {
    // An operation is in progress, add to queue to send out later

    // Construct queue item
    write_queue_item_t queue_item;
    queue_item.client = client;
    queue_item.attribute_handle = characteristic;
    queue_item.size = size;
    memcpy(queue_item.data, data, size);

    // Add to the queue
    sc = app_queue_add(&write_queue[connection_index], (uint8_t*)&queue_item);

    // Set write state if adding was successful
    if (sc == SL_STATUS_OK) {
      SET_STATE_OACP(client);
    }
  } else if (sc == SL_STATUS_OK) {
    // Set write state if sending request was successful
    SET_STATE_OACP(client);
    // Set active client for the connection
    active_client[connection_index] = client;
  }
  return sc;
}

static sl_status_t send_read_request(sl_bt_ots_client_t                  *client,
                                     sl_bt_ots_characteristic_uuid_index handle_index)
{
  sl_status_t sc = SL_STATUS_IN_PROGRESS;

  uint16_t characteristic = client->gattdb_handles.characteristics.array[handle_index];
  uint8_t connection_index = HANDLE_TO_INDEX(client->connection);

  // If the connection is not used
  if (active_client[connection_index] == NULL) {
    // Try to send out the request
    sc = sl_bt_gatt_read_characteristic_value(client->connection,
                                              characteristic);
  }
  if (sc == SL_STATUS_IN_PROGRESS) {
    // An operation is in progress, add to queue

    // Construct queue item
    read_queue_item_t queue_item;
    queue_item.client = client;
    queue_item.attribute_handle = characteristic;

    // Add to the queue
    sc = app_queue_add(&read_queue[connection_index], (uint8_t*)&queue_item);

    // Set read state if adding was successful
    if (sc == SL_STATUS_OK) {
      SET_STATE_READ(client, handle_index);
    }
  } else if (sc == SL_STATUS_OK) {
    // Set read state if sending request was successful
    SET_STATE_READ(client, handle_index);
    // Set active client for the connection
    active_client[connection_index] = client;
  }
  return sc;
}

static sl_status_t send_olcp_request(sl_bt_ots_client_t *client,
                                     uint16_t           size,
                                     uint8_t            *data)
{
  sl_status_t sc = SL_STATUS_IN_PROGRESS;

  // Set OACP as index
  uint8_t handle_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OLCP;
  uint8_t connection_index = HANDLE_TO_INDEX(client->connection);

  uint16_t characteristic = client->gattdb_handles.characteristics.array[handle_index];

  // If the connection is not used
  if (active_client[connection_index] == NULL) {
    // Try to send out the request
    sc = sl_bt_gatt_write_characteristic_value(client->connection,
                                               characteristic,
                                               size,
                                               data);
  }

  if (sc == SL_STATUS_IN_PROGRESS) {
    // An operation is in progress, add to queue to send out later
    // Construct queue item
    write_queue_item_t queue_item;
    queue_item.client = client;
    queue_item.attribute_handle = characteristic;
    queue_item.size = size;
    memcpy(queue_item.data, data, size);

    // Add to the queue
    sc = app_queue_add(&write_queue[connection_index], (uint8_t*)&queue_item);

    // Set write state if adding was successful
    if (sc == SL_STATUS_OK) {
      SET_STATE_OLCP(client);
    }
  } else if (sc == SL_STATUS_OK) {
    // Set write state if sending request was successful
    SET_STATE_OLCP(client);
    // Set active client for the connection
    active_client[connection_index] = client;
  }
  return sc;
}

static void clear_queue(uint8_t connection)
{
  // Reinit queues
  uint8_t connection_index = HANDLE_TO_INDEX(connection);

  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  // Read queue
  app_queue_init(&read_queue[connection_index],
                 SL_BT_OTS_CLIENT_CONFIG_READ_REQUEST_QUEUE_SIZE,
                 sizeof(read_queue_item_t),
                 (uint8_t *)read_data[connection_index]);
  // Write queue
  app_queue_init(&write_queue[connection_index],
                 SL_BT_OTS_CLIENT_CONFIG_WRITE_REQUEST_QUEUE_SIZE,
                 sizeof(write_queue_item_t),
                 (uint8_t *)write_data[connection_index]);
  CORE_EXIT_CRITICAL();
}
