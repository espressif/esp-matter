/***************************************************************************//**
 * @file
 * @brief Wi-SUN CoAP memory handler module
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "sli_wisun_coap_mem.h"
#include <assert.h>
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/* Using static memory pool */
#if SL_WISUN_COAP_MEM_USE_STATIC_MEMORY_POOL

#include "sl_mempool.h"

/// Internal CoAP memory pool handler structure
typedef struct _coap_mem {
  /// ID
  uint32_t id;
  /// Memory Pool object
  sl_mempool_t mempool;
  /// Size of the blocks
  uint16_t block_size;
  /// Count of the allocated blocks
  uint16_t block_count;
  /// buffer pointer
  uint8_t *buff;
} _coap_mem_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Very Low memory buffer
static uint8_t _mem0_very_low
[(WISUN_COAP_MEMORY_VERY_LOW_SIZE * WISUN_COAP_MEMORY_VERY_LOW_COUNT)] = { 0 };

/// Low memory buffer
static uint8_t _mem1_low
[(WISUN_COAP_MEMORY_LOW_SIZE      * WISUN_COAP_MEMORY_LOW_COUNT)] = { 0 };

/// Medium memory buffer
static uint8_t _mem2_medium
[(WISUN_COAP_MEMORY_MEDIUM_SIZE   * WISUN_COAP_MEMORY_MEDIUM_COUNT)] = { 0 };

/// High memory buffer
static uint8_t _mem3_high
[(WISUN_COAP_MEMORY_HIGH_SIZE     * WISUN_COAP_MEMORY_HIGH_COUNT)] = { 0 };

/// Memory handler array
static _coap_mem_t _mem[WISUN_COAP_MEMORY_OPTION_COUNT] = {
  {
    .id          = WISUN_COAP_MEMORY_VERY_LOW_ID,
    .block_size  = WISUN_COAP_MEMORY_VERY_LOW_SIZE,
    .block_count = WISUN_COAP_MEMORY_VERY_LOW_COUNT,
    .buff        = _mem0_very_low
  },
  {
    .id          = WISUN_COAP_MEMORY_LOW_ID,
    .block_size  = WISUN_COAP_MEMORY_LOW_SIZE,
    .block_count = WISUN_COAP_MEMORY_LOW_COUNT,
    .buff        = _mem1_low
  },
  {
    .id          = WISUN_COAP_MEMORY_MEDIUM_ID,
    .block_size  = WISUN_COAP_MEMORY_MEDIUM_SIZE,
    .block_count = WISUN_COAP_MEMORY_MEDIUM_COUNT,
    .buff        = _mem2_medium
  },
  {
    .id          = WISUN_COAP_MEMORY_HIGH_ID,
    .block_size  = WISUN_COAP_MEMORY_HIGH_SIZE,
    .block_count = WISUN_COAP_MEMORY_HIGH_COUNT,
    .buff        = _mem3_high,
  }
};

/* Init Static memory pool */
void _wisun_coap_mem_init(void)
{
  for (uint8_t i = 0; i < WISUN_COAP_MEMORY_OPTION_COUNT; ++i) {
    // create memory poo
    assert(sl_mempool_create(&_mem[i].mempool,
                             _mem[i].block_count,
                             _mem[i].block_size,
                             _mem[i].buff,
                             _mem[i].block_count * _mem[i].block_size) == SL_STATUS_OK);
  }
}

void *sl_mempool_not_enough_runtime_block_handler(sl_mempool_t *memp)
{
  (void) memp;
  assert("[Not enough runtime memorypool handler]" == NULL);
  return NULL;
}
#else

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/* Using dynamic memory allocation which provided by OS */
/**************************************************************************//**
 * @brief OS malloc
 * @details based on the OS (FreeRTOS or Micrium OS)
 * @param size size
 * @return void* allocated memory ptr, on failure NULL
 *****************************************************************************/
static inline void* __os_malloc(size_t size);

/**************************************************************************//**
 * @brief OS free
 * @details based on the OS (FreeRTOS or Micrium OS)
 * @param addr address to set free
 *****************************************************************************/
static inline void  __os_free(void *addr);

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* os malloc */
static inline void* __os_malloc(size_t size)
{
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  // FreeRTOS
  return pvPortMalloc(size);
#else
  // MicriumOS
  return sl_malloc(size);
#endif
}

/* os free */
static inline void __os_free(void *addr)
{
#if defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
  // FreeRTOS
  vPortFree(addr);
#else
  // MicriumOS
  sl_free(addr);
#endif
}
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Wi-SUN Coap malloc */
void *_wisun_coap_mem_malloc(size_t size)
{
  void *p = NULL;
  if (!size) {
    return NULL;
  }
// STATIC MEMORY POOL
#if SL_WISUN_COAP_MEM_USE_STATIC_MEMORY_POOL

  for (uint8_t i = 0; i < WISUN_COAP_MEMORY_OPTION_COUNT; ++i) {
    if (size <= _mem[i].block_size) {   // check size
      p = sl_mempool_alloc(&_mem[i].mempool);
      if (p != NULL) {   // if allocated, return, otherwise continue to try a bigger memory chunk
        return p;
      }
    }
  }

// OS MALLOC
#else
  p = __os_malloc(size);
#endif

  return p;
}

/* Wi-SUN Coap free */
void _wisun_coap_mem_free(void *addr)
{
  if (addr == NULL) {
    return;
  }

// STATIC MEMORY POOL
#if SL_WISUN_COAP_MEM_USE_STATIC_MEMORY_POOL

  for (uint8_t i = 0; i < WISUN_COAP_MEMORY_OPTION_COUNT; ++i) {
    if (sl_mempool_is_addr_in_buff(&_mem[i].mempool, addr)) {
      sl_mempool_free(&_mem[i].mempool, addr);
    }
  }

// OS FREE
#else
  __os_free(addr);
#endif
}
