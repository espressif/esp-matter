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
package ti.uia.runtime;

import xdc.runtime.Types;

/*!
 *  ======== IUIATimestampProvider ========
 *  UIA timestamp provider interface
 *
 *  This interface is implemented by platform-specific modules that
 *  "provide" underlying timestamp services that allow 64b timestamp values
 *  to be read directly into event parameters.
 *
 *  Note that the implementation is typically not thread safe - it
 *  is up to the caller to ensure that the get64() API is protected from
 *  preemption by another thread or another CPU core in order to ensure
 *  that a coherent 64b result is returned.  For timers that are a shared
 *  resource, some form of system-level lock (e.g. Lampart's bakery algorithm,
 *  spinlock or Peterson lock) or IPC round-robin mechanism may be required in
 *  order to ensure that there are no race conditions.
 */
interface IUIATimestampProvider inherits xdc.runtime.ITimestampProvider {

    /*!
     * ======== maxTimerClockFreqLSW =========
     * The highest timer clock frequency.
     *
     * The default ticks per second rate of the timer is calculated by dividing
     * the timer's bus clock frequency by the cyclesPerTick config parameter.
     *
     * @a(returns) the 32 LSBs of the highest timer clock frequency
     *   (i.e. ticksPerSecond).
     */
    config Types.FreqHz maxTimerClockFreq;

    /*!
     * ======== maxBusClockFreqMSW =========
     * The highest bus clock frequency used to drive the timer.
     *
     * The default ticks per second rate of the timer is calculated by dividing
     * the timer's bus clock frequency by the cyclesPerTick config parameter.
     *
     * @a(returns) the 32 MSBs of the highest bus clock frequency used to drive
     *    the timer.
     */
    config Types.FreqHz maxBusClockFreq;

    /*!
     * ======== canFrequencyBeChanged =========
     * Indicates whether the timer frequency can be changed or not
     *
     * @a(returns) true if the timer's clock frequency can be changed
     */
    metaonly config Bool canFrequencyBeChanged = false;

    /*!
     * ======== cpuCyclesPerTick =========
     * The number of CPU cycles each tick of the timestamp corresponds to
     *
     * A value of 0 indicates that no conversion between the timer's tick count
     * and CPU cycles is possible.
     */
    metaonly config UInt32 cpuCyclesPerTick = 0;

    /*!
     * ======== canCpuCyclesPerTickBeChanged =========
     * Indicates whether the timer's cycles per tick divide down ratio can be
     * changed or not
     *
     * @a(returns) true if the timer's CPU cycles per tick can be changed
     */
    metaonly config Bool canCpuCyclesPerTickBeChanged = false;
}
