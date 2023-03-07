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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "uart.h"
#include "tcp.h"
#include "app_assert.h"
#include "app_log.h"
#include "host_comm.h"
#include "app_sleep.h"
#include "host_comm_config.h"

#if defined(POSIX) && POSIX == 1
#include "named_socket.h"
#if defined (CPC) && CPC == 1
#include "cpc.h"
#endif // defined (CPC) && CPC == 1
#else
#include <windows.h>
#endif // defined(POSIX) && POSIX == 1

// Default parameter values.
#define DEFAULT_UART_PORT             ""
#define DEFAULT_UART_BAUD_RATE        115200
#define DEFAULT_UART_FLOW_CONTROL     1
#define DEFAULT_UART_TIMEOUT          100
#define DEFAULT_TCP_ADDRESS           ""
#define DEFAULT_TCP_PORT              "4901"
#define MAX_OPT_LEN                   255
#define DEFAULT_CPC_INST_NAME         "cpcd_0"

#define IS_EMPTY_STRING(s)            ((s)[0] == '\0')
#define HANDLE_VALUE_MIN              0

// Define global HOST_COMM_API_DEFINE library.
HOST_COMM_API_DEFINE();

typedef struct {
  uint16_t len;
  uint8_t buf[DEFAULT_HOST_BUFLEN];
} buf_t;

static buf_t buf_tmp = { 0 };
static buf_t buf_in = { 0 };

// end the receiving loop if signol is received.
static volatile bool run = true;

// UART serial port options.
static char uart_port[MAX_OPT_LEN] = DEFAULT_UART_PORT;
static uint32_t uart_baud_rate = DEFAULT_UART_BAUD_RATE;
static uint32_t uart_flow_control = DEFAULT_UART_FLOW_CONTROL;

// TCP/IP address.
static char tcp_address[MAX_OPT_LEN] = DEFAULT_TCP_ADDRESS;

#if defined(POSIX) && POSIX == 1
// AF socket descriptor path
static char named_socket_target_address[MAX_OPT_LEN];
#if defined (CPC) && CPC == 1
// CPCd instance name.
static char cpc_instance_name[MAX_OPT_LEN] = DEFAULT_CPC_INST_NAME;

// CPC connection
static bool cpc_conn = false;
#endif // defined (CPC) && CPC == 1

#endif // defined(POSIX) && POSIX == 1

#if defined(POSIX) && POSIX == 1
int32_t handle = -1;
#else // defined(POSIX) && POSIX == 1
HANDLE serial_handle;
SOCKET socket_handle;
#endif // defined(POSIX) && POSIX == 1
void *handle_ptr;

// Static receive function
void *msg_recv_func(void *ptr);

pthread_t thread_rx;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**************************************************************************//**
 * Initialize low level connection.
 *****************************************************************************/
sl_status_t host_comm_init(void)
{
  int32_t status;
  int iret;

  if (!IS_EMPTY_STRING(uart_port)) {
    // Initialise UART serial connection.
#if defined(POSIX) && POSIX == 1
    handle_ptr = &handle;
#else // defined(POSIX) && POSIX == 1
    handle_ptr = &serial_handle;
#endif // defined(POSIX) && POSIX == 1
    HOST_COMM_API_INITIALIZE_NONBLOCK(uartTx, uartRx, uartRxPeek);
    status = uartOpen(handle_ptr, (int8_t *)uart_port, uart_baud_rate,
                      uart_flow_control, DEFAULT_UART_TIMEOUT);
    app_assert(status >= HANDLE_VALUE_MIN,
               "[E: %d] Failed to open UART serial connection"
               APP_LOG_NL, status);
    uartFlush(handle_ptr);
  } else if (!IS_EMPTY_STRING(tcp_address)) {
    // Initialise TCP/IP connection.
#if defined(POSIX) && POSIX == 1
    handle_ptr = &handle;
#else // defined(POSIX) && POSIX == 1
    handle_ptr = &socket_handle;
#endif // defined(POSIX) && POSIX == 1
    HOST_COMM_API_INITIALIZE_NONBLOCK(tcp_tx, tcp_rx, tcp_rx_peek);
    status = tcp_open(handle_ptr, tcp_address, DEFAULT_TCP_PORT);
    app_assert(status == HANDLE_VALUE_MIN,
               "[E: %d] Failed to open TCP/IP connection" APP_LOG_NL,
               status);
#if defined(POSIX) && POSIX == 1
  } else if (named_socket_target_address[0] != '\0') {
    handle_ptr = &handle;
    // Initialise serial communication as non-blocking.
    HOST_COMM_API_INITIALIZE_NONBLOCK(tcp_tx, tcp_rx, tcp_rx_peek);
    if (connect_domain_socket_server(handle_ptr, named_socket_target_address,
                                     CLIENT_PATH, 0)) {
      app_log_critical("Connection to domain socket unsuccessful. Exiting.." APP_LOG_NL);
      exit(EXIT_FAILURE);
    }
#if defined (CPC) && CPC == 1
  } else if (cpc_conn) {
    handle_ptr = &handle;
    HOST_COMM_API_INITIALIZE_NONBLOCK(cpc_tx, cpc_rx, cpc_rx_peek);
    if (cpc_open(handle_ptr, cpc_instance_name)) {
      app_log_critical("Connection to CPCd unsuccessful. Exiting.." APP_LOG_NL);
      exit(EXIT_FAILURE);
    }
#endif // defined (CPC) && CPC == 1
#endif // defined(POSIX) && POSIX == 1
  } else {
    app_log_error("Either UART serial port or TCP/IP address is mandatory."
                  APP_LOG_NL);
    return SL_STATUS_INVALID_PARAMETER;
  }

  iret = pthread_create(&thread_rx, NULL, msg_recv_func, NULL);
  if (iret) {
    app_log_error("Error - pthread_create() return code: %d" APP_LOG_NL, iret);
    return SL_STATUS_FAIL;
  }

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Set low level host communication connection options.
 *****************************************************************************/
sl_status_t host_comm_set_option(char option, char *value)
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
      uart_flow_control = 1;
      break;
#if defined(POSIX) && POSIX == 1
    // AF socket descriptor
    case 'n':
      strncpy(named_socket_target_address, value, MAX_OPT_LEN);
      break;
#if defined (CPC) && CPC == 1
    // CPC connection
    case 'C':
      strncpy(cpc_instance_name, value, MAX_OPT_LEN);
      cpc_conn = true;
      break;
#endif // defined (CPC) && CPC == 1
#endif // defined(POSIX) && POSIX == 1
    // Unknown option.
    default:
      sc = SL_STATUS_NOT_FOUND;
      break;
  }
  return sc;
}

/**************************************************************************//**
 * Deinitialize low level connection.
 *****************************************************************************/
void host_comm_deinit(void)
{
  run = false;
  pthread_cancel(thread_rx);

  if (!IS_EMPTY_STRING(uart_port)) {
    uartClose(handle_ptr);
  } else if (!IS_EMPTY_STRING(tcp_address)) {
    tcp_close(handle_ptr);
  }
}

/**************************************************************************//**
 * Write data to NCP through low level drivers.
 *****************************************************************************/
int32_t host_comm_tx(uint32_t len, uint8_t* data)
{
  return host_comm_output(handle_ptr, len, data);
}

/**************************************************************************//**
 * Read data from NCP.
 *****************************************************************************/
int32_t host_comm_rx(uint32_t len, uint8_t* data)
{
  int32_t ret = -1;

  pthread_mutex_lock(&mutex);
  if (buf_in.len >= len) {
    memcpy(data, &buf_in.buf[0], len);
    buf_in.len -= len;
    memmove(buf_in.buf, &buf_in.buf[len], buf_in.len);
    ret = len;
  }
  pthread_mutex_unlock(&mutex);

  return ret;
}

/**************************************************************************//**
 * Peek if readable data exists.
 *****************************************************************************/
int32_t host_comm_peek(void)
{
  int32_t len = 0;
  pthread_mutex_lock(&mutex);
  len = buf_in.len;
  pthread_mutex_unlock(&mutex);
  return len;
}

/**************************************************************************//**
 * Read data from low level drivers.
 *****************************************************************************/
void *msg_recv_func(void *ptr)
{
  int32_t ret;

  // unused variable
  (void)ptr;

  while (run) {
    int32_t len;
    len = host_comm_pk(handle_ptr);
    if (len < 0) {
      // Peek is not supported, read data one by one
      len = 1;
    }
    if (len > sizeof(buf_in.buf)) {
      // If readable data exceeds the buffer size then
      // read it one by one to avoid overflow
      len = 1;
      app_log_warning("Input buffer size too low, please increase it." APP_LOG_NL);
    }
    if (len > 0) {
      memset(&buf_tmp, 0, sizeof(buf_tmp));
      ret = host_comm_input(handle_ptr, len, buf_tmp.buf);
      if (ret <= (sizeof(buf_in.buf) - buf_in.len)) {
        pthread_mutex_lock(&mutex);
        memcpy(&buf_in.buf[buf_in.len], &buf_tmp.buf[0], ret);
        buf_in.len += ret;
        pthread_mutex_unlock(&mutex);
      }
    }
    app_sleep_us(RECV_FUNC_US_SLEEP);
  }
  return 0;
}
