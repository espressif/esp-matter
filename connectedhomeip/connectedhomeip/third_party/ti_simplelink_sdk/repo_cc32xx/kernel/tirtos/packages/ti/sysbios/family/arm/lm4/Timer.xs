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

var Program = xdc.module('xdc.cfg.Program');

var Timer = null;
var Hwi = null;
var BIOS = null;
var Startup = null;
var timers = null;

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

    var peripheralTimer = xdc.module('ti.catalog.arm.peripherals.timers.Timer');
    timers = peripheralTimer.getAll();

    /* initialize timer fields */
    Timer.anyMask = (1 << timers.length) - 1;
    Timer.numTimerDevices = timers.length;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Hwi = xdc.useModule("ti.sysbios.family.arm.m3.Hwi");
    BIOS = xdc.useModule('ti.sysbios.BIOS');
    Startup = xdc.useModule('xdc.runtime.Startup');

    if (Program.cpu.deviceName.match(/^CC26/) ||
        Program.cpu.deviceName.match(/^CC13/)) {
        Timer.supportsAltclk = false;
    }

    if (Timer.enableFunc == null) {
        if (Program.cpu.deviceName.match(/^CC32/)) {
            Timer.enableFunc = '&ti_sysbios_family_arm_lm4_Timer_enableCC3200';
        }
        else if (Program.cpu.deviceName.match(/^CC26/) ||
                 Program.cpu.deviceName.match(/^CC13/)) {
            Timer.enableFunc =
                '&ti_sysbios_family_arm_lm4_Timer_enableCC26xx';
        }
        else {
            Timer.enableFunc = '&ti_sysbios_family_arm_lm4_Timer_enableTiva';
        }
    }

    if (Timer.disableFunc == null) {
        if (Program.cpu.deviceName.match(/^CC32/)) {
            Timer.disableFunc =
                '&ti_sysbios_family_arm_lm4_Timer_disableCC3200';
        }
        else if (Program.cpu.deviceName.match(/^CC26/) ||
                 Program.cpu.deviceName.match(/^CC13/)) {
            Timer.disableFunc =
                '&ti_sysbios_family_arm_lm4_Timer_disableCC26xx';
        }
        else {
            Timer.disableFunc = '&ti_sysbios_family_arm_lm4_Timer_disableTiva';
        }
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{

    mod.availMask = (1 << timers.length) - 1;

    mod.device.length = timers.length;
    mod.handles.length = timers.length;

    if (params.anyMask > mod.availMask) {
        Timer.$logError("Incorrect anyMask (" + params.anyMask
            + "). Should be <= " + mod.availMask + ".", Timer, "anyMask");
    }

    /* init the module state fields for each timer */
    for (var i=0; i < mod.device.length; i++) {
        mod.device[i].baseAddr = timers[i].$orig.baseAddr;
        mod.device[i].intNum = timers[i].$orig.intNum;
        mod.handles[i] = null;
    }

    /*
     * plug Timer.startup into BIOS.startupFxns array
     */
    BIOS.addUserStartupFunction(Timer.startup);

    /* enable Timer access early */
    Startup.firstFxns.$add(Timer.enableTimers);
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

    if (id >= Timer.numTimerDevices) {
        if (id != Timer.ANY) {
            Timer.$logFatal("Invalid Timer ID " + id + "!", this);
        }
    }

    if (id == Timer.ANY) {
        for (var i = 0; i < Timer.numTimerDevices; i++) {
            if ((Timer.anyMask & (1 << i)) && (modObj.availMask & (1 << i))) {
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

    /*
     * If a timer has not been assigned because either the requested timer was
     * unavailable or 'any timer' was requested and none were available...
     */
    if (obj.id == undefined) {
        if (id == Timer.ANY) {
            Timer.$logFatal("No available timers.", this);
        }
        else {
            Timer.$logFatal("Timer " + id + " is already in use or reserved (check availMask).", this);
        }
    }

    obj.runMode = params.runMode;
    obj.startMode = params.startMode;
    obj.altclk = params.altclk;

    if (Program.cpu.deviceName.match(/^CC26/) ||
        Program.cpu.deviceName.match(/^CC13/)) {
        if (obj.altclk == 1) {
            Timer.$logError("Timer does not support altclk.", obj);
            return;
        }
    }

    /* if using altClk the freq is always 16MHz */
    if (obj.altclk == 1)
    {
        obj.extFreq.lo = 16000000;
        obj.extFreq.hi = 0;
    }
    /* else use specified extFreq */
    else
    {
        obj.extFreq.lo = params.extFreq.lo;
        obj.extFreq.hi = params.extFreq.hi;
    }

    if (params.periodType == Timer.PeriodType_MICROSECS) {
        if (setPeriodMicroSecs(obj, params.period) == false) {
            Timer.$logError("Timer does not support configured period.",
                                 this, params.period)
        }
    }
    else {
        obj.period = params.period;
    }
    obj.periodType = Timer.PeriodType_COUNTS;

    obj.intNum = modObj.device[obj.id].intNum;
    obj.arg = params.arg;
    obj.tickFxn = tickFxn;
    obj.prevThreshold = 0xFFFFFFFF;
    obj.rollovers = 0;
    obj.savedCurrCount = 0;


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

        obj.hwi = Hwi.create(obj.intNum, Timer.isrStub, hwiParams);
    }
    else {
        obj.hwi = null;
    }

    modObj.handles[obj.id] = this;
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
        freq = BIOS.cpuFreq.lo / 1000000;       /* freq in MHz */
    }

    obj.period = Math.floor(freq * period);

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
 *  Initialize the 'Basic' Timer instance view.
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var halTimer = xdc.useModule('ti.sysbios.hal.Timer');
    var Timer = xdc.useModule('ti.sysbios.family.arm.lm4.Timer');
    
    if (viewCheckForNullObject(Timer, obj)) {
        view.halTimerHandle = "Uninitialized Timer object";
        return;
    }

    view.halTimerHandle =  halTimer.viewGetHandle(obj.$addr);
    view.label       = Program.getShortName(obj.$label);
    view.id          = obj.id;

    view.device      = "GPTM["+view.id+"]";
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

    var Timer = xdc.useModule('ti.sysbios.family.arm.lm4.Timer');
    
    if (viewCheckForNullObject(Timer, obj)) {
        view.id = "Uninitialized Timer object";
        return;
    }

    view.id          = obj.id;
    view.device      = "GPTM["+view.id+"]";
    view.intNum = obj.intNum;
    view.runMode     = getEnumString(obj.runMode);

    if (Program.build.target.name == "M3") {
        var gptmAddr = 0x40010000 + 0x1000 * view.id;
    }
    else {
        var gptmAddr = 0x40030000 + 0x1000 * view.id;
    }
    view.devAddr = "0x" + gptmAddr.toString(16);

    var GPTM = Program.fetchArray(
        {   type: 'xdc.rov.support.ScalarStructs.S_UInt32',
            isScalar: true
        },
        Number(view.devAddr), 20, false);

    view.period = GPTM[10];
    view.currCount = GPTM[18];
    view.remainingCount = view.currCount;

    view.prevThreshold = obj.prevThreshold;
    view.altclk = obj.altclk;

    if (GPTM[3] & 1) {
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
