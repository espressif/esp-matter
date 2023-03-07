/*
 * Copyright (c) 2012-2019, Texas Instruments Incorporated
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
 *  ======== Semaphore.xs ========
 *
 *1                     aren't synced to Semaphore state
 *! 05-Mar-2008 agd     Rework Semaphore/Event design
 *! 01-Feb-2007 rt      Added useModule(Log)
 *! 22-Sep-2006 agd     added eventId null check
 *! 17-May-2005 nitya/kw  created
 */

var Queue = null;
var Semaphore = null;
var Event = null;
var Clock = null;
var Program = null;
var BIOS = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Semaphore = this;

    BIOS = xdc.module('ti.sysbios.BIOS');
    if (!(BIOS.libType == BIOS.LibType_Custom && BIOS.logsEnabled == false)) {
        xdc.useModule('xdc.runtime.Log');
    }
    if (!(BIOS.libType == BIOS.LibType_Custom
        && BIOS.assertsEnabled == false)) {
        xdc.useModule('xdc.runtime.Assert');
    }

    Program = xdc.module('xdc.cfg.Program');
    Queue = xdc.useModule('ti.sysbios.knl.Queue');
    xdc.useModule("ti.sysbios.hal.Hwi");
    xdc.useModule("ti.sysbios.knl.Task");

    if (BIOS.clockEnabled == true) {
        Clock = xdc.useModule("ti.sysbios.knl.Clock");
    }

    if (Semaphore.supportsEvents == true) {
        Event = xdc.useModule('ti.sysbios.knl.Event');
        Semaphore.eventPost = '&ti_sysbios_knl_Event_post__E';
        Semaphore.eventSync = '&ti_sysbios_knl_Event_sync__E';
    }
    else {
        Semaphore.eventPost = null;
        Semaphore.eventSync = null;
    }
}

/*
 * used to translate an eventId into its corresponding Event.Id string
 */
var id_to_string = {
    0x10000 :        "Event.Id_16",
    0x20000 :        "Event.Id_17",
    0x40000 :        "Event.Id_18",
    0x80000 :        "Event.Id_19",
    0x100000 :       "Event.Id_20",
    0x200000 :       "Event.Id_21",
    0x400000 :       "Event.Id_22",
    0x800000 :       "Event.Id_23",
    0x1000000 :      "Event.Id_24",
    0x2000000 :      "Event.Id_25",
    0x4000000 :      "Event.Id_26",
    0x8000000 :      "Event.Id_27",
    0x10000000 :     "Event.Id_28",
    0x20000000 :     "Event.Id_29",
    0x40000000 :     "Event.Id_30",
    0x80000000 :     "Event.Id_31"
};

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, count, params)
{
    obj.mode = params.mode;
    obj.count = count;

    if (obj.mode == Semaphore.Mode_BINARY) {
        if ((count != 0) && (count != 1)) {
            Semaphore.$logError("Count must be 0 or 1 for binary semaphores!",
                this);
        }
    }
    obj.event = params.event;
    obj.eventId = params.eventId;

    /* validate event params */
    if (obj.event != null) {
        if (Semaphore.supportsEvents) {
            if ((BIOS.bitsPerInt == 16) && (obj.eventId > Event.Id_15)) {
                Semaphore.$logError(id_to_string[obj.eventId] + 
                        " is not supported by this target. " +
                        "The maximum Event ID supported is " +
                        "Event.Id_15.",
                        obj, "eventId");
            }
            else {
                if (obj.eventId == 0) {
                    Semaphore.$logFatal("eventId must not be zero.", 
                        this, "supportsEvents");
                }
                else {
                    obj.event.syncMeta(obj.eventId, obj.count);
                }
            }
        }
        /* set event Mode to semaphore Mode */
        else {
            Semaphore.$logError(
                "To support semaphores with events, " +
                "ti.sysbios.knl.Semaphore.supportsEvents " +
                "must be set to true.", this, "supportsEvents");
        }
    }

    Queue.construct(obj.pendQ);
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
 *  Process the 'Basic' view for a Semaphore instance.
 *
 *  TODO:
 *    - Add clock info to pending Tasks
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Semaphore = xdc.useModule('ti.sysbios.knl.Semaphore');
    var Clock = xdc.useModule('ti.sysbios.knl.Clock');
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    if (viewCheckForNullObject(Semaphore, obj)) {
        view.label = "Uninitialized Semaphore object";
        return;
    }

    view.label = Program.getShortName(obj.$label);

    if (obj.event != 0) {
        view.event = String(obj.event);
        view.eventId = String(obj.eventId);
    }
    else {
        view.event = "none";
        view.eventId = "n/a";
    }

    switch (obj.mode) {
        case Semaphore.Mode_COUNTING:
            view.mode = "counting";
            break;
        case Semaphore.Mode_BINARY:
            view.mode = "binary";
            break;
        case Semaphore.Mode_COUNTING_PRIORITY:
            view.mode = "counting (priority)";
            break;
        case Semaphore.Mode_BINARY_PRIORITY:
            view.mode = "binary (priority)";
            break;
    }

    view.count = obj.count;

    /* Validate count is not greater than one if binary sem */
    if ((obj.count > 1) && (obj.mode == Semaphore.Mode_BINARY)) {
        view.$status["count"] = "Error: Count value of binary semaphore should not be greater than 1.";
    }

    /* Scan the pendQ to get its elements */
    try {
        var pendQView = Program.scanObjectView('ti.sysbios.knl.Queue', obj.pendQ, 'Basic');
    }
    catch (e) {
        view.$status["pendElems"] = "Error: Problem scanning pend Queue: " + e.toString();
        return;
    }

    /* Add to the pendQ label */
    pendQView.label = (view.label + ".pendQ");

    /* Retrieve the Task label for each of the pending Tasks. */
    for (var i=0; i<pendQView.elems.length; i++) {

        /* Get the element address from the Queue view. */
        var pendElemAddr = pendQView.elems[i];

        /* Fetch the PendElem struct */
        try {
           var pendElem = Program.fetchStruct(Semaphore.PendElem$fetchDesc, pendElemAddr);
        }
        catch (e) {
            view.$status["pendElems"] = "Problem with fetching pend element 0x" +
                                        Number(pendElemAddr).toString(16) + 
                                        ": " + e.toString();
            return;
        }


        if (pendElem.tpElem.clock != 0) {
            try {
                var clockView = Program.scanHandleView('ti.sysbios.knl.Clock', pendElem.tpElem.clock, 'Basic');
            }
            catch (e) {
                view.$status["timeout"] = "Problem scanning pending Clock 0x" +
                                        Number(pendElem.tpElem.clock).toString(16) + 
                                        ": " + e.toString();
                return;
            }
        }

        var pendState;
        switch (pendElem.pendState) {
            case Semaphore.PendState_TIMEOUT:
                pendState = "Timed Out";
                break;
            case Semaphore.PendState_POSTED:
                pendState = "Posted";
                break;
            case Semaphore.PendState_CLOCK_WAIT:
                pendState = "Waiting for " + clockView.tRemaining + " more ticks";
                break;
            case Semaphore.PendState_WAIT_FOREVER:
                pendState = "Waiting forever";
                break;
        }
        
        /* 
         * The PendElem struct contains a handle to a Task instance.
         * Scan the handle. 
         */
        try {
            var taskView = Program.scanHandleView('ti.sysbios.knl.Task', pendElem.tpElem.task, 'Basic');
        }
        catch (e) {
            view.$status["pendElems"] = "Problem scanning pending Task 0x" +
                                        Number(pendElem.tpElem.task).toString(16) + 
                                        ": " + e.toString();
            return;
        }
        
        var pendElemString = Task.getNickName(taskView) + 
                             ", priority: " + taskView.priority + 
                             ", pendState: " + pendState;

        view.pendedTasks.$add(pendElemString);
    }

    if (view.pendedTasks.length == 0) {
        view.pendedTasks.$add("none");
    }
}
