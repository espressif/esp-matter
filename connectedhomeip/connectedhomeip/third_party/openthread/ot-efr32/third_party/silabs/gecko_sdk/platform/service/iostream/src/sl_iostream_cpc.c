/***************************************************************************//**
 * @file
 * @brief IO Stream CPC Component.
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

#include "sl_atomic.h"
#include "sl_iostream_cpc.h"
#include "sl_iostream_cpc_config.h"
#include "sli_cpc.h"
#include "sl_sleeptimer.h"
#include "sl_assert.h"
#include "em_core.h"

#include <string.h>

/*******************************************************************************
 *********************************** DEFINES ***********************************
 ******************************************************************************/

#ifndef SL_IOSTREAM_CPC_TX_RETRY_TIMEOUT
#define SL_IOSTREAM_CPC_TX_RETRY_TIMEOUT 2
#endif

#ifndef SL_IOSTREAM_CPC_DISCONNECT_TIMEOUT
#define SL_IOSTREAM_CPC_DISCONNECT_TIMEOUT 500
#endif

/*******************************************************************************
 ********************************** TYPEDEFS ***********************************
 ******************************************************************************/

typedef struct {
  uint16_t idx;
  uint16_t cnt;
  uint8_t *buf;
} iostream_buffer_t;

static sl_sleeptimer_timer_handle_t write_retry_timer;
static sl_sleeptimer_timer_handle_t disconnect_timer;
static bool write_completed = true;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void cpc_on_write_retry(sl_sleeptimer_timer_handle_t *handle, void *data);

static void cpc_on_write_completed(sl_cpc_user_endpoint_id_t endpoint_id,
                                   void *buffer,
                                   void *arg,
                                   sl_status_t status);

static void on_error_timer_timeout(sl_sleeptimer_timer_handle_t *handle, void *data);

static void cpc_on_endpoint_error(uint8_t endpoint_id,
                                  void *arg);

static void cpc_flush(void *data);

static void on_error_reopen_endpoint(void *data);

static sl_status_t cpc_write(void *context,
                             const void *buffer,
                             size_t buffer_length);

static sl_status_t cpc_read(void *context,
                            void *buffer,
                            size_t buffer_length,
                            size_t *bytes_read);

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

static iostream_buffer_t tx_buf = { .buf = (uint8_t[(size_t)SL_IOSTREAM_CPC_TX_BUFFER_SIZE]){ 0 } };

static sl_cpc_dispatcher_handle_t sl_iostream_cpc_dispatcher_handle;

sl_cpc_endpoint_handle_t sl_iostream_cpc_endpoint_handle;

static sl_iostream_t sl_iostream_cpc = {
  .read = cpc_read,
  .write = cpc_write,
  .context = NULL
};

sl_iostream_t *sl_iostream_cpc_handle = &sl_iostream_cpc;

sl_iostream_instance_info_t sl_iostream_instance_cpc_info = {
  .handle = &sl_iostream_cpc,
  .name = "cpc",
  .type = SL_IOSTREAM_TYPE_UNDEFINED,
  .periph_id = 0,
  .init = sl_iostream_cpc_init
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * CPC Stream init
 ******************************************************************************/
sl_status_t sl_iostream_cpc_init(void)
{
  sl_status_t ret;

  sli_cpc_dispatcher_init_handle(&sl_iostream_cpc_dispatcher_handle);

  ret = sli_cpc_open_service_endpoint(&sl_iostream_cpc_endpoint_handle, SL_CPC_ENDPOINT_CLI, 0, 1);
  if (ret == SL_STATUS_OK) {
    ret = sl_cpc_set_endpoint_option(&sl_iostream_cpc_endpoint_handle, SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED, cpc_on_write_completed);
    if (ret == SL_STATUS_OK) {
      ret = sl_cpc_set_endpoint_option(&sl_iostream_cpc_endpoint_handle, SL_CPC_ENDPOINT_ON_ERROR, cpc_on_endpoint_error);
      if (ret == SL_STATUS_OK) {
        ret = sl_iostream_set_system_default(sl_iostream_cpc_handle);
        if (ret != SL_STATUS_OK) {
          return SL_STATUS_FAIL;
        }
        return SL_STATUS_OK;
      }
    }

    ret = sl_cpc_close_endpoint(&sl_iostream_cpc_endpoint_handle);
    EFM_ASSERT(ret == SL_STATUS_OK);
  }

  return ret;
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Internal CPC stream on write retry implementation
 ******************************************************************************/
static void cpc_on_write_retry(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  // Unused parameters.
  (void)handle;
  (void)data;

  // Retry.
  sli_cpc_dispatcher_push(&sl_iostream_cpc_dispatcher_handle, cpc_flush, NULL);

  return;
}

/***************************************************************************//**
 * Internal CPC stream on disconnect implementation
 ******************************************************************************/
static void on_error_timer_timeout(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  // Unused parameters.
  (void)handle;
  (void)data;

  // Retry.
  sli_cpc_dispatcher_push(&sl_iostream_cpc_dispatcher_handle, on_error_reopen_endpoint, NULL);
}

/***************************************************************************//**
 * Internal CPC stream on write completed implementation
 ******************************************************************************/
static void cpc_on_write_completed(sl_cpc_user_endpoint_id_t endpoint_id,
                                   void *buffer,
                                   void *arg,
                                   sl_status_t status)
{
  // Local variables.
  uint16_t cnt = (uintptr_t)arg;
  CORE_DECLARE_IRQ_STATE;

  // Unused parameters.
  (void)endpoint_id;
  (void)buffer;

  // Check if the transmit buffer was flushed successfully.
  CORE_ENTER_ATOMIC();
  if (status == SL_STATUS_OK) {
    tx_buf.idx = ((size_t)tx_buf.idx + (size_t)cnt) % (size_t)SL_IOSTREAM_CPC_TX_BUFFER_SIZE;
    cnt = tx_buf.cnt - cnt;
    tx_buf.cnt = cnt;
  }
  CORE_EXIT_ATOMIC();

  sl_atomic_store(write_completed, true);

  // Check if there's data to be flushed from the transmit buffer.
  if (cnt != 0) {
    sli_cpc_dispatcher_push(&sl_iostream_cpc_dispatcher_handle, cpc_flush, NULL);
  }

  return;
}

/***************************************************************************//**
 * Internal CPC stream on error implementation
 ******************************************************************************/
static void cpc_on_endpoint_error(uint8_t endpoint_id, void *arg)
{
  (void) arg;

  if (endpoint_id == SL_CPC_ENDPOINT_CLI) {
    // Invalidate pending message
    CORE_ATOMIC_SECTION(tx_buf.cnt = 0; );

    //iostream_endpoint_disconnected = true;
    sl_sleeptimer_stop_timer(&write_retry_timer);
    sl_sleeptimer_stop_timer(&disconnect_timer);
    on_error_reopen_endpoint(NULL);
  } else {
    EFM_ASSERT(0);
  }
}

/***************************************************************************//**
 * Internal CPC stream flush implementation
 ******************************************************************************/
static void cpc_flush(void *data)
{
  // Local variables.
  bool ready;
  uint16_t idx;
  uint16_t cnt;
  sl_status_t ret;
  CORE_DECLARE_IRQ_STATE;

  // Unused parameters.
  (void)data;

  sl_atomic_load(ready, write_completed);

  if (!ready) {
    // Retry later
    ret = sl_sleeptimer_start_timer_ms(&write_retry_timer, SL_IOSTREAM_CPC_TX_RETRY_TIMEOUT, cpc_on_write_retry, NULL, 0, 0);
    EFM_ASSERT(ret == SL_STATUS_OK);
    return;
  }

  if (sl_cpc_get_endpoint_state(&sl_iostream_cpc_endpoint_handle) != SL_CPC_STATE_OPEN) {
    return;
  }

  // Atomic copy.
  CORE_ENTER_ATOMIC();
  idx = tx_buf.idx;
  cnt = tx_buf.cnt;
  CORE_EXIT_ATOMIC();

  // Get the maximum number of bytes that can be flushed from the transmit buffer.
  // TODO: When available use the TX payload max length.
  cnt = MIN(cnt, SL_CPC_RX_PAYLOAD_MAX_LENGTH);
  cnt = MIN(cnt, (size_t)SL_IOSTREAM_CPC_TX_BUFFER_SIZE - idx);

  // Flush as many bytes as possible from the transmit buffer.
  ret = sl_cpc_write(&sl_iostream_cpc_endpoint_handle, tx_buf.buf + idx, cnt, 0, (void *)(uintptr_t)cnt);
  if (ret != SL_STATUS_OK) {
    ret = sl_sleeptimer_start_timer_ms(&write_retry_timer, SL_IOSTREAM_CPC_TX_RETRY_TIMEOUT, cpc_on_write_retry, NULL, 0, 0);
    EFM_ASSERT(ret == SL_STATUS_OK);
  }

  sl_atomic_store(write_completed, false);

  return;
}

/***************************************************************************//**
 * Internal CPC disconnect implementation
 ******************************************************************************/
static void on_error_reopen_endpoint(void *data)
{
  sl_status_t status;

  // Unused parameters.
  (void)data;

  status = sl_cpc_close_endpoint(&sl_iostream_cpc_endpoint_handle);
  //EFM_ASSERT(status == SL_STATUS_OK);

  if (sl_cpc_get_endpoint_state(&sl_iostream_cpc_endpoint_handle) != SL_CPC_STATE_FREED) {
    sl_sleeptimer_stop_timer(&disconnect_timer);
    status = sl_sleeptimer_start_timer_ms(&disconnect_timer, SL_IOSTREAM_CPC_DISCONNECT_TIMEOUT, on_error_timer_timeout, NULL, 0, 0);
    return; // Endpoint is still closing. Try again later.
  }

  sli_cpc_dispatcher_cancel(&sl_iostream_cpc_dispatcher_handle);

  status = sl_iostream_cpc_init();
  EFM_ASSERT(status == SL_STATUS_OK);
}

/***************************************************************************//**
 * Internal CPC stream write implementation
 ******************************************************************************/
static sl_status_t cpc_write(void *context,
                             const void *buffer,
                             size_t buffer_length)
{
  // Local variables.
  uint16_t idx;
  uint16_t cnt;
  CORE_DECLARE_IRQ_STATE;

  // Unused parameters.
  (void)context;

  // Atomic copy.
  CORE_ENTER_ATOMIC();
  idx = tx_buf.idx;
  cnt = tx_buf.cnt;
  CORE_EXIT_ATOMIC();

  // Check if there's something to write.
  if (buffer_length == 0) {
    return SL_STATUS_OK;
  }

  // Check if buffer is valid.
  if (buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Check if buffer_length is valid.
  if (buffer_length > (size_t)SL_IOSTREAM_CPC_TX_BUFFER_SIZE - cnt) {
    return SL_STATUS_MESSAGE_TOO_LONG;
  }

  // Add buffer's data to the transmit buffer.
  // The transmit buffer is circular, the new data is appended to the previous:
  // Index in the transmit buffer where to start appending.
  idx = ((size_t)idx + (size_t)cnt) % (size_t)SL_IOSTREAM_CPC_TX_BUFFER_SIZE;
  // Number of bytes that can be copied without wrapping around.
  cnt = MIN(buffer_length, (size_t)SL_IOSTREAM_CPC_TX_BUFFER_SIZE - idx);
  // Not all bytes can be copied without wrapping around?
  if (buffer_length != cnt) {
    // Copy remaining bytes at the beginning of the transmit buffer.
    memcpy(tx_buf.buf, buffer + cnt, buffer_length - cnt);
  }
  // Copy the bytes that could be copied without wrapping around.
  memcpy(tx_buf.buf + idx, buffer, cnt);
  // The transmit buffer could have been flushed, we were not in an atomic section.
  CORE_ENTER_ATOMIC();
  cnt = tx_buf.cnt;
  tx_buf.cnt += buffer_length;
  CORE_EXIT_ATOMIC();

  // Check if the transmit buffer was flushed entirely.
  if (cnt == 0) {
    sli_cpc_dispatcher_push(&sl_iostream_cpc_dispatcher_handle, cpc_flush, NULL);
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Internal CPC stream read implementation
 ******************************************************************************/
static sl_status_t cpc_read(void *context,
                            void *buffer,
                            size_t buffer_length,
                            size_t *bytes_read)
{
  // Local variables.
  static iostream_buffer_t rx_buf;
  sl_status_t ret = SL_STATUS_OK;
  uint16_t cpy;

  // Unused parameters.
  (void)context;

  // Check if bytes_read is valid.
  if (bytes_read == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  *bytes_read = 0;

  // Check if there's something to read.
  if (buffer_length == 0) {
    return SL_STATUS_OK;
  }

  // Check if buffer is valid.
  if (buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Was the last payload read entirely?
  if (rx_buf.cnt == 0) {
    rx_buf.idx = 0;
    ret = sl_cpc_read(&sl_iostream_cpc_endpoint_handle, (void **)&rx_buf.buf, &rx_buf.cnt, -1, 0);
  }

  // Until buffer is full or nothing to read.
  while (rx_buf.cnt != 0) {
    // Number of bytes from the payload that can be copied to buffer.
    cpy = MIN(buffer_length, rx_buf.cnt);

    // Copy those bytes.
    memcpy(buffer, rx_buf.buf + rx_buf.idx, cpy);
    buffer = (uint8_t *)buffer + cpy;
    buffer_length -= cpy;
    *bytes_read += cpy;

    rx_buf.idx += cpy;
    rx_buf.cnt -= cpy;

    // Has the payload been read entirely?
    if (rx_buf.cnt == 0) {
      ret = sl_cpc_free_rx_buffer(rx_buf.buf);
      EFM_ASSERT(ret == SL_STATUS_OK);

      // Any space left in buffer?
      if (buffer_length != 0) {
        rx_buf.idx = 0;
        sl_cpc_read(&sl_iostream_cpc_endpoint_handle, (void **)&rx_buf.buf, &rx_buf.cnt, 0, SL_CPC_FLAG_NO_BLOCK);
      }
    } else {
      return ret;
    }
  }

  return ret;
}
