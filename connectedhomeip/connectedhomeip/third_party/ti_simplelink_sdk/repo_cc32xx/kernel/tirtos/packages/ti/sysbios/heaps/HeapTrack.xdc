/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
/*
 *  ======== HeapTrack.xdc ========
 *
 */
package ti.sysbios.heaps;

import xdc.runtime.Memory;
import xdc.runtime.IHeap;
import ti.sysbios.knl.Task;
import ti.sysbios.knl.Queue;
import xdc.rov.ViewInfo;

/*!
 *  ======== HeapTrack ========
 *  Heap manager that enables tracking of all allocated blocks.
 *
 *  HeapTrack manager provides functions to allocate and free storage from a
 *  configured heap which inherits from IHeap. The calling context is going to
 *  match the heap being used.
 *
 *  HeapTrack is useful for detecting memory leaks, double frees and buffer
 *  overflows.  There is a performance overhead cost when using heap track as
 *  well as a size impact. Every alloc will include a {@link #Tracker}
 *  structure (plus memory to get proper alignment of the stucture) at
 *  the end of the buffer that should not be modified by the user. It is
 *  important to remember this when deciding on heap sizes and you may have to
 *  adjust the total size or buffer sizes (for HeapBuf/HeapMultiBuf).
 *
 *  ROV displays peaks and current in-use for both allocated memory (requested
 *  size + Tracker structure) and requested memory (without Tracker).
 *
 *  The information stored in the tracker packet is used to display information
 *  in RTOS Object Viewer (ROV) as well as with the printTask and printHeap
 *  functions.
 *
 *  The asserts used in this module are listed below and include error checking
 *  for double frees, calling printHeap with a null handle, buffer overflows
 *  and deleting a non empty heap.
 */

@InstanceFinalize  /* Destroys the trackQueue Q */

module HeapTrack inherits xdc.runtime.IHeap {

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        IHeap.Handle heapHandle;
        String inUse;
        String inUsePeak;
        String inUseWithoutTracker;
        String inUsePeakWithoutTracker;
    }

    /*!
     *  ======== TaskView ========
     *  @_nodoc
     */
    metaonly struct TaskView {
        String block;
        String heapHandle;
        String blockAddr;
        String requestedSize;
        String clockTick;
        String overflow;
    }

    /*!
     *  ======== HeapListView ========
     *  @_nodoc
     */
    metaonly struct HeapListView {
        String block;
        String taskHandle;
        String heapHandle;
        String blockAddr;
        String requestedSize;
        String clockTick;
        String overflow;
    }

    /*! @_nodoc */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Basic',    {type: ViewInfo.INSTANCE, viewInitFxn:
                'viewInitBasic', structName: 'BasicView'}],
                ['HeapAllocList', {type: ViewInfo.INSTANCE_DATA, viewInitFxn:
                'viewInitHeapList',  structName: 'HeapListView'}],
                ['TaskAllocList',     {type: ViewInfo.TREE_TABLE, viewInitFxn:
                'viewInitTask',  structName: 'TaskView'}],
            ]
        });

    /*!
     *  ======== Tracker ========
     *  Structure added to the end of each allocated block
     *
     *  When a block is allocated from a HeapTrack heap with a size,
     *  internally HeapTrack calls Memory_alloc on the configured
     *  {@link #heap}. The value of sizeof(HeapTrack_Tracker)
     *  is added to the requested size.
     *
     *  For example, if the caller makes the following call (where heapHandle
     *  is an HeapTrack handle that has been converted to an IHeap_Handle).
     *  @p(code)
     *  buf = Memory_alloc(heapHandle, MYSIZE, MYALIGN, &eb);
     *  @p
     *
     *  Internally, HeapTrack will make the following call
     *  (where size is MYSIZE, align is MYALIGN and obj is the HeapTrack handle).
     *  @p(code)
     *  block = Memory_alloc(obj->heap, size + sizeof(HeapTrack_Tracker), align, &eb);
     *  @p
     *
     *  When using HeapTrack, depending on the  actual heap
     *  (i.e. {@link #heap}), you might need to make adjustments to the heap
     *  (e.g. increase the blockSize if using a HeapBuf instance).
     *
     *  The HeapTrack module manages the contents of this structure and should
     *  not be directly accessing them.
     */
    struct Tracker {
        UArg scribble;
        Queue.Elem queElem;
        SizeT size;
        UInt32 tick;
        Task.Handle taskHandle;
    }

    /*!
     *  ======== STARTSCRIBBLE ========
     *  @_nodoc
     *  Constant used to help detect over-writing of a buffer
     */
    const UArg STARTSCRIBBLE = 0xa5a5a5a5;

    /*!
     *  ======== printTask ========
     *  Print out the blocks that are currently allocated by a task
     *
     *  Iterates through all instances of HeapTrack and prints out information
     *  about all allocated blocks of memory for a given task handle. This
     *  function is not thread safe.
     *
     *  @params(taskHandle)  Task to print stats for
     */
    Void printTask(Task.Handle taskHandle);

    /*!
     *  ======== A_doubleFree ========
     *  Assert raised when freeing a buffer that was already freed
     */
    config xdc.runtime.Assert.Id A_doubleFree =
            {msg: "A_doubleFree: Buffer already free"};

    /*!
     *  ======== A_bufOverflow ========
     *  Assert raised when freeing memory with corrupted data or using the
     *  wrong size
     */
    config xdc.runtime.Assert.Id A_bufOverflow =
            {msg: "A_bufOverflow: Buffer overflow"};

    /*!
     *  ======== A_notEmpty ========
     *  Assert raised when deleting a non-empty heap
     */
    config xdc.runtime.Assert.Id A_notEmpty =
        {msg: "A_notEmpty: Heap not empty"};


    /*!
     *  ======== A_nullObject ========
     *  Assert raised when calling printTask with a null HeapTrack object
     */
    config xdc.runtime.Assert.Id A_nullObject =
            {msg: "A_nullObject: HeapTrack_printHeap called with null obj"};

instance:

    /*!
     *  ======== internalHeap ========
     *  Heap to use with HeapTrack
     */
    config IHeap.Handle heap = null;

    /*!
     *  ======== printHeap ========
     *  Print details for a HeapTrack instance
     *
     *  Print the details of all allocated blocks of memory for a HeapTrack
     *  instance.  This function is not thread safe.
     */
    Void printHeap();

internal:

    /*
     *  ======== NOSCRIBBLE ========
     *  Using a non-zero constant in the free to aid in the development
     *  of this module.
     */
    const UArg NOSCRIBBLE = 0x05101920;

    /*
     *  ======== printTrack ========
     *  Returns FALSE if a corrupted scribble is found.
     */
    Bool printTrack(Tracker *tracker, Handle handle);

    /* instance object */
    struct Instance_State {
        IHeap.Handle                internalHeap;
        ti.sysbios.knl.Queue.Object trackQueue;
        SizeT                       size;
        SizeT                       peak;
        SizeT                       sizeWithoutTracker;
        SizeT                       peakWithoutTracker;
    };
}
