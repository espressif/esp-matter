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
 *  ======== LogCtxChg.xdc ========
 */
package ti.uia.runtime;
import xdc.runtime.Types;
import ti.uia.runtime.LoggerTypes;
import xdc.runtime.ILogger;
import ti.uia.runtime.CtxFilter;
import xdc.runtime.ILogger;
import xdc.runtime.Diags;
import xdc.runtime.Text;

/*!
 *  ======== LogCtxChg ========
 *  Context Change Event logging manager for logging context change events
 *
 *  Allows context change events to be logged to a logger that is different from the one used by
 *  xdc.runtime.Log.
 *
 *  Users can provide a special function to inject context change information into trace or log
 *  sync points whenever a context change is logged.  See ctxFilterEnabled config parameter for more info.
 */
@CustomHeader
module LogCtxChg {

    /*!
     *  ======== isTimestampEnabled ========
     *  used to enable or disable logging the 64b local CPU timestamp
     *  at the start of each event
     */
    config Bool isTimestampEnabled = true;

    /*!
     *  ======== loggerDefined ========
     *  set to true in the configuration script when a logger that implements ILoggerSnapshot is attached
     */
    config Bool loggerDefined = false;

    /*!
     *  ======== ctxFilterEnabled ========
     *  set to true in the configuration script in order to enable context filtering.
     *
     *  User can implement a custom function that is called instead of ti_uia_runtime_CtxFilter_isCtxEnabled()
     *  to control context-aware filtering.  This function can also e.g. inject context information into
     *  the GEM trace or log sync point information if desired.
     *  @see ti.uia.runtime.CtxFilter
     */
     config Bool ctxFilterEnabled = false;

    /*!
     *  ======== loggerObj ========
     *  handle of the logger that is to be used to log snapshot events
     */
    config Ptr loggerObj = null;

    config xdc.runtime.Types.LoggerFxn2 loggerFxn2 = null;
    config xdc.runtime.Types.LoggerFxn8 loggerFxn8 = null;

    /*!
     *  ======== putCtxChg1 ========
     *  Unconditionally put the specified `Types` event along with file name, line number, fmt string and 1 arg.
     *
     *  This method unconditionally puts the specified context change`{@link Types#Event}`
     *  `evt` into the log along with the fmt string, line and file name that the event was logged from,
     *   and one parameter
     *
     *  @param(evt)   the `Types` event to put into the log
     *  @param(mask)  the diags mask of the event
     *  @param(syncPtSeqNum) the sync point sequence number to log with the event for correlation with trace
     *  @param(fmt)   a constant string that provides format specifiers for up to 6 additional parameters
     *  @param(arg1)  the context change argument to log
     */
    @Macro UInt32 putCtxChg1(Types.Event evt, Types.ModuleId mid, UInt32 syncPtSeqNum, IArg fmt, IArg arg1);

    /*!
     *  ======== putCtxChg2 ========
     *  Unconditionally put the specified `Types` event along with file name, line number, fmt string and 2 args.
     *
     *  This method unconditionally puts the specified context change`{@link Types#Event}`
     *  `evt` into the log along with the fmt string, line and file name that the event was logged from,
     *   and two parameters
     *
     *  @param(evt)   the `Types` event to put into the log
     *  @param(mask)  the diags mask of the event
     *  @param(a1)  the first context change argument to log
     *  @param(a2)  the second context change argument to log
     */
    @Macro UInt32 putCtxChg2(Types.Event evt, Types.ModuleId mid, IArg a1, IArg a2);

    /*!
     *  ======== putCtxChg8 ========
     *  Unconditionally put the specified `Types` event along with file name, line number, fmt string and 4 args.
     *
     *  This method unconditionally puts the specified context change`{@link Types#Event}`
     *  `evt` into the log along with the fmt string, line and file name that the event was logged from,
     *   and 4 parameters
     *
     *  @param(evt)   the `Types` event to put into the log
     *  @param(mask)  the diags mask of the event
     *  @param(syncPtSeqNum) the sync point sequence number to log with the event for correlation with trace
     *  @param(fmt)   a constant string that provides format specifiers for up to 6 additional parameters
     *  @param(a1)  the first context change argument to log
     *  @param(a2)  the second context change argument to log
     *  @param(a3)  the third context change argument to log
     *  @param(a4)  the fourth context change argument to log
     *  @param(a5)  the fifth context change argument to log
     *  @param(a6)  the sixth context change argument to log
     *  @param(a7)  the seventh context change argument to log
     *  @param(a8)  the eighth context change argument to log
     */
    @Macro UInt32 putCtxChg8(Types.Event evt, Types.ModuleId mid, IArg a1, IArg a2, IArg a3, IArg a4,IArg a5, IArg a6, IArg a7, IArg a8);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_app ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAAppCtx#ctxChg
     */
    @Macro Void app(String fmt, IArg newAppId);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_channel ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAChanCtx#ctxChg
     */
    @Macro Void channel(String fmt, IArg newChanId);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_thread ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAThreadCtx#ctxChg
     */
    @Macro Void thread(String fmt, IArg newThreadId);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_threadAndFunc ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAThreadCtx#ctxChgWithFunc
     */
    @Macro Void threadAndFunc(String fmt, IArg newThreadId, IArg oldFunc, IArg newFunc);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_frame ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAFrameCtx#ctxChg
     */
    @Macro Void frame(String fmt, IArg newFrameId);


    /*!
     *  ======== ti_uia_runtime_LogCtxChg_hwiStart ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAHWICtx#start
     */
    @Macro Void hwiStart(String fmt, IArg hwiId);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_hwiStop ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAHWICtx#stop
     */
     @Macro Void hwiStop(String fmt, IArg hwiId);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_swiStart ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIASWICtx#start
     */
    @Macro Void swiStart(String fmt, IArg swiId);
    /*!
     *  ======== ti_uia_runtime_LogCtxChg_swiStop ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIASWICtx#stop
     */
    @Macro Void swiStop(String fmt, IArg swiId);

    /*!
     *  ======== ti_uia_runtime_LogCtxChg_user ========
     *  Log a context change event that can be used to enable context-aware event
     *  filtering, context-aware profiling, etc.
     *
     *  @see ti.uia.events.UIAUserCtx#ctxChg
     */
    @Macro Void user(String fmt, IArg newCtxId);


    internal:

    /*
     *  ======== idToInfo ========
     *  Map event ID strings into a string of the form <eventName>::<eventMsg>
     */
    metaonly config String idToInfo[string] = [];
}
