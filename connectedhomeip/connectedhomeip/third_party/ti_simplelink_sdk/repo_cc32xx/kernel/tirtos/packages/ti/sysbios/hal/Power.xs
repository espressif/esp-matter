/*
 * Copyright (c) 2013-2015, Texas Instruments Incorporated
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
 *  ======== Power.xs ========
 */

var Power = null;

/*
 *  ======== module$use ========
 */
function module$use()
{
    Power = this;
    Power.common$.fxntab = false;

    if ((Power.PowerProxy === undefined) || Power.PowerProxy == null) {
        var Settings = xdc.module('ti.sysbios.family.Settings');
        var PowerDelegate = Settings.getDefaultPowerDelegate();

       if (PowerDelegate == null) {
            PowerDelegate = "ti.sysbios.hal.PowerNull";
        }
        Power.PowerProxy = xdc.useModule(PowerDelegate, true);
    }

    if (Power.idle == undefined) {
        if (Power.PowerProxy.idle == undefined) {
            /* Both are undefined, set them to true. */
            Power.idle = true;
            Power.PowerProxy.idle = true;
        }
        else {
            /* User has set delegate, so use its settings. */
            Power.idle = Power.PowerProxy.idle;
        }
    }
    else {
        /* ti.sysbios.hal.Power settings are the master. */
        Power.PowerProxy.idle = Power.idle;
    }

    /*
     * Push down common$ settings to the delegates
     */
    for (var dl in Power.common$) {
        if (dl.match(/^diags_/) || dl.match(/^logger/)) {
            /*
             * Extra check below to check if PowerProxy delegate is in ROM.
             * If delegate is in ROM, do NOT push down common$ settings
             */
            if (Power.PowerProxy &&
                    (Power.PowerProxy.delegate$.$$scope != -1)) {
                Power.PowerProxy.delegate$.common$[dl] = Power.common$[dl];
            }
        }
    }
}
