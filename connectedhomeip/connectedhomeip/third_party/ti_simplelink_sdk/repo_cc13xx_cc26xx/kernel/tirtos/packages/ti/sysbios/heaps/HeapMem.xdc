/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== HeapMem.xdc ========
 *
 */

package ti.sysbios.heaps;

import xdc.rov.ViewInfo;
import xdc.runtime.Memory;
import xdc.runtime.Error;

/*!
 *  ======== HeapMem ========
 *  Variable size buffer heap manager
 *
 *  HeapMem manager provides functions to allocate and free storage from a
 *  heap of type HeapMem which inherits from IHeap.
 *
 *  In addition to allowing multiple static HeapMem instances to be created
 *  who's buffer placements and sizes are defined by their instance
 *  configuration parameters,
 *  HeapMem allows one Heap instance to be defined who's heap
 *  memory is defined by buffer start and end symbols in the linker command
 *  file. This singular Heap instance is referred to as the 'Primary Heap'.
 *
 *  see {@link #primaryHeapBaseAddr}, {@link #primaryHeapEndAddr},
 *  and {@link #usePrimaryHeap}
 *
 *  @p(html)
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
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function    </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                     -->
 *    <tr><td> Params_init         </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> alloc               </td><td>   N**  </td><td>   N**  </td>
 *    <td>   Y**  </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> construct           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> create              </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> delete              </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> destruct            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> free                </td><td>   N**  </td><td>   N**  </td>
 *    <td>   Y**  </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getExtendedStats    </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getStats            </td><td>   N**  </td><td>   N**  </td>
 *    <td>   Y**  </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> isBlocking          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> restore             </td><td>   Y+   </td><td>   Y+   </td>
 *    <td>   Y+   </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. HeapMem_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. HeapMem_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       <li> <b>*</b> : Assuming blocking Heap is used for creation. </li>
 *       <li> <b>**</b>: Assuming GateMutex is used as HeapMem's Gate. </li>
 *       <li> <b>+</b> : Cannot use HeapMem object while it is being
 *    restored. </li>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */
@Gated
module HeapMem inherits xdc.runtime.IHeap {

    /*! @_nodoc */
    @XmlDtd
    metaonly struct BasicView {
        String            label;
        Ptr               buf;
        SizeT             minBlockAlign;
        String            sectionName;
    }

    /*! @_nodoc */
    @XmlDtd
    metaonly struct DetailedView {
        String            label;
        Ptr               buf;
        SizeT             minBlockAlign;
        String            sectionName;
        Memory.Size       totalSize;
        Memory.Size       totalFreeSize;
        Memory.Size       largestFreeSize;
    }

    /*!
     *  ======== FreeBlockView ========
     *  @_nodoc
     */
    metaonly struct FreeBlockView {
        String         Address;
        String         size;
        String  next;
        String         status;
    }

    /*! @_nodoc */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
            [
                'Basic',
                {
                    type: ViewInfo.INSTANCE,
                    viewInitFxn: 'viewInitBasic',
                    structName: 'BasicView'
                }
            ],
            [
                'Detailed',
                {
                    type: ViewInfo.INSTANCE,
                    viewInitFxn: 'viewInitDetailed',
                    structName: 'DetailedView'
                }
            ],
            [
                'FreeList',
                {
                    type: ViewInfo.INSTANCE_DATA,
                    viewInitFxn: 'viewInitData',
                    structName: 'FreeBlockView'
                }
            ]
            ]
        });


    /*!
     *  ======== ExtendedStats ========
     *  Stat structure for the HeapMem_getExtendedStats function
     *
     *  @field(buf)           Base address of the internal buffer.
     *                        This may be different from the original buf
     *                        parameter due to alignment requirements.
     *  @field(size)          Size of the internal buffer.
     *                        This may be different from the original size
     *                        parameter due to alignment requirements.
     */
    struct ExtendedStats {
        Ptr   buf;
        SizeT size;
    }

    /*!
     *  ======== A_zeroBlock ========
     *  Assert raised when a block of size 0 is requested
     *
     *  This error can also be raised if the requested size wraps
     *  the contents of a SizeT type when it is adjusted for a minimum
     *  alignment. For example, when SizeT is 16-bits and a size request
     *  is made for 0xFFFB.
     */
    config xdc.runtime.Assert.Id A_zeroBlock =
        {msg: "A_zeroBlock: Cannot allocate size 0"};

    /*!
     *  ======== A_heapSize ========
     *  Assert raised when the requested heap size is too small
     */
    config xdc.runtime.Assert.Id A_heapSize =
        {msg: "A_heapSize: Requested heap size is too small"};

    /*!
     *  ======== A_align ========
     *  Assert raised when the requested alignment is not a power of 2
     */
    config xdc.runtime.Assert.Id A_align =
        {msg: "A_align: Requested align is not a power of 2"};

    /*!
     *  ======== E_memory ========
     *  Raised when requested size exceeds largest free block
     */
    config Error.Id E_memory = {msg: "out of memory: handle=0x%x, size=%u"};

    /*!
     *  ======== A_invalidFree ========
     *  Assert raised when the free detects that an invalid addr or size
     *
     *  This could arise when multiple frees are done on the same buffer or
     *  if corruption occurred.
     *
     *  This also could occur when an alloc is made with size N and the
     *  free for this buffer specifies size M where M > N. Note: not every
     *  case is detectable.
     *
     *  This assert can also be caused when passing an invalid addr to free
     *  or if the size is causing the end of the buffer to be
     *  out of the expected range.
     */
    config xdc.runtime.Assert.Id A_invalidFree =
        {msg: "A_invalidFree: Invalid free"};

    /*!
     *  ======== primaryHeapBaseAddr ========
     *  Base address of the 'Primary Heap' buffer.
     *
     *  HeapMem allows one Heap instance to be defined who's heap
     *  memory is defined by symbols in the linker command file.
     *  This singular Heap instance is referred to as the 'Primary Heap'.
     *
     *  see {@link #primaryHeapEndAddr}
     *  see {@link #usePrimaryHeap}
     *
     *  The following example will create a HeapMem instance whose
     *  size and buffer will be determined at runtime based on the
     *  values of the symbols `__primary_heap_start__` and
     *  `__primary_heap_end__`. It is assumed the user will define these
     *  symbols in their linker command file.
     *
     *  @p(code)
     *  var HeapMem = xdc.useModule('ti.sysbios.heaps.HeapMem');
     *
     *  HeapMem.primaryHeapBaseAddr = "&__primary_heap_start__";
     *  HeapMem.primaryHeapEndAddr = "&__primary_heap_end__";
     *
     *  var heapMemParams = new HeapMem.Params;
     *  heapMemParams.usePrimaryHeap = true;
     *
     *  var heap0 = HeapMem.create(heapMemParams);
     *  @p
     */
    config Char *primaryHeapBaseAddr = null;

    /*!
     *  ======== primaryHeapEndAddr ========
     *  End address of the 'Primary Heap' buffer, plus one.
     *
     *  see {@link #primaryHeapBaseAddr}
     *  see {@link #usePrimaryHeap}
     *
     *  @p(code)
     *  HeapMem.primaryHeapEndAddr = "&__primary_heap_end__";
     *  @p
     */
    config Char *primaryHeapEndAddr = null;

    /*!
     *  ======== enter ========
     *  @_nodoc
     *  Enter the module's gate. This is needed to support
     *  the legacy MEM module. It allows MEM to use the same
     *  gate for thread-safety.
     */
    IArg enter();

    /*!
     *  ======== leave ========
     *  @_nodoc
     *  Leave the module's gate. This is needed to support
     *  the legacy MEM module. It allows MEM to use the same
     *  gate for thread-safety.
     */
    Void leave(IArg key);

instance:

    /*!
     *  ======== usePrimaryHeap ========
     *  Use {@link #primaryHeapBaseAddr} and {@link #primaryHeapEndAddr}
     *  to define the Heap buffer managed by this static instance.
     *
     *  @a(warning)
     *  This instance parameter only exists for statically defined
     *  HeapMem objects and can only be set to true in the creation
     *  of one static HeapMem object!
     */
    metaonly config Bool usePrimaryHeap = false;

    /*!
     *  ======== align ========
     *  Alignment of the buffer being managed by this heap instance
     *
     *  In the static HeapMem.create() call, the buffer allocated for the
     *  HeapMem instance will have the alignment specified by this parameter.
     *
     *  In the dynamic case, the client must supply the buffer, so it is the
     *  client's responsibility to manage the buffer's alignment, and there is
     *  no 'align' parameter.
     *
     *  The specified alignment must be a power of 2.
     *
     *  HeapMem requires that the buffer be aligned on a target-specific minimum
     *  alignment, and will adjust (round up) the requested alignment as
     *  necessary to satisfy this requirement.
     *
     *  The default alignment is 0.
     */
    metaonly config SizeT align = 0;

    /*!
     *  ======== minBlockAlign ========
     *  Minimum alignment for each block allocated
     *
     *  This parameter dictates the minimum alignment for each
     *  block that is allocated. If an alignment request of greater than
     *  minBlockAlign is made in the alloc, it will be honored. If an
     *  alignment request of less than minBlockAlign is made, the request will
     *  be ignored and minBlockAlign will be used.
     *
     *  @p(code)
     *  HeapMem_Params_init(&prms);
     *  prms.minBlockAlign = 32;
     *  handle = HeapMem_create(&prms, &eb);
     *  ...
     *  // buf will be aligned on a 32 MAU boundary
     *  buf = Memory_alloc(HeapMem_Handle_upCast(handle), SIZE, 8, &eb);
     *
     *  // buf will be aligned on a 64 MAU boundary
     *  buf = Memory_alloc(HeapMem_Handle_upCast(handle), SIZE, 64, &eb);
     *  @p
     *
     *  The default alignment is 0 (which means this parameter is ignored).
     */
    config SizeT minBlockAlign = 0;

    /*!
     *  ======== sectionName ========
     *  Section name for the buffer managed by the instance
     *
     *  The default section is the 'dataSection' in the platform.
     */
    metaonly config String sectionName = null;

    /*!
     *  ======== buf ========
     *  Buffer being managed by this heap instance
     *
     *  This parameter is ignored in the static HeapMem.create() call. It is a
     *  required parameter in the dynamic HeapMem_create() call.
     *
     *  HeapMem requires that the buffer be aligned on a target-specific minimum
     *  alignment, and will adjust the buffer's start address and size as
     *  necessary to satisfy this requirement.
     */
    config Ptr buf = 0;

    /*!
     *  ======== size ========
     *  Size of buffer being managed by this heap instance
     *
     *  The usable size may be smaller depending on alignment requirements.
     */
    config Memory.Size size = 0;

    /*!
     *  ======== alloc ========
     *
     *  @HeapMem
     *  The actual block returned may be larger than requested to satisfy
     *  alignment requirements, and its size will always be a multiple of
     *  the size of the HeapMem_Header data structure (usually 8 bytes)
     *
     *  HeapMem_alloc() will lock the heap using the HeapMem Gate while it
     *  traverses the list of free blocks to find a large enough block for
     *  the request.
     *
     *  HeapMem_alloc() should not be called directly.  Application code
     *  should use Memory_alloc() with a HeapMem_Handle as the first
     *  parameter.  Among other things, Memory_alloc() makes sure that the
     *  alignment parameter is greater than or equal to the minimum alignment
     *  required for the HeapMem_Header data structure for a given C compiler
     *  (8 bytes in most cases). HeapMem_alloc() may crash if you pass a
     *  smaller alignment.
     *
     *  Guidelines for using large heaps and multiple alloc() calls.
     *  @p(blist)
     *          - If possible, allocate larger blocks first. Previous
     *            allocations of small memory blocks can reduce the size
     *            of the blocks available for larger memory allocations.
     *          - Realize that alloc() can fail even if the heap contains a
     *            sufficient absolute amount of unallocated space. This is
     *            because the largest free memory block may be smaller than
     *            total amount of unallocated memory.
     *  @p
     *
     *  @param(size)  Requested size
     *  @param(align) Requested alignment
     *  @param(eb)    Error_Block used to denote location in case of a failure
     *
     *  @b(returns)    allocated block or NULL is request cannot be honored
     */
    override Ptr alloc(SizeT size, SizeT align, xdc.runtime.Error.Block *eb);

    /*!
     *  ======== allocUnprotected ========
     *
     *  @HeapMem
     *  The actual block returned may be larger than requested to satisfy
     *  alignment requirements
     *
     *  HeapMem_allocUnprotected() will
     *  traverses the list of free blocks to find a large enough block for
     *  the request.
     *
     *  The caller of this API must provider the thread-safety
     *  of this call.
     *
     *  Guidelines for using large heaps and multiple alloc() calls.
     *  @p(blist)
     *          - If possible, allocate larger blocks first. Previous
     *            allocations of small memory blocks can reduce the size
     *            of the blocks available for larger memory allocations.
     *          - Realize that alloc() can fail even if the heap contains a
     *            sufficient absolute amount of unallocated space. This is
     *            because the largest free memory block may be smaller than
     *            total amount of unallocated memory.
     *  @p
     *
     *  @param(size)  Requested size
     *  @param(align) Requested alignment
     *
     *  @b(returns)    allocated block or NULL is request cannot be honored
     */
    Ptr allocUnprotected(SizeT size, SizeT align);

    /*!
     *  ======== free ========
     *
     *  @HeapMem
     *  free() places the memory block specified by addr and size back into the
     *  free pool of the heap specified. The newly freed block is combined with
     *  any adjacent free blocks. The space is then available for further
     *  allocation by alloc().
     *
     *  free() will lock the heap using the HeapMem Gate, if one is specified
     *  using 'HeapMem.common$.gate'.
     *
     *  @param(block) Block to be freed
     *  @param(size)  Size of block to free
     */
    override Void free(Ptr block, SizeT size);

    /*!
     *  ======== freeUnprotected ========
     *
     *  @HeapMem
     *  freeNoGate() places the memory block specified by addr and size back into the
     *  free pool of the heap specified. The newly freed block is combined with
     *  any adjacent free blocks. The space is then available for further
     *  allocation by alloc().
     *
     *  The caller of this API must provider the thread-safety
     *  of this call.
     *
     *  @param(block) Block to be freed
     *  @param(size)  Size of block to free
     */
    Void freeUnprotected(Ptr block, SizeT size);

    /*!
     *  ======== isBlocking ========
     *
     *  @HeapMem
     *  This function queries the gate (as specified by 'HeapMem.common$.gate')
     *  to determine if the alloc/free can be blocking.
     */
    override Bool isBlocking();

    /*!
     *  ======== getStats ========
     *
     *  @HeapMem
     *  getStats() will lock the heap using the HeapMem Gate while it retrieves
     *  the HeapMem's statistics.
     *
     *  The returned totalSize reflects the usable size of the buffer, not
     *  necessarily the size specified during create.
     */
    override Void getStats(xdc.runtime.Memory.Stats *stats);


    /*!
     *  ======== restore ========
     *  Restores an instance to its original created state
     *
     *  This function restores a static or dynamically created instance to
     *  its original created state. Any memory previously allocated from the
     *  heap is no longer valid after this API is called. This function
     *  does not check whether there is allocated memory or not.
     */
    Void restore();

    /*!
     *  ======== getExtendedStats ========
     *  Retrieves the extended statistics for a HeapMem instance
     *
     *  This function retrieves the extended statistics for a HeapMem instance.
     *  It does not retrieve the standard xdc.runtime.Memory.Stats information.
     *
     *  @param(stats)  Location for the returned extended statistics.
     */
    Void getExtendedStats(ExtendedStats *stats);


internal:   /* not for client use */

    /*!
     *  ======== init ========
     *  Initialize instance at runtime
     *
     *  This function is plugged as a Startup.firstFxn so that the
     *  HeapMem objects are ready and usable by malloc() and
     *  Memory_alloc() by the time the module startup functions
     *  get called so that any calls to atexit(), which in some targets
     *  invokes malloc(), will be handled cleanly.
     */
    Void init();

    /*!
     *  ======== initPrimary ========
     *  Initialize instance at runtime
     *
     *  Same as 'init' but supports the use of 'HeapMem.primaryHeapBaseAddress'.
     */
    Void initPrimary();

    /* Required alignment. Must be a power of 2 */
    config SizeT reqAlign;

    /* Header maintained at the lower address of every free block */
    struct Header {
        Header *next;
        Memory.Size size;
    };

    struct Instance_State {
        xdc.runtime.Memory.Size align;
        Char                    buf[];      /* The heap used by HeapMem. */
        Header                  head;       /* First free block pointer.
                                             * The size field will be used
                                             * to store original heap size.
                                             */
        SizeT                   minBlockAlign;
    };
}

