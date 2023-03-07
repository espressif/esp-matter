/***************************************************************************//**
 * @file
 * @brief Object Transfer Service Object Server implementation
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

#include "sl_bt_ots_server.h"
#include "sl_bt_ots_server_instances.h"
#include "sli_bt_ots_datatypes.h"
#include "sl_status.h"

#define OTS_L2CAP_MAX_PDU       250
#define OTS_L2CAP_MAX_SDU       252
#define OTS_L2CAP_SDU_PDU_DIFF  (OTS_L2CAP_MAX_SDU - OTS_L2CAP_MAX_PDU)

#define OTS_SERVER_BUFFER_SIZE        255

#define OACP_MESSAGE_LENGTH_CREATE_16           sizeof(sl_bt_ots_oacp_opcode_t) + sizeof(uint32_t) + SL_BT_OTS_UUID_SIZE_16
#define OACP_MESSAGE_LENGTH_CREATE_128          sizeof(sl_bt_ots_oacp_opcode_t) + sizeof(uint32_t) + SL_BT_OTS_UUID_SIZE_128
#define OACP_MESSAGE_LENGTH_DELETE              sizeof(sl_bt_ots_oacp_opcode_t)
#define OACP_MESSAGE_LENGTH_CALCULATE_CHECKSUM  sizeof(sl_bt_ots_oacp_opcode_t) + (sizeof(sl_bt_ots_oacp_calculate_checksum_parameters_t))
#define OACP_MESSAGE_LENGTH_EXECUTE_MIN         sizeof(sl_bt_ots_oacp_opcode_t)
#define OACP_MESSAGE_LENGTH_READ                sizeof(sl_bt_ots_oacp_opcode_t) + (sizeof(sl_bt_ots_oacp_read_parameters_t))
#define OACP_MESSAGE_LENGTH_WRITE               sizeof(sl_bt_ots_oacp_opcode_t) + (sizeof(sl_bt_ots_oacp_write_parameters_t))
#define OACP_MESSAGE_LENGTH_ABORT               sizeof(sl_bt_ots_oacp_opcode_t)
#define OACP_MESSAGE_LENGTH_RESPONSE_CODE       sizeof(sl_bt_ots_oacp_opcode_t)

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS
#define OLCP_MESSAGE_LENGTH_GO_TO      sizeof(sl_bt_ots_olcp_opcode_t) + sizeof(sl_bt_ots_object_id_complete_t)
#define OLCP_MESSAGE_LENGTH_ORDER      sizeof(sl_bt_ots_olcp_opcode_t) + sizeof(sl_bt_ots_olcp_order_parameters_t)
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS

#define CALL_SAFE(handle, cb, ...)      \
  if (handle->callbacks->cb != NULL) {  \
    handle->callbacks->cb(__VA_ARGS__); \
  }

// -----------------------------------------------------------------------------
// Type definitions

/// Type for indication queue
typedef struct {
  uint8_t  connection;
  uint16_t gattdb_handle;
  uint8_t  data_length;
  uint8_t  data[SL_BT_OTS_SERVER_CONFIG_INDICATION_SIZE_MAX];
} indication_queue_item_t;

// -----------------------------------------------------------------------------
// Forward declaration of private functions

// L2CAP transfer callback for data transmit
static void l2cap_transfer_data_transmit_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                uint32_t                               offset,
                                                uint32_t                               size,
                                                uint8_t                                **data,
                                                uint32_t                               *data_size);

// L2CAP transfer callback for data reception
static uint16_t l2cap_transfer_data_received_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                    uint32_t                               offset,
                                                    uint8array                             *data);

// L2CAP transfer callback for transfer finish
static void l2cap_transfer_transfer_finished_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                    sl_status_t error_code);

// L2CAP transfer callback for channel open
static void l2cap_transfer_channel_opened_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object);

// Utilities
static sl_bt_ots_server_client_db_entry_t *find_client(sl_bt_ots_server_t *server, uint8_t connection_handle);

static sl_bt_ots_server_client_db_entry_t *find_client_by_transfer(sl_bt_ots_server_t *server, sl_bt_l2cap_transfer_transfer_handle_t transfer);

static sl_bt_ots_characteristic_uuid_index find_characteristic_index(uint16_t handle,
                                                                     sl_bt_ots_server_t **server);
static bool check_object_valid(sl_bt_ots_object_id_t *object);
static void set_object_invalid(sl_bt_ots_object_id_t *object);

// Connection handling
static void client_disconnected(uint16_t connection);
static void client_connected(uint16_t connection);

// GATT reads
static void handle_gatt_read(sl_bt_evt_gatt_server_user_read_request_t *read_request);
static uint8_t handle_metadata_read(sl_bt_ots_server_t *server,
                                    sl_bt_ots_server_client_db_entry_t *client,
                                    sl_bt_ots_characteristic_uuid_index characteristic_index);

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT
static uint8_t handle_filter_read(sl_bt_ots_server_t *server,
                                  sl_bt_ots_server_client_db_entry_t *client);
static uint8_t handle_filter_write(sl_bt_ots_server_t *server,
                                   sl_bt_ots_server_client_db_entry_t *client,
                                   sl_bt_evt_gatt_server_user_write_request_t *write_request);
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT

// GATT Writes
static void handle_gatt_write(sl_bt_evt_gatt_server_user_write_request_t *write_request);
static uint8_t handle_metadata_write(sl_bt_ots_server_t *server,
                                     sl_bt_ots_server_client_db_entry_t *client,
                                     sl_bt_ots_characteristic_uuid_index characteristic_index,
                                     sl_bt_evt_gatt_server_user_write_request_t *write_request);

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS
static uint8_t handle_olcp_write(sl_bt_ots_server_t *server,
                                 sl_bt_ots_server_client_db_entry_t *client,
                                 sl_bt_evt_gatt_server_user_write_request_t *write_request);
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS

static uint8_t handle_oacp_write(sl_bt_ots_server_t *server,
                                 sl_bt_ots_server_client_db_entry_t *client,
                                 sl_bt_evt_gatt_server_user_write_request_t *write_request);

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
// Object change helpers
static sl_status_t send_object_changed(sl_bt_ots_server_handle_t        server,
                                       sl_bt_ots_object_id_t            *object,
                                       sl_bt_ots_object_changed_flags_t flags,
                                       uint16_t                         client);
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT

static void handle_cccd(sl_bt_evt_gatt_server_characteristic_status_t *characteristic_status);

// Queue management
static void clear_queue(uint8_t connection);

static sl_status_t send_indication(sl_bt_ots_server_client_db_entry_t *client_entry,
                                   uint8_t                            connection,
                                   uint16_t                           characteristic,
                                   size_t                             value_len,
                                   const uint8_t                      *value);

static bool transfer_is_in_progress(sl_bt_ots_server_handle_t        server,
                                    sl_bt_ots_object_id_t            *object);

// -----------------------------------------------------------------------------
// Private variables

// Callbacks for L2CAP Transfer
static sl_bt_l2cap_transfer_callbacks_t l2cap_transfer_callbacks_server = {
  .on_transmit = l2cap_transfer_data_transmit_server,
  .on_receive  = l2cap_transfer_data_received_server,
  .on_open     = l2cap_transfer_channel_opened_server,
  .on_finish   = l2cap_transfer_transfer_finished_server
};

// -----------------------------------------------------------------------------
// Public functions

sl_status_t sl_bt_ots_server_init(sl_bt_ots_server_handle_t    server,
                                  sl_bt_ots_gattdb_handles_t   *handles,
                                  sl_bt_ots_server_callbacks_t *callbacks)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t i;

  // Check arguments
  CHECK_NULL(server);
  CHECK_NULL(handles);
  CHECK_NULL(callbacks);

  // Assign variables
  server->callbacks = callbacks;
  server->gattdb_handles = handles;

  // Initialize client database
  for (i = 0; i < server->concurrency; i++) {
    // Clear current object
    set_object_invalid(&server->client_db[i].current_object);
    // Clear connection handle
    server->client_db[i].connection_handle = INVALID_CONNECTION_HANDLE;
    // Clear subscription status
    server->client_db[i].subscription_status.data = SL_BT_OTS_SUBSCRIPTION_STATUS_NONE;
    // Add transfer callbacks
    server->client_db[i].l2cap_transfer.callbacks = &l2cap_transfer_callbacks_server;
    // Set L2CAP inactive
    server->client_db[i].l2cap_transfer.mode = SL_BT_L2CAP_TRANSFER_MODE_INACTIVE;
    // Set current object properties
    server->client_db[i].current_object_properties = NULL;
    // Set current object properties
    server->client_db[i].current_object_size = NULL;
  }

  if (server->capability_init != NULL) {
    server->capability_init();
  }

  return sc;
}

sl_status_t sl_bt_ots_server_set_current_object(sl_bt_ots_server_handle_t     server,
                                                uint16_t                      client,
                                                sl_bt_ots_object_id_t         *object,
                                                sl_bt_ots_object_properties_t *properties,
                                                sl_bt_ots_object_size_t       *size)
{
  uint8_t i;
  // Check arguments
  CHECK_NULL(server);

  // Search for client in client database
  for (i = 0; i < server->concurrency; i++) {
    // Set client if found
    if (server->client_db[i].connection_handle == client) {
      // Check Object
      if (object == NULL) {
        // Set to invalid, if the object is NULL
        set_object_invalid(&server->client_db[i].current_object);
        // Set Object Properties to NULL
        server->client_db[i].current_object_properties = NULL;
        // Set Object Size to NULL
        server->client_db[i].current_object_size = NULL;
      } else {
        // Copy Object ID to Current Object ID
        memcpy(server->client_db[i].current_object.data,
               object->data,
               SL_BT_OTS_OBJECT_ID_SIZE);
        // Set Object Properties
        server->client_db[i].current_object_properties = properties;
        // Set Object Size
        server->client_db[i].current_object_size = size;
      }
      return SL_STATUS_OK;
    }
  }

  // Client not found
  return SL_STATUS_NOT_FOUND;
}

sl_status_t sl_bt_ots_server_get_current_object(sl_bt_ots_server_handle_t server,
                                                uint16_t                  client,
                                                sl_bt_ots_object_id_t     *object)
{
  uint8_t i;
  // Check arguments
  CHECK_NULL(server);
  CHECK_NULL(object);

  // Search for client in client database
  for (i = 0; i < server->concurrency; i++) {
    // Set client if found
    if (server->client_db[i].connection_handle == client) {
      memcpy(object->data,
             server->client_db[i].current_object.data,
             SL_BT_OTS_OBJECT_ID_SIZE);
      return SL_STATUS_OK;
    }
  }

  // Client not found
  return SL_STATUS_NOT_FOUND;
}

sl_status_t sl_bt_ots_server_abort(sl_bt_ots_server_handle_t server,
                                   uint16_t                  client)
{
  sl_bt_ots_server_client_db_entry_t *client_entry = NULL;

  // Check arguments
  CHECK_NULL(server);

  // Find client
  client_entry = find_client(server, client);

  // If client has found
  if (client_entry != NULL) {
    // If transfer in progress, abort and return
    return sl_bt_l2cap_transfer_abort_transfer(&client_entry->l2cap_transfer);
  }

  return SL_STATUS_NOT_FOUND;
}

sl_status_t sl_bt_ots_server_object_changed(sl_bt_ots_server_handle_t        server,
                                            sl_bt_ots_object_id_t            *object,
                                            sl_bt_ots_object_changed_flags_t flags)
{
  sl_status_t sc = SL_STATUS_NOT_SUPPORTED;

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
  // Clear flags as it is a server initiated change
  sl_bt_ots_object_changed_flags_t new_flags = flags & ~SL_BT_OTS_OBJECT_CHANGE_SOURCE_MASK;

  // Send indications to clients
  sc = send_object_changed(server, object, new_flags, INVALID_CONNECTION_HANDLE);
#else // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
  (void)server;
  (void)object;
  (void)flags;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT

  return sc;
}

sl_status_t sl_bt_ots_server_increase_credit(sl_bt_ots_server_handle_t server,
                                             uint16_t                  client,
                                             uint16_t                  credit)
{
  sl_bt_ots_server_client_db_entry_t *client_entry = NULL;
  sl_status_t sc = SL_STATUS_NOT_FOUND;

  // Check arguments
  CHECK_NULL(server);

  // Find client
  client_entry = find_client(server, client);

  if (client_entry != NULL) {
    // Check for transfer in progress
    sc = sl_bt_l2cap_transfer_check_progress(&client_entry->l2cap_transfer);
    if (sc == SL_STATUS_IN_PROGRESS) {
      sc = sl_bt_l2cap_transfer_increase_credit(&client_entry->l2cap_transfer, credit);
    } else {
      sc = SL_STATUS_INVALID_STATE;
    }
  }
  return sc;
}

// -----------------------------------------------------------------------------
// Event handlers

void sli_bt_ots_server_on_bt_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      break;
    case sl_bt_evt_connection_opened_id:
      // Connection opened
      client_connected(evt->data.evt_connection_opened.connection);
      break;
    case sl_bt_evt_connection_closed_id:
      // Connection closed
      client_disconnected(evt->data.evt_connection_closed.connection);
      break;
    case sl_bt_evt_gatt_server_user_read_request_id:
      // Handle read requests
      handle_gatt_read(&evt->data.evt_gatt_server_user_read_request);
      break;
    case sl_bt_evt_gatt_server_user_write_request_id:
      // Handle write requests
      handle_gatt_write(&evt->data.evt_gatt_server_user_write_request);
      break;
    case sl_bt_evt_gatt_server_characteristic_status_id:
      handle_cccd(&evt->data.evt_gatt_server_characteristic_status);
      break;
    case sl_bt_evt_gatt_server_indication_timeout_id:
      // Indication timed out
      break;
  }
}

void sli_bt_ots_server_step(void)
{
  sl_status_t sc;

  for (uint8_t s = 0; s < SL_BT_OTS_SERVER_COUNT; s++) {
    const sl_bt_ots_server_t *server = sl_bt_ots_server_instances[s];
    for (uint8_t index = 0; index < server->concurrency; index++) {
      app_queue_t queue = server->client_db[index].indication_queue;
      if (!app_queue_is_empty(&queue)) {
        // Send out the indication
        indication_queue_item_t item;
        CORE_DECLARE_IRQ_STATE;
        CORE_ENTER_CRITICAL();
        sc = app_queue_peek(&queue, (uint8_t *)&item);
        if (sc == SL_STATUS_OK) {
          // Calling from critical section
          sc = sl_bt_gatt_server_send_indication(item.connection,
                                                 item.gattdb_handle,
                                                 item.data_length,
                                                 item.data);
          if (sc == SL_STATUS_OK) {
            (void)app_queue_remove(&queue, (uint8_t *)&item);
          }
        }
        CORE_EXIT_CRITICAL();
      }
    }
  }
}

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT

static bool server_has_task(void)
{
  for (uint8_t s = 0; s < SL_BT_OTS_SERVER_COUNT; s++) {
    const sl_bt_ots_server_t *server = sl_bt_ots_server_instances[s];
    for (uint8_t index = 0; index < server->concurrency; index++) {
      if (!app_queue_is_empty(&server->client_db[index].indication_queue)) {
        return true;
      }
    }
  }
  return false;
}

sl_power_manager_on_isr_exit_t sli_bt_ots_server_sleep_on_isr_exit(void)
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;
  if (server_has_task()) {
    ret = SL_POWER_MANAGER_WAKEUP;
  }
  return ret;
}

bool sli_bt_ots_server_is_ok_to_sleep(void)
{
  bool ret = true;
  if (server_has_task()) {
    ret = false;
  }
  return ret;
}

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

// -----------------------------------------------------------------------------
// Private functions

// L2CAP transfer callback for data transmission
static void l2cap_transfer_data_transmit_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                uint32_t                               offset,
                                                uint32_t                               size,
                                                uint8_t                                **data,
                                                uint32_t                               *data_size)
{
  uint8_t index = 0;
  sl_bt_ots_server_t *server = NULL;
  sl_bt_ots_server_client_db_entry_t *client = NULL;

  for (index = 0; index < SL_BT_OTS_SERVER_COUNT; index++) {
    server = (sl_bt_ots_server_t *)sl_bt_ots_server_instances[index];

    client = find_client_by_transfer(server, transfer_object);

    if (client != NULL) {
      CALL_SAFE(server,
                on_data_transmit,
                server,
                client->connection_handle,
                &client->current_object,
                offset,
                size,
                data,
                data_size);
    }
  }
}

// L2CAP transfer callback for data reception
static uint16_t l2cap_transfer_data_received_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                    uint32_t                               offset,
                                                    uint8array                             *data)
{
  uint8_t index = 0;
  sl_bt_ots_server_t *server = NULL;
  sl_bt_ots_server_client_db_entry_t *client = NULL;

  for (index = 0; index < SL_BT_OTS_SERVER_COUNT; index++) {
    server = (sl_bt_ots_server_t *)sl_bt_ots_server_instances[index];

    client = find_client_by_transfer(server, transfer_object);

    if (client != NULL
        && (sl_bt_l2cap_transfer_transfer_t *)transfer_object == &client->l2cap_transfer
        && server->callbacks->on_data_received != NULL) {
      uint16_t credit = server->callbacks->on_data_received(server,
                                                            client->connection_handle,
                                                            &client->current_object,
                                                            offset,
                                                            data->data,
                                                            data->len);
      return credit;
    }
  }

  return 0;
}

// L2CAP transfer callback for transfer finish
static void l2cap_transfer_transfer_finished_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                    sl_status_t                                   error_code)
{
  uint8_t index = 0;
  sl_bt_ots_server_t *server = NULL;
  sl_bt_ots_server_client_db_entry_t *client = NULL;

  sl_bt_ots_transfer_result_t result = SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_SUCCESS;

  // Search for owner and call back
  for (index = 0; index < SL_BT_OTS_SERVER_COUNT; index++) {
    server = (sl_bt_ots_server_t *)sl_bt_ots_server_instances[index];
    client = find_client_by_transfer(server, transfer_object);

    if (error_code != SL_STATUS_OK) {
      result = SL_BT_OTS_TRANSFER_FINISHED_RESPONSE_CODE_CHANNEL_ERROR;
    }

    if (client != NULL) {
      CALL_SAFE(server,
                on_data_transfer_finished,
                server,
                client->connection_handle,
                &client->current_object,
                result);
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
      (void)send_object_changed(server,
                                &client->current_object,
                                SL_BT_OTS_OBJECT_CHANGE_SOURCE_MASK
                                | SL_BT_OTS_OBJECT_CHANGE_CONTENTS_MASK,
                                client->connection_handle);
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
    }
  }
}

// L2CAP transfer callback for channel open
static void l2cap_transfer_channel_opened_server(sl_bt_l2cap_transfer_transfer_handle_t transfer_object)
{
  (void)transfer_object;
}

static bool check_object_valid(sl_bt_ots_object_id_t *object)
{
  if (object == NULL) {
    return false;
  }
  // If usable data is not zero, ID is valid
  for (uint8_t i = 0; i < SL_BT_OTS_OBJECT_ID_USABLE_SIZE; i++) {
    if (object->id.usable[i] > 0) {
      return true;
    }
  }
  // If usable data is zero only, check for RFU
  return (object->id.rfu == 0);
}

static void set_object_invalid(sl_bt_ots_object_id_t *object)
{
  if (object != NULL) {
    // Initialize usable values
    memset(object->id.usable, 0, SL_BT_OTS_OBJECT_ID_USABLE_SIZE);
    // Set the RFU
    object->id.rfu = SL_BT_OTS_INVALID_OBJECT_RFU;
  }
}

static sl_bt_ots_characteristic_uuid_index find_characteristic_index(uint16_t handle,
                                                                     sl_bt_ots_server_t **server)
{
  *server = NULL;
  // Find handle on a server
  if (INVALID_CHARACTERISTIC_HANDLE == handle) {
    return SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID;
  }
  // Find handle on a server
  // Iterate over servers
  for (uint8_t s = 0; s < SL_BT_OTS_SERVER_COUNT; s++) {
    // Search in the handles
    for (uint8_t c = 0; c < SL_BT_OTS_CHARACTERISTIC_UUID_COUNT; c++) {
      if (sl_bt_ots_server_instances[s]->gattdb_handles->characteristics.array[c] == handle) {
        *server = (sl_bt_ots_server_t *)sl_bt_ots_server_instances[s];
        return (sl_bt_ots_characteristic_uuid_index)c;
      }
    }
  }
  return SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID;
}

static sl_bt_ots_server_client_db_entry_t *find_client(sl_bt_ots_server_t *server, uint8_t connection_handle)
{
  if (server != NULL) {
    for (uint8_t index = 0; index < server->concurrency; index++) {
      if (server->client_db[index].connection_handle == connection_handle) {
        return &server->client_db[index];
      }
    }
  }
  return NULL;
}

static sl_bt_ots_server_client_db_entry_t *find_client_by_transfer(sl_bt_ots_server_t *server, sl_bt_l2cap_transfer_transfer_handle_t transfer)
{
  if (server != NULL) {
    for (uint8_t index = 0; index < server->concurrency; index++) {
      if (&server->client_db[index].l2cap_transfer == (sl_bt_l2cap_transfer_transfer_t *)transfer) {
        return &server->client_db[index];
      }
    }
  }
  return NULL;
}

static void client_connected(uint16_t connection)
{
  // Assign the next client to servers
  for (uint8_t s = 0; s < SL_BT_OTS_SERVER_COUNT; s++) {
    // Check the status of the server
    if (sl_bt_ots_server_instances[s] != NULL
        && sl_bt_ots_server_instances[s]->callbacks != NULL) {
      sl_bt_ots_server_client_db_entry_t *client = find_client((sl_bt_ots_server_t *)sl_bt_ots_server_instances[s],
                                                               INVALID_CONNECTION_HANDLE);
      // Found an empty slot
      if (client != NULL) {
        // Set data
        client->connection_handle = connection;
        set_object_invalid(&client->current_object);
        client->subscription_status.data = SL_BT_OTS_SUBSCRIPTION_STATUS_NONE;
        // Call back
        if (sl_bt_ots_server_instances[s]->callbacks->on_client_connect) {
          sl_bt_ots_server_instances[s]->callbacks->on_client_connect((sl_bt_ots_server_t *)sl_bt_ots_server_instances[s],
                                                                      connection);
        }

        // Set up L2CAP transfer
        client->l2cap_transfer.connection = client->connection_handle;
      }
    }
  }
}

static void client_disconnected(uint16_t connection)
{
  for (uint8_t s = 0; s < SL_BT_OTS_SERVER_COUNT; s++) {
    sl_bt_ots_server_client_db_entry_t *client = find_client((sl_bt_ots_server_t *)sl_bt_ots_server_instances[s],
                                                             connection);
    if (client != NULL) {
      // Call back
      if (sl_bt_ots_server_instances[s]->callbacks->on_client_disconnect != NULL) {
        sl_bt_ots_server_instances[s]->callbacks->on_client_disconnect((sl_bt_ots_server_t *)sl_bt_ots_server_instances[s],
                                                                       connection);
      }

      // Abort transfer if it is in progress
      if (sl_bt_l2cap_transfer_check_progress(&client->l2cap_transfer)) {
        sl_bt_l2cap_transfer_abort_transfer(&client->l2cap_transfer);
      }

      // Clear client queue
      clear_queue(client->connection_handle);

      // Clear ID
      set_object_invalid(&client->current_object);

      // Clear client properties
      client->subscription_status.data = SL_BT_OTS_SUBSCRIPTION_STATUS_NONE;
      client->current_object_properties = NULL;
      client->current_object_size = NULL;
      client->l2cap_transfer_pdu = 0;
      client->l2cap_transfer_sdu = 0;

      // Remove client
      client->connection_handle = INVALID_CONNECTION_HANDLE;
    }
  }
}

static sl_status_t handle_feature_read(sl_bt_ots_server_t *server,
                                       sl_bt_ots_server_client_db_entry_t *client)
{
  sl_status_t sc;
  uint16_t sent_len;

  sc = sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                 server->gattdb_handles->characteristics.array[SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OTS_FEATURE],
                                                 ATT_ERR_SUCCESS,
                                                 sizeof(server->features),
                                                 (uint8_t *)&server->features,
                                                 &sent_len);
  return sc;
}

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT
static uint8_t handle_filter_read(sl_bt_ots_server_t *server,
                                  sl_bt_ots_server_client_db_entry_t *client)
{
  uint8_t att_error = ATT_ERR_SUCCESS;
  uint16_t sent_len;
  sl_bt_ots_object_list_filter_content_t filter;
  sl_status_t sc;
  uint8_t buffer[OTS_SERVER_BUFFER_SIZE];
  uint16_t length = 0;

  if (server->callbacks->on_object_list_filter_get != NULL) {
    sl_bt_ots_object_metadata_read_response_code_t response =
      server->callbacks->on_object_list_filter_get(server,
                                                   client->connection_handle,
                                                   &filter);
    if (response == ATT_ERR_SUCCESS) {
      buffer[0] = filter.filter_type;

      switch (filter.filter_type) {
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NO_FILTER:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MARKED_OBJECTS:
          // No params
          length = 0;
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_CONTAINS:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_STARTS_WITH:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_ENDS_WITH:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_IS_EXACTLY:
          length = filter.parameters.name.length;
          // Copy data after type
          memcpy(&buffer[1], filter.parameters.name.name, length);
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CREATED_BETWEEN:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MODIFIED_BETWEEN:
          length = sizeof(filter.parameters.time);
          memcpy(&buffer[1], &filter.parameters.time, length);
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_OBJECT_TYPE:
          if (filter.parameters.type.uuid_is_sig) {
            length = SL_BT_OTS_UUID_SIZE_16;
          } else {
            length = SL_BT_OTS_UUID_SIZE_128;
          }
          // Copy data after type
          memcpy(&buffer[1], filter.parameters.type.uuid_data, length);
          break;
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CURRENT_SIZE_BETWEEN:
        case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_ALLOCATED_SIZE_BETWEEN:
          length = sizeof(filter.parameters.size);
          // Copy data after type
          memcpy(&buffer[1], &filter.parameters.size, length);
          break;
        default:
          break;
      }
      // Sent read response
      sc = sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                     server->gattdb_handles->characteristics.array[SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LIST_FILTER],
                                                     ATT_ERR_SUCCESS,
                                                     length + 1,
                                                     (uint8_t *)buffer,
                                                     &sent_len);
      (void)sc;
    } else {
      if (response != ATT_ERR_SUCCESS) {
        att_error = response;
      } else {
        att_error = ATT_ERR_NOT_SUPPORTED;
      }
    }
  } else {
    // Not handled
    att_error = ATT_ERR_NOT_SUPPORTED;
  }

  return att_error;
}
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT

static uint8_t handle_metadata_read(sl_bt_ots_server_t *server,
                                    sl_bt_ots_server_client_db_entry_t *client,
                                    sl_bt_ots_characteristic_uuid_index characteristic_index)
{
  uint8_t att_error = ATT_ERR_SUCCESS;
  sl_bt_ots_object_metadata_read_parameters_t parameters;
  sl_bt_ots_object_metadata_read_event_type_t event;
  uint16_t sent_len;
  uint8_t len = 0;

  // Check if object is valid
  if (check_object_valid(&client->current_object)) {
    if (server->callbacks->on_object_metadata_read != NULL) {
      // Set event type
      event = (sl_bt_ots_object_metadata_read_event_type_t)characteristic_index;

      // Call back to higher layers
      sl_bt_ots_object_metadata_read_response_code_t response =
        server->callbacks->on_object_metadata_read(server,
                                                   client->connection_handle,
                                                   &client->current_object,
                                                   event,
                                                   &parameters);
      if (response == ATT_ERR_SUCCESS) {
        // Send read response for metadata
        switch (characteristic_index) {
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME:
            (void)sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                            server->gattdb_handles->characteristics.array[characteristic_index],
                                                            ATT_ERR_SUCCESS,
                                                            parameters.object_name.length,
                                                            (uint8_t *)parameters.object_name.name,
                                                            &sent_len);
            break;
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_TYPE:
            len = SL_BT_OTS_UUID_SIZE_128;
            if ( parameters.object_type.uuid_is_sig ) {
              len = SL_BT_OTS_UUID_SIZE_16;
            }
            (void)sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                            server->gattdb_handles->characteristics.array[characteristic_index],
                                                            ATT_ERR_SUCCESS,
                                                            len,
                                                            (uint8_t *)parameters.object_type.uuid_data,
                                                            &sent_len);
            break;
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_SIZE:
            (void)sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                            server->gattdb_handles->characteristics.array[characteristic_index],
                                                            ATT_ERR_SUCCESS,
                                                            sizeof(parameters.object_size),
                                                            (uint8_t *)&parameters.object_size,
                                                            &sent_len);
            break;
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED:
            if (server->capabilities.capability_time
                && server->capabilities.capability_first_created) {
              (void)sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                              server->gattdb_handles->characteristics.array[characteristic_index],
                                                              ATT_ERR_SUCCESS,
                                                              sizeof(parameters.object_first_created),
                                                              (uint8_t *)&parameters.object_first_created,
                                                              &sent_len);
            } else {
              att_error = ATT_ERR_NOT_SUPPORTED;
            }
            break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED:
            if (server->capabilities.capability_time
                && server->capabilities.capability_last_modified) {
              (void)sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                              server->gattdb_handles->characteristics.array[characteristic_index],
                                                              ATT_ERR_SUCCESS,
                                                              sizeof(parameters.object_last_modified),
                                                              (uint8_t *)&parameters.object_last_modified,
                                                              &sent_len);
            } else {
              att_error = ATT_ERR_NOT_SUPPORTED;
            }
            break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_ID:
            (void)sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                            server->gattdb_handles->characteristics.array[characteristic_index],
                                                            ATT_ERR_SUCCESS,
                                                            sizeof(parameters.object_id.data),
                                                            (uint8_t *)&parameters.object_id.data,
                                                            &sent_len);
            break;
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES:
            (void)sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                            server->gattdb_handles->characteristics.array[characteristic_index],
                                                            ATT_ERR_SUCCESS,
                                                            sizeof(parameters.object_properties),
                                                            (uint8_t *)&parameters.object_properties,
                                                            &sent_len);
            break;
          default:
            att_error = ATT_ERR_ATTRIBUTE_NOT_FOUND;
            break;
        }
      } else {
        if (response != ATT_ERR_SUCCESS) {
          att_error = response;
        } else {
          att_error = ATT_ERR_NOT_SUPPORTED;
        }
      }
    } else {
      // No callback present
      att_error = ATT_ERR_NOT_SUPPORTED;
    }
  } else {
    // Object is not valid
    att_error = ATT_ERR_OBJECT_NOT_SELECTED;
  }
  return att_error;
}

static void handle_gatt_read(sl_bt_evt_gatt_server_user_read_request_t *read_request)
{
  sl_bt_ots_server_t *server = NULL;
  sl_bt_ots_server_client_db_entry_t *client = NULL;
  sl_bt_ots_characteristic_uuid_index characteristic_index =  SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID;
  uint8_t att_error = ATT_ERR_SUCCESS;

  // Find characteristic handle
  characteristic_index = find_characteristic_index(read_request->characteristic,
                                                   &server);
  // Check server hand characteristic index
  if (server != NULL && characteristic_index != SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID) {
    // Identify client
    client = find_client(server, read_request->connection);

    // check if client is found
    if (client != NULL) {
      switch (characteristic_index) {
        // Handle feature
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OTS_FEATURE:
          (void)handle_feature_read(server, client);
          break;
        // Handle metadata
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME:
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_TYPE:
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_SIZE:
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED:
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED:
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_ID:
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES:
          att_error = handle_metadata_read(server,
                                           client,
                                           characteristic_index);
          break;
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT
        // Handle Object List Filter
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LIST_FILTER:
          if (server->capabilities.capability_object_list_filter) {
            att_error = handle_filter_read(server, client);
          } else {
            att_error = ATT_ERR_NOT_SUPPORTED;
          }
          break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT
        default:
          att_error = ATT_ERR_ATTRIBUTE_NOT_FOUND;
          break;
      }
    }
  }
  if (att_error != ATT_ERR_SUCCESS) {
    sl_status_t sc;
    uint16_t sent_len;
    sc = sl_bt_gatt_server_send_user_read_response(client->connection_handle,
                                                   server->gattdb_handles->characteristics.array[SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OTS_FEATURE],
                                                   att_error,
                                                   0,
                                                   NULL,
                                                   &sent_len);
    (void)sc;
  }
}

static uint8_t handle_oacp_write(sl_bt_ots_server_t *server,
                                 sl_bt_ots_server_client_db_entry_t *client,
                                 sl_bt_evt_gatt_server_user_write_request_t *write_request)
{
  sl_bt_ots_oacp_response_code_t response_code = SL_BT_OTS_OACP_RESPONSE_CODE_SUCCESS;

  bool truncation_requested = false;
  bool patching_requested = false;

  // Return value for the function
  uint8_t sc = ATT_ERR_SUCCESS;

  // general message frame that will be forwarded to event handler
  sl_bt_ots_oacp_message_t *oacp_msg = ((sl_bt_ots_oacp_message_t *)write_request->value.data);
  sl_bt_ots_oacp_parameters_t oacp_msg_parameters;

  // Needed for checksum and execute response process
  sl_bt_ots_oacp_response_data_t oacp_response_data;
  uint8_t oacp_response_data_len = 0;
  // Buffer to initialize response message
  uint8_t buffer[SL_BT_OTS_SERVER_CONFIG_INDICATION_SIZE_MAX];
  // Needed for BLE indication message
  sl_bt_ots_oacp_response_message_t *oacp_response_msg = (sl_bt_ots_oacp_response_message_t *)buffer;

  // Message length is incorrect
  if (write_request->value.len < sizeof(sl_bt_ots_oacp_opcode_t)) {
    sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
    return sc;
  }

  // Object is not selected
  if (!check_object_valid(&client->current_object)) {
    sc = ATT_ERR_OBJECT_NOT_SELECTED;
    return sc;
  }
  if (!client->subscription_status.subscribed.oacp) {
    sc = ATT_ERR_IMPORER_CCCD;
    return sc;
  }

  uint8_t *data_pointer = (uint8_t *)&oacp_msg->opcode;
  data_pointer += sizeof(sl_bt_ots_oacp_opcode_t);

  switch (oacp_msg->opcode) {
    case SL_BT_OTS_OACP_OPCODE_CREATE:
      if ((server->features.oacp_features
           & (SL_BT_OTS_OACP_FEATURE_CREATE_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Parameter length does are incorrect
      if (write_request->value.len != OACP_MESSAGE_LENGTH_CREATE_16
          && write_request->value.len != OACP_MESSAGE_LENGTH_CREATE_128) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }

      oacp_msg_parameters.create.size = *(uint32_t *)data_pointer;
      data_pointer += sizeof(oacp_msg_parameters.create.size);

      if (write_request->value.len == OACP_MESSAGE_LENGTH_CREATE_16) {
        memcpy(&oacp_msg_parameters.create.type, data_pointer, SL_BT_OTS_UUID_SIZE_16);
      } else if (write_request->value.len == OACP_MESSAGE_LENGTH_CREATE_128) {
        memcpy(&oacp_msg_parameters.create.type, data_pointer, SL_BT_OTS_UUID_SIZE_128);
      }

      // Check if it is used in another transfer on the server
      if (transfer_is_in_progress(server, &client->current_object)) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_OBJECT_LOCKED;
      }
      break;

    case SL_BT_OTS_OACP_OPCODE_DELETE:
      if ((server->features.oacp_features
           & (SL_BT_OTS_OACP_FEATURE_CREATE_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Parameter length does are incorrect
      if (write_request->value.len != OACP_MESSAGE_LENGTH_DELETE) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }

      // Handle properties
      if (client->current_object_properties != NULL) {
        sl_bt_ots_object_properties_t properties = *client->current_object_properties;
        // Check if it is not writable
        if ( !(properties & SL_BT_OTS_OBJECT_PROPERTY_DELETE_MASK) ) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        }
      }

      break;

    case SL_BT_OTS_OACP_OPCODE_CALCULATE_CHECKSUM:
      if ((server->features.oacp_features
           & (SL_BT_OTS_OACP_FEATURE_CALCULATE_CHECKSUM_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Parameters are incorrect
      if (write_request->value.len != OACP_MESSAGE_LENGTH_CALCULATE_CHECKSUM) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }

      oacp_msg_parameters.calculate_checksum.offset = *(uint32_t *)data_pointer;
      data_pointer += sizeof(oacp_msg_parameters.calculate_checksum.offset);

      oacp_msg_parameters.calculate_checksum.length = *(uint32_t *)data_pointer;

      if (oacp_msg_parameters.calculate_checksum.length == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
        break;
      }

      // Handle size conditions
      if (client->current_object_size != NULL) {
        // Check for overflow
        if ( (oacp_msg_parameters.calculate_checksum.offset
              + oacp_msg_parameters.calculate_checksum.length)
             > client->current_object_size->current_size) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
          break;
        }
      }

      // Check if it is used in another transfer on the server
      if (transfer_is_in_progress(server, &client->current_object)) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_OBJECT_LOCKED;
      }

      break;

    case SL_BT_OTS_OACP_OPCODE_EXECUTE:
      if ((server->features.oacp_features
           & (SL_BT_OTS_OACP_FEATURE_EXECUTE_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Handle properties
      if (client->current_object_properties != NULL) {
        sl_bt_ots_object_properties_t properties = *client->current_object_properties;
        // Check if it is not writable
        if ( !(properties & SL_BT_OTS_OBJECT_PROPERTY_EXECUTE_MASK) ) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
          break;
        }
      }

      memcpy(&oacp_msg_parameters.execute.data,
             data_pointer,
             write_request->value.len - sizeof(sl_bt_ots_oacp_opcode_t));

      break;

    case SL_BT_OTS_OACP_OPCODE_READ:
      if ((server->features.oacp_features
           & (SL_BT_OTS_OACP_FEATURE_READ_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Parameter length does are incorrect
      if (write_request->value.len != OACP_MESSAGE_LENGTH_READ) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }

      oacp_msg_parameters.read.offset = *(uint32_t *)data_pointer;

      data_pointer += sizeof(oacp_msg_parameters.read.length);
      oacp_msg_parameters.read.length = *(uint32_t *)data_pointer;

      if (oacp_msg_parameters.read.length == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
        break;
      }

      // Handle properties
      if (client->current_object_properties != NULL) {
        sl_bt_ots_object_properties_t properties = *client->current_object_properties;
        // Check if it is not writable
        if ( !(properties & SL_BT_OTS_OBJECT_PROPERTY_READ_MASK) ) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
          break;
        }
      }

      // Handle size conditions
      if (client->current_object_size != NULL) {
        // Check for overflow
        if ( (oacp_msg_parameters.calculate_checksum.offset
              + oacp_msg_parameters.calculate_checksum.length)
             > client->current_object_size->current_size) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
          break;
        }
      }

      // Check if it is used in another transfer on the server
      if (transfer_is_in_progress(server, &client->current_object)) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_OBJECT_LOCKED;
      }

      break;

    case SL_BT_OTS_OACP_OPCODE_WRITE:
      // Check if server supports write
      if (!(server->features.oacp_features
            & SL_BT_OTS_OACP_FEATURE_WRITE_OP_CODE_SUPPORTED_MASK)) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Parameter length does are incorrect
      if (write_request->value.len != OACP_MESSAGE_LENGTH_WRITE) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }

      oacp_msg_parameters.write.offset = *(uint32_t *)data_pointer;

      data_pointer += sizeof(oacp_msg_parameters.write.offset);
      oacp_msg_parameters.write.length = *(uint32_t *)data_pointer;

      data_pointer += sizeof(oacp_msg_parameters.write.length);
      oacp_msg_parameters.write.mode = *(sl_bt_ots_oacp_write_mode_t *)data_pointer;

      truncation_requested = oacp_msg_parameters.write.mode
                             & SL_BT_OTS_WRITE_MODE_TRUNCATE_MASK;

      // Check if truncation specified but it is not supported by the server
      if (truncation_requested
          && !(server->features.oacp_features
               & SL_BT_OTS_OACP_FEATURE_TRUNCATION_OF_OBJECTS_SUPPORTED_MASK)) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Check for zero length
      if (oacp_msg_parameters.write.length == 0u) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
        break;
      }

      // Check for enabled RFU bits
      if (oacp_msg_parameters.write.mode & ~SL_BT_OTS_WRITE_MODE_TRUNCATE_MASK) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
        break;
      }

      // Handle size conditions
      if (client->current_object_size != NULL) {
        // Check for overflow if appending is not allowed
        if (!(server->features.oacp_features
              & SL_BT_OTS_OACP_FEATURE_APPEND_ADDITIONAL_DATA_SUPPORTED_MASK)) {
          if ( (oacp_msg_parameters.calculate_checksum.offset
                + oacp_msg_parameters.calculate_checksum.length)
               > client->current_object_size->allocated_size) {
            response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
            break;
          }
        }
        // Check if the offset is outside object data
        if (oacp_msg_parameters.calculate_checksum.offset
            > client->current_object_size->current_size) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_INVALID_PARAMETER;
          break;
        }
        // Assign patching request
        patching_requested = ((oacp_msg_parameters.write.offset > 0)
                              || (oacp_msg_parameters.write.length
                                  < client->current_object_size->allocated_size
                                  && !truncation_requested));
      }

      // Check if patching is requested but not allowed for this server
      if (patching_requested
          && (server->features.oacp_features
              & SL_BT_OTS_OACP_FEATURE_PATCHING_OF_OBJECTS_SUPPORTED_MASK)) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Handle properties
      if (client->current_object_properties != NULL) {
        sl_bt_ots_object_properties_t properties = *client->current_object_properties;
        // Check if it is not writable
        if ( !(properties & SL_BT_OTS_OBJECT_PROPERTY_WRITE_MASK) ) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
          break;
        }
        // Check if truncate is requested but not allowed in the properties
        if (!(properties & SL_BT_OTS_OBJECT_PROPERTY_TRUNCATE_MASK)
            && truncation_requested) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
          break;
        }
        // Check if patching requested but not allowed in the properties
        if (patching_requested
            && !(properties & SL_BT_OTS_OBJECT_PROPERTY_PATCH_MASK)) {
          response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
          break;
        }
      }

      // Check if it is used in another transfer on the server
      if (transfer_is_in_progress(server, &client->current_object)) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_OBJECT_LOCKED;
      }

      break;

    case SL_BT_OTS_OACP_OPCODE_ABORT:
      if ((server->features.oacp_features
           & (SL_BT_OTS_OACP_FEATURE_ABORT_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OACP_RESPONSE_CODE_PROCEDURE_NOT_PERMITTED;
        break;
      }

      // Parameter length does are incorrect
      if (write_request->value.len != OACP_MESSAGE_LENGTH_ABORT) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }

      break;

    default:
      response_code = SL_BT_OTS_OACP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED;
      break;
  }

  // Execute OP code
  if (server->callbacks->on_oacp_event != NULL
      && response_code == SL_BT_OTS_OACP_RESPONSE_CODE_SUCCESS
      && sc == ATT_ERR_SUCCESS) {
    uint32_t  max_pdu_suggested = 0;
    uint32_t  max_sdu_suggested = 0;

    // Default values for max_pdu, max_sdu based on data length
    if (oacp_msg->opcode == SL_BT_OTS_OACP_OPCODE_READ) {
      max_pdu_suggested = oacp_msg_parameters.read.length;
      max_sdu_suggested = oacp_msg_parameters.read.length;
    } else if (oacp_msg->opcode == SL_BT_OTS_OACP_OPCODE_WRITE) {
      max_pdu_suggested = oacp_msg_parameters.write.length;
      max_sdu_suggested = oacp_msg_parameters.write.length;
    }

    // Precheck in-out params
    if (max_pdu_suggested > OTS_L2CAP_MAX_PDU) {
      max_pdu_suggested = OTS_L2CAP_MAX_PDU;
    }
    if (max_sdu_suggested > OTS_L2CAP_MAX_SDU) {
      max_sdu_suggested = OTS_L2CAP_MAX_SDU;
    }

    // check relative value
    if ((max_pdu_suggested + OTS_L2CAP_SDU_PDU_DIFF) > max_sdu_suggested) {
      max_sdu_suggested = max_pdu_suggested + OTS_L2CAP_SDU_PDU_DIFF;
    }

    // Suggest a PDU and SDU
    uint16_t max_pdu = max_pdu_suggested;
    uint16_t max_sdu = max_sdu_suggested;

    response_code = server->callbacks->on_oacp_event(server,
                                                     client->connection_handle,
                                                     &client->current_object,
                                                     oacp_msg->opcode,
                                                     &oacp_msg_parameters,
                                                     &oacp_response_data,
                                                     &max_sdu,
                                                     &max_pdu);
    // Check values
    if (max_pdu > OTS_L2CAP_MAX_PDU || max_pdu == 0) {
      max_pdu = OTS_L2CAP_MAX_PDU;
    }
    if (max_sdu > OTS_L2CAP_MAX_SDU || max_sdu == 0) {
      max_sdu = OTS_L2CAP_MAX_SDU;
    }
    // check relative value
    if ((max_pdu + OTS_L2CAP_SDU_PDU_DIFF) > max_sdu) {
      max_sdu = max_pdu + OTS_L2CAP_SDU_PDU_DIFF;
    }

    // Trigger an L2CAP transfer for Read or Write only if it is requested by
    // higher layers
    if (response_code == SL_BT_OTS_OACP_RESPONSE_CODE_SUCCESS
        && (oacp_msg->opcode == SL_BT_OTS_OACP_OPCODE_READ
            || oacp_msg->opcode == SL_BT_OTS_OACP_OPCODE_WRITE)) {
      client->l2cap_transfer.connection = client->connection_handle;
      if (oacp_msg->opcode == SL_BT_OTS_OACP_OPCODE_WRITE) {
        // Write case
        client->l2cap_transfer.mode = SL_BT_L2CAP_TRANSFER_MODE_RECEIVE;
        client->l2cap_transfer.data_length = oacp_msg_parameters.write.length;
        client->l2cap_transfer.data_offset = oacp_msg_parameters.write.offset;
      } else {
        // Read case
        client->l2cap_transfer.mode = SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT;
        client->l2cap_transfer.data_length = oacp_msg_parameters.read.length;
        client->l2cap_transfer.data_offset = oacp_msg_parameters.read.offset;
      }

      client->l2cap_transfer.max_pdu = max_pdu;
      client->l2cap_transfer.max_sdu = max_sdu;

      client->l2cap_transfer.credit = 1;

      sc = sl_bt_l2cap_transfer_start_data_transfer(&client->l2cap_transfer,
                                                    false);
    }

    if (oacp_msg->opcode == SL_BT_OTS_OACP_OPCODE_EXECUTE) {
      oacp_response_data_len = oacp_response_data.execute.len;

      memcpy(&oacp_response_msg->data,
             oacp_response_data.execute.data,
             oacp_response_data_len);
    } else if (oacp_msg->opcode == SL_BT_OTS_OACP_OPCODE_CALCULATE_CHECKSUM) {
      oacp_response_data_len = sizeof(oacp_response_data.checksum);

      memcpy(&oacp_response_msg->data,
             (uint8_t *)&oacp_response_data.checksum,
             oacp_response_data_len);
    }
  }

  oacp_response_msg->response_opcode = SL_BT_OTS_OACP_OPCODE_RESPONSE_CODE;
  oacp_response_msg->opcode = oacp_msg->opcode;
  oacp_response_msg->response = response_code;

  if (sc == ATT_ERR_SUCCESS) {
    (void)sl_bt_gatt_server_send_user_write_response(write_request->connection,
                                                     write_request->characteristic,
                                                     sc);
    send_indication(client,
                    client->connection_handle,
                    server->gattdb_handles->characteristics.handles.object_action_control_point,
                    sizeof(oacp_response_msg->opcode)
                    + sizeof(oacp_response_msg->opcode)
                    + sizeof(oacp_response_msg->response)
                    + oacp_response_data_len,
                    (uint8_t *)oacp_response_msg);

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
    if (oacp_response_msg->response == SL_BT_OTS_OACP_RESPONSE_CODE_SUCCESS
        && server->capabilities.capability_object_changed) {
      // Send indications to clients
      switch (oacp_response_msg->opcode) {
        case SL_BT_OTS_OACP_OPCODE_CREATE:
          (void)send_object_changed(server,
                                    &client->current_object,
                                    SL_BT_OTS_OBJECT_CHANGE_SOURCE_MASK
                                    | SL_BT_OTS_OBJECT_CHANGE_CREATION_MASK,
                                    client->connection_handle);
          break;
        case SL_BT_OTS_OACP_OPCODE_DELETE:
          (void)send_object_changed(server,
                                    &client->current_object,
                                    SL_BT_OTS_OBJECT_CHANGE_SOURCE_MASK
                                    | SL_BT_OTS_OBJECT_CHANGE_DELETION_MASK,
                                    client->connection_handle);
          break;
        default:
          break;
      }
    }
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
  }

  return sc;
}

static sl_status_t send_indication(sl_bt_ots_server_client_db_entry_t *client_entry,
                                   uint8_t                            connection,
                                   uint16_t                           characteristic,
                                   size_t                             value_len,
                                   const uint8_t                      *value)
{
  sl_status_t sc;
  // Try to indicate the client
  sc = sl_bt_gatt_server_send_indication(connection,
                                         characteristic,
                                         value_len,
                                         value);
  if (sc == SL_STATUS_IN_PROGRESS) {
    // Could not indicate now, save to the queue
    indication_queue_item_t item;
    item.connection = connection;
    item.gattdb_handle = characteristic;
    item.data_length = value_len;
    memcpy(item.data, value, value_len);
    sc = app_queue_add(&client_entry->indication_queue, (uint8_t *)&item);
  }

  return sc;
}

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS
static uint8_t handle_olcp_write(sl_bt_ots_server_t *server,
                                 sl_bt_ots_server_client_db_entry_t *client,
                                 sl_bt_evt_gatt_server_user_write_request_t *write_request)
{
  uint8_t sc = ATT_ERR_SUCCESS;

  sl_bt_ots_olcp_response_code_t response_code = SL_BT_OTS_OLCP_RESPONSE_CODE_SUCCESS;
  uint8_t response_code_len = sizeof(sl_bt_ots_olcp_opcode_t)
                              + sizeof(sl_bt_ots_olcp_opcode_t)
                              + sizeof(sl_bt_ots_olcp_response_code_t);

  sl_bt_ots_olcp_message_t *olcp_msg = ((sl_bt_ots_olcp_message_t *)write_request->value.data);
  sl_bt_ots_olcp_parameters_t olcp_msg_parameters;

  // Buffer to initialize response message
  uint8_t buffer[sizeof(sl_bt_ots_olcp_response_message_t)];
  // Used for BLE indication message
  sl_bt_ots_olcp_response_message_t *olcp_response_msg = (sl_bt_ots_olcp_response_message_t *)buffer;
  // Object is not selected

  if (!client->subscription_status.subscribed.olcp) {
    sc = ATT_ERR_IMPORER_CCCD;
    return sc;
  }

  if (!check_object_valid(&client->current_object)
      && (olcp_msg->opcode == SL_BT_OTS_OLCP_OPCODE_PREVIOUS
          || olcp_msg->opcode == SL_BT_OTS_OLCP_OPCODE_NEXT)) {
    sc = ATT_ERR_OBJECT_NOT_SELECTED;
    return sc;
  }

  uint8_t *data_pointer = (uint8_t *)&olcp_msg->opcode;

  switch (olcp_msg->opcode) {
    case SL_BT_OTS_OLCP_OPCODE_FIRST:
    case SL_BT_OTS_OLCP_OPCODE_LAST:
    case SL_BT_OTS_OLCP_OPCODE_PREVIOUS:
    case SL_BT_OTS_OLCP_OPCODE_NEXT:
      if (write_request->value.len != sizeof(sl_bt_ots_olcp_opcode_t)) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
      }
      break;

    case SL_BT_OTS_OLCP_OPCODE_GO_TO:
      if (write_request->value.len != OLCP_MESSAGE_LENGTH_GO_TO) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }
      if ((server->features.olcp_features
           & (SL_BT_OTS_OLCP_FEATURE_GO_TO_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OLCP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED;
        break;
      }

      data_pointer += sizeof(sl_bt_ots_olcp_opcode_t);
      memcpy(&olcp_msg_parameters.go_to.id,
             data_pointer,
             sizeof(olcp_msg_parameters.go_to.id));
      break;

    case SL_BT_OTS_OLCP_OPCODE_ORDER:
      if (write_request->value.len != OLCP_MESSAGE_LENGTH_ORDER) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }
      if ((server->features.olcp_features
           & (SL_BT_OTS_OLCP_FEATURE_ORDER_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OLCP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED;
        break;
      }
      data_pointer += sizeof(sl_bt_ots_olcp_opcode_t);

      switch (*(sl_bt_ots_list_sort_order_t *)data_pointer) {
        case SL_BT_OTS_LIST_SORT_ORDER_NAME_ASC:
        case SL_BT_OTS_LIST_SORT_ORDER_TYPE_ASC:
        case SL_BT_OTS_LIST_SORT_ORDER_CURRENT_SIZE_ASC:
        case SL_BT_OTS_LIST_SORT_ORDER_FIRST_CREATED_ASC:
        case SL_BT_OTS_LIST_SORT_ORDER_LAST_MODIFIED_ASC:
        case SL_BT_OTS_LIST_SORT_ORDER_NAME_DESC:
        case SL_BT_OTS_LIST_SORT_ORDER_TYPE_DESC:
        case SL_BT_OTS_LIST_SORT_ORDER_CURRENT_SIZE_DESC:
        case SL_BT_OTS_LIST_SORT_ORDER_FIRST_CREATED_DESC:
        case SL_BT_OTS_LIST_SORT_ORDER_LAST_MODIFIED_DESC:
          break;
        default:
          response_code = SL_BT_OTS_OLCP_RESPONSE_CODE_INVALID_PARAMETER;
          break;
      }

      olcp_msg_parameters.order.list_sort_order = *(sl_bt_ots_list_sort_order_t *)data_pointer;
      break;

    case SL_BT_OTS_OLCP_OPCODE_REQUEST_NUMBER_OF_OBJECTS:
      if (write_request->value.len != sizeof(sl_bt_ots_olcp_opcode_t)) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }
      if ((server->features.olcp_features
           & (SL_BT_OTS_OLCP_FEATURE_REQUEST_NUMBER_OF_OBJECTS_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OLCP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED;
        break;
      }
      response_code_len += sizeof(olcp_response_msg->number_of_objects);
      break;

    case SL_BT_OTS_OLCP_OPCODE_CLEAR_MARKING:
      if (write_request->value.len != sizeof(sl_bt_ots_olcp_opcode_t)) {
        sc = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        break;
      }
      if ((server->features.olcp_features
           & (SL_BT_OTS_OLCP_FEATURE_CLEAR_MARKING_OP_CODE_SUPPORTED_MASK)) == 0) {
        response_code = SL_BT_OTS_OLCP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED;
        break;
      }
      break;

    default:
      response_code = SL_BT_OTS_OLCP_RESPONSE_CODE_OP_CODE_NOT_SUPPORTED;
  }

  uint32_t number_of_objects = 0;
  // Execute OP code
  if (server->callbacks->on_olcp_event != NULL
      && response_code == SL_BT_OTS_OLCP_RESPONSE_CODE_SUCCESS
      && sc == ATT_ERR_SUCCESS) {
    response_code = server->callbacks->on_olcp_event(server,
                                                     client->connection_handle,
                                                     &client->current_object,
                                                     olcp_msg->opcode,
                                                     &olcp_msg_parameters,
                                                     &number_of_objects);
    olcp_response_msg->number_of_objects = number_of_objects;
  }

  olcp_response_msg->response_opcode = SL_BT_OTS_OLCP_OPCODE_RESPONSE_CODE;
  olcp_response_msg->opcode = olcp_msg->opcode;
  olcp_response_msg->response = response_code;

  if (sc == ATT_ERR_SUCCESS) {
    (void)sl_bt_gatt_server_send_user_write_response(write_request->connection,
                                                     write_request->characteristic,
                                                     sc);
    send_indication(client,
                    client->connection_handle,
                    server->gattdb_handles->characteristics.handles.object_list_control_point,
                    response_code_len,
                    (uint8_t *)olcp_response_msg);
  }

  return sc;
}
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT
static uint8_t handle_filter_write(sl_bt_ots_server_t *server,
                                   sl_bt_ots_server_client_db_entry_t *client,
                                   sl_bt_evt_gatt_server_user_write_request_t *write_request)
{
  uint8_t att_error = ATT_ERR_SUCCESS;
  sl_bt_ots_object_list_filter_content_t content;
  bool filter_is_valid = false;

  if (write_request->value.len >= 1) {
    // Get filter type
    content.filter_type = *((sl_bt_ots_object_list_filter_type_t *)write_request->value.data);
    uint8_t parameter_length = write_request->value.len - 1;

    switch (content.filter_type) {
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NO_FILTER:
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MARKED_OBJECTS:
        if (parameter_length == 0) {
          filter_is_valid = true;
        } else {
          att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        }
        break;
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_CONTAINS:
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_STARTS_WITH:
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_ENDS_WITH:
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_NAME_IS_EXACTLY:
        content.parameters.name.length = parameter_length;
        content.parameters.name.name   = (char *)&(write_request->value.data[1]);
        filter_is_valid = true;
        break;
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CREATED_BETWEEN:
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_MODIFIED_BETWEEN:
#endif
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT || SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
        if (parameter_length == sizeof(sl_bt_ots_object_list_filter_time_parameters_t)) {
          memcpy(content.parameters.time.from.data, &(write_request->value.data[1]), parameter_length);
          if (content.parameters.time.from.time.year > content.parameters.time.to.time.year) {
            att_error = ATT_ERR_WRITE_REQUEST_REJECTED;
            break;
          } else if (content.parameters.time.from.time.year == content.parameters.time.to.time.year) {
            for (uint8_t i = sizeof(content.parameters.time.from.time.year); i < sizeof(sl_bt_ots_time_t); i++) {
              if (content.parameters.time.from.data[i] > content.parameters.time.to.data[i]) {
                att_error = ATT_ERR_WRITE_REQUEST_REJECTED;
                break;
              } else if (content.parameters.time.from.data[i] < content.parameters.time.to.data[i]) {
                break;
              }
            }
          }
          if (att_error == ATT_ERR_SUCCESS) {
            filter_is_valid = true;
          }
        } else {
          att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        }
        break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT || SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_OBJECT_TYPE:
        if (parameter_length == SL_BT_OTS_UUID_SIZE_16
            || parameter_length == SL_BT_OTS_UUID_SIZE_128) {
          content.parameters.type.uuid_is_sig = (parameter_length == SL_BT_OTS_UUID_SIZE_16);
          content.parameters.type.uuid_data = &write_request->value.data[1];
          filter_is_valid = true;
        } else {
          att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        }
        break;
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_CURRENT_SIZE_BETWEEN:
      case SL_BT_OTS_OBJECT_LIST_FILTER_TYPE_ALLOCATED_SIZE_BETWEEN:
        if (parameter_length == sizeof(sl_bt_ots_object_list_filter_size_parameters_t)) {
          memcpy(&content.parameters.size, &write_request->value.data[1], parameter_length);
          if (content.parameters.size.min > content.parameters.size.max) {
            att_error = ATT_ERR_WRITE_REQUEST_REJECTED;
          } else {
            filter_is_valid = true;
          }
        } else {
          att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
        }
        break;
      default:
        att_error = ATT_ERR_WRITE_REQUEST_REJECTED;
        break;
    }
  } else {
    att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
  }

  // Call back on success
  if (att_error == ATT_ERR_SUCCESS) {
    if (server->callbacks->on_object_list_filter_set != NULL
        && filter_is_valid) {
      sl_bt_ots_object_metadata_write_response_code_t result =
        server->callbacks->on_object_list_filter_set(server,
                                                     client->connection_handle,
                                                     &content);
      // Use result as ATT error code
      att_error = (uint8_t)result;
    }
  }

  if (att_error == ATT_ERR_SUCCESS) {
    (void)sl_bt_gatt_server_send_user_write_response(write_request->connection,
                                                     write_request->characteristic,
                                                     att_error);
  }
  return att_error;
}
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT

static uint8_t handle_metadata_write(sl_bt_ots_server_t *server,
                                     sl_bt_ots_server_client_db_entry_t *client,
                                     sl_bt_ots_characteristic_uuid_index characteristic_index,
                                     sl_bt_evt_gatt_server_user_write_request_t *write_request)
{
  uint8_t att_error = ATT_ERR_SUCCESS;
  sl_bt_ots_object_metadata_write_parameters_t data;
  sl_bt_ots_object_metadata_write_parameters_t *data_ptr;
  // Check and create callback arguments
  switch (characteristic_index) {
    // Handle metadata write
    case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME:
      data.object_name.length = write_request->value.len;
      data.object_name.name = (char *)write_request->value.data;
      data_ptr = &data;
      break;
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
    case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED:
      if (write_request->value.len == sizeof(sl_bt_ots_object_first_created_t)) {
        data_ptr = (sl_bt_ots_object_metadata_write_parameters_t *)write_request->value.data;
      } else {
        att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
      }
      break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
    case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED:
      if (write_request->value.len == sizeof(sl_bt_ots_object_last_modified_t)) {
        data_ptr = (sl_bt_ots_object_metadata_write_parameters_t *)write_request->value.data;
      } else {
        att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
      }
      break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
    case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES:
      if (write_request->value.len == sizeof(sl_bt_ots_object_properties_t)) {
        data_ptr = (sl_bt_ots_object_metadata_write_parameters_t *)write_request->value.data;
      } else {
        att_error = ATT_ERR_INVALID_ATTRIBUTE_LEN;
      }
      break;
    default:
      att_error = ATT_ERR_ATTRIBUTE_NOT_FOUND;
      break;
  }

  // Call back on success
  if (att_error == ATT_ERR_SUCCESS) {
    // Check if an object is selected
    if (!check_object_valid(&client->current_object)) {
      att_error = ATT_ERR_OBJECT_NOT_SELECTED;
    } else {
      if (server->callbacks->on_object_metadata_write != NULL) {
        sl_bt_ots_object_metadata_write_response_code_t result =
          server->callbacks->on_object_metadata_write(server,
                                                      client->connection_handle,
                                                      &server->client_db->current_object,
                                                      (sl_bt_ots_object_metadata_write_event_type_t)characteristic_index,
                                                      data_ptr);
        // Use result as ATT error code
        att_error = (uint8_t)result;
      }
    }
  }
  if (att_error == ATT_ERR_SUCCESS) {
    (void)sl_bt_gatt_server_send_user_write_response(write_request->connection,
                                                     write_request->characteristic,
                                                     att_error);
  }
  return att_error;
}

static void handle_gatt_write(sl_bt_evt_gatt_server_user_write_request_t *write_request)
{
  sl_bt_ots_server_t *server = NULL;
  sl_bt_ots_server_client_db_entry_t *client = NULL;
  sl_bt_ots_characteristic_uuid_index characteristic_index = SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID;
  uint8_t att_error = ATT_ERR_SUCCESS;

  // Find characteristic handle
  characteristic_index = find_characteristic_index(write_request->characteristic,
                                                   &server);
  // Check server hand characteristic index
  if (server != NULL
      && characteristic_index != SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID) {
    // Identify client
    client = find_client(server, write_request->connection);

    // handle writes if client was found
    if (client != NULL) {
      // Check and create callback arguments
      switch (characteristic_index) {
        // Handle OACP
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OACP:
          att_error = handle_oacp_write(server,
                                        client,
                                        write_request);
          break;
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS
        // Handle OLCP
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OLCP:
          if (server->capabilities.capability_multiple_objects) {
            att_error = handle_olcp_write(server,
                                          client,
                                          write_request);
          } else {
            att_error = ATT_ERR_NOT_SUPPORTED;
          }
          break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS
        // Handle metadata write
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_NAME:
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_FIRST_CREATED:
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_FIRST_CREATED_SUPPORT
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LAST_MODIFIED:
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_TIME_LAST_MODIFIED_SUPPORT
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_PROPERTIES:
          att_error = handle_metadata_write(server,
                                            client,
                                            characteristic_index,
                                            write_request);

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
          if (att_error == SL_BT_OTS_OBJECT_METADATA_WRITE_RESPONSE_CODE_SUCCESS
              && server->capabilities.capability_object_changed) {
            // Send indications to clients
            (void)send_object_changed(server,
                                      &client->current_object,
                                      SL_BT_OTS_OBJECT_CHANGE_SOURCE_MASK
                                      | SL_BT_OTS_OBJECT_CHANGE_METADATA_MASK,
                                      client->connection_handle);
          }
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
          break;
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT
        // Handle Object List filter write
        case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_LIST_FILTER:
          if (server->capabilities.capability_object_list_filter) {
            att_error = handle_filter_write(server,
                                            client,
                                            write_request);
          } else {
            att_error = ATT_ERR_NOT_SUPPORTED;
          }
          break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_LIST_FILTER_SUPPORT
        default:
          att_error = ATT_ERR_ATTRIBUTE_NOT_FOUND;
          break;
      }
    } else {
      // Client not found, respond with Concurrency Limit Exceeded.
      att_error = ATT_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }
  }
  if (att_error != ATT_ERR_SUCCESS) {
    // Send response to write
    (void)sl_bt_gatt_server_send_user_write_response(write_request->connection,
                                                     write_request->characteristic,
                                                     att_error);
  }
}

static void handle_cccd(sl_bt_evt_gatt_server_characteristic_status_t *characteristic_status)
{
  sl_bt_ots_server_t *server = NULL;
  if (sl_bt_gatt_server_client_config == characteristic_status->status_flags ) {
    sl_bt_ots_characteristic_uuid_index characteristic_index
      = find_characteristic_index(characteristic_status->characteristic,
                                  &server);
    bool value = (sl_bt_gatt_indication & characteristic_status->client_config_flags) > 0;
    if (server != NULL
        && characteristic_index != SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_INVALID) {
      sl_bt_ots_server_client_db_entry_t *client = find_client(server,
                                                               characteristic_status->connection);
      bool changed = false;
      if (client != NULL) {
        switch (characteristic_index) {
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OACP:
            client->subscription_status.subscribed.oacp = value;
            changed = true;
            break;
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OLCP:
            if (server->capabilities.capability_multiple_objects) {
              client->subscription_status.subscribed.olcp = value;
              changed = true;
            }
            break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_MULTIPLE_OBJECTS
#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
          case SL_BT_OTS_CHARACTERISTIC_UUID_INDEX_OBJECT_CHANGED:
            if (server->capabilities.capability_object_changed) {
              client->subscription_status.subscribed.object_changed = value;
              changed = true;
            }
            break;
#endif // SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
          default:
            break;
        }
        if (changed) {
          CALL_SAFE(server,
                    on_client_subscription_change,
                    server,
                    characteristic_status->connection,
                    client->subscription_status);
        }
      }
    }
  }
}

#if SL_BT_OTS_SERVER_CONFIG_GLOBAL_OBJECT_CHANGED_SUPPORT
static sl_status_t send_object_changed(sl_bt_ots_server_handle_t        server,
                                       sl_bt_ots_object_id_t            *object,
                                       sl_bt_ots_object_changed_flags_t flags,
                                       uint16_t                         client)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t i;
  // Check input
  CHECK_NULL(server);
  CHECK_NULL(object);

  sl_bt_ots_object_changed_content_t indication_content;

  bool client_initiated = ((flags & SL_BT_OTS_OBJECT_CHANGE_SOURCE_MASK) > 0);

  for (i = 0; i < server->concurrency; i++) {
    if ( (!client_initiated)
         || (client != server->client_db[i].connection_handle) ) {
      if (server->client_db[i].subscription_status.subscribed.object_changed) {
        // Arrange data to send
        indication_content.flags = flags;
        memcpy(indication_content.object.data,
               object->data,
               SL_BT_OTS_OBJECT_ID_SIZE);
        sc |= send_indication(&server->client_db[i],
                              server->client_db[i].connection_handle,
                              server->gattdb_handles->characteristics.handles.object_changed,
                              sizeof(indication_content),
                              (uint8_t *)&indication_content);
      }
    }
  }

  return sc;
}
#endif

static void clear_queue(uint8_t connection)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  for (uint8_t s = 0; s < SL_BT_OTS_SERVER_COUNT; s++) {
    const sl_bt_ots_server_t *server = sl_bt_ots_server_instances[s];
    for (uint8_t index = 0; index < server->concurrency; index++) {
      if (connection == server->client_db[index].connection_handle) {
        // Reinit queue
        app_queue_t *queue = &server->client_db[index].indication_queue;
        app_queue_init(queue, queue->size, queue->item_size, queue->data);
      }
    }
  }
  CORE_EXIT_CRITICAL();
}

static bool transfer_is_in_progress(sl_bt_ots_server_handle_t        server,
                                    sl_bt_ots_object_id_t            *object)
{
  sl_status_t sc = SL_STATUS_OK;

  for (uint8_t index = 0; index < server->concurrency; index++) {
    // Check if it is the same object as specified
    if (memcmp(server->client_db[index].current_object.data, object->data, sizeof(object->data)) == 0) {
      // Check if the transfer is in progress
      sc = sl_bt_l2cap_transfer_check_progress(&server->client_db[index].l2cap_transfer);
      if (sc == SL_STATUS_IN_PROGRESS) {
        return (sc == SL_STATUS_IN_PROGRESS);
      }
    }
  }
  return (sc == SL_STATUS_IN_PROGRESS);
}
