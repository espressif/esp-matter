/***************************************************************************//**
 * @file
 * @brief NCP host application module.
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

#ifndef NCP_HOST_H
#define NCP_HOST_H

#include "sl_status.h"

// Optstring argument for getopt.
#define NCP_HOST_OPTSTRING "t:u:b:f"

// Usage info.
#define NCP_HOST_USAGE "-t <tcp_address> | -u <serial_port> [-b <baud_rate>] [-f]"

// Options info.
#define NCP_HOST_OPTIONS                                                                                                       \
  "    -t  TCP/IP connection option.\n"                                                                                        \
  "        <tcp_address>    TCP/IP address of the dev board.\n"                                                                \
  "    -u  UART serial connection option.\n"                                                                                   \
  "        <serial_port>    Serial port assigned to the dev board by the host system. (COM# on Windows, /dev/tty# on POSIX)\n" \
  "    -b  Baud rate of the serial connection.\n"                                                                              \
  "        <baud_rate>      Baud rate, default: 115200\n"                                                                      \
  "    -f  Disable flow control (RTS/CTS), default: enabled\n"

/**************************************************************************//**
 * Initialize NCP connection.
 *
 * @retval SL_STATUS_OK Connection successful.
 * @retval SL_STATUS_INVALID_PARAMETER Invalid options.
 *****************************************************************************/
sl_status_t ncp_host_init(void);

/**************************************************************************//**
 * Flushes RX buffer.
 *
 * @retval SL_STATUS_OK RX buffer flushed.
 * @retval SL_STATUS_NOT_INITIALIZED Module is not initialized.
 *****************************************************************************/
sl_status_t ncp_host_flush_data(void);

/**************************************************************************//**
 * Set NCP connection options.
 *
 * @param[in] option Option to set.
 * @param[in] value Value of the option.
 *
 * @retval SL_STATUS_OK Option set successfully.
 * @retval SL_STATUS_NOT_FOUND Unknown option.
 *****************************************************************************/
sl_status_t ncp_host_set_option(char option, char *value);

/**************************************************************************//**
 * Deinitialize NCP connection.
 *****************************************************************************/
void ncp_host_deinit(void);

#endif // NCP_HOST_H
