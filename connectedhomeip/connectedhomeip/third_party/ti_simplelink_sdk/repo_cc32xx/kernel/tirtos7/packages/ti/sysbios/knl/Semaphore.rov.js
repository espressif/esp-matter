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
 *  ======== Semaphore.rov.js ========
 */

var moduleName = "ti.sysbios.knl::Semaphore";

var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewSemaphoreBasic", structName: "BasicInfo"}
];

/*
 * Semaphore Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol = null;
    this.event = null;
    this.eventId = null;
    this.mode = null;
    this.count = null;
    this.pendedTasks = [];
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
 *  ======== viewGetSemaphoreObjectArray ========
 *  returns an array of the current Semaphore objects.
 */
function viewGetSemaphoreObjectArray()
{
    semArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Semaphore_Module_state");
    }
    catch (e) {
        return (semArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_knl_Semaphore_Module_state");
    var semStructType = Program.lookupType("ti_sysbios_knl_Semaphore_Struct");
    var offsetOfObjElem = semStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var semAddr;

    while (Number(next) != Number(modStateAddr)) {
        semAddr = next - offsetOfObjElem;
        var sem = new Object();
        sem = Program.fetchFromAddr(semAddr, "ti_sysbios_knl_Semaphore_Struct", 1);
        sem.$addr = semAddr;
        semArray.push(sem);
        next = sem.objElem.next;
    }

    return (semArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    view.address = toHexString(obj.$addr);
    if (obj.event == 0x0) {
        view.event = "none";
        view.eventId = "n/a";
    }
    else {
        view.event = obj.event;
        view.eventId = obj.eventId;
    }

    switch (obj.mode) {
        case 0x0:
            view.mode = "counting";
            break;
        case 0x1:
            view.mode = "binary";
            break;
        case 0x2:
            view.mode = "counting (priority)";
            break;
        case 0x3:
            view.mode = "binary (priority)";
            break;
        default:
            view.mode = "error: invalid mode";
            break;
    }

    if ((obj.count > 1) && (obj.mode == 0x1 || obj.mode == 0x3)) {
        view.count = "Error: Count value of binary semaphore should not be \
                      greater than 1."
    }
    else {
        view.count = obj.count;
    }
    view.symbol = String(Program.lookupDataSymbol(Number(obj.$addr)));
    /*
     *  In case the dynamically created object was the first thing in the
     *  BIOS_heap.
     */
    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }

    var typespec = Program.lookupType("ti_sysbios_knl_Semaphore_Struct");
    var offset = typespec.member["pendQ"].offset;
    var head = obj.$addr + offset
    var next = obj.pendQ.next;

    if (next == head) {
        view.pendedTasks.push("none");
    }

    while (next != head) {

        var semPendElem = new Object();
        semPendElem = Program.fetchFromAddr(next, "ti_sysbios_knl_Semaphore_PendElem", 1);

        var clockObj = new Object();
        clockObj = Program.fetchFromAddr(semPendElem.tpElem.clockHandle, "ti_sysbios_knl_Clock_Struct", 1);

        var taskObj = new Object();
        taskObj = Program.fetchFromAddr(semPendElem.tpElem.taskHandle, "ti_sysbios_knl_Task_Struct", 1);
        taskObj.$addr = semPendElem.tpElem.taskHandle;

        var pendState;

        switch (semPendElem.pendState) {
            case 0: //Semaphore_PendState_TIMEOUT
                 pendState = "Timed Out";
            break;
            case 1: //Semaphore_PendState_POSTED
                pendState = "Posted";
                break;
            case 2: //Semaphore_PendState_CLOCK_WAIT
                try {
                    var modClockState = Program.fetchVariable("Clock_Module_state");
                    pendState = "Waiting for " +
                        (clockObj.currTimeout - modClockState.ticks) +
                        " more ticks";
                }
                catch (e) {
                    pendState = "Waiting for timeout but no Clock module present!";
                }
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
        view.pendedTasks.push(pendString);

        next = semPendElem.tpElem.qElem.next;
    }
}

/*
 *  ======== viewSemaphoreBasic ========
 */
function viewSemaphoreBasic()
{
    var view = new Array();

    var sems = viewGetSemaphoreObjectArray();

    for (var i = 0; i < sems.length; i++) {
        var obj = sems[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}
