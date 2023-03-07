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

/* global xdc */
var Program = xdc.module('xdc.rov.Program');
var Monitor = xdc.module("xdc.rov.runtime.Monitor");

/* eslint-disable-next-line no-unused-vars */
var moduleName = "FreeRTOS";

/* eslint-disable-next-line no-unused-vars */
var viewMap = [
    {name: "ISR Stack", fxn: "getSystemStack", structName: "SystemStack"},
    {name: "Heap Stats", fxn: "getHeap", structName: "Heap"},
    {name: "Task Module", fxn: "getTaskModule", structName: "TaskModule"},
    {name: "Task Instances", fxn: "getTaskInstances", structName: "TaskInstance"}
];

function SystemStack()
{
    this.Address   = null;
    this.StackSize = null;
    this.StackPeak = null;
}

function Heap()
{
    this.HeapType       = null;
    this.HeapAddr       = null;
    this.TotalSize      = null;
    this.TotalFree      = null;
    this.FirstFreeBlock = null;
}

function TaskModule()
{
    this.NumPriorities   = null;
    this.NumTasks        = null;
}

function TaskInstance()
{
    this.Address         = null;
    this.TaskName        = null;
    //this.FxnName         = null;
    this.Priority        = null;
    this.BasePriority    = null;
    this.State           = null;
    this.StackBase       = null;
    this.UnusedStackSize = null;
    this.CurrentTaskSP   = null;
}

/*
 * ======== getSystemStack ========
 */
/* eslint-disable-next-line no-unused-vars */
function getSystemStack()
{
    var view = new Array();

    var stackInfo = new SystemStack();

    /* __STACK_END is not present in the GCC compiler...just TI's */
    var ccsCompiler = Program.lookupSymbolValue("__STACK_END");

    stackInfo.Address   = "0x" + Program.lookupSymbolValue("__stack").toString(16);
    if (ccsCompiler == -1) {
        stackInfo.StackSize = Program.lookupSymbolValue("STACKSIZE");
    }
    else {
        stackInfo.StackSize = Program.lookupSymbolValue("__STACK_SIZE");
    }

    var stackData = Program.fetchArray(
        {
            type: 'xdc.rov.support.ScalarStructs.S_UChar',
            isScalar: true
        }, stackInfo.Address, stackInfo.StackSize);

    var index = 0;
    /*
     * Find the first non-0xa5.
     */
    while (stackData[index] == 0xa5) {
        index++;
    }
    if (index < 4) {
        /*
         *  Might want to add a symbol to figure out that stack init was
         *  not initialized.
         */
        stackInfo.StackPeak = "Stack Overflow \
           (or configENABLE_ISR_STACK_INIT is 0 in FreeRTOSConfig.h)"
    }
    else {
        stackInfo.StackPeak   = stackInfo.StackSize - index;
    }

    view.push(stackInfo);

    return (view);
}


/*
 * ======== getHeap ========
 */
/* eslint-disable-next-line no-unused-vars */
function getHeap()
{
    var view = new Array();

    var heapInfo = new Heap();

    try {
        var current = Program.fetchVariable("xStart");
        var end  = Program.fetchVariable("pxEnd");
        var ucHeapType = Program.lookupTypeByVariable("ucHeap");
    }
    catch (e) {
        heapInfo.HeapType       = "Heap implementation not supported in ROV \
                                   (currently only heap_4 is)";
        view.push(heapInfo);
        return (view);
    }

    heapInfo.HeapAddr = "0x" + Program.lookupSymbolValue("ucHeap").toString(16);
    heapInfo.HeapType    = "heap_4";

    if (end == 0) {
        heapInfo.TotalSize = "Heap not initialized";
    }
    else {
        heapInfo.TotalSize      = ucHeapType.elnum; // sizeof(BlockLink_t)
        heapInfo.FirstFreeBlock = current.pxNextFreeBlock;

        while (current.pxNextFreeBlock < end) {
            current = Program.fetchFromAddr(current.pxNextFreeBlock,
                                            "BlockLink_t");
            heapInfo.TotalFree += current.xBlockSize;
        }
    }

    view.push(heapInfo);

    return (view);
}

/*
 * ======== getTaskModule ========
 */
/* eslint-disable-next-line no-unused-vars */
function getTaskModule()
{
    var view = new Array();

    var taskInfo = new TaskModule();

    var readyList = Program.fetchVariable("pxReadyTasksLists");

    taskInfo.NumPriorities = readyList.length;
    taskInfo.NumTasks      = Program.fetchVariable("uxCurrentNumberOfTasks");

    view.push(taskInfo);

    return (view);
}

/*
 * ======== compareAddress ========
 */
function compareAddress(a, b) {
  let comparison = 0;
  if (a.Address > b.Address) {
    comparison = 1;
  } else if (a.Address < b.Address) {
    comparison = -1;
  }
  return comparison;
}

/*
 * ======== getTaskInstances ========
 */
/* eslint-disable-next-line no-unused-vars */
function getTaskInstances()
{
    var table = new Array();

    /* Ready List */
    var readyList = Program.fetchVariable("pxReadyTasksLists");
    for (var i = 0; i < readyList.length; i++) {
        fillInTaskInstance(table, readyList[i], "Ready");
    }

    /* Suspended List */
    var suspendedList = Program.fetchVariable("xSuspendedTaskList");
    fillInTaskInstance(table, suspendedList, "Blocked");

    /* Delay1 List */
    var delay1List = Program.fetchVariable("xDelayedTaskList1");
    fillInTaskInstance(table, delay1List, "Delayed");

    /* Delay2 List */
    var delay2List = Program.fetchVariable("xDelayedTaskList2");
    fillInTaskInstance(table, delay2List, "Delayed");

    /* Terminated List */
    var terminatedList = Program.fetchVariable("xTasksWaitingTermination");
    fillInTaskInstance(table, terminatedList, "Terminated");

    /* Sort by Address so the tasks don't bounce around in ROV */
    table.sort(compareAddress);

    return (table);
}

/*
 * ======== fillInTaskInstance ========
 */
/* eslint-disable-next-line no-unused-vars */
function fillInTaskInstance(table, list, state)
{
    var currentTask     = Program.fetchVariable("pxCurrentTCB");

    var tcbBase         = list.xListEnd.pxNext - 4;

    for (var i = 0; i < list.uxNumberOfItems; i++) {
        var task = Program.fetchFromAddr(tcbBase, "TCB_t");

        var taskInfo = new TaskInstance();

        // Commenting out since currently the task function is stored on the
        // stack and generally is wiped out. Maybe when we get CallStack we
        // can use it to get the entry function.
        //var nameAddr = tcbBase - 24;
        //var functionAddr = Program.fetchFromAddr(nameAddr, "uintptr_t");

        taskInfo.Address      = "0x" + tcbBase.toString(16);
        var name = "";
        for (var j = 0; j < 12; j++) {
            if (task.pcTaskName[j] == 0) break;
            name = name + String.fromCharCode(task.pcTaskName[j]);
        }
        taskInfo.TaskName     = name;
        //taskInfo.FxnName      = String(Program.lookupFuncName(functionAddr));

        taskInfo.Priority     = task.uxPriority;
        taskInfo.BasePriority = task.uxBasePriority;
        if (tcbBase == currentTask) {
            taskInfo.State    = "Running";
        }
        else {
            taskInfo.State    = state;
        }
        taskInfo.StackBase   = task.pxStack;
        taskInfo.CurrentTaskSP  = task.pxTopOfStack;

        /*
         * We don't know the size of the task stack, so look every 4 bytes :(
         * The stack size is stored in the "malloc" header right before the
         * stack (for dynamically allocated stacks). Tmr Svc and IDLE tasks
         * stack size probably can be queried from the target. These can be
         * used to improve the performance (e.g. read the whole stack instead
         * a word at a time!).
         */
        var stackData = Program.fetchArray(
            {
                type: 'xdc.rov.support.ScalarStructs.S_UChar',
                isScalar: true
            }, task.pxStack, 4);

        var index = task.pxStack;
        /*
         * Find the first non-0xa5.
         */
        while ((stackData[0] == 0xa5) &&
           (stackData[1] == 0xa5) &&
           (stackData[2] == 0xa5) &&
           (stackData[3] == 0xa5)) {
            index += 4;
            var stackData = Program.fetchArray(
                {
                    type: 'xdc.rov.support.ScalarStructs.S_UChar',
                    isScalar: true
                }, index, 4);
        }

        /*
         * The r4, r5, r6, r7, r8, r9, r10, r11, r14 registers could have
         * had 0xa5a5a5a5 in them and make the stack "shrink" when they
         * are written back onto the stack. This happens in the
         * xPortPendSVHandler function.
         * To error on the safe side, subtract 8 registers * 4 bytes
         * from the index.
         * TIRTOS-2091 covers this in more details.
         */
        index -= 8 * 4;

        if ((taskInfo.StackBase >= index) ||
            (taskInfo.StackBase >= taskInfo.CurrentTaskSP)) {
            taskInfo.UnusedStackSize = "Stack Overflow";
        }
        else {
            index -= 4;
            taskInfo.UnusedStackSize = index - taskInfo.StackBase;
        }

        table.push(taskInfo);

        /* Traverse the list */
        tcbBase = task.xStateListItem.pxNext - 4;
    }
}
