/***************************************************************************//**
 * @file
 * @brief Host communication application module.
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

#ifndef HOST_COMM_H
#define HOST_COMM_H

#include "sl_status.h"

// Macros used by CPC
#if defined(POSIX) && POSIX == 1 && defined (CPC) && CPC == 1
#define HOST_COMM_CPC_OPTSTRING  "C:"

// Usage info.
#define HOST_COMM_CPC_USAGE " | -C <cpcd_instance_name>"

// Options info.
#define HOST_COMM_CPC_OPTIONS \
  "    -C  CPC connection\n"  \
  "        <cpcd_instance_name> Name of the CPCd instance to connect to.\n"
#else // defined(POSIX) && POSIX == 1 && defined (CPC) && CPC == 1
#define HOST_COMM_CPC_OPTSTRING
#define HOST_COMM_CPC_USAGE
#define HOST_COMM_CPC_OPTIONS
#endif // defined(POSIX) && POSIX == 1 && defined (CPC) && CPC == 1

// Macros used by Named Socket
#if defined(POSIX) && POSIX == 1
#define CLIENT_PATH "client_unencrypted"

// Optstring argument for getopt.
#define HOST_COMM_NS_OPTSTRING  "n:"

// Usage info.
#define HOST_COMM_NS_USAGE " | -n <server_socket>"

// Options info.
#define HOST_COMM_NS_OPTIONS                \
  "    -n  AF socket connection options.\n" \
  "        <server_socket>  Path to AF socket file descriptor\n"
#else // defined(POSIX) && POSIX == 1
#define HOST_COMM_NS_OPTSTRING
#define HOST_COMM_NS_USAGE
#define HOST_COMM_NS_OPTIONS
#endif // defined(POSIX) && POSIX == 1

// Optstring argument for getopt.
#define HOST_COMM_OPTSTRING  HOST_COMM_NS_OPTSTRING HOST_COMM_CPC_OPTSTRING "t:u:b:f"

// Usage info.
#define HOST_COMM_USAGE "-t <tcp_address> | -u <serial_port>" HOST_COMM_NS_USAGE HOST_COMM_CPC_USAGE " [-b <baud_rate>] [-f]"

// Options info.
#define HOST_COMM_OPTIONS                                                                                                      \
  "    -t  TCP/IP connection option.\n"                                                                                        \
  "        <tcp_address>    TCP/IP address of the dev board.\n"                                                                \
  "    -u  UART serial connection option.\n"                                                                                   \
  "        <serial_port>    Serial port assigned to the dev board by the host system. (COM# on Windows, /dev/tty# on POSIX)\n" \
  HOST_COMM_NS_OPTIONS HOST_COMM_CPC_OPTIONS                                                                                   \
  "    -b  Baud rate of the serial connection.\n"                                                                              \
  "        <baud_rate>      Baud rate, default: 115200\n"                                                                      \
  "    -f  Disable flow control (RTS/CTS), default: enabled\n"                                                                 \

#define HOST_COMM_API_DEFINE()                                            \
  int32_t (*host_comm_output)(void *handle, uint32_t len, uint8_t *data); \
  int32_t (*host_comm_input)(void *handle, uint32_t len, uint8_t *data);  \
  int32_t (*host_comm_pk)(void *handle);

/**
 * Initialize HOST_COMM_API to support nonblocking mode
 * @param OFUNC
 * @param IFUNC
 * @param PFUNC peek function to check if there is data to be read from UART
 */
#define HOST_COMM_API_INITIALIZE_NONBLOCK(OFUNC, IFUNC, PFUNC) host_comm_output = OFUNC; host_comm_input = IFUNC; host_comm_pk = PFUNC;

/**************************************************************************//**
 * Initialize low level connection.
 *
 * @retval SL_STATUS_OK if connection successful.
 * @return Error code otherwise.
 *****************************************************************************/
sl_status_t host_comm_init(void);

/**************************************************************************//**
 * Set low level host communication connection options.
 *
 * @param[in] option Option to set.
 * @param[in] value Value of the option.
 *
 * @retval SL_STATUS_OK Option set successfully.
 * @retval SL_STATUS_NOT_FOUND Unknown option.
 *****************************************************************************/
sl_status_t host_comm_set_option(char option, char *value);

/**************************************************************************//**
 * Deinitialize low level connection.
 *****************************************************************************/
void host_comm_deinit(void);

/**************************************************************************//**
 * Write data to NCP through low level drivers.
 *
 * @param[in] len Number of bytes to write.
 * @param[in] data Data to write.
 *
 * @return Number of written bytes, -1 on error.
 *****************************************************************************/
int32_t host_comm_tx(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Read data from NCP.
 *
 * @param[in] len Number of bytes to read.
 * @param[in] data Read data.
 *
 * @return Number of bytes read, -1 on error.
 *****************************************************************************/
int32_t host_comm_rx(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Peek if readable data exists.
 *
 * @return Number of bytes on the buffer, -1 on error.
 *****************************************************************************/
int32_t host_comm_peek(void);

#endif // HOST_COMM_H
