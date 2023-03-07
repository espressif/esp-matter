/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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

var Timer = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Timer = this;
    Timer.common$.fxntab = false;

    if ((Timer.TimerProxy === undefined) || Timer.TimerProxy == null) {
        var Settings = xdc.module("ti.sysbios.family.Settings");
        var TimerDelegate = Settings.getDefaultTimerDelegate();
        if (TimerDelegate == null) {
            TimerDelegate = "ti.sysbios.hal.TimerNull";
        }
        Timer.TimerProxy = xdc.useModule(TimerDelegate, true);
    }

    Timer.supportsDynamic = Timer.TimerProxy.supportsDynamic;

    /*
     * Push down common$ settings to the delegates
     */
    for (var dl in Timer.common$) {
        if (dl.match(/^diags_/) || dl.match(/^logger/)) {
            /*
             * Extra check below to check if TimerProxy delegate is in ROM.
             * If delegate is in ROM, do NOT push down common$ settings
             */
            if (Timer.TimerProxy.delegate$.$$scope != -1) {
                Timer.TimerProxy.delegate$.common$[dl] = Timer.common$[dl];
            }
        }
    }

    /*
     * Call create for all instances of this module that were created
     * in the config script. This will ensure that by the time the
     * Clock module's module$static$init calls its timer create, the
     * config script's timer creates have already been called.
     */
    for (var realInst in this.$instances) {
        var inst = this.$instances[realInst];
        inst.$object.pi =
            Timer.TimerProxy.create(inst.$args.id, inst.$args.tickFxn, inst);
    }
}

/*
 *  ======== instance$static$init ========
 */
function instance$static$init(obj, id, tickFxn, params)
{
    if (obj.pi == undefined) {
        obj.pi = Timer.TimerProxy.create(id, tickFxn, params);
    }
}

/*!
 *  ======== viewGetHandle ========
 */
function viewGetHandle(pi)
{
    var Program = xdc.useModule('xdc.rov.Program');

    try {
        var halTimerRawView = Program.scanRawView('ti.sysbios.hal.Timer');
        for (var i in halTimerRawView.instStates) {
            if (Number(halTimerRawView.instStates[i].pi) == Number(pi)) {
                return (halTimerRawView.instStates[i].$addr);
            }
        }

    }
    catch(e) {
        return (null);
    }

    return (null);
}



