/***************************************************************************//**
 * @file
 * @brief simple_rail_tx_cli.c
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
#include "sl_cli.h"
#include "app_log.h"
#include "simple_rail_rx.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Used for indicates the current status of forwarding rx packets on UART
#define ON   "ON"
/// Used for indicates the current status of forwarding rx packets on UART
#define OFF  "OFF"
/// RAIL payload length
#define PAYLOAD_LENGTH (16U)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/// Indicates whether the RAIL rx packets shall be forwarded on CLI
/// (0 - doesn't forward, forward anyway)
static uint8_t rx_forward = 1;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// The flag indicates whether an Rx packet is received and copied to the rx FIFO
extern volatile bool sl_rx_packet_copied;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Rx Fifo, where RAIL packet content is copied
static uint8_t* rx_fifo;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * CLI - receive
 * Sets/clears the flag indicating whether the RAIL rx packet shall be
 * forwarded on CLI or not. The flag is used by the rail_rx_cli state machine.
 *****************************************************************************/
void cli_receive_packet(sl_cli_command_arg_t *arguments)
{
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  // String representation of rx_forward
  const char* str_rx_forward;
  rx_forward = sl_cli_get_argument_uint8(arguments, 0);
  if (rx_forward == 0) {
    str_rx_forward = OFF;
  } else {
    str_rx_forward = ON;
  }

  app_log_info("Forward received packets: %s\n", str_rx_forward);
}

/******************************************************************************
 * Initializes a static rx_fifo ptr, points to the buffer where the
 * RAIL packet content is copied
 *****************************************************************************/
void sl_simple_rail_rx_cli_init(void)
{
  rx_fifo = sl_simple_rail_rx_get_rx_fifo();
}

/******************************************************************************
 * State machine for rail_rx_cli
 *****************************************************************************/
void sl_simple_rail_rx_cli_tick(void)
{
  // Forward RAIL rx messages if the flag is enabled
  if (rx_forward) {
    if (sl_rx_packet_copied) {
      app_log_info("Packet has been received: ");
      for (uint8_t i = 0; i < PAYLOAD_LENGTH; i++) {
        app_log_info("0x%02X, ", rx_fifo[i]);
      }
      app_log_info("\n");
      sl_rx_packet_copied = false;
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
