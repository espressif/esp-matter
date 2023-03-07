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
 *  ======== HeapTrack.rov.js ========
 */

var moduleName = "ti.sysbios.heaps::HeapTrack";

var Task = xdc.loadCapsule("ti/sysbios/knl/Task.rov.js");
var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewHeapTrackBasic", structName: "BasicInfo"},
    {name: "HeapAllocList", fxn: "viewHeapList", structName: "AllocInfo",
        viewType: "Pages"},
    {name: "TaskAllocList", fxn: "viewTaskList", structName: "AllocInfo",
        viewType: "Pages"}
];

/*
 * HeapTrack Object Basic Information
 */
function BasicInfo()
{
    this.address               = null;
    this.heapHandle            = null;
    this.inUse                 = null;
    this.inUsePeak             = null;
    this.inUseWithoutTrack     = null;
    this.inUsePeakWithoutTrack = null;
}

/*
 * Allocated blocks
 */
function AllocInfo()
{
    this.block         = null;
    this.taskHandle    = null;
    this.heapHandle    = null;
    this.blockAddr     = null;
    this.requestedSize = null;
    this.clockTick     = null;
    this.overflow      = null;
}

/*
 * ======== findTask ========
 *  Search all the task handles in the system to see if the handle in the
 *  Tracker strucutre matches any one them. If the handle is null, then
 *  the block was allocated during the boot/main. If the handle is not
 *  found, the block was allocated from a task that is no longer around.
 *  We call this an Orphan. It might not be a problem, since the application
 *  could have allocate everything in a task that it then deletes.
 */
function findTask(tasks, handle) {
    var returnStr;

    if (handle == 0) {
        returnStr = "Boot";
    }
    else {

        /* Return "Orphan" if no matching handle in the tasks is found */
        returnStr = "Orphan";
        for (var i = 0; i < tasks.length; i++) {
            if (handle == tasks[i].$addr) {
                returnStr = toHexString(handle);
            }
        }
    }

    return (returnStr);
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
 *  ======== viewGetHeapTrackObjectArray ========
 *  returns an array of the current HeapTrack objects.
 */
function viewGetHeapTrackObjectArray()
{
    heapArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_heaps_HeapTrack_Module_state");
    }
    catch (e) {
        return (heapArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_heaps_HeapTrack_Module_state");
    var heapStructType = Program.lookupType("ti_sysbios_heaps_HeapTrack_Struct");
    var offsetOfObjElem = heapStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var heapAddr;

    while (Number(next) != Number(modStateAddr)) {
        heapAddr = next - offsetOfObjElem;
        var heap = new Object();
        heap = Program.fetchFromAddr(heapAddr, "ti_sysbios_heaps_HeapTrack_Struct", 1);
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
    view.heapHandle = obj.internalHeap;
    view.inUse = Number(obj.size);
    view.inUsePeak = Number(obj.peak);
    view.inUseWithoutTrack = Number(obj.sizeWithoutTracker);
    view.inUsePeakWithoutTrack = Number(obj.peakWithoutTracker);
}

/*
 *  ======== viewHeapTrackBasic ========
 */
function viewHeapTrackBasic()
{
    view = new Array();

    var heaps = viewGetHeapTrackObjectArray();

    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 *  ======== viewHeapList ========
 */
function viewHeapList()
{
    var view = [];
    var heaps = viewGetHeapTrackObjectArray();
    var index = 0;
    var isListCorrupt = false;
    var activeTasks = Task.viewGetTaskObjectArray();

    /* Loop through all the HeapTrack instances */
    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];

        var labelName = "HeapTrack@" + toHexString(obj.$addr);
        view[index] = {label: labelName, elements: []};

        var elem = 0;

        var typespec = Program.lookupType("ti_sysbios_heaps_HeapTrack_Struct");
        var offset = typespec.member["trackQueue"].offset;
        var head = obj.$addr + offset
        var next = obj.trackQueue.next;

        /* Loop through the blocks on the trackQueue linked list */
        while (next != head) {
            var trackAddr = next - 4; // todo sizeof UArg
            var tracker = new Object();
            tracker = Program.fetchFromAddr(trackAddr, "ti_sysbios_heaps_HeapTrack_Tracker", 1);

            view[index].elements[elem] = new AllocInfo();
            view[index].elements[elem].block = elem + 1;
            view[index].elements[elem].taskHandle =
                findTask(activeTasks, tracker.taskHandle);
            view[index].elements[elem].heapHandle = obj.internalHeap;
            view[index].elements[elem].blockAddr =
                toHexString(trackAddr - tracker.size); //todo add remainder logic
            view[index].elements[elem].requestedSize = tracker.size;
            view[index].elements[elem].clockTick = tracker.tick;
            if (tracker.scribble == 0xa5a5a5a5 ) {
                view[index].elements[elem].overflow = "no";
            }
            else {
                view[index].elements[elem].overflow = "YES";
                Program.displayError(view[index].elements[elem], "overflow",
                                     "Error: Memory overflow");
                break;
            }

            next = tracker.queElem.next; // todo validate
            elem++
        }
        index++;
    }

    return (view);
}

/*
 *  ======== viewTaskList ========
 */
function viewTaskList()
{
    var view = [];
    var heaps = viewGetHeapTrackObjectArray();
    var activeTasks = Task.viewGetTaskObjectArray();
    var index = 0;
    var isListCorrupt = false;
    var taskString;
    var displayNames = ["Boot"];

    /*
     *  Loop through all the HeapTrack instances to find all the task that
     *  have allocated memory.
     */
    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];

        var typespec = Program.lookupType("ti_sysbios_heaps_HeapTrack_Struct");
        var offset = typespec.member["trackQueue"].offset;
        var head = obj.$addr + offset
        var next = obj.trackQueue.next;

        /* Loop through the blocks on the trackQueue linked list */
        while (next != head) {
            var trackAddr = next - 4; // todo sizeof UArg
            var tracker = new Object();
            tracker = Program.fetchFromAddr(trackAddr, "ti_sysbios_heaps_HeapTrack_Tracker", 1);
            for (j = 0; j < activeTasks.length; j++) {
                if (tracker.taskHandle == activeTasks[j].$addr) {
                    taskString = toHexString(tracker.taskHandle);
                    if ((displayNames.indexOf(taskString)) == -1) {
                        displayNames.push(taskString);
                    }
                }
            }

            next = tracker.queElem.next; // todo validate
        }
        index++;
    }
    displayNames.push("Orphan");

    var elem = [];
    for (i = 0; i < displayNames.length; i++) {
        elem[i] = 0;
        var labelName = "Task@" + displayNames[i];
        view[i] = {label: labelName, elements: []};
    }

    for (var i = 0; i < heaps.length; i++) {
        var obj = heaps[i];

        var typespec = Program.lookupType("ti_sysbios_heaps_HeapTrack_Struct");
        var offset = typespec.member["trackQueue"].offset;
        var head = obj.$addr + offset
        var next = obj.trackQueue.next;

        /* Loop through the blocks on the trackQueue linked list */
        while (next != head) {
            var trackAddr = next - 4; // todo sizeof UArg
            var tracker = new Object();
            tracker = Program.fetchFromAddr(trackAddr, "ti_sysbios_heaps_HeapTrack_Tracker", 1);
            taskString = findTask(activeTasks, tracker.taskHandle)
            index = displayNames.indexOf(taskString);

            view[index].elements[elem[index]] = new AllocInfo();
            view[index].elements[elem[index]].block = elem[index] + 1;
            view[index].elements[elem[index]].taskHandle = taskString;
            view[index].elements[elem[index]].heapHandle = obj.internalHeap;
            view[index].elements[elem[index]].blockAddr =
                toHexString(trackAddr - tracker.size); //todo add remainder logic
            view[index].elements[elem[index]].requestedSize = tracker.size;
            view[index].elements[elem[index]].clockTick = tracker.tick;
            if (tracker.scribble == 0xa5a5a5a5 ) {
                view[index].elements[elem[index]].overflow = "no";
            }
            else {
                view[index].elements[elem[index]].overflow = "YES";
                Program.displayError(view[index].elements[elem[index]], "overflow",
                                     "Error: Memory overflow");
                break;
            }

            next = tracker.queElem.next; // todo validate
            elem[index]++;
        }
    }

    return (view);
}
