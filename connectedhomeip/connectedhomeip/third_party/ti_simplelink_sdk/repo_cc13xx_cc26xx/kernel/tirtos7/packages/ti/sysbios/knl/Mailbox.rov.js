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
 *  ======== Mailbox.rov.js ========
 */

var moduleName = "ti.sysbios.knl::Mailbox";

var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewMailboxBasic", structName: "BasicInfo"},
    {name: "Detailed", fxn: "viewMailboxDetailed", structName: "DetailedInfo"}
];

/*
 * Mailbox Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol = null;
    this.msgSize = null;
    this.numMsgs = null;
}

/*
 * Mailbox Object Detailed Information
 */
function DetailedInfo()
{
    this.address = null;
    this.symbol = null;
    this.msgSize = null;
    this.numMsgs = null;
    this.freeSlots = null;
    this.pendQueue = [];
    this.postQueue = [];
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
 * ======== getPending ========
 *  Grab the pending task on the queue denoted by the semOffset value.
 *  Most of this repeated from Semaphore.rov.js. Probably could just call
 *  that instead of repeating the code...
 */
function getPending(viewQueue, obj, semOffset)
{
    var semObj = new Object();
    semObj = Program.fetchFromAddr(obj.$addr + semOffset, "ti_sysbios_knl_Semaphore_Struct", 1);

    var typespec = Program.lookupType("ti_sysbios_knl_Semaphore_Struct");
    var offset = typespec.member["pendQ"].offset;

    var head = obj.$addr + semOffset + offset
    var next = semObj.pendQ.next;

    if (next == head) {
        viewQueue.push("none");
    }

    while (next != head) {
        var semPendElem = new Object();
        semPendElem = Program.fetchFromAddr(next, "ti_sysbios_knl_Semaphore_PendElem", 1);

        var clockObj = new Object();
        clockObj = Program.fetchFromAddr(semPendElem.tpElem.clockHandle, "ti_sysbios_knl_Clock_Struct", 1);

        var taskObj = new Object();
        taskObj = Program.fetchFromAddr(semPendElem.tpElem.taskHandle, "ti_sysbios_knl_Task_Struct", 1);
        taskObj.$addr = semPendElem.tpElem.taskHandle;

        var modClockState = Program.fetchVariable("ti_sysbios_knl_Clock_Module_state");

        var pendState;

        switch (semPendElem.pendState) {
            case 0: //Semaphore_PendState_TIMEOUT
                 pendState = "Timed Out";
            break;
            case 1: //Semaphore_PendState_POSTED
                pendState = "Posted";
                break;
            case 2: //Semaphore_PendState_CLOCK_WAIT
                pendState = "Waiting for " +
                    (clockObj.currTimeout - modClockState.ticks) +
                    " more ticks";
                break;
            case 3: //Semaphore_PendState_WAIT_FOREVER
                pendState = "Waiting forever";
                break;
            default:
                pendState = "Error: pendState = " + semPendElem.pendState;
                break;
        }


        var pendString = "task: " + toHexString(taskObj.$addr);
        pendString += ", Fxn: " + String(Program.lookupFuncName(Number(taskObj.fxn)));
        pendString += ", priority: " + Number(taskObj.priority);
        pendString += ", pendState: " + pendState;
        viewQueue.push(pendString);

        next = semPendElem.tpElem.qElem.next;
    }
}

/*
 *  ======== viewGetMailboxObjectArray ========
 *  returns an array of the current Mailbox objects.
 */
function viewGetMailboxObjectArray()
{
    mbxArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Mailbox_Module_state");
    }
    catch (e) {
        return (mbxArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_knl_Mailbox_Module_state");
    var mbxStructType = Program.lookupType("ti_sysbios_knl_Mailbox_Struct");
    var offsetOfObjElem = mbxStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var mbxAddr;

    while (Number(next) != Number(modStateAddr)) {
        mbxAddr = next - offsetOfObjElem;
        var mbx = new Object();
        mbx = Program.fetchFromAddr(mbxAddr, "ti_sysbios_knl_Mailbox_Struct", 1);
        mbx.$addr = mbxAddr;
        mbxArray.push(mbx);
        next = mbx.objElem.next;
    }

    return (mbxArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    view.address = toHexString(obj.$addr);
    view.msgSize = obj.msgSize;
    view.numMsgs = obj.numMsgs;
    view.symbol = String(Program.lookupDataSymbol(Number(obj.$addr)));
    /*
     *  In case the dynamically created object was the first thing in the
     *  BIOS_heap.
     */
    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }
}

/*
 *  ======== viewMailboxBasic ========
 */
function viewMailboxBasic()
{
    view = new Array();

    var mbxs = viewGetMailboxObjectArray();

    for (var i = 0; i < mbxs.length; i++) {
        var obj = mbxs[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 *  ======== viewMailboxDetailed ========
 */
function viewMailboxDetailed()
{
    view = new Array();
    var offset

    var mbxs = viewGetMailboxObjectArray();

    for (var i = 0; i < mbxs.length; i++) {
        var obj = mbxs[i];
        var detailedView = new DetailedInfo();
        viewInitBasic(detailedView, obj);

        detailedView.freeSlots = obj.numFreeMsgs;

        var typespec = Program.lookupType("ti_sysbios_knl_Mailbox_Struct");
        offset = typespec.member["dataSem"].offset;
        getPending(detailedView.pendQueue, obj, offset);

        offset = typespec.member["freeSem"].offset;
        getPending(detailedView.postQueue, obj, offset);

        view.push(detailedView);
    }

    return (view);
}
