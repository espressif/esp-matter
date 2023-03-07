/***************************************************************************//**
 * @file
 * @brief Simple code to link this memory manager with a RAIL application by
 *        implementing the appropriate callbacks.
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

#include <stdint.h>
#include "rail.h"
#include "buffer_pool_allocator.h"

#ifdef  RAIL_STATE_UINT32_BUFFER_SIZE // New symbol created in RAIL 2.0
#error  "rail_integration.c is only compatible with RAIL prior to 2.0"
#endif//RAIL_STATE_UINT32_BUFFER_SIZE

/// Rely on the pool allocator's allocate function to get memory
void *RAILCb_AllocateMemory(uint32_t size)
{
  return memoryAllocate(size);
}

/// Use the pool allocator's free function to return the memory to the pool
void RAILCb_FreeMemory(void *ptr)
{
  memoryFree(ptr);
}

/// Get the memory pointer for this handle and offset into it as requested
void *RAILCb_BeginWriteMemory(void *handle,
                              uint32_t offset,
                              uint32_t *available)
{
  void *memoryPtr = memoryPtrFromHandle(handle);
  if (memoryPtr == NULL) {
    *available = 0;
    return NULL;
  }
  return (void *)(((uint8_t *)memoryPtr) + offset);
}

/// We don't need to track the completion of a memory write so do nothing
void RAILCb_EndWriteMemory(void *handle, uint32_t offset, uint32_t size)
{
}
