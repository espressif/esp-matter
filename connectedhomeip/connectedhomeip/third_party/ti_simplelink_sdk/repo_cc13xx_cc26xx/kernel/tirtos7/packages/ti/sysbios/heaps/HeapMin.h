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
 * @file ti/sysbios/heaps/HeapMin.h
 * @brief Growth-only based heap implementation.
 *
 * `HeapMin` is a minimal footprint heap implementation. This module is is
 * designed for applications that only create module instances and generally
 * only allocate memory at runtime, but never delete created instances or free
 * memory explicitly.
 *
 * When calling @link HeapMin_create() @endlink at runtime, the client is
 * responsible for aligning the buffer.
 */

/*
 * ======== HeapMin.h ========
 */

#ifndef ti_sysbios_heaps_HeapMin__include
#define ti_sysbios_heaps_HeapMin__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_heaps_HeapMin_long_names
#include "HeapMin_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Assert that the heap size is non-zero on the create
 */
#define HeapMin_A_zeroSize "heap size must be > 0"

/*!
 * @brief Error raised if HeapMin_free() is called.
 *
 * This error is only raised if a HeapMin_free() is called and
 * the HeapMin.freeError configuration parameter is set to true.
 */
#define HeapMin_E_freeError "free() invalid in growth-only HeapMin"

typedef struct {
    IHeap_Object iheap;
    Queue_Elem objElem;
    size_t remainSize;
    size_t startSize;
    /*!
     * @brief Buffer that will be managed by the heap instance.
     *
     * When creating a heap at runtime, the user must supply the memory that the
     * heap will manage.  It is up to the caller to align the buffer as needed.
     *
     * This parameter is ignored when creating heaps during configuration.
     */
    char *buf;
} HeapMin_Struct, HeapMin_Object, *HeapMin_Handle;

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} HeapMin_Module_State;
/*! @endcond */

typedef struct {
    /*!
     * @brief Buffer that will be managed by the heap instance.
     *
     * When creating a heap at runtime, the user must supply the memory that the
     * heap will manage.  It is up to the caller to align the buffer as needed.
     *
     * This parameter is ignored when creating heaps during configuration.
     */
    char *buf;
    /*!
     * @brief Size (in MADUs) of the heap.
     *
     * This parameter specifies the size of the heap managed by a `HeapMin`
     * instance.  In the static case, a buffer of length `size` will be created.
     * In the dynamic case, `size` specifies the size of the buffer (i.e.
     * parameter `buf`) that the caller provides.
     *
     * This is a required parameter. It must be set by the caller. Failure to do
     * so will result in an Assert (if BIOS.assertsEnabled config parameter is
     * true).
     */
    size_t size;
} HeapMin_Params;


/*!
 * @brief cast handle to an IHeap_Handle for use by Memory_alloc, etc.
 *
 * @param handle heap handle
 *
 * @retval IHeap_Handle
 */
#define HeapMin_Handle_upCast(handle) ((IHeap_Handle)(handle))

/*!
 * @brief Allocate a block of memory from the heap.
 *
 * @pre
 * The alignment must be a power of 2.
 */
extern void * HeapMin_alloc(void *handle, size_t size, size_t align, Error_Block *eb);

/*!
 * @brief Free a block of memory back to the heap.
 *
 * This is a growth only heap. Calling the `HeapMin_free` function will result
 * in a HeapMin_E_freeError error unless the HeapMin.freeError
 * configuration parameter is set to `false`.
 */
extern void HeapMin_free(void *handle, void * buf, size_t size);

/*!
 * @brief Can this heap block the caller
 *
 * `HeapMin` always returns `false` since it never blocks on a resource.
 *
 * @param handle heap handle
 *
 * @retval always returns `false`
 */
extern bool HeapMin_isBlocking(void *handle);

/*!
 * @brief get memory stats for a HeapMin object
 *
 * @param handle heap handle
 * @param statBuf pointer to a @link Memory_Stats @endlink object
 */
extern void HeapMin_getStats(void *handle, Memory_Stats *statBuf);

/*!
 * @brief Create a `HeapMin` heap
 *
 * This heap is a growth-only heap that is intended to be used by systems that
 * never delete objects or free memory.  Objects can be created at runtime based
 * on values determined at runtime, but objects can not be deleted.
 *
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapMin handle (NULL on failure)
 */
extern HeapMin_Handle HeapMin_create(const HeapMin_Params *params, Error_Block *eb);

/*!
 * @brief Construct a `HeapMin` heap
 *
 * HeapMin_construct is equivalent to HeapMin_create except that the 
 * HeapMin_Struct is pre-allocated.
 *
 * @param obj pointer to a HeapMin object
 * @param params optional parameters
 *
 * @retval HeapMin handle (NULL on failure)
 */
extern HeapMin_Handle HeapMin_construct(HeapMin_Struct *obj, const HeapMin_Params *params);

/*!
 * @brief Delete a `HeapMin` heap
 *
 * Note that HeapMin_delete takes a pointer to a HeapMin_Handle which enables
 * HeapMin_delete to set the HeapMin handle to NULL.
 *
 * @param handle pointer to a HeapMin handle
 */
extern void HeapMin_delete(HeapMin_Handle *handle);

/*!
 * @brief Destruct a `HeapMin` heap
 *
 * @param obj pointer to a HeapMin objects
 */
extern void HeapMin_destruct(HeapMin_Struct *obj);

/*!
 * @brief Initialize the HeapMin_Params structure with default values.
 *
 * HeapMin_Params_init initializes the HeapMin_Params structure with
 * default values. HeapMin_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be
 * added in the future with compatible defaults -- existing source code
 * does not need to change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void HeapMin_Params_init(HeapMin_Params *prms);

/*!
 * @brief return handle of the first HeapMin on HeapMin list
 *
 * Return the handle of the first HeapMin on the create/construct list. NULL if
 * no HeapMins have been created or constructed.
 *
 * @retval HeapMin handle
 */
extern HeapMin_Handle HeapMin_Object_first(void);

/*!
 * @brief return handle of the next HeapMin on HeapMin list
 *
 * Return the handle of the next HeapMin on the create/construct list. NULL if
 * no more HeapMins are on the list.
 *
 * @param heap HeapMin handle
 *
 * @retval HeapMin handle
 */
extern HeapMin_Handle HeapMin_Object_next(HeapMin_Handle heap);

/*! @cond NODOC */
extern void HeapMin_init(void);

static inline HeapMin_Handle HeapMin_handle(HeapMin_Struct *str)
{
    return ((HeapMin_Handle)str);
}

static inline HeapMin_Struct * HeapMin_struct(HeapMin_Handle h)
{
    return ((HeapMin_Struct *)h);
}

#define HeapMin_module ((HeapMin_Module_State *) &(HeapMin_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_heaps_HeapMin__include */

/*! @cond NODOC */
#undef ti_sysbios_heaps_HeapMin_long_names
#include "HeapMin_defs.h"
/*! @endcond */
