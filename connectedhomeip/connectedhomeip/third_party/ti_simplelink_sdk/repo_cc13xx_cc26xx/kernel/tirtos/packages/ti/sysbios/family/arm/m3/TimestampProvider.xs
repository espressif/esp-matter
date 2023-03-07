/*
 * Copyright (c) 2012-2013, Texas Instruments Incorporated
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
 *  ======== TimestampProvider.xs ========
 *
 *
 */

var Timer = null;
var Startup = null;
var TimestampProvider = null;
var Clock = null;
var BIOS = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    TimestampProvider = this;
    /* set fxntab default */
    TimestampProvider.common$.fxntab = false;
    Timer = xdc.useModule('ti.sysbios.family.arm.m3.Timer');
    Clock = xdc.module('ti.sysbios.knl.Clock');
    Startup = xdc.useModule('xdc.runtime.Startup');
    BIOS = xdc.useModule('ti.sysbios.BIOS');

    var Diags = xdc.module('xdc.runtime.Diags');

    for (var dl in TimestampProvider.common$) {
        if (dl.match(/^diags_/) && dl != 'diags_ASSERT') {
            TimestampProvider.common$[dl] = Diags.ALWAYS_OFF;
        }
    }
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    var configTimer = false;

    /* Configure the timer only if nobody else is  */
    if (!TimestampProvider.$written("configTimer")) {
	if (Timer.timerInUse == false) {
	    if (BIOS.clockEnabled == false) {
		configTimer = true;
	    }
	    else if (Clock.tickSource != Clock.TickSource_TIMER) {
		configTimer = true;
	    }
	    else if (Clock.TimerProxy.delegate$.$name != "ti.sysbios.family.arm.m3.Timer") {
		configTimer = true;
	    }
	}
    }
    else {
        configTimer = TimestampProvider.configTimer;
    }

    if (configTimer == true) {
        var  timerParams = new Timer.Params();

        timerParams.period = Timer.MAX_PERIOD;
        timerParams.periodType = Timer.PeriodType_COUNTS;
        timerParams.runMode = Timer.RunMode_CONTINUOUS;
        timerParams.startMode = Timer.StartMode_USER;

        mod.timer = Timer.create(Timer.ANY, 
                     TimestampProvider.rolloverFunc,
                     timerParams);
    }
    else {
        mod.timer = null;
    }
}

/*
 *  ======== getFreqMeta ========
 */
function getFreqMeta()
{
    var BIOS = xdc.module("ti.sysbios.BIOS");

    return (BIOS.getCpuFreqMeta());
}
