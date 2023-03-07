/***************************************************************************/ /**
 * @file
 * @brief CPC API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdlib.h>
#include <stddef.h>

#include "sl_status.h"

#include "sl_cpc_config.h"
#include "sli_cpc_system_common.h"
#include "sli_cpc.h"
#include "sli_cpc_hdlc.h"
#include "sli_mem_pool.h"
#include "sli_cpc_debug.h"
#include "sli_cpc_drv.h"
#include "sl_assert.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

#if (SL_CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS == 1)
#define MEM_POOL_ALLOC(mempool, block) { if (block != NULL) \
                                         { mempool->used_block_cnt++; } }

#define MEM_POOL_FREE(mempool) mempool->used_block_cnt--
#else
#define MEM_POOL_ALLOC(mempool, block)
#define MEM_POOL_FREE(mempool)
#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static sli_mem_pool_handle_t mempool_buffer_handle;
SLI_MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_buffer_handle,
                                      sl_cpc_buffer_handle_t,
                                      SLI_CPC_BUFFER_HANDLE_MAX_COUNT);

static sl_cpc_mem_pool_handle_t cpc_mempool_buffer_handle =
{ .pool_handle = &mempool_buffer_handle,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_hdlc_header;
SLI_MEM_POOL_DECLARE_BUFFER(mempool_hdlc_header,
                            SLI_CPC_HDLC_HEADER_RAW_SIZE,
                            SLI_CPC_HDLC_HEADER_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_hdlc_header =
{ .pool_handle = &mempool_hdlc_header,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_hdlc_reject;
SLI_MEM_POOL_DECLARE_BUFFER(mempool_hdlc_reject,
                            SLI_CPC_HDLC_REJECT_PAYLOAD_SIZE,
                            SLI_CPC_HDLC_REJECT_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_hdlc_reject =
{ .pool_handle = &mempool_hdlc_reject,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_rx_buffer;
SLI_MEM_POOL_DECLARE_BUFFER(mempool_rx_buffer,
                            SLI_CPC_RX_DATA_MAX_LENGTH,
                            SL_CPC_RX_BUFFER_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_rx_buffer =
{ .pool_handle = &mempool_rx_buffer,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_endpoint;
SLI_MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_endpoint,
                                      sl_cpc_endpoint_t,
                                      SLI_CPC_ENDPOINT_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_endpoint =
{ .pool_handle = &mempool_endpoint,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_rx_queue_item;
SLI_MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_rx_queue_item,
                                      sl_cpc_receive_queue_item_t,
                                      SLI_CPC_RX_QUEUE_ITEM_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_rx_queue_item =
{ .pool_handle = &mempool_rx_queue_item,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_tx_queue_item;
SLI_MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_tx_queue_item,
                                      sl_cpc_transmit_queue_item_t,
                                      SL_CPC_TX_QUEUE_ITEM_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_tx_queue_item =
{ .pool_handle = &mempool_tx_queue_item,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_tx_queue_item_sframe;
SLI_MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_tx_queue_item_sframe,
                                      sl_cpc_transmit_queue_item_t,
                                      SLI_CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_tx_queue_item_sframe =
{ .pool_handle = &mempool_tx_queue_item_sframe,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_endpoint_closed_arg_item;
SLI_MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_endpoint_closed_arg_item,
                                      sl_cpc_endpoint_closed_arg_t,
                                      SL_CPC_TX_QUEUE_ITEM_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_endpoint_closed_arg_item =
{ .pool_handle = &mempool_endpoint_closed_arg_item,
  .used_block_cnt = 0 };

static sli_mem_pool_handle_t mempool_system_command;
SLI_MEM_POOL_DECLARE_BUFFER(mempool_system_command,
                            SLI_CPC_SYSTEM_COMMAND_BUFFER_SIZE,
                            SLI_CPC_SYSTEM_COMMAND_BUFFER_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_system_command =
{ .pool_handle = &mempool_system_command,
  .used_block_cnt = 0 };

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
static sli_mem_pool_handle_t mempool_tx_security_tag;
SLI_MEM_POOL_DECLARE_BUFFER(mempool_tx_security_tag,
                            SLI_SECURITY_TAG_LENGTH_BYTES,
                            SL_CPC_TX_QUEUE_ITEM_MAX_COUNT);
static sl_cpc_mem_pool_handle_t cpc_mempool_tx_security_tag =
{ .pool_handle = &mempool_tx_security_tag,
  .used_block_cnt = 0 };
#endif

extern sli_cpc_drv_capabilities_t sli_cpc_driver_capabilities;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

static void* alloc_object(sl_cpc_mem_pool_handle_t *pool);

static void free_object(sl_cpc_mem_pool_handle_t *pool,
                        void *block);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

__WEAK void sli_cpc_memory_on_rx_buffer_free(void);

/***************************************************************************//**
 * Initialize CPC buffers' handling module.
 ******************************************************************************/
void sli_cpc_init_buffers(void)
{
  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_buffer_handle.pool_handle,
                      sizeof(sl_cpc_buffer_handle_t),
                      SLI_CPC_BUFFER_HANDLE_MAX_COUNT,
                      mempool_buffer_handle_buffer,
                      sizeof(mempool_buffer_handle_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_hdlc_header.pool_handle,
                      SLI_CPC_HDLC_HEADER_RAW_SIZE,
                      SLI_CPC_HDLC_HEADER_MAX_COUNT,
                      mempool_hdlc_header_buffer,
                      sizeof(mempool_hdlc_header_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_hdlc_reject.pool_handle,
                      SLI_CPC_HDLC_REJECT_PAYLOAD_SIZE,
                      SLI_CPC_HDLC_REJECT_MAX_COUNT,
                      mempool_hdlc_reject_buffer,
                      sizeof(mempool_hdlc_reject_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_rx_buffer.pool_handle,
                      SLI_CPC_RX_DATA_MAX_LENGTH,
                      SL_CPC_RX_BUFFER_MAX_COUNT,
                      mempool_rx_buffer_buffer,
                      sizeof(mempool_rx_buffer_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_endpoint.pool_handle,
                      sizeof(sl_cpc_endpoint_t),
                      SLI_CPC_ENDPOINT_COUNT,
                      mempool_endpoint_buffer,
                      sizeof(mempool_endpoint_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_rx_queue_item.pool_handle,
                      sizeof(sl_cpc_receive_queue_item_t),
                      SLI_CPC_RX_QUEUE_ITEM_MAX_COUNT,
                      mempool_rx_queue_item_buffer,
                      sizeof(mempool_rx_queue_item_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_tx_queue_item.pool_handle,
                      sizeof(sl_cpc_transmit_queue_item_t),
                      SL_CPC_TX_QUEUE_ITEM_MAX_COUNT,
                      mempool_tx_queue_item_buffer,
                      sizeof(mempool_tx_queue_item_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_tx_queue_item_sframe.pool_handle,
                      sizeof(sl_cpc_transmit_queue_item_t),
                      SLI_CPC_TX_QUEUE_ITEM_SFRAME_MAX_COUNT,
                      mempool_tx_queue_item_sframe_buffer,
                      sizeof(mempool_tx_queue_item_sframe_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_endpoint_closed_arg_item.pool_handle,
                      sizeof(sl_cpc_endpoint_closed_arg_t),
                      SL_CPC_TX_QUEUE_ITEM_MAX_COUNT,
                      mempool_endpoint_closed_arg_item_buffer,
                      sizeof(mempool_endpoint_closed_arg_item_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_endpoint_closed_arg_item.pool_handle,
                      sizeof(sl_cpc_endpoint_closed_arg_t),
                      SL_CPC_TX_QUEUE_ITEM_MAX_COUNT,
                      mempool_endpoint_closed_arg_item_buffer,
                      sizeof(mempool_endpoint_closed_arg_item_buffer));

  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_system_command.pool_handle,
                      SLI_CPC_SYSTEM_COMMAND_BUFFER_SIZE,
                      SLI_CPC_SYSTEM_COMMAND_BUFFER_COUNT,
                      mempool_system_command_buffer,
                      sizeof(mempool_system_command_buffer));

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  sli_mem_pool_create((sli_mem_pool_handle_t *)cpc_mempool_tx_security_tag.pool_handle,
                      SLI_SECURITY_TAG_LENGTH_BYTES,
                      SL_CPC_TX_QUEUE_ITEM_MAX_COUNT,
                      mempool_tx_security_tag_buffer,
                      sizeof(mempool_tx_security_tag_buffer));
#endif

  SLI_CPC_DEBUG_MEMORY_POOL_INIT();
}

/***************************************************************************//**
 * Get a CPC handle buffer
 ******************************************************************************/
sl_status_t sli_cpc_get_buffer_handle(sl_cpc_buffer_handle_t **handle)
{
  sl_cpc_buffer_handle_t *buf_handle;

  if (handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  buf_handle = alloc_object(&cpc_mempool_buffer_handle);
  if (buf_handle == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  buf_handle->hdlc_header = NULL;
  buf_handle->data = NULL;
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  buf_handle->security_tag = NULL;
#endif
  buf_handle->data_length = 0;
  buf_handle->endpoint = NULL;
  buf_handle->fcs[0] = 0;
  buf_handle->fcs[1] = 0;
  buf_handle->control = 0;
  buf_handle->address = 0;
  buf_handle->buffer_type = SL_CPC_UNKNOWN_BUFFER;
  buf_handle->arg = NULL;
  buf_handle->reason = SL_CPC_REJECT_NO_ERROR;
  buf_handle->ref_count = 0;

  *handle = buf_handle;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get a CPC header buffer
 ******************************************************************************/
sl_status_t sli_cpc_get_hdlc_header_buffer(void **header)
{
  if (header == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *header = alloc_object(&cpc_mempool_hdlc_header);
  if (*header == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get a CPC header buffer for transmitting a reject
 ******************************************************************************/
sl_status_t sli_cpc_get_reject_buffer(sl_cpc_buffer_handle_t **handle)
{
  sl_status_t status;
  sl_cpc_buffer_handle_t *buf_handle;

  status = sli_cpc_get_buffer_handle(&buf_handle);
  if (status != SL_STATUS_OK) {
    return status;
  }

  buf_handle->data = alloc_object(&cpc_mempool_hdlc_reject);
  buf_handle->buffer_type = SL_CPC_HDLC_REJECT_BUFFER;

  if (buf_handle->data == NULL) {
    free_object(&cpc_mempool_buffer_handle, buf_handle);
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  buf_handle->data_length = 1;
  *handle = buf_handle;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get a CPC buffer for reception.
 ******************************************************************************/
sl_status_t sli_cpc_get_buffer_handle_for_rx(sl_cpc_buffer_handle_t **handle)
{
  sl_cpc_buffer_handle_t *buf_handle;
  sl_status_t status;

  if (handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  status = sli_cpc_get_buffer_handle(&buf_handle);
  if (status != SL_STATUS_OK) {
    return status;
  }

  sli_cpc_get_hdlc_header_buffer(&buf_handle->hdlc_header);
  if (buf_handle->hdlc_header == NULL) {
    free_object(&cpc_mempool_buffer_handle, buf_handle);
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  if (!sli_cpc_driver_capabilities.use_raw_rx_buffer) {
    buf_handle->data = alloc_object(&cpc_mempool_rx_buffer);
    if (buf_handle->data == NULL) {
      sli_cpc_free_hdlc_header(buf_handle->hdlc_header);
      free_object(&cpc_mempool_buffer_handle, buf_handle);
      return SL_STATUS_NO_MORE_RESOURCE;
    }
  } else {
    buf_handle->data = NULL;
  }

  buf_handle->data_length = 0u;
  buf_handle->buffer_type = SL_CPC_RX_BUFFER;
  buf_handle->reason = SL_CPC_REJECT_NO_ERROR;

  *handle = buf_handle;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get a CPC RAW buffer for reception.
 ******************************************************************************/
sl_status_t sli_cpc_get_raw_rx_buffer(void **raw_rx_buffer)
{
  EFM_ASSERT(sli_cpc_driver_capabilities.use_raw_rx_buffer);

  *raw_rx_buffer = alloc_object(&cpc_mempool_rx_buffer);
  if (*raw_rx_buffer == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get a CPC RAW buffer for reception.
 ******************************************************************************/
sl_status_t sli_cpc_free_raw_rx_buffer(void *raw_rx_buffer)
{
  EFM_ASSERT(sli_cpc_driver_capabilities.use_raw_rx_buffer);

  if (raw_rx_buffer != NULL) {
    free_object(&cpc_mempool_rx_buffer, raw_rx_buffer);
  } else {
    return SL_STATUS_NULL_POINTER;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free rx handle and all associate buffers
 ******************************************************************************/
sl_status_t sli_cpc_drop_buffer_handle(sl_cpc_buffer_handle_t *handle)
{
  bool is_rx_buffer = false;

  if (handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (handle->buffer_type == SL_CPC_RX_BUFFER) {
    is_rx_buffer = true;
  }

  if (handle->ref_count > 0) {
    // Can't free the buffer_handle, it is being used elsewhere
    return SL_STATUS_BUSY;
  }

  if (handle->data != NULL) {
    if (handle->buffer_type == SL_CPC_HDLC_REJECT_BUFFER) {
      free_object(&cpc_mempool_hdlc_reject, handle->data);
    } else if (handle->buffer_type == SL_CPC_RX_BUFFER) {
      free_object(&cpc_mempool_rx_buffer, handle->data);

#ifdef CPC_DEBUG_TRACE
      if (handle->endpoint != NULL) {
        SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_DATA_FRAME_DROPPED(handle->endpoint);
      } else {
        SLI_CPC_DEBUG_TRACE_CORE_RXD_DATA_FRAME_DROPPED();
      }
#endif
    } else {
      // If no type, it's a buffer used for transmit operation.
      // So no need to free any data since the buffer is passed by the application.
    }
  }

  if (handle->hdlc_header != NULL) {
    sli_cpc_free_hdlc_header(handle->hdlc_header);
  }

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  sli_cpc_free_security_tag_buffer(handle->security_tag);
#endif

  free_object(&cpc_mempool_buffer_handle, handle);

  if (is_rx_buffer) {
    // Notify that at least one RX buffer is available
    sli_cpc_memory_on_rx_buffer_free();
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Convert rx buffer handle to a receive queue item;
 *  Free hdlc header and handle
 *  Alloc queue item and set fields
 ******************************************************************************/
sl_status_t sli_cpc_push_back_rx_data_in_receive_queue(sl_cpc_buffer_handle_t *handle,
                                                       sl_slist_node_t **head,
                                                       uint16_t data_length)
{
  sl_cpc_receive_queue_item_t *item;
  sl_status_t status;

  if (handle == NULL || head == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  status = sli_cpc_get_receive_queue_item(&item);
  if (status != SL_STATUS_OK) {
    return status;
  }

  item->node.node = NULL;
  item->data = handle->data;
  item->buffer_type = handle->buffer_type;
  item->data_length = data_length;

  sl_slist_push_back(head, &item->node);

  sli_cpc_free_hdlc_header(handle->hdlc_header);
  free_object(&cpc_mempool_buffer_handle, handle);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free rx buffer returned by sl_cpc_read().
 ******************************************************************************/
sl_status_t sl_cpc_free_rx_buffer(void *data)
{
  if (data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  free_object(&cpc_mempool_rx_buffer, data);

  // Notify that at least one RX buffer is available
  sli_cpc_memory_on_rx_buffer_free();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free CPC hdlc header
 ******************************************************************************/
sl_status_t sli_cpc_free_hdlc_header(void *data)
{
  if (data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  free_object(&cpc_mempool_hdlc_header, data);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get receive queue item
 ******************************************************************************/
sl_status_t sli_cpc_get_receive_queue_item(sl_cpc_receive_queue_item_t **item)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *item = alloc_object(&cpc_mempool_rx_queue_item);
  if (*item == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free receive queue item
 ******************************************************************************/
sl_status_t sli_cpc_free_receive_queue_item(sl_cpc_receive_queue_item_t *item,
                                            void **data,
                                            uint16_t *data_length)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *data = item->data;
  *data_length = item->data_length;

  free_object(&cpc_mempool_rx_queue_item, item);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free receive queue item and the data buffer
 ******************************************************************************/
void sli_cpc_drop_receive_queue_item(sl_cpc_receive_queue_item_t *item)
{
  if (item == NULL) {
    return;
  }

  if (item->data != NULL) {
    if (item->buffer_type == SL_CPC_HDLC_REJECT_BUFFER) {
      free_object(&cpc_mempool_hdlc_reject, item->data);
    } else if (item->buffer_type == SL_CPC_RX_BUFFER) {
      free_object(&cpc_mempool_rx_buffer, item->data);

      // Notify that at least one RX buffer is available
      sli_cpc_memory_on_rx_buffer_free();
    }
  }

  free_object(&cpc_mempool_rx_queue_item, item);
}

/***************************************************************************//**
 * Get a transmit queue item.
 ******************************************************************************/
sl_status_t sli_cpc_get_transmit_queue_item(sl_cpc_transmit_queue_item_t **item)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *item = alloc_object(&cpc_mempool_tx_queue_item);
  if (*item == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free transmit queue item.
 ******************************************************************************/
sl_status_t sli_cpc_free_transmit_queue_item(sl_cpc_transmit_queue_item_t *item)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  free_object(&cpc_mempool_tx_queue_item, item);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get a transmit queue item from S-Frame.
 ******************************************************************************/
sl_status_t sli_cpc_get_sframe_transmit_queue_item(sl_cpc_transmit_queue_item_t **item)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *item = alloc_object(&cpc_mempool_tx_queue_item_sframe);
  if (*item == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free transmit queue item from S-Frame pool.
 ******************************************************************************/
sl_status_t sli_cpc_free_sframe_transmit_queue_item(sl_cpc_transmit_queue_item_t *item)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  free_object(&cpc_mempool_tx_queue_item_sframe, item);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get endpoint
 ******************************************************************************/
sl_status_t sli_cpc_get_endpoint(sl_cpc_endpoint_t **endpoint)
{
  if (endpoint == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *endpoint = alloc_object(&cpc_mempool_endpoint);
  if (*endpoint == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  memset(*endpoint, 0, sizeof(sl_cpc_endpoint_t));

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free endpoint
 ******************************************************************************/
void sli_cpc_free_endpoint(sl_cpc_endpoint_t *endpoint)
{
  free_object(&cpc_mempool_endpoint, endpoint);
}

/***************************************************************************//**
 * Get endpoint closed argument item
 ******************************************************************************/
sl_status_t sli_cpc_get_closed_arg(sl_cpc_endpoint_closed_arg_t **arg)
{
  if (arg == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *arg = alloc_object(&cpc_mempool_endpoint_closed_arg_item);
  if (*arg == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free endpoint closed argument item
 ******************************************************************************/
void sli_cpc_free_closed_arg(sl_cpc_endpoint_closed_arg_t *arg)
{
  free_object(&cpc_mempool_endpoint_closed_arg_item, arg);
}

/***************************************************************************//**
 * Push back a list item containing an allocated buffer handle
 ******************************************************************************/
void sli_cpc_push_back_buffer_handle(sl_slist_node_t **head, sl_slist_node_t *item, sl_cpc_buffer_handle_t *buf_handle)
{
  EFM_ASSERT(buf_handle != NULL);
  buf_handle->ref_count++;
  sl_slist_push_back(head, item);
}

/***************************************************************************//**
 * Push a list item containing an allocated buffer handle
 ******************************************************************************/
void sli_cpc_push_buffer_handle(sl_slist_node_t **head, sl_slist_node_t *item, sl_cpc_buffer_handle_t *buf_handle)
{
  EFM_ASSERT(buf_handle != NULL);
  buf_handle->ref_count++;
  sl_slist_push(head, item);
}

/***************************************************************************//**
 * Get a command buffer
 ******************************************************************************/
sl_status_t sli_cpc_get_system_command_buffer(sli_cpc_system_cmd_t **item)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *item = alloc_object(&cpc_mempool_system_command);
  if (*item == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Free a command buffer
 ******************************************************************************/
sl_status_t sli_cpc_free_command_buffer(sli_cpc_system_cmd_t *item)
{
  if (item == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  free_object(&cpc_mempool_system_command, item);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Allocate a security tag buffer
 ******************************************************************************/
sl_status_t sli_cpc_get_security_tag_buffer(void **tag_buffer)
{
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  if (tag_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  *tag_buffer = alloc_object(&cpc_mempool_tx_security_tag);
  if (*tag_buffer == NULL) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  return SL_STATUS_OK;
#else
  (void)tag_buffer;

  return SL_STATUS_NOT_AVAILABLE;
#endif
}

/***************************************************************************//**
 * Free a security tag buffer
 ******************************************************************************/
sl_status_t sli_cpc_free_security_tag_buffer(void *tag_buffer)
{
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  if (tag_buffer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  free_object(&cpc_mempool_tx_security_tag, tag_buffer);

  return SL_STATUS_OK;
#else
  (void)tag_buffer;

  return SL_STATUS_NOT_AVAILABLE;
#endif
}

/***************************************************************************//**
 * Alloc object from a specified pool; Manage stat as well
 ******************************************************************************/
static void* alloc_object(sl_cpc_mem_pool_handle_t *pool)
{
  void *block;

  block = sli_mem_pool_alloc((sli_mem_pool_handle_t *)pool->pool_handle);
  MEM_POOL_ALLOC(pool, block);

  return block;
}

/***************************************************************************//**
 * Free object from a specified pool; Manage stat as well
 ******************************************************************************/
static void free_object(sl_cpc_mem_pool_handle_t *pool,
                        void *block)
{
  sli_mem_pool_free((sli_mem_pool_handle_t *)pool->pool_handle, block);
  MEM_POOL_FREE(pool);
}
