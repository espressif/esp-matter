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

var BIOS = null;
var Timer = null;
var Startup = null;
var TimestampProvider = null;
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
    TimestampProvider = this;
    /* set fxntab default */
    TimestampProvider.common$.fxntab = false;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.useModule('ti.sysbios.BIOS');
    Timer = xdc.useModule('ti.sysbios.family.arm.cc26xx.Timer');
    Startup = xdc.useModule('xdc.runtime.Startup');

    var Diags = xdc.useModule('xdc.runtime.Diags');
    for (var dl in TimestampProvider.common$) {
        if (dl.match(/^diags_/) && dl != 'diags_ASSERT') {
            TimestampProvider.common$[dl] = Diags.ALWAYS_OFF;
        }
    }

    /* if user has not specified useClockTimer ... */
    if (TimestampProvider.useClockTimer == undefined) {
	/* assume false */
	TimestampProvider.useClockTimer = false;
        /* if Clock is enabled, default to share its timer */
        if (BIOS.clockEnabled) {
            Clock = xdc.module('ti.sysbios.knl.Clock');
            if (Clock.tickSource != Clock.TickSource_NULL){
                TimestampProvider.useClockTimer = true;
            }
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
                    null, timerParams);

        Startup.lastFxns.$add(TimestampProvider.startTimer);
    }
    else {
        mod.timer = null;
    }
}

/*
 *  ======== module$view$init ========
 */
function module$view$init(view, mod)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Timer = Program.scanModule('ti.sysbios.family.arm.cc26xx.Timer');

    view.timer = Timer.$scanHandle(mod.timer).$view;
    view.usesClockTimer = Program.$modules['ti.sysbios.family.arm.cc26xx.TimestampProvider'].useClockTimer;
}

/*
 *  ======== module$validate ========
 */
function module$validate()
{
    var BIOS = xdc.module('ti.sysbios.BIOS');
    var Clock = xdc.module('ti.sysbios.knl.Clock');

    if ((!BIOS.clockEnabled || (Clock.tickSource == Clock.TickSource_NULL))
        && (this.useClockTimer == true)) {
        TimestampProvider.$logError("Clock is not enabled, cannot share its Timer", TimestampProvider, "useClockTimer");
    }
}

/*
 *  ======== getFreqMeta ========
 */
function getFreqMeta()
{
    return ({lo : 65536, hi : 0});  /* report only upper 48-bits of RTC */
}
