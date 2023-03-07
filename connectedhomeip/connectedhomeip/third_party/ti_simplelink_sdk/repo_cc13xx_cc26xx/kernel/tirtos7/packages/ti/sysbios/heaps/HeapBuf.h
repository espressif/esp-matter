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
 * @file ti/sysbios/heaps/HeapBuf.h
 * @brief Fixed size buffer heap manager
 *
 * The HeapBuf manager provides functions to allocate and free storage from a
 * heap of type HeapBuf which inherits from IHeap. HeapBuf manages a single
 * fixed-size buffer, split into equally sized allocable blocks.
 *
 * The HeapBuf manager is intended as a very fast and deterministic memory
 * manager which can only allocate blocks of a single size. It is ideal for
 * managing a heap that is only used for allocating a single type of object, or
 * for objects that have very similar sizes.
 *
 * Allocation from and freeing to a HeapBuf instance is non-blocking and
 * deterministic: a call to alloc or free will always take the same amount of
 * time.
 *
 * Constraints:
 *
 *  - Align parameter must be set to 0 for default, or be a power of 2 that is
 *    greater than or equal to the value returned by
 *    @link Memory_getMaxDefaultTypeAlign @endlink.
 *  - The buffer passed to dynamically create a HeapBuf must be aligned
 *    according to the alignment parameter, and must be large enough to account
 *    for the actual block size after it has been rounded up to a multiple of
 *    the alignment.
 *
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *  <colgroup span="1"></colgroup>
 *  <colgroup span="5" align="center"></colgroup>
 *
 *    <tr><th> Function    </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                       -->
 *    <tr><td> Params_init      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> alloc            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> construct        </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> create           </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> delete           </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> destruct         </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> free             </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getStats         </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getExtendedStats </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> isBlocking       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. HeapBuf_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. HeapBuf_Module_startupDone() returns false).</li>
 *           </ul>
 *       </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== HeapBuf.h ========
 */

#ifndef ti_sysbios_heaps_HeapBuf__include
#define ti_sysbios_heaps_HeapBuf__include

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

#define ti_sysbios_heaps_HeapBuf_long_names
#include "HeapBuf_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error and Assert Ids
 */

/*!
 * @brief Null buf parameter
 *
 * This Assert is raised when the buf parameter has been omitted (null) for a
 * dynamic create.
 */
#define HeapBuf_A_nullBuf "buf parameter cannot be null"

/*!
 * @brief Misalligned buffer
 *
 * This Assert is raised when the buf parameter has not been aligned on the
 * requested alignment.
 */
#define HeapBuf_A_bufAlign "buf not properly aligned"

/*!
 * @brief Invalid alignment
 *
 * This Assert is raised when the align parameter is not the default value of
 * zero, or a power of 2.
 */
#define HeapBuf_A_invalidAlign "align parameter must be 0 or a power of 2 >= the value of Memory_getMaxDefaultTypeAlign()"

/*!
 * @brief Invalid alignment parameter in the alloc
 *
 * This Assert is raised when the align argument is not a power of 2 (or 0). It
 * is also raised if the parameter is greater the alignment specified when
 * creating the heap.
 */
#define HeapBuf_A_invalidRequestedAlign "align parameter 1) must be 0 or a power of 2 and 2) not greater than the heaps alignment"

/*!
 * @brief Assert raised when the blockSize parameter is not large enough to hold
 * two pointers.
 */
#define HeapBuf_A_invalidBlockSize "blockSize must be large enough to hold at least two pointers"

/*!
 * @brief Assert raised when the numBlocks parameter is 0
 */
#define HeapBuf_A_zeroBlocks "numBlocks cannot be zero"

/*!
 * @brief Assert raised when the bufSize parameter is 0
 */
#define HeapBuf_A_zeroBufSize "bufSize cannot be zero"

/*!
 * @brief Invalid buffer size
 *
 * This Assert is raised when the bufSize parameter is too small to handle
 * requested blocks.
 */
#define HeapBuf_A_invalidBufSize "HeapBuf_create's bufSize parameter is invalid (too small)"

/*!
 * @brief No allocated blocks
 *
 * This Assert is raised when free is called and there are no blocks allocated.
 */
#define HeapBuf_A_noBlocksToFree "Cannot call HeapBuf_free when no blocks have been allocated"

/*!
 * @brief Assert raised when an invalid free occurs
 *
 * This assert can be caused for the following reasons
 *
 *  - Passing a NULL pointer to the free.
 *  - The block is not within the heap's buffer range. This can occur if
 *    the block is freed to the wrong heap.
 *  - The block is not on the proper boundary. This can occur if the
 *    pointer returned from alloc is changed by the application and not set
 *    back to its original value when free is called.
 *
 */
#define HeapBuf_A_invalidFree "Invalid free"

/*!
 * @brief Raised when requested size exceeds blockSize
 */
#define HeapBuf_E_size "requested size is too big: handle=0x%x, size=%u"

/*!
 * @brief Stat structure for the HeapBuf_getExtendedStats function
 */
typedef struct {
    /*!
     * @brief The maximum number of blocks allocated from this
     * heap at any single point in time, during the lifetime of this HeapBuf
     * instance. The configuration parameter HeapBuf.trackMaxAllocs
     * must be set to true for this field to have any meaning. Otherwise,
     * this field will be set to '0'.
     */
    unsigned int maxAllocatedBlocks;
    /*!
     * @brief The total number of blocks currently allocated in
     * this HeapBuf instance.
     */
    unsigned int numAllocatedBlocks;
} HeapBuf_ExtendedStats;

typedef struct {
    /*!
     * @brief Alignment (in MAUs) of each block
     *
     * The alignment must be a power of 2 that is greater than or equal to the
     * target dependent value defined by @link
     * Memory_getMaxDefaultTypeAlign @endlink. If the default value
     * of 0 is specified, this value will be changed to be the minimum structure
     * alignment requirement, as defined by @link
     * Memory_getMaxDefaultTypeAlign @endlink.
     *
     * For static creates, the HeapBuf module will allocate space for the buffer
     * and will align it on this requested alignment.
     *
     * For dynamic creates, this parameter is used solely for error checking.
     * The buffer provided to a dynamically created HeapBuf must be aligned
     * manually, and an assert will be raised if the buffer is not properly
     * aligned. For dynamic creates, HeapBuf will NOT adjust the buffer to
     * satisfy the alignment.
     */
    size_t align;
    /*!
     * @brief Number of fixed-size blocks
     *
     * Required parameter.
     *
     * The default number of blocks is 0.
     */
    unsigned int numBlocks;
    /*!
     * @brief Size (in MAUs) of each block
     *
     * HeapBuf will round the blockSize up to the nearest multiple of the
     * alignment, so the actual blockSize may be larger. When creating a HeapBuf
     * dynamically, this needs to be taken into account to determine the proper
     * buffer size to pass in.
     *
     * Required parameter.
     *
     * The default size of the blocks is 0 MAUs.
     */
    size_t blockSize;
    /*!
     * @brief Size (in MAUs) of the entire buffer; for dynamic creates only
     *
     * When dynamically creating a HeapBuf, this should be the actual size of
     * the memory buffer provided for HeapBuf to manage.
     *
     * This parameter is used solely for error checking; an Error is raised if
     * the bufSize is too small for the requested blockSize and numBlocks.
     * Because of minimum alignment considerations, it's easy to provide too
     * small of a buffer to manage, and difficult to detect if it happened.
     *
     * This parameter is required for dynamic creates and ignored for static
     * creates.
     *
     * The default buffer size is 0 MAUs.
     */
    size_t bufSize;
    /*!
     * @brief User supplied buffer; for dynamic creates only
     *
     * The caller must supply the memory buffer for a dynamically created
     * HeapBuf to manage. The provided buffer must be aligned according to the
     * HeapBuf's alignment parameter. i.e., if a buffer's alignment parameter is
     * 128, then the buffer's start address must be a multiple of 128.
     *
     * For static creates, the buffer is allocated automatically based on the
     * blockSize and numBlocks parameters.
     *
     * This parameter is required for dynamic creates and ignored for static
     * creates.
     */
    void * buf;
} HeapBuf_Params;

typedef struct {
    IHeap_Object iheap;
    Queue_Elem objElem;
    /*!
     * @brief Size (in MAUs) of each block
     *
     * HeapBuf will round the blockSize up to the nearest multiple of the
     * alignment, so the actual blockSize may be larger. When creating a HeapBuf
     * dynamically, this needs to be taken into account to determine the proper
     * buffer size to pass in.
     *
     * Required parameter.
     *
     * The default size of the blocks is 0 MAUs.
     */
    size_t blockSize;
    /*!
     * @brief Alignment (in MAUs) of each block
     *
     * The alignment must be a power of 2 that is greater than or equal to the
     * target dependent value defined by @link
     * Memory_getMaxDefaultTypeAlign @endlink. If the default value
     * of 0 is specified, this value will be changed to be the minimum structure
     * alignment requirement, as defined by @link
     * Memory_getMaxDefaultTypeAlign @endlink.
     *
     * For static creates, the HeapBuf module will allocate space for the buffer
     * and will align it on this requested alignment.
     *
     * For dynamic creates, this parameter is used solely for error checking.
     * The buffer provided to a dynamically created HeapBuf must be aligned
     * manually, and an assert will be raised if the buffer is not properly
     * aligned. For dynamic creates, HeapBuf will NOT adjust the buffer to
     * satisfy the alignment.
     */
    size_t align;
    /*!
     * @brief Number of fixed-size blocks
     *
     * Required parameter.
     *
     * The default number of blocks is 0.
     */
    unsigned int numBlocks;
    /*!
     * @brief Size (in MAUs) of the entire buffer; for dynamic creates only
     *
     * When dynamically creating a HeapBuf, this should be the actual size of
     * the memory buffer provided for HeapBuf to manage.
     *
     * This parameter is used solely for error checking; an Error is raised if
     * the bufSize is too small for the requested blockSize and numBlocks.
     * Because of minimum alignment considerations, it's easy to provide too
     * small of a buffer to manage, and difficult to detect if it happened.
     *
     * This parameter is required for dynamic creates and ignored for static
     * creates.
     *
     * The default buffer size is 0 MAUs.
     */
    size_t bufSize;
    /*!
     * @brief User supplied buffer; for dynamic creates only
     *
     * The caller must supply the memory buffer for a dynamically created
     * HeapBuf to manage. The provided buffer must be aligned according to the
     * HeapBuf's alignment parameter. i.e., if a buffer's alignment parameter is
     * 128, then the buffer's start address must be a multiple of 128.
     *
     * For static creates, the buffer is allocated automatically based on the
     * blockSize and numBlocks parameters.
     *
     * This parameter is required for dynamic creates and ignored for static
     * creates.
     */
    char *buf;
    unsigned int numFreeBlocks;
    unsigned int minFreeBlocks;
    Queue_Struct freeList;
} HeapBuf_Struct, *HeapBuf_Handle;

typedef HeapBuf_Struct HeapBuf_Object;

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} HeapBuf_Module_State;
/*! @endcond */

/*!
 * @brief cast handle to an IHeap_Handle for use by Memory_alloc, etc.
 *
 * @param handle heap handle
 *
 * @retval IHeap_Handle
 */
#define ti_sysbios_heaps_HeapBuf_Handle_upCast(handle) ((IHeap_Handle)(handle))

/*!
 * @brief All alignment is handled in the create, therefore the align
 * argument in alloc is ignored.
 *
 * HeapBuf_alloc will only fail if there are no available blocks or if the
 * requested block size is larger than the HeapBuf's blockSize. All other
 * requests, regardless of size, will succeed.
 *
 * @param heap heap handle
 * @param size size
 * @param align alignment
 * @param eb error block
 *
 * @retval address of allocated block (NULL on error)
 */
extern void * HeapBuf_alloc(HeapBuf_Handle heap, size_t size, size_t align, Error_Block *eb);

/*
 * @brief Free a block of allocated memory.
 *
 * @param heap heap handle
 * @param buf address of allocated block
 * @param size size
 */
extern void HeapBuf_free(HeapBuf_Handle heap, void * buf, size_t size);

/*!
 * @brief This function always returns false since the alloc/free never
 * block on a resource.
 *
 * @param heap heap handle
 *
 * @retval always returns false
 */
extern bool HeapBuf_isBlocking(HeapBuf_Handle heap);

/*!
 * @brief This function returns state information for a HeapBuf.
 *
 * @param heap heap handle
 * @param statBuf pointer to stats object
 */
extern void HeapBuf_getStats(HeapBuf_Handle heap, Memory_Stats *statBuf);

/*!
 * @brief Retrieves the extended statistics for a HeapBuf instance.
 *
 * This function retrieves the extended statistics for a HeapBuf instance. It
 * does not retrieve the standard xdc.runtime.Memory.Stats information.
 *
 * @param heap heap handle
 * @param statBuf pointer to the extended stats structure.
 */
extern void HeapBuf_getExtendedStats(HeapBuf_Handle heap, HeapBuf_ExtendedStats *statBuf);

/*!
 * @brief Create a `HeapBuf` heap
 *
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapBuf handle (NULL on failure)
 */
extern HeapBuf_Handle HeapBuf_create(const HeapBuf_Params *params, Error_Block *eb);


/*!
 * @brief Construct a `HeapBuf` heap
 *
 * HeapBuf_construct is equivalent to HeapBuf_create except that the 
 * HeapBuf_Struct is pre-allocated.
 *
 * @param obj pointer to a HeapBuf object
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapBuf handle (NULL on failure)
 */
extern HeapBuf_Handle HeapBuf_construct(HeapBuf_Struct *obj, const HeapBuf_Params *params, Error_Block *eb);

/*!
 * @brief Delete a `HeapBuf` heap
 *
 * Note that HeapBuf_delete takes a pointer to a HeapBuf_Handle which enables
 * HeapBuf_delete to set the HeapBuf handle to NULL.
 *
 * @param heap pointer to a HeapBuf handle
 */
extern void HeapBuf_delete(HeapBuf_Handle *heap);

/*!
 * @brief Destruct a `HeapBuf` heap
 *
 * @param obj pointer to a HeapBuf objects
 */
extern void HeapBuf_destruct(HeapBuf_Struct *obj);

/*!
 * @brief Initialize the HeapBuf_Params structure with default values.
 *
 * HeapBuf_Params_init initializes the HeapBuf_Params structure with default
 * values. HeapBuf_Params_init should always be called before setting individual
 * parameter fields. This allows new fields to be added in the future with
 * compatible defaults -- existing source code does not need to change when
 * new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void HeapBuf_Params_init(HeapBuf_Params *prms);

/*!
 * @brief return handle of the first HeapBuf on HeapBuf list
 *
 * Return the handle of the first HeapBuf on the create/construct list. NULL if
 * no HeapBufs have been created or constructed.
 *
 * @retval HeapBuf handle
 */
extern HeapBuf_Handle HeapBuf_Object_first(void);

/*!
 * @brief return handle of the next HeapBuf on HeapBuf list
 *
 * Return the handle of the next HeapBuf on the create/construct list. NULL if
 * no more HeapBufs are on the list.
 *
 * @param heap HeapBuf handle
 *
 * @retval HeapBuf handle
 */
extern HeapBuf_Handle HeapBuf_Object_next(HeapBuf_Handle heap);

/*! @cond NODOC */
extern size_t HeapBuf_getBlockSize(HeapBuf_Handle heap);

extern void HeapBuf_mergeHeapBufs(HeapBuf_Handle h1, HeapBuf_Handle h2);

static inline HeapBuf_Handle HeapBuf_handle(HeapBuf_Struct *str)
{
    return ((HeapBuf_Handle)str);
}

static inline HeapBuf_Struct * HeapBuf_struct(HeapBuf_Handle h)
{
    return ((HeapBuf_Struct *)h);
}

#define HeapBuf_module ((HeapBuf_Module_State *) &(HeapBuf_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif

/*! @cond NODOC */
#undef ti_sysbios_heaps_HeapBuf_long_names
#include "HeapBuf_defs.h"
/*! @endcond */
