/***************************************************************************//**
 * @brief Radio Stream CLI commands.
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

#include "stack/include/ember.h"
#include "sl_cli.h"
#include "debug_print.h"
#include "radio-stream.h"

void start_tx_stream_command(sl_cli_command_arg_t *arguments)
{
  uint8_t mode = sl_cli_get_argument_uint8(arguments, 0);
  uint16_t channel = sl_cli_get_argument_uint16(arguments, 1);
  EmberStatus status = emberStartTxStream(mode, channel);

  if (status == EMBER_SUCCESS) {
    connect_app_debug_print("OK\n");
  } else {
    connect_app_debug_print("Failed to start Tx Stream %d\n", status);
  }
}

void stop_tx_stream_command(void)
{
  EmberStatus status = emberStopTxStream();

  if (status == EMBER_SUCCESS) {
    connect_app_debug_print("OK\n");
  } else {
    connect_app_debug_print("Failed to stop Tx Stream %d\n", status);
  }
}
