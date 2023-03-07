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
 *  ======== Load.xs ========
 */

var Task = null;
var Idle = null;
var Swi = null;
var Hwi = null;
var Load = null;
var BIOS = null;
var Timestamp = null;
var Queue = null;
var Diags = null;
var Core = null;

var powerName = "";

/*
 * ======== getCFiles ========
 * return the array of C language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getCFiles(targetName)
{
    return (["Load.c", "Load_CPU.c"]);
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    /* provide getCFiles() for Build.getCFiles() */
    this.$private.getCFiles = getCFiles;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Load = this;
    BIOS = xdc.useModule("ti.sysbios.BIOS");
    Idle = xdc.useModule("ti.sysbios.knl.Idle");
    Timestamp = xdc.useModule("xdc.runtime.Timestamp");
    Queue = xdc.useModule("ti.sysbios.knl.Queue");
    Hwi = xdc.useModule("ti.sysbios.hal.Hwi");
    Diags = xdc.useModule('xdc.runtime.Diags');

    var Settings = xdc.module('ti.sysbios.family.Settings');
    var Power = xdc.module('ti.sysbios.hal.Power');
    var PowerProxy = null;

    Load.powerEnabled = isPowerEnabled();  /* Save this for Load.xdt */

    if ((BIOS.smpEnabled == true) || (BIOS.libType == BIOS.LibType_Debug)) {
        Core = xdc.useModule('ti.sysbios.hal.Core');
    }

    if (BIOS.swiEnabled == true) {
        Swi = xdc.useModule("ti.sysbios.knl.Swi");
    }

    if (BIOS.taskEnabled == true) {
        Task = xdc.useModule("ti.sysbios.knl.Task");
    }

    /*
     *  TODO: Remove 'false' if we want to support Task, Swi, and Hwi
     *  load logging with power enabled.
     */
    if (Load.powerEnabled && Load.swiEnabled && false) {
        Load.$logWarning("Swi load monitoring is not supported when using " +
                powerName + " to idle the CPU. Setting swiEnabled to false",
                Load, "swiEnabled");
        Load.swiEnabled = false;
    }
    if (Load.powerEnabled && Load.hwiEnabled && false) {
        Load.$logWarning("Hwi load monitoring is not supported when using " +
                powerName + " to idle the CPU. Setting hwiEnabled to false",
                Load, "hwiEnabled");
        Load.hwiEnabled = false;
    }
    if (Load.powerEnabled && Load.taskEnabled && false) {
        Load.$logWarning("Task load monitoring is not supported when using " +
                powerName + " to idle the CPU. Setting taskEnabled to false",
                Load, "taskEnabled");
        Load.taskEnabled = false;
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    var numCores = 1;

    if (BIOS.smpEnabled == true) {
        numCores = Core.numCores;
    }

    /*
     *  Load_hwiBeginHook is in Load.xdt and will pull in appropriate
     *  code depending on hwiEnabled and powerEnabled.  This way, we
     *  have one Hwi hook function if both are enabled.
     */
    if (Load.hwiEnabled) {
        Hwi.addHookSet({
            createFxn: null,
            beginFxn: Load.hwiBeginHook,
            endFxn: Load.hwiEndHook
        });
    }
    else if (Load.powerEnabled) {
        Hwi.addHookSet({
            createFxn: null,
            beginFxn: Load.hwiBeginHook,
            endFxn: null
        });
    }

    if ((BIOS.taskEnabled == true) && (Load.taskEnabled == true)) {
        Task.addHookSet({
            registerFxn: '&ti_sysbios_utils_Load_taskRegHook__E',
            createFxn: Load.taskCreateHook,
            readyFxn:  null,
            switchFxn: Load.taskSwitchHook,
            exitFxn: null,
            deleteFxn: Load.taskDeleteHook
        });
        if (Load.common$.diags_USER4 == Diags.ALWAYS_ON ||
            Load.common$.diags_USER4 == Diags.RUNTIME_ON ) {
            Task.common$.namedInstance = true; /* auto turn on task names*/
        }

        /*
         *  Set the taskEnv array size to the number of statically configured
         *  tasks (Task.$instances.length) + the number of constructed tasks
         *  (Task.$objects.length).
         */
        mod.taskEnv.length = Task.$instances.length + Task.$objects.length;

        if (BIOS.smpEnabled) {
            /*
             *  If nothing is running on the other cores except the
             *  Idle task, there will be no context switch to update
             *  the Idle task's running time.  This can cause the CPU
             *  load to be artificially high.  To fix this, call
             *  Load_updateCurrentThreadTime() in the idle loop.
             */
            for (var i = 1; i < numCores; i++) {
                Idle.addCoreFunc(Load.updateCurrentThreadTime, i);
            }
        }
    }
    else {
        mod.taskEnv.length = 0;
    }

    /*
     *  Only add the Load idle function when Power idling is disabled.
     */
    if (Load.powerEnabled) {
        /*
         *  PowerProxy.idle == true
         *  Add Load hwi hook function.
         */
    }
    else {
        Idle.addFunc(Load.idleFxn);
    }

    mod.taskEnvLen = mod.taskEnv.length;
    mod.taskNum = 0;

    for (var i = 0; i < mod.taskEnv.length; i++) {
        mod.taskEnv[i].totalTimeElapsed = 0;
        mod.taskEnv[i].totalTime = 0;
        mod.taskEnv[i].nextTotalTime = 0;
        mod.taskEnv[i].timeOfLastUpdate = 0;
        mod.taskEnv[i].threadHandle = null;
        Queue.elemClearMeta(mod.taskEnv[i].qElem);
    }

    if ((BIOS.swiEnabled == true) && (Load.swiEnabled == true)) {
        Swi.addHookSet({
            registerFxn: null,
            createFxn: null,
            readyFxn:  null,
            beginFxn: Load.swiBeginHook,
            endFxn: Load.swiEndHook,
            deleteFxn: null
        });
    }

    /* Construct the taskList */
    Queue.construct(mod.taskList);

    mod.taskStartTime.length = numCores;
    mod.runningTask.length = numCores;

    for (var i = 0; i < numCores; i++) {
        mod.taskStartTime[i] = 0;
        mod.runningTask[i] = null;
    }

    mod.taskHId = 0;
    mod.timeElapsed = 0;
    mod.firstSwitchDone = false;
    mod.swiStartTime = 0;
    mod.swiCnt = 0;
    mod.swiEnv.totalTimeElapsed = 0;
    mod.swiEnv.totalTime = 0;
    mod.swiEnv.nextTotalTime = 0;
    mod.swiEnv.timeOfLastUpdate = 0;
    mod.swiEnv.threadHandle = null;
    Queue.elemClearMeta(mod.swiEnv.qElem);
    mod.hwiStartTime = 0;
    mod.hwiCnt = 0;
    mod.hwiEnv.totalTimeElapsed = 0;
    mod.hwiEnv.totalTime = 0;
    mod.hwiEnv.nextTotalTime = 0;
    mod.hwiEnv.timeOfLastUpdate = 0;
    mod.hwiEnv.threadHandle = null;
    mod.timeSlotCnt = 0;
    mod.minLoop = 0xFFFFFFFF;
    mod.minIdle = Load.minIdle;
    mod.t0 = 0;
    mod.idleCnt = 0;
    mod.cpuLoad = 0;

    mod.powerEnabled = Load.powerEnabled;
    mod.idleStartTime = 0;
    mod.busyStartTime = 0;
    mod.busyTime = 0;

    Queue.elemClearMeta(mod.hwiEnv.qElem);
}

/*
 *  ======== module$validate ========
 *  some redundant tests are here to catch changes since
 *  module$static$init()
 */
function module$validate()
{
    if ((Load.swiEnabled == true) && (BIOS.swiEnabled == false)) {
        Load.$logWarning("Load has swi monitoring enabled, but BIOS has swi"
            + " disabled.", Load, "swiEnabled");
    }

    if ((Load.taskEnabled == true) && (BIOS.taskEnabled == false)) {
        Load.$logWarning("Load has task monitoring enabled, but BIOS has task"
            + " disabled.", Load, "taskEnabled");
    }

    if ((Load.taskEnabled == true) && (BIOS.taskEnabled == true)) {
        if (Task.enableIdleTask == false) {
            Load.$logError("Load has task monitoring enabled, but the Idle "
                + "task is disabled. The Idle task must be enabled to perform "
                + "task monitoring. " + Load, "taskEnabled");
        }
    }

    if ((Load.taskEnabled == false) && (Load.autoAddTasks == true)) {
        Load.autoAddTasks = false;
    }

    if ((Load.updateInIdle == true) && (Load.windowInMs == 0)) {
        Load.$logWarning("windowInMs must be set to a non-zero value.", Load, "windowInMs");
    }
}

/*
 *  ======== viewInitModule ========
 *  Initialize the Load 'Module' view.
 */
function viewInitModule(view, obj)
{
    var Task = xdc.useModule('ti.sysbios.knl.Task');

    var LoadCfg = Program.getModuleConfig('ti.sysbios.utils.Load');

    /* compute cpu load in the same manner as Load_getCPULoad() C API: */
    var cpuLoadValue = obj.cpuLoad;
    if ((cpuLoadValue > 100) || (cpuLoadValue < 0)) {
        view.$status["cpuLoad"] =
            "Error: CPU load computation resulted in out of range number: "
            + cpuLoadValue;
    }
    /* call toFixed() to specify only 1 digit after the decimal */
    view.cpuLoad = cpuLoadValue.toFixed(1);

    /*
     *  Only compute Swi stats if user set Load module hwiEnabled flag. Also
     *  check for divide by zero:
     */
    if (LoadCfg.swiEnabled && obj.swiEnv.totalTimeElapsed > 0) {
        /* first compute the load to get a floating point value */
        var swiLoadValue = obj.swiEnv.totalTime / obj.swiEnv.totalTimeElapsed * 100;

        if ((swiLoadValue > 100) || (swiLoadValue < 0)) {
            view.$status["swiLoad"] =
                "Error: Swi load computation resulted in out of range number: "
                + swiLoadValue;
        }

        /* call toFixed() to specify only 1 digit after the decimal */
        view.swiLoad = swiLoadValue.toFixed(1);
    }
    else if (!(LoadCfg.swiEnabled)) {
        view.swiLoad = "disabled";
    }
    else if (obj.swiEnv.totalTimeElapsed == 0) {
        /* protect against divide by 0 */
        view.swiLoad = "undefined";
    }
    else {
        view.$status["swiLoad"] = "Error: Swi time elaspsed is negative: "
                + obj.swiEnv.totalTimeElapsed;
    }

    /*
     *  Only compute Hwi stats if user set Load module hwiEnabled flag. Also
     *  check for divide by zero:
     */
    if (LoadCfg.hwiEnabled && obj.hwiEnv.totalTimeElapsed > 0) {
        /* first compute the load to get a floating point value */
        var hwiLoadValue = obj.hwiEnv.totalTime / obj.hwiEnv.totalTimeElapsed * 100;

        if ((hwiLoadValue > 100) || (hwiLoadValue < 0)) {
            view.$status["hwiLoad"] =
                "Error: Swi load computation resulted in out of range number: "
                + hwiLoadValue;
        }

        /* call toFixed() to specify only 1 digit after the decimal */
        view.hwiLoad = hwiLoadValue.toFixed(1);
    }
    else if (!(LoadCfg.hwiEnabled)) {
        view.hwiLoad = "disabled";
    }
    else if (obj.hwiEnv.totalTimeElapsed == 0) {
        /* protect against divide by 0 */
        view.hwiLoad = "undefined";
    }
    else {
        view.$status["hwiLoad"] = "Error: Hwi time elaspsed is negative: "
                + obj.hwiEnv.totalTimeElapsed;
    }

    /*
     *  compute an accuracy metric.  This only applies if power
     *  management is not enabled.
     */
    var minIdle = obj.minLoop > obj.minIdle ? obj.minLoop : obj.minIdle;
    if (minIdle > 0) {
        var err = 1 - ((minIdle - 1) / minIdle);
        view.idleError = (err * 100).toFixed(1) + "%";

        /* if total error is more than 15% warn user */
        err = err * (1 - obj.cpuLoad / 100);
        if ((err >= 0.15) && !LoadCfg.powerEnabled) {
            view.$status["idleError"] =
                "Warning: estimated error in total CPU load may be off by as much as "
                + (err * 100).toFixed(1);
        }
    }
    else {
        view.idleError = "unknown";
    }
}

/*
 *  ======== isPowerEnabled ========
 */
function isPowerEnabled()
{
    var Settings = xdc.module('ti.sysbios.family.Settings');
    var Power = xdc.module('ti.sysbios.hal.Power');
    var PowerProxy = null;
    var powerEnabled = false;
    var powerProxyIdle = false;

    /*
     * See if the Power delegate was used and initialize
     * powerEnabled to the delegate's setting.
     */
    powerName = Settings.getDefaultPowerDelegate();
    if (powerName == null) {
        powerName = "ti.sysbios.hal.PowerNull";
    }

    PowerProxy = xdc.module(powerName);

    /*
     * If hal Power is in the config, but not the delegate,
     * hal.Power.module$use() may not have been called yet to set
     * hal.Power.idle, so save the value of the delegate's idle,
     * in case this is the situation.
     */
    powerProxyIdle = PowerProxy.idle;
    if (PowerProxy.$used) {
        /*
         * The user config'd the delegate or hal.Power.module$use() has
         * been called.
         */
        powerEnabled = PowerProxy.idle;
    }

    if (Power.$used) {
        /*
         *  Power module$use may not have been called yet to push the
         *  delegate's idle value to hal Power.idle.
         */
        if (Power.idle != undefined) {
	    /*
             * If hal Power is used, and hal.Power.idle is not undefined,
	     * the user has set it, so that setting rules.
	     */
            powerEnabled = Power.idle;
            powerName = "ti.sysbios.hal.Power";
        }
        else {
            /*
	     * hal.Power.idle has not been initialized yet,
	     * use the delagate's idle.
	     */
            powerEnabled = powerProxyIdle;
        }
    }

    return (powerEnabled);
}
