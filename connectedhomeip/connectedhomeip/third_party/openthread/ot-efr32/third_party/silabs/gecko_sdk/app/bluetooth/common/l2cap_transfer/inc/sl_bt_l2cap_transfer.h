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

#ifndef SL_BT_L2CAP_TRANSFER_H
#define SL_BT_L2CAP_TRANSFER_H

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_POWER_MANAGER_PRESENT
#include "sl_power_manager.h"
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

#include "sl_bt_api.h"
#include "sl_slist.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Define sender and receiver sides
typedef enum {
  SL_BT_L2CAP_TRANSFER_MODE_RECEIVE  = 0, ///< Transfer side is responding to data receive events
  SL_BT_L2CAP_TRANSFER_MODE_TRANSMIT = 1, ///< Transfer side is initiating data transfer
  SL_BT_L2CAP_TRANSFER_MODE_INACTIVE = 2  ///< Transfer side is inactive - closing
} sl_bt_l2cap_transfer_transfer_mode_t;

/// Forward declaration
typedef struct sl_bt_l2cap_transfer_transfer_s *sl_bt_l2cap_transfer_transfer_handle_t;

// -----------------------------------------------------------------------------
// Callback functions

/**************************************************************************//**
 * Callback is invoked when data on L2CAP channel is received.
 *
 * @param[in] transfer_object Active transfer
 * @param[in] offset          Current data offset
 * @param[in] data            Incoming data array
 *
 * @return credit Requested credit that should be sent to the other side
 *****************************************************************************/
typedef uint16_t (*sl_bt_l2cap_transfer_data_received_cb_t)(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                            uint32_t                               offset,
                                                            uint8array                             *data);

/**************************************************************************//**
 * Callback is invoked when data transfer on L2CAP channel is finished.
 *
 * @param[in] transfer_object Active transfer
 * @param[in] error_code SL_STATUS type error code
 *****************************************************************************/
typedef void (*sl_bt_l2cap_transfer_transfer_finished_cb_t)(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                            sl_status_t                            error_code);

/**************************************************************************//**
 * Callback for data transmission on L2CAP channel. Called when data is about
 * to transmit on the channel to provide the next data packet.
 *
 * @param[in]  transfer_object Active transfer
 * @param[in]  offset          Current offset of requested data
 * @param[in]  size            Maximum size of requested data
 * @param[out] data            Pointer of pointer to data
 * @param[out] data_size       Size of the provided data
 *****************************************************************************/
typedef void (*sl_bt_l2cap_transfer_data_transmit_cb_t)(sl_bt_l2cap_transfer_transfer_handle_t transfer_object,
                                                        uint32_t                               offset,
                                                        uint32_t                               size,
                                                        uint8_t                                **data,
                                                        uint32_t                               *data_size);

/**************************************************************************//**
 * Callback is invoked when a channel is opened.
 *
 * @param[in] transfer_object Active transfer
 *****************************************************************************/
typedef void (*sl_bt_l2cap_transfer_channel_opened_cb_t)(sl_bt_l2cap_transfer_transfer_handle_t transfer_object);

/// Callback functions used by the transfer object
typedef struct callbacks_s{
  sl_bt_l2cap_transfer_data_transmit_cb_t     on_transmit; ///< Callback function called when transferring data
  sl_bt_l2cap_transfer_data_received_cb_t     on_receive;  ///< Callback function called when data is received (Mandatory for receiver)
  sl_bt_l2cap_transfer_transfer_finished_cb_t on_finish;   ///< Callback function called when transfer is finished/closed (Mandatory)
  sl_bt_l2cap_transfer_channel_opened_cb_t    on_open;     ///< Callback function called when transfer channel is opened (Optional)
} sl_bt_l2cap_transfer_callbacks_t;

/// Transfer structure that is used for the L2CAP data channels
typedef struct sl_bt_l2cap_transfer_transfer_s {
  sl_slist_node_t node;                          ///< Node for linked lists
  sl_bt_l2cap_transfer_callbacks_t *callbacks;   ///< Link to callback functions - Mandatory
  uint16_t cid;                                  ///< Channel identifier - automatically assigned
  uint16_t max_sdu;                              ///< The Maximum Service Data Unit size the local channel endpoint can accept - Range: 23 to 65533. - Mandatory
  uint16_t max_pdu;                              ///< The maximum PDU payload size the local channel endpoint can accept - Range:23 to 252. - Mandatory
  uint16_t spsm;                                 ///< The protocol/services implemented by the local channel endpoint - Mandatory
  uint16_t credit;                               ///< The initial credit value of the local channel endpoint, i.e., number of PDUs the peer channel endpoint can send. - Mandatory for responder
  uint16_t data_length;                          ///< Total length of data to be sent - Mandatory for initiator
  uint16_t data_offset;                          ///< Used to indicate how much data was sent
  uint8_t connection;                            ///< The connection handle - Mandatory
  sl_status_t channel_error;                     ///< Channel error code
  sl_bt_l2cap_transfer_transfer_mode_t mode;     ///< Transfer mode: initiator/responder - Mandatory
} sl_bt_l2cap_transfer_transfer_t;

/**************************************************************************//**
 * L2CAP event handler.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sli_bt_l2cap_transfer_on_bt_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Function called periodically by scheduler.
 * Main task: handle transfers in different states
 *
 *****************************************************************************/
void sli_bt_l2cap_transfer_process_action(void);

/**************************************************************************//**
 * Initialize linked list which are storing transfers
 *
 *****************************************************************************/
void sli_bt_l2cap_transfer_init(void);

/**************************************************************************//**
 * Start data transfer process. Both sides should call this function
 * and decide which side will send the open request.
 *
 * Its important that the transfer object shall be well configured to create
 * a valid open request.
 *
 * @param[in] transfer     Active transfer object. Mandatory settings are:
 *                          - callbacks
 *                          - connection,
 *                          - max_pdu,
 *                          - max_sdu,
 *                          - data,
 *                          - data_length,
 *                          - mode
 *                          - spsm
 *
 * @param[in] open_channel If true - this transfer will initiate the
 *                         L2CAP channel opening
 *
 * @retval SL_STATUS_OK Success.
 * @retval SL_STATUS_INVALID_HANDLE Connection handle is invalid.
 * @retval SL_STATUS_NULL_POINTER One of the callback functions is not set for
 *                                the transfer object
 * @retval SL_STATUS_INVALID_PARAMETER One of the following parameters is not
 *                                     set in the transfer object: max_pdu,
 *                                     max_sdu, data, data_length, mode
 *****************************************************************************/
sl_status_t sl_bt_l2cap_transfer_start_data_transfer(sl_bt_l2cap_transfer_transfer_t *transfer,
                                                     bool                            open_channel);

/**************************************************************************//**
 * API call to send credit to the initiator.
 *
 * @param[in] transfer Targeted transfer object - name of the transfer
 * @param[in] credit The credit value, i.e., the additional number of PDUs the
 *            peer channel endpoint can send. Range: 1 to 65535.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise
 *****************************************************************************/
sl_status_t sl_bt_l2cap_transfer_increase_credit(sl_bt_l2cap_transfer_transfer_t *transfer,
                                                 uint16_t                        credit);

/**************************************************************************//**
 * API call to abort L2CAP data transfer - will invoke a close channel event
 *
 * @param[in] transfer Targeted transfer object - name of the transfer
 *
 * @return SL_STATUS_OK if successful. Error code otherwise
 *****************************************************************************/
sl_status_t sl_bt_l2cap_transfer_abort_transfer(sl_bt_l2cap_transfer_transfer_t *transfer);

/**************************************************************************//**
 * Check if a transfer is in progress
 *
 * @param[in] transfer Targeted transfer object - name of the transfer
 *
 * @retval SL_STATUS_IN_PROGRESS if a transfer is in progress
 * @retval SL_STATUS_OK if a transfer is not in progress
 * @retval SL_STATUS_NULL_POINTER if a transfer reference is NULL
 *****************************************************************************/
sl_status_t sl_bt_l2cap_transfer_check_progress(sl_bt_l2cap_transfer_transfer_t *transfer);

#ifdef SL_CATALOG_POWER_MANAGER_PRESENT

/***************************************************************************//**
 * Checks if it is ok to sleep now / power manager
 *
 * @return true - if ready to sleep, false otherwise
 ******************************************************************************/
bool sli_bt_l2cap_transfer_is_ok_to_sleep(void);

/***************************************************************************//**
 * Routine for power manager handler
 *
 * @return sl_power_manager_on_isr_exit_t
 * @retval SL_POWER_MANAGER_IGNORE = (1UL << 0UL), < The module did not trigger an ISR and it doesn't want to contribute to the decision
 * @retval SL_POWER_MANAGER_SLEEP  = (1UL << 1UL), < The module was the one that caused the system wakeup and the system SHOULD go back to sleep
 * @retval SL_POWER_MANAGER_WAKEUP = (1UL << 2UL), < The module was the one that caused the system wakeup and the system MUST NOT go back to sleep
 ******************************************************************************/
sl_power_manager_on_isr_exit_t sli_bt_l2cap_transfer_sleep_on_isr_exit(void);

#endif // SL_CATALOG_POWER_MANAGER_PRESENT

#ifdef __cplusplus
};
#endif

#endif // SL_BT_L2CAP_TRANSFER_H
