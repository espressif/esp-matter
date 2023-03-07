/***************************************************************************//**
 * @brief Frequency Hopping commands.
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

void stack_set_fh_channel_mask_command(sl_cli_command_arg_t *arguments)
{
  size_t channelMaskLength;
  uint8_t *channelMask = sl_cli_get_argument_hex(arguments, 0, &channelMaskLength);

  EmberStatus status = emberFrequencyHoppingSetChannelMask((uint8_t)channelMaskLength, channelMask);

  if (status != EMBER_SUCCESS) {
    connect_app_debug_print("FH Channel Mask Failed, 0x%x\n", status);
  } else {
    connect_app_debug_print("FH Channel Mask Success\n");
  }
}

void stack_start_fh_server_command(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  EmberStatus status = emberFrequencyHoppingStartServer();

  if (status != EMBER_SUCCESS) {
    connect_app_debug_print("FH Server Failed, 0x%x\n", status);
  } else {
    connect_app_debug_print("FH Server Success\n");
  }
}

void stack_start_fh_client_command(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberNodeId nodeId;
  EmberPanId panId;

  nodeId = sl_cli_get_argument_uint16(arguments, 0);
  panId = sl_cli_get_argument_uint16(arguments, 1);

  status = emberFrequencyHoppingStartClient(nodeId, panId);

  if (status != EMBER_SUCCESS) {
    connect_app_debug_print("FH Client Failed, 0x%x\n", status);
  } else {
    connect_app_debug_print("FH Client Success\n");
  }
}

void stack_stop_fh_command(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  EmberStatus status = emberFrequencyHoppingStop();

  if (status != EMBER_SUCCESS) {
    connect_app_debug_print("FH Stop Failed, 0x%x\n", status);
  } else {
    connect_app_debug_print("FH Stopped\n");
  }
}
