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
 *  ======== HeapMem.rov.js ========
 */

var moduleName = "ti.sysbios.heaps::HeapMem";

var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewHeapMemBasic", structName: "BasicInfo"},
    {name: "Detailed", fxn: "viewHeapMemDetailed", structName: "DetailedInfo"},
    {name: "FreeList", fxn: "viewFreeList", structName: "FreeListInfo", viewType: "Pages"}
];

/*
 * HeapMem Object Basic Information
 */
function BasicInfo()
{
    this.address       = null;
    this.symbol        = null;
    this.buf           = null;
    this.minBlockAlign = null;
}

/*
 * HeapMem Object Detailed Information
 */
function DetailedInfo()
{
    this.address         = null;
    this.symbol          = null;
    this.buf             = null;
    this.minBlockAlign   = null;
    this.totalSize       = null;
    this.totalFreeSize   = null;
    this.largestFreeSize = null;
}

/*
 * HeapMem Object Detailed Information
 */
function FreeListInfo()
{
    this.address    = null;
    this.size       = null;
    this.next       = null;
    this.status     = null;
}

/*
 * This is a helper function which
 * - validates that there are no loops in the list.
 * - the addr is in the Heap's buffer.
 */
function addressValidate(addr, base, size, knownAddrs) {

    // Check for loops in know address list, ignoring null pointers
    if ((addr != 0) && (addr in knownAddrs)) {
        return (false);
    }

    // Add now validated addresses
    knownAddrs[addr] = true;

    // Make sure the addr is in the buffer
    if ((addr < base) || (addr > (base + size))) {
        return (false);
    }

    return (true);
}

/*
 * ======== toHexString ========
 * converts a number to a hex string with "0x" prefix.
 */
function toHexString(num) {
    return ("0x" + Number(num).toString(16));
}

/*
 * ======== toPaddedHexString ========
 * converts a number to a hex string with "0x" prefix.
 * inserts up to (len - 1) leading zeros.
 * up to a max of 7 zeros.
 */
function toPaddedHexString(number, len)
{
    return ("0x" + ("0000000" + number.toString(16)).substr(-len));
}

/*
 *  ======== viewGetHeapMemObjectArray ========
 *  returns an array of the current HeapMem objects.
 */
function viewGetHeapMemObjectArray()
{
    heapArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_heaps_HeapMem_Module_state");
    }
    catch (e) {
        return (heapArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_heaps_HeapMem_Module_state");
    var heapStructType = Program.lookupType("ti_sysbios_heaps_HeapMem_Struct");
    var offsetOfObjElem = heapStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var heapAddr;

    while (Number(next) != Number(modStateAddr)) {
        heapAddr = next - offsetOfObjElem;
        var heap = new Object();
        heap = Program.fetchFromAddr(heapAddr, "ti_sysbios_heaps_HeapMem_Struct", 1);
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
    view.buf           = obj.buf;
    view.minBlockAlign = obj.minBlockAlign;
    view.symbol        = String(Program.lookupDataSymbol(Number(obj.$addr)));
    /*
     *  In case the dynamically created object was the first thing in the
     *  BIOS_heap.
     */
    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }
}

/*
 *  ======== viewHeapMemBasic ========
 */
function viewHeapMemBasic()
{
    view = new Array();

    var heaps = viewGetHeapMemObjectArray();

    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 *  ======== viewHeapMemDetailed ========
 */
function viewHeapMemDetailed()
{
    var view = new Array();
    var knownAddrs = {};
    var heaps = viewGetHeapMemObjectArray();

    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];
        var detailedView = new DetailedInfo();
        viewInitBasic(detailedView, obj);

        detailedView.totalSize = obj.head.size;

        if (obj.head.next == 0) {
            detailedView.totalFreeSize = 0;
            detailedView.largestFreeSize = 0;
        }
        else {
            if (addressValidate(obj.head.next, obj.buf, obj.head.size,
                                knownAddrs) == false) {
                Program.displayError(detailedView, "totalFreeSize",
                                     "Error: Corrupted free list, loop " +
                                     "or invalid size detected");
                view.push(detailedView);
                continue;
            }
            var curElem = Program.fetchFromAddr(Number(obj.head.next),
                                                "ti_sysbios_heaps_HeapMem_Header", 1);

            while (Number(curElem) != 0) {
                detailedView.totalFreeSize += curElem.size;
                if (curElem.size > detailedView.largestFreeSize) {
                    detailedView.largestFreeSize = curElem.size;
                }

                if (curElem.next == 0) {
                    curElem = 0;
                }
                else {
                    if (addressValidate(curElem.next, obj.buf, obj.head.size,
                                        knownAddrs) == false) {
                        Program.displayError(detailedView, "totalFreeSize",
                                     "Error: Corrupted free list, loop " +
                                     "or invalid size detected");
                        break;
                    }
                    curElem = Program.fetchFromAddr(Number(curElem.next),
                                                    "ti_sysbios_heaps_HeapMem_Header", 1);
                }
            }
        }

        if (detailedView.totalFreeSize > detailedView.totalSize) {
            Program.displayError(detailedView, "totalFreeSize",
                "Error: totalFreeSize (" +
                detailedView.totalFreeSize + ") > totalSize (" +
                detailedView.totalSize + ")!");
        }

        if (detailedView.totalFreeSize < 0) {
            Program.displayError(detailedView, "totalFreeSize",
                "Error: totalFreeSize (" +
                detailedView.totalFreeSize + ") is negative!");
        }

        if (detailedView.totalFreeSize > detailedView.totalSize) {
            Program.displayError(detailedView, "largestFreeSize",
                "Error: largestFreeSize (" +
                detailedView.largestFreeSize + ") > totalSize (" +
                detailedView.totalSize + ")!");
        }

        if (detailedView.largestFreeSize < 0) {
            Program.displayError(detailedView, "largestFreeSize",
                "Error: largestFreeSize (" +
                detailedView.largestFreeSize + ") is negative!");
        }

        if (detailedView.largestFreeSize > detailedView.totalFreeSize) {
            Program.displayError(detailedView, "largestFreeSize",
                "Error: largestFreeSize (" +
                detailedView.largestFreeSize + ") > totalSize (" +
                detailedView.totalFreeSize + ")!");
        }

        view.push(detailedView);
    }

    return (view);
}

/*
 *  ======== viewFreeList ========
 */
function viewFreeList()
{
    var view = [];
    var knownAddrs = {};
    var heaps = viewGetHeapMemObjectArray();
    var index = 0;
    var isListCorrupt = false;

    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];

        var labelName = "Heap@" + toHexString(obj.$addr);
        view[index] = {label: labelName, elements: []};
        var elem = 0;
        if (obj.head.next == 0) {
            view[index].elements[elem] = new FreeListInfo();
            view[index].elements[elem].address = toHexString(obj.buf);
            view[index].elements[elem].size = obj.head.size;
            view[index].elements[elem].next = "N/A";
            view[index].elements[elem].status = "In Use";
        }
        else {
            var curElem = Program.fetchFromAddr(Number(obj.head.next),
                                                "ti_sysbios_heaps_HeapMem_Header", 1);

            var prevAddr = obj.buf;
            var currAddr = obj.head.next;
            var isCorrupted = false;
            while (Number(curElem) != 0) {

                if (addressValidate(currAddr, obj.buf, obj.head.size,
                                    knownAddrs) == false) {
                    view[index].elements[elem] = new FreeListInfo();
                    view[index].elements[elem].address = "Error: Corrupted free \
                                        list, loop or invalid size detected";
                    isCorrupted = true;
                    break;
                }

                // determine if preceding memory is in use
                if (prevAddr < currAddr) {
                    view[index].elements[elem] = new FreeListInfo();
                    view[index].elements[elem].address = toHexString(prevAddr);
                    view[index].elements[elem].size = currAddr - prevAddr;
                    view[index].elements[elem].next = "";
                    view[index].elements[elem].status = "In Use";
                    elem++;
                }

                view[index].elements[elem] = new FreeListInfo();
                view[index].elements[elem].address = toHexString(currAddr);
                view[index].elements[elem].size = curElem.size;
                view[index].elements[elem].next = toHexString(curElem.next);
                view[index].elements[elem].status = "Free";

                prevAddr = currAddr + curElem.size;
                currAddr = curElem.next;
                if (curElem.next == 0) {
                   curElem = 0;
                }
                else {
                    curElem = Program.fetchFromAddr(Number(curElem.next),
                                                    "ti_sysbios_heaps_HeapMem_Header", 1);
                }
                elem++;
            }

            /* Check to see if the end of the buf is allocated */
            if ((prevAddr < obj.buf + obj.head.size) && (isCorrupted == false)){
                view[index].elements[elem] = new FreeListInfo();
                view[index].elements[elem].address = toHexString(prevAddr);
                view[index].elements[elem].size = obj.buf + obj.head.size - prevAddr;
                view[index].elements[elem].next = "";
                view[index].elements[elem].status = "In Use";
            }
        }
        index++;
    }

    return (view);
}
