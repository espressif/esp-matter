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
 * @file ti/sysbios/heaps/HeapTrack.h
 * @brief Heap manager that enables tracking of all allocated blocks.
 *
 * HeapTrack manager provides functions to allocate and free storage from a
 * configured heap which inherits from IHeap. The calling context is going to
 * match the heap being used.
 *
 * HeapTrack is useful for detecting memory leaks, double frees and buffer
 * overflows.  There is a performance overhead cost when using heap track as
 * well as a size impact. Every alloc will include a @link HeapTrack_Tracker
 * @endlink structure (plus memory to get proper alignment of the stucture) at
 * the end of the buffer that should not be modified by the user. It is
 * important to remember this when deciding on heap sizes and you may have to
 * adjust the total size or buffer sizes (for HeapBuf/HeapMultiBuf).
 *
 * ROV displays peaks and current in-use for both allocated memory (requested
 * size + Tracker structure) and requested memory (without Tracker).
 *
 * The information stored in the tracker packet is used to display information
 * in RTOS Object Viewer (ROV) as well as with the printTask and printHeap
 * functions.
 *
 * The asserts used in this module are listed below and include error checking
 * for double frees, calling printHeap with a null handle, buffer overflows and
 * deleting a non empty heap.
 */

/*
 * ======== HeapTrack.h ========
 */

#ifndef ti_sysbios_heaps_HeapTrack__include
#define ti_sysbios_heaps_HeapTrack__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_heaps_HeapTrack_long_names
#include "HeapTrack_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Assert raised when freeing a buffer that was already freed
 */
#define HeapTrack_A_doubleFree "Buffer already free"

/*!
 * @brief Assert raised when freeing memory with corrupted data or using the
 * wrong size
 */
#define HeapTrack_A_bufOverflow "Buffer overflow"

/*!
 * @brief Assert raised when deleting a non-empty heap
 */
#define HeapTrack_A_notEmpty "Heap not empty"

/*!
 * @brief Assert raised when calling printTask with a null HeapTrack object
 */
#define HeapTrack_A_nullObject "HeapTrack_printHeap called with null obj"

/*!
 * @brief Structure added to the end of each allocated block
 *
 * When a block is allocated from a HeapTrack heap with a size, internally
 * HeapTrack calls Memory_alloc on the configured heap.
 * The value of sizeof(HeapTrack_Tracker) is added to the requested size.
 *
 * For example, if the caller makes the following call (where heapHandle is an
 * HeapTrack handle that has been converted to an IHeap_Handle).
 *
 * @code
 *  buf = Memory_alloc(heapHandle, MYSIZE, MYALIGN, &eb);
 * @endcode
 *
 * Internally, HeapTrack will make the following call (where size is MYSIZE,
 * align is MYALIGN and obj is the HeapTrack handle).
 *
 * @code
 *  block = Memory_alloc(obj->heap, size + sizeof(HeapTrack_Tracker), align, &eb);
 * @endcode
 *
 * When using HeapTrack, depending on the  actual heap,
 * you might need to make adjustments to the heap
 * (e.g. increase the blockSize if using a HeapBuf instance).
 *
 * The HeapTrack module manages the contents of this structure and should not be
 * directly accessing them.
 */
typedef struct {
    uintptr_t scribble;
    Queue_Elem queElem;
    size_t size;
    uint32_t tick;
    Task_Handle taskHandle;
} HeapTrack_Tracker;

typedef struct {
    IHeap_Handle heap;
} HeapTrack_Params;

typedef struct {
    IHeap_Object iheap;
    Queue_Elem objElem;
    /*!
     * @brief Heap to use with HeapTrack
     */
    IHeap_Handle internalHeap;
    Queue_Struct trackQueue;
    size_t size;
    size_t peak;
    size_t sizeWithoutTracker;
    size_t peakWithoutTracker;
} HeapTrack_Struct, *HeapTrack_Handle;

typedef struct {
    Queue_Struct objQ;
} HeapTrack_Module_State;

/*! @cond NODOC */
#define ti_sysbios_heaps_HeapTrack_NOSCRIBBLE    0x05101920
#define ti_sysbios_heaps_HeapTrack_STARTSCRIBBLE 0xa5a5a5a5
/*! @endcond */

typedef HeapTrack_Struct HeapTrack_Object;

/*!
 * @brief cast handle to an IHeap_Handle for use by Memory_alloc, etc.
 *
 * @param handle heap handle
 *
 * @retval IHeap_Handle
 */
#define ti_sysbios_heaps_HeapTrack_Handle_upCast(handle) ((IHeap_Handle)(handle))

/*!
 * @brief Allocate a block of memory from the heap.
 *
 * @pre
 * The alignment must be a power of 2.
 */
extern void * HeapTrack_alloc(HeapTrack_Handle heap, size_t size, size_t align, Error_Block *eb);

/*!
 * @brief Free a block of memory back to the heap.
 */
extern void HeapTrack_free(HeapTrack_Handle heap, void * buf, size_t size);

/*!
 * @brief Can this heap block the caller
 *
 * `HeapTrack` returns `true` if the underlying heap is blocking.
 * `HeapTrack` returns `false` if the underlying heap not blocking.
 *
 * @param heap heap handle
 *
 * @retval `true` if the underlying heap if blocking, `false` otherwise
 */
extern bool HeapTrack_isBlocking(HeapTrack_Handle heap);

/*!
 * @brief get memory stats for a HeapTrack object
 *
 * @param heap heap handle
 * @param stats pointer to a @link Memory_Stats @endlink object
 */
extern void HeapTrack_getStats(HeapTrack_Handle heap, Memory_Stats *stats);

/*!
 * @brief Print out the blocks that are currently allocated by a task
 *
 * Iterates through all instances of HeapTrack and prints out information about
 * all allocated blocks of memory for a given task handle. This function is not
 * thread safe.
 */
extern void HeapTrack_printTask(Task_Handle task);

/*!
 * @brief Print details for a HeapTrack instance
 *
 * Print the details of all allocated blocks of memory for a HeapTrack instance.
 * This function is not thread safe.
 */
extern void HeapTrack_printHeap(HeapTrack_Handle heap);

/*!
 * @brief return handle of the first HeapTrack on HeapTrack list
 *
 * Return the handle of the first HeapTrack on the create/construct list. NULL if
 * no HeapTracks have been created or constructed.
 *
 * @retval HeapTrack handle
 */
extern HeapTrack_Handle HeapTrack_Object_first(void);

/*!
 * @brief return handle of the next HeapTrack on HeapTrack list
 *
 * Return the handle of the next HeapTrack on the create/construct list. NULL if
 * no more HeapTracks are on the list.
 *
 * @param heap HeapTrack handle
 *
 * @retval HeapTrack handle
 */
extern HeapTrack_Handle HeapTrack_Object_next(HeapTrack_Handle heap);

/*!
 * @brief Create a `HeapTrack` heap
 *
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapTrack handle (NULL on failure)
 */
extern HeapTrack_Handle HeapTrack_create(const HeapTrack_Params *params, Error_Block *eb);

/*!
 * @brief Construct a `HeapTrack` heap
 *
 * HeapTrack_construct is equivalent to HeapTrack_create except that the 
 * HeapTrack_Struct is pre-allocated.
 *
 * @param obj pointer to a HeapTrack object
 * @param params optional parameters
 *
 * @retval HeapTrack handle (NULL on failure)
 */
extern HeapTrack_Handle HeapTrack_construct(HeapTrack_Struct *obj, const HeapTrack_Params *params);

/*!
 * @brief Delete a `HeapTrack` heap
 *
 * Note that HeapTrack_delete takes a pointer to a HeapTrack_Handle which enables
 * HeapTrack_delete to set the HeapTrack handle to NULL.
 *
 * @param heap pointer to a HeapTrack handle
 */
extern void HeapTrack_delete(HeapTrack_Handle *heap);

/*!
 * @brief Destruct a `HeapTrack` heap
 *
 * @param obj pointer to a HeapTrack objects
 */
extern void HeapTrack_destruct(HeapTrack_Struct *obj);

/*!
 * @brief Initialize the HeapTrack_Params structure with default values.
 *
 * HeapTrack_Params_init initializes the HeapTrack_Params structure with default
 * values. HeapTrack_Params_init should always be called before setting
 * individual parameter fields. This allows new fields to be added in the future
 * with compatible defaults -- existing source code does not need to change when
 * new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void HeapTrack_Params_init(HeapTrack_Params *prms);

/*!
 * @brief return handle of the first HeapTrack on HeapTrack list
 *
 * Return the handle of the first HeapTrack on the create/construct list.
 * NULL if no HeapTracks have been created or constructed.
 *
 * @retval HeapTrack handle
 */
extern HeapTrack_Handle HeapTrack_Object_first(void);

/*!
 * @brief return handle of the next HeapTrack on HeapTrack list
 *
 * Return the handle of the next HeapTrack on the create/construct list.
 * NULL if no more HeapTracks are on the list.
 *
 * @param heap HeapTrack handle
 *
 * @retval HeapTrack handle
 */
extern HeapTrack_Handle HeapTrack_Object_next(HeapTrack_Handle heap);

static inline HeapTrack_Handle HeapTrack_handle(HeapTrack_Struct *str)
{
    return ((HeapTrack_Handle)str);
}

static inline HeapTrack_Struct * HeapTrack_struct(HeapTrack_Handle h)
{
    return ((HeapTrack_Struct *)h);
}

/*! @cond NODOC */
extern void HeapTrack_init(void);
#define HeapTrack_module ((HeapTrack_Module_State *) &(HeapTrack_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif

/*! @cond NODOC */
#undef ti_sysbios_heaps_HeapTrack_long_names
#include "HeapTrack_defs.h"
/*! @endcond */
