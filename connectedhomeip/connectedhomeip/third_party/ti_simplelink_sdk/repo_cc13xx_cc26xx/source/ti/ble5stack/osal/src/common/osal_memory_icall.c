/******************************************************************************

 @file  osal_memory_icall.c

 @brief OSAL Heap Memory management functions implemented on top of
        ICall primitive service.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "comdef.h"
#include "osal.h"
#include "osal_memory.h"

#ifdef ICALL_JT
#include "icall_jt.h"
#else
#include <icall.h>
#endif /* ICALL_JT */

/**************************************************************************************************
 * @fn          osal_mem_alloc
 *
 * @brief       This function implements the OSAL dynamic memory allocation functionality.
 *
 * input parameters
 *
 * @param size - the number of bytes to allocate from the HEAP.
 *
 * output parameters
 *
 * None.
 *
 * @return      Pointer to dynamically allocated memory, or NULL if operation
 *              failed.
 */
#ifdef DPRINTF_OSALHEAPTRACE
void *osal_mem_alloc_dbg( uint16 size, const char *fname, unsigned lnum )
#else
void *osal_mem_alloc( uint16 size )
#endif /* DPRINTF_OSALHEAPTRACE */
{
  uint8 *buf;

  // Attempt to allocate memory.
  buf = ICall_malloc(size);

#ifdef MEM_ALLOC_ASSERT
  // If allocation failed, assert.
  if ( buf == NULL )
  {
    // When EXT_HAL_ASSERT is defined, set the assert cause.  Otherwise, trap
    // the assert with argument FALSE.
#ifdef EXT_HAL_ASSERT
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
#else /* !EXT_HAL_ASSERT */
    HAL_ASSERT( FALSE );
#endif /* EXT_HAL_ASSERT */
  }
#endif /* MEM_ALLOC_ASSERT */

  // Return pointer to buffer.
  return (void *)buf;
}

/**************************************************************************************************
 * @fn          osal_mem_allocLimited
 *
 * @brief       This function implements the OSAL dynamic memory allocation functionality.
 *              However, first a check is done to ensure that enough heap will remain after the allocation
 *
 * input parameters
 *
 * @param size - the number of bytes to allocate from the HEAP.
 *
 * output parameters
 *
 * None.
 *
 * @return      Pointer to dynamically allocated memory, or NULL if operation
 *              failed.
 */
void *osal_mem_allocLimited( uint16 size )
{
  uint8 *buf;

  // Attempt to allocate memory.
  buf = ICall_mallocLimited(size);

#ifdef MEM_ALLOC_ASSERT
  // If allocation failed, assert.
  if ( buf == NULL )
  {
    // When EXT_HAL_ASSERT is defined, set the assert cause.  Otherwise, trap
    // the assert with argument FALSE.
#ifdef EXT_HAL_ASSERT
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
#else /* !EXT_HAL_ASSERT */
    HAL_ASSERT( FALSE );
#endif /* EXT_HAL_ASSERT */
  }
#endif /* MEM_ALLOC_ASSERT */

  // Return pointer to buffer.
  return (void *)buf;
}

/**************************************************************************************************
 * @fn          osal_mem_free
 *
 * @brief       This function implements the OSAL dynamic memory de-allocation functionality.
 *
 * input parameters
 *
 * @param ptr - A valid pointer (i.e. a pointer returned by osal_mem_alloc()) to the memory to free.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
#ifdef DPRINTF_OSALHEAPTRACE
void osal_mem_free_dbg(void *ptr, const char *fname, unsigned lnum)
#else /* DPRINTF_OSALHEAPTRACE */
void osal_mem_free(void *ptr)
#endif /* DPRINTF_OSALHEAPTRACE */
{
  ICall_free(ptr);
}

/**************************************************************************************************
*/
