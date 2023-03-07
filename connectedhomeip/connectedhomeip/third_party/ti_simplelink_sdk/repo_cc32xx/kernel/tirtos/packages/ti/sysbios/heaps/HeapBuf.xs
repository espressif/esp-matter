/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 *  ======== HeapBuf.xs ========
 *
 */

var Queue = null;
var HeapBuf;

/*
 *  ======== module$use ========
 */
function module$use()
{
    /* For error logging */
    HeapBuf = this;
    
    Queue = xdc.useModule("ti.sysbios.knl.Queue");
    xdc.useModule("ti.sysbios.hal.Hwi");
    xdc.useModule("xdc.runtime.Assert");
}

/*
 *  ======== instance$static$init ========
 *  Initializes a statically created HeapBuf object.
 */
function instance$static$init(obj, params)
{   
    var Memory = xdc.module('xdc.runtime.Memory');
    var Program = xdc.module('xdc.cfg.Program');
    var target = Program.build.target;
    var mod = this.$module.$object;

    var minAlign = Memory.getMaxDefaultTypeAlignMeta();

    checkParams.call(this, params, minAlign);
    
    /* Calculate the total length of the buffer */
    if (target.alignDirectiveSupported == true) {
        obj.buf.length = params.blockSize * params.numBlocks;
    }
    else {
        /* 
         * Add padding of (align - 1) if alignment is done manually at 
         * runtime. 
         */
        obj.buf.length = params.blockSize * params.numBlocks + (params.align - 1);
    }
    
    /*  
     *  The following will place a memory buffer into section name. This
     *  buffer is the memory that this instance will manage. 
     */
    Memory.staticPlace(obj.buf, params.align, params.sectionName);
        
    /* Finish initializing the object. */
    Queue.construct(obj.freeList);
    obj.blockSize       = params.blockSize;
    obj.align           = params.align;
    obj.numBlocks       = params.numBlocks;
    obj.numFreeBlocks   = params.numBlocks;
    obj.minFreeBlocks   = -1;
    obj.bufSize         = obj.numBlocks * obj.blockSize;
    
    /* The buffer will be divided into blocks at runtime. */

    /* add constructed Heaps to constructedHeaps array */
    if (this.$category == "Struct") {
        mod.constructedHeaps.$add(this);
        HeapBuf.numConstructedHeaps += 1;
    }
}

/*
 *  ======== checkParams ========
 */
function checkParams(params, minAlign)
{
    /* section name should not be undefined */
    if (params.sectionName == undefined) {
        params.sectionName = null;
    }
    
    /* numBlocks must be specified by the user. */
    if ((params.numBlocks == undefined) || (params.numBlocks == 0)) {
        HeapBuf.$logFatal("numBlocks must be specified.", this, "numBlocks");
    }

    /* blockSize must be specified by the user. */
    if ((params.blockSize == undefined) || (params.blockSize == 0)) {
        HeapBuf.$logFatal("blockSize must be specified.", this, "blockSize");
    }
    
    /* If requested align = 0, give minimum alignment */
    if (params.align == 0) {
        params.align = minAlign; 
    }
    
    /* align must be power of 2 >= minAlign */
    if (((~(params.align-1) & params.align) != params.align) ||
            (params.align < minAlign)) {
        HeapBuf.$logFatal("The requested alignment (" + params.align +
                ") is invalid; it must be 0 or a power of 2 >= the value of" +
                " Memory.getMaxDefaultTypeAlignMeta().", this, "align");
    }

    /* Block size must be multiple of align */
    if ((params.blockSize % params.align) != 0) {
        var oldBlockSize = params.blockSize;
        
        /* Round up blockSize */
        params.blockSize = 
            (params.blockSize + (params.align - 1)) & ~(params.align - 1);
        
        /* Warn that blockSize has increased. */
        HeapBuf.$logWarning("Requested blockSize (" + oldBlockSize +
                            ") is not large enough to satisfy " +
                            "alignment (" + params.align + "). " +
                            "blockSize is now " + params.blockSize +
                            ".", this, "blockSize");
    }
        
    /* Set the bufSize */
    params.buf = 0;
}

/*
 *  ======== viewInitInstance ========
 *  Initialize the 'Basic' HeapBuf instance view.
 */
function viewInitInstance(view, obj)
{    
    var HeapBuf = xdc.useModule('ti.sysbios.heaps.HeapBuf');
    var Program = xdc.useModule('xdc.rov.Program');

    view.label         = Program.getShortName(obj.$label);
    view.bufSize       = obj.bufSize;
    view.blockSize     = obj.blockSize;
    view.numBlocks     = obj.numBlocks;
    view.totalFreeSize = obj.blockSize * obj.numFreeBlocks;

    view.numberAllocatedBlocks = obj.numBlocks - obj.numFreeBlocks;
    view.numFreeBlocks         = obj.numFreeBlocks;

    if (view.numFreeBlocks > view.numBlocks) {
        view.$status["numFreeBlocks"] = "Error: number of free blocks is " +
                "greater than the total number of blocks (possibly caused " +
                "by an invalid free)";
    }

    if (view.numFreeBlocks < 0) {
        view.$status["numFreeBlocks"] = "Error: numFreeBlocks < 0!";
    }
   
    if (view.numberAllocatedBlocks < 0) {
        view.$status["numberAllocatedBlocks"] = "Error: numberAllocatedBlocks" +
                " < 0! (possibly caused by an invalid free)";
    }
  
    /*
     * compute maxAllocatedBlocks
     * 
     * Since minFreeBlocks is read as an unsigned int, its initial value of -1
     * will be read as 65,535 for the 16-bit processor case, and as
     * 4,294,967,295 for the 32-bit case.  So, for 16-bit compare against 0xFFFF
     * and for 32-bit case compare against 0xFFFFFFFF.
     */
    var numBitsPerInt = Program.build.target.bitsPerChar *
            Program.build.target.stdTypes.t_Int.size;

    if (numBitsPerInt == 16) {
        view.maxAllocatedBlocks = (obj.minFreeBlocks == 0xFFFF) ?
                                   0 : obj.numBlocks - obj.minFreeBlocks;
    }
    else if (numBitsPerInt == 32) {
        view.maxAllocatedBlocks = (obj.minFreeBlocks == 0xFFFFFFFF) ?
                                   0 : obj.numBlocks - obj.minFreeBlocks;
    }
    else {
        view.$status["maxAllocatedBlocks"] = "error: computation of " +
                "maxAllocatedBlocks unsupported for " + numBitsPerInt +
                "-bit processors";
    }

    // create a "statistics string" - this is used by other module's ROV views:
    view.statStr = (view.label == "" || view.label == undefined) ?
            "" + utils.toHex(Number(view.address)) : view.label;

    // now append the stats we want to display for HeapBuf by other modules
    view.statStr += ", Size: " + view.bufSize + ", Free: " +
            view.numFreeBlocks + " / " + view.numBlocks;

    view.buf = obj.buf;
}

/*
 *  ======== viewInitDetailed ========
 *  Initialize the 'Detailed' HeapBuf instance view.
 */
function viewInitDetailed(view, obj)
{    
    var HeapBuf = xdc.useModule('ti.sysbios.heaps.HeapBuf');
    var Queue = xdc.useModule('ti.sysbios.knl.Queue');
    var Program = xdc.useModule('xdc.rov.Program');

    // first get the Basic view:
    viewInitInstance(view, obj);

    // display the address of the Queue object for freeList
    view.freeList = obj.freeList.$addr;

    /*
     * *** traverse the freeList Q (linked list of) free blocks ***
     *  
     *  Count the number of elements in freeList, verify it matches
     *  'numFreeBlocks'
     */

    /* 
     * To check for loops, store each address as a key in a map,
     * then with each new address check first if it is already in
     * the map.
     */
    var addrs = {};

    var head = obj.freeList.elem; // beginning of linked list

    try {
        // e is the current element
        var e = Program.fetchStruct(Queue.Elem$fetchDesc, head.next);
    }
    catch (error) {
        view.$status["freeList"] = "Error fetching Queue Elem struct: " + error.toString();
        throw (error);
    }

    var freeListLen = 0;
    while (Number(e.$addr) != Number(head.$addr)) {
        
        // Before fetching the next element, verify we're not in a loop.
        if (e.$addr in addrs) {
            view.$status["freeList"] = "freeList Queue contains loop." +
                    "  Element " + e.$addr + " points to element " + e.next;
            break;
        }

        freeListLen++;
 
        addrs[e.$addr] = true;

        var e = Program.fetchStruct(Queue.Elem$fetchDesc, e.next);
    }

    if (freeListLen != obj.numFreeBlocks) {
        view.$status["freeList"] = "numFreeBlocks: " + obj.numFreeBlocks +
                " inconsistent with number of elements in HeapBuf.freeList: " +
                freeListLen;
    }
}
