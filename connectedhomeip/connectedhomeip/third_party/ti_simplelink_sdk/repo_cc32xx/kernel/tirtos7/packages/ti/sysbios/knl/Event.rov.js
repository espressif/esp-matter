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
 *  ======== Event.rov.js ========
 */

var moduleName = "ti.sysbios.knl::Event";

var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewEventBasic", structName: "BasicInfo"}
];

/*
 * Event Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol = null;
    this.postedEvents = null;
    this.pendedTask = null;
    this.andMask = null;
    this.orMask = null;
    this.timeout = null;
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
 *  ======== viewGetEventObjectArray ========
 *  returns an array of the current Event objects.
 */
function viewGetEventObjectArray()
{
    evtArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Event_Module_state");
    }
    catch (e) {
        return (evtArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_knl_Event_Module_state");
    var evtStructType = Program.lookupType("ti_sysbios_knl_Event_Struct");
    var offsetOfObjElem = evtStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var evtAddr;

    while (Number(next) != Number(modStateAddr)) {
        evtAddr = next - offsetOfObjElem;
        var evt = new Object();
        evt = Program.fetchFromAddr(evtAddr, "ti_sysbios_knl_Event_Struct", 1);
        evt.$addr = evtAddr;
        evtArray.push(evt);
        next = evt.objElem.next;
    }

    return (evtArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    view.address = toHexString(obj.$addr);
    view.postedEvents = toHexString(obj.postedEvents);
    view.symbol = String(Program.lookupDataSymbol(Number(obj.$addr)));
    /*
     *  In case the dynamically created object was the first thing in the
     *  BIOS_heap.
     */
    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }

    var typespec = Program.lookupType("ti_sysbios_knl_Event_Struct");
    var offset = typespec.member["pendQ"].offset;
    var head = obj.$addr + offset
    var next = obj.pendQ.next;

    /* Below loop will over-write as needed */
    view.pendedTask = "none";

    while (next != head) {

        var eventPendElem = new Object();
        eventPendElem = Program.fetchFromAddr(next, "ti_sysbios_knl_Event_PendElem", 1);

        var clockObj = new Object();
        clockObj = Program.fetchFromAddr(eventPendElem.tpElem.clockHandle, "ti_sysbios_knl_Clock_Struct", 1);

        var taskObj = new Object();
        taskObj = Program.fetchFromAddr(eventPendElem.tpElem.taskHandle, "ti_sysbios_knl_Task_Struct", 1);
        taskObj.$addr = eventPendElem.tpElem.taskHandle;

        var modClockState = Program.fetchVariable("ti_sysbios_knl_Clock_Module_state");

        var pendState;

        switch (eventPendElem.pendState) {
            case 0: //Event_PendState_TIMEOUT
                 pendState = "Timed Out";
            break;
            case 1: //Event_PendState_POSTED
                pendState = "Posted";
                break;
            case 2: //Event_PendState_CLOCK_WAIT
                pendState = "Waiting for " +
                    (clockObj.currTimeout - modClockState.ticks) +
                    " more ticks";
                view.timeout = clockObj.timeout;
                break;
            case 3: //Event_PendState_WAIT_FOREVER
                pendState = "Waiting forever";
                view.timeout = "WAIT_FOREVER";
                break;
            default:
                pendState = "Error: pendState = " + eventPendElem.pendState;
                break;
        }


        var pendString = "task: " + toHexString(taskObj.$addr);
        pendString += ", Fxn: " + String(Program.lookupFuncName(Number(taskObj.fxn)));
        pendString += ", priority: " + Number(taskObj.priority);
        pendString += ", pendState: " + pendState;
        view.pendedTask = pendString;
        view.andMask = toHexString(eventPendElem.andMask);
        view.orMask = toHexString(eventPendElem.orMask);

        next = eventPendElem.tpElem.qElem.next;
    }

}

/*
 *  ======== viewEventBasic ========
 */
function viewEventBasic()
{
    var view = new Array();

    var evts = viewGetEventObjectArray();

    for (var i = 0; i < evts.length; i++) {
        var obj = evts[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}
