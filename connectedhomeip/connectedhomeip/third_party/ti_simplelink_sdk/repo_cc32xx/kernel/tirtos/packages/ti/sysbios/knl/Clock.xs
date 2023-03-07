/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *  ======== Clock.xs ========
 *
 */

var Queue = null;
var Clock = null;
var Timer = null;
var TimerProxy = null;
var Swi = null;
var BIOS = null;
var Settings = null;
var Build = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    Clock = this;

    Settings = xdc.module("ti.sysbios.family.Settings");
    Clock.tickPeriod = Settings.getDefaultClockTickPeriod();

}

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.useModule('ti.sysbios.BIOS');
    Build = xdc.module('ti.sysbios.Build');
    /* last parameter is true to disable expected cycle warnings  */
    Queue = xdc.useModule('ti.sysbios.knl.Queue');
    xdc.useModule('ti.sysbios.hal.Hwi');
    if (!(BIOS.libType == BIOS.LibType_Custom && BIOS.logsEnabled == false)) {
        xdc.useModule('xdc.runtime.Log');
    }
    if (!(BIOS.libType == BIOS.LibType_Custom
        && BIOS.assertsEnabled == false)) {
        xdc.useModule('xdc.runtime.Assert');
    }

    if (BIOS.swiEnabled) {
        Swi = xdc.useModule('ti.sysbios.knl.Swi');
        if (Clock.swiPriority === undefined) {
            Clock.swiPriority = Swi.numPriorities - 1;
        }
        else if (Clock.swiPriority >= Swi.numPriorities) {
             Clock.$logError (
                 "Clock.swiPriority (" + Clock.swiPriority +
                 ") must be less than Swi.numPriorities (" +
                 Swi.numPriorities + ")", Clock, "swiPriority");
        }
    }
    else {
        Clock.swiPriority = 0;
    }

    if (!BIOS.clockEnabled) {
        if (Clock.$written("tickSource") &&
           (Clock.tickSource != Clock.TickSource_NULL)) {
            Clock.$logWarning("The Clock.tickSource setting is internally "
            + "forced to Clock.TickSource_NULL if the Clock Manager has "
            + "been disabled (BIOS.clockEnabled == false).",
            Clock, "tickSource");
        }
        /*
         * Set Clock.tickSource to NULL to avoid instantiating timer
         */
        Clock.tickSource = Clock.TickSource_NULL;
    }

    /* if app hasn't declared Clock_stop() behavior, choose a default */
    if (!Clock.$written("stopCheckNext")) {

        /* if cc26xx or cc13xx, check/trigger clock when next is stopped */
        if (Program.cpu.deviceName.match(/^CC26/) ||
            Program.cpu.deviceName.match(/^CC13/)) {
            Clock.stopCheckNext = true;
        }
        else {
            Clock.stopCheckNext = false;
        }
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    /* for some reason BIOS from module$use doesn't exist anymore */
    BIOS = xdc.module('ti.sysbios.BIOS');

    Queue.construct(mod.clockQ);

    mod.ticks = 0;
    mod.swiCount = 0;
    mod.numTickSkip = 1;
    mod.nextScheduledTick = 1;
    mod.maxSkippable = 0;
    mod.inWorkFunc = false;
    mod.ticking = false;
    mod.startDuringWorkFunc = false;

    if (BIOS.clockEnabled == true) {
	Clock.doTickFunc = '&ti_sysbios_knl_Clock_doTick__I';
    }
    else {
	Clock.doTickFunc = null;
    }

    /*
     * Minimize footprint by only creating Clock Swi if Clock and Swi
     * are enabled.
     */
    if ((BIOS.clockEnabled == true) && (BIOS.swiEnabled == true)) {
        /* Create Clock Swi */
        var swiParams = new Swi.Params();
        swiParams.arg0 = 0;
        swiParams.arg1 = 0;
        swiParams.priority = Clock.swiPriority;
        swiParams.trigger = 0;
        if (Clock.tickMode == Clock.TickMode_DYNAMIC) {
            mod.swi = Swi.create(Clock.workFuncDynamic, swiParams);
        }
        else {
            mod.swi = Swi.create(Clock.workFunc, swiParams);
        }
    }
    else {
        mod.swi = null;
    }

    if (Clock.tickSource == Clock.TickSource_TIMER) {
        var BIOS = xdc.om['ti.sysbios.BIOS'];
        var timerParams = new Clock.TimerProxy.Params();
        if (Clock.tickMode == Clock.TickMode_DYNAMIC) {
            timerParams.runMode = Clock.TimerProxy.delegate$.RunMode_DYNAMIC;
        }
        else {
            timerParams.runMode = Clock.TimerProxy.delegate$.RunMode_CONTINUOUS;
        }
        timerParams.startMode = Clock.TimerProxy.delegate$.StartMode_AUTO;
        // Compute period in timer counts
        timerParams.period = Clock.tickPeriod;
        timerParams.periodType = Clock.TimerProxy.delegate$.PeriodType_MICROSECS;
        mod.timer = Clock.TimerProxy.create(Clock.timerId, Clock.doTick, timerParams);
    }
    else {
        mod.timer = null;
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, func, timeout, params)
{
    var modObj = this.$module.$object;

    if (BIOS.clockEnabled == false) {
        Clock.$logFatal("Can't create a Clock with BIOS.clockEnabled == " +
                        " false", this);
    }

    /* Integer-ize the timeout param before checking it */
    timeout = Math.floor(timeout);

    if ((timeout == 0) && (params.startFlag == true)) {
        Clock.$logError("The timeout parameter must not be zero.", this,
                        "timeout");
    }

    obj.timeout = timeout;
    obj.currTimeout = timeout;
    obj.period = params.period;
    obj.fxn = func;
    obj.arg = params.arg;
    obj.active = false;

    /*
     * Clock object is always placed on Clock work Q
     */
    modObj.clockQ.putMeta(obj.elem);

    /* if enabled, set active flag to true */
    if (params.startFlag && (timeout != 0)) {
        obj.active = true;
    }
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    /* check constraints for using TickMode_DYNAMIC */
    if ((Clock.tickMode == Clock.TickMode_DYNAMIC) &&
        (Clock.tickSource == Clock.TickSource_TIMER)) {

        /* verify that the bound Timer supports RunMode_DYNAMIC */
        if (Clock.TimerProxy.supportsDynamic == false) {
            Clock.$logError("The selected Clock.TimerProxy does not support TickMode_DYNAMIC", Clock, "tickMode");
        }
    }

    /* validate all "created" instances */
    for (var i = 0; i < Clock.$instances.length; i++) {
        instance_validate(Clock.$instances[i]);
    }

    /* validate all "constructed" instances */
    for (var i = 0; i < Clock.$objects.length; i++) {
        instance_validate(Clock.$objects[i]);
    }
}

/*
 *  ======== instance_validate ========
 *  common function to test instance configuration
 */
function instance_validate(instance)
{
    if (instance.$object.fxn == null) {
        Clock.$logError("function cannot be null", instance);
    }
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
 */
function viewInitBasic(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Model = xdc.useModule("xdc.rov.Model");
    var modCfg = Program.getModuleConfig('ti.sysbios.knl.Clock');
    var Clock = xdc.useModule('ti.sysbios.knl.Clock');

    if (viewCheckForNullObject(Clock, obj)) {
        view.label = "Uninitialized Clock object";
        return;
    }

    view.label = obj.$label;
    view.timeout = obj.timeout;
    view.period = obj.period;

    view.fxn = Program.lookupFuncName(Number(obj.fxn));

    view.arg = obj.arg;

    /* The inst is started if active is TRUE */
    if (obj.active == false) {
        view.started = false;
    }
    else {
        view.started = true;

        var modRaw = Program.scanRawView("ti.sysbios.knl.Clock");

        /*
         * If operating in dynamic mode and skipping ticks, try to query the
         * timer to compute the current Clock tick count.
         */
        var compTicks = false;
        if (modRaw.modState.numTickSkip > 1) {
            var timer = xdc.module(modCfg.TimerProxy.$name);
            try {
                var ticks = timer.viewGetCurrentClockTick();
                compTicks = true;
            }
            catch (e) {
            }
        }

        if (compTicks) {
            var remain = obj.currTimeout - ticks;
        }
        else {
            var remain = obj.currTimeout - modRaw.modState.ticks;
        }

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

        /*
         * If Timer didn't compute/report tick count, but skipping ticks,
         * indicate 'stale data'.
         */
        if ((!compTicks) && (modRaw.modState.numTickSkip > 1)) {
            view.tRemaining = String(remain) + " (stale data)";
        }
        /* else, just show remaining ticks */
        else {
            view.tRemaining = String(remain);
        }
    }

    view.periodic = (obj.period > 0);
}

/*
 *  ======== viewInitModule ========
 */
function viewInitModule(view, mod)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var modRaw = Program.scanRawView("ti.sysbios.knl.Clock");
    var modCfg = Program.getModuleConfig('ti.sysbios.knl.Clock');

    /*
     * If operating in dynamic mode and skipping ticks, query the timer
     * to compute the current Clock tick count.  If the timer doesn't support
     * computing Clock ticks (i.e., it doesn't implement
     * viewGetCurrentClockTick()), just report the most recent tick count, and
     * note 'stale data' (because  ROV's reported count is likely stale, since
     * it indicates the last serviced timer interrupt).
     */
    if (modRaw.modState.numTickSkip > 1) {
        var timer = xdc.module(modCfg.TimerProxy.$name);
        try {
            var ticks = timer.viewGetCurrentClockTick();
            view.ticks = String(ticks);
        }
        catch (e) {
            view.ticks = String(mod.ticks) + " (stale data)";
        }
    }
    /* else, just show the tick counter */
    else {
        view.ticks = String(mod.ticks);
    }

    view.tickSource = Program.$modules['ti.sysbios.knl.Clock'].tickSource;
    view.tickMode = Program.$modules['ti.sysbios.knl.Clock'].tickMode;

    try {
        var timerRawView = Program.scanRawView(modCfg.TimerProxy.$name);
    }
    catch (e) {
        print (e);
        return;
    }

    view.timerHandle = String(mod.timer);

    /* Check each instance */
    for (var i in timerRawView.instStates) {
        if (Number(mod.timer) == Number(timerRawView.instStates[i].$addr)) {
            if (modCfg.TimerProxy.$name.match(/ti.sysbios.timers.timer64.Timer/)) {
		/*
		 *  timer64 uses a logical timer id because each 64-bit timer
		 *  can be used as 2 32-bit timers. When displaying the timer
		 *  id it needs to be right shifted by 1 to match what is
		 *  specified in 'Clock.timerId = #'.
		 */
	        view.timerId = Number(timerRawView.instStates[i].id) >>> 1;
	    }
            else {
                view.timerId = Number(timerRawView.instStates[i].id);
            }
        }
    }

    view.swiPriority = Program.$modules['ti.sysbios.knl.Clock'].swiPriority;
    view.tickPeriod = Program.$modules['ti.sysbios.knl.Clock'].tickPeriod;
    view.nSkip = mod.numTickSkip;
}
