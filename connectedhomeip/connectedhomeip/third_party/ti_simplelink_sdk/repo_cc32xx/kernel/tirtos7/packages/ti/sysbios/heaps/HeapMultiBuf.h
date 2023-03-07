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
 * @file ti/sysbios/heaps/HeapMultiBuf.h
 * @brief Multiple fixed size buffer heap manager
 *
 * The HeapMultiBuf manager provides functions to allocate and free storage from
 * a heap of type HeapMultiBuf which inherits from IHeap. HeapMultiBuf manages
 * multiple fixed-size memory buffers. Each buffer contains a fixed number of
 * allocable memory 'blocks' of the same size. Simply put, a HeapMultiBuf
 * instance manages a collection of HeapBuf instances. HeapMultiBuf is intended
 * as a fast and deterministic memory manager which can service requests for
 * blocks of arbitrary size.
 *
 * An example HeapMultiBuf instance might have sixteen 32-byte blocks in one
 * buffer, and four 128-byte blocks in another buffer. A request for memory will
 * be serviced by the smallest possible block, so a request for 100 bytes would
 * receive a 128-byte block in our example.
 *
 * Allocating from HeapMultiBuf will try to return a block from the first buffer
 * which has:
 *
 * 1. A block size that is >= to the requested size
 *
 * AND
 *
 * 2. An alignment that is >= to the requested alignment
 *
 * If the first matching buffer is empty, HeapMultiBuf will only continue
 * searching for a block if 'block borrowing' is enabled (see Block Borrowing).
 *
 * @htmlonly
 *  <h4>HeapMultiBuf and HeapBuf</h4>
 *  The HeapMultiBuf module is built on top of the HeapBuf module. Each buffer
 *  in a HeapMultiBuf is in fact managed by a HeapBuf instance. Configuration
 *  of a HeapMultiBuf is done by providing an array of configured HeapBuf
 *  parameter structures. Refer to the HeapBuf documentation for information on
 *  the buffer parameters. All of the documentation and parameters for HeapBuf
 *  apply equally to HeapMultiBuf.
 *  Another consequence of this is that configuration checking is left to the
 *  HeapBuf module. If a buffer in a HeapMultiBuf has been incorrectly
 *  configured (with blockSize = 0, for example), HeapBuf, not HeapMultiBuf,
 *  will raise an Assert.
 *  Since HeapMultiBuf is built on HeapBuf, it simply performs the logic to
 *  determine which HeapBuf to allocate a block from or which HeapBuf to free
 *  a block to.
 *
 * @endhtmlonly
 *
 * <h4>Configuration Example</h4> The following configuration code creates a
 * HeapMultiBuf instance which manages 3 pools of 10 blocks each, with block
 * sizes of 64, 128 and 256.
 *
 * @code
 *  var HeapMultiBuf = xdc.useModule('ti.sysbios.heaps.HeapMultiBuf');
 *  var HeapBuf = xdc.useModule('ti.sysbios.heaps.HeapBuf');
 *
 *  // Create parameter structure for HeapMultiBuf instance.
 *  var hmbParams = new HeapMultiBuf.Params();
 *  hmbParams.numBufs = 3;
 *
 *  // Create the parameter structures for each of the three
 *  // HeapBufs to be managed by the HeapMultiBuf instance.
 *  hmbParams.bufParams.$add(new HeapBuf.Params());
 *  hmbParams.bufParams[0].blockSize = 64;
 *  hmbParams.bufParams[0].numBlocks = 10;
 *
 *  hmbParams.bufParams.$add(new HeapBuf.Params());
 *  hmbParams.bufParams[1].blockSize = 128;
 *  hmbParams.bufParams[1].numBlocks = 10;
 *
 *  hmbParams.bufParams.$add(new HeapBuf.Params());
 *  hmbParams.bufParams[2].blockSize = 256;
 *  hmbParams.bufParams[2].numBlocks = 10;
 *
 *
 *  // Create the HeapMultiBuf instance, and assign the global handle
 *  // 'multiBufHeap' to it. Add '#include <xdc/cfg/global.h>' to your
 *  // .c file to reference the instance by this handle.
 *  Program.global.multiBufHeap = HeapMultiBuf.create(hmbParams);
 * @endcode
 *
 * @htmlonly
 *  <h4>Block Borrowing</h4>
 *  HeapMultiBuf can support "block borrowing". With this feature turned on,
 *  if a request is made for a 32-byte block and none are available,
 *  HeapMultiBuf will continue looking for an available block in other buffers.
 *  When a borrowed block is freed, it will be returned back to its original
 *  buffer. Enabling Block Borrowing changes the determinism of alloc, since it
 *  may have to check any number of buffers to find an available block.
 *
 *  Block borrowing may also occur, even if it is disabled, if a block of a
 *  particular size is requested with an alignment that is greater than the
 *  configured alignment for that block size. For example, a HeapMultiBuf is
 *  configured with a buffer of 32-byte blocks with an alignment of 8, and
 *  a buffer of 64-byte blocks with an alignment of 16. If a request is made
 *  for a 32-byte block with an alignment of 16, it will be serviced by the
 *  buffer of 64-byte blocks.
 *
 *  <h4>Static vs. Dynamic Creation</h4>
 *  As with HeapBuf, a statically created HeapMultiBuf instance will ignore the
 *  bufSize and buf parameters. Dynamic creates require all of the parameters.
 *
 *  It should be noted that static creates are ideal if code space is a
 *  concern; dynamically creating a HeapMultiBuf requires a relatively large
 *  amount of initialization code to be pulled in to the executable.
 *
 *  <h4>Block Sizes and Alignment</h4>
 * @endhtmlonly
 *
 * - A buffer with a requested alignment of 0 will receive the target- specific
 * minimum alignment. - The actual block sizes will be a multiple of the
 * alignment. For example, if a buffer is configured to have 56-byte blocks with
 * an alignment of 32, HeapMultiBuf will actually create 64-byte blocks. When
 * providing the buffer for a dynamic create, make sure it is large enough to
 * take this into account. - Multiple buffers with the same block size ARE
 * allowed. This may occur, for example, if sizeof is used to specify the block
 * sizes. - If any buffers have both the same block size and alignment, they
 * will be merged. If this is a problem, consider managing these buffers
 * directly with HeapBuf objects.
 *
 * @htmlonly
 *  <h4>Real-Time Concerns</h4>
 *  Allocation from and freeing to a HeapMultiBuf instance is non-blocking.
 *
 *  HeapMultiBuf is deterministic:
 * @endhtmlonly
 *
 * - A call to alloc will always take the same amount of time for a given block
 * size (with block borrowing disabled). - The worst case call to free is
 * constant and proportional to the number of buffers managed.
 *
 * @htmlonly
 *  <h4>Restrictions</h4>
 * @endhtmlonly
 *
 * - Align parameters must be a power of 2. - The buffers passed to dynamically
 * create a HeapMultiBuf must be aligned according to the alignment parameter,
 * and must be large enough to account for the actual block size after it has
 * been rounded up to a multiple of the alignment. - Buffers must be provided to
 * dynamically create a HeapMultiBuf, and cannot be provided to statically
 * create a HeapMultiBuf.
 *
 * @htmlonly
 *  <h4>Unconstrained Functions</h4>
 *  All functions
 *
 * <h3> Calling Context </h3> <table border="1" cellpadding="3">
 * <colgroup span="1"></colgroup>
 * <colgroup span="5" align="center"></colgroup>
 *
 * <tr><th> Function    </th><th>  Hwi   </th><th>  Swi   </th> <th>  Task
 * </th><th>  Main  </th><th>  Startup  </th></tr> <!--
 * --> <tr><td> Params_init      </td><td>   Y    </td><td>   Y    </td> <td>
 * Y    </td><td>   Y    </td><td>   Y    </td></tr> <tr><td> alloc
 * </td><td>   Y    </td><td>   Y    </td> <td>   Y    </td><td>   Y
 * </td><td>   N    </td></tr> <tr><td> construct        </td><td>   N*
 * </td><td>   N*   </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> create           </td><td>   N*   </td><td>   N*   </td>
 * <td>   Y    </td><td>   Y    </td><td>   N    </td></tr> <tr><td> delete
 * </td><td>   N*   </td><td>   N*   </td> <td>   Y    </td><td>   Y
 * </td><td>   N    </td></tr> <tr><td> destruct         </td><td>   N*
 * </td><td>   N*   </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> free             </td><td>   Y    </td><td>   Y    </td>
 * <td>   Y    </td><td>   Y    </td><td>   N    </td></tr> <tr><td> getStats
 * </td><td>   Y    </td><td>   Y    </td> <td>   Y    </td><td>   Y
 * </td><td>   N    </td></tr> <tr><td> isBlocking       </td><td>   Y
 * </td><td>   Y    </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td colspan="6"> Definitions: <br /> <ul> <li> <b>Hwi</b>: API
 * is callable from a Hwi thread. </li> <li> <b>Swi</b>: API is callable from a
 * Swi thread. </li> <li> <b>Task</b>: API is callable from a Task thread. </li>
 * <li> <b>Main</b>: API is callable during any of these phases: </li> <ul> <li>
 * In your module startup after this module is started (e.g.
 * HeapMultiBuf_Module_startupDone() returns true). </li> <li> During
 * xdc.runtime.Startup.lastFxns. </li> <li> During main().</li> <li> During
 * BIOS.startupFxns.</li> </ul> <li> <b>Startup</b>: API is callable during any
 * of these phases:</li> <ul> <li> During xdc.runtime.Startup.firstFxns.</li>
 * <li> In your module startup before this module is started (e.g.
 * HeapMultiBuf_Module_startupDone() returns false).</li> </ul> <li> <b>*</b>:
 * Assuming blocking Heap is used for creation. </li> <li> <b>**</b>: Assuming
 * GateMutex is used as HeapMem's Gate. </li> <li> <b>+</b> : Cannot use HeapMem
 * object while it is being restored. </li> </ul> </td></tr>
 *
 * </table>
 *
 * @endhtmlonly
 */

/*
 * ======== HeapMultiBuf.h ========
 */

#ifndef ti_sysbios_heaps_HeapMultiBuf__include
#define ti_sysbios_heaps_HeapMultiBuf__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/heaps/HeapBuf.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>
#include <ti/sysbios/runtime/Memory.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_heaps_HeapMultiBuf_long_names
#include "HeapMultiBuf_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Invalid block pointer
 *
 * This Assert is raised if a call to free does not successfully free the block
 * back to any of the buffers. Indicates that the block pointer is invalid, or
 * that the HeapMultiBuf state has been corrupted.
 */
#define HeapMultiBuf_A_blockNotFreed "Invalid block address on the free. Failed to free block back to heap."

/*!
 * @brief Raised when requested size exceeds all blockSizes
 */
#define HeapMultiBuf_E_size "requested size is too big: handle=0x%x, size=%u"

typedef struct {
    /*!
     * @brief Turn block borrowing on (true) or off (false)
     *
     * With block borrowing on, if there are no blocks available of the
     * requested size, then alloc will look for a larger block to return. Calls
     * to alloc which borrow blocks will be slower, and will cause internal
     * fragmentation of the heap (until the block is freed), so it is ideal to
     * configure a HeapMultiBuf such that block borrowing is not needed.
     */
    bool blockBorrow;
    /*!
     * @brief Number of memory buffers
     *
     * The number of different fixed size memory buffers that are managed by the
     * heap instance. The bufParams array has length numBufs.
     *
     * The default number of buffers is 0.
     */
    int numBufs;
    /*!
     * @brief Config parameters for each buffer
     *
     * Each buffer in a HeapMultiBuf is in fact managed by a HeapBuf instance.
     * Configuration of a HeapMultiBuf is done by providing an array of
     * configured HeapBuf parameter structures. Refer to the HeapBuf
     * documentation for information on the buffer parameters. All of the
     * documentation and parameters for HeapBuf apply to HeapMultiBuf. If a
     * buffer is configured incorrectly, HeapBuf, not HeapMultiBuf, will raise
     * an Assert.
     */
    HeapBuf_Params *bufParams;
} HeapMultiBuf_Params;

/*! @cond NODOC */
typedef struct {
    HeapBuf_Handle heapBuf;
    void * lastAddr;
} HeapMultiBuf_AddrPair;
/*! @endcond */

typedef struct {
    IHeap_Object iheap;
    Queue_Elem objElem;
    /*!
     * @brief Turn block borrowing on (true) or off (false)
     *
     * With block borrowing on, if there are no blocks available of the
     * requested size, then alloc will look for a larger block to return. Calls
     * to alloc which borrow blocks will be slower, and will cause internal
     * fragmentation of the heap (until the block is freed), so it is ideal to
     * configure a HeapMultiBuf such that block borrowing is not needed.
     */
    bool blockBorrow;
    int numHeapBufs;
    /*!
     * @brief Number of memory buffers
     *
     * The number of different fixed size memory buffers that are managed by the
     * heap instance. The bufParams array has length numBufs.
     *
     * The default number of buffers is 0.
     */
    int numBufs;
    HeapBuf_Handle *bufsBySize;
    HeapMultiBuf_AddrPair *bufsByAddr;
} HeapMultiBuf_Struct, *HeapMultiBuf_Handle;

typedef HeapMultiBuf_Struct HeapMultiBuf_Object;

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} HeapMultiBuf_Module_State;
/*! @endcond */


/*!
 * @brief cast handle to an IHeap_Handle for use by Memory_alloc, etc.
 *
 * @param handle heap handle
 *
 * @retval IHeap_Handle
 */
#define ti_sysbios_heaps_HeapMultiBuf_Handle_upCast(handle) ((IHeap_Handle)(handle))

/*!
 * @brief HeapMultiBuf will return a block that is >= 'size' with
 * an alignment that is >= 'align'.  The HeapMultiBuf will attempt to service a
 * request for any size; the  specified size does not need to match the
 * configured block sizes of the buffers.
 */
extern void * HeapMultiBuf_alloc(HeapMultiBuf_Handle handle, size_t size, size_t align, Error_Block *eb);

/*!
 * @brief HeapMultiBuf ignores the 'size' parameter to free. It
 * determines the correct buffer to free the block to by comparing  addresses.
 */
extern void HeapMultiBuf_free(HeapMultiBuf_Handle handle, void * buf, size_t size);

/*!
 * @brief This function always returns false since the alloc/free
 * never block on a resource.
 */
extern bool HeapMultiBuf_isBlocking(HeapMultiBuf_Handle handle);

/*!
 * @brief get memory stats for a HeapMultiBuf object
 *
 * @param handle heap handle
 * @param statBuf pointer to a @link Memory_Stats @endlink object
 */
extern void HeapMultiBuf_getStats(HeapMultiBuf_Handle handle, Memory_Stats *statBuf);

/*!
 * @brief Create a `HeapMultiBuf` heap
 *
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapMultiBuf handle (NULL on failure)
 */
extern HeapMultiBuf_Handle HeapMultiBuf_create(const HeapMultiBuf_Params *params, Error_Block *eb);

/*!
 * @brief Construct a `HeapMultiBuf` heap
 *
 * HeapMultiBuf_construct is equivalent to HeapMultiBuf_create except that the 
 * HeapMultiBuf_Struct is pre-allocated.
 *
 * @param obj pointer to a HeapMultiBuf object
 * @param params optional parameters
 * @param eb error block
 *
 * @retval HeapMultiBuf handle (NULL on failure)
 */
extern HeapMultiBuf_Handle HeapMultiBuf_construct(HeapMultiBuf_Struct *obj, const HeapMultiBuf_Params *params, Error_Block *eb);

/*!
 * @brief Delete a `HeapMultiBuf` heap
 *
 * Note that HeapMultiBuf_delete takes a pointer to a HeapMultiBuf_Handle which enables
 * HeapMultiBuf_delete to set the HeapMultiBuf handle to NULL.
 *
 * @param handle pointer to a HeapMultiBuf handle
 */
extern void HeapMultiBuf_delete(HeapMultiBuf_Handle *handle);

/*!
 * @brief Destruct a `HeapMultiBuf` heap
 *
 * @param obj pointer to a HeapMultiBuf objects
 */
extern void HeapMultiBuf_destruct(HeapMultiBuf_Struct *obj);

/*!
 * @brief Initialize the HeapMultiBuf_Params structure with default values.
 *
 * HeapMultiBuf_Params_init initializes the HeapMultiBuf_Params structure with
 * default values. HeapMultiBuf_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be added in
 * the future with compatible defaults -- existing source code does not need to
 * change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void HeapMultiBuf_Params_init(HeapMultiBuf_Params *prms);

/*!
 * @brief return handle of the first HeapMultiBuf on HeapMultiBuf list
 *
 * Return the handle of the first HeapMultiBuf on the create/construct list.
 * NULL if no HeapMultiBufs have been created or constructed.
 *
 * @retval HeapMultiBuf handle
 */
extern HeapMultiBuf_Handle HeapMultiBuf_Object_first(void);

/*!
 * @brief return handle of the next HeapMultiBuf on HeapMultiBuf list
 *
 * Return the handle of the next HeapMultiBuf on the create/construct list.
 * NULL if no more HeapMultiBufs are on the list.
 *
 * @param heap HeapMultiBuf handle
 *
 * @retval HeapMultiBuf handle
 */
extern HeapMultiBuf_Handle HeapMultiBuf_Object_next(HeapMultiBuf_Handle heap);

/*! @cond NODOC */
static inline HeapMultiBuf_Handle HeapMultiBuf_handle(HeapMultiBuf_Struct *str)
{
    return ((HeapMultiBuf_Handle)str);
}

static inline HeapMultiBuf_Struct * HeapMultiBuf_struct(HeapMultiBuf_Handle h)
{
    return ((HeapMultiBuf_Struct *)h);
}

#define HeapMultiBuf_module ((HeapMultiBuf_Module_State *) &(HeapMultiBuf_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif

/*! @cond NODOC */
#undef ti_sysbios_heaps_HeapMultiBuf_long_names
#include "HeapMultiBuf_defs.h"
/*! @endcond */
