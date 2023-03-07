/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== Hwi.xs ========
 *
 */

var Hwi = null;
var Idle = null;
var Startup = null;
var BIOS = null;
var Core = null;

/*
 * ======== getCFiles ========
 * return the array of C language files associated
 * with targetName (ie Program.build.target.$name)
 */
function getCFiles(targetName)
{
    return (["Hwi.c", "Hwi_stack.c", "Hwi_startup.c"]);
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
    Hwi = this;
    Hwi.common$.fxntab = false;

    Core = xdc.module('ti.sysbios.hal.Core');

    Startup = xdc.useModule('xdc.runtime.Startup');

    Idle = xdc.useModule('ti.sysbios.knl.Idle', true);

    /* if Hwi proxy not bound yet, do it */
  
    if ((Hwi.HwiProxy === undefined) || (Hwi.HwiProxy == null)) {
        /*
         * Need to do a useModule here to force the module$use in
         * ti.sysbios.family.Settings to run.
         */
        var Settings = xdc.useModule("ti.sysbios.family.Settings");
        var HwiDelegate = Settings.getDefaultHwiDelegate();
        Hwi.HwiProxy = xdc.useModule(HwiDelegate, true);
    }

    BIOS = xdc.module("ti.sysbios.BIOS");

    /* resolve proxy/delegate config settings */

    if (Hwi.dispatcherSwiSupport == undefined) {
        if (Hwi.HwiProxy.delegate$.dispatcherSwiSupport == undefined) {
            /* 
             * if neither hal.Hwi nor the delegate.Hwi have been 
             * set by the user, then let BIOS.swiEnabled rule.
             */
            Hwi.dispatcherSwiSupport = BIOS.swiEnabled;
            Hwi.HwiProxy.delegate$.dispatcherSwiSupport = BIOS.swiEnabled;
        }
        else {
            /* 
             * if hal.Hwi is uninited but delegate.Hwi has been set by the
             * user, then let the delegate.Hwi rule
             */
            Hwi.dispatcherSwiSupport = Hwi.HwiProxy.delegate$.dispatcherSwiSupport;
        }
    }
    else {
        /*
         * If the user has configured hal.Hwi, it should be the master.
         */
        Hwi.HwiProxy.delegate$.dispatcherSwiSupport = Hwi.dispatcherSwiSupport;
    }

    if (Hwi.dispatcherTaskSupport == undefined) {
        if (Hwi.HwiProxy.delegate$.dispatcherTaskSupport == undefined) {
            Hwi.dispatcherTaskSupport = BIOS.taskEnabled;
            Hwi.HwiProxy.delegate$.dispatcherTaskSupport = BIOS.taskEnabled;
        }
        else {
            Hwi.dispatcherTaskSupport = Hwi.HwiProxy.delegate$.dispatcherTaskSupport;
        }
    }
    else {
        Hwi.HwiProxy.delegate$.dispatcherTaskSupport = Hwi.dispatcherTaskSupport;
    }

    /*
     * The default value for AutoNesting is true!
     * If either hal.Hwi or delegate.Hwi is false then the user has actively
     * done so.  Therefore, set both to false.
     */
    if (Hwi.HwiProxy.delegate$.dispatcherAutoNestingSupport 
                        != Hwi.dispatcherAutoNestingSupport) {
        Hwi.HwiProxy.delegate$.dispatcherAutoNestingSupport = false;
        Hwi.dispatcherAutoNestingSupport = false;
    }

    /*
     * The default value for IrpTracking is true!
     * If either hal.Hwi or delegate.Hwi is false then the user has actively
     * done so.  Therefore, set both to false.
     */
    if (Hwi.HwiProxy.delegate$.dispatcherIrpTrackingSupport
                        != Hwi.dispatcherIrpTrackingSupport) {
        Hwi.HwiProxy.delegate$.dispatcherIrpTrackingSupport = false;
        Hwi.dispatcherIrpTrackingSupport = false;
    }

    /*
     * Use delegate's default maskSetting
     */
    Hwi.PARAMS.maskSetting = Hwi.HwiProxy.delegate$.PARAMS.maskSetting;
    
    /*
     * Push down common$ settings to the delegates
     */
    for (var dl in Hwi.common$) {
        if (dl.match(/^diags_/) || dl.match(/^logger/)) {
            /*
             * Extra check below to check if HwiProxy delegate is in ROM.
             * If delegate is in ROM, do NOT push down common$ settings
             */
            if (Hwi.HwiProxy.delegate$.$$scope != -1) {
                Hwi.HwiProxy.delegate$.common$[dl] = Hwi.common$[dl];
            }
        }
    }

    /*
     * Warn IAR users that Program.stack is ignored
     */
    if (Program.build.target.$name.match(/iar/)) {
        if (Program.$written("stack") == true && (Program.stack != 0)) {
            Program.$logWarning("For IAR targets, Program.stack is ignored. " +
                    "The C and Hwi stack sizes are determined by the size " +
                    "of the CSTACK section definition within the linker " +
                    "command file.", Program, "stack");
        }
    }
    else {
        var Settings = xdc.module("ti.sysbios.family.Settings");
        var TaskSupportDelegate = xdc.module(Settings.getDefaultTaskSupportDelegate());
        var align = TaskSupportDelegate.stackAlignment;
        var stackSizeBefore = Program.stack;
        var stackSizeAfter = Program.stack;
        if (align != 0) {
            stackSizeAfter &= (-align);
        }
        if (stackSizeBefore != stackSizeAfter) {
            Hwi.$logWarning("Program.stack (" + stackSizeBefore +
                ") was adjusted to " + stackSizeAfter +
                " to guarantee proper stack alignment.",
                Program, "stack");
            Program.stack = stackSizeAfter;
        }
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    if (Program.build.target.suffix != "86W") {

        if (Hwi.initStackFlag == true) {
            /* initialize the C stack; do it early to catch early oveflows */
            Startup.firstFxns.$add("&ti_sysbios_hal_Hwi_initStack");
        }
        else if (Hwi.checkStackFlag == true) {
            /* only initialize the top of the stack */
            Startup.firstFxns.$add("&ti_sysbios_hal_Hwi_initStackMin");
        }

        if (Hwi.checkStackFlag == true) {
            if (BIOS.smpEnabled) {
                for (var i = 0; i < Core.numCores; i++) {
                    Idle.addCoreFunc("&ti_sysbios_hal_Hwi_checkStack", i);
                }
            }
            else {
                Idle.funcList.$add("&ti_sysbios_hal_Hwi_checkStack");
            }
        }
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, intNum, fxn, params)
{
    obj.pi = Hwi.HwiProxy.create(intNum, fxn, params);
}

/*
 *  ======== addHookSet ========
 */
function addHookSet(hookSet)
{
    Hwi = this;
  
    /* if Hwi proxy not bound yet, do it */
  
    if ((Hwi.HwiProxy === undefined) || (Hwi.HwiProxy == null)) {
        var Settings = xdc.module("ti.sysbios.family.Settings");
        var HwiDelegate = Settings.getDefaultHwiDelegate();
        Hwi.HwiProxy = xdc.useModule(HwiDelegate, true);
    }

    Hwi.HwiProxy.delegate$.addHookSet(hookSet);
    Hwi.numHooks++;
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    var Settings = xdc.module("ti.sysbios.family.Settings");
    var HwiDelegate = xdc.module(Settings.getDefaultHwiDelegate());
    if ((BIOS.swiEnabled == true) 
         && (HwiDelegate.dispatcherSwiSupport == false)) {
        if (BIOS.clockEnabled == true) {
            HwiDelegate.$logError("Hwi.dispatchSwiSupport must be enabled when "
               + "BIOS.clockEnabled is true and "
               + "BIOS.swiEnabled is true.",
               HwiDelegate, "dispatcherSwiSupport");
        }
        else {
            HwiDelegate.$logWarning("When BIOS.swiEnabled is true and "
               + "Hwi.dispatcherSwiSupport is false, Swis MUST NOT be "
               + "posted from Hwi functions!",
               HwiDelegate, "dispatcherSwiSupport");
        }
    }
}

/*!
 *  ======== viewGetStackInfo ========
 */
function viewGetStackInfo()
{
    var Program = xdc.useModule('xdc.rov.Program');
    var HwiProxy = Program.$modules['ti.sysbios.hal.Hwi'].HwiProxy.$name;
    var HwiDelegate = xdc.useModule(HwiProxy, true);

    return (HwiDelegate.viewGetStackInfo());
}

/*!
 *  ======== viewGetHandle ========
 */
function viewGetHandle(pi)
{
    var Program = xdc.useModule('xdc.rov.Program');

    try {
        var halHwiRawView = Program.scanRawView('ti.sysbios.hal.Hwi');
        for (var i in halHwiRawView.instStates) {
            if (Number(halHwiRawView.instStates[i].pi) == Number(pi)) {
                return (halHwiRawView.instStates[i].$addr);
            }
        }

    }
    catch (e) {
        return (null);
    }

    return (null);
}

/*!
 *  ======== viewGetLabel ========
 */
function viewGetLabel(pi)
{
    var Program = xdc.useModule('xdc.rov.Program');

    try {
        var halHwiRawView = Program.scanRawView('ti.sysbios.hal.Hwi');
        for (var i in halHwiRawView.instStates) {
            if (Number(halHwiRawView.instStates[i].pi) == Number(pi)) {
                return (halHwiRawView.instStates[i].$label);
            }
        }

    }
    catch (e) {
        return (null);
    }

    return (null);
}
