/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
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
 *  ======== LoggerTypes.xdc ========
 *  TODO: This module is only used in LogSnapshot.  Can probably
 *  just move the LogMemoryRangeFxn typedef to LogSnapshot and then
 *  remove this module.
 */

package ti.uia.runtime;
import xdc.runtime.Types;
import xdc.runtime.Log;
import ti.uia.runtime.IUIATraceSyncProvider;

/*!
 *  ======== LoggerTypes ========
 *  Function pointer type definitions for various types of loggers
 *
 *  This module defines function prototypes for use in defining
 *  callback functions that support device-specific features (e.g.
 *  synchronization with CPU Trace) and support customization and
 *  extensibility of core UIA features (e.g. logging a range of memory values).
 */
module LoggerTypes {

    /*!
     *  ======== LogMemoryRangeFxn ========
     *  Log an event along with values from a range of memory addresses
     *
     *  Note that this function can support logging of null terminated strings,
     *  arrays of characters and memory mapped registgers as well as blocks of
     *  memory.  The LogSnapshot module uses this callback function.
     *  Modules that implement the ILoggerSnapshot interface provide implementations
     *  of this callback function.
     *
     *  @param(ptr)         module instance object handle
     *  @param(evt)         event to be logged
     *  @param(snapshotId)  0 = no other snapshot groups, Use value from
     *                      LogSnapshot.getSnapshotId() for all snapshots to be
     *                      grouped.
     *  @param(fileName)    __FILE__ result
     *  @param(lineNum)     __LINE__ result
     *  @param(fmt)         a `printf` style format string
     *  @param(startAdrs)   value for first format conversion character
     *  @param(lengthInMAUs) value for second format conversion character
     *
     *  @see Log#Event
     *  @see ILoggerSnapshot#writeMemoryRange
     */
     typedef Void (*LogMemoryRangeFxn)(Ptr, xdc.runtime.Log.Event,
        UInt32, UInt32, IArg, IArg, IArg, IArg, IArg);

    /*!
     * ====== InjectIntoTraceFxn ======
     * Callback function that injects syncPoint info into GEM Trace
     *
     * This callback function injects correlation info into the trace stream
     * to enable correlation between software events and hardware trace.
     *
     * @param(serialNum)   the serial number that is to be injected
     * @param(ctxType)     the context type ID to be injected into the trace
     *
     * @see IUIATraceSyncProvider
     * @see IUIATraceSyncClient
     */
    typedef Void (*InjectIntoTraceFxn)(UInt32, IUIATraceSyncProvider.ContextType);
}
