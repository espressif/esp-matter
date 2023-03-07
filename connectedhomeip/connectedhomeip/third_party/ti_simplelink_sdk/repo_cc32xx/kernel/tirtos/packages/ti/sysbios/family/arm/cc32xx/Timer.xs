/*
 * Copyright (c) 2014-2015, Texas Instruments Incorporated
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

var INTNUM = 188; /* Timer interrupt */

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

    /*
     * plug Timer.startup into BIOS.startupFxns array
     */
    var BIOS = xdc.module('ti.sysbios.BIOS');
    BIOS.addUserStartupFunction(Timer.startup);

    if (params.anyMask > mod.availMask) {
        Timer.$logError("Incorrect anyMask (" + params.anyMask
            + "). Should be <= " + mod.availMask + ".", Timer);
    }

    mod.handle = null;
    mod.timeUpper = 0;
    mod.period64 = Timer.MAX_PERIOD;
    mod.clock = null;
    mod.nextThreshold = 0;
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

    if (modObj.availMask) {
        obj.id = 0;
        modObj.availMask = 0;
    }
    else {
        Timer.$logFatal("Timer device 0 is unavailable.", this);
    }

    obj.startMode = params.startMode;
    obj.arg = params.arg;
    obj.tickFxn = tickFxn;

    obj.frequency.lo = 32768;
    obj.frequency.hi = 0;

    if (params.periodType == Timer.PeriodType_MICROSECS) {
        if (setPeriodMicroSecs(obj, params.period) == false) {
            Timer.$logError("Timer doesn't support configured period.",
                                this, params.period);
        }
    }
    else {
        obj.period64 = params.period;
    }

    //obj.period64 = obj.period;
    obj.savedCurrCount = 0;

    if (obj.tickFxn) {
        if (!params.hwiParams) {
            params.hwiParams = new Hwi.Params;
        }
        var hwiParams = params.hwiParams;

        hwiParams.arg = obj.id;

        if (params.runMode == Timer.RunMode_CONTINUOUS) {
            obj.hwi = Hwi.create(INTNUM, Timer.periodicStub, hwiParams);
        }
        else if (params.runMode == Timer.RunMode_DYNAMIC) {
            obj.hwi = Hwi.create(INTNUM, Timer.dynamicStub, hwiParams);
        }
    }
    else {
        obj.hwi = null;
    }

    if (BIOS.clockEnabled) {
        /* If Clock is using this timer, start this Clock object */
        if (Clock.TimerProxy.delegate$.$name.match(/ti.sysbios.family.arm.cc32xx.Timer/)) {
            var clockParams = new Clock.Params();
            clockParams.period = 0;
            clockParams.startFlag = false;
            clockParams.arg = null;

            var timeout = 0;

            clockParams.startFlag = false;

            /*
             *  The first timeout will be the last tick before the first
             *  half of the timer counter rolls over.  The timeout will
             *  be re-adjusted in Timer_startup(), but we need to set it
             *  to something non-zero for now.
             */
            timeout = (0x100000000 - 1) / obj.period64;

            /* Construct the Clock object */
            modObj.clock =  Clock.create(Timer.setNextMaxTick, timeout, clockParams);
        }
    }

    modObj.period64 = obj.period64;
    modObj.handle = this;
}

/*
 *  ======== setPeriodMicroSecs ========
 */
function setPeriodMicroSecs(obj, period)
{
    var freq = 32768;  // 32KHz slow clock counter

    /* Convert period in microseconds to number of timer ticks */
    obj.period64 = Math.floor(freq * period / 1000000);

    if (obj.period64 > Timer.MAX_PERIOD) {
        return (false);
    }

    return (true);
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
    view.period     = obj.period64;

    view.tickFxn    = Program.lookupFuncName(Number(obj.tickFxn));
    view.arg        = obj.arg;

    view.hwiHandle  = "0x" + Number(obj.hwi).toString(16);
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
 *  ======== viewInitDevice ========
 */
function viewInitDevice(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');

    try {
        var timerRawView =
            Program.scanRawView('ti.sysbios.family.arm.cc32xx.Timer');
    }
    catch (e) {
        Program.displayError(view,
                'devAddr', "Caught exception retrieving Timer state: " + e);
        return;
    }

    view.intNum	    = INTNUM;
    view.id         = obj.id;
    view.period64   = obj.period64;
    view.nextThreshold = obj.nextThreshold;
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Timer 'Module' view.
 */
function viewInitModule(view, obj)
{
    view.availMask = Number(obj.availMask).toString(2);
}
