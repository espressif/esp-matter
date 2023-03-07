/***************************************************************************//**
 * @file
 * @brief Application Queue implementation
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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "app_queue.h"
#include "em_core.h"

// -----------------------------------------------------------------------------
// Public functions

sl_status_t app_queue_init(app_queue_t *queue,
                           uint16_t    size,
                           uint16_t    item_size,
                           uint8_t     *data)
{
  if (queue == NULL || data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  // Atomically initialize the queue structure
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  queue->item_size = item_size;
  queue->data = data;
  queue->size = size;
  queue->head = 0;
  queue->count = 0;
  queue->callback = NULL;
  CORE_EXIT_CRITICAL();

  return SL_STATUS_OK;
}

sl_status_t app_queue_set_overflow_callback(app_queue_t                   *queue,
                                            app_queue_overflow_callback_t callback)
{
  if (queue == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  queue->callback = callback;
  return SL_STATUS_OK;
}

sl_status_t app_queue_add(app_queue_t *queue, uint8_t *data)
{
  sl_status_t sc = SL_STATUS_OK;

  // Do nothing if there's no queue or data given
  if (queue == NULL || data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (queue->data == NULL) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  bool added = false;

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (queue->count < queue->size) {
    uint16_t offset = (queue->head + queue->count) % queue->size;

    // Insert this item at the end of the queue
    uint8_t *ptr = queue->data + (offset * queue->item_size);
    memcpy(ptr, data, queue->item_size);
    queue->count++;
    added = true;
  } else {
    // If an overflow callback exists, call it to see if the oldest queued
    // item is to be replaced (default) or not.
    uint8_t *ptr = (queue->data + (queue->head * queue->item_size));
    if (queue->callback != NULL) {
      added = queue->callback(queue, ptr);
    } else {
      // There is no callback set, queue would overflow
      sc = SL_STATUS_WOULD_OVERFLOW;
    }
    // Overwrite what's at the head of the queue since we're out of space
    if (added) {
      memcpy(ptr, data, queue->item_size);
      queue->head = (queue->head + 1) % queue->size;
    }
  }
  CORE_EXIT_CRITICAL();

  return sc;
}

sl_status_t app_queue_peek(app_queue_t *queue, uint8_t * data)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t *ptr = NULL;

  // Do nothing if there's no queue or data given
  if (queue == NULL || data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (queue->data == NULL) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (queue->count > 0) {
    ptr = (queue->data + (queue->head * queue->item_size));
    memcpy(data, ptr, queue->item_size);
  } else {
    sc = SL_STATUS_EMPTY;
  }
  CORE_EXIT_CRITICAL();

  return sc;
}

sl_status_t app_queue_remove(app_queue_t *queue, uint8_t * data)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t *ptr = NULL;

  // Do nothing if there's no queue or data given
  if (queue == NULL || data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }
  if (queue->data == NULL) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (queue->count > 0) {
    ptr = (queue->data + queue->head * queue->item_size);
    memcpy(data, ptr, queue->item_size);
    queue->head = (queue->head + 1) % queue->size;
    queue->count--;
  } else {
    sc = SL_STATUS_EMPTY;
  }
  CORE_EXIT_CRITICAL();

  return sc;
}

bool app_queue_is_empty(app_queue_t *queue)
{
  bool result;

  // Do nothing if there's no queue or output given
  if (queue == NULL) {
    return true;
  }
  if (queue->data == NULL) {
    return true;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  result = (queue->count == 0);
  CORE_EXIT_CRITICAL();

  return result;
}

bool app_queue_is_full(app_queue_t *queue)
{
  bool result;

  // Do nothing if there's no queue or output given
  if (queue == NULL) {
    return false;
  }
  if (queue->data == NULL) {
    return false;
  }

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  result = (queue->count == queue->size);
  CORE_EXIT_CRITICAL();

  return result;
}
