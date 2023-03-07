/*
 * Copyright (c) 2012, Texas Instruments Incorporated
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
 * */

/*
 * ======== UIASync.xdc ========
 */
import xdc.runtime.Diags;
import ti.uia.events.IUIAEvent;

/*!
 * UIA Synchronization Events
 *
 *  The UIASync module defines events that enable
 *  correlation of events from multiple cores as well as
 *  correlation of software events with hardware trace
 *
 *  The following configuration script demonstrates how to configure an
 *  application to log sync point events, using the default configuration
 *  settings provided by the LogSync module.  By default, the LogSync module
 *  creates a dedicated 256 byte LoggerCircBuf logger to use to capture
 *  sync point events.   By default, the Rta module logs sync point events
 *  when it receives a start or stop command, or before sending up an
 *  event packet if the LogSync.isSyncPointRequired API returns 'true'.
 *  @see ti.uia.runtime.LogSync
 *
 * @a(Examples)
 * Example 1: This is part of the XDC configuration file for the application:
 *
 *  @p(code)
 *  // the LogSync module internally does xdc.useModule('ti.uia.events.UIASync')
 *  var LogSync = xdc.useModule('ti.uia.runtime.LogSync');
 *  var Rta = xdc.useModule('ti.uia.services.Rta');
 *  @p
 *
 *  @p(html)
 *  <hr />
 *  @p
 */
module UIASync inherits IUIAEvent {

    /*!======= syncPoint =======
     * syncPoint event
     *
     * This event logs both a local CPU timestamp (the event timestamp) and a
     * global timestamp, which enables time correlation of the
     * local CPU timestamps that each event can be tagged with
     * against the global timebase, thus enabling multi-core
     * event correlation.  It also logs clock information that allows the host
     * to determine how to convert CPU timestamp tick counts into time values
     * and CPU cycle counts.
     * @param(serialNumber) a 32b serial number, used for correlation with HW
     *    trace sync points
     * @param(CpuTimestampLSW) the 32 LSBs of the global timestamp
     * @param(CpuTimestampMSW) the 32 MSBs of the global timestamp
     * @param(GlobalTimestampLSW) the 32 LSBs of the global timestamp
     * @param(GlobalTimestampMSW) the 32 MSBs of the global timestamp
     * @param(CpuCyclesPerCpuTimerTick) the number of CPU cycles per CPU timer
     *    increment.  0 if no conversion possible.
     * @param(CPUFreqLSW) the 32 LSBs of the CPU frequency
     * @param(CPUFreqMSW) the 32 MSBs of the CPU frequency
     */
    config xdc.runtime.Log.Event syncPoint = {
            mask: Diags.ANALYSIS,
            msg: "Sync Point:  SerialNumber=0x%x, CpuTStamp [LSW=0x%x, MSW=0x%x], GlobalTStamp [LSW=0x%x, MSW=0x%x], CpuCyclesPerCpuTimerTick=%d, CpuFreq [LSW=0x%x, MSW=0x%x]"};


    /*!======= syncPointAfterHalt =======
     * syncPoint event logged after the CPU has resumed running after being
     *   reset, suspended or halted by the debugger
     *
     * This event logs both a local CPU timestamp (the event timestamp) and a
     * global timestamp, which enables time correlation of the
     * local CPU timestamps that each event can be tagged with
     * against the global timebase, thus enabling multi-core
     * event correlation.  It also logs clock information that allows the host
     * to determine how to convert CPU timestamp tick counts into time values
     *   and CPU cycle counts.
     * @param(serialNumber) a 32b serial number, used for correlation with HW
     *   trace sync points
     * @param(CpuTimestampLSW) the 32 LSBs of the global timestamp
     * @param(CpuTimestampMSW) the 32 MSBs of the global timestamp
     * @param(GlobalTimestampLSW) the 32 LSBs of the global timestamp
     * @param(GlobalTimestampMSW) the 32 MSBs of the global timestamp
     * @param(CpuCyclesPerCpuTimerTick) the number of CPU cycles per CPU timer
     *   increment.  0 if no conversion possible.
     * @param(CPUFreqLSW) the 32 LSBs of the CPU frequency
     * @param(CPUFreqMSW) the 32 MSBs of the CPU frequency
     */
    config xdc.runtime.Log.Event syncPointAfterHalt = {
            mask: Diags.ANALYSIS,
            msg: "Sync Point After Halt:  SerialNumber=0x%x, CpuTStamp [LSW=0x%x, MSW=0x%x], GlobalTStamp [LSW=0x%x, MSW=0x%x], CpuCyclesPerCpuTimerTick=%d, CpuFreq [LSW=0x%x, MSW=0x%x]"};

    /*!======= globalTimerFreq =======
     * syncPoint event containing global timer frequency information
     *
     * This event logs clock information that allows the host to determine how
     * to convert global imestamp tick counts into time values and CPU cycle
     * counts.
     * @param(serialNumber) a 32b serial number, used for correlation with HW
     *    trace sync points
     * @param(CpuCyclesPerGlobalTimerTick) the number of CPU cycles per global
     *    timer increment.  0 if no conversion possible.
     * @param(GlobalTimerFreqLSW) the 32 LSBs of the Global Timer frequency
     * @param(GlobalTimerFreqMSW) the 32 MSBs of the Global Timer frequency
     */
    config xdc.runtime.Log.Event globalTimerFreq = {
            mask: Diags.ANALYSIS,
            msg: "Sync Point Global Timer Freq:  SerialNumber=0x%x, CpuCyclesPerGlobalTimerTick=%d, GlobalTimerFreq [LSW=0x%x, MSW=0x%x]"};

    /*!======= syncPointUserData =======
     * An event that provides supplementary info to a sync point event
     *
     * This event can be used by user-provided code to log supplementary
     * event data whenever a sync point event is logged.
     * @param(serialNumber) a 32b serial number, used for correlation with
     *    HW trace sync points
     * @param(fmt) a format specifier string for up to 6 additional parameters
     */
    config xdc.runtime.Log.Event syncPointUserProvidedData = {
            mask: Diags.ANALYSIS,
            msg: "Sync Point User Data: SerialNumber=0x%x, %$S"};


}
