/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== HeapTrack.xs ========
 *
 */

/*
 *  ======== HeapTrack.Module.init ========
 */
var HeapTrack;
var Queue;
var Task;
var Memory;
var System;
var Error;
var IHeap;

/*
 *  ======== module$use ========
 */
function module$use()
{
    HeapTrack = this;
    Queue  = xdc.useModule('ti.sysbios.knl.Queue');
    Task   = xdc.useModule('ti.sysbios.knl.Task');
    Memory = xdc.useModule('xdc.runtime.Memory');
    System = xdc.useModule('xdc.runtime.System');
    Error  = xdc.useModule('xdc.runtime.Error');
    IHeap  = xdc.useModule('xdc.runtime.IHeap');
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
}

/*
 *  ======== HeapTrack.Instance.init ========
 */
function instance$static$init(obj, params)
{
    Queue.construct(obj.trackQueue);
    obj.internalHeap = params.heap;
    obj.size = 0;
    obj.peak = 0;
    obj.sizeWithoutTracker = 0;
    obj.peakWithoutTracker = 0;
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' HeapTrack instance view.
 */
function viewInitBasic(view, obj)
{
    var HeapTrack = xdc.useModule('ti.sysbios.heaps.HeapTrack');
    var Program = xdc.useModule('xdc.rov.Program');

    view.heapHandle = obj.internalHeap;
    view.inUse = "0x" + Number(obj.size).toString(16);
    view.inUsePeak = "0x" + Number(obj.peak).toString(16);
    view.inUseWithoutTracker = "0x" + Number(obj.sizeWithoutTracker).toString(16);
    view.inUsePeakWithoutTracker = "0x" + Number(obj.peakWithoutTracker).toString(16);
}

/*
 *  ======== viewInitTask ========
 *  Initialize the 'Task' HeapTrack instance view.
 */
function viewInitTask()
{
    var index;
    var Program = xdc.useModule('xdc.rov.Program');
    HeapTrack = xdc.useModule('ti.sysbios.heaps.HeapTrack');
    Queue = xdc.useModule('ti.sysbios.knl.Queue');

    /* Get HeapTrack and Task module description */
    var modHeapTrack = Program.scanRawView('ti.sysbios.heaps.HeapTrack');
    var modTask = Program.scanRawView('ti.sysbios.knl.Task');

    /* Array used to index into root children table */
    var tasks = new Array();

    /* Top level TreeNode */
    var root = new Program.TreeNode();
    root.label = "Task List";
    root.children = new Array();

    /* Add boot task to array and root node */
    tasks[tasks.length] = 0;
    var bootNode = new Program.TreeNode();
    bootNode.label = "Boot";
    bootNode.properties = new Array();
    root.children[root.children.length++] = bootNode;

    /* Add orphan task to array and root node */
    var orphanIndex = -1;
    tasks[tasks.length] = orphanIndex;
    var orphanNode = new Program.TreeNode();
    orphanNode.label = "Orphan";
    orphanNode.properties = new Array();
    root.children[root.children.length++] = orphanNode;

    for (index in modTask.instStates) {
        var taskAddr = Number(modTask.instStates[index].$addr);
        var taskLabel = Program.getShortName(modTask.instStates[index].$label);

        /* Add task to array */
        tasks[tasks.length] = taskAddr;

        /* Create a new TreeNode */
        var taskNode = new Program.TreeNode();
        if (taskLabel.length == 0) {
            taskNode.label = "0x" + Number(taskAddr).toString(16);
        }
        else {
            taskNode.label = taskLabel;
        }
        taskNode.properties = new Array();

        /* Save TreeNode in root tree */
        root.children[root.children.length++] = taskNode;
    }

    for (index in modHeapTrack.instStates) {
        /* Fetch the queue structure */
        var queElem = modHeapTrack.instStates[index].trackQueue.elem;
        var start = queElem.$addr;
        var continueFlag = true;

        /* Loop while through the list but stop if a corrupted scribble found */
        while ((Number(queElem.next) != start) && (continueFlag == true)) {
            var heapView = Program.newViewStruct('ti.sysbios.heaps.HeapTrack',
                                                               'TaskAllocList');
            var tracker = Program.fetchStruct(HeapTrack.Tracker$fetchDesc,
                     queElem.next - Program.build.target.stdTypes.t_Int32.size);

            /* Scribble was word aligned, reverse our logic */
            var rem = 0;
            if (tracker.size % Program.build.target.stdTypes.t_Int32.size) {
                rem = Program.build.target.stdTypes.t_Int32.size -
                    (tracker.size % Program.build.target.stdTypes.t_Int32.size);
            }

            heapView.heapHandle = "0x" +
               Number(modHeapTrack.instStates[index].internalHeap).toString(16);
            heapView.blockAddr = "0x" +
                        Number(tracker.$addr - tracker.size - rem).toString(16);
            heapView.requestedSize = "0x" + Number(tracker.size).toString(16);
            heapView.clockTick = Number(tracker.tick).toString();

            /* Check scribble */
            if (tracker.scribble == HeapTrack.STARTSCRIBBLE) {
                heapView.overflow = "NO";
                queElem = tracker.queElem;
            }
            else {
                heapView.overflow = "YES";
                continueFlag = false;
                Program.displayError(heapView, "overflow",
                                                      "Error: Memory overflow");
            }

            var taskAddr = Number(tracker.taskHandle);

            /* If the task for this HeapTrack is not found it is an orphan */
            if (tasks.indexOf(taskAddr) == -1) {
                taskAddr = orphanIndex;
            }

            /* Get the array of heapViews from the TreeNode properties */
            var heapViews = root.children[tasks.indexOf(taskAddr)].properties;
            /* Set the block number and save the individual heapView */
            heapView.block = (heapViews.length + 1).toString();
            heapViews[heapViews.length++] = heapView;
        }
    }

    root.children[root.children.length++] = root.children[tasks.indexOf(orphanIndex)];
    for (var i = tasks.indexOf(orphanIndex);
            i < root.children.length - 1; i++) {
        root.children[i] = root.children[i + 1];
    }
    root.children.length--;

    return ([root]);
}

/*
 *  ======== viewInitHeap ========
 *  Initialize the 'Heap' HeapTrack instance view.
 */
function viewInitHeapList(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    HeapTrack = xdc.useModule('ti.sysbios.heaps.HeapTrack');
    Queue = xdc.useModule('ti.sysbios.knl.Queue');

    var listViews = new Array();
    view.label = Program.getShortName(obj.$label);

    if (view.label == "") {

        // split the label to rid of the long full package name that precedes
        // format will be: "HeapTrack@0x12345678"
        var splitStr = String(obj.$label).split(".");

        view.label = splitStr[splitStr.length - 1];
    }

    // Fetch the queue structure
    var queElem = obj.trackQueue.elem;
    var start = queElem.$addr;
    var block = 1;
    var continueFlag = true;

    /* Loop while through the list but stop if a corrupted scribble found */
    while((Number(queElem.next) != start) && (continueFlag == true)) {
        var listView = Program.newViewStruct('ti.sysbios.heaps.HeapTrack',
                                                               'HeapAllocList');
        var tracker = Program.fetchStruct(HeapTrack.Tracker$fetchDesc,
                     queElem.next - Program.build.target.stdTypes.t_Int32.size);

        listView.block = block.toString();
        if (tracker.taskHandle == 0) {
            listView.taskHandle = "Boot";
        }
        else {
            listView.taskHandle = "0x" +
                                        Number(tracker.taskHandle).toString(16);
        }

        /* Scribble was word aligned, reverse our logic */
        var rem = 0;
        if (tracker.size % Program.build.target.stdTypes.t_Int32.size) {
            rem = Program.build.target.stdTypes.t_Int32.size - (tracker.size %
                                    Program.build.target.stdTypes.t_Int32.size);
        }

        listView.heapHandle = "0x" + Number(obj.internalHeap).toString(16);
        listView.blockAddr = "0x" +
                        Number(tracker.$addr - tracker.size - rem).toString(16);
        listView.requestedSize = "0x" + Number(tracker.size).toString(16);
        listView.clockTick = Number(tracker.tick).toString();
        /* Check scribble */
        if (tracker.scribble == HeapTrack.STARTSCRIBBLE) {
            listView.overflow = "NO";
            queElem = tracker.queElem;
            block++;
        }
        else {
            listView.overflow = "YES";
            continueFlag = false;
            Program.displayError(listView, "overflow","Error: Memory overflow");
        }

        listViews[listViews.length] = listView;
    }

    view.elements = listViews;
}
