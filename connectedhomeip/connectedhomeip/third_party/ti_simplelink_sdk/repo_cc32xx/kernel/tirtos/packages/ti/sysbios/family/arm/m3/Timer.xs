/*
 * Copyright (c) 2013-2020, Texas Instruments Incorporated
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

var Program = xdc.module('xdc.cfg.Program');

var Timer = null;
var TimestampProvider = null;
var Hwi = null;
var BIOS = null;

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
     * instead of an abstract intsance
     */
    Timer.common$.fxntab = false;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Hwi = xdc.useModule("ti.sysbios.family.arm.m3.Hwi");
    BIOS = xdc.useModule('ti.sysbios.BIOS');
    TimestampProvider = xdc.module('ti.sysbios.family.arm.m3.TimestampProvider');

    if ((BIOS.smpEnabled == true) && (BIOS.buildingAppLib == true)) {
        Timer.$logError("This Timer module is not supported in SMP mode", this);
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    /* availMask has 1 bit set for the 1 timer */
    mod.availMask = 0x1;

    mod.tickCount = 0;

    if (params.anyMask > mod.availMask) {
        Timer.$logError("Incorrect anyMask (" + params.anyMask
            + "). Should be <= " + mod.availMask + ".", Timer, "anyMask");
    }

    mod.handle = null;

    /*
     * plug Timer.startup into BIOS.startupFxns array
     */
    BIOS.addUserStartupFunction(Timer.startup);
}

/*
 *  ======== instance$meta$init ========
 */
function instance$meta$init()
{
    Timer.timerInUse = true;  /* Inform TimestampProvider that */
                              /* does not need to be configured */
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, id, tickFxn, params)
{
    var modObj = this.$module.$object;

    /* If TimestampProvider is used, it will start the timer */
    if (TimestampProvider.$used == false) {
        /* set flag because static instances need to be started */
        Timer.startupNeeded = true;
    }

    obj.staticInst = true;

    if ((id != 0) && (id != Timer.ANY)) {
        Timer.$logFatal("Invalid Timer ID " + id + "!", this);
    }

    if (id == Timer.ANY) {
        if ((Timer.anyMask & 1) && (modObj.availMask & 1)) {
            modObj.availMask &= ~(1);
            obj.id = 0;
        }
    }
    else if (modObj.availMask & 1) {
        modObj.availMask &= ~(1);
        obj.id = id;
    }

    if (obj.id == undefined) {
        Timer.$logFatal("Timer device unavailable.", this)
    }

    obj.runMode = params.runMode;
    obj.startMode = params.startMode;
    obj.extFreq.lo = params.extFreq.lo;
    obj.extFreq.hi = params.extFreq.hi;

    if (params.periodType == Timer.PeriodType_MICROSECS) {
        if (setPeriodMicroSecs(obj, params.period) == false) {
            Timer.$logError("SysTick Timer doesn't support configured period.",
                                 this, params.period)
        }
    }
    else {
        obj.period = params.period;
    }
    obj.periodType = Timer.PeriodType_COUNTS;

    obj.intNum = 15;
    obj.arg = params.arg;
    obj.tickFxn = tickFxn;

    if (obj.tickFxn) {
        if (!params.hwiParams) {
            params.hwiParams = new Hwi.Params;
        }
        var hwiParams = params.hwiParams;

        if (Hwi.inUseMeta(obj.intNum) == true) {
            Timer.$logError("Timer interrupt " + obj.intNum +
                            " already in use!", obj);
            return;
        }

        hwiParams.arg = this;

        if (obj.runMode == Timer.RunMode_CONTINUOUS) {
            obj.hwi = Hwi.create(obj.intNum, Timer.periodicStub, hwiParams);
        }
        else {
            obj.hwi = Hwi.create(obj.intNum, Timer.oneShotStub , hwiParams);
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
    var freq;

    if (obj.extFreq.lo) {
        freq = (obj.extFreq.hi << 32) + obj.extFreq.lo;
        freq /= 1000000;                /* convert to MHz */
    }
    else {
        freq = BIOS.cpuFreq.lo / 1000000; /* freq in MHz */
    }

    obj.period = Math.floor(freq * period);

    if (obj.period > 0xffffff) {
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
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' Timer instance view.
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var halTimer = xdc.useModule('ti.sysbios.hal.Timer');

    view.halTimerHandle =  halTimer.viewGetHandle(obj.$addr);
    view.label       = Program.getShortName(obj.$label);
    view.id          = obj.id;

    view.device      = "SysTick";
    view.runMode     = getEnumString(obj.runMode);
    view.startMode   = getEnumString(obj.startMode);
    view.periodType  = getEnumString(obj.periodType);
    view.period      = obj.period;
    view.intNum      = obj.intNum;

    view.tickFxn     = Program.lookupFuncName(Number(obj.tickFxn));
    view.arg         = obj.arg;

    view.extFreq     = Number(obj.extFreq.lo).toString(10);

    view.hwiHandle   = "0x" + Number(obj.hwi).toString(16);
}

/*
 *  ======== viewInitDevice ========
 */
function viewInitDevice(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');

    view.id          = obj.id;
    view.device      = "SysTick";
    view.intNum = obj.intNum;
    view.runMode     = getEnumString(obj.runMode);

    view.devAddr = "0xe000e010";
    var Systick = Program.fetchArray(
            {   type: 'xdc.rov.support.ScalarStructs.S_UInt32',
                isScalar: true
            },
            0xe000e010, 4, false);
    view.period = Systick[1];
    if (getEnumString(obj.runMode) == "RunMode__CONTINUOUS") {
        view.period += 1;
    }
    view.currCount = Systick[2];
    view.remainingCount = view.currCount;
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Timer 'Module' view.
 */
function viewInitModule(view, mod)
{
    view.availMask = Number(mod.availMask).toString(2);
    view.tickCount = mod.tickCount;
}



