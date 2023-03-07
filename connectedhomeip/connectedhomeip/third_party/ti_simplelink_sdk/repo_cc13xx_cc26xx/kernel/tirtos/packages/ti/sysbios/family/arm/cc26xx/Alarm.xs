/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== Alarm.xs ========
 *
 */

var Alarm = null;
var Clock = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    Alarm = this;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Alarm = this;
    Clock = xdc.module('ti.sysbios.knl.Clock');
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var Clock = xdc.module('ti.sysbios.knl.Clock');

    if (!BIOS.clockEnabled) {
        Alarm.$logError("Clock must be enabled to use Alarm", Alarm);
        return;
    }

    if (Clock.tickSource != Clock.TickSource_TIMER) {
        Alarm.$logError("To use Alarm, Clock.tickSource must be Clock.TickSource_TIMER", Alarm);
    }

    if (!Clock.TimerProxy.delegate$.$name.match(/ti.sysbios.family.arm.cc26xx.Timer/)) {
        Alarm.$logError("Alarm can only be used with the RTC timer: ti.sysbios.family.arm.cc26xx.Timer", Alarm);
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, func, arg, params)
{
    obj.rtcCount = 0;

    var clockParams = new Clock.Params();
    clockParams.period = 0;
    clockParams.startFlag = false;
    clockParams.arg = arg;
    Clock.construct(obj.clockObj, func, 0, clockParams);
}

/*
 *  ======== viewInitBasic ========
 *  Initialize the 'Basic' Alarm instance view.
 */
function viewInitBasic(view, obj)
{
    view.label = Program.getShortName(obj.$label);

    view.timeout = obj.clockObj.timeout;
    view.rtcCount = obj.rtcCount;

    /* get RTC timer period from the single timer instance */
    var timerRaw = Program.scanRawView('ti.sysbios.family.arm.cc26xx.Timer');
    var period = timerRaw.instStates[0].period;
    view.rtcInterrupt = Math.floor(obj.rtcCount / period) * period;

    if (obj.clockObj.active == true) {
        view.active = true;
    }
    else {
        view.active = false;
    }
}
