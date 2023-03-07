/***************************************************************************//**
 * @file
 * @brief NCP host application module without threading.
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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "uart.h"
#include "tcp.h"
#include "app_assert.h"
#include "app_log.h"
#include "sl_bt_ncp_host.h"
#include "ncp_host.h"

#if !(defined(POSIX) && POSIX == 1)
#include <windows.h>
#endif

// Default parameter values.
#define DEFAULT_UART_PORT             ""
#define DEFAULT_UART_BAUD_RATE        115200
#define DEFAULT_UART_FLOW_CONTROL     1
#define DEFAULT_UART_TIMEOUT          100
#define DEFAUKT_TCP_ADDRESS           ""
#define DEFAULT_TCP_PORT              "4901"
#define MAX_OPT_LEN                   255

#define IS_EMPTY_STRING(s)            ((s)[0] == '\0')
#define HANDLE_VALUE_MIN              0

// UART serial port options.
static char uart_port[MAX_OPT_LEN] = DEFAULT_UART_PORT;
static uint32_t uart_baud_rate = DEFAULT_UART_BAUD_RATE;
static uint32_t uart_flow_control = DEFAULT_UART_FLOW_CONTROL;

// TCP/IP address.
static char tcp_address[MAX_OPT_LEN] = DEFAUKT_TCP_ADDRESS;

#if defined(POSIX) && POSIX == 1
static int32_t handle = -1;
#else // defined(POSIX) && POSIX == 1
static HANDLE serial_handle;
static SOCKET socket_handle;
#endif // defined(POSIX) && POSIX == 1

static void *handle_ptr = NULL;
static int32_t (*tx_ptr)(void *handle, uint32_t len, uint8_t *data);
static int32_t (*rx_ptr)(void *handle, uint32_t len, uint8_t *data);
static int32_t (*peek_ptr)(void *handle);

static void ncp_host_tx(uint32_t len, uint8_t *data);
static int32_t ncp_host_rx(uint32_t len, uint8_t *data);
static int32_t ncp_host_peek(void);

/**************************************************************************//**
 * Initialize NCP connection.
 *****************************************************************************/
sl_status_t ncp_host_init(void)
{
  int32_t status;

  if (!IS_EMPTY_STRING(uart_port)) {
    // Initialise UART serial connection.
#if defined(POSIX) && POSIX == 1
    handle_ptr = &handle;
#else // defined(POSIX) && POSIX == 1
    handle_ptr = &serial_handle;
#endif // defined(POSIX) && POSIX == 1
    tx_ptr = uartTx;
    rx_ptr = uartRx;
    peek_ptr = uartRxPeek;
    status = uartOpen(handle_ptr, (int8_t *)uart_port, uart_baud_rate,
                      uart_flow_control, DEFAULT_UART_TIMEOUT);
    app_assert(status >= HANDLE_VALUE_MIN,
               "[E: %d] Failed to open UART serial connection"
               APP_LOG_NL, status);
  } else if (!IS_EMPTY_STRING(tcp_address)) {
    // Initialise TCP/IP connection.
#if defined(POSIX) && POSIX == 1
    handle_ptr = &handle;
#else // defined(POSIX) && POSIX == 1
    handle_ptr = &socket_handle;
#endif // defined(POSIX) && POSIX == 1
    tx_ptr = tcp_tx;
    rx_ptr = tcp_rx;
    peek_ptr = tcp_rx_peek;
    status = tcp_open(handle_ptr, tcp_address, DEFAULT_TCP_PORT);
    app_assert(status == HANDLE_VALUE_MIN,
               "[E: %d] Failed to open TCP/IP connection" APP_LOG_NL,
               status);
  } else {
    app_log_error("Either UART serial port or TCP/IP address is mandatory."
                  APP_LOG_NL);
    return SL_STATUS_INVALID_PARAMETER;
  }

  return sl_bt_api_initialize_nonblock(ncp_host_tx, ncp_host_rx, ncp_host_peek);
}

/**************************************************************************//**
 * Flushes data.
 *****************************************************************************/
sl_status_t ncp_host_flush_data(void)
{
  if (handle_ptr == NULL) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (!IS_EMPTY_STRING(uart_port)) {
    uartFlush(handle_ptr);
  }

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Set NCP connection options.
 *****************************************************************************/
sl_status_t ncp_host_set_option(char option, char *value)
{
  sl_status_t sc = SL_STATUS_OK;

  switch (option) {
    // TCP/IP address.
    case 't':
      strncpy(tcp_address, value, MAX_OPT_LEN);
      break;
    // UART serial port.
    case 'u':
      strncpy(uart_port, value, MAX_OPT_LEN);
      break;
    // UART baud rate.
    case 'b':
      uart_baud_rate = atol(value);
      break;
    // UART flow control disable.
    case 'f':
      uart_flow_control = 0;
      break;
    // Unknown option.
    default:
      sc = SL_STATUS_NOT_FOUND;
      break;
  }
  return sc;
}

/**************************************************************************//**
 * Deinitialize NCP connection.
 *****************************************************************************/
void ncp_host_deinit(void)
{
  if (!IS_EMPTY_STRING(uart_port)) {
    uartClose(handle_ptr);
  } else if (!IS_EMPTY_STRING(tcp_address)) {
    tcp_close(handle_ptr);
  }
}

/**************************************************************************//**
 * BGAPI TX wrapper.
 *****************************************************************************/
static void ncp_host_tx(uint32_t len, uint8_t* data)
{
  if (tx_ptr(handle_ptr, len, data) < 0) {
    ncp_host_deinit();
    app_assert(false, "Failed to write data\n");
  }
}

/**************************************************************************//**
 * BGAPI RX wrapper.
 *****************************************************************************/
static int32_t ncp_host_rx(uint32_t len, uint8_t* data)
{
  return rx_ptr(handle_ptr, len, data);
}

/**************************************************************************//**
 * BGAPI peek wrapper.
 *****************************************************************************/
static int32_t ncp_host_peek(void)
{
  int32_t sc;

  sc = peek_ptr(handle_ptr);
  if (sc < 0) {
    ncp_host_deinit();
    app_assert(false,
               "Peek is not supported in your environment, the program will hang.\n"
               "Please try other OS or environment.\n");
  }
  return sc;
}
