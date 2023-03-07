/***************************************************************************//**
 * @file
 * @brief Circular Buffer API
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

#ifndef CIRCULAR_BUFF_H_
#define CIRCULAR_BUFF_H_

#include <stdbool.h>
#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Circular_Buffer
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup Circular_Buffer_Config_Settings Circular Buffer configuration
 * @{
 * @brief Circular Buffer configuration setting macro definitions
 ******************************************************************************/

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/
typedef enum {
  cb_err_ok = 0,            /**< No error */
  cb_err_full,              /**< Buffer is full */
  cb_err_empty,             /**< Buffer is empty */
  cb_err_no_mem,            /**< No memory for buffer allocation */
  cb_err_too_much_data,     /**< To much data to be push into the circular buffer */
  cb_err_insuff_data,       /**< Insufficient amount of data to be pop */
}cb_err_code_t;

typedef struct {
  void *buffer;             /**< Data buffer */
  void *buffer_end;         /**< End of data buffer */
  size_t capacity;           /**< Maximum number of items in the buffer */
  size_t count;             /**< Number of items in the buffer */
  size_t item_size;         /**< Size of each item in the buffer */
  void *head;               /**< Pointer to head */
  void *tail;               /**< Pointer to tail */
} circular_buffer_t;

/** @} {end defgroup Circular_Buffer_Config_Settings} */

/***************************************************************************//**
 * @defgroup Circular_Buffer_Functions Circular Buffer Functions
 * @{
 * @brief Circular Buffer support functions
 ******************************************************************************/

cb_err_code_t cb_init(circular_buffer_t *cb, size_t capacity, size_t sz);
cb_err_code_t cb_push_buff(circular_buffer_t *cb, void *inBuff, size_t len);
cb_err_code_t cb_pop_buff(circular_buffer_t *cb, void *outBuff, size_t len);
void cb_free(circular_buffer_t *cb);

/** @} {end defgroup Circular_Buffer_Functions}*/

/** @} {end addtogroup Circular_Buffer} */

#endif /* CIRCULAR_BUFF_H_ */
