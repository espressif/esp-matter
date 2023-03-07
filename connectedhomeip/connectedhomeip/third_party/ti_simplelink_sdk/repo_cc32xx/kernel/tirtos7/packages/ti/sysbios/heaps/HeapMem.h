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
 * @file ti/sysbios/heaps/HeapMem.h
 * @brief Variable size buffer heap manager
 *
 * HeapMem manager provides functions to allocate and free storage from a heap
 * of type HeapMem which inherits from IHeap.
 *
 * In addition to allowing multiple static HeapMem instances to be created who's
 * buffer placements and sizes are defined by their instance configuration
 * parameters, HeapMem allows one Heap instance to be defined who's heap memory
 * is defined by buffer start and end symbols in the linker command file. This
 * singular Heap instance is referred to as the 'Primary Heap'.
 *
 * @htmlonly
 *  <h3> HeapMem Gate </h3>
 *  A HeapMem instance will use the HeapMem module Gate to protect any accesses
 *  to its list of free memory blocks. The HeapMem instance will enter and
 *  leave the module Gate when allocating blocks, freeing blocks, and
 *  retrieving statistics about the HeapMem.
 *
 *  By default, the Gate is of type GateMutex.
 *
 *  A different Gate can be specified using the common$.gate parameter.
 *  For example, to use a GateTask to protect HeapMem:
 *      HeapMem.common$.gate = GateTask.create();
 *
 *  To save on overhead, HeapMem does not create additional Gate instances on a
 *  per-instance basis; there is only one Gate instance shared across all of
 *  the HeapMem instances. Therefore, the HeapMem.common$.gateParams parameter
 *  (used for configuring per-instance Gates) is ignored.
 *
 *  The type of Gate used should be chosen based on the type of threads (Hwi,
 *  Swi, Task, etc.) using the heap. It should also consider the
 *  non-deterministic nature of the HeapMem. A call to alloc or free will
 *  traverse a list of free blocks, so a GateHwi, for example, is likely an
 *  inappropriate Gate for HeapMem.
 *
 * <h3> Calling Context </h3> <table border="1" cellpadding="3">
 * <colgroup span="1"></colgroup>
 * <colgroup span="5" align="center"></colgroup>
 *
 * <tr><th> Function    </th><th>  Hwi   </th><th>  Swi   </th> <th>  Task
 * </th><th>  Main  </th><th>  Startup  </th></tr> <!--
 * --> <tr><td> Params_init         </td><td>   Y    </td><td>   Y    </td> <td>
 * Y    </td><td>   Y    </td><td>   Y    </td></tr> <tr><td> alloc
 * </td><td>   N**  </td><td>   N**  </td> <td>   Y**  </td><td>   Y
 * </td><td>   N    </td></tr> <tr><td> construct           </td><td>   Y
 * </td><td>   Y    </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> create              </td><td>   N*   </td><td>   N*
 * </td> <td>   Y    </td><td>   Y    </td><td>   N    </td></tr> <tr><td>
 * delete              </td><td>   N*   </td><td>   N*   </td> <td>   Y
 * </td><td>   Y    </td><td>   N    </td></tr> <tr><td> destruct
 * </td><td>   Y    </td><td>   Y    </td> <td>   Y    </td><td>   Y
 * </td><td>   N    </td></tr> <tr><td> free                </td><td>   N**
 * </td><td>   N**  </td> <td>   Y**  </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> getExtendedStats    </td><td>   Y    </td><td>   Y
 * </td> <td>   Y    </td><td>   Y    </td><td>   N    </td></tr> <tr><td>
 * getStats            </td><td>   N**  </td><td>   N**  </td> <td>   Y**
 * </td><td>   Y    </td><td>   N    </td></tr> <tr><td> isBlocking
 * </td><td>   Y    </td><td>   Y    </td> <td>   Y    </td><td>   Y
 * </td><td>   N    </td></tr> <tr><td> restore             </td><td>   Y+
 * </td><td>   Y+   </td> <td>   Y+   </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td colspan="6"> Definitions: <br /> <ul> <li> <b>Hwi</b>: API
 * is callable from a Hwi thread. </li> <li> <b>Swi</b>: API is callable from a
 * Swi thread. </li> <li> <b>Task</b>: API is callable from a Task thread. </li>
 * <li> <b>Main</b>: API is callable during any of these phases: </li> <ul> <li>
 * In your module startup after this module is started (e.g.
 * HeapMem_Module_startupDone() returns true). </li> <li> During
 * xdc.runtime.Startup.lastFxns. </li> <li> During main().</li> <li> During
 * BIOS.startupFxns.</li> </ul> <li> <b>Startup</b>: API is callable during any
 * of these phases:</li> <ul> <li> During xdc.runtime.Startup.firstFxns.</li>
 * <li> In your module startup before this module is started (e.g.
 * HeapMem_Module_startupDone() returns false).</li> </ul> <li> <b>*</b> :
 * Assuming blocking Heap is used for creation. </li> <li> <b>**</b>: Assuming
 * GateMutex is used as HeapMem's Gate. </li> <li> <b>+</b> : Cannot use HeapMem
 * object while it is being restored. </li> </ul> </td></tr>
 *
 * </table>
 *
 * @endhtmlonly
 */

/*
 * ======== HeapMem.h ========
 */

#ifndef ti_sysbios_heaps_HeapMem__include
#define ti_sysbios_heaps_HeapMem__include

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

#define ti_sysbios_heaps_HeapMem_long_names
#include "HeapMem_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

#define HeapMem_reqAlign sizeof(HeapMem_Header)

/*!
 * @brief Assert raised when the requested alignment is not a power of 2
 */
#define HeapMem_A_align       "Requested align is not a power of 2"

/*!
 * @brief Assert raised when the requested heap size is too small
 */
#define HeapMem_A_heapSize    "Requested heap size is too small"

/*!
 * @brief Assert raised when the free detects that an invalid addr or size
 *
 * This could arise when multiple frees are done on the same buffer or if
 * corruption occurred.
 *
 * This also could occur when an alloc is made with size N and the free for this
 * buffer specifies size M where M > N. Note: not every case is detectable.
 *
 * This assert can also be caused when passing an invalid addr to free or if the
 * size is causing the end of the buffer to be out of the expected range.
 */
#define HeapMem_A_invalidFree "Heap-block free operations must not leave the heap in an invalid state"

/*!
 * @brief Assert raised when a block of size 0 is requested
 *
 * This error can also be raised if the requested size wraps the contents of a
 * SizeT type when it is adjusted for a minimum alignment. For example, when
 * SizeT is 16-bits and a size request is made for 0xFFFB.
 */
#define HeapMem_A_zeroBlock   "Cannot allocate size 0"

/*!
 * @brief Raised when requested size exceeds largest free block
 */
#define HeapMem_E_memory      "out of memory: handle = 0x%x, size=%u"

/*!
 * @brief Stat structure for the HeapMem_getExtendedStats function
 */
typedef struct {
    /*!
     * @brief Buffer being managed by this heap instance
     *
     * This parameter is ignored in the static HeapMem.create() call. It is a
     * required parameter in the dynamic HeapMem_create() call.
     *
     * HeapMem requires that the buffer be aligned on a target-specific minimum
     * alignment, and will adjust the buffer's start address and size as
     * necessary to satisfy this requirement.
     */
    void * buf;
    /*!
     * @brief Size of buffer being managed by this heap instance
     *
     * The usable size may be smaller depending on alignment requirements.
     */
    size_t size;
} HeapMem_ExtendedStats;

/*! @cond NODOC */
typedef struct HeapMem_Header {
    struct HeapMem_Header *next;
    /*!
     * @brief Size of buffer being managed by this heap instance
     *
     * The usable size may be smaller depending on alignment requirements.
     */
    size_t size;
} HeapMem_Header;
/*! @endcond */

typedef struct {
    /*!
     * @brief Minimum alignment for each block allocated
     *
     * This parameter dictates the minimum alignment for each block that is
     * allocated. If an alignment request of greater than minBlockAlign is made
     * in the alloc, it will be honored. If an alignment request of less than
     * minBlockAlign is made, the request will be ignored and minBlockAlign will
     * be used.
     *
     * @code
     *  HeapMem_Params_init(&prms);
     *  prms.minBlockAlign = 32;
     *  handle = HeapMem_create(&prms, &eb);
     *  ...
     *  // buf will be aligned on a 32 MAU boundary
     *  buf = Memory_alloc(HeapMem_Handle_upCast(handle), SIZE, 8, &eb);
     *
     *  // buf will be aligned on a 64 MAU boundary
     *  buf = Memory_alloc(HeapMem_Handle_upCast(handle), SIZE, 64, &eb);
     * @endcode
     *
     * The default alignment is 0 (which means this parameter is ignored).
     */
    size_t minBlockAlign;
    /*!
     * @brief Buffer being managed by this heap instance
     *
     * This parameter is ignored in the static HeapMem.create() call. It is a
     * required parameter in the dynamic HeapMem_create() call.
     *
     * HeapMem requires that the buffer be aligned on a target-specific minimum
     * alignment, and will adjust the buffer's start address and size as
     * necessary to satisfy this requirement.
     */
    void * buf;
    /*!
     * @brief Size of buffer being managed by this heap instance
     *
     * The usable size may be smaller depending on alignment requirements.
     */
    size_t size;
} HeapMem_Params;

/*! @cond  NODOC */
typedef struct {
    IHeap_Object iheap;
    Queue_Elem objElem;
    /*!
     * @brief Alignment of the buffer being managed by this heap instance
     *
     * In the static HeapMem.create() call, the buffer allocated for the HeapMem
     * instance will have the alignment specified by this parameter.
     *
     * In the dynamic case, the client must supply the buffer, so it is the
     * client's responsibility to manage the buffer's alignment, and there is no
     * 'align' parameter.
     *
     * The specified alignment must be a power of 2.
     *
     * HeapMem requires that the buffer be aligned on a target-specific minimum
     * alignment, and will adjust (round up) the requested alignment as
     * necessary to satisfy this requirement.
     *
     * The default alignment is 0.
     */
    size_t align;
    /*!
     * @brief Buffer being managed by this heap instance
     *
     * This parameter is ignored in the static HeapMem.create() call. It is a
     * required parameter in the dynamic HeapMem_create() call.
     *
     * HeapMem requires that the buffer be aligned on a target-specific minimum
     * alignment, and will adjust the buffer's start address and size as
     * necessary to satisfy this requirement.
     */
    char *buf;
    HeapMem_Header head;
    /*!
     * @brief Minimum alignment for each block allocated
     *
     * This parameter dictates the minimum alignment for each block that is
     * allocated. If an alignment request of greater than minBlockAlign is made
     * in the alloc, it will be honored. If an alignment request of less than
     * minBlockAlign is made, the request will be ignored and minBlockAlign will
     * be used.
     *
     * @code
     *  HeapMem_Params_init(&prms);
     *  prms.minBlockAlign = 32;
     *  handle = HeapMem_create(&prms, &eb);
     *  ...
     *  // buf will be aligned on a 32 MAU boundary
     *  buf = Memory_alloc(HeapMem_Handle_upCast(handle), SIZE, 8, &eb);
     *
     *  // buf will be aligned on a 64 MAU boundary
     *  buf = Memory_alloc(HeapMem_Handle_upCast(handle), SIZE, 64, &eb);
     * @endcode
     *
     * The default alignment is 0 (which means this parameter is ignored).
     */
    size_t minBlockAlign;
} HeapMem_Struct, *HeapMem_Handle;
/*! @endcond */

typedef HeapMem_Struct HeapMem_Object;

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} HeapMem_Module_State;
/*! @endcond */

/*!
 * @brief cast handle to an IHeap_Handle for use by Memory_alloc, etc.
 *
 * @param handle heap handle
 *
 * @retval IHeap_Handle
 */
#define ti_sysbios_heaps_HeapMem_Handle_upCast(handle) ((IHeap_Handle)(handle))

/*!
 * @brief The actual block returned may be larger than requested to
 * satisfy alignment requirements, and its size will always be a multiple of the
 * size of the HeapMem_Header data structure (usually 8 bytes)
 *
 * HeapMem_alloc() will lock the heap using the HeapMem Gate while it traverses
 * the list of free blocks to find a large enough block for the request.
 *
 * HeapMem_alloc() should not be called directly.  Application code should use
 * Memory_alloc() with a HeapMem_Handle as the first parameter.  Among other
 * things, Memory_alloc() makes sure that the alignment parameter is greater
 * than or equal to the minimum alignment required for the HeapMem_Header data
 * structure for a given C compiler (8 bytes in most cases). HeapMem_alloc() may
 * crash if you pass a smaller alignment.
 *
 * Guidelines for using large heaps and multiple alloc() calls.
 *
 *          - If possible, allocate larger blocks first. Previous
 *            allocations of small memory blocks can reduce the size
 *            of the blocks available for larger memory allocations.
 *          - Realize that alloc() can fail even if the heap contains a
 *            sufficient absolute amount of unallocated space. This is
 *            because the largest free memory block may be smaller than
 *            total amount of unallocated memory.
 *
 *
 * @param heap heap handle
 * @param size Requested size
 * @param align Requested alignment
 * @param eb Error_Block used to denote location in case of a failure
 *
 * @retval allocated block or NULL is request cannot be honored
 */
extern void * HeapMem_alloc(HeapMem_Handle heap, size_t size, size_t align, Error_Block *eb);

/*!
 * @brief HeapMem_free() places the memory block specified by addr and size
 * back into the free pool of the heap specified. The newly freed block is
 * combined with any adjacent free blocks. The space is then available for
 * further allocation by alloc().
 *
 * free() will lock the heap using the HeapMem Gate, if one is specified using
 * 'HeapMem.common$.gate'.
 *
 * @param heap heap handle
 * @param buf block of memory to be freed
 * @param size size of block to free
 */
extern void HeapMem_free(HeapMem_Handle heap, void * buf, size_t size);

/*!
 * @brief This function queries the gate
 * to determine if the alloc/free can be blocking.
 *
 * @param heap heap handle
 *
 * @retval returns true if the gate can block, false otherwise
 */
extern bool HeapMem_isBlocking(HeapMem_Handle heap);

/*!
 * @brief HeapMem_getStats will lock the heap using the HeapMem Gate while it
 * retrieves the HeapMem's statistics.
 *
 * The returned totalSize reflects the usable size of the buffer, not
 * necessarily the size specified during create.
 *
 * @param heap heap handle
 * @param statBuf pointer to stat buffer
 */
extern void HeapMem_getStats(HeapMem_Handle heap, Memory_Stats *statBuf);

/*!
 * @brief Retrieves the extended statistics for a HeapMem instance
 *
 * This function retrieves the extended statistics for a HeapMem instance. It
 * does not retrieve the standard xdc.runtime.Memory.Stats information.
 *
 * @param heap heap handle
 * @param statBuf Location for the returned extended statistics.
 */
extern void HeapMem_getExtendedStats(HeapMem_Handle heap, HeapMem_ExtendedStats *statBuf);

/*!
 * @brief Create a `HeapMem` heap
 *
 * This heap is a growth-only heap that is intended to be used by systems that
 * never delete objects or free memory.  Objects can be created at runtime based
 * on values determined at runtime, but objects can not be deleted.
 *
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapMem handle (NULL on failure)
 */
extern HeapMem_Handle HeapMem_create(const HeapMem_Params *params, Error_Block *eb);

/*!
 * @brief Construct a `HeapMem` heap
 *
 * HeapMem_construct is equivalent to HeapMem_create except that the 
 * HeapMem_Struct is pre-allocated.
 *
 * @param obj pointer to a HeapMem object
 * @param params optional parameters
 *
 * @retval HeapMem handle (NULL on failure)
 */
extern HeapMem_Handle HeapMem_construct(HeapMem_Struct *obj, const HeapMem_Params *params);

/*!
 * @brief Delete a `HeapMem` heap
 *
 * Note that HeapMem_delete takes a pointer to a HeapMem_Handle which enables
 * HeapMem_delete to set the HeapMem handle to NULL.
 *
 * @param heap pointer to a HeapMem handle
 */
extern void HeapMem_delete(HeapMem_Handle *heap);

/*!
 * @brief Destruct a `HeapMem` heap
 *
 * @param obj pointer to a HeapMem objects
 */
extern void HeapMem_destruct(HeapMem_Struct *obj);

/*!
 * @brief Restore heap to its original created state
 *
 * This function restores a heap to its original created state.
 * Any memory previously allocated from the heap is no longer valid
 * after this API is called. This function does not check whether
 * there is allocated memory or not.
 *
 * @param heap heap handle
 */
extern void HeapMem_restore(HeapMem_Handle heap);

/*!
 * @brief Initialize instance at runtime
 *
 * This function is plugged as a Startup.firstFxn so that the HeapMem objects
 * are ready and usable by malloc() and Memory_alloc() by the time the module
 * startup functions get called so that any calls to atexit(), which in some
 * targets invokes malloc(), will be handled cleanly.
 */
extern void HeapMem_init(void);

/*!
 * @brief Initialize the HeapMem_Params structure with default values.
 *
 * HeapMem_Params_init initializes the HeapMem_Params structure with default
 * values. HeapMem_Params_init should always be called before setting individual
 * parameter fields. This allows new fields to be added in the future with
 * compatible defaults -- existing source code does not need to change when
 * new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void HeapMem_Params_init(HeapMem_Params *prms);

/*!
 * @brief return handle of the first HeapMem on HeapMem list
 *
 * Return the handle of the first HeapMem on the create/construct list. NULL if
 * no HeapMems have been created or constructed.
 *
 * @retval HeapMem handle
 */
extern HeapMem_Handle HeapMem_Object_first(void);

/*!
 * @brief return handle of the next HeapMem on HeapMem list
 *
 * Return the handle of the next HeapMem on the create/construct list. NULL if
 * no more HeapMems are on the list.
 *
 * @param heap HeapMem handle
 *
 * @retval HeapMem handle
 */
extern HeapMem_Handle HeapMem_Object_next(HeapMem_Handle heap);

/*! @cond NODOC */
static inline HeapMem_Handle HeapMem_handle(HeapMem_Struct *str)
{
    return ((HeapMem_Handle)str);
}

static inline HeapMem_Struct * HeapMem_struct(HeapMem_Handle h)
{
    return ((HeapMem_Struct *)h);
}

#define HeapMem_module ((HeapMem_Module_State *) &(HeapMem_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif

/*! @cond NODOC */
#undef ti_sysbios_heaps_HeapMem_long_names
#include "HeapMem_defs.h"
/*! @endcond */
