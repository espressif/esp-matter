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
#include <stdio.h>
#include "sl_cli.h"
#include "sl_wisun_cli_core.h"
#include "sl_wisun_ping.h"
#include "sl_wisun_ping_config.h"
#include "socket_hnd.h"
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
/* CLI app ping */
void app_ping(sl_cli_command_arg_t *arguments)
{
  const char *remote_ip = NULL;
  wisun_addr_t remote_addr;

  app_wisun_cli_mutex_lock();

  remote_ip = sl_cli_get_argument_string(arguments, 0);
  remote_addr.sin6_family = AF_WISUN;
  if (inet_pton(AF_WISUN, remote_ip, &remote_addr.sin6_addr) == RETVAL_ERROR ) {
    printf("[Failed: invalid remote address parameter]\r\n");
  }
  if (!sl_wisun_ping(&remote_addr,
                     SL_WISUN_PING_PACKET_COUNT,
                     SL_WISUN_PING_PACKET_SIZE,
                     NULL, NULL)) {
    printf("[Failed: ping was not successful]\n");
  }

  app_wisun_cli_mutex_unlock();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
