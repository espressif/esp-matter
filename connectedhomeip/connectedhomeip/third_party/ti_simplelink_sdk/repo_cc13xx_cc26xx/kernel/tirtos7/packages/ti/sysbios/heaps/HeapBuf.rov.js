/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 *  ======== HeapBuf.rov.js ========
 */

var moduleName = "ti.sysbios.heaps::HeapBuf";

var viewMap = [
    {name: "Basic", fxn: "viewHeapBufBasic", structName: "BasicInfo"},
    {name: "Detailed", fxn: "viewHeapBufDetailed", structName: "DetailedInfo"}
];

/*
 * HeapBuf Object Basic Information
 */
function BasicInfo()
{
    this.address            = null;
    this.symbol             = null;
    this.bufSize            = null;
    this.blockSize          = null;
    this.numBlocks          = null;
    this.totalFreeSize      = null;
    this.numAllocatedBlocks = null;
    this.numFreeBlocks      = null;
    this.maxAllocatedBlocks = null;
    this.buf                = null;
}

/*
 * HeapBuf Object Detailed Information
 */
function DetailedInfo()
{
    this.address = null;
    this.symbol = null;
    this.bufSize = null;
    this.blockSize = null;
    this.numBlocks = null;
    this.totalFreeSize = null;
    this.numAllocatedBlocks = null;
    this.numFreeBlocks = null;
    this.maxAllocatedBlocks = null;
    this.buf = null;
    this.freeList = null;
}

/*
 * ======== toHexString ========
 * Converts a number to a hex string with "0x" prefix.
 */
function toHexString(num) {
    return ("0x" + Number(num).toString(16));
}

/*
 * ======== validateFreeList ========
 * Ensures number of blocks in HeapBuf.freeList matches the number stored in
 * HeapBuf.numFreeBlocks, and that there are no loops within the list.
 */
function validateFreeList(heapObj, numFreeBlocks)
{
    var typespec = Program.lookupType("ti_sysbios_heaps_HeapBuf_Struct");
    var offset = typespec.member["freeList"].offset;
    var head = heapObj.$addr + offset
    var next = heapObj.freeList.next;
    var count = 0;
    var listIsValid = false;
    var knownAddrs = {};
    var base = heapObj.buf;
    var size = heapObj.bufSize;

    if ((next < base) || (next > (base + size))) {
        return(false);
    }

    knownAddrs[head] = true;

    while (next != head) {
        if (next in knownAddrs) {
            break;
        }
        if ((next < base) || (next > (base + size))) {
            break;
        }
        knownAddrs[next] = true;
        var elem = Program.fetchFromAddr(Number(next), "Queue_Elem", 1);
        next = elem.next;
        count++;
    }

    if (count == numFreeBlocks) {
        listIsValid = true;
    }

    return (listIsValid);
}

/*
 * ======== viewGetHeapBufObjectArray ========
 * Gathers the data from the target.
 */
function viewGetHeapBufObjectArray()
{
    heapArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_heaps_HeapBuf_Module_state");
    }
    catch (e) {
        return (heapArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_heaps_HeapBuf_Module_state");
    var heapStructType = Program.lookupType("ti_sysbios_heaps_HeapBuf_Struct");
    var offsetOfObjElem = heapStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var heapAddr;

    while (Number(next) != Number(modStateAddr)) {
        heapAddr = next - offsetOfObjElem;
        var heap = new Object();
        heap = Program.fetchFromAddr(heapAddr, "ti_sysbios_heaps_HeapBuf_Struct", 1);
        heap.$addr = heapAddr;
        heapArray.push(heap);
        next = heap.objElem.next;
    }

    return (heapArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    view.address       = toHexString(obj.$addr);
    view.bufSize       = obj.bufSize;
    view.blockSize     = obj.blockSize;
    view.numBlocks     = obj.numBlocks;
    view.totalFreeSize = obj.numFreeBlocks * obj.blockSize;

    if (obj.numFreeBlocks > obj.numBlocks) {
        Program.displayError(view, "numFreeBlocks", "Error: number of free " +
                             "blocks is greater than the total number of " +
                             " blocks (possibly caused by an invalid free)");
    }

    view.numAllocatedBlocks = obj.numBlocks - obj.numFreeBlocks;
    view.numFreeBlocks      = obj.numFreeBlocks;

    if (view.numAllocatedBlocks < 0) {
        Program.displayError(view, "numAllocatedBlocks",
                             "Error: numAllocatedBlocks < 0! " +
                             "(possibly caused by an invalid free)");
    }

    // Assumes 32-bit ints
    if (Number(obj.minFreeBlocks) == 0xFFFFFFFF) {
        view.maxAllocatedBlocks = "Not enabled";
    } else {
        view.maxAllocatedBlocks = obj.numBlocks - obj.minFreeBlocks;
        if (view.maxAllocatedBlocks < 0) {
            Program.displayError(view, "maxAllocatedBlocks",
                             "Error: maxAllocatedBlocks < 0!");
        }
    }

    view.buf    = obj.buf;
    view.symbol = String(Program.lookupDataSymbol(Number(obj.$addr)));

    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }
}

/*
 * ======== viewHeapBufBasic ========
 * Arranges data gathered from the target into structs intended for this
 * module.
 */
function viewHeapBufBasic()
{
    view = new Array();

    var heaps = viewGetHeapBufObjectArray();
    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 * ======== viewHeapBufDetailed ========
 * Arranges data gathered from the target into structs representing this
 * module.
 */
function viewHeapBufDetailed()
{
    view = new Array();

    var heaps = viewGetHeapBufObjectArray();
    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];
        var detailedView = new DetailedInfo();

        viewInitBasic(detailedView, obj);

        if (validateFreeList(obj, Number(obj.numFreeBlocks)) == true) {
            detailedView.freeList = obj.freeList.next;

        }
        else {
            Program.displayError(detailedView, "freeList",
                          "Error: Corrupted free list! " +
                          "numFreeBlocks is out of sync or a loop is present");
        }
        view.push(detailedView);
    }

    return (view);
}

