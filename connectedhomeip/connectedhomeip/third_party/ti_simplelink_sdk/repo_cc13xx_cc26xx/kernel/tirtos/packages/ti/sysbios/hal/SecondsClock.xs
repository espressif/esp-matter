/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== SecondsClock.xs ========
 */

var BIOS;
var Clock;
var SecondsClock;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    SecondsClock = this;

    Clock = xdc.module('ti.sysbios.knl.Clock');
}

/*
 *  ======== module$static$init ========
 */
function module$static$init(mod, params)
{
    mod.secondsHi = 0;
    mod.seconds = 0;
    mod.c1 = 0;
    mod.c2 = 0;
    mod.count1 = 0;
    mod.count2 = 0;
    mod.c1Inc = 0;
    mod.ticks = 0;

    /*
     *  Clock tickPeriod is in usec units.  Use it to compute a 1 second
     *  clock period.
     */
    var oneSecond = 1000000 / Clock.tickPeriod;

    var clockParams = new Clock.Params();
    clockParams.period = oneSecond;

    Clock = Clock.construct(mod.clock, this.increment, oneSecond, clockParams);
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    BIOS = xdc.module("ti.sysbios.BIOS");
    SecondsClock = this;
}
