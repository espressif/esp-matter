/***************************************************************************//**
 * @file
 * @brief Application commandline interface handler
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
#include "sl_cli.h"
#include "sl_wisun_cli_core.h"
#include "sl_wisun_tcp_client.h"

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

/* CLI app tcp client create handler */
void app_tcp_client(sl_cli_command_arg_t *arguments)
{
  const char *remote_ip = NULL;
  uint16_t remote_port = 0;

  (void) arguments;

  app_wisun_cli_mutex_lock();
  remote_ip = sl_cli_get_argument_string(arguments, 0);
  remote_port = sl_cli_get_argument_uint16(arguments, 1);
  sl_wisun_tcp_client_create(remote_ip, remote_port);
  app_wisun_cli_mutex_unlock();
}

/* CLI app tcp client close */
void app_socket_close(sl_cli_command_arg_t *arguments)
{
  uint32_t sockid = 0;

  (void) arguments;

  app_wisun_cli_mutex_lock();
  sockid = sl_cli_get_argument_uint32(arguments, 0);
  sl_wisun_tcp_client_close((int32_t)sockid);
  app_wisun_cli_mutex_unlock();
}

/* CLI app tcp client write handler */
void app_socket_write(sl_cli_command_arg_t *arguments)
{
  uint32_t sockid = 0;
  const char *str;

  (void) arguments;

  app_wisun_cli_mutex_lock();
  sockid = sl_cli_get_argument_uint32(arguments, 0);
  str = sl_cli_get_argument_string(arguments, 1);
  sl_wisun_tcp_client_write(sockid, str);
  app_wisun_cli_mutex_unlock();
}

/* CLI app tcp client read handler */
void app_socket_read(sl_cli_command_arg_t *arguments)
{
  uint32_t sockid = 0;
  uint16_t size;

  (void) arguments;

  app_wisun_cli_mutex_lock();
  sockid = sl_cli_get_argument_uint32(arguments, 0);
  size = sl_cli_get_argument_uint16(arguments, 1);
  sl_wisun_tcp_client_read(sockid, size);
  app_wisun_cli_mutex_unlock();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
