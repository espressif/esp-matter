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
 *  ======== GateMutex.xs ========
 *
 */

var Semaphore = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Semaphore = xdc.useModule("ti.sysbios.knl.Semaphore");
    xdc.useModule("ti.sysbios.knl.Task");
    var BIOS = xdc.useModule("ti.sysbios.BIOS");
    if (!(BIOS.libType == BIOS.LibType_Custom
        && BIOS.assertsEnabled == false)) {
        xdc.useModule('xdc.runtime.Assert');
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    Semaphore.construct(obj.sem, 1);
    obj.owner = null;
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
 *  ======== viewCheckForNullObject ========
 *  Returns true if the object is all zeros.
 */
function viewCheckForNullObject(mod, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var objSize = mod.Instance_State.$sizeof();

    /* skip uninitialized objects */
    try {
        var objArray = Program.fetchArray({type: 'xdc.rov.support.ScalarStructs.S_UInt8',
                                    isScalar: true},
                                    Number(obj.$addr),
                                    objSize,
                                    true);
    }
    catch(e) {
        print(e.toString());
    }

    for (var i = 0; i < objSize; i++) {
        if (objArray[i] != 0) return (false);
    }

    return (true);
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' Task instance view.
 */
function viewInitBasic(view, obj)
{
    var GateMutex = xdc.useModule('ti.sysbios.gates.GateMutex');
    if (viewCheckForNullObject(GateMutex, obj)) {
        view.status = "Uninitialized GateMutex object";
        return;
    }

    /* Get owner task. Note: if NULL, there is no owner. */
    if (obj.owner == 0) {
        view.owner  = "N/A";
        view.status = "Not entered"
    }
    else {
        try {
            var taskView = Program.scanHandleView('ti.sysbios.knl.Task', obj.owner, 'Basic');
        }
        catch (e) {
            view.$status["owner"] = "Error: Problem getting owning Task: " + e;
            return;
        }

        // if this Task has no label, use the function name
        if (taskView.label == "" && taskView.fxn[0] != "") {
            view.owner = "taskfxn: " + taskView.fxn[0];
        }
        else {
            view.owner = taskView.label;
        }

        view.status = "Entered"
    }

    /*
     *  Get the GateMutex's Semaphore and use it to get the list of pended
     *  Tasks
     */
    try {
        var semView =
                Program.scanObjectView('ti.sysbios.knl.Semaphore', obj.sem,
                'Basic');
    }
    catch (e) {
        view.$status["pendedTasks"] =
                "Error: Problem getting the list of pending Tasks: " + e;
        return;
    }

    if (semView.pendedTasks.length > 0) {
        view.pendedTasks = semView.pendedTasks;
    }
    else {
         view.pendedTasks.$add("There are no Tasks currently pending.");
    }
}
