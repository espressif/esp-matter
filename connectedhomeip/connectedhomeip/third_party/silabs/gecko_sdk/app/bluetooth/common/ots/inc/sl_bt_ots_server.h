/***************************************************************************//**
 * @file
 * @brief Object Transfer Service Object Server Core API
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

#ifndef SL_BT_OTS_SERVER_H
#define SL_BT_OTS_SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include "app_queue.h"
#include "sl_bt_ots_datatypes.h"
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
// Type definitions

/// OTS Object Server Capabilities
typedef struct {
  uint8_t capability_multiple_objects   : 1; ///< Allow multiple objects.
  uint8_t capability_object_list_filter : 1; ///< Support Object List Filter.
  uint8_t capability_dlo                : 1; ///< Support Directory Listing Object.
  uint8_t capability_time               : 1; ///< Support Time
  uint8_t capability_first_created      : 1; ///< Support Object Frist Created.
  uint8_t capability_last_modified      : 1; ///< Support Object Last Modified.
  uint8_t capability_object_changed     : 1; ///< Support Object Changed.
} sl_bt_ots_server_capabilities_t;

/// OTS Object Server handle
typedef struct sl_bt_ots_server *sl_bt_ots_server_handle_t;

/***************************************************************************//**
 * OTS Object Server callback function prototype for client connection or
 * disconnection
 * @param[in] server Server handle.
 * @param[in] client Connection handle for the client.
 ******************************************************************************/
typedef void (*sl_bt_ots_server_connection_callback_t)(sl_bt_ots_server_handle_t server,
                                                       uint16_t                  client);

/***************************************************************************//**
 * OTS callback function prototype for indication subscription status
 * @param[in] server Server handle.
 * @param[in] client Connection handle for the client.
 * @param[in] status Subscription status.
 ******************************************************************************/
typedef void (*sl_bt_ots_subscription_callback_t)(sl_bt_ots_server_handle_t       server,
                                                  uint16_t                        client,
                                                  sl_bt_ots_subscription_status_t status);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle OLCP events
 * @param[in]  server            Server handle.
 * @param[in]  client            Connection handle for the client.
 * @param[in]  object            Object ID.
 * @param[in]  event             OLCP event.
 * @param[in]  parameters        OLCP event parameters.
 * @param[out] number_of_objects Number of objects in case of that opcode/event.
 * @return                       OLCP response code
 ******************************************************************************/
typedef sl_bt_ots_olcp_response_code_t (*sl_bt_ots_server_olcp_callback_t)(sl_bt_ots_server_handle_t   server,
                                                                           uint16_t                    client,
                                                                           sl_bt_ots_object_id_t       *object,
                                                                           sl_bt_ots_olcp_event_t      event,
                                                                           sl_bt_ots_olcp_parameters_t *parameters,
                                                                           uint32_t                    *number_of_objects);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle OACP events
 * @param[in]  server        Server handle.
 * @param[in]  client        Connection handle for the client.
 * @param[in]  object        Object ID.
 * @param[in]  event         OACP event.
 * @param[in]  parameters    OACP event parameters.
 * @param[out] response_data The response data
 * @param[in,out] max_sdu @parblock
 *   The Maximum Service Data Unit size the local channel endpoint can accept
 *
 *   Range: 23 to 65533.
 *
 *   Caller passes a suggested value of SDU as an input.
 *
 *   @endparblock
 * @param[in,out] max_pdu @parblock
 *   The maximum PDU payload size the local channel endpoint can accept
 *
 *   Range:23 to 252.
 *
 *   PDU and SDU are optimal when SDU = n * PDU + 2.
 *   Caller passes a suggested value of PDU as an input.
 *
 *   @endparblock
 * @return                   OACP response code
 ******************************************************************************/
typedef sl_bt_ots_oacp_response_code_t (*sl_bt_ots_server_oacp_callback_t)(sl_bt_ots_server_handle_t      server,
                                                                           uint16_t                       client,
                                                                           sl_bt_ots_object_id_t          *object,
                                                                           sl_bt_ots_oacp_event_t         event,
                                                                           sl_bt_ots_oacp_parameters_t    *parameters,
                                                                           sl_bt_ots_oacp_response_data_t *response_data,
                                                                           uint16_t                       *max_sdu,
                                                                           uint16_t                       *max_pdu);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle data reception
 * @param[in] server         Server handle.
 * @param[in] client         Connection handle for the client.
 * @param[in] object         Object ID.
 * @param[in] current_offset Current offset from the beginning of the object.
 * @param[in] data           Pointer to the received data.
 * @param[in] size           Size of the received data.
 * @return                   Credits to give to the transmitter.
 ******************************************************************************/
typedef sl_bt_ots_l2cap_credit_t (*sl_bt_ots_server_data_callback_t)(sl_bt_ots_server_handle_t server,
                                                                     uint16_t                  client,
                                                                     sl_bt_ots_object_id_t     *object,
                                                                     int32_t                   current_offset,
                                                                     void                      *data,
                                                                     int32_t                   size);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle transmission status
 * @param[in] server Server handle.
 * @param[in] client Connection handle for the client.
 * @param[in] object Object ID.
 * @param[in] result Size of the data.
 ******************************************************************************/
typedef void (*sl_bt_ots_server_data_transfer_finished_t)(sl_bt_ots_server_handle_t   server,
                                                          uint16_t                    client,
                                                          sl_bt_ots_object_id_t       *object,
                                                          sl_bt_ots_transfer_result_t result);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle data transmission
 * @param[in] server     Client handle.
 * @param[in] object     Object ID reference.
 * @param[in]  offset    Current offset of requested data
 * @param[in]  size      Maximum size of requested data
 * @param[out] data      Pointer of pointer to data
 * @param[out] data_size Size of the provided data
 ******************************************************************************/
typedef void (*sl_bt_ots_server_data_transmit_callback_t)(sl_bt_ots_server_handle_t server,
                                                          uint16_t                  client,
                                                          sl_bt_ots_object_id_t     *object,
                                                          uint32_t                  current_offset,
                                                          uint32_t                  size,
                                                          uint8_t                   **data,
                                                          uint32_t                  *data_size);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle metadata writes
 * @param[in] server     Server handle.
 * @param[in] client     Connection handle for the client.
 * @param[in] object     Object ID.
 * @param[in] event      Metadata write event.
 * @param[in] parameters Metadata event parameters.
 * @return               Response code
 ******************************************************************************/
typedef sl_bt_ots_object_metadata_write_response_code_t (*sl_bt_ots_server_object_metadata_write_callback_t)(sl_bt_ots_server_handle_t                    server,
                                                                                                             uint16_t                                     client,
                                                                                                             sl_bt_ots_object_id_t                        *object,
                                                                                                             sl_bt_ots_object_metadata_write_event_type_t event,
                                                                                                             sl_bt_ots_object_metadata_write_parameters_t *parameters);

/***************************************************************************//**
 * OTS Object Server callback function prototype to handle metadata writes
 * @param[in]  server     Server handle.
 * @param[in]  client     Connection handle for the client.
 * @param[in]  object     Object ID.
 * @param[in]  event      Metadata read event.
 * @param[out] parameters Metadata event parameters to be filled.
 * @return                Response code
 ******************************************************************************/
typedef sl_bt_ots_object_metadata_read_response_code_t (*sl_bt_ots_server_object_metadata_read_callback_t)(sl_bt_ots_server_handle_t                   server,
                                                                                                           uint16_t                                    client,
                                                                                                           sl_bt_ots_object_id_t                       *object,
                                                                                                           sl_bt_ots_object_metadata_read_event_type_t event,
                                                                                                           sl_bt_ots_object_metadata_read_parameters_t *parameters);

/***************************************************************************//**
 * OTS Object Server callback function prototype to set Object List Filter
 * @param[in]  server     Server handle.
 * @param[in]  client     Connection handle for the client.
 * @param[in]  filter     Pointer to the filter
 * @return                Response code
 ******************************************************************************/
typedef sl_bt_ots_object_metadata_write_response_code_t (*sl_bt_ots_server_object_list_filter_wtite_callback_t)(sl_bt_ots_server_handle_t              server,
                                                                                                                uint16_t                               client,
                                                                                                                sl_bt_ots_object_list_filter_content_t *filter);

/***************************************************************************//**
 * OTS Object Server callback function prototype to get Object List Filter
 * @param[in]  server     Server handle.
 * @param[in]  client     Connection handle for the client.
 * @param[out] filter     Pointer to the filter to be filled.
 * @return                Response code
 ******************************************************************************/
typedef sl_bt_ots_object_metadata_read_response_code_t (*sl_bt_ots_server_object_list_filter_read_callback_t)(sl_bt_ots_server_handle_t              server,
                                                                                                              uint16_t                               client,
                                                                                                              sl_bt_ots_object_list_filter_content_t *filter);
/***************************************************************************//**
 * OTS Object Server capability initialization function type
 ******************************************************************************/
typedef void (*sli_bt_ots_server_capability_init_t)(void);

/// OTS Object Server Callbacks
typedef struct {
  sl_bt_ots_server_connection_callback_t               on_client_connect;             ///< Callback to handle connection of a client
  sl_bt_ots_server_connection_callback_t               on_client_disconnect;          ///< Callback to handle disconnection of a client
  sl_bt_ots_subscription_callback_t                    on_client_subscription_change; ///< Callback to handle client subscription status changes
  sl_bt_ots_server_object_metadata_write_callback_t    on_object_metadata_write;      ///< Callback to handle metadata writes
  sl_bt_ots_server_object_metadata_read_callback_t     on_object_metadata_read;       ///< Callback to handle metadata read operations
  sl_bt_ots_server_object_list_filter_wtite_callback_t on_object_list_filter_set;     ///< Callback to handle Object List Filter write operations
  sl_bt_ots_server_object_list_filter_read_callback_t  on_object_list_filter_get;     ///< Callback to handle Object List Filter read operations
  sl_bt_ots_server_olcp_callback_t                     on_olcp_event;                 ///< Callback to handle OLCP List management events
  sl_bt_ots_server_oacp_callback_t                     on_oacp_event;                 ///< Callback to handle OACP Data management events
  sl_bt_ots_server_data_callback_t                     on_data_received;              ///< Callback to handle incoming data
  sl_bt_ots_server_data_transfer_finished_t            on_data_transfer_finished;     ///< Callback to handle the end of a data transfer
  sl_bt_ots_server_data_transmit_callback_t            on_data_transmit;              ///< Callback to handle outgoing data.
} sl_bt_ots_server_callbacks_t;

/// OTS Object Server Client database item
typedef struct {
  uint16_t                        connection_handle;          ///< The connection handle that identifies the client.
  sl_bt_ots_object_id_t           current_object;             ///< Selected Current Object ID
  sl_bt_ots_object_properties_t   *current_object_properties; ///< Pointer to Current Object Properties (or NULL if not specified)
  sl_bt_ots_object_size_t         *current_object_size;       ///< Pointer to Current Object Size (or NULL if not specified)
  sl_bt_ots_subscription_status_t subscription_status;        ///< Subscription status for indication to OACP, OLCP and Object Change characteristics
  sl_bt_l2cap_transfer_transfer_t l2cap_transfer;             ///< Current object transfer channel (L2CAP channel)
  uint16_t                        l2cap_transfer_sdu;         ///< SDU of the current L2CAP transfer
  uint16_t                        l2cap_transfer_pdu;         ///< PDU of the current L2CAP transfer
  app_queue_t                     indication_queue;           ///< Queue for indications (responses and object changed events)
} sl_bt_ots_server_client_db_entry_t;

/// OTS Object Server instance type
typedef struct sl_bt_ots_server {
  sl_bt_ots_gattdb_handles_t          *gattdb_handles; ///< GATT database handles for the Object Server Instance
  sl_bt_ots_server_callbacks_t        *callbacks;      ///< Callbacks for the higher layers
  uint8_t                             concurrency;     ///< Maximum number of concurrent connections
  sl_bt_ots_features_t                features;        ///< Features of the server
  sl_bt_ots_server_client_db_entry_t  *client_db;      ///< Internal database of clients
  sl_bt_ots_server_capabilities_t     capabilities;    ///< Server capabilites
  sli_bt_ots_server_capability_init_t capability_init; ///< Initialization function for capabilities
} sl_bt_ots_server_t;

// -----------------------------------------------------------------------------
// Public functions

/***************************************************************************//**
 * Initialize the OTS Object Server.
 * @param[in] server     Server handle.
 * @param[in] handles    GATT database handles.
 * @param[in] callbacks  Callbacks to handle requests.
 * @return               Response code
 ******************************************************************************/
sl_status_t sl_bt_ots_server_init(sl_bt_ots_server_handle_t      server,
                                  sl_bt_ots_gattdb_handles_t     *handles,
                                  sl_bt_ots_server_callbacks_t   *callbacks);

/***************************************************************************//**
 * Set the current object on the server for the specified client
 * @param[in] server     Server handle.
 * @param[in] client     Connection handle for the client.
 * @param[in] object     Pointer to the Object ID.  If NULL, Current Object will
 *                       set to Invalid Object.
 * @param[in] properties Pointer to the Object Properties. If NULL, all OACP
 *                       actions will be passed using the callbacks. If not
 *                       NULL, the server will respond with OACP response codes
 *                       automatically and pass only the OACP operations those
 *                       are permitted by the given object properties.
 * @param[in] size       Pointer to the Object Size. If NULL, all OACP
 *                       actions will be passed using the callbacks. If not
 *                       NULL, the server will respond with OACP response codes
 *                       automatically and pass only the OACP operations those
 *                       are permitted by the given object size and server
 *                       capability configuration.
 * @return               Response code
 ******************************************************************************/
sl_status_t sl_bt_ots_server_set_current_object(sl_bt_ots_server_handle_t     server,
                                                uint16_t                      client,
                                                sl_bt_ots_object_id_t         *object,
                                                sl_bt_ots_object_properties_t *properties,
                                                sl_bt_ots_object_size_t       *size);

/***************************************************************************//**
 * Get the current object on the server for the specified client
 * @param[in] server     Server handle.
 * @param[in] client     Connection handle for the client.
 * @param[out] object    Pointer to the Object ID.
 * @return               Response code
 ******************************************************************************/
sl_status_t sl_bt_ots_server_get_current_object(sl_bt_ots_server_handle_t server,
                                                uint16_t                  client,
                                                sl_bt_ots_object_id_t     *object);

/***************************************************************************//**
 * Abort transmission on the Object Server instance for a specified client.
 * @param[in] server     Server handle.
 * @param[in] client     Connection handle for the client.
 * @return               Response code
 ******************************************************************************/
sl_status_t sl_bt_ots_server_abort(sl_bt_ots_server_handle_t server,
                                   uint16_t                  client);

/***************************************************************************//**
 * Indicate that an object has been changed on the server.
 * @param[in] server     Server handle.
 * @param[in] object     Pointer to the Object ID.
 * @param[in] flags      The following masks can be used to indicate the nature
 *                       of change:
 *                       - SL_BT_OTS_OBJECT_CHANGE_CONTENTS_MASK : object contents changed
 *                       - SL_BT_OTS_OBJECT_CHANGE_METADATA_MASK : metadata changed
 *                       - SL_BT_OTS_OBJECT_CHANGE_CREATION_MASK : object creation
 *                       - SL_BT_OTS_OBJECT_CHANGE_DELETION_MASK : object deletion
 * @return               Response code
 ******************************************************************************/
sl_status_t sl_bt_ots_server_object_changed(sl_bt_ots_server_handle_t        server,
                                            sl_bt_ots_object_id_t            *object,
                                            sl_bt_ots_object_changed_flags_t flags);

/***************************************************************************//**
 * Increase the credit for the L2CAP transfer that is in progress
 * @param[in] server     Server handle.
 * @param[in] client     Connection handle for the client.
 * @param[in] credit     Number of credit to give (in packets).
 * @return               Response code
 ******************************************************************************/
sl_status_t sl_bt_ots_server_increase_credit(sl_bt_ots_server_handle_t server,
                                             uint16_t                  client,
                                             uint16_t                  credit);

/***************************************************************************//**
 * Internal Bluetooth event handler.
 * @param[in] evt  Bluetooth event.
 ******************************************************************************/
void sli_bt_ots_server_on_bt_event(sl_bt_msg_t *evt);

/***************************************************************************//**
 * Internal step for queue handling.
 ******************************************************************************/
void sli_bt_ots_server_step(void);

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT

/***************************************************************************//**
 * Internal routine for power manager handler
 * @return SL_POWER_MANAGER_WAKEUP if the test has been started
 ******************************************************************************/
sl_power_manager_on_isr_exit_t sli_bt_ots_server_sleep_on_isr_exit(void);

/***************************************************************************//**
 * Internal check if it is ok to sleep now
 * @return false if the test has been started
 ******************************************************************************/
bool sli_bt_ots_server_is_ok_to_sleep(void);

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

#ifdef __cplusplus
};
#endif

#endif // SL_BT_OTS_SERVER_H
