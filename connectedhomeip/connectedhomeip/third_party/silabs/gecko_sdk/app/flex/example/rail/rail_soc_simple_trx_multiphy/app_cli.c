/***************************************************************************//**
 * @file
 * @brief app_cli.c
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
#include <stdint.h>
#include "sl_component_catalog.h"
#include "em_chip.h"
#include "app_log.h"
#include "sl_cli.h"
#include "app_process.h"
#include "sl_rail_util_init.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Used for indicates the current status of forwarding rx packets on UART
#define ON   "ON"
/// Used for indicates the current status of forwarding rx packets on UART
#define OFF  "OFF"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// The variable shows whether only one channel is used for rx, or both (scan mode)
extern volatile bool scan_mode;

/// Indicates the actual rx channel
extern volatile uint8_t rx_active_channel;

/// Which channel will be used for sending
extern volatile uint8_t send_channel;

/// Flag that indicates that send already happend in scan mode
extern volatile bool send_requested;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * CLI - info message: Unique ID of the board
 *****************************************************************************/
void cli_info(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  app_log_info("Info:\n");
  app_log_info("  MCU Id:          0x%llx\n", SYSTEM_GetUnique());
  app_log_info("  Scan mode:       %s\n", scan_mode ? ON : OFF);
  // Print the actual channel if scan mode is OFF
  if (!scan_mode) {
    app_log_info("  Current channel: %d\n", rx_active_channel);
  }
}

/******************************************************************************
 * CLI - send: Sets a flag indicating that a packet has to be sent
 *****************************************************************************/
void cli_send_packet(sl_cli_command_arg_t *arguments)
{
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);

  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  if ( RAIL_IsValidChannel(rail_handle, channel) != RAIL_STATUS_NO_ERROR ) {
    app_log_info("Channel is not valid\n");
    return;
  }

  send_channel = channel;

  if ( !scan_mode ) {
    set_next_state(S_START_SENDING);
  } else {
    send_requested = true;
  }

  app_log_info("Send packet request on channel %d\n", send_channel);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - rx_channel: Receive on a particular channel, instead of scanning both
 *****************************************************************************/
void cli_rx_channel(sl_cli_command_arg_t *arguments)
{
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);

  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  if ( RAIL_IsValidChannel(rail_handle, channel) != RAIL_STATUS_NO_ERROR ) {
    app_log_info("Channel is not valid\n");
    return;
  }

  if (scan_mode) {
    app_log_info("Exiting scan mode\n");
  }

  scan_mode = false;
  rx_active_channel = channel;

  stop_timer_wait_for_idle();

  set_next_state(S_START_RECEIVING);
  RAIL_StateTransitions_t default_state_transition_to_rx = {
    .error = RAIL_RF_STATE_RX,
    .success = RAIL_RF_STATE_RX
  };
  RAIL_SetRxTransitions(rail_handle, &default_state_transition_to_rx);

  app_log_info("Receive packet is enabled on channel: %d\n", rx_active_channel);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * CLI - rx_scan: Sets a flag indicating that a device should be in scan mode
 *****************************************************************************/
void cli_rx_scan(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  stop_timer_wait_for_idle();

  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  set_next_state(S_START_RECEIVING);
  RAIL_StateTransitions_t default_state_transition_to_idle = {
    .error = RAIL_RF_STATE_IDLE,
    .success = RAIL_RF_STATE_IDLE
  };
  RAIL_SetRxTransitions(rail_handle, &default_state_transition_to_idle);

  scan_mode = true;
  app_log_info("Scan mode is enabled\n");
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
