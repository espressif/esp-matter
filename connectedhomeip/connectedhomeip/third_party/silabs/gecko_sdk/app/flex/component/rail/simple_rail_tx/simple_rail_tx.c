/***************************************************************************//**
 * @file
 * @brief simple_rail_tx_.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_assert.h"
#include "app_log.h"
#include "sl_rail_util_init.h"
#include "simple_rail_tx.h"
#include "sl_status.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Sets the current state of the simple_rail_tx state machine
 *
 * @param requested_state New state of simple_rail_tx state machine
 * @returns None
 *****************************************************************************/
__STATIC_INLINE sl_status_t simple_rail_tx_set_state(simple_rail_tx_state_t requested_state);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// RAIL Transmit FIFO, can be updated from app
uint8_t sl_tx_fifo[RAIL_TX_FIFO_SIZE] = UNIVERSAL_PAYLOAD;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// A handle of a RAIL instance, returned from RAIL_Init()
static RAIL_Handle_t sl_rail_handle;
/// The variable shows the actual state of the RAIL Tx state machine
static volatile simple_rail_tx_state_t simple_rail_tx_state = S_TX_IDLE;
/// Contains the last RAIL Tx error events
static volatile uint64_t current_rail_tx_err = 0;
// Return value of setting the state machine of simple_rail_tx
static sl_status_t simple_rail_tx_status;
/// If state machine update has an error in interrupt, print it in tick
static volatile bool tx_state_machine_update_failed = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Init of rail_tx
 *****************************************************************************/
void sl_simple_rail_tx_init(void)
{
  // Get RAIL handle, used later by the application
  sl_rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
}

/******************************************************************************
 * RAIL callback, handles Tx events
 *****************************************************************************/
void sl_simple_rail_tx_on_event(RAIL_Handle_t rail_handle,
                                RAIL_Events_t events)
{
  // Eliminate compiler warnings
  (void) rail_handle;

  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION ) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      simple_rail_tx_status = simple_rail_tx_set_state(S_TX_PACKET_SENT);
      tx_state_machine_update_failed = !(simple_rail_tx_status == SL_STATUS_OK);
    } else {
      // Handle Tx error
      current_rail_tx_err = (events & RAIL_EVENTS_TX_COMPLETION);
      simple_rail_tx_status = simple_rail_tx_set_state(S_TX_PACKET_ERROR);
      tx_state_machine_update_failed = !(simple_rail_tx_status == SL_STATUS_OK);
    }
  }
}

/******************************************************************************
 * State machine for simple_rail_tx
 *****************************************************************************/
void sl_simple_rail_tx_tick(void)
{
  // Status of RAIL_StartTx() call
  sl_status_t start_tx_status;
  // RAIL FIFO size allocated by RAIL_SetTxFifo() call
  uint16_t allocated_fifo_size = 0;

  if (tx_state_machine_update_failed) {
    app_log_error("Simple RAIL Tx state machine update failed in sl_simple_rail_tx_on_event");
    tx_state_machine_update_failed = false;
  }

  switch (sl_simple_rail_tx_get_state()) {
    case S_TX_PACKET_SEND_REQUEST:
      // Set RAIL FIFO
      allocated_fifo_size = RAIL_SetTxFifo(sl_rail_handle, sl_tx_fifo, TX_PAYLOAD_LENGTH, RAIL_TX_FIFO_SIZE);
      app_assert(allocated_fifo_size == RAIL_TX_FIFO_SIZE,
                 "RAIL_SetTxFifo() failed to allocate a large enough buffer (%d bytes instead of %d bytes)\n",
                 allocated_fifo_size,
                 RAIL_TX_FIFO_SIZE);

      // Send RAIL messages
      start_tx_status = RAIL_StartTx(sl_rail_handle, TX_CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);
      app_assert_status_f(start_tx_status,
                          "[E: 0x%04x]: RAIL_StartTx() failed\n",
                          (int)start_tx_status);
      simple_rail_tx_status = simple_rail_tx_set_state(S_TX_PACKET_SENDING);
      if (simple_rail_tx_status != SL_STATUS_OK) {
        app_log_error("Simple RAIL Tx state machine update failed");
      }
      break;
    case S_TX_PACKET_SENT:
      app_log_info("Packet has been sent\n");
      simple_rail_tx_status = simple_rail_tx_set_state(S_TX_IDLE);
      if (simple_rail_tx_status != SL_STATUS_OK) {
        app_log_error("Simple RAIL Tx state machine update failed");
      }
      break;
    case S_TX_PACKET_ERROR:
      app_log_warning("Radio TX Error occurred\nEvents: %llX\n", (long long unsigned int) current_rail_tx_err);
      current_rail_tx_err = 0;
      simple_rail_tx_status = simple_rail_tx_set_state(S_TX_IDLE);
      if (simple_rail_tx_status != SL_STATUS_OK) {
        app_log_error("Simple RAIL Tx state machine update failed");
      }
      break;
    default:
      // Do nothing
      break;
  }
}

/******************************************************************************
 * Updates sl_tx_fifo, i.e. updates RAIL transmit data (max 16 bytes) and
 * triggers the Tx state machine (TX_PACKET_SEND_REQUEST)
 *****************************************************************************/
sl_status_t sl_simple_rail_tx_write_tx_fifo(const void * const src_fifo,
                                            const uint8_t num_of_bytes)
{
  sl_status_t status = SL_STATUS_OK;

  if ((src_fifo == NULL)
      || (num_of_bytes == 0)
      || (num_of_bytes > TX_PAYLOAD_LENGTH)) {
    status = SL_STATUS_INVALID_PARAMETER;
  }

  if (status == SL_STATUS_OK) {
    // Copy payload and update state machine
    memcpy(sl_tx_fifo, src_fifo, num_of_bytes);
    simple_rail_tx_status = simple_rail_tx_set_state(S_TX_PACKET_SEND_REQUEST);
    if (simple_rail_tx_status != SL_STATUS_OK) {
      app_log_error("Simple RAIL Tx state machine update failed");
    }
  }

  return status;
}

/******************************************************************************
 * Gets actual state of the state machine of simple_rail_tx.
 * The function shall be called from exclusive area!
 *****************************************************************************/
simple_rail_tx_state_t sl_simple_rail_tx_get_state(void)
{
  return simple_rail_tx_state;
}

/******************************************************************************
 * Updates the payload to be sent, and triggers a transmission
 *****************************************************************************/
sl_status_t sl_simple_rail_tx_transmit(const void * const p_src,
                                       const uint8_t num_of_bytes)
{
  sl_status_t transmit_status = SL_STATUS_OK;

  if (p_src == NULL || num_of_bytes == 0) {
    transmit_status = SL_STATUS_INVALID_PARAMETER;
  }
  if (transmit_status == SL_STATUS_OK) {
    transmit_status = sl_simple_rail_tx_write_tx_fifo(p_src, num_of_bytes);
  }
  if (transmit_status == SL_STATUS_OK) {
    transmit_status = simple_rail_tx_set_state(S_TX_PACKET_SEND_REQUEST);
  }
  return transmit_status;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Sets the actual state of the state machine of simple_rail_tx
 *****************************************************************************/
__STATIC_INLINE sl_status_t simple_rail_tx_set_state(simple_rail_tx_state_t requested_state)
{
  sl_status_t status = SL_STATUS_OK;
  if (requested_state < S_TX_ENUM) {
    simple_rail_tx_state = requested_state;
  } else {
    status = SL_STATUS_INVALID_PARAMETER;
  }

  return status;
}
