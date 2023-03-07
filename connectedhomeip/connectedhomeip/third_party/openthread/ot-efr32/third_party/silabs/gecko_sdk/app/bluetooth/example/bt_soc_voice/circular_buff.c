/***************************************************************************//**
 * @file
 * @brief Circular Buffer implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "circular_buff.h"

/***************************************************************************//**
 * @defgroup Circular_Buffer Circular Buffer implementation
 * @{
 * @brief Circular Buffer as FIFO implementation.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @defgroup Circular_Buffer_Locals Circular Buffer Local Variables
 * @{
 * @brief Circular Buffer local variables
 ******************************************************************************/

/** @} {end defgroup Circular_Buffer_Locals} */

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @defgroup Circular_Buffer_Functions Circular Buffer Functions
 * @{
 * @brief Circular Buffer support functions
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

inline static bool is_empty(circular_buffer_t *cb);
inline static bool is_full(circular_buffer_t *cb);

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Circular Buffer initialization
 *
 * @param[in] cb
 *    Circular buffer to be initialized
 *
 * @param[in] capacity
 *    Maximum number of items in circular buffer
 *
 * @param[in] item_size
 *    Size of one item
 *
 * @return
 *    Returns zero on OK, error code otherwise
 ******************************************************************************/
cb_err_code_t cb_init(circular_buffer_t *cb, size_t capacity, size_t item_size)
{
  cb->buffer = malloc(capacity * item_size);
  if (cb->buffer == NULL) {
    return cb_err_no_mem;
  }

  cb->buffer_end = (char *)cb->buffer + capacity * item_size;
  cb->capacity = capacity;
  cb->count = 0;
  cb->item_size = item_size;
  cb->head = cb->buffer;
  cb->tail = cb->buffer;

  return cb_err_ok;
}

/***************************************************************************//**
 * @brief
 *    Push item into the queue
 *
 * @param[in] cb
 *    Circular buffer to which item is to be pushed
 *
 * @param[in] item
 *    Item to be pushed
 *
 * @return
 *    Returns zero on OK, error code otherwise
 ******************************************************************************/
cb_err_code_t push(circular_buffer_t *cb, void *item)
{
  if ( is_full(cb) ) {
    return cb_err_full;
  }

  memcpy(cb->head, item, cb->item_size);
  cb->head = (char *)cb->head + cb->item_size;
  if (cb->head == cb->buffer_end) {
    cb->head = cb->buffer;
  }
  cb->count++;

  return cb_err_ok;
}

/***************************************************************************//**
 * @brief
 *    Pop item from the queue
 *
 * @param[in] cb
 *    Circular buffer from which item is to be poped
 *
 * @param[in] item
 *    Item to be pop
 *
 * @return
 *    Returns zero on OK, error code otherwise
 ******************************************************************************/
cb_err_code_t pop(circular_buffer_t *cb, void *item)
{
  if ( is_empty(cb) ) {
    return cb_err_empty;
  }
  memcpy(item, cb->tail, cb->item_size);
  cb->tail = (char *)cb->tail + cb->item_size;
  if (cb->tail == cb->buffer_end) {
    cb->tail = cb->buffer;
  }
  cb->count--;

  return cb_err_ok;
}

/***************************************************************************//**
 * @brief
 *    Free Circular Buffer
 *
 * @param[in] cb
 *    Circular Buffer to be freed
 *
 * @return
 *    None
 ******************************************************************************/
void cb_free(circular_buffer_t *cb)
{
  free(cb->buffer);
}

/***************************************************************************//**
 * @brief
 *    Data Buffer to be pushed to circular buffer
 *
 * @param[in] cb
 *    Circular buffer to which buffer is to be pushed
 *
 * @param[in] inBuff
 *    Pointer to beginning of buffer
 *
 * @param[in] len
 *    inBuff length
 *
 * @return
 *    Returns zero on OK, error code otherwise
 ******************************************************************************/
cb_err_code_t cb_push_buff(circular_buffer_t *cb, void *inBuff, size_t len)
{
  cb_err_code_t err = cb_err_ok;
  if ( len > (cb->capacity - cb->count) ) {
    return cb_err_too_much_data;
  }

  for (uint16_t i = 0; i < len; i++) {
    err = push(cb, ((char *)inBuff + cb->item_size * i));
    if ( err != cb_err_ok ) {
      return err;
    }
  }

  return err;
}

/***************************************************************************//**
 * @brief
 *    Data Buffer to be poped from circular buffer
 *
 * @param[in] cb
 *    Circular buffer from which buffer is to be poped
 *
 * @param[in] outBuff
 *    Pointer to beginning of buffer
 *
 * @param[in] len
 *    outBuff length
 *
 * @return
 *    Returns zero on OK, error code otherwise
 ******************************************************************************/
cb_err_code_t cb_pop_buff(circular_buffer_t *cb, void *outBuff, size_t len)
{
  cb_err_code_t err = cb_err_ok;
  if ( len > cb->count) {
    return cb_err_insuff_data;
  }

  for (uint16_t i = 0; i < len; i++) {
    err = pop(cb, ((char *)outBuff + cb->item_size * i));
    if ( err != cb_err_ok) {
      return err;
    }
  }
  return err;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    Function checks if circular buffer is empty
 *
 * @param[in] cb
 *    Circular buffer from which buffer status is checked
 *
 * @return
 *    Return true if there is place to push item, otherwise false
 ******************************************************************************/
inline static bool is_empty(circular_buffer_t *cb)
{
  return cb->count ? false : true;
}

/***************************************************************************//**
 * @brief
 *    Function checks if circular buffer is full
 *
 * @param[in] cb
 *    Circular buffer from which buffer status is checked
 *
 * @return
 *    Return true if there is no place in circular buffer, otherwise false
 ******************************************************************************/
inline static bool is_full(circular_buffer_t *cb)
{
  return cb->count == cb->capacity ? true : false;
}

/** @endcond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** @} {end defgroup Circular_Buffer_Functions} */

/** @} {end defgroup Circular_Buffer} */
