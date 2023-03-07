/*
 * Copyright (c) 2012-2020, Texas Instruments Incorporated
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
 *  ======== GateMutexPri.xs ========
 *
 */

var Queue = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Queue = xdc.useModule("ti.sysbios.knl.Queue");
    xdc.useModule("ti.sysbios.knl.Task");
    xdc.useModule("ti.sysbios.hal.Hwi");
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    obj.mutexCnt = 1;
    obj.owner = null;
    obj.ownerOrigPri = 0;
    Queue.construct(obj.pendQ);
}

/*
 *  ======== queryMeta ========
 */
function queryMeta(qual)
{
    var rc = false;
    var IGateProvider = xdc.module('xdc.runtime.IGateProvider');

    switch (qual) {
        case IGateProvider.Q_BLOCKING:
        case IGateProvider.Q_PREEMPTING:
            rc = true;
            break;

        default:
            this.$logWarning("Invalid quality. Returning false", this, qual);
            break;
    }

    return (rc);
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' GateMutexPri instance view.
 */
function viewInitBasic(view, obj)
{
    var Task = xdc.useModule('ti.sysbios.knl.Task');
    view.label     = Program.getShortName(obj.$label);
    view.mutexCnt  = obj.mutexCnt;

    // if mutexCnt == 1, then gate is un-owned
    if (obj.mutexCnt == 1) {
        view.owner  = "None";
        view.ownerOrigPri  = 0;
        view.ownerCurrPri  = 0;
        view.status = "Not entered"
    }
    else {
        try {
            var taskView =
              Program.scanHandleView('ti.sysbios.knl.Task', obj.owner, 'Basic');
        }
        catch (e) {
            view.$status["owner"] = "Could not find this gate's owning Task";
            view.$status["ownerOrigPri"] =
                    "Error: Could not find this gate's owning Task";
            return;
        }

        view.owner = Task.getNickName(taskView);

        view.ownerOrigPri = obj.ownerOrigPri;
        view.ownerCurrPri = taskView.priority;
        view.status = "Entered"
    }
}

/*
 *  ======== viewInitDetailed ========
 *  Initialize the 'Detailed' GateMutexPri instance view.
 */
function viewInitDetailed(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    // first get the Basic view:
    viewInitBasic(view, obj);

    /* Scan the pendQ to get its elements */
    try {
        var pendQView =
            Program.scanObjectView('ti.sysbios.knl.Queue', obj.pendQ, 'Basic');
    }
    catch (e) {
        view.$status["pendElems"] =
                "Error: Problem scanning pend Queue: " + e.toString();
        return;
    }

    /* Loop over each of the pending Tasks. */
    for (var i = 0; i < pendQView.elems.length; i++) {

        /* Get the element address from the Queue view. */
        var pendElemAddr = pendQView.elems[i];

        var pendElem = Program.fetchStruct(Task.PendElem$fetchDesc, pendElemAddr);

        // element 'i' of the pendQ contains the current Task handle
        var currTaskHandle = pendElem.task;

        // scan the curr Task handle to get its view
        try {
            var currTaskView = Program.scanHandleView('ti.sysbios.knl.Task',
                    currTaskHandle, 'Basic');
        }
        catch (e) {
            view.$status["pendElems"] =
                    "Error: Problem scanning pending Task 0x" +
                    Number(currTaskHandle).toString(16) +
                    ": " + e.toString();
            return;
        }

        var pendElemString = Task.getNickName(currTaskView) +
                             ", priority: " + currTaskView.priority;
        view.pendedTasks.$add(pendElemString);
    }

    // if the pendQ String array is empty, then there weren't any pending Tasks
    if (view.pendedTasks.length == 0) {
        view.pendedTasks.$add("There are no Tasks currently pending.");
    }
}
