/***************************************************************************/ /**
 * @file
 * @brief Memory Pool API.
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

#ifndef SL_MEM_POOL_H
#define SL_MEM_POOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef DOXYGEN
/***************************************************************************/ /**
 * @addtogroup mempool Memory Pool
 * @brief Memory Pool
 * @{
 ******************************************************************************/

/// @brief Memory Pool Handle
#endif // DOXYGEN
typedef struct sli_mem_pool_handle{
  void* free_block_addr;   ///< Pointer to head of free block list.
  void* data;              ///< Pointer to buffer.
  uint16_t block_size;     ///< Size of the blocks.
  uint16_t block_count;    ///< Total number of blocks in pool.
} sli_mem_pool_handle_t;

#define SLI_MEM_POOL_PAD_OBJECT_SIZE(block_size) (block_size + sizeof(uint32_t) - 1)

#define SLI_MEM_POOL_BUFFER_SIZE(block_size, block_count) (block_count                                \
                                                           * SLI_MEM_POOL_PAD_OBJECT_SIZE(block_size) \
                                                           / sizeof(uint32_t))

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Helper macro to create a memory pool buffer.
 *
 * @param[in] mempool_name  Name of memory pool.
 * @param[in] block_size  Size of the blocks.
 * @param[in] block_count  Number of blocks in pool.
 ******************************************************************************/
#endif // DOXYGEN
#define SLI_MEM_POOL_DECLARE_BUFFER(mempool_name, block_size, block_count) \
  static uint32_t mempool_name##_buffer[SLI_MEM_POOL_BUFFER_SIZE(block_size, block_count)]

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Helper macro to create a memory pool buffer by providing the block type.
 *
 * @param[in] mempool_name  Name of memory pool.
 * @param[in] block_type  Type of the blocks.
 * @param[in] block_count  Number of blocks in pool.
 ******************************************************************************/
#endif // DOXYGEN
#define SLI_MEM_POOL_DECLARE_BUFFER_WITH_TYPE(mempool_name, block_type, block_count) \
  static struct {                                                                    \
    union {                                                                          \
      void *ptr;                                                                     \
      block_type entry;                                                              \
    };                                                                               \
  } mempool_name##_buffer[block_count]

#ifndef DOXYGEN
// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************/ /**
 * Creates a memory pool.
 *
 * @param[in] mem_pool  Pointer to memory pool handle.
 * @param[in] block_size  Size of the blocks.
 * @param[in] block_count  Number of blocks in pool.
 * @param[in] buffer  Pointer to buffer to use as pool storage.
 * @param[in] buffer_size  Size of the buffer to use as pool storage.
 ******************************************************************************/
#endif // DOXYGEN
void sli_mem_pool_create(sli_mem_pool_handle_t *mem_pool,
                         uint32_t block_size,
                         uint32_t block_count,
                         void* buffer,
                         uint32_t buffer_size);

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Allocates an object from a memory pool.
 *
 * @param[in] mem_pool  Pointer to memory pool handle.
 *
 * @return Pointer to allocated block. Null if pool is empty.
 ******************************************************************************/
#endif // DOXYGEN
void* sli_mem_pool_alloc(sli_mem_pool_handle_t *mem_pool);

#ifndef DOXYGEN
/***************************************************************************/ /**
 * Frees an object previously allocated to a memory pool.
 *
 * @param[in] mem_pool  Pointer to memory pool handle.
 * @param[in] block  Pointer to block to free.
 ******************************************************************************/
#endif // DOXYGEN
void sli_mem_pool_free(sli_mem_pool_handle_t *mem_pool, void *block);

#ifndef DOXYGEN
/** @} (end addtogroup mempool) */
#endif // DOXYGEN

#ifdef __cplusplus
}
#endif

#endif // SL_MEM_POOL_H
