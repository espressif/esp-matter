/*
 * Copyright (c) 2012-2015, Texas Instruments Incorporated
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
 *  ======== Power.xdc ========
 *
 *
 */

package ti.sysbios.family.arm.cc26xx;

import ti.sysbios.knl.Clock;

/*!
 *  @_nodoc
 *  ======== Power ========
 */

metaonly module Power
{
    /*! @_nodoc Idle function prototype */
    typedef Void (*FuncPtr)();

    // configs

    /*!
     *  ======== idle ========
     *  Idle the CPU during idle time?
     *
     *  When this configuration parameter is set to true, the Power module will
     *  insert a power policy function into the list of {@link
     *  ti.sysbios.knl.Idle Idle} loop functions.
     *  When the policy function executes, it will take action to save power
     *  until the next CPU interrupt occurs.  The default power policy will
     *  simply invoke a wait for interrupt (WFI) instruction.
     *
     *  @see #policyFunc
     *  @see #doWFI
     *  @see #standbyPolicy
     */
    metaonly config Bool idle = false;

    /*!
     *  @_nodoc
     *  ======== resumeSTANDBY ========
     *  Latency to resume from STANDBY (in microseconds).
     *  NB: this is a temporary value pending further characterization.
     */
    metaonly config UInt resumeSTANDBY = 750;

    /*!
     *  @_nodoc
     *  ======== resumeTicksSTANDBY ========
     *  Latency to resume from STANDBY (in units of Clock ticks).
     */
    config UInt resumeTicksSTANDBY;

    /*!
     *  @_nodoc
     *  ======== totalSTANDBY ========
     *  Total latency to enter and resume from STANDBY (in microseconds).
     *  NB: this is a temporary value pending further characterization.
     */
    metaonly config UInt totalSTANDBY = 1000;

    /*!
     *  @_nodoc
     *  ======== totalTicksSTANDBY ========
     *  Total latency to enter and resume from STANDBY (in units of Clock
     *  ticks).
     */
    config UInt totalTicksSTANDBY;

    /*!
     *  @_nodoc
     *  ======== wakeDelaySTANDBY ========
     *  Device initial wakeup latency from STANDBY (in units of microseconds).
     */
    config UInt wakeDelaySTANDBY = 130;

    /*!
     *  @_nodoc
     *  ======== initialWaitRCOSC_LF ========
     *  Time (in units of usec) to wait to see if RCOSC_LF is stable.
     */
    config UInt initialWaitRCOSC_LF = 1000;

    /*!
     *  @_nodoc
     *  ======== retryWaitRCOSC_LF ========
     *  Time (in units of usec) to wait when retrying to see if RCOSC_LF is
     *   stable.
     */
    config UInt retryWaitRCOSC_LF = 1000;

    /*!
     *  @_nodoc
     *  ======== initialWaitXOSC_HF ========
     *  Time (in units of usec) to wait to see if XOSC_HF is stable.
     */
    config UInt initialWaitXOSC_HF = 50;

    /*!
     *  @_nodoc
     *  ======== retryWaitXOSC_HF ========
     *  Time (in units of usec) to wait when retrying to see if XOSC_HF is
     *   stable.
     */
    config UInt retryWaitXOSC_HF = 50;

    /*!
     *  @_nodoc
     *  ======== initialWaitXOSC_LF ========
     *  Time (in units of usec) to wait to see if XOSC_LF is stable.
     */
    config UInt initialWaitXOSC_LF = 10000;

    /*!
     *  @_nodoc
     *  ======== retryWaitXOSC_LF ========
     *  Time (in units of usec) to wait when retrying to see if XOSC_LF is
     *   stable.
     */
    config UInt retryWaitXOSC_LF = 5000;

    /*!
     *  @_nodoc
     *  ======== calibrateRCOSC ========
     *  Enable RCOSC calibration?   Default is true.
     */
    config Bool calibrateRCOSC = true;

    /*!
     *  @_nodoc
     *  ======== calibrateRCOSC_LF ========
     *  Enable RCOSC_LF calibration?  Default is true;
     */
    config Bool calibrateRCOSC_LF = true;

    /*!
     *  @_nodoc
     *  ======== calibrateRCOSC_HF ========
     *  Enable RCOSC_HF calibration?  Default is true;
     */
    config Bool calibrateRCOSC_HF = true;

    /*!
     *  @_nodoc
     *   Wakeup Clock object's function.  Default is Power_defaultClockFunc.
     */
    metaonly config Clock.FuncPtr clockFunc = null;

    /*! Policy function.  Default is Power_doWFI. */
    metaonly config FuncPtr policyFunc = null;

    /*!
     * @_nodoc
     *  Notify trap function.  Default is Power.defaultNotifyTrapFunc.
     */
    config FuncPtr notifyTrapFunc = null;
}
