/***************************************************************************//**
 * @file
 * @brief Memory Pool
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

#ifndef __SL_MEMPOOL_H__
#define __SL_MEMPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stddef.h>
#include <stdbool.h>
#include "sl_status.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Memory Pool block handler structure type definition
typedef struct sl_mempool_block_hnd {
  /// Start address
  void *start_addr;
  /// Next block pointer
  struct sl_mempool_block_hnd *next;
} sl_mempool_block_hnd_t;

/// Memory Pool handler structure type definition
typedef struct sl_mempool {
  /// Block count
  size_t block_count;
  /// Block size
  size_t block_size;
  /// Buffer pointer
  void * buff;
  /// Buffer size
  size_t buff_size;
  /// Block linked list
  sl_mempool_block_hnd_t *blocks;
  /// Used block count
  size_t used_block_count;
} sl_mempool_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Memory Pool component init.
 * @details Initializing mutex and call sl_mempool_blocks_init()
 *****************************************************************************/
void sl_mempool_init(void);

/**************************************************************************//**
 * @brief Init memory pool blocks.
 * @details Initializing the array of runtime block handlers
 *****************************************************************************/
void sl_mempool_blocks_init(void);

/**************************************************************************//**
 * @brief Create Memory Pool.
 * @details Initializing the memory pool handler with argument check
 * @param[out] memp Memory Pool object ptr
 * @param[in] block_count Block count
 * @param[in] block_size Block size in bytes
 * @param[in] buff Buffer ptr
 * @param[in] buff_size Buffer size
 * @return sl_status_t SL_STATUS_OK on success, SL_STATUS_FAIL on error
 *****************************************************************************/
sl_status_t sl_mempool_create(sl_mempool_t *memp,
                              const size_t block_count,
                              const size_t block_size,
                              void * const buff,
                              const size_t buff_size);

/**************************************************************************//**
 * @brief Alloc Memory Pool.
 * @details Allocation block and add block handler to the 'blocks' linked list
 * @param[in,out] memp Memory Pool object pointer
 * @return void* Pointer of allocated block or NULL on error
 *****************************************************************************/
void * sl_mempool_alloc(sl_mempool_t * const memp);

/**************************************************************************//**
 * @brief Free allocated memory pool.
 * @details Finding the requested address and free the allocated block, destroying the block handler
 * @param[in,out] memp Memory Pool object pointer
 * @param[in] addr
 *****************************************************************************/
void sl_mempool_free(sl_mempool_t * const memp, const void * const addr);

/**************************************************************************//**
 * @brief Not enough runtime block handler.
 * @details Weak implementation. It's called when there isn't any available free block handler
 * @param[in,out] memp Memory Pool object pointer
 * @return void* Address of handled block allocation
 *****************************************************************************/
void *sl_mempool_not_enough_runtime_block_handler(sl_mempool_t *memp);

/**************************************************************************//**
 * @brief Check address whether is in the buffer.
 * @details Helper function
 * @param[in] memp Memory Pool object
 * @param[in] addr Address
 * @return true Address is in the buffer
 * @return false Address is not in the buffer
 *****************************************************************************/
static inline bool sl_mempool_is_addr_in_buff(const sl_mempool_t * const memp,
                                              const void * const addr)
{
  return (const uint8_t *)addr >= (const uint8_t *)memp->buff
         && (const uint8_t *)addr < ((const uint8_t *)memp->buff + memp->buff_size) ? true : false;
}

/**************************************************************************//**
 * @brief Get free block count of Memory Pool.
 * @details Helper function
 * @param[in] memp Memory Pool object
 * @return size_t Count of free blocks
 *****************************************************************************/
static inline size_t sl_mempool_get_free_block_count(const sl_mempool_t * const memp)
{
  return memp->block_count - memp->used_block_count;
}

#ifdef __cplusplus
}
#endif

#endif // __SL_MEMPOOL_H__
