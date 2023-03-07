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
 *  ======== package.xs ========
 */

 /*
 *  ======== Package.getLibs ========
 *  This function is called when a program's configuration files are
 *  being generated and it returns the name of a library appropriate
 *  for the program's configuration.
 */
function getLibs(prog)
{
    var Build = xdc.module("ti.sysbios.Build");

    /* use shared getLibs() */
    return (Build.getLibs(this));
}

/*
 *  ======== close ========
 */
function close()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    var Build = xdc.module("ti.sysbios.Build");
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var Clock = xdc.module('ti.sysbios.knl.Clock');

    /* Set the Clock TimerProxy if not yet set */
    if ((Clock.TimerProxy === undefined) || (Clock.TimerProxy == null)) {
        if (Clock.tickSource == Clock.TickSource_TIMER) {
            var Settings = xdc.module("ti.sysbios.family.Settings");
            var TimerProxy = Settings.getDefaultClockTimerDelegate();
            if (TimerProxy == null) {
                TimerProxy = "ti.sysbios.hal.TimerNull";
            }
        }
        else {
            /*
             *  For TickSource_USER and TickSource_NULL, set the Clock
             *  TimerProxy to hal TimerNull.
             */
            TimerProxy = "ti.sysbios.hal.TimerNull";
        }
        Clock.TimerProxy = xdc.module(TimerProxy);
        Clock.timerSupportsDynamic = Clock.TimerProxy.supportsDynamic;
    }

    if (Clock.$used) {
        xdc.useModule(Clock.TimerProxy.delegate$.$name);
    }

    Clock.timerSupportsDynamic = Clock.TimerProxy.supportsDynamic;

    /* if user has not defined the TickMode ... */
    if (Clock.tickMode == undefined) {
        if (Clock.TimerProxy.defaultDynamic == true) {
            Clock.tickMode = Clock.TickMode_DYNAMIC;
        }
        else {
            Clock.tickMode = Clock.TickMode_PERIODIC;
        }
    }

    if (BIOS.clockEnabled && (Clock.tickMode == Clock.TickMode_DYNAMIC) &&
            (Clock.stopCheckNext == true)) {
        var clockParams = new Clock.Params();
        clockParams.period = 0;
        clockParams.startFlag = false;
        Clock.triggerClock = Clock.create(Clock.triggerFunc, 1, clockParams);
    }
    else {
        Clock.stopCheckNext = false;
        Clock.triggerClock = null;
    }

    /* add -D to compile line with definition for Clock.stopCheckNext */
    Build.ccArgs.$add("-Dti_sysbios_knl_Clock_stopCheckNext__D=" +
            (Clock.stopCheckNext ? "TRUE" : "FALSE"));
}
