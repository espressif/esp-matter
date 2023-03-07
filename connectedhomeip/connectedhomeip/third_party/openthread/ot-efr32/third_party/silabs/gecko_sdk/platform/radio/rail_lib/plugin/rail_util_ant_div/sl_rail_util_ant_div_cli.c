/***************************************************************************//**
 * @file
 * @brief sl_rail_util_ant_div_cli.c
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_cli.h"
#include "response_print.h"
#include "sl_rail_util_ant_div.h"

void cli_ant_div_set_tx_antenna_mode(sl_cli_command_arg_t *args)
{
  sl_rail_util_antenna_mode_t mode = (sl_rail_util_antenna_mode_t)sl_cli_get_argument_uint8(args, 0);
  sl_status_t status = sl_rail_util_ant_div_set_tx_antenna_mode(mode);
  responsePrint(sl_cli_get_command_string(args, 0), "Status:0x%x", status);
}

void cli_ant_div_get_tx_antenna_mode(sl_cli_command_arg_t *args)
{
  sl_rail_util_antenna_mode_t mode = sl_rail_util_ant_div_get_tx_antenna_mode();
  responsePrint(sl_cli_get_command_string(args, 0), "TxAntennaMode:%d", mode);
}

void cli_ant_div_get_tx_antenna_selected(sl_cli_command_arg_t *args)
{
  sl_rail_util_antenna_selection_t antenna = sl_rail_util_ant_div_get_tx_antenna_selected();
  responsePrint(sl_cli_get_command_string(args, 0), "SelectedAntenna:%d", antenna);
}

void cli_ant_div_toggle_tx_antenna(sl_cli_command_arg_t *args)
{
  sl_status_t status = sl_rail_util_ant_div_toggle_tx_antenna();
  responsePrint(sl_cli_get_command_string(args, 0), "Status:0x%x", status);
}

void cli_ant_div_set_rx_antenna_mode(sl_cli_command_arg_t *args)
{
  sl_rail_util_antenna_mode_t mode = (sl_rail_util_antenna_mode_t)sl_cli_get_argument_uint8(args, 0);
  sl_status_t status = sl_rail_util_ant_div_set_rx_antenna_mode(mode);
  responsePrint(sl_cli_get_command_string(args, 0), "Status:0x%x", status);
}

void cli_ant_div_get_rx_antenna_mode(sl_cli_command_arg_t *args)
{
  sl_rail_util_antenna_mode_t mode = sl_rail_util_ant_div_get_rx_antenna_mode();
  responsePrint(sl_cli_get_command_string(args, 0), "RxAntennaMode:%d", mode);
}

void cli_ant_div_get_phy_select(sl_cli_command_arg_t *args)
{
  bool phySelect = sl_rail_util_ant_div_get_phy_select();
  responsePrint(sl_cli_get_command_string(args, 0), "AntDivPhySelected:%d", phySelect);
}
