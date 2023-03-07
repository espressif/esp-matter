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
 *  ======== TimestampProvider.xs ========
 *
 */

var Timer = null;
var Startup = null;
var TimestampProvider = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    TimestampProvider = this;
    /* set fxntab default */
    TimestampProvider.common$.fxntab = false;
    Timer = xdc.useModule('ti.sysbios.family.arm.lm3.Timer');
    Startup = xdc.useModule('xdc.runtime.Startup');

    var Diags = xdc.useModule('xdc.runtime.Diags');

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
    if (TimestampProvider.useClockTimer == false) {
        var  timerParams = new Timer.Params();

        timerParams.period = Timer.MAX_PERIOD;
        timerParams.periodType = Timer.PeriodType_COUNTS;
        timerParams.runMode = Timer.RunMode_CONTINUOUS;
        timerParams.startMode = Timer.StartMode_USER;

        mod.timer = Timer.create(TimestampProvider.timerId, 
                    TimestampProvider.rolloverFunc,
                    timerParams);

        Startup.lastFxns.$add(TimestampProvider.startTimer);
    }
    else {
        /* timer handle filled in at runtime */
        mod.timer = null;

        /*  Install firstFxn ONLY if useClockTimer == true */
        Startup.firstFxns.$add(TimestampProvider.initTimerHandle);
    }

    mod.hi = 0;
}

/*
 *  ======== getFreqMeta ========
 */
function getFreqMeta()
{
    var BIOS = xdc.module("ti.sysbios.BIOS");

    return (BIOS.getCpuFreqMeta());
}

/*
 *  ======== module$validate ========
 *  Verify that if we're using the Clock timer for Timestamp, that the
 *  Clock is enabled and that Clock's tick source is a timer.
 */
function module$validate()
{
    var Clock = xdc.module('ti.sysbios.knl.Clock');
    var BIOS = xdc.module("ti.sysbios.BIOS"); 
    
    if (TimestampProvider.useClockTimer) {
        /* Verify Clock is enabled. */
        if (!BIOS.clockEnabled) {
            TimestampProvider.$logError("Clock must be enabled to use Clock " +
                                        "timer for timestamp.", 
                                        TimestampProvider, "useClockTimer");
        }
        /* Verify Clock tick source is a timer. */
        if (Clock.tickSource != Clock.TickSource_TIMER) {
            TimestampProvider.$logError("Clock must use a timer as its tick " +
                                        " source to use Clock timer for " + 
                                        "timestamp.", 
                                        TimestampProvider, "useClockTimer");

        }
    }
}

