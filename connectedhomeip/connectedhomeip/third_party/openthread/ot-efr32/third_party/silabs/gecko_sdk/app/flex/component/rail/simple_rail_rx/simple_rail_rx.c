/***************************************************************************//**
 * @file
 * @brief simple_rail_rx_.c
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
#include "app_log.h"
#include "em_common.h"
#include "sl_rail_util_init.h"
#include "simple_rail_rx.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Sets the current state of the simple_rail_rx state machine
 *
 * @param requested_state New state of simple_rail_rx state machine
 * @returns None
 *****************************************************************************/
__STATIC_INLINE sl_status_t simple_rail_rx_set_state(simple_rail_rx_state_t requested_state);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// The flag indicates whether an Rx packet is received and copied to the rx FIFO
volatile bool sl_rx_packet_copied = false;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// A handle of a RAIL instance, returned from RAIL_Init()
static RAIL_Handle_t sl_rail_handle;
/// The variable shows the actual state of the RAIL Rx state machine
static volatile simple_rail_rx_state_t simple_rail_rx_state = S_RX_IDLE;
/// Contains the last RAIL Tx error events
static volatile uint64_t current_rail_rx_err = 0;
// Return value of setting the state machine of simple_rail_rx
static sl_status_t simple_rail_rx_status;
/// Receive FIFO, can be used by other C modules
static uint8_t sl_rx_fifo[RAIL_RX_FIFO_SIZE];
/// If state machine update has an error in interrupt, print it in tick
static volatile bool rx_state_machine_update_failed = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * RAIL callback, handles Rx events
 *****************************************************************************/
void sl_simple_rail_rx_on_event(RAIL_Handle_t rail_handle,
                                RAIL_Events_t events)
{
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      RAIL_RxPacketHandle_t rx_packet_handle;
      // Keep the packet in the radio buffer, download it later at the state machine
      rx_packet_handle = RAIL_HoldRxPacket(rail_handle);
      bool is_valid_handle = (rx_packet_handle != RAIL_RX_PACKET_HANDLE_INVALID);
      if (!is_valid_handle) {
        simple_rail_rx_status = simple_rail_rx_set_state(S_RX_NOT_VALID_PACKET_RECEIVED);
        rx_state_machine_update_failed = !(simple_rail_rx_status == SL_STATUS_OK);
      } else {
        simple_rail_rx_status = simple_rail_rx_set_state(S_RX_PACKET_RECEIVED);
        rx_state_machine_update_failed = !(simple_rail_rx_status == SL_STATUS_OK);
      }
    } else {
      // Handle Rx error
      current_rail_rx_err = (events & RAIL_EVENTS_RX_COMPLETION);
      simple_rail_rx_status = simple_rail_rx_set_state(S_RX_PACKET_ERROR);
      rx_state_machine_update_failed = !(simple_rail_rx_status == SL_STATUS_OK);
    }
  }
}

/******************************************************************************
 * Init of rail_rx
 *****************************************************************************/
void sl_simple_rail_rx_init(void)
{
  // Get RAIL handle, used later by the application
  sl_rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Starts RAIL reception
  RAIL_StartRx(sl_rail_handle, RX_CHANNEL, NULL);
}

/******************************************************************************
 * Callback, called if a received packet is copied to the rx fifo
 *****************************************************************************/
SL_WEAK void sl_simple_rail_rx_packet_copied_cbk(const uint8_t* rx_fifo)
{
  // Eliminate compiler warnings
  (void) rx_fifo;

  /////////////////////////////////////////////////////////////////////////////
  // Put your application code here, or implement non-weak function!         //
  /////////////////////////////////////////////////////////////////////////////
}

/******************************************************************************
 * State machine for simple_rail_rx
 *****************************************************************************/
void sl_simple_rail_rx_tick(void)
{
  // Packet info and handle
  RAIL_RxPacketInfo_t packet_info;
  RAIL_RxPacketHandle_t packet_handle;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;

  if (rx_state_machine_update_failed) {
    app_log_error("Simple RAIL Rx state machine update failed in sl_simple_rail_rx_on_event");
    rx_state_machine_update_failed = false;
  }

  switch (sl_simple_rail_rx_get_state()) {
    case S_RX_PACKET_RECEIVED:
      // Packet received:
      //  - Copy it to the application FIFO and set flag to the application
      //  - Free up the radio FIFO
      //  - Return to Rx (auto state transition)
      packet_handle = RAIL_GetRxPacketInfo(sl_rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
      RAIL_CopyRxPacket(sl_rx_fifo, &packet_info);
      sl_rx_packet_copied = true;
      sl_simple_rail_rx_packet_copied_cbk(&sl_rx_fifo[0]);
      rail_status = RAIL_ReleaseRxPacket(sl_rail_handle, packet_handle);
      if (rail_status != RAIL_STATUS_NO_ERROR) {
        app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
      }
      simple_rail_rx_status = simple_rail_rx_set_state(S_RX_IDLE);
      if (simple_rail_rx_status != SL_STATUS_OK) {
        app_log_error("Simple RAIL Rx state machine update failed");
      }
      break;
    case S_RX_NOT_VALID_PACKET_RECEIVED:
      app_log_warning("No such RAIL Rx packet yet exists or RAIL handle is not active");
      simple_rail_rx_status = simple_rail_rx_set_state(S_RX_IDLE);
      if (simple_rail_rx_status != SL_STATUS_OK) {
        app_log_error("Simple RAIL Rx state machine update failed");
      }
      break;
    case S_RX_PACKET_ERROR:
      // Handle Rx error
      app_log_error("Radio RX Error occurred\nEvents: %llX\n", (long long unsigned int) current_rail_rx_err);
      simple_rail_rx_status = simple_rail_rx_set_state(S_RX_IDLE);
      if (simple_rail_rx_status != SL_STATUS_OK) {
        app_log_error("Simple RAIL Rx state machine update failed");
      }
      break;
    default:
      // Do nothing
      break;
  }
}

/******************************************************************************
 * Gets actual state of the state machine of simple_rail_rx.
 * The function shall be called from exclusive area!
 *****************************************************************************/
simple_rail_rx_state_t sl_simple_rail_rx_get_state(void)
{
  return simple_rail_rx_state;
}

/******************************************************************************
 * Returns with the rx_fifo
 *****************************************************************************/
uint8_t* sl_simple_rail_rx_get_rx_fifo(void)
{
  return sl_rx_fifo;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Sets the actual state of the state machine of simple_rail_rx
 *****************************************************************************/
__STATIC_INLINE sl_status_t simple_rail_rx_set_state(simple_rail_rx_state_t requested_state)
{
  sl_status_t status = SL_STATUS_OK;
  if (requested_state < S_RX_ENUM) {
    simple_rail_rx_state = requested_state;
  } else {
    status = SL_STATUS_INVALID_PARAMETER;
  }

  return status;
}
