/***************************************************************************//**
 * @file
 * @brief Application Queue header
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

#ifndef APP_QUEUE_H
#define APP_QUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"

// -----------------------------------------------------------------------------
// Structures and Types

/// Application Queue pointer type forward declaration
typedef struct app_queue *app_queue_ptr_t;

/***************************************************************************//**
 * Prototype of queue overflow callback function.
 *
 * @param[in] queue Pointer to the queue structure being overflowed.
 * @param[in] data  Pointer to the item being considered for removal from
 *                  the queue on overflow.
 *
 * @retval true     Discard the queue item being considered for removal
 *                  (default behavior).
 * @retval false    Nothing to be discarded from the queue.
 ******************************************************************************/
typedef bool (*app_queue_overflow_callback_t)(app_queue_ptr_t queue, \
                                              uint8_t         *data);

/// Application Queue structure
typedef struct app_queue {
  uint16_t                      head;      ///< Head position
  uint16_t                      count;     ///< Count of items in the list
  uint16_t                      size;      ///< Size of the queue
  uint16_t                      item_size; ///< Size of one queue item
  app_queue_overflow_callback_t callback;  ///< Overflow callback
  uint8_t                       *data;     ///< Data storage for the queue
} app_queue_t;

/***************************************************************************//**
 * Static allocation macro
 *
 * @param[in] HANDLE Handle name of the queue.
 * @param[in] TYPE   Base element type of the queue
 * @param[in] SIZE   Requested size of the queue in elements.
 ******************************************************************************/
#define APP_QUEUE(HANDLE, TYPE, SIZE) \
  app_queue_t HANDLE;                 \
  static TYPE HANDLE##_data[SIZE]

/***************************************************************************//**
 * Initialization macro
 *
 * @param[in] HANDLE Handle name of the queue.
 * @param[in] TYPE   Base element type of the queue
 * @param[in] SIZE   Requested size of the queue in elements.
 *
 * @retval SL_STATUS_OK           Initialization was successful.
 * @retval SL_STATUS_NULL_POINTER Pointer to the queue or data is invalid.
 ******************************************************************************/
#define APP_QUEUE_INIT(HANDLE, TYPE, SIZE) \
  app_queue_init(HANDLE, SIZE, sizeof(TYPE), (void *)HANDLE##_data)

// -----------------------------------------------------------------------------
// Queue Functions

/***************************************************************************//**
 * Initialize the queue.
 *
 * @param[in] queue     Pointer to the queue structure to initialize.
 * @param[in] size      The number of entries to store in this queue
 * @param[in] item_size The size of a queue element.
 * @param[in] data      Pointer to the data storage.
 *
 * @retval SL_STATUS_OK           Initialization was successful.
 * @retval SL_STATUS_NULL_POINTER Pointer to the queue or data is invalid.
 ******************************************************************************/
sl_status_t app_queue_init(app_queue_t *queue,
                           uint16_t    size,
                           uint16_t    item_size,
                           uint8_t     *data);

/***************************************************************************//**
 * Add item to the end of the queue.
 *
 * If the queue is full, the oldest queued item will be replaced by default.
 * Register an overflow callback to change this default behavior.
 *
 * @param[in] queue The queue to add the item to.
 * @param[in] data  The pointer object to store in the queue.
 *
 * @retval SL_STATUS_OK              Adding item was successful.
 * @retval SL_STATUS_NOT_INITIALIZED Queue was not initialized.
 * @retval SL_STATUS_WOULD_OVERFLOW  Queue would overflow and the overflow event
 *                                   was not handled.
 * @retval SL_STATUS_NULL_POINTER    Pointer to the queue or data is invalid.
 ******************************************************************************/
sl_status_t app_queue_add(app_queue_t *queue, uint8_t *data);

/***************************************************************************//**
 * Specify a callback to be called upon queue overflow.
 *
 * The oldest queued item being considered for removal on queue overflow is
 * provided by this callback.
 *
 * @param[in] queue     The queue to specify an overflow callback for.
 * @param[in] callback  The callback to be called on queue overflow. If callback
 *                      is NULL, no callback will be issued on queue overflow.
 *
 * @retval SL_STATUS_OK           Setting callback was successful.
 * @retval SL_STATUS_NULL_POINTER Pointer to the queue is invalid.
 ******************************************************************************/
sl_status_t app_queue_set_overflow_callback(app_queue_t                   *queue,
                                            app_queue_overflow_callback_t callback);

/***************************************************************************//**
 * Get a pointer to the head of the queue without removing that item.
 *
 * @param[in]  queue The queue to peek at the item from.
 * @param[out] data  Data output for the item to copy into.
 *
 * @retval SL_STATUS_OK              Getting item was successful.
 * @retval SL_STATUS_NOT_INITIALIZED Queue was not initialized.
 * @retval SL_STATUS_EMPTY           Queue is empty.
 * @retval SL_STATUS_NULL_POINTER    Pointer to the queue or data is invalid.
 ******************************************************************************/

sl_status_t app_queue_peek(app_queue_t *queue, uint8_t *data);

/***************************************************************************//**
 * Remove an item from the head of the queue and return its data pointer.
 *
 * @param[in]  queue The queue to remove the item from.
 * @param[out] data  Data output for the item to copy into.
 *
 * @retval SL_STATUS_OK              Removing item was successful.
 * @retval SL_STATUS_NOT_INITIALIZED Queue was not initialized.
 * @retval SL_STATUS_EMPTY           Queue is empty.
 * @retval SL_STATUS_NULL_POINTER    Pointer to the queue or data is invalid.
 ******************************************************************************/
sl_status_t app_queue_remove(app_queue_t *queue, uint8_t *data);

/***************************************************************************//**
 * Determine if the given queue is empty.
 *
 * @param[in]  queue The queue to check the status of.
 *
 * @return     True, if the queue is empty.
 ******************************************************************************/
bool app_queue_is_empty(app_queue_t *queue);

/***************************************************************************//**
 * Determine if the given queue is full.
 *
 * @param[in]  queue The queue to check the status of.
 *
 * @return     True, if the queue is full.
 ******************************************************************************/
bool app_queue_is_full(app_queue_t *queue);

#endif // APP_QUEUE_H
