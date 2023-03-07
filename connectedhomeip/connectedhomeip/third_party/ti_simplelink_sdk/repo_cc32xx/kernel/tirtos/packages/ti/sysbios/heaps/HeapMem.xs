/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
 *  ======== HeapMem.xs ========
 *
 */

var HeapMem = null;
var Memory = null;
var Startup = null;
var BIOS = null;
var primaryHeapNotUsed = true;
var primaryHeapInitialized = false;

/* Table to round up to power of 2 (grow if need) */
var powerOf2 = [2, 2, 2,                        /* 0-2 */
                4, 4,                           /* 3-4 */
                8, 8 ,8, 8,                     /* 5-8 */
                16, 16, 16, 16, 16, 16, 16, 16]; /* 9-16 */

/*
 *  ======== module$use ========
 *  Initialize module values.
 */
function module$use(mod, params)
{
    HeapMem = this;
    Memory = xdc.useModule('xdc.runtime.Memory');

    Startup = xdc.useModule('xdc.runtime.Startup');

    /* initialize HeapMem objects early */
    if (HeapMem.primaryHeapBaseAddr == null) {
        Startup.firstFxns.$add(HeapMem.init);
    }
    else {
        Startup.firstFxns.$add(HeapMem.initPrimary);
    }

    /*
     *  Set the HeapMem module Gate to an appropriate type.
     */
    if (HeapMem.common$.gate === undefined) {
        var BIOS = xdc.module('ti.sysbios.BIOS');
        if (BIOS.taskEnabled == true) {
            var GateMutex = xdc.useModule('ti.sysbios.gates.GateMutex');
            HeapMem.common$.gate = GateMutex.create();
        }
        else if (BIOS.swiEnabled == true) {
            var GateSwi = xdc.useModule('ti.sysbios.gates.GateSwi');
            HeapMem.common$.gate = GateSwi.create();
        }
        else {
            var GateHwi = xdc.useModule('ti.sysbios.gates.GateHwi');
            HeapMem.common$.gate = GateHwi.create();
        }
    }

    this.reqAlign = this.Header.$sizeof();

    if (Memory.getMaxDefaultTypeAlignMeta() > this.reqAlign) {
        this.reqAlign = Memory.getMaxDefaultTypeAlignMeta();
    }

    /* Make sure it is still a power of 2 for the logic to work properly */
    this.reqAlign = powerOf2[this.reqAlign];
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    var Program = xdc.module('xdc.cfg.Program');
    var target  = Program.build.target;
    var bufAlign;

    /* Don't support static Constructs */
    if (this.$category == "Struct") {
        HeapMem.$logFatal("HeapMem does not support statically" +
                          "Constructed heaps.", this);
    }

    /* Verify requested alignment is a power of 2 */
    if ((params.align != 0) &&
        ((params.align - 1) & params.align) != 0) {
        HeapMem.$logFatal("Requested alignment must be a power of 2.", this, "align");
    }

    /* Verify minimum block alignment is a power of 2 */
    if ((params.minBlockAlign != 0) &&
        ((params.minBlockAlign - 1) & params.minBlockAlign) != 0) {
        HeapMem.$logFatal("Requested minBlockAlign must be a power of 2.", this, "align");
    }

    if (params.minBlockAlign < HeapMem.reqAlign) {
        obj.minBlockAlign = HeapMem.reqAlign;
        bufAlign = HeapMem.reqAlign;
    }
    else {
        obj.minBlockAlign = params.minBlockAlign;
        bufAlign = obj.minBlockAlign;
    }

    /* Make sure the alignment is at least large enough to hold all types. */
    obj.align = params.align;
    if (obj.align < bufAlign) {
        obj.align = bufAlign;
    }

    if (params.usePrimaryHeap) {
        if (primaryHeapNotUsed) {
            primaryHeapNotUsed = false;
            if (HeapMem.primaryHeapBaseAddr != null) {
                obj.head.next = null;
                obj.head.size = 0;
                return;
            }
        }
        else {
            HeapMem.$logError("Only one primary HeapMem instance can be defined.",
                               this);
        }
    }

    /* Make sure the size is at least large enough to hold all types. */
    if (params.size < bufAlign) {
        params.size = bufAlign;
    }

    /*
     *  Make sure the size is a multiple of bufAlign. If not
     *  round down.
     */
    params.size = (Math.floor(params.size / bufAlign)) * bufAlign;
    obj.head.next = null;
    obj.head.size = params.size;

    /*
     *  If the align directive is not supported, (add align - 1) MAUs to the
     *  size of the buffer. Then during runtime find the proper alignment. This
     *  wastes some memory, but allows proper allignment on targets that do not
     *  support it.
     */
    if (target.alignDirectiveSupported == true) {
        obj.buf.length = params.size;
    }
    else {
        obj.buf.length = params.size + (obj.align - 1);
    }
    Memory.staticPlace(obj.buf, obj.align, params.sectionName);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');

    view.label = Program.getShortName(obj.$label);

    view.buf = "0x" + Number(obj.buf.$addr).toString(16);
    view.minBlockAlign = obj.minBlockAlign;
    view.sectionName = obj.sectionName;
}

/*
 *  ======== viewInitDetailed ========
 *  Initialize the 'Detailed' HeapMem instance view.
 */
function viewInitDetailed(view, obj)
{
    var HeapMem = xdc.useModule('ti.sysbios.heaps.HeapMem');

    var totalSize = 0;
    var totalFreeSize = 0;
    var largestFreeSize = 0;
    var isListCorrupt = false;
    var addrs = {};

    // first get the Basic view:
    viewInitBasic(view, obj);

    totalSize = obj.head.size;
    view.totalSize = "0x" + Number(totalSize).toString(16);

    /* get the free list head; it should always exist even if list is empty */
    var currElem;
    try {
        currElem =
                Program.fetchStruct(HeapMem.Header$fetchDesc, obj.head.$addr);
    }
    catch (e) {
        view.$status["totalFreeSize"] =
                "Error: could not access head element of free list: " + e;
        return;
    }

    /*
     * Traverse free list and add up all of the sizes.  This loop condition
     * ensures that we don't try to access a null element (i.e. it avoids trav-
     * ersing past the header on an empty list and it ends once the end of a non
     * empty list is reached).
     */
    while ((currElem.next != 0) && (!isListCorrupt)) {

        // Fetch the next free block (next linked list element)
        try {
            currElem = Program.fetchStruct(HeapMem.Header$fetchDesc,
                    currElem.next);
        }
        catch (e) {
            view.$status["totalFreeSize"] = "Corrupted free list header next ptr (0x" +
                    Number(currElem.next.$addr).toString(16) + ") " +
                    "at freeList entry 0x" + Number(currElem.$addr).toString(16);
            return;
        }

        // add up the total free space
        totalFreeSize += currElem.size;

        // check for a new max free size
        if (currElem.size > largestFreeSize) {
            largestFreeSize = currElem.size;
        }

        if (!addressValidate(Number(currElem.$addr),
            Number(currElem.next.$addr), addrs) ||
            (Number(currElem.size) < 0)) {
            view.$status["buf"] = "Error: Corrupted free list, loop or invalid size detected";
            isListCorrupt = true;
        }
    }

    view.totalFreeSize = "0x" + Number(totalFreeSize).toString(16);
    view.largestFreeSize = "0x" + Number(largestFreeSize).toString(16);

    if (totalFreeSize > totalSize) {
        view.$status["totalFreeSize"] = "Error: totalFreeSize (" +
                view.totalFreeSize + ") > totalSize (" + view.totalSize +
                ")!";
    }

    if (totalFreeSize < 0) {
        view.$status["totalFreeSize"] = "Error: totalFreeSize (" +
                view.totalFreeSize + ") is negative!";
    }

    if (largestFreeSize > totalSize) {
        view.$status["largestFreeSize"] = "Error: largestFreeSize (" +
                view.largestFreeSize + ") > totalSize (" + view.totalSize +
                ")!";
    }

    if (largestFreeSize < 0) {
        view.$status["largestFreeSize"] = "Error: largestFreeSize (" +
                view.largestFreeSize + ") is negative!";
    }
}

/*
 *  ======== viewInitData ========
 */
function viewInitData(view, obj)
{
    view.label = Program.getShortName(obj.$label);
    if (view.label == "") {

        // split the label to rid of the long full package name that precedes
        // format will be: "HeapMem@0x12345678"
        var splitStr = String(obj.$label).split(".");

        view.label = splitStr[splitStr.length - 1];
    }
    view.label += "-freeList";

    view.elements = getFreeList(obj);

    return(view);
}

/*
 *  ======== getFreeList ========
 */
function getFreeList(obj)
{
    var HeapMem = xdc.useModule('ti.sysbios.heaps.HeapMem');
    var Program = xdc.useModule('xdc.rov.Program');

    // the freeList display will now contain both free and allocated blocks

    var size;
    // Create a temporary map of the addresses discovered to use for
    // detecting loops in the linked list.
    var nextAddrs     = {};var freeList = new Array();
    var start         = obj.buf.$addr;         // buffer start addr
    var end           = start + obj.head.size;  // buffer end addr
    var header        = obj.head;
    var isListCorrupt = false;

    // get the first free list element
    if (header.next.$addr != 0) {

        /* Fetch the next block */
        try {
            header =
                    Program.fetchStruct(HeapMem.Header$fetchDesc, header.next);
        }
        catch (e) {
            print("Error: Caught exception from fetchStruct: " +
                e.toString());
            var allocatedBlock =
                Program.newViewStruct('ti.sysbios.heaps.HeapMem', 'FreeList');
            allocatedBlock.size = "0x" + Number(obj.head.size).toString(16);
            allocatedBlock.Address = "0x" + Number(start).toString(16);
            allocatedBlock.next = "0x" + Number(obj.head.next.$addr).toString(16);
            allocatedBlock.next.status = "Bad Address";
            allocatedBlock.status = "In Use";
            freeList[freeList.length] = allocatedBlock;
            freeList[freeList.length-1].$status["Address"] = "Bad next pointer = 0x" + header.next.$addr.toString(16);
            // since free list is empty, we're done
            return (freeList);
        }
    }
    else {
        // if there is no element after header, then are no free blocks (i.e.
        // the entire heap is allocated.  Update the free list to show this:
        var allocatedBlock =
                Program.newViewStruct('ti.sysbios.heaps.HeapMem', 'FreeList');
        allocatedBlock.size = "0x" + Number(obj.head.size).toString(16);
        allocatedBlock.Address = "0x" + Number(start).toString(16);
        allocatedBlock.next = "0x" + Number(obj.head.next.$addr).toString(16);
        allocatedBlock.status = "In Use";
        freeList[freeList.length] = allocatedBlock;

        // since free list is empty, we're done
        return (freeList);
    }

    /* For each block on the free list unless it is corrupted */
    while ((start != end) && (!isListCorrupt)) {

        // compute the end address of the current free block
        var freeBlkEndAddr = header.$addr + header.size;

        if (header.$addr == start) {
            // in this case, the current free block is first, possibly followed
            // by an allocated block

            /* Add the current free block to the list */
            size = Number(header.size);

            var freeBlock =
                  Program.newViewStruct('ti.sysbios.heaps.HeapMem', 'FreeList');
            freeBlock.size = "0x" + size.toString(16);
            freeBlock.Address = "0x" + Number(header.$addr).toString(16);
            freeBlock.next = "0x" + Number(header.next.$addr).toString(16);

            if (addressValidate(Number(header.$addr), Number(header.next.$addr), nextAddrs)
                && (size > 0)) {
                freeBlock.status = "Free";
            }
            else {
                freeBlock.status = "Error: Corrupted free list, loop or invalid size detected";

                // Terminate loop
                isListCorrupt = true;
            }

            freeList[freeList.length] = freeBlock;

            if (freeBlkEndAddr != end) { // only add end allocated  block if it exists
                //compute the size of this allocated block
                var allocBlkSize = (header.next.$addr == 0) ?
                        (end - freeBlkEndAddr) :
                        (header.next.$addr - freeBlkEndAddr);

                var allocatedBlock =
                  Program.newViewStruct('ti.sysbios.heaps.HeapMem', 'FreeList');
                allocatedBlock.size = "0x" + Number(allocBlkSize).toString(16);
                allocatedBlock.Address =
                        "0x" + Number(freeBlkEndAddr).toString(16);
                allocatedBlock.next =
                        "0x" + Number(freeBlkEndAddr.$addr + allocBlkSize).toString(16);

                if (addressValidate(Number(freeBlkEndAddr),
                    Number(freeBlkEndAddr + allocBlkSize), nextAddrs) &&
                    (Number(allocBlkSize) > 0)) {
                    allocatedBlock.status = "In Use";
                }
                else {
                    allocatedBlock.status = "Error: Corrupted free list, loop or invalid size detected";

                    // Terminate loop
                    isListCorrupt = true;
                }

                freeList[freeList.length] = allocatedBlock;

                // increment the start pointer to the memory address immediately
                // after the allocated block
                start = freeBlkEndAddr + allocBlkSize;
            }
            else {
                // this free block isn't followed by an allocated block.
                // increment the start pointer to the memory address
                // immediately after the free block
                start = freeBlkEndAddr;
            }

            // increment header to point to the next free block
            if (header.next.$addr != 0) {

                /* Fetch the next block */
                try {
                    header =
                     Program.fetchStruct(HeapMem.Header$fetchDesc, header.next);
                }
                catch (e) {
                    print("Error: Caught exception from fetchStruct: " +
                        e.toString());
                    freeList[freeList.length-1].$status["next"] = "Bad address";
                    start = end;
                }
            }

        }
        else {
            // in this case, the current free block is preceeded by an allocated
            // block.

            // first block is an allocated block, followed by the current
            // free block

            size =  Number(header.$addr - start);
            // first add the allocated block (start is the beginning of the
            // allocated block in this case)
            var allocatedBlock =
                  Program.newViewStruct('ti.sysbios.heaps.HeapMem', 'FreeList');
            allocatedBlock.size = "0x" + size.toString(16);
            allocatedBlock.Address = "0x" + Number(start).toString(16);
            allocatedBlock.next = "0x" + Number(header.$addr).toString(16);

            if (addressValidate(Number(start), Number(header.$addr), nextAddrs) &&
                (size > 0)) {
                allocatedBlock.status = "In Use";
            }
            else {
                allocatedBlock.status = "Error: Corrupted free list, loop or invalid size detected";

                // Terminate loop
                isListCorrupt = true;
            }

            freeList[freeList.length] = allocatedBlock;

            /* Second add the current free block to the list */
            size = Number(header.size);

            var freeBlock =
                  Program.newViewStruct('ti.sysbios.heaps.HeapMem', 'FreeList');
            freeBlock.size = "0x" + size.toString(16);
            freeBlock.Address = "0x" + Number(header.$addr).toString(16);
            freeBlock.next = "0x" + Number(header.next.$addr).toString(16);

            if (addressValidate(Number(header.$addr), Number(header.next.$addr), nextAddrs) &&
                (size > 0)) {
                freeBlock.status = "Free";
            }
            else {
                freeBlock.status = "Error: Corrupted free list, loop or invalid size detected";

                // Terminate loop
                isListCorrupt = true;
            }

            freeList[freeList.length] = freeBlock;

            // increment the start pointer to the memory address immediately
            // after the free block
            start = freeBlkEndAddr;

            // increment header to the next free block
            if (header.next.$addr != 0) {
                /* Fetch the next block */
                try {
                    header =
                     Program.fetchStruct(HeapMem.Header$fetchDesc, header.next);
                }
                catch (e) {
                    print("Error: Caught exception from fetchStruct: " +
                        e.toString());
                    freeList[freeList.length-1].$status["next"] = "Bad address";
                    start = end;
                }
            }
            else { // if next elem is null, then this was the last free block

                if (freeBlkEndAddr < end) {
                    // this was the last free block, but there's an allocated
                    // block after it

                    size = Number(end - freeBlkEndAddr);

                    allocatedBlock = Program.newViewStruct(
                            'ti.sysbios.heaps.HeapMem', 'FreeList');
                    allocatedBlock.size = "0x" + size.toString(16);

                    allocatedBlock.Address = "0x" +
                            Number(freeBlkEndAddr).toString(16);
                    allocatedBlock.next = "0x" + Number(end).toString(16);

                    if (addressValidate(Number(freeBlkEndAddr), Number(end), nextAddrs) &&
                        (size > 0)) {
                        allocatedBlock.status = "In Use";
                    }
                    else {
                        allocatedBlock.status = "Error: Corrupted free list, loop or invalid size detected";
                    }

                    freeList[freeList.length] = allocatedBlock;

                    // terminate loop
                    start = end;
                }
                // else do nothing - freeBlkEndAddr == end, so there was
                // nothing following this free block
            }
        }
    } // end while (start != end)

    return (freeList);
}

/*
 * This is a helper function which validates that there are no loops in the
 * list.
 */
function addressValidate(baseAddr, nextAddr, knownAddrs) {
        knownAddrs[baseAddr] = true;

    // Check for loops in know address list, ignoring null pointers
    if ((nextAddr != 0) && (nextAddr in knownAddrs)) {
        return (false);
    }
    // Add now validated addresses
    return (true);
}