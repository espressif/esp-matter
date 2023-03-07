/***************************************************************************//**
 * @file
 * @brief L2CAP transfer interface
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

#include "sl_bt_l2cap_transfer.h"
#include "em_core.h"
#include "sl_slist.h"

// -----------------------------------------------------------------------------
// Definitions
#define INVALID_CONNECTION_HANDLE 0
#define DEFAULT_CREDIT_VALUE      1

// -----------------------------------------------------------------------------
// Local variables
// State list. They are used to store transfers in a linked list - based on
// their current state

// Store channel open request
static sl_slist_node_t *request_transfer_list;
// Store pending open request
static sl_slist_node_t *request_pending_transfer_list;
// Store all active data transfers
static sl_slist_node_t *active_list;

// Store request responses
static sl_slist_node_t *request_response_transfer_list;
// Store pending request responses
static sl_slist_node_t *request_response_buffer_transfer_list;
// Store transfer which are receiving data
static sl_slist_node_t *receive_active_transfer_list;

// -----------------------------------------------------------------------------
//  Forward declaration of private functions

/******************************************************************************
 * Open transfer channel on request.
 *****************************************************************************/
static sl_status_t open_channel(void);

/******************************************************************************
 * Check if the transfer instance is valid (all necessary parameters are set)
 *
 * @param[in] transfer Pointer to the transfer object
 *
 * @retval SL_STATUS_OK Success.
 * @retval SL_STATUS_INVALID_HANDLE Connection handle is invalid.
 * @retval SL_STATUS_NULL_POINTER One of the callback functions is not set for
 *                                the transfer object, or the transfer object is
 *                                not set
 * @retval SL_STATUS_INVALID_PARAMETER One of the following parameters is not
 *                                     set in the transfer object: max_pdu,
 *                                     max_sdu, data, data_length, mode
 *
 *****************************************************************************/
static sl_status_t check_transfer_init_conditions(sl_bt_l2cap_transfer_transfer_t *transfer);

/******************************************************************************
 * Set up a channel open request.
 *
 * @param[in] data Pointer to the le_channel_open_response event
 *
 * @return SL_STATUS_OK if successful. Error code otherwise
 *****************************************************************************/
static sl_status_t create_channel_open_response(sl_bt_evt_l2cap_le_channel_open_request_t *request_event);

/******************************************************************************
 * Moves pending transfers to active state / sender or receiver.
 *
 * @param[in] data Pointer to the le_channel_open_request event
 *
 *                       An L2CAP error code as the outcome of the connection
 *                       request.
 *
 *                       Result @ref sl_bt_l2cap_connection_result_successful
 *                       indicates the connection request was accepted and
 *                       the logical channel is established. Other error code
 *                       value indicates the connection request was refused
 *                       by the peer device and other parameters of this
 *                       event must be ignored.
 *
 *****************************************************************************/
static void register_open_response_from_server(sl_bt_evt_l2cap_le_channel_open_response_t *response_event);

/******************************************************************************
 * Select transfer by connection handle and channel ID
 *
 * @param[in] connection connection handle
 * @param[in] cid Channel ID
 * @param[in] head_node Pointer to the transfer list containing the transfer
 *
 * @return sl_bt_l2cap_transfer_transfer_t The selected transfer object
 *****************************************************************************/
static sl_bt_l2cap_transfer_transfer_t *select_transfer_by_cid(uint8_t connection,
                                                               uint16_t cid,
                                                               sl_slist_node_t *head_node);

/******************************************************************************
 * Remove transfer by connection handle and channel ID
 *
 * @param[in] connection connection handle
 * @param[in] cid Channel ID
 * @param[in] head_node Pointer to the transfer list reference
 *****************************************************************************/
static void remove_transfer(uint8_t connection,
                            uint16_t cid,
                            sl_slist_node_t **head_node);

/******************************************************************************
 * Select transfer by connection handle - it will stop at the result
 *
 * @param[in] connection connection handle
 * @param[in] head_node Pointer to the transfer list containing the transfer
 *
 * @return sl_bt_l2cap_transfer_transfer_t The selected transfer object
 *****************************************************************************/
static sl_bt_l2cap_transfer_transfer_t *select_transfer_by_connection(uint8_t connection,
                                                                      sl_slist_node_t *head_node);

/******************************************************************************
 * Send data through L2CAP data channel - data is loaded from transfer object
 *
 * @param[in] transfer Transfer object containing all information for
 *            data transfer
 *
 *****************************************************************************/
static void send_data(sl_bt_l2cap_transfer_transfer_t *transfer);

/******************************************************************************
 * Process incoming data
 *
 * @param[in] data Incoming data - contaning connection handle, CID and the data
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *****************************************************************************/
static sl_status_t process_received_data(sl_bt_evt_l2cap_channel_data_t *data);

/******************************************************************************
 * Remove registered transfer from all state-lists
 *
 * @param[in] connection connection handle
 * @param[in] cid Channel ID
 *****************************************************************************/
static void remove_transfer_from_everywhere(uint16_t connection, uint16_t cid);

/******************************************************************************
 * Close selected transfer - by connection handle and channel ID and remove
 * from state-lists.
 *
 * @param[in] connection connection handle
 * @param[in] cid Channel ID
 * @param[in] reason  The disconnection reason
 *****************************************************************************/
static void close_transfer(uint16_t connection,
                           uint16_t cid,
                           sl_status_t error);

/******************************************************************************
 * Search transfer through all available state-lists.
 *
 * @param[in] connection connection handle
 * @param[in] cid Channel ID
 *
 * @return sl_bt_l2cap_transfer_transfer_t The searched transfer object
 *****************************************************************************/
static sl_bt_l2cap_transfer_transfer_t *search_for_transfer(uint16_t connection,
                                                            uint16_t cid);

// -----------------------------------------------------------------------------
// Public functions

/*******************************************************************************
 * Event handler
 ******************************************************************************/
void sli_bt_l2cap_transfer_on_bt_event(sl_bt_msg_t *evt)
{
  sl_bt_l2cap_transfer_transfer_t *transfer;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_l2cap_le_channel_open_request_id:
      // Responder
      create_channel_open_response(&evt->data.evt_l2cap_le_channel_open_request);
      break;

    case sl_bt_evt_l2cap_le_channel_open_response_id:
      // Initiator
      register_open_response_from_server(&evt->data.evt_l2cap_le_channel_open_response);
      break;

    case sl_bt_evt_l2cap_channel_data_id:
      process_received_data(&evt->data.evt_l2cap_channel_data);
      break;

    case sl_bt_evt_l2cap_channel_credit_id:
      transfer = select_transfer_by_cid(evt->data.evt_l2cap_channel_credit.connection,
                                        evt->data.evt_l2cap_channel_credit.cid,
                                        active_list);
      if (transfer != NULL) {
        transfer->credit += evt->data.evt_l2cap_channel_credit.credit;
      }
      break;

    case sl_bt_evt_l2cap_command_rejected_id:
      // Remote rejected the command.
      break;

    case sl_bt_evt_l2cap_channel_closed_id:

      close_transfer(evt->data.evt_l2cap_channel_closed.connection,
                     evt->data.evt_l2cap_channel_closed.cid,
                     evt->data.evt_l2cap_channel_closed.reason);
      break;
  }
}

sl_status_t sl_bt_l2cap_transfer_start_data_transfer(sl_bt_l2cap_transfer_transfer_t *transfer,
                                                     bool opening_channel)
{
  sl_status_t sc = SL_STATUS_OK;
  CORE_DECLARE_IRQ_STATE;

  sc = check_transfer_init_conditions(transfer);

  if (sc == SL_STATUS_OK ) {
    CORE_ENTER_CRITICAL();
    if (opening_channel == true) {
      sl_slist_push_back(&request_transfer_list, &transfer->node);
    } else {
      sl_slist_push_back(&request_response_transfer_list, &transfer->node);
    }
    CORE_EXIT_CRITICAL();
  }

  return sc;
}

// Looping
void sli_bt_l2cap_transfer_process_action(void)
{
  sl_slist_node_t *iterator;

  // Initiator
  if (request_transfer_list != NULL) {
    open_channel();
  }

  // Neutral
  if (active_list != NULL) {
    SL_SLIST_FOR_EACH(active_list, iterator) {
      if (((sl_bt_l2cap_transfer_transfer_t *)iterator)->mode == SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT) {
        send_data((sl_bt_l2cap_transfer_transfer_t *)iterator);
      }
    }
  }
}

void sli_bt_l2cap_transfer_init(void)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  sl_slist_init(&request_transfer_list);
  sl_slist_init(&request_pending_transfer_list);
  sl_slist_init(&active_list);

  sl_slist_init(&request_response_transfer_list);
  sl_slist_init(&request_response_buffer_transfer_list);
  sl_slist_init(&receive_active_transfer_list);
  CORE_EXIT_CRITICAL();
}

sl_status_t sl_bt_l2cap_transfer_increase_credit(sl_bt_l2cap_transfer_transfer_t *transfer,
                                                 uint16_t credit)
{
  sl_status_t sc = SL_STATUS_NULL_POINTER;

  if (transfer != NULL) {
    sc = sl_bt_l2cap_channel_send_credit(transfer->connection, transfer->cid, credit);
  }

  return sc;
}

sl_status_t sl_bt_l2cap_transfer_abort_transfer(sl_bt_l2cap_transfer_transfer_t *transfer)
{
  sl_status_t sc = SL_STATUS_NULL_POINTER;

  if (transfer != NULL) {
    sc = sl_bt_l2cap_close_channel(transfer->connection, transfer->cid);
    if (sc != SL_STATUS_OK) {
      remove_transfer_from_everywhere(transfer->connection, transfer->cid);
    }
  }

  return sc;
}

sl_status_t sl_bt_l2cap_transfer_check_progress(sl_bt_l2cap_transfer_transfer_t *transfer)
{
  sl_status_t sc = SL_STATUS_OK;
  if (transfer == NULL) {
    sc = SL_STATUS_NULL_POINTER;
  } else if (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT
             || transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_RECEIVE) {
    // Search for the transfer
    sl_bt_l2cap_transfer_transfer_t *transfer_found = search_for_transfer(transfer->connection,
                                                                          transfer->cid);
    // Status is in progress only if identical
    if (transfer_found == transfer) {
      sc = SL_STATUS_IN_PROGRESS;
    }
  }
  return sc;
}

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT

sl_power_manager_on_isr_exit_t sli_bt_l2cap_transfer_sleep_on_isr_exit()
{
  sl_power_manager_on_isr_exit_t ret = SL_POWER_MANAGER_IGNORE;

  // Sleep should be avoided if there is an active transfer
  if ((request_transfer_list != NULL)
      || (request_pending_transfer_list != NULL)
      || (active_list != NULL)) {
    ret = SL_POWER_MANAGER_WAKEUP;
  }

  return ret;
}

bool sli_bt_l2cap_transfer_is_ok_to_sleep()
{
  bool ret = false;

  // Sleep should be avoided if there is an active transfer
  if ((request_transfer_list == NULL)
      && (request_pending_transfer_list == NULL)
      && (active_list == NULL)) {
    ret = true;
  }
  return ret;
}

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

// -----------------------------------------------------------------------------
// Private functions

static sl_status_t create_channel_open_response(sl_bt_evt_l2cap_le_channel_open_request_t *request_event)
{
  CORE_DECLARE_IRQ_STATE;
  sl_status_t sc = SL_STATUS_FAIL;
  sl_bt_l2cap_transfer_transfer_t *transfer;

  transfer = select_transfer_by_connection(request_event->connection,
                                           request_response_transfer_list);

  if (transfer != NULL) {
    transfer->connection = request_event->connection;
    transfer->cid = request_event->cid;
    transfer->spsm = request_event->spsm;

    if (transfer->max_sdu > request_event->max_sdu) {
      transfer->max_sdu = request_event->max_sdu;
    }

    if (transfer->max_pdu > request_event->max_pdu) {
      transfer->max_pdu = request_event->max_pdu;
    }

    if (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT) {
      // The RECEIVER opened the l2cap channel, its value should be used
      transfer->credit = (request_event->credit != 0) ? request_event->credit : DEFAULT_CREDIT_VALUE;
    } else if (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_RECEIVE) {
      // The TRANSMITTER opened the l2cap channel, credit is stored in transfer obj
      // or is overwritten to default value, if its 0
      if (transfer->credit == 0) {
        transfer->credit = DEFAULT_CREDIT_VALUE;
      }
    }

    // When parameters are set, send open response
    sc = sl_bt_l2cap_send_le_channel_open_response(transfer->connection,
                                                   transfer->cid,
                                                   transfer->max_sdu,
                                                   transfer->max_pdu,
                                                   transfer->credit,
                                                   sl_bt_l2cap_connection_result_successful);

    if (sc == SL_STATUS_OK) {
      CORE_ENTER_CRITICAL();
      sl_slist_remove(&request_response_transfer_list, &transfer->node);

      if (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT) {
        sl_slist_push(&active_list, &transfer->node);
      } else {
        sl_slist_push(&receive_active_transfer_list, &transfer->node);
      }
      CORE_EXIT_CRITICAL();
    } else {
      close_transfer(transfer->connection, transfer->cid, sc);
    }
  } else {
    sc = sl_bt_l2cap_send_le_channel_open_response(request_event->connection,
                                                   request_event->cid,
                                                   request_event->max_sdu,
                                                   request_event->max_pdu,
                                                   request_event->credit,
                                                   sl_bt_l2cap_connection_result_no_resources_available);
  }

  return sc;
}

static void register_open_response_from_server(sl_bt_evt_l2cap_le_channel_open_response_t *response_event)
{
  CORE_DECLARE_IRQ_STATE;
  sl_bt_l2cap_transfer_transfer_t *transfer;
  uint16_t error_code = response_event->errorcode;

  // Check if the response was coming from the right connection
  transfer = select_transfer_by_connection(response_event->connection,
                                           request_pending_transfer_list);

  if (transfer != NULL) {
    transfer->max_pdu = response_event->max_pdu;
    transfer->max_sdu = response_event->max_sdu;
    transfer->credit = response_event->credit;

    CORE_ENTER_CRITICAL();

    sl_slist_remove(&request_pending_transfer_list, &transfer->node);

    if (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT
        && sl_bt_l2cap_connection_result_successful == error_code) {
      sl_slist_push(&active_list, &transfer->node);
    } else if (sl_bt_l2cap_connection_result_successful == error_code) {
      sl_slist_push(&receive_active_transfer_list, &transfer->node);
    }

    CORE_EXIT_CRITICAL();

    if (sl_bt_l2cap_connection_result_successful != error_code) {
      transfer->channel_error = error_code;
      close_transfer(transfer->connection, transfer->cid, error_code);
    }
  }
}

static sl_status_t check_transfer_init_conditions(sl_bt_l2cap_transfer_transfer_t *transfer)
{
  sl_status_t sc = SL_STATUS_OK;

  if (transfer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_INACTIVE) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  if ((transfer->connection == INVALID_CONNECTION_HANDLE)
      && (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_RECEIVE)) {
    sc = SL_STATUS_INVALID_HANDLE;
  }
  // Receiver side
  if (transfer->mode == SL_BT_L2CAP_TRANSFER_MODE_RECEIVE) {
    if (transfer->callbacks->on_finish == NULL
        || transfer->callbacks->on_receive == NULL) {
      sc = SL_STATUS_NULL_POINTER;
    }
    // Transmitter side
  } else {
    if (transfer->data_length == 0) {
      sc = SL_STATUS_INVALID_PARAMETER;
    } else if (transfer->callbacks->on_finish == NULL
               || transfer->callbacks->on_transmit == NULL) {
      sc = SL_STATUS_NULL_POINTER;
    }
  }
  return sc;
}

static sl_status_t process_received_data(sl_bt_evt_l2cap_channel_data_t *data)
{
  sl_status_t sc = SL_STATUS_FAIL;
  sl_bt_l2cap_transfer_transfer_t *transfer;
  uint16_t credit = 0;

  transfer = select_transfer_by_cid(data->connection,
                                    data->cid,
                                    receive_active_transfer_list);

  if (transfer != NULL) {
    // Register credit decrease after data was received
    if (transfer->credit > 0) {
      transfer->credit--;
    }

    credit = transfer->callbacks->on_receive(transfer,
                                             transfer->data_offset,
                                             &data->data);
    // If credit increase was requested, execute it.
    if (credit > 0) {
      sc = sl_bt_l2cap_channel_send_credit(transfer->connection,
                                           transfer->cid,
                                           credit);
    }

    // If data is received, the offset will be shifted by the pdu size
    transfer->data_offset += data->data.len;

    if (transfer->data_offset >= transfer->data_length) {
      (void)sl_bt_l2cap_close_channel(transfer->connection, transfer->cid);
      transfer->mode = SL_BT_L2CAP_TRANSFER_MODE_INACTIVE;
    }
  }

  return sc;
}

static sl_status_t open_channel(void)
{
  sl_status_t sc = SL_STATUS_FAIL;
  sl_bt_l2cap_transfer_transfer_t *transfer;

  CORE_DECLARE_IRQ_STATE;

  if (request_pending_transfer_list == NULL) {
    transfer = (sl_bt_l2cap_transfer_transfer_t *) request_transfer_list;

    sc = sl_bt_l2cap_open_le_channel(transfer->connection,
                                     transfer->spsm,
                                     transfer->max_sdu,
                                     transfer->max_pdu,
                                     transfer->credit,
                                     &transfer->cid);

    // Try to open the channel only once
    CORE_ENTER_CRITICAL();
    sl_slist_remove(&request_transfer_list, &transfer->node);

    if (sc == SL_STATUS_OK) {
      sl_slist_push_back(&request_pending_transfer_list, &transfer->node);
      CORE_EXIT_CRITICAL();
    } else {
      CORE_EXIT_CRITICAL();
      close_transfer(transfer->connection, transfer->cid, sc);
    }
  }
  return sc;
}

static void send_data(sl_bt_l2cap_transfer_transfer_t *transfer)
{
  sl_bt_l2cap_connection_result_t sc = sl_bt_l2cap_connection_result_successful;

  uint8_t *data_ptr = NULL;
  uint32_t data_len = 0;
  uint32_t request_len = 0;

  if (transfer->credit == 0) {
    return;
  }

  request_len = transfer->max_pdu;
  // If package size is smaller than max SDU, correct length
  if ((transfer->data_offset + request_len) > transfer->data_length) {
    request_len = transfer->data_length - transfer->data_offset;
  }

  // Get the data from the user
  transfer->callbacks->on_transmit(transfer,
                                   transfer->data_offset,
                                   request_len,
                                   &data_ptr,
                                   &data_len);

  if (data_len > 0 && data_len <= transfer->max_pdu) {
    // Send data through channel, then increase offset, decrease credit
    sc = (sl_bt_l2cap_connection_result_t)sl_bt_l2cap_channel_send_data(transfer->connection,
                                                                        transfer->cid,
                                                                        data_len,
                                                                        data_ptr);

    if (sc != SL_STATUS_OK) {
      // Close channel case 1 - error
      transfer->channel_error = sc;
      (void)sl_bt_l2cap_close_channel(transfer->connection, transfer->cid);

      transfer->mode = SL_BT_L2CAP_TRANSFER_MODE_INACTIVE;
    } else {
      transfer->data_offset += data_len;
      transfer->credit--;

      // Close channel case 2 - finished
      if (transfer->data_length <= transfer->data_offset) {
        transfer->channel_error = sl_bt_l2cap_connection_result_successful;
        (void)sl_bt_l2cap_close_channel(transfer->connection, transfer->cid);

        transfer->mode = SL_BT_L2CAP_TRANSFER_MODE_INACTIVE;
      }
    }
  }
}

static sl_bt_l2cap_transfer_transfer_t *select_transfer_by_cid(uint8_t connection,
                                                               uint16_t cid,
                                                               sl_slist_node_t *head_node)
{
  sl_slist_node_t *iterator;

  SL_SLIST_FOR_EACH(head_node, iterator)
  {
    if ( ((sl_bt_l2cap_transfer_transfer_t *)(iterator))->cid == cid
         && ((sl_bt_l2cap_transfer_transfer_t *)(iterator))->connection == connection) {
      break;
    }
  }
  return (sl_bt_l2cap_transfer_transfer_t *)iterator;
}

static void remove_transfer(uint8_t connection,
                            uint16_t cid,
                            sl_slist_node_t **head_node)
{
  sl_slist_node_t *iterator;
  sl_slist_node_t *head_ptr = *head_node;

  CORE_DECLARE_IRQ_STATE;

  SL_SLIST_FOR_EACH(head_ptr, iterator)
  {
    if ( ((sl_bt_l2cap_transfer_transfer_t *)(iterator))->cid == cid
         && ((sl_bt_l2cap_transfer_transfer_t *)(iterator))->connection == connection) {
      CORE_ENTER_CRITICAL();
      sl_slist_remove(head_node, iterator);
      CORE_EXIT_CRITICAL();
    }
  }
}

static sl_bt_l2cap_transfer_transfer_t *select_transfer_by_connection(uint8_t connection,
                                                                      sl_slist_node_t *head_node)
{
  sl_slist_node_t *iterator = NULL;

  SL_SLIST_FOR_EACH(head_node, iterator)
  {
    if (((sl_bt_l2cap_transfer_transfer_t *)(iterator))->connection == connection) {
      break;
    }
  }

  return (sl_bt_l2cap_transfer_transfer_t *) iterator;
}

static void close_transfer(uint16_t connection, uint16_t cid, sl_status_t error)
{
  sl_bt_l2cap_transfer_transfer_t *transfer;

  sl_status_t sc = error;
  bool disconnect_status = false;

  if ((sc == SL_STATUS_BT_L2CAP_REMOTE_DISCONNECTED)
      || (sc == SL_STATUS_BT_L2CAP_LOCAL_DISCONNECTED) ) {
    disconnect_status = true;
  }

  transfer = search_for_transfer(connection, cid);

  if (transfer != NULL) {
    transfer->channel_error = sc;

    // check if transfer is finished because data was transmitted
    if ((disconnect_status == true)
        && (transfer->data_length <= transfer->data_offset)) {
      sc = SL_STATUS_OK;
    }

    transfer->callbacks->on_finish(transfer,
                                   sc);
    remove_transfer_from_everywhere(connection, cid);
  }
}

static void remove_transfer_from_everywhere(uint16_t connection, uint16_t cid)
{
  remove_transfer(connection,
                  cid,
                  &request_transfer_list);
  remove_transfer(connection,
                  cid,
                  &request_pending_transfer_list);
  remove_transfer(connection,
                  cid,
                  &active_list);
  remove_transfer(connection,
                  cid,
                  &request_response_transfer_list);
  remove_transfer(connection,
                  cid,
                  &request_response_buffer_transfer_list);
  remove_transfer(connection,
                  cid,
                  &receive_active_transfer_list);
}

static sl_bt_l2cap_transfer_transfer_t *search_for_transfer(uint16_t connection,
                                                            uint16_t cid)
{
  sl_bt_l2cap_transfer_transfer_t *transfer;

  transfer = select_transfer_by_cid(connection,
                                    cid,
                                    request_transfer_list);
  if (transfer != NULL) {
    return transfer;
  }

  transfer = select_transfer_by_cid(connection,
                                    cid,
                                    request_pending_transfer_list);
  if (transfer != NULL) {
    return transfer;
  }

  transfer = select_transfer_by_cid(connection,
                                    cid,
                                    active_list);
  if (transfer != NULL) {
    return transfer;
  }

  transfer = select_transfer_by_cid(connection,
                                    cid,
                                    request_response_transfer_list);
  if (transfer != NULL) {
    return transfer;
  }

  transfer = select_transfer_by_cid(connection,
                                    cid,
                                    request_response_buffer_transfer_list);
  if (transfer != NULL) {
    return transfer;
  }

  transfer = select_transfer_by_cid(connection,
                                    cid,
                                    receive_active_transfer_list);
  if (transfer != NULL) {
    return transfer;
  }

  return NULL;
}
