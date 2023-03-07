/*
 * Copyright (c) 2013-2019, Texas Instruments Incorporated
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
 *  ======== Event.xs ========
 *
 *1                     aren't synced to Semaphore state
 *! 04-Mar-2008 agd     Stripped down to support binary only and
 *!                     single task.
 *! 23-May-2007 agd     Addressed SDSCM00017699
 *! 01-Feb-2007 rt      Added useModule(Log)
 *! 22-Sep-2006 agd     added eventId null check
 *! 20-Feb-2006 agd     reworked per current design requirements
 *! 31-Jan-2006 nitya/agd created
 */

var Queue = null;
var Clock = null;
var Program = null;
var BIOS = null;
var Event = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Event = this;
    BIOS = xdc.module('ti.sysbios.BIOS');
    if (!(BIOS.libType == BIOS.LibType_Custom && BIOS.logsEnabled == false)) {
        xdc.useModule('xdc.runtime.Log');
    }

    Program = xdc.module('xdc.cfg.Program');
    Queue = xdc.useModule("ti.sysbios.knl.Queue");
    xdc.useModule("ti.sysbios.hal.Hwi");
    xdc.useModule("ti.sysbios.knl.Task");

    /*
     * Pulling in Clock should be conditioned on BIOS.clockEnabled
     * but this causes linker issues. Pull it in
     * anyway and let Clock.xs and Clock.c
     * minimize their own code/data footprints
     * when BIOS.clockEnabled is false
     */

    Clock = xdc.useModule("ti.sysbios.knl.Clock");

    /*
     * Add a "null" instance entry option for the user to select in
     * Grace's Semaphore 'event' instance config parameter.
     */
    Event.eventInstances[Event.eventInstances.length++] = "null";
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, params)
{
    obj.postedEvents = 0;
    Queue.construct(obj.pendQ);

    /* keep Grace informed of the new Event instance */
    Event.eventInstances[Event.eventInstances.length++] = params.instance.name;
}

/*
 *  ======== Event.syncMeta ========
 */
function syncMeta(eventId, count)
{
    if (count == 0) {
        this.$object.postedEvents &= ~eventId;
    }
    else {
        this.$object.postedEvents |= eventId;
    }
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
 *  Process the 'Basic' view for an Event instance.
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Event = xdc.useModule('ti.sysbios.knl.Event');
    var Queue = xdc.useModule('ti.sysbios.knl.Queue');
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    if (viewCheckForNullObject(Event, obj)) {
        view.label = "Uninitialized Event object";
        return;
    }

    view.label = Program.getShortName(obj.$label);
    view.postedEvents = "0x" + obj.postedEvents.toString(16);

    /* Scan the pendQ to get its elements */
    try {
        var pendQView = Program.scanObjectView('ti.sysbios.knl.Queue', obj.pendQ, 'Basic');
    }
    catch (e) {
        view.$status["pendElems"] = "Error: Problem scanning pend Queue: " + e.toString();
        return;
    }

    // initialize view variables so they are never blank in ROV view
    view.pendedTask = "none";
    view.andMask    = "n/a";
    view.orMask     = "n/a";
    view.timeout    = "n/a";

    /* Add to the pendQ label */
    pendQView.label = (view.label + ".pendQ");

    for (var i=0; i<pendQView.elems.length; i++) {
        /* Get the element address from the Queue view. */
        var pendElemAddr = pendQView.elems[i];

        /* Fetch the PendElem struct */
        try {
           var pendElem = Program.fetchStruct(Event.PendElem$fetchDesc, pendElemAddr);
        }
        catch (e) {
            view.$status["pendElems"] = "Problem with fetching pend element 0x" +
                                        Number(pendElemAddr).toString(16) +
                                        ": " + e.toString();
            return;
        }

        if (pendElem.tpElem.clock == 0) {
            var timeout = "WAIT_FOREVER";
        }
        else {
            try {
                var clockView = Program.scanHandleView('ti.sysbios.knl.Clock', pendElem.tpElem.clock, 'Basic');
            }
            catch (e) {
                view.$status["timeout"] = "Problem scanning pending Clock 0x" +
                                        Number(pendElem.tpElem.clock).toString(16) +
                                        ": " + e.toString();
                return;
            }
            var timeout = clockView.timeout.toString(10);
        }

        var pendState;
        switch (pendElem.pendState) {
            case Event.PendState_TIMEOUT:
                pendState = "Timed Out";
                break;
            case Event.PendState_POSTED:
                pendState = "Posted";
                break;
            case Event.PendState_CLOCK_WAIT:
                pendState = "Waiting for " + clockView.tRemaining + " more ticks";
                break;
            case Event.PendState_WAIT_FOREVER:
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

        var andMask = "0x" + pendElem.andMask.toString(16);
        var orMask = "0x" + pendElem.orMask.toString(16);

        var pendElemString = Task.getNickName(taskView) +
                             ", priority: " + taskView.priority +
                             ", pendState: " + pendState;

        view.pendedTask = pendElemString;
        view.andMask = andMask;
        view.orMask = orMask;
        view.timeout = timeout;
    }
}
