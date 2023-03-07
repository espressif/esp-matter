/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 */
/*
 *  ======== HeapBuf.xdc ========
 *
 */

package ti.sysbios.heaps;

import xdc.rov.ViewInfo;

/*!
 *  ======== HeapBuf ========
 *  Fixed size buffer heap manager
 *
 *  The HeapBuf manager provides functions to allocate and free storage from a
 *  heap of type HeapBuf which inherits from IHeap. HeapBuf manages a single
 *  fixed-size buffer, split into equally sized allocable blocks.
 *
 *  The HeapBuf manager is intended as a very fast and deterministic memory
 *  manager which can only allocate blocks of a single size. It is ideal for
 *  managing a heap that is only used for allocating a single type of object,
 *  or for objects that have very similar sizes.
 *
 *  Allocation from and freeing to a HeapBuf instance is non-blocking and
 *  deterministic: a call to alloc or free will always take the same amount of
 *  time.
 *
 *  Constraints:
 *  @p(blist)
 *  - Align parameter must be set to 0 for default, or be a power of 2 that is
 *    greater than or equal to the value returned by
 *    {@link xdc.runtime.Memory#getMaxDefaultTypeAlign}.
 *  - The buffer passed to dynamically create a HeapBuf must be aligned
 *    according to the alignment parameter, and must be large enough to account
 *    for the actual block size after it has been rounded up to a multiple of
 *    the alignment.
 *  @p
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
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
 *    (e.g. HeapBuf_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. HeapBuf_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@InstanceInitError /* Initialization may throw errors */
@ModuleStartup     /* Instances require more initialization at startup */
@InstanceFinalize  /* Destroys the freeList Q */

module HeapBuf inherits xdc.runtime.IHeap {

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String    label;
        xdc.runtime.Memory.Size bufSize;    /* Size of the entire buffer */
        SizeT     blockSize;
        UInt      numBlocks;
        Int       totalFreeSize;
        UInt      numberAllocatedBlocks;
        UInt      numFreeBlocks;
        UInt      maxAllocatedBlocks;
        Ptr       buf;

        /* statStr contains "quick" info 4 HeapBuf that other mods will
         * display. statStr won't be displayed by the HeapBuf ROV view itself.
         */
        String    statStr;
    }

    /*!
     *  ======== DetailedView ========
     *  @_nodoc
     */
    metaonly struct DetailedView {
        String    label;
        xdc.runtime.Memory.Size bufSize;    /* Size of the entire buffer */
        SizeT     blockSize;
        UInt      numBlocks;
        Int       totalFreeSize;
        UInt      numberAllocatedBlocks;
        UInt      numFreeBlocks;
        UInt      maxAllocatedBlocks;
        Ptr       buf;
        Ptr       freeList;

        /* statStr contains "quick" info 4 HeapBuf that other mods will
         * display. statStr won't be displayed by the HeapBuf ROV view itself.
         */
        String    statStr;
    }

    /*! @_nodoc */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                    ['Basic',    {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitInstance', structName: 'BasicView'}],
                    ['Detailed', {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitDetailed', structName: 'DetailedView'}]
            ]
        });

    /*!
     *  ======== ExtendedStats ========
     *  Stat structure for the HeapBuf_getExtendedStats function
     *
     *  @field(maxAllocatedBlocks) The maximum number of blocks allocated from
     *                             this heap at any single point in time, during
     *                             the lifetime of this HeapBuf instance.
     *                             The configuration parameter
     *                             {@link #trackMaxAllocs} must be set to true
     *                             for this field to have any meaning.
     *                             Otherwise, this field will be set to '0'.
     *
     *  @field(numAllocatedBlocks) The total number of blocks currently
     *                             allocated in this HeapBuf instance.
     */
    struct ExtendedStats {
        UInt maxAllocatedBlocks;
        UInt numAllocatedBlocks;
    }

    /*!
     *  ======== A_nullBuf ========
     *  Null buf parameter
     *
     *  This Assert is raised when the buf parameter has been omitted (null)
     *  for a dynamic create.
     */
    config xdc.runtime.Assert.Id A_nullBuf =
        {msg: "buf parameter cannot be null"};

    /*!
     *  ======== A_bufAlign ========
     *  Misalligned buffer
     *
     *  This Assert is raised when the buf parameter has not been aligned
     *  on the requested alignment.
     */
    config xdc.runtime.Assert.Id A_bufAlign =
        {msg: "buf not properly aligned"};

    /*!
     *  ======== A_invalidAlign ========
     *  Invalid alignment
     *
     *  This Assert is raised when the align parameter is not the default
     *  value of zero, or a power of 2 that is greater than or equal to
     *  the value defined by
     *  {@link xdc.runtime.Memory#getMaxDefaultTypeAlign}.
     */
    config xdc.runtime.Assert.Id A_invalidAlign =
        {msg: "align parameter must be 0 or a power of 2 >= the value of Memory_getMaxDefaultTypeAlign()"};

    /*!
     *  ======== A_invalidRequestedAlign ========
     *  Invalid alignment parameter in the alloc
     *
     *  This Assert is raised when the align argument is not a power of 2 (or 0).
     *  It is also raised if the parameter is greater the alignment
     *  specified when creating the heap.
     */
    config xdc.runtime.Assert.Id A_invalidRequestedAlign =
        {msg: "align parameter 1) must be 0 or a power of 2 and 2) not greater than the heaps alignment"};

    /*!
     *  ======== A_invalidBlockSize ========
     *  Assert raised when the blockSize parameter is not large enough to
     *  hold two pointers.
     */
    config xdc.runtime.Assert.Id A_invalidBlockSize =
        {msg: "blockSize must be large enough to hold atleast two pointers"};

    /*!
     *  ======== A_zeroBlocks ========
     *  Assert raised when the numBlocks parameter is 0
     */
    config xdc.runtime.Assert.Id A_zeroBlocks =
        {msg: "numBlocks cannot be zero"};

    /*!
     *  ======== A_zeroBufSize ========
     *  Assert raised when the bufSize parameter is 0
     */
    config xdc.runtime.Assert.Id A_zeroBufSize =
        {msg: "bufSize cannot be zero"};

    /*!
     *  ======== A_invalidBufSize ========
     *  Invalid buffer size
     *
     *  This Assert is raised when the bufSize parameter is too small
     *  to handle requested blocks.
     */
    config xdc.runtime.Assert.Id A_invalidBufSize =
        {msg: "HeapBuf_create's bufSize parameter is invalid (too small)"};

    /*!
     *  ======== A_noBlocksToFree ========
     *  No allocated blocks
     *
     *  This Assert is raised when free is called and there are no blocks
     *  allocated.
     */
    config xdc.runtime.Assert.Id A_noBlocksToFree =
        {msg: "Cannot call HeapBuf_free when no blocks have been allocated"};

    /*!
     *  ======== A_invalidFree ========
     *  Assert raised when an invalid free occurs
     *
     *  This assert can be caused for the following reasons
     *  @p(blist)
     *  - Passing a NULL pointer to the free.
     *  - The block is not within the heap's buffer range. This can occur if
     *    the block is freed to the wrong heap.
     *  - The block is not on the proper boundary. This can occur if the
     *    pointer returned from alloc is changed by the application and not set
     *    back to its original value when free is called.
     *  @p
     */
    config xdc.runtime.Assert.Id A_invalidFree =
        {msg: "A_invalidFree: Invalid free"};

    /*!
     *  ======== E_size ========
     *  Raised when requested size exceeds blockSize
     */
    config xdc.runtime.Error.Id E_size =
        {msg: "requested size is too big: handle=0x%x, size=%u"};

    /*!
     *  ======== trackMaxAllocs ========
     *  Track maximum number of allocations
     *
     *  This will enable/disable the tracking of the maximum number of
     *  allocations for a HeapBuf instance.  This maximum refers to the "all
     *  time" maximum number of allocations for the history of a HeapBuf
     *  instance, not the current number of allocations.
     */
    config Bool trackMaxAllocs = false;

instance:

    /*!
     *  ======== align ========
     *  Alignment (in MAUs) of each block
     *
     *  The alignment must be a power of 2 that is greater than or equal to
     *  the target dependent value defined by
     *  {@link xdc.runtime.Memory#getMaxDefaultTypeAlign}. If the default value
     *  of 0 is specified, this value will be changed to be the minimum
     *  structure alignment requirement, as defined by
     *  {@link xdc.runtime.Memory#getMaxDefaultTypeAlign}.
     *
     *  For static creates, the HeapBuf module will allocate space for the
     *  buffer and will align it on this requested alignment.
     *
     *  For dynamic creates, this parameter is used solely for error checking.
     *  The buffer provided to a dynamically created HeapBuf must be aligned
     *  manually, and an assert will be raised if the buffer is not properly
     *  aligned. For dynamic creates, HeapBuf will NOT adjust the buffer to
     *  satisfy the alignment.
     *
     */
    config SizeT align = 0;

    /*!
     *  ======== sectionName ========
     *  Section name for the buffer managed by the instance
     *
     *  The buffer allocated for a statically created HeapBuf instance will be
     *  placed in this section.
     *
     *  The default section is the 'dataSection' in the platform.
     */
    metaonly config String sectionName = null;

    /*!
     *  ======== numBlocks ========
     *  Number of fixed-size blocks
     *
     *  Required parameter.
     *
     *  The default number of blocks is 0.
     */
    config UInt numBlocks = 0;

    /*!
     *  ======== blockSize ========
     *  Size (in MAUs) of each block
     *
     *  HeapBuf will round the blockSize up to the nearest multiple of the
     *  alignment, so the actual blockSize may be larger. When creating a
     *  HeapBuf dynamically, this needs to be taken into account to determine
     *  the proper buffer size to pass in.
     *
     *  Required parameter.
     *
     *  The default size of the blocks is 0 MAUs.
     */
    config SizeT blockSize = 0;

    /*!
     *  ======== bufSize ========
     *  Size (in MAUs) of the entire buffer; for dynamic creates only
     *
     *  When dynamically creating a HeapBuf, this should be the actual size
     *  of the memory buffer provided for HeapBuf to manage.
     *
     *  This parameter is used solely for error checking; an Error is raised
     *  if the bufSize is too small for the requested blockSize and numBlocks.
     *  Because of minimum alignment considerations, it's easy to provide too
     *  small of a buffer to manage, and difficult to detect if it happened.
     *
     *  This parameter is required for dynamic creates and ignored for static
     *  creates.
     *
     *  The default buffer size is 0 MAUs.
     */
    config xdc.runtime.Memory.Size bufSize = 0;

    /*!
     *  ======== buf ========
     *  User supplied buffer; for dynamic creates only
     *
     *  The caller must supply the memory buffer for a dynamically created
     *  HeapBuf to manage. The provided buffer must be aligned according to the
     *  HeapBuf's alignment parameter. i.e., if a buffer's alignment parameter
     *  is 128, then the buffer's start address must be a multiple of 128.
     *
     *  For static creates, the buffer is allocated automatically based on the
     *  blockSize and numBlocks parameters.
     *
     *  This parameter is required for dynamic creates and ignored for static
     *  creates.
     *
     */
    config Ptr buf = 0;

    /*!
     *  ======== alloc ========
     *  @HeapBuf All alignment is handled in the create, therefore the
     *  align argument in alloc is ignored.
     *
     *  HeapBuf_alloc will only fail if there are no available blocks or if the
     *  requested block size is larger than the HeapBuf's blockSize. All other
     *  requests, regardless of size, will succeed.
     */
    override Ptr alloc(SizeT size, SizeT align, xdc.runtime.Error.Block *eb);

    /*!
     *  ======== isBlocking ========
     *  @HeapBuf This function always returns FALSE since the alloc/free
     *  never block on a resource.
     */
    override Bool isBlocking();

    /*!
     *  ======== getBlockSize ========
     *  @_nodoc
     *  Returns the HeapBuf's actual blockSize (adjusted for alignment)
     *
     *  This getter is provided to allow other memory managers to build off
     *  of HeapBuf.
     *
     *  The blockSize returned has been adjusted to match the alignment,
     *  so it may be larger than the user-specified blockSize.
     */
    SizeT getBlockSize();

    /*!
     *  ======== getAlign ========
     *  @_nodoc
     *  Returns the HeapBuf's actual alignment (adjusted for minimum
     *  alignment requirements)
     *
     *  This getter is provided to allow other memory managers to build off
     *  of HeapBuf.
     */
    SizeT getAlign();

    /*!
     *  ======== getEndAddr ========
     *  @_nodoc
     *  Returns the last address in the buffer managed by the HeapBuf
     *
     *  This getter is provided to allow other memory managers to build off
     *  of HeapBuf:  Compare a block's address to the end address of the buffer
     *  to determine which buffer the block came from. The end address is the
     *  last address in the buffer, not the next address after the buffer.
     *
     *  This will not return the correct value after addMemory has been called
     *  to add a noncontiguous buffer of memory to the HeapBuf.
     *
     */
    Ptr getEndAddr();

    /*! ======== getExtendedStats ========
     *  Retrieves the extended statistics for a HeapBuf instance.
     *
     *  This function retrieves the extended statistics for a HeapBuf instance.
     *  It does not retrieve the standard xdc.runtime.Memory.Stats information.
     *
     *  @param(stats)  Location for the returned extended statistics.
     */
    Void getExtendedStats(ExtendedStats *stats);

    /*!
     *  ======== mergeHeapBufs ========
     *  @_nodoc
     */
    Void mergeHeapBufs(HeapBuf.Handle heapBuf2);

internal:   /* not for client use */

    /*!
     *  ======== numConstructedHeaps ========
     *  @_nodoc
     *  Number of statically constructed Heap objects.
     *  Shouldn't be set directly by the user's
     *  config (it gets set by instance$static$init).
     */
    config UInt numConstructedHeaps = 0;

    /*
     *  ======== postInit ========
     *  finish initializing static and dynamic HeapBufs
     */
    Void postInit(Object *heap);

    /*
     *  ======== Instance_State ========
     *  Instance state
     */
    struct Instance_State {
        SizeT        blockSize;             /* True size of each block       */
        SizeT        align;                 /* Alignment of each block       */
        UInt         numBlocks;             /* Number of individual blocks.  */
        xdc.runtime.Memory.Size bufSize;    /* Size of the entire buffer     */
        Char         buf[];                 /* Memory managed by instance    */
        ti.sysbios.knl.Queue.Object freeList;  /* List of free buffers      */
        UInt         numFreeBlocks;
        UInt         minFreeBlocks;         /* used to compute max # allocs  */
    };

    /*
     *  ======== Module_State ========
     *  Module state
     */
    struct Module_State {
        Handle  constructedHeaps[];         /* array of statically */
                                            /* constructed Heaps */
    };
}
