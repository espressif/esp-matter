/***************************************************************************//**
 * @file
 * @brief Heap and stack allocation
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdint.h>
#include "sl_memory.h"
#include "sl_memory_config.h"

#if defined(__GNUC__)
  /* Declare stack object used with gcc */
  static char sl_stack[SL_STACK_SIZE] __attribute__ ((aligned(8), used, section(".stack")));

  /* Declare the minimum heap object used with gcc */
  #if SL_HEAP_SIZE > 0
    static char sl_heap[SL_HEAP_SIZE]   __attribute__ ((aligned(8), used, section(".heap")));
  #endif

  /*
   * Declare the base and limit of the full heap region used with gcc. To make
   * use of otherwise unused memory, the total heap region be larger than the
   * minimum heap allocation above.
   */
  extern char __HeapBase[];
  extern char __HeapLimit[];

#elif defined(__ICCARM__)
  /* Declare stack object used with iar */
  __root char sl_stack[SL_STACK_SIZE] @ ".stack";

  /* Declare the minimum heap object used with iar */
  #if SL_HEAP_SIZE > 0
    __root char sl_heap[SL_HEAP_SIZE] @ ".heap";
  #endif
  #pragma section="HEAP"

#endif

sl_memory_region_t sl_memory_get_stack_region(void)
{
  sl_memory_region_t region;

  region.addr = &sl_stack;
  region.size = SL_STACK_SIZE;
  return region;
}

sl_memory_region_t sl_memory_get_heap_region(void)
{
  sl_memory_region_t region;

  /*
   * Report the actual heap region which may be larger then the minimum
   * allocation of SL_HEAP_SIZE bytes
   */
#if defined(__GNUC__)
  region.addr = __HeapBase;
  region.size = (size_t) ((uintptr_t) __HeapLimit - (uintptr_t) __HeapBase);

#elif defined(__ICCARM__)
  region.addr = __section_begin("HEAP");
  region.size = __section_size("HEAP");

#endif

  return region;
}

#if defined(__GNUC__)
void * _sbrk(int incr)
{
  static char *heap_end = __HeapBase;
  char *prev_heap_end;

  if ((heap_end + incr) > __HeapLimit) {
    // Not enough heap
    return (void *) -1;
  }

  prev_heap_end = heap_end;
  heap_end += incr;

  return prev_heap_end;
}
#endif
