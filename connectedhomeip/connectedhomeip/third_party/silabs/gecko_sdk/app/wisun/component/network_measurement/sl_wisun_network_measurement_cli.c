/***************************************************************************//**
 * @file
 * @brief
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include "socket_hnd.h"
#include "sl_cli.h"
#include "sl_wisun_app_core.h"
#include "sl_wisun_ping.h"
#include "sl_wisun_network_measurement.h"
#include "sl_wisun_network_measurement_config.h"
#include "sl_wisun_cli_core.h"
#include "sl_wisun_trace_util.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void app_measure(sl_cli_command_arg_t *arguments)
{
  const char *remote_ip_str = NULL;
  uint16_t meas_count = 0;
  uint16_t meas_packet_length = 0;
  wisun_addr_t remote_addr = { 0 };

  // get arguments
  remote_ip_str = sl_cli_get_argument_string(arguments, 0);
  meas_count = sl_cli_get_argument_uint16(arguments, 1);
  meas_packet_length = sl_cli_get_argument_uint16(arguments, 2);

  // parse message count
  if (!meas_count) {
    printf("[Measurement count is not valid]\n");
    return;
  }

  // parse message count
  if (!meas_packet_length) {
    printf("[Measurement packet length is not valid]\n");
    return;
  }

  // parse measurement type: quick or custom
  if (!strcmp(remote_ip_str, "all")) {
    sl_wisun_nwm_quick_measure(SL_WISUN_NWM_TARGET_TYPE_ALL, meas_count, meas_packet_length);
  } else if (!strcmp(remote_ip_str, "parent")) {
    sl_wisun_nwm_quick_measure(SL_WISUN_NWM_TARGET_TYPE_PARENT, meas_count, meas_packet_length);
  } else if (!strcmp(remote_ip_str, "br")) {
    sl_wisun_nwm_quick_measure(SL_WISUN_NWM_TARGET_TYPE_BORDER_ROUTER, meas_count, meas_packet_length);
  } else {
    if (inet_pton(AF_WISUN, remote_ip_str, &remote_addr.sin6_addr) == RETVAL_ERROR) {
      printf("[IP address is not set]\n");
      return;
    }
    sl_wisun_nwm_measure(&remote_addr, meas_count, meas_packet_length, false);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
