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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "em_common.h"
#include "sl_mempool.h"
#include "sl_mempool_config.h"
#include "sl_component_catalog.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include <assert.h>
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#if defined(SL_CATALOG_KERNEL_PRESENT)
/// Release mutex and return macro function
#define __release_mutex_and_ret() \
  do {                            \
    _mempool_mtx_release();       \
    return;                       \
  } while (0)

/// Release mutex and return with value macro function
#define __release_mutex_and_retval(__retval) \
  do {                                       \
    _mempool_mtx_release();                  \
    return __retval;                         \
  } while (0)
#endif

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Init block handler
 * @details Helper function
 * @param[out] block Block handler pointer
 *****************************************************************************/
static inline void _init_block(sl_mempool_block_hnd_t * const block);

/**************************************************************************//**
 * @brief Get next free block handler
 * @details Helper function
 * @return sl_mempool_block_hnd_t* Block handler pointer on succes
 *                                 or NULL on error
 *****************************************************************************/
static sl_mempool_block_hnd_t *_get_next_free_block_hnd(void);

/**************************************************************************//**
 * @brief Get block handler by address
 * @details Helper function
 * @param[in] addr Address
 * @param[in] memp Memory Pool object
 * @return sl_mempool_block_hnd_t* Block handler pointer on succes
 *                                 or NULL on error
 *****************************************************************************/
static sl_mempool_block_hnd_t *_get_block_hnd_by_addr(const void * const addr,
                                                      const sl_mempool_t * const memp);

#if defined(SL_CATALOG_KERNEL_PRESENT)
/**************************************************************************//**
 * @brief Memory Pool mutex acquire
 * @details Helper function
 *****************************************************************************/
static inline void _mempool_mtx_acquire(void);

/**************************************************************************//**
 * @brief Memory Pool mutex release
 * @details Helper function
 *****************************************************************************/
static inline void _mempool_mtx_release(void);
#endif

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Block handler storage static allocation
static sl_mempool_block_hnd_t _blocks[SL_MEMPOOL_MAX_RUNTIME_AVAILABLE_BLOCKS] = { 0 };

#if defined(SL_CATALOG_KERNEL_PRESENT)

/// Memory Pool mutex ID
static osMutexId_t _mempool_mtx = NULL;

/// Memory Pool mutex control block allocation
__ALIGNED(8) static uint8_t _mempool_mtx_cb[osMutexCbSize] = { 0 };

/// Memory Poll mutex attribute structure
static const osMutexAttr_t _mempool_mtx_attr = {
  .name      = "MemPool",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _mempool_mtx_cb,
  .cb_size   = sizeof(_mempool_mtx_cb)
};

#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_mempool_init(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  _mempool_mtx = osMutexNew(&_mempool_mtx_attr);
  assert(_mempool_mtx != NULL);
#endif

  sl_mempool_blocks_init();
}

void sl_mempool_blocks_init(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  osKernelState_t kernel_state = osKernelLocked;
  kernel_state = osKernelGetState();

  if (kernel_state == osKernelRunning) {
    _mempool_mtx_acquire();
  }
#endif

  for (size_t i = 0; i < SL_MEMPOOL_MAX_RUNTIME_AVAILABLE_BLOCKS; ++i) {
    _init_block(&_blocks[i]);
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (kernel_state == osKernelRunning) {
    _mempool_mtx_release();
  }
#endif
}

sl_status_t sl_mempool_create(sl_mempool_t *memp,
                              const size_t block_count,
                              const size_t block_size,
                              void * const buff,
                              const size_t buff_size)
{
  // Check arguments
  if (memp == NULL
      || buff == NULL
      || !buff_size
      || (block_count * block_size) > buff_size
      || (block_count * block_size) == 0) {
    return SL_STATUS_FAIL;
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  _mempool_mtx_acquire();
#endif

  memp->block_count = block_count;
  memp->block_size = block_size;
  memp->buff = buff;
  memp->buff_size = buff_size;
  memp->blocks = NULL;
  memp->used_block_count = 0;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  _mempool_mtx_release();
#endif

  return SL_STATUS_OK;
}

void * sl_mempool_alloc(sl_mempool_t * const memp)
{
  void * tmp_addr = NULL;
  sl_mempool_block_hnd_t *tmp_block = NULL;

  if (memp == NULL) {
    return NULL;
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  _mempool_mtx_acquire();
#endif

  // block list is full
  if (memp->used_block_count == memp->block_count) {
#if defined(SL_CATALOG_KERNEL_PRESENT)
    __release_mutex_and_retval(NULL);
#else
    return NULL;
#endif
  }

  for (size_t i = 0; i < memp->block_count; ++i) {
    tmp_addr = (void *)((uint8_t *)memp->buff + (i * memp->block_size));
    // find free address of the buffer
    if (_get_block_hnd_by_addr(tmp_addr, memp) != NULL) {
      continue;
    }
    tmp_block = memp->blocks;
    memp->blocks = _get_next_free_block_hnd();
    if (memp->blocks == NULL) {
      memp->blocks = tmp_block;
      // Weak handler to optionally solve the missing handler issue
#if defined(SL_CATALOG_KERNEL_PRESENT)
      __release_mutex_and_retval(sl_mempool_not_enough_runtime_block_handler(memp));
#else
      return sl_mempool_not_enough_runtime_block_handler(memp);
#endif
    }

    // init new block handler
    memp->blocks->start_addr = tmp_addr;
    memp->blocks->next = tmp_block;
    ++memp->used_block_count;
#if defined(SL_CATALOG_KERNEL_PRESENT)
    __release_mutex_and_retval(memp->blocks->start_addr);
#else
    return memp->blocks->start_addr;
#endif
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  _mempool_mtx_release();
#endif

  return NULL;
}

void sl_mempool_free(sl_mempool_t * const memp, const void * const addr)
{
  sl_mempool_block_hnd_t *block = NULL;
  sl_mempool_block_hnd_t *prev_block = NULL;

  block = memp->blocks;
  prev_block = block;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  _mempool_mtx_acquire();
#endif

  while (block != NULL) {
    if (block->start_addr == addr) {
      // remove block
      prev_block->next = block->next;

      if (block == memp->blocks) {
        memp->blocks = block->next;
      }

      _init_block(block);

      --memp->used_block_count;
      break;
    }
    prev_block = block;
    block = block->next;
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  _mempool_mtx_release();
#endif
}

SL_WEAK void *sl_mempool_not_enough_runtime_block_handler(sl_mempool_t *memp)
{
  (void) memp;
  return NULL;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static inline void _init_block(sl_mempool_block_hnd_t * const block)
{
  block->next = NULL;
  block->start_addr = NULL;
}

static sl_mempool_block_hnd_t *_get_next_free_block_hnd(void)
{
  sl_mempool_block_hnd_t *res = NULL;

  for (size_t i = 0; i < SL_MEMPOOL_MAX_RUNTIME_AVAILABLE_BLOCKS; ++i) {
    if (_blocks[i].start_addr == NULL) {
      res = &_blocks[i];
      break;
    }
  }

  return res;
}

static sl_mempool_block_hnd_t * _get_block_hnd_by_addr(const void * const addr,
                                                       const sl_mempool_t * const memp)
{
  sl_mempool_block_hnd_t *block = NULL;

  block = memp->blocks;

  while (block != NULL) {
    if (addr == block->start_addr) {
      return block;
    }
    block = block->next;
  }

  return NULL;
}

#if defined(SL_CATALOG_KERNEL_PRESENT)
static inline void _mempool_mtx_acquire(void)
{
  assert(osMutexAcquire(_mempool_mtx, osWaitForever) == osOK);
}

static inline void _mempool_mtx_release(void)
{
  assert(osMutexRelease(_mempool_mtx) == osOK);
}
#endif
