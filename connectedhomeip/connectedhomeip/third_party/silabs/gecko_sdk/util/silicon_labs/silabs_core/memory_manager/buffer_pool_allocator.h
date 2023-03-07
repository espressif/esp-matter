/***************************************************************************//**
 * @file
 * @brief This is a simple memory allocator that uses a build time defined pool
 *   of constant sized buffers. It's a very simple allocator, but one that can
 *   be easily used in any application.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef BUFFER_POOL_ALLOCATOR_H__
#define BUFFER_POOL_ALLOCATOR_H__

// Get the standard include types
#include <stdint.h>

/**
 * Allocate a buffer with at least the number of bytes specified. If there is
 * not enough space then this function will return NULL.
 * @param size The number of bytes to allocate for this buffer
 * @return Returns a handle to a buffer at least size bytes long or NULL if no
 *   buffer could be allocated.
 */
void *memoryAllocate(uint32_t size);

/**
 * Free the buffer pointed to by handle. This will only decrement the reference
 * counter for this buffer. The memory is not freed until the reference counter
 * reaches zero.
 * @param handle The handle to free. Must match the value returned by
 *   the memoryAllocate() function.
 */
void memoryFree(void *handle);

/**
 * Take a memory handle and get the data pointer associated with it.
 * @param handle The handle to get the pointer for. Must match the value
 *   returned by the memoryAllocate() function.
 * @return Returns the pointer to memory associated with the provided memory
 *   handle. Returns NULL if passed an invalid or unallocated handle.
 */
void *memoryPtrFromHandle(void *handle);

/**
 * Increment the reference counter on the memory pointed to by handle. After
 * doing this there will have to be an additional call to memoryFree() to
 * release the memory.
 * @param handle The handle to the object which needs its reference count
 *   increased. Must match the value returned by the memoryAllocate() function.
 */
void memoryTakeReference(void *handle);

#endif // BUFFER_POOL_ALLOCATOR_H__
