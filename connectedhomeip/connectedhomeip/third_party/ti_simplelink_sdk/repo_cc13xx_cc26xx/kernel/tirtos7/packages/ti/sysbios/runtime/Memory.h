/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*!
 * @file ti/sysbios/runtime/Memory.h
 * @brief Static and run-time memory manager
 *
 * All memory allocations are performed either by 
 * @link Memory_alloc @endlink (or its varients @link Memory_calloc @endlink,
 * @link Memory_valloc @endlink, etc.) at run-time.
 *
 * Run-time memory management is performed by modules that implement the
 * ti/sysbios/runtime/IHeap.h interface.  The Memory module itself
 * simply provides a common interface for any variety of system and application
 * specific memory management policies implemented by IHeap modules.
 *
 * Heap instances are created statically or dynamically via heap specific create
 * functions and these instances are then passed as an input parameter to the
 * Memory calls that have an IHeap_Handle parameter.
 */

/*
 * ======== Memory.h ========
 */

#ifndef ti_sysbios_runtime_Memory__include
#define ti_sysbios_runtime_Memory__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_Memory_long_names
#include "Memory_defs.h"

#define Memory_Size size_t   /* for BIOS 6.x compatibility */
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Blocking quality
 *
 * Heaps with this "quality" may cause the
 * calling thread to block; i.e., suspend execution until another thread leaves
 * the gate.
 */
#define Memory_Q_BLOCKING (1)

/*!
 * @brief Memory heap statistics
 *
 * This structure defines generic statistics that must be supplied by each
 * module that implements the ti/sysbios/runtime/IHeap.h interface.
 */
typedef struct {
    /*!
     * @brief total size (in MADUs) of heap.
     */
    size_t totalSize;
    /*!
     * @brief current size (in MADUs) of free memory in the heap
     */
    size_t totalFreeSize;
    /*!
     * @brief current largest contiguous free block (in MADUs)
     */
    size_t largestFreeSize;
} Memory_Stats;

typedef void * (*IHeap_AllocFxn) (void *, size_t, size_t, Error_Block *);
typedef void (*IHeap_FreeFxn) (void *, void *, size_t);
typedef bool (*IHeap_IsBlockingFxn) (void *);
typedef void (*IHeap_GetStatsFxn) (void *, Memory_Stats *);

/*! @cond NODOC */
/*
 * TODO -- should be in IHeap.h -- break the circular dependency
 * between Memory.h and IHeap.h
 */
typedef struct IHeap_Object {
    IHeap_AllocFxn alloc;
    IHeap_FreeFxn free;
    IHeap_IsBlockingFxn isBlocking;
    IHeap_GetStatsFxn getStats; 
} IHeap_Object, *IHeap_Handle;
/*! @endcond */

/*!
 * @brief The default heap.
 *
 * If no heap is specified in the Memory module's methods (i.e. heap ==
 * NULL) defaultHeapInstance is used. 
 */
extern IHeap_Handle Memory_defaultHeapInstance;

/*!
 * @brief Allocate a block of memory from a heap.
 *
 * The heap is created by a module that implements the
 * ti/sysbios/runtime/IHeap.h interface. If heap is NULL, the @link
 * Memory_defaultHeapInstance @endlink is used.
 *
 * A value of 0 denotes maximum default type alignment.
 *
 * If the allocation was successful, Memory_alloc() returns non-NULL pointer
 * to the allocated and uninitialized block; otherwise it returns NULL and the
 * error block will indicate the cause of the error.
 *
 * @param heap heap from which the memory is allocated
 * @param size requested memory block size (in MADUs)
 * @param align alignment (in MADUs) of the block of memory
 * @param eb pointer to error block
 * 
 * @retval pointer to allocated memory (NULL on failure)
 */
extern void * Memory_alloc(IHeap_Handle heap, size_t size, size_t align, Error_Block *eb);

/*!
 * @brief Allocate a block of memory from a heap and zero out the contents.
 *
 * The heap is created by a module that implements the
 * ti/sysbios/runtime/IHeap.h interface. If heap is NULL, the @link
 * Memory_defaultHeapInstance @endlink is used.
 *
 * A value of 0 denotes maximum default type alignment.
 *
 * If the allocation was successful, Memory_calloc() returns non-NULL
 * pointer to the allocated and initialized block; otherwise it returns NULL
 * and the error block will indicate the cause of the error.
 *
 * @param heap heap from which the memory is allocated
 * @param size requested memory block size (in MADUs)
 * @param align alignment (in MADUs) of the block of memory
 * @param eb pointer to error block
 * 
 * @retval pointer to allocated memory (NULL on failure)
 */
extern void * Memory_calloc(IHeap_Handle heap, size_t size, size_t align, Error_Block *eb);

/*!
 * @brief Frees the space if the heap manager offers such functionality.
 *
 * The heap is created by a module that implements the
 * ti/sysbios/runtime/IHeap.h interface. If heap is NULL, the @link
 * Memory_defaultHeapInstance @endlink is used.
 *
 * @param heap heap that the block of memory will be freed back to.
 * @param block block of memory to free back to the heap
 * @param size size (in MADUs) of the block of memory to free.
 */
extern void Memory_free(IHeap_Handle heap, void * block, size_t size);

/*!
 * @brief Return the largest alignment required by the target
 *
 * Returns the largest alignment required for all the
 * standard base types supported by the current target
 *
 * @retval Returns target-specific alignment in MADUs. 
 */
extern size_t Memory_getMaxDefaultTypeAlign(void);

/*!
 * @brief Obtain statistics from a heap.
 *
 * The heap is created by a module that implements the
 * ti/sysbios/runtime/IHeap.h interface. If heap is NULL, the @link
 * Memory_defaultHeapInstance @endlink is used.
 *
 * @param heap the heap to get the statistics from
 * @param stats the output buffer for the returned statistics
 */
extern void Memory_getStats(IHeap_Handle heap, Memory_Stats *stats);

/*!
 * @brief Test for a particular heap quality.
 *
 * There currently is only one quality, namely @link Memory_Q_BLOCKING @endlink.
 *
 * The heap is created by a module that implements the 
 * ti/sysbios/runtime/IHeap.h interface.  If heap is NULL, the @link
 * Memory_defaultHeapInstance @endlink is queried
 *
 * For example: @link Memory_Q_BLOCKING @endlink.
 *
 * If heap has the "qual" quality, this method returns true, otherwise it
 * returns false.
 *
 * @param heap the heap to query
 * @param qual quality to test
 * 
 * @retval quality
 */
extern bool Memory_query(IHeap_Handle heap, int qual);

/*!
 * @brief Get the default heap handle
 *
 * When Memory_alloc(), etc. are passed 'NULL' as the heap parameter, the
 * default heap is used. This API can be used to get the default heap handle.
 */
extern IHeap_Handle Memory_getDefaultHeap(void);

/*!
 * @brief Set the default heap handle
 *
 * When Memory_alloc(), etc. are passed 'NULL' as the heap parameter, the
 * default heap is used. This API can be used to set this default handle to
 * a user defined heap. Note that most use cases can be managed with the
 * BIOS.heapType and BIOS.heapSize configuration parameters.
 */
extern void Memory_setDefaultHeap(IHeap_Handle heap);

/*!
 * @brief Allocate a block of memory from a heap and initialize the contents to
 * the value specified.
 *
 * The heap is created by a module that implements the
 * ti/sysbios/runtime/IHeap.h interface. If heap is NULL, the @link
 * Memory_defaultHeapInstance @endlink is used.
 *
 * A value of 0 denotes maximum default type alignment.
 *
 * If the allocation was successful, Memory_valloc() returns non-NULL
 * pointer to the allocated and initialized block; otherwise it returns `NULL`
 * and the error block will indicate the cause of the error.
 *
 * @param heap heap from which the memory is allocated
 * @param size requested memory block size (in MADUs)
 * @param align alignment (in MADUs) of the block of memory
 * @param value value to initialize the contents of the block
 * @param eb pointer to error block
 * 
 * @retval pointer to allocated memory (NULL on failure)
 */
extern void * Memory_valloc(IHeap_Handle heap, size_t size, size_t align, char value, Error_Block *eb);

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_runtime_Memory__include */

/*! @cond NODOC */
#undef ti_sysbios_runtime_Memory_long_names
#include "Memory_defs.h"
/*! @endcond */
