/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *  ======== HeapMultiBuf.xdc ========
 *
 */

package ti.sysbios.heaps;

import xdc.rov.ViewInfo;

/*!
 *  ======== HeapMultiBuf ========
 *  Multiple fixed size buffer heap manager
 *
 *  The HeapMultiBuf manager provides functions to allocate and free storage 
 *  from a heap of type HeapMultiBuf which inherits from IHeap. HeapMultiBuf 
 *  manages multiple fixed-size memory buffers. Each buffer contains a fixed 
 *  number of allocable memory 'blocks' of the same size. Simply put, a 
 *  HeapMultiBuf instance manages a collection of HeapBuf instances.
 *  HeapMultiBuf is intended as a fast and deterministic memory manager which
 *  can service requests for blocks of arbitrary size. 
 *
 *  An example HeapMultiBuf instance might have sixteen 32-byte blocks in one 
 *  buffer, and four 128-byte blocks in another buffer. A request for memory
 *  will be serviced by the smallest possible block, so a request for 100
 *  bytes would receive a 128-byte block in our example.
 *
 *  Allocating from HeapMultiBuf will try to return a block from the first 
 *  buffer which has:
 *
 *    1. A block size that is >= to the requested size
 *
 *    AND
 *
 *    2. An alignment that is >= to the requested alignment 
 *
 *  If the first matching buffer is empty, HeapMultiBuf will only continue
 *  searching for a block if 'block borrowing' is enabled (see Block
 *  Borrowing).
 *
 *  @p(html)
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
 *  @p(html)
 *  <h4>Configuration Example</h4>
 *  The following configuration code creates a HeapMultiBuf instance which
 *  manages 3 pools of 10 blocks each, with block sizes of 64, 128 and 256.
 *
 *  @p(code)
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
 *  @p
 *
 *  @p(html)
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
 *  @p(blist)
 *      - A buffer with a requested alignment of 0 will receive the target-
 *        specific minimum alignment.
 *      - The actual block sizes will be a multiple of the alignment. For
 *        example, if a buffer is configured to have 56-byte blocks with an
 *        alignment of 32, HeapMultiBuf will actually create 64-byte blocks. 
 *        When providing the buffer for a dynamic create, make sure it is 
 *        large enough to take this into account.
 *      - Multiple buffers with the same block size ARE allowed. This may
 *        occur, for example, if sizeof is used to specify the block sizes.
 *      - If any buffers have both the same block size and alignment, they
 *        will be merged. If this is a problem, consider managing these buffers
 *        directly with HeapBuf objects.
 *  @p
 *
 *  @p(html)
 *  <h4>Real-Time Concerns</h4>
 *  Allocation from and freeing to a HeapMultiBuf instance is non-blocking.
 *  
 *  HeapMultiBuf is deterministic:
 *  @p(blist) 
 *    - A call to alloc will always take the same amount of time for a given
 *      block size (with block borrowing disabled).
 *    - The worst case call to free is constant and proportional to the number
 *      of buffers managed.
 *  @p
 *
 *  @p(html)
 *  <h4>Restrictions</h4>
 *  @p(blist)
 *  - Align parameters must be a power of 2.
 *  - The buffers passed to dynamically create a HeapMultiBuf must be aligned
 *    according to the alignment parameter, and must be large enough to account
 *    for the actual block size after it has been rounded up to a multiple of 
 *    the alignment. 
 *  - Buffers must be provided to dynamically create a HeapMultiBuf, and cannot
 *    be provided to statically create a HeapMultiBuf.
 *  @p
 *
 *  @p(html)
 *  <h4>Unconstrained Functions</h4>
 *  All functions
 *
 *  @p(html) 
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *  
 *    <tr><th> Function    </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                         -->
 *    <tr><td> Params_init      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> alloc            </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> construct        </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> create           </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> delete           </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> destruct         </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> free             </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getStats         </td><td>   Y    </td><td>   Y    </td>
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
 *    (e.g. HeapMultiBuf_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul> 
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started 
 *    (e.g. HeapMultiBuf_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *       <li> <b>**</b>: Assuming GateMutex is used as HeapMem's Gate. </li>
 *       <li> <b>+</b> : Cannot use HeapMem object while it is being 
 *    restored. </li>
 *       </ul>
 *    </td></tr>
 *  
 *  </table>
 *  @p 
 */

@InstanceInitError     /* Instance_init may throw an error */
@InstanceFinalize      /* to destruct HeapBufs */
@ModuleStartup         /* to determine buffer addresses */
 
module HeapMultiBuf inherits xdc.runtime.IHeap {

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String            label;
        Bool              blockBorrow;
        Int               numHeapBufs;
    }

    /*!
     *  ======== DetailedView ========
     *  @_nodoc
     */
    metaonly struct DetailedView {
        String                  label;
        Bool              blockBorrow;
        Int               numHeapBufs;
        String       HeapBufHandles[];
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
            ]
        });
    
    /*!
     *  ======== A_blockNotFreed ========
     *  Invalid block pointer
     *
     *  This Assert is raised if a call to free does not successfully
     *  free the block back to any of the buffers. Indicates that the
     *  block pointer is invalid, or that the HeapMultiBuf state has been
     *  corrupted.
     */
    config xdc.runtime.Assert.Id A_blockNotFreed = 
        {msg: "Invalid block address on the free. Failed to free block back to heap."};
       
    /*!
     *  ======== E_size ========
     *  Raised when requested size exceeds all blockSizes
     */
    config xdc.runtime.Error.Id E_size = 
        {msg: "requested size is too big: handle=0x%x, size=%u"};    
    
instance:
    
    /*!
     *  ======== numBufs ========
     *  Number of memory buffers
     *
     *  The number of different fixed size memory buffers that are managed 
     *  by the heap instance. The bufParams array has length numBufs.
     *
     *  The default number of buffers is 0.
     */
    config Int numBufs = 0;

    /*!
     *  ======== blockBorrow ========
     *  Turn block borrowing on (true) or off (false)
     *
     *  With block borrowing on, if there are no blocks available of the
     *  requested size, then alloc will look for a larger block to return.
     *  Calls to alloc which borrow blocks will be slower, and will cause
     *  internal fragmentation of the heap (until the block is freed), so it 
     *  is ideal to configure a HeapMultiBuf such that block borrowing is not
     *  needed.
     */
    config Bool blockBorrow = false; 

    /*!
     *  ======== bufParams ========
     *  Config parameters for each buffer
     *
     *  Each buffer in a HeapMultiBuf is in fact managed by a HeapBuf instance.
     *  Configuration of a HeapMultiBuf is done by providing an array of 
     *  configured HeapBuf parameter structures. Refer to the HeapBuf
     *  documentation for information on the buffer parameters. All of the
     *  documentation and parameters for HeapBuf apply to HeapMultiBuf.
     *  If a buffer is configured incorrectly, HeapBuf, not HeapMultiBuf,
     *  will raise an Assert.
     */
    config HeapBuf.Params bufParams[];

    /*!
     *  ======== alloc ========
     *
     *  @HeapMultiBuf
     *  HeapMultiBuf will return a block that is >= 'size' with
     *  an alignment that is >= 'align'. 
     *  The HeapMultiBuf will attempt to service a request for any size; the 
     *  specified size does not need to match the configured block sizes of
     *  the buffers.
     */
    override Ptr alloc(SizeT size, SizeT align, xdc.runtime.Error.Block *eb);

    /*!
     *  ======== free ========
     *
     *  @HeapMultiBuf
     *  HeapMultiBuf ignores the 'size' parameter to free. It
     *  determines the correct buffer to free the block to by comparing 
     *  addresses.
     */
    override Void free(Ptr block, SizeT size);
    
    /*!
     *  ======== isBlocking ========
     *
     *  @HeapMultiBuf
     *  This function always returns FALSE since the alloc/free
     *  never block on a resource.
     */
    override Bool isBlocking();    
    
internal:   /* not for client use */

    /*
     *  ======== AddrPair ========
     *  Used to sort bufs by address
     *
     *  A dynamically created HeapMultiBuf may have multiple AddrPairs
     *  per HeapBuf because of merging.
     */
    struct AddrPair {
        HeapBuf.Handle      heapBuf;  /* The heapBuf that manages lastAddr */
        Ptr                 lastAddr; /* The last address in the buffer */
    }

    /*
     *  ======== addrPairCompare ========
     */
    Int addrPairCompare(const Void *a, const Void *b);

    /*
     *  ======== sizeAlignCompare ========
     */
    Int sizeAlignCompare(const Void *a, const Void *b);

    /*
     *  ======== borrowBlock ========
     */
    Void *borrowBlock(HeapMultiBuf.Object *obj, SizeT size, SizeT align,
                      Int startIndex);
    /*
     *  ======== Instance_State ========
     * The bufsBySize array has length numHeapBufs and is used for alloc.
     * The bufsyByAddr array has length numBufs and is used for free.
     */
    struct Instance_State {        
        Bool              blockBorrow;  /* Enable/Disable block borrowing */
        Int               numHeapBufs;  /* Number of HeapBuf instances    */
        HeapBuf.Handle    bufsBySize[]; /* HeapBufs, sorted by size       */
        Int               numBufs;      /* Number of memory buffers       */
        AddrPair          bufsByAddr[]; /* HeapBufs, sorted by address    */
    };
}
