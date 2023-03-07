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
 *  ======== HeapCallback.rov.js ========
 */

var moduleName = "ti.sysbios.heaps::HeapCallback";

var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewHeapCallbackBasic", structName: "BasicInfo"}
];

/*
 * HeapCallback Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol  = null;
    this.context = null;
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
 *  ======== viewGetHeapCallbackObjectArray ========
 *  returns an array of the current HeapCallback objects.
 */
function viewGetHeapCallbackObjectArray()
{
    heapArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_heaps_HeapCallback_Module_state");
    }
    catch (e) {
        return (heapArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_heaps_HeapCallback_Module_state");
    var heapStructType = Program.lookupType("ti_sysbios_heaps_HeapCallback_Struct");
    var offsetOfObjElem = heapStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var heapAddr;

    while (Number(next) != Number(modStateAddr)) {
        heapAddr = next - offsetOfObjElem;
        var heap = new Object();
        heap = Program.fetchFromAddr(heapAddr, "ti_sysbios_heaps_HeapCallback_Struct", 1);
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
    view.address = toHexString(obj.$addr);
    view.context = toPaddedHexString(Number(obj.context), 8);
    view.symbol  = String(Program.lookupDataSymbol(Number(obj.$addr)));

    /*
     *  In case the dynamically created object was the first thing in the
     *  BIOS_heap.
     */
    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }
}

/*
 *  ======== viewHeapCallbackBasic ========
 */
function viewHeapCallbackBasic()
{
    view = new Array();

    var heaps = viewGetHeapCallbackObjectArray();

    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}
