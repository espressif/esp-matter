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
 *  ======== Seconds.xs ========
 *
 */

var Seconds = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Seconds = this;
    Seconds.common$.fxntab = false;

    if ((Seconds.SecondsProxy === undefined) || Seconds.SecondsProxy == null) {
        var Settings = xdc.module("ti.sysbios.family.Settings");
        var SecondsDelegate = Settings.getDefaultSecondsDelegate();
        if (SecondsDelegate == null) {
            SecondsDelegate = "ti.sysbios.hal.SecondsClock";
        }
        Seconds.SecondsProxy = xdc.useModule(SecondsDelegate, true);
    }

    /*
     * Push down common$ settings to the delegates
     */
    for (var dl in Seconds.common$) {
        if (dl.match(/^diags_/) || dl.match(/^logger/)) {
            /*
             * Extra check below to check if SecondsProxy delegate is in ROM.
             * If delegate is in ROM, do NOT push down common$ settings
             */
            if (Seconds.SecondsProxy.delegate$.$$scope != -1) {
                Seconds.SecondsProxy.delegate$.common$[dl] = Seconds.common$[dl];
            }
        }
    }
}
