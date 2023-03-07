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
 *  ======== Swi.rov.js ========
 */

var moduleName = "ti.sysbios.knl::Swi";

var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewSwiBasic", structName: "BasicInfo"},
    {name: "Module", fxn: "viewSwiModule", structName: "ModuleInfo"}
];

/*
 * Swi Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol = null;
    this.name = null;
    this.state = null;
    this.priority = 0;
    this.fxn = null;
    this.arg0 = null;
    this.arg1 = null;
    this.initTrigger = null;
    this.curTrigger = null;
}

/*
 * Swi Module Information
 */
function ModuleInfo()
{
    this.address = null;
    this.schedulerState = null;
    this.readyQMask = null;
    this.currentSwi = null;
    this.currentFxn = null;
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
 *  ======== viewGetSwiObjectArray ========
 *  returns an array of the current swi objects.
 */
function viewGetSwiObjectArray()
{
    swiArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Swi_Module_state");
    }
    catch (e) {
        return (swiArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_knl_Swi_Module_state");
    var swiStructType = Program.lookupType("ti_sysbios_knl_Swi_Struct");
    var offsetOfObjElem = swiStructType.member.objElem.offset;
    var next = modState.objQ.next;
    var swiAddr;

    while (Number(next) != Number(modStateAddr)) {
        swiAddr = next - offsetOfObjElem;
        var swi = new Object();
        swi = Program.fetchFromAddr(swiAddr, "ti_sysbios_knl_Swi_Struct", 1);
        swi.$addr = swiAddr;
        swiArray.push(swi);
        next = swi.objElem.next;
    }

    return (swiArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    var numPriorities = Program.fetchVariable("ti_sysbios_knl_Swi_numPriorities");
    
    view.address = toHexString(obj.$addr);
    view.fxn = String(Program.lookupFuncName(Number(obj.fxn)));
    view.arg0 = toHexString(obj.arg0);
    view.arg1 = toHexString(obj.arg1);
    view.priority = obj.priority;
    if (obj.priority >= numPriorities) {
        Program.displayError(view, "priority",
        "Corrupted data: Swi priority is greater than Swi.numPriorities");
    }
    view.initTrigger = obj.initTrigger;
    view.curTrigger = obj.trigger;
    viewGetSwiState(view, obj);
    if (Number(obj.name) != 0) {
        view.name = Program.fetchString(Number(obj.name));
    }
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
 *  ======== viewSwiBasic ========
 */
function viewSwiBasic()
{
    view = new Array();

    var swis = viewGetSwiObjectArray();

    for (var i = 0; i < swis.length; i++) {
        var obj = swis[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 *  ======== viewSwiModule ========
 */
function viewSwiModule()
{
    var modView = new ModuleInfo();
    
    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Swi_Module_state");
    }
    catch (e) {
        return (modView);
    }

    modView.address = toHexString(Program.lookupSymbolValue("ti_sysbios_knl_Swi_Module_state"));
    modView.schedulerState = modState.locked ? "Locked" : "Unlocked";
    modView.readyQMask = toPaddedHexString(modState.curSet, 8);
    modView.currentSwi = toHexString(modState.curSwi);
    if (modState.curSwi != 0) {
        var swiAddr = modState.curSwi;
        var swi = Program.fetchFromAddr(swiAddr, "ti_sysbios_knl_Swi_Struct", 1);
        modView.currentFxn = String(Program.lookupFuncName(Number(swi.fxn)));
    }

    return (modView);
}

/* BIOS ThreadType
enum BIOS_ThreadType {
    BIOS_ThreadType_Hwi,    0
    BIOS_ThreadType_Swi,    1
    BIOS_ThreadType_Swi,   2
    BIOS_ThreadType_Main    3
};
*/

function viewGetCurrentThreadType()
{
    var biosModState = Program.fetchVariable("BIOS_Module_state");

    switch (biosModState.threadType) {
        case 0: return ("Hwi");
        case 1: return ("Swi");
        case 2: return ("Swi");
        case 3: return ("Main");
    }
}

/*!
 *  ======== viewGetSwiState ========
 */
function viewGetSwiState(view, obj)
{
    var modState = Program.fetchVariable("ti_sysbios_knl_Swi_Module_state");
    var biosModState = Program.fetchVariable("BIOS_Module_state");
    var mode = obj.mode;

    if (obj.priority == -1) {
        mode = 4;
    }

    if (Number(modState.curSwi) == (view.address)) {
        mode = 0;
        view.state = "Running";
    }
    else if (obj.posted) {
        view.state = "Posted";
    }
    else {
        view.state = "Idle";
    }
}
