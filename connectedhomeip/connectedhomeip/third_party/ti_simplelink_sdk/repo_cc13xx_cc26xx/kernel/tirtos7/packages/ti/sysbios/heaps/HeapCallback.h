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
 * @file ti/sysbios/heaps/HeapCallback.h
 * @brief A heap that calls user supplied callback functions
 *
 * The HeapCallback module enables users to provide a custom heap implementation
 * by providing callback functions that will be invoked by HeapCallback for the
 * various heap management functions.
 *
 * The user-supplied HeapCallback.initInstFxn is called during boot time to
 * initialize any HeapCallback objects that were created in the .cfg file. The
 * user-supplied HeapCallback.createInstFxn is called during runtime for any
 * calls to HeapCallback_create().  Both of these functions return a context
 * value (typically a pointer to an object managed by the user-supplied heap
 * code).  This context value is passed to subsequent user allocInstFxn,
 * freeInstFxn, etc. functions.
 *
 * HeapCallback_alloc(), HeapCallback_free() and HeapCallback_getStats() call
 * the user-supplied allocInstFxn, freeInstFxn and getStatsInstFxn functions
 * with the context value returned by initInstFxn or createInstFxn.
 *
 * HeapCallback_delete() calls the user-supplied instDeleteFxn with the context
 * returned by the createInstFxn.
 *
 * <h3>Examples</h3>
 * Configuration example: The following XDC configuration statements creates a
 * HeapCallback instance and plugs in the user defined functions.
 *
 * @code
 *  const HeapCallback = scripting.addModule("/ti/sysbios/heaps/HeapCallback");
 *
 *  HeapCallback.initInstFxn = "userInitFxn";
 *  HeapCallback.createInstFxn = "userCreateFxn";
 *  HeapCallback.deleteInstFxn = "userDeleteFxn";
 *  HeapCallback.allocInstFxn = "userAllocFxn";
 *  HeapCallback.freeInstFxn = "userFreeFxn";
 *  HeapCallback.getStatsInstFxn = "userGetStatsFxn";
 *  HeapCallback.isBlockingInstFxn = "userIsBlockingFxn";
 *
 * @endcode
 */

/*
 * ======== HeapCallback.h ========
 */

#ifndef ti_sysbios_heaps_HeapCallback__include
#define ti_sysbios_heaps_HeapCallback__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_heaps_HeapCallback_long_names
#include "HeapCallback_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief cast handle to an IHeap_Handle for use by Memory_alloc, etc.
 *
 * @param handle heap handle
 *
 * @retval IHeap_Handle
 */
#define ti_sysbios_heaps_HeapCallback_Handle_upCast(handle) ((IHeap_Handle)(handle))

/*!
 * @brief Instance alloc callback function signature
 *
 * This function takes the context return from createInstFxn(), the size to be
 * allocated and the align value.  The return value from this function is a
 * pointer to the allocated memory block.
 */
typedef void * (*HeapCallback_AllocInstFxn)(uintptr_t context, size_t size, size_t align);

/*!
 * @brief Instance create callback function signature
 *
 * The 'arg' is passed as an argument to this function.
 * The return value from this function (context) will be passed as an argument
 * to the other instance functions.
 */
typedef uintptr_t (*HeapCallback_CreateInstFxn)(uintptr_t arg);

/*!
 * @brief Instance delete callback function signature
 *
 * The context returned from createInstFxn() is passed as an argument to this
 * function.
 */
typedef void (*HeapCallback_DeleteInstFxn)(uintptr_t context);

/*!
 * @brief Instance free callback function signature
 *
 * This function takes the context returned by createInstFxn() and a pointer to
 * the buffer to be freed and the size to be freed.
 */
typedef void (*HeapCallback_FreeInstFxn)(uintptr_t context, void * addr, size_t size);

/*!
 * @brief Instance getStats callback function signature
 *
 * This function takes the context returned by createInstFxn() and a pointer to
 * a memory stats object.
 */
typedef void (*HeapCallback_GetStatsInstFxn)(uintptr_t context, Memory_Stats *stats);

/*!
 * @brief Instance isblocking callback function signature
 *
 * The context return from createInstFxn() is passed as an argument to this
 * function. The return value is 'true' or 'false'.
 */
typedef bool (*HeapCallback_IsBlockingInstFxn)(uintptr_t context);

typedef struct {
    /*!
     * @brief User supplied argument for the user supplied create function.
     *
     * This user supplied argument will be passed back as an argument to the
     * `createInstFxn` function. It can be used by the
     * createInstFxn function at runtime to differentiate
     * between the multiple Heap instances configured in the user config script.
     *
     * The user can skip configuring this argument. In such a case, the default
     * value `0` will be passed back as an argument to the `createInstFxn`
     * function.
     */
    uintptr_t arg;
} HeapCallback_Params;

typedef struct {
    IHeap_Object iheap;
    Queue_Elem objElem;
    uintptr_t context;
} HeapCallback_Struct, *HeapCallback_Handle;

typedef HeapCallback_Struct HeapCallback_Object;

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} HeapCallback_Module_State;
/*! @endcond */

/*!
 * @brief Allocate a block of memory from the heap.
 */
extern void * HeapCallback_alloc(HeapCallback_Handle handle, size_t size, size_t align, Error_Block *eb);

/*!
 * @brief Free a block of memory back to the heap.
 */
extern void HeapCallback_free(HeapCallback_Handle handle, void * buf, size_t size);

/*!
 * @brief Can this heap block the caller
 *
 * `HeapCallback` returns `true` if the configured callback heap can block.
 * `HeapCallback` returns `false` if the configured callback heap cannot block.
 *
 * @param handle heap handle
 *
 * @retval always returns `false`
 */
extern bool HeapCallback_isBlocking(HeapCallback_Handle handle);

/*!
 * @brief get memory stats for a HeapCallback object
 *
 * @param handle heap handle
 * @param stats pointer to a @link Memory_Stats @endlink object
 */
extern void HeapCallback_getStats(HeapCallback_Handle handle, Memory_Stats *stats);

/*!
 * @brief Create a `HeapCallback` heap
 *
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapMin handle (NULL on failure)
 */
extern HeapCallback_Handle HeapCallback_create(const HeapCallback_Params *params, Error_Block *eb);

/*!
 * @brief Construct a `HeapCallback` heap
 *
 * HeapCallback_construct is equivalent to HeapCallback_create except that the 
 * HeapCallback_Struct is pre-allocated.
 *
 * @param obj pointer to a HeapCallback object
 * @param params optional parameters
 *
 * @retval HeapCallback handle (NULL on failure)
 */
extern HeapCallback_Handle HeapCallback_construct(HeapCallback_Struct *obj, const HeapCallback_Params *params);

/*!
 * @brief Delete a `HeapCallback` heap
 *
 * Note that HeapCallback_delete takes a pointer to a HeapCallback_Handle which enables
 * HeapCallback_delete to set the HeapCallback handle to NULL.
 *
 * @param handle pointer to a HeapCallback handle
 */
extern void HeapCallback_delete(HeapCallback_Handle *handle);

/*!
 * @brief Destruct a `HeapCallback` heap
 *
 * @param obj pointer to a HeapCallback objects
 */
extern void HeapCallback_destruct(HeapCallback_Struct *obj);

/*!
 * @brief Initialize the HeapCallback_Params structure with default values.
 *
 * HeapCallback_Params_init initializes the HeapCallback_Params structure with
 * default values. HeapCallback_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be
 * added in the future with compatible defaults -- existing source code
 * does not need to change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void HeapCallback_Params_init(HeapCallback_Params *prms);

/*!
 * @brief return handle of the first HeapCallback on HeapCallback list
 *
 * Return the handle of the first HeapCallback on the create/construct list.
 * NULL if no HeapCallbacks have been created or constructed.
 *
 * @retval HeapCallback handle
 */
extern HeapCallback_Handle HeapCallback_Object_first(void);

/*!
 * @brief return handle of the next HeapCallback on HeapCallback list
 *
 * Return the handle of the next HeapCallback on the create/construct list.
 * NULL if no more HeapCallbacks are on the list.
 *
 * @param heap HeapCallback handle
 *
 * @retval HeapCallback handle
 */
extern HeapCallback_Handle HeapCallback_Object_next(HeapCallback_Handle heap);

static inline HeapCallback_Handle HeapCallback_handle(HeapCallback_Struct *str)
{
    return ((HeapCallback_Handle)str);
}

static inline HeapCallback_Struct * HeapCallback_struct(HeapCallback_Handle h)
{
    return ((HeapCallback_Struct *)h);
}

/*! @cond NODOC */
extern void HeapCallback_init(void);
#define HeapCallback_module ((HeapCallback_Module_State *) &(HeapCallback_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif

/*! @cond NODOC */
#undef ti_sysbios_heaps_HeapCallback_long_names
#include "HeapCallback_defs.h"
/*! @endcond */
