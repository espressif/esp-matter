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
 *  ======== GateMutex.rov.js ========
 */

var moduleName = "ti.sysbios.gates::GateMutex";

var Program = xdc.module('xdc.rov.Program');

/* To extract pending tasks */
var Mailbox = xdc.loadCapsule('/ti/sysbios/knl/Mailbox.rov.js');

var viewMap = [
    {name: "Basic", fxn: "viewGateMutexBasic", structName: "BasicInfo"}
];

/*
 * GateMutex Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol = null;
    this.status = null;
    this.owner = null;
    this.pendedTasks = [];
}

/*
 * ======== toHexString ========
 * Converts a number to a hex string with "0x" prefix.
 */
function toHexString(num) {
    return ("0x" + Number(num).toString(16));
}

/*
 * ======== toPaddedHexString ========
 * Converts a number to a hex string with "0x" prefix.
 * inserts up to (len - 1) leading zeros.
 * up to a max of 7 zeros.
 */
function toPaddedHexString(number, len)
{
    return ("0x" + ("0000000" + number.toString(16)).substr(-len));
}

/*
 * ======== viewGetGateMutexObjectArray ========
 * Returns an array of the current GateMutex objects.
 */
function viewGetGateMutexObjectArray()
{
    gateArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_gates_GateMutex_Module_state");
    }
    catch (e) {
        return (gateArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_gates_GateMutex_Module_state");
    var gateStructType = Program.lookupType("ti_sysbios_gates_GateMutex_Struct");
    var offsetOfObjElem = gateStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var gateAddr;

    while (Number(next) != Number(modStateAddr)) {
        gateAddr = next - offsetOfObjElem;
        var gate = new Object();
        gate = Program.fetchFromAddr(gateAddr, "ti_sysbios_gates_GateMutex_Struct", 1);
        gate.$addr = gateAddr;
        gateArray.push(gate);
        next = gate.objElem.next;
    }

    return (gateArray);
}

/*
 * ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    view.address = toHexString(obj.$addr);
    view.symbol = String(Program.lookupDataSymbol(Number(obj.$addr)));

    /*
     *  In case the dynamically created object was the first thing in the
     *  BIOS_heap.
     */
    if (view.symbol == "BIOS_heap") {
        view.symbol = "";
    }

    if (obj.owner == 0) {
        view.status = "Not Entered";
        view.owner = "N/A";
    } else {
        view.status = "Entered";
        var taskInfo = Program.fetchFromAddr(obj.owner, "ti_sysbios_knl_Task_Struct", 1);
        view.owner = String(Program.lookupFuncName(Number(taskInfo.fxn)));
    }

    var gateMutexTypeSpec = Program.lookupType("ti_sysbios_gates_GateMutex_Struct");
    var gateMutexSemOffset = gateMutexTypeSpec.member["sem"].offset;
    try {
        Mailbox.getPending(view.pendedTasks, obj, gateMutexSemOffset);
    } catch (e) {
        view.pendedTasks = ["none"];
    }
}

/*
 * ======== viewGateMutexBasic ========
 */
function viewGateMutexBasic()
{
    view = new Array();

    var gates = viewGetGateMutexObjectArray();

    for (var i = 0; i < gates.length; i++) {
        var obj = gates[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}
