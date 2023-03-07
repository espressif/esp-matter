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
 *  ======== Clock.rov.js ========
 */

var moduleName = "ti.sysbios.knl::Clock";

var Program = xdc.module('xdc.rov.Program');

var viewMap = [
    {name: "Basic", fxn: "viewClockBasic", structName: "BasicInfo"},
    {name: "Module", fxn: "viewClockModule", structName: "ModuleInfo"}
];

/*
 * Clock Object Basic Information
 */
function BasicInfo()
{
    this.address = null;
    this.symbol = null;
    this.timeout = 0;
    this.period = 0;
    this.fxn = null;
    this.arg = null;
    this.started = null;
    this.tRemaining = null;
    this.periodic = null;
}

/*
 * Clock Module Information
 */
function ModuleInfo()
{
    this.address = null;
    this.ticks = null;
    this.tickSource = null;
    this.tickMode = null;
    this.timerHandle = null;
    this.timerId = 0;
    this.swiPriority = 0;
    this.tickPeriod = 0;
    this.nSkip = 0;
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
 *  ======== viewGetClockObjectArray ========
 *  returns an array of the current clk objects.
 */
function viewGetClockObjectArray()
{
    clkArray = new Array();

    try {
        var modState = Program.fetchVariable("ti_sysbios_knl_Clock_Module_state");
    }
    catch (e) {
        return (clkArray);
    }
    var modStateAddr = Program.lookupSymbolValue("ti_sysbios_knl_Clock_Module_state");
    var clkStructType = Program.lookupType("ti_sysbios_knl_Clock_Struct");
    var offsetOfElem = clkStructType.member.objElem.offset;
    var next = modState.clockQ.next;
    var clkAddr;

    while (Number(next) != Number(modStateAddr)) {
        clkAddr = next - offsetOfElem;
        var clk = new Object();
        clk = Program.fetchFromAddr(clkAddr, "ti_sysbios_knl_Clock_Struct", 1);
        clk.$addr = clkAddr;
        clk.$ticks = modState.ticks;
        clkArray.push(clk);
        next = clk.objElem.next;
    }

    return (clkArray);
}

/*
 *  ======== viewInitBasic ========
 */
function viewInitBasic(view, obj)
{
    view.address = toHexString(obj.$addr);
    view.timeout = obj.timeout;
    view.period = obj.period;
    view.fxn = String(Program.lookupFuncName(Number(obj.fxn)));
    view.arg = toHexString(obj.arg);
    if (obj.active) {
        view.started = "true";

        /*
         * Calculate remaining ticks (tRemaining).
         */
        var modState = Program.fetchVariable("ti_sysbios_knl_Clock_Module_state");
        if (modState.numTickSkip > 1) {
            var ticks = ClockSupport_viewGetCurrentClockTick(modState);
            if (ticks == 0x80000000) {
                /*
                 * If Timer didn't compute/report tick count, but skipping ticks,
                 * indicate 'stale data'.
                 */
                view.tRemaining = "Indeterminate";
            }
            else {
                var remain = obj.currTimeout - ticks;
                /*
                 * Check if 'currTimeout' has wrapped.
                 *
                 * 'currTimeout' is initially calculated as the timeout value plus the
                 * current tick count. This value may be greater than 2^32 and wrap.
                 * We need to account for this here.
                 */
                if (remain < 0) {
                    remain += Math.pow(2, 32);
                }
                view.tRemaining = String(remain);
            }
        }
    }
    else {
        view.started = "false";
    }

    view.periodic = (obj.period > 0) ? "true" : "false";
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
 *  ======== viewClockBasic ========
 */
function viewClockBasic()
{
    view = new Array();

    var clks = viewGetClockObjectArray();

    for (var i = 0; i < clks.length; i++) {
        var obj = clks[i];
        var basicView = new BasicInfo();
        viewInitBasic(basicView, obj);
        view.push(basicView);
    }

    return (view);
}

/*
 *  ======== viewClockModule ========
 */
function viewClockModule()
{
    var modView = new ModuleInfo();

    var modState = Program.fetchVariable("ti_sysbios_knl_Clock_Module_state");

    modView.address = toHexString(Program.lookupSymbolValue("ti_sysbios_knl_Clock_Module_state"));
    modView.ticks = modState.ticks;

    switch (modState.tickSource) {
        case 0:
            modView.tickSource = "TickSource_TIMER";
            var clockTimerAddr = Program.lookupSymbolValue("ti_sysbios_knl_ClockSupport_timerStruct");
            modView.timerHandle = toPaddedHexString(clockTimerAddr, 8);
            break;
        case 1:
            modView.tickSource = "TickSource_USER";
            break;
        case 2:
            modView.tickSource = "TickSource_NULL";
            break;
    }

    modView.tickMode = (modState.tickMode == 0) ? "TickMode_PERIODIC" : "TickMode_DYNAMIC";

    var swi = Program.fetchFromAddr(modState.swi, "ti_sysbios_knl_Swi_Struct", 1);
    modView.swiPriority = swi.priority;

    modView.tickPeriod = modState.tickPeriod;

    return (modView);
}
