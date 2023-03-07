/***************************************************************************//**
 * @file
 * @brief CPC communication through UART on POSIX platform
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_cpc.h"
#include "app_log.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <pty.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define RETRY_COUNT         10
#define CPC_RETRY_SLEEP_NS  100000000L
#define CPC_RESET_SLEEP_NS  10000L
#define CPC_TRANSMIT_WINDOW 1
#define FROM_CPC_BUF_SIZE   SL_CPC_READ_MINIMUM_SIZE

// cpc related structures
static cpc_handle_t lib_handle;
static cpc_endpoint_t endpoint;

// temporary rx buffer
typedef struct {
  int32_t len;
  uint8_t buf[FROM_CPC_BUF_SIZE];
} buf_t;

static buf_t buf_rx = { 0 };

static uint8_t handshake_msg[4] = { 0x20, 0x00, 0x01, 0x00 };

// end the receiving loop if signal is received.
static volatile bool run = true;
// signal if the controller was reset
static volatile bool has_reset = false;

// two worker threads
static pthread_t sv;

// Static receive function
static void *supervisor(void *ptr);

// -----------------------------------------------------------------------------
// Private Function Declaration

static void reset_callback(void);

// -----------------------------------------------------------------------------
// Public Function Definitions

int32_t cpc_open(void *handle, char *cpc_instance)
{
  int ret;
  uint8_t retry = 0;

  // Initialize CPC communication
  do {
    ret = cpc_init(&lib_handle, cpc_instance, false, reset_callback);
    if (ret == 0) {
      // speed up boot process if everything seems ok
      break;
    }
    nanosleep((const struct timespec[]){{ 0, CPC_RETRY_SLEEP_NS } }, NULL);
    retry++;
  } while ((ret != 0) && (retry < RETRY_COUNT));

  if (ret < 0) {
    perror("cpc_init: ");
    return ret;
  }

  // Start Bluetooth endpoint
  ret = cpc_open_endpoint(lib_handle,
                          &endpoint,
                          SL_CPC_ENDPOINT_BLUETOOTH,
                          CPC_TRANSMIT_WINDOW);
  if (ret < 0) {
    perror("cpc_open_endpoint ");
    return ret;
  }

  // Create supervisory thread
  ret = pthread_create(&sv, NULL, supervisor, NULL);
  if (ret) {
    perror("Couldn't create thread ");
    return ret;
  }

  handle = endpoint.ptr;

  // Send handshake msg
  (void)cpc_write_endpoint(endpoint, &handshake_msg[0], 4, 0);

  // Discard response
  (void)cpc_read_endpoint(endpoint, &buf_rx.buf, FROM_CPC_BUF_SIZE, 0);
  buf_rx.len = 0;
  memset(buf_rx.buf, 0, FROM_CPC_BUF_SIZE);

  return ret;
}

int32_t cpc_tx(void *handle, uint32_t data_length, uint8_t *data)
{
  (void)handle;
  ssize_t size = 0;

  if (!has_reset) {
    size = cpc_write_endpoint(endpoint, &data[0], data_length, 0);
  } else {
    // In case of reset we don't care if send was succesfull or not
    size = data_length;
  }

  return (int32_t)size;
}

int32_t cpc_rx(void *handle, uint32_t data_length, uint8_t *data)
{
  (void)handle;

  if (buf_rx.len > 0) {
    memcpy(data, buf_rx.buf, buf_rx.len);
  }

  return buf_rx.len;
}

int32_t cpc_rx_peek(void *handle)
{
  (void)handle;

  if (!has_reset) {
    // Make read blocking - possible because threaded structure in host_comm
    buf_rx.len = (int32_t)cpc_read_endpoint(endpoint, &buf_rx.buf,
                                            FROM_CPC_BUF_SIZE, 0);
  } else {
    // If in reset, don't try to read
    buf_rx.len = 0;
  }
  if (buf_rx.len < 0) {
    buf_rx.len = 0;
  }
  return buf_rx.len;
}

int32_t cpc_close(void *handle)
{
  return cpc_close_endpoint(&endpoint);
}

// -----------------------------------------------------------------------------
// Public Function Definitions

/**************************************************************************//**
 * Callback to register reset from other end.
 *****************************************************************************/
static void reset_callback(void)
{
  has_reset = true;
}

/**************************************************************************//**
 * Reset CPC communication after other end restarted.
 *****************************************************************************/
int reset_cpc(void)
{
  int ret;
  uint8_t retry = 0;

  app_log_debug("RESET" APP_LOG_NL);

  // Restart cpp communication
  do {
    ret = cpc_restart(&lib_handle);
    if (ret == 0) {
      // speed up boot process if everything seems ok
      break;
    }
    nanosleep((const struct timespec[]){{ 0, CPC_RETRY_SLEEP_NS } }, NULL);
    retry++;
  } while ((ret != 0) && (retry < RETRY_COUNT));

  if (ret < 0) {
    perror("cpc restart ");
    return ret;
  }

  // Open Bluetooth endpoint
  ret = cpc_open_endpoint(lib_handle,
                          &endpoint,
                          SL_CPC_ENDPOINT_BLUETOOTH,
                          CPC_TRANSMIT_WINDOW);
  if (ret < 0) {
    perror(" open endpoint ");
    return ret;
  }

  // Send handshake msg, but don't discard the answer, as upper layers need it.
  cpc_write_endpoint(endpoint, &handshake_msg[0], 4, 0);
  has_reset = false;
  return ret;
}

/**************************************************************************//**
 * Supervisor thread
 *****************************************************************************/
void *supervisor(void *ptr)
{
  // unused variable
  (void)ptr;
  int ret;

  while (run) {
    if (has_reset) {
      ret = reset_cpc();
      if (ret < 0) {
        perror("reset ");
        // better to die here than continue to work falsely
        exit(EXIT_FAILURE);
      }
    }
    nanosleep((const struct timespec[]){{ 0, CPC_RESET_SLEEP_NS } }, NULL);
  }
  return NULL;
}
