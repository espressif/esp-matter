/*
 * Copyright (c) 2014-2017, Texas Instruments Incorporated
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
 *  ======== Timer.xs ========
 *
 */

var Timer = null;
var BIOS = null;
var Clock = null;
var Hwi = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }
    Timer = this;

    /*
     * set fxntab to false because ti.sysbios.hal.Timer can be used
     * instead of an abstract instance
     */
    Timer.common$.fxntab = false;

    /* initialize timer fields */
    Timer.anyMask = 1;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Hwi = xdc.useModule("ti.sysbios.family.arm.m3.Hwi");
    BIOS = xdc.useModule('ti.sysbios.BIOS');
    Clock = xdc.module('ti.sysbios.knl.Clock');
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{

    mod.availMask = 1;

    if (params.anyMask > mod.availMask) {
        Timer.$logError("Incorrect anyMask (" + params.anyMask
            + "). Should be <= " + mod.availMask + ".", Timer);
    }

    mod.handle = null;

    /*
     * plug Timer.startup into BIOS.startupFxns array
     */
    var BIOS = xdc.module ('ti.sysbios.BIOS');
    BIOS.addUserStartupFunction(Timer.startup);
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, id, tickFxn, params)
{
    var modObj = this.$module.$object;

    /* set flag because static instances need to be started */
    Timer.startupNeeded = true;
    obj.staticInst = true;

    if (id >= 1) {
        if (id != Timer.ANY) {
            Timer.$logFatal("Invalid Timer ID: " + id
                            + "; the id must be -1, or 0", this);
        }
    }

    if (id == Timer.ANY) {
        var mask = Timer.anyMask & modObj.availMask;
        for (var i = 0; i < 1; i++) {
            if (mask & (1 << i)) {
                modObj.availMask &= ~(1 << i);
                obj.id = i;
                break;
            }
        }
    }
    else if (modObj.availMask & (1 << id)) {
        modObj.availMask &= ~(1 << id);
        obj.id = id;
    }

    if (obj.id == undefined) {
        var alist = [];
        var mask = Timer.anyMask & modObj.availMask;
        for (var i = 0; i < 1; i++) {
            if (mask & (1 << i)) {
                alist.push(i);
            }
        }
        var avail = alist.length == 0
            ? "no timers are available"
            : ("the current set of  available timer ids includes: "
               + alist.join(", ") + ", or Timer.ANY (-1)");
        Timer.$logFatal("Timer device id " + id + " is unavailable; "
                        + avail, this);
    }

    obj.startMode = params.startMode;
    obj.arg = params.arg;
    obj.tickFxn = tickFxn;


    /* rate of upper 48-bits of RTC */
    obj.frequency.lo = 65536;
    obj.frequency.hi = 0;

    if (params.periodType == Timer.PeriodType_MICROSECS) {
        if (setPeriodMicroSecs(obj, params.period) == false) {
            Timer.$logError("Timer doesn't support configured period.",
                                this, params.period);
        }
    }
    else {
        obj.period = params.period;
    }

    obj.period64 = obj.period;

    obj.savedCurrCount = 0;

    if (params.runMode == Timer.RunMode_DYNAMIC) {
        obj.prevThreshold = obj.period;
    }
    else {
        obj.prevThreshold = 0;
    }
    obj.nextThreshold = obj.period;

    if (obj.tickFxn) {
        if (!params.hwiParams) {
            params.hwiParams = new Hwi.Params;
        }
        var hwiParams = params.hwiParams;

        hwiParams.arg = obj.id;

        if (params.runMode == Timer.RunMode_CONTINUOUS) {
            obj.hwi = Hwi.create(20, Timer.periodicStub, hwiParams);
        }
        else if (params.runMode == Timer.RunMode_DYNAMIC) {

            /* if other RTC channels not hooked, use normal dynamic stub */
            if ((Timer.funcHookCH1 == null) && (Timer.funcHookCH2 == null)) {
                obj.hwi = Hwi.create(20, Timer.dynamicStub, hwiParams);
            }
            /* else, other RTC channels are hooked, use dynamic multi stub */
            else {
                obj.hwi = Hwi.create(20, Timer.dynamicMultiStub, hwiParams);
            }
        }
    }
    else {
        obj.hwi = null;
    }

    modObj.handle = this;
}

/*
 *  ======== setPeriodMicroSecs ========
 */
function setPeriodMicroSecs(obj, period)
{
    /*
     * The upper 16-bits of SUBSEC will roll over every second, so the
     * 'effective' rate the RTC for generating interrupt events is
     * 0x100000000 Hz.
     */
    obj.period = Math.floor(0x100000000 * period / 1000000);

    if (obj.period > 0xffffffff) {
        return false;
    }
    else {
        return true;
    }
}

/*
 *  ======== getEnumString ========
 *  Get the enum value string name, not 0, 1, 2 or 3, etc.  For an enumeration
 *  type property.
 *
 *  Example usage:
 *  if obj contains an enumeration type property "Enum enumProp"
 *
 *  view.enumString = getEnumString(obj.enumProp);
 *
 */
function getEnumString(enumProperty)
{
    /*
     *  Split the string into tokens in order to get rid of the huge package
     *  path that precedes the enum string name. Return the last 2 tokens
     *  concatenated with "_"
     */
    var enumStrArray = String(enumProperty).split(".");
    var len = enumStrArray.length;
    return (enumStrArray[len - 1]);
}

/*
 *  ======== viewGetCurrentClockTick ========
 *  Compute and return the current Clock tick value.
 */
function viewGetCurrentClockTick()
{
    var tickPeriod = Program.$modules['ti.sysbios.knl.Clock'].tickPeriod;
    var period64 = Math.floor(0x100000000 * tickPeriod / 1000000);
    var ticks = 0;

    /*
     * ROV2 brings realtime reads of target memory and registers while a
     * program is running.  Attempting such reads of the RTC shadow registers
     * can freeze program execution in some cases. This problem may be
     * addressed in the future by automatically returning a read error when
     * realtime reads of these registers are attempted.  In the meantime, as
     * a workaround a global Boolean (DISABLE_READ_RTC) will be
     * set when ROV2 is in use.  In the code below, if this flag is defined and
     * set to 'true', an error will be thrown instead of attempting to read
     * the registers; this error will be interpreted as the Timer view code
     * does not support dynamic tick computation, so the most recently updated
     * tick count in Clock module state will be used instead, with a stale data
     * indication. If DISABLE_READ_RTC is defined but set to 'false', or
     * undefined, the RTC registers will be read and the tick count computed
     * and returned.
     */
    if (typeof DISABLE_READ_RTC !== 'undefined') {
        if (DISABLE_READ_RTC == true) {
            throw 'RTC reads disabled';
        }
    }

    try {
        var SEC = Program.fetchArray(
            { type: 'xdc.rov.support.ScalarStructs.S_UInt32', isScalar: true },
            Number("0x40092008"), 1, false);
        var SUBSEC = Program.fetchArray(
            { type: 'xdc.rov.support.ScalarStructs.S_UInt32', isScalar: true },
            Number("0x4009200C"), 1, false);

        /*
         * only 51 bits resolution in JavaScript; break into SEC & SUBSEC
         * pieces
         */
        ticks = SUBSEC / period64;                    /* ticks from SUBSEC */
        ticks = ticks + (SEC * 1000000 / tickPeriod); /* plus ticks from SEC */
        ticks = Math.floor(ticks);                    /* clip total */
    }
    catch (e) {
        print("Error: Problem fetching RTC values: " + e.toString());
    }

    return ticks;
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' Timer instance view.
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var halTimer = xdc.useModule('ti.sysbios.hal.Timer');

    view.halTimerHandle =  halTimer.viewGetHandle(obj.$addr);
    view.label      = Program.getShortName(obj.$label);
    view.id         = obj.id;
    view.startMode  = getEnumString(obj.startMode);
    view.tickFxn    = Program.lookupFuncName(Number(obj.tickFxn));
    view.arg        = obj.arg;
    view.hwiHandle  = "0x" + Number(obj.hwi).toString(16);
}

/*
 *  ======== viewInitDevice ========
 */
function viewInitDevice(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var tNames = ["RTC"];

    view.id = obj.id;
    view.device = tNames[obj.id];

    if ((typeof DISABLE_READ_RTC != "undefined") && DISABLE_READ_RTC) {
        Program.displayError(view, 'devAddr', "Realtime read of RTC is disabled");
        return;
    }

    view.devAddr    = "0x40092000";
    view.intNum	    = 20;

    var TMR = Program.fetchArray(
                {   type: 'xdc.rov.support.ScalarStructs.S_UInt32',
                    isScalar: true
                },
                Number(view.devAddr),
                7,   /* fetch 7 words, including CH0CMP */
                false); /* disable address range check */

    view.currCount = ((TMR[2] & 0xffff) << 16) + ((TMR[3] >> 16) & 0xffff);
    view.nextCompareCount = TMR[6];
    view.remainingCount = view.nextCompareCount - view.currCount; /* compare - count */

    if (TMR[0] & 0x00000001) {
        view.state = "Enabled";
    }
    else {
        view.state = "Disabled";
    }
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Timer 'Module' view.
 */
function viewInitModule(view, obj)
{
    view.availMask = Number(obj.availMask).toString(2);
}
