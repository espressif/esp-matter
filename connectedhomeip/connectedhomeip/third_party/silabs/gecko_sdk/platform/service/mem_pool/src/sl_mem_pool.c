/***************************************************************************/ /**
 * @file
 * @brief Memory Pool API implementation.
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

#include "em_core.h"
#include "sli_mem_pool.h"

#include <stddef.h>

#define SLI_MEM_POOL_OUT_OF_MEMORY     0xFFFFFFFF
#define SLI_MEM_POOL_REQUIRED_PADDING(obj_size) (((sizeof(size_t) - ((obj_size) % sizeof(size_t))) % sizeof(size_t)))

/***************************************************************************//**
 * Creates a memory pool
 ******************************************************************************/
void sli_mem_pool_create(sli_mem_pool_handle_t *mem_pool,
                         uint32_t block_size,
                         uint32_t block_count,
                         void* buffer,
                         uint32_t buffer_size)
{
  EFM_ASSERT(mem_pool != NULL);
  EFM_ASSERT(buffer != NULL);
  EFM_ASSERT(block_count != 0);
  EFM_ASSERT(block_size != 0);
  EFM_ASSERT(buffer_size >= block_count * (block_size + SLI_MEM_POOL_REQUIRED_PADDING(block_size)));

  mem_pool->block_size = block_size + SLI_MEM_POOL_REQUIRED_PADDING(block_size);
  mem_pool->block_count = block_count;
  mem_pool->data = buffer;
  mem_pool->free_block_addr = mem_pool->data;

  uint32_t block_addr = (uint32_t)mem_pool->data;

  // Populate the list of free blocks (except last block)
  for (uint32_t i = 0; i < (block_count - 1); i++) {
    *(uint32_t *)block_addr = (uint32_t)(block_addr + mem_pool->block_size);
    block_addr += mem_pool->block_size;
  }

  // Last element will indicate OOM
  *(uint32_t *)block_addr = SLI_MEM_POOL_OUT_OF_MEMORY;
}

/***************************************************************************//**
 * Allocates an object from a memory pool
 ******************************************************************************/
void* sli_mem_pool_alloc(sli_mem_pool_handle_t *mem_pool)
{
  CORE_DECLARE_IRQ_STATE;

  EFM_ASSERT(mem_pool != NULL);

  CORE_ENTER_ATOMIC();

  if ((uint32_t)mem_pool->free_block_addr == SLI_MEM_POOL_OUT_OF_MEMORY) {
    CORE_EXIT_ATOMIC();
    return NULL;
  }

  // Get the next free block
  void *block_addr = mem_pool->free_block_addr;

  // Update the next free block using the address saved in that block
  mem_pool->free_block_addr = (void *)*(uint32_t *)block_addr;

  CORE_EXIT_ATOMIC();

  return block_addr;
}

/***************************************************************************//**
 * Frees an object previously allocated to a memory pool.
 ******************************************************************************/
void sli_mem_pool_free(sli_mem_pool_handle_t *mem_pool, void *block)
{
  CORE_DECLARE_IRQ_STATE;

  EFM_ASSERT(mem_pool != NULL);

  // Validate that the provided address is in the buffer range
  EFM_ASSERT((block >= mem_pool->data) && ((uint32_t)block <= ((uint32_t)mem_pool->data + (mem_pool->block_size * mem_pool->block_count))));

  CORE_ENTER_ATOMIC();

  // Save the current free block addr in this block
  *(uint32_t *)block = (uint32_t)mem_pool->free_block_addr;
  mem_pool->free_block_addr = block;

  CORE_EXIT_ATOMIC();
}
