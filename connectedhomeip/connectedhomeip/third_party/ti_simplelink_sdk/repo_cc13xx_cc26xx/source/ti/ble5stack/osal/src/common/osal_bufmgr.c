/******************************************************************************

 @file  osal_bufmgr.c

 @brief This file contains the buffer management APIs. These APIs are not
        reentrant hence cannot be called from an interrupt context.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2008-2022, Texas Instruments Incorporated
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


/*********************************************************************
 * INCLUDES
 */
#include "osal.h"
#include "onboard.h"
#include "osal_bufmgr.h"

/*********************************************************************
 * MACROS
 */
// 'bd_ptr' used with these macros must be of the type 'bm_desc_t *'
#define START_PTR( bd_ptr )  ( (bd_ptr) + 1 )
#define END_PTR( bd_ptr )    ( (uint8 *)START_PTR( bd_ptr ) + (bd_ptr)->payload_len )

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct bm_desc
{
  struct bm_desc *next_ptr;    // pointer to next buffer descriptor
  uint16          payload_len; // length of user's buffer
} bm_desc_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Linked list of allocated buffer descriptors
static bm_desc_t *bm_list_ptr = NULL;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bm_desc_t *bm_desc_from_payload ( uint8 *payload_ptr );

/*********************************************************************
 * @fn      osal_bm_alloc
 *
 * @brief   Implementation of the allocator functionality.
 *
 *          WARNING: THIS FUNCTION CAN BE CALLED ONLY FROM THE BOTTOM
 *                   LAYER OF THE STACK (HCI FOR DUAL MODE AND LINK
 *                   LAYER FOR SINGLE MODE).
 *
 * @param   size - number of bytes to allocate from the heap.
 *
 * @return  pointer to the heap allocation; NULL if error or failure.
 */
void *osal_bm_alloc( uint16 size )
{
  halIntState_t  cs;
  bm_desc_t     *bd_ptr;

  HAL_ENTER_CRITICAL_SECTION(cs);

  bd_ptr = osal_mem_alloc( sizeof( bm_desc_t ) + size );

  if ( bd_ptr != NULL )
  {
    // set the buffer descriptor info
    bd_ptr->payload_len  = size;

    // add item to the beginning of the list
    bd_ptr->next_ptr = bm_list_ptr;
    bm_list_ptr = bd_ptr;

    // return start of the buffer
    bd_ptr = START_PTR( bd_ptr );
  }

  HAL_EXIT_CRITICAL_SECTION(cs);

  return ( (void *)bd_ptr );
}

/*********************************************************************
 * @fn      osal_bm_free
 *
 * @brief   Implementation of the de-allocator functionality.
 *
 * @param   payload_ptr - pointer to the memory to free.
 *
 * @return  none
 */
void osal_bm_free( void *payload_ptr )
{
  halIntState_t cs;
  bm_desc_t *loop_ptr;
  bm_desc_t *prev_ptr;

  HAL_ENTER_CRITICAL_SECTION(cs);

  prev_ptr = NULL;

  loop_ptr = bm_list_ptr;
  while ( loop_ptr != NULL )
  {
    if ( payload_ptr >= (void *)START_PTR( loop_ptr ) &&
         payload_ptr <= (void *)END_PTR( loop_ptr) )
    {
      // unlink item from the linked list
      if ( prev_ptr == NULL )
      {
        // it's the first item on the list
        bm_list_ptr = loop_ptr->next_ptr;
      }
      else
      {
        prev_ptr->next_ptr = loop_ptr->next_ptr;
      }

      // free the memory

      osal_mem_free( loop_ptr );

      // we're done here
      break;
    }

    // move on to next item
    prev_ptr = loop_ptr;
    loop_ptr = loop_ptr->next_ptr;
  }

  HAL_EXIT_CRITICAL_SECTION(cs);

  return;
}

/*********************************************************************
 * @fn      osal_bm_adjust_header
 *
 * @brief   Add or remove header space for the payload pointer. A positive
 *          adjustment adds header space, and negative removes header space.
 *
 * @param   payload_ptr - pointer to payload
 * @param   size - +/- number of bytes to move (affecting header area)
 *
 * @return  pointer to payload at the new adjusted location
 */
void *osal_bm_adjust_header( void *payload_ptr, int16 size )
{
  bm_desc_t *bd_ptr;
  uint8 *new_payload_ptr;

  bd_ptr = bm_desc_from_payload( (uint8 *)payload_ptr );
  if ( bd_ptr != NULL )
  {
    new_payload_ptr = (uint8 *)( (uint8 *)payload_ptr - size );

    // make sure the new payload is within valid range
    if ( new_payload_ptr >= (uint8 *)START_PTR( bd_ptr ) &&
         new_payload_ptr <= (uint8 *)END_PTR( bd_ptr ) )
    {
      // return new payload pointer
      return ( (void *)new_payload_ptr );
    }
  }

  // return original value
  return ( payload_ptr );
}

/*********************************************************************
 * @fn      osal_bm_adjust_tail
 *
 * @brief   Add or remove tail space for the payload pointer. A positive
 *          adjustment adds tail space, and negative removes tail space.
 *
 * @param   payload_ptr - pointer to payload
 * @param   size - +/- number of bytes to move (affecting header area)
 *
 * @return  pointer to payload at the new adjusted location
 */
void *osal_bm_adjust_tail( void *payload_ptr, int16 size )
{
  bm_desc_t *bd_ptr;
  uint8 *new_payload_ptr;

  bd_ptr = bm_desc_from_payload( (uint8 *)payload_ptr );
  if ( bd_ptr != NULL )
  {
    new_payload_ptr = (uint8 *)END_PTR( bd_ptr ) - size;

    // make sure the new payload is within valid range
    if ( new_payload_ptr >= (uint8 *)START_PTR( bd_ptr ) &&
         new_payload_ptr <= (uint8 *)END_PTR( bd_ptr ) )
    {
      // return new payload pointer
      return ( (void *)new_payload_ptr );
    }
  }

  // return original value
  return ( payload_ptr );
}

/*********************************************************************
 * @fn      bm_desc_from_payload
 *
 * @brief   Find buffer descriptor from payload pointer
 *
 * @param   payload_ptr - pointer to payload
 *
 * @return  pointer to buffer descriptor
 */
static bm_desc_t *bm_desc_from_payload ( uint8 *payload_ptr )
{
  halIntState_t cs;
  bm_desc_t *loop_ptr;

  HAL_ENTER_CRITICAL_SECTION(cs);

  loop_ptr = bm_list_ptr;
  while ( loop_ptr != NULL )
  {
    if ( payload_ptr >= (uint8 *)START_PTR( loop_ptr ) &&
         payload_ptr <= (uint8 *)END_PTR( loop_ptr) )
    {
      // item found
      break;
    }

    // move on to next item
    loop_ptr = loop_ptr->next_ptr;
  }

  HAL_EXIT_CRITICAL_SECTION(cs);
  
  return ( loop_ptr );
}


/****************************************************************************
****************************************************************************/
