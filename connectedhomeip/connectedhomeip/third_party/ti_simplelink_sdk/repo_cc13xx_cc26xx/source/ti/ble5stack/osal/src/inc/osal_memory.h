/******************************************************************************

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @file  osal_memory.h
 *  @brief      his module defines the OSAL memory control functions.
 */

#ifndef OSAL_MEMORY_H
#define OSAL_MEMORY_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "comdef.h"

/*********************************************************************
 * CONSTANTS
 */

#if !defined ( OSALMEM_METRICS )
  #define OSALMEM_METRICS  FALSE      //!< Set to TRUE to gather OSAL heap metrics
#endif

/*********************************************************************
 * MACROS
 */

/// @cond NODOC
#define osal_stack_used()  OnBoard_stack_used()

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/**
 * @brief       This function is the OSAL heap memory management initialization callback.
 */
  void osal_mem_init( void );

/**
 * @brief       This function is the OSAL task initialization callback.
 *
 * Kick the ff1 pointer out past the long-lived OSAL Task blocks.
 * Invoke this once after all long-lived blocks have been allocated -
 * presently at the end of osal_init_system().
 */
  void osal_mem_kick( void );

/// @endcond //NODOC

 /*
  * Allocate a block of memory.
  */
#ifdef DPRINTF_OSALHEAPTRACE
  void *osal_mem_alloc_dbg( uint16 size, const char *fname, unsigned lnum );
#define osal_mem_alloc(_size ) osal_mem_alloc_dbg(_size, __FILE__, __LINE__)
#else /* DPRINTF_OSALHEAPTRACE */
/**
 * @brief Implement the OSAL dynamic memory allocation functionality.
 *
 * @param size the number of bytes to allocate from the HEAP.
 */
  void *osal_mem_alloc( uint16 size );
#endif /* DPRINTF_OSALHEAPTRACE */

/**
* @brief Implement the OSAL dynamic memory allocation functionality.
* However, a first check is done to ensure that enough heap will remain after the allocation
*
* @param size the number of bytes to allocate from the HEAP.
*/
void *osal_mem_allocLimited( uint16 size );

 /*
  * Free a block of memory.
  */
#ifdef DPRINTF_OSALHEAPTRACE
  void osal_mem_free_dbg( void *ptr, const char *fname, unsigned lnum );
#define osal_mem_free(_ptr ) osal_mem_free_dbg(_ptr, __FILE__, __LINE__)
#else /* DPRINTF_OSALHEAPTRACE */
/**
 * @brief Implement the OSAL dynamic memory de-allocation functionality.
 *
 * @param ptr A valid pointer (i.e. a pointer returned by @ref osal_mem_alloc()) to the memory to free.
 */
  void osal_mem_free( void *ptr );
#endif /* DPRINTF_OSALHEAPTRACE */

#if ( OSALMEM_METRICS )
/**
 * @brief Return the maximum number of blocks ever allocated at once.
 *
 * @return Maximum number of blocks ever allocated at once.
 */
  uint16 osal_heap_block_max( void );

/**
 * @brief   Return the current number of blocks now allocated.
 *
 * @return  Current number of blocks now allocated.
 */
  uint16 osal_heap_block_cnt( void );

/**
 * @brief   Return the current number of free blocks.
 *
 * @return  Current number of free blocks.
 */
  uint16 osal_heap_block_free( void );

/**
 * @brief   Return the current number of bytes allocated.
 *
 * @return  Current number of bytes allocated.
 */
  uint16 osal_heap_mem_used( void );
#endif

#if defined (ZTOOL_P1) || defined (ZTOOL_P2)
/**
 * @brief   Return the highest byte ever allocated in the heap.
 *
 * @return  Highest number of bytes ever used by the stack.
 */
  uint16 osal_heap_high_water( void );
#endif

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef OSAL_MEMORY_H */
