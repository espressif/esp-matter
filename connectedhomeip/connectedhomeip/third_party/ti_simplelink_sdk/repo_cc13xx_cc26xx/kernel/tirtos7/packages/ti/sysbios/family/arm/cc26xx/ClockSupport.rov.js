/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 * ======== ClockSupport.rov.js ========
 */

// Make this function global
xdc.global.ClockSupport_viewGetCurrentClockTick = viewGetCurrentClockTick;

/*
 *  ======== viewGetCurrentClockTick ========
 *  Compute and return the current Clock tick value
 */
function viewGetCurrentClockTick(clockModState)
{
    var tickPeriod = clockModState.tickPeriod;
    var period64 = Math.floor(0x100000000 * tickPeriod / 1000000);
    var ticks = 0;

    /*
     * ROV2 brings realtime reads of target memory and registers while a
     * program is running.  Attempting such reads of the RTC shadow registers
     * can freeze program execution in some cases. This problem may be
     * addressed in the future by automatically returning a read error when
     * realtime reads of these registers are attempted.  In the meantime, as
     * a workaround a global Boolean (DISABLE_READ_RTC) will be
     * set when ROV2 is in use.  In the code below, if this flag is defined and
     * set to 'true', an error will be thrown instead of attempting to read
     * the registers; this error will be interpreted as the Timer view code
     * does not support dynamic tick computation, so the most recently updated
     * tick count in Clock module state will be used instead, with a stale data
     * indication. If DISABLE_READ_RTC is defined but set to 'false', or
     * undefined, the RTC registers will be read and the tick count computed
     * and returned.
     */
    if (typeof DISABLE_READ_RTC !== 'undefined') {
        if (DISABLE_READ_RTC == true) {
            throw 'RTC reads disabled';
        }
    }

    try {
        var SEC = Program.fetchFromAddr(Number("0x40092008"), "UInt32", 1);
        var SUBSEC = Program.fetchFromAddr(Number("0x4009200C"), "UInt32", 1);

        /*
         * only 51 bits resolution in JavaScript; break into SEC & SUBSEC
         * pieces
         */
        ticks = SUBSEC / period64;                    /* ticks from SUBSEC */
        ticks = ticks + (SEC * 1000000 / tickPeriod); /* plus ticks from SEC */
        ticks = Math.floor(ticks);                    /* clip total */
    }
    catch (e) {
        ticks = 0x80000000;
    }

    return ticks;
}
